/*

Copyright (C) 2017, 2018 Ivan Kniazkov

This file is part of interpreter of programming language
codenamed "Goat" ("Goat interpreter").

Goat interpreter is free software: you can redistribute it and/or
modify it under the terms of the GNU General Public License as published
by the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Goat interpreter is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with Goat interpreter.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "Object.h"
#include "ObjectArray.h"
#include "ObjectBoolean.h"
#include "ObjectBuiltIn.h"
#include "ObjectByteArray.h"
#include "ObjectChar.h"
#include "ObjectException.h"
#include "ObjectFile.h"
#include "ObjectFunction.h"
#include "ObjectInteger.h"
#include "ObjectNull.h"
#include "ObjectReal.h"
#include "ObjectString.h"
#include "ObjectStringBuilder.h"
#include "ObjectThread.h"
#include "ObjectUndefined.h"
#include "WideStringBuilder.h"
#include "Assert.h"
#include "Resource.h"

namespace goat {

	StringIndex Object::indexes;
	ObjectList ObjectList::global;
	ObjectList ObjectList::forMarking;

	Object::Object() {
		status = MARKED;
		list = nullptr;
		ObjectList::global.pushBack(this);
		proto.pushBack(SuperObject::getInstance());
	}

	Object::Object(bool builtIn) {
		status = PERMANENT | LOCKED;
	}

	Object::Object(Object *_proto) : Object() {
		status = MARKED;
		list = nullptr;
		ObjectList::global.pushBack(this);
		proto.pushBack(_proto);
	}

	Object::~Object() {
		if ((status & PERMANENT) == 0) {
			ObjectList::global.remove(this);
		}
	}

	void Object::mark() {
		if ((status & (MARKED | PERMANENT)) == 0) {
			status |= MARKED;
			ObjectList::forMarking.pushBack(this);
		}
	}

	void Object::mark_2() {
		objects.forEach([](int32 &index, Container &container) {
			container.mark();
		});
		chain.forEach([](Pair &pair) {
			pair.key.mark();
			pair.value.mark();
		});
		proto.forEach([](Object *obj) {
			if (obj)
				obj->mark();
		});
		trace();
		ObjectList::global.pushBack(this);
	}

	void Object::trace() {

	}

	Container * Object::find_(int32 index) {
		Container *found = objects.find(index);
		if (!found) {
			List<Pair>::Item *pair = chain.first;
			while (pair) {
				ObjectString *objStr = pair->data.key.toObjectString();
				if (objStr) {
					if (getKey(index) == objStr->value) {
						return &pair->data.value;
					}
				}
				pair = pair->next;
			}
			for (unsigned int i = 0; !found && i < proto.len(); i++) {
				found = proto[i]->find_(index);
			}
		}
		return found;
	}

	Container * Object::find(int32 index) {
		Container *found = find_(index);
		return found ? found : ObjectUndefined::getContainer();
	}

	Container * Object::find_(WideString key) {
		Container *found = key.isAscii() ? objects.find(searchIndex(key.toString())) : nullptr;
		if (!found) {
			List<Pair>::Item *pair = chain.first;
			while (pair) {
				ObjectString *objStr = pair->data.key.toObjectString();
				if (objStr) {
					if (key == objStr->value) {
						return &pair->data.value;
					}
				}
				pair = pair->next;
			}
			for (unsigned int i = 0; !found && i < proto.len(); i++) {
				found = proto[i]->find_(key);
			}
		}
		return found;
	}

	Container * Object::find(WideString key) {
		Container *found = find_(key);
		return found ? found : ObjectUndefined::getContainer();
	}

	Container * Object::find_(Container *key) {
		ObjectString *objStr = key->toObjectString();
		if (objStr) {
			return find_(objStr->value);
		}
		Container *found = nullptr;
		List<Pair>::Item *pair = chain.first;
		while (pair) {
			if (key->equals(&pair->data.key)) {
				return &pair->data.value;
			}
			pair = pair->next;
		}
		for (unsigned int i = 0; !found && i < proto.len(); i++) {
			found = proto[i]->find_(key);
		}
		return found;
	}

	Container * Object::find(Container *key) {
		Container *found = find_(key);
		return found ? found : ObjectUndefined::getContainer();
	}

	void Object::insert(int32 index, Object *value) {
		objects.insert(index, value);
		List<Pair>::Item *pair = chain.first;
		while (pair) {
			auto next = pair->next;
			ObjectString *objStr = pair->data.key.toObject()->toObjectString();
			if (objStr && objStr->value == getKey(index)) {
				chain.remove(pair);
			}
			pair = next;
		}
	}

	void Object::insert(Object *key, Object *value) {
		ObjectString *objStr = key->toObjectString();
		if (objStr) {
			objects.remove(searchIndex(objStr->value.toString()));
		}
		List<Pair>::Item *pair = chain.first;
		while (pair) {
			if (key->equals(&pair->data.key)) {
				pair->data.value = value;
				return;
			}
			pair = pair->next;
		}
		chain.pushBack(Pair(key, value));
	}

	bool Object::replace(int32 index, Object *repl) {
		if (objects.replace(index, repl)) {
			return true;
		}
		for (unsigned int i = 0; i < proto.len(); i++) {
			if (proto[i]->replace(index, repl)) {
				return true;
			}
		}
		return false;
	}

	void Object::findAll(String key, PlainVector<Object *> *vector) {
		Container *container = objects.find(createIndex(key));
		if (container) {
			vector->pushBack(container->toObject());
		}
		for (unsigned int i = 0; i < proto.len(); i++) {
			proto[i]->findAll(key, vector);
		}
	}

	void Object::findUnique(String key, PlainVector<Object *> *result) {
		PlainVector<Object *> tmp;
		findAll(key, &tmp);
		int32 l = (int32)tmp.len();
		if (l > 0) {
			result->pushBack(tmp[l - 1]);
			for (int32 i = l - 2; i >= 0; i--) {
				Object *obj = tmp[i];
				if (!result->indexOf(obj)) {
					result->pushBack(obj);
				}
			}
		}
	}

	void Object::flat(Object *fobj) {
		proto.forEachReverse([&](Object *pobj) {
			pobj->flat(fobj);
		});

		objects.forEach([&](int32 &index, Container &container) {
			fobj->insert(index, container.toObject());
		});

		chain.forEach([&](Pair pair) {
			fobj->insert(pair.key.toObject(), pair.value.toObject());
		});
	}

	void Object::enumerate(Vector<Pair> *vector) {
		Object *fobj = new Object();
		flat(fobj);

		fobj->objects.forEach([&](int32 &index, Container &container) {
			vector->pushBack(Pair(new ObjectString(getKey(index).toWideString()), container.toObject()));
		});

		fobj->chain.forEach([&](Pair pair) {
			vector->pushBack(pair);
		});
	}

	void Object::clone(Object *_clone) {
		objects.clone(_clone->objects);
		_clone->proto.clean();
		proto.clone(_clone->proto);
	}

	bool Object::equals(Container *ctr) {
		if (ctr->isPrimitive()) {
			return false;
		}
		return this == ctr->data.obj;
	}

	bool Object::instanceOf(Object *_obj) {
		if (_obj == this) {
			return true;
		}

		for (unsigned int i = 0, l = proto.len(); i < l; i++) {
			if (proto[i]->instanceOf(_obj)) {
				return true;
			}
		}

		return false;
	}

	WideString Object::toWideString() {
		WideStringBuilder b;
		b << (wchar)'{';
		int i = 0;
		objects.forEach([&](int32 &index, Container &container) {
			if (i) {
				b << L',';
			}
			i++;
			b << getKey(index) << L':' << container.toWideStringNotation();
		});
		chain.forEach([&](Pair &pair) {
			if (i) {
				b << L',';
			}
			i++;
			b << pair.key.toWideStringNotation() << L':' << pair.value.toWideStringNotation();
		});
		b << (wchar)'}';
		return b.toWideString();
	}

	WideString Object::toWideStringNotation() {
		return toWideString();
	}

	RawString Object::toRawString() {
		return toWideString().toRawString();
	}

	ObjectString * Object::toObjectString() {
		return nullptr;
	}

	ObjectFunction * Object::toObjectFunction() {
		return nullptr;
	}

	ObjectThread * Object::toObjectThread() {
		return nullptr;
	}

	ObjectArray * Object::toObjectArray() {
		return nullptr;
	}

	ObjectBuiltIn * Object::toObjectBuiltIn() {
		return nullptr;
	}

	ObjectInteger * Object::toObjectInteger() {
		return nullptr;
	}

	ObjectReal * Object::toObjectReal() {
		return nullptr;
	}

	ObjectException * Object::toObjectException() {
		return nullptr;
	}

	ObjectBoolean * Object::toObjectBoolean() {
		return nullptr;
	}

	ObjectChar * Object::toObjectChar() {
		return nullptr;
	}

	ObjectVoid * Object::toObjectVoid() {
		return nullptr;
	}

	ObjectNull * Object::toObjectNull() {
		return nullptr;
	}

	ObjectUndefined * Object::toObjectUndefined() {
		return nullptr;
	}

	ObjectFile * Object::toObjectFile() {
		return nullptr;
	}

	ObjectStringBuilder * Object::toObjectStringBuilder() {
		return nullptr;
	}

	ObjectByteArray * Object::toObjectByteArray() {
		return nullptr;
	}

	void ObjectList::unmark() {
		forEach([](Object *obj) {
			obj->status &= ~Object::MARKED;
		});
	}

	bool ObjectList::mark_2() {
		if(count > 0) {
			forEach([](Object *obj) {
				obj->mark_2();
			});
			return false;
		}
		return true;
	}

	void ObjectList::sweep() {
		forEach([](Object *obj) {
			if ((obj->status & Object::PERMANENT) == 0)
			{
				if ((obj->status & Object::MARKED) == 0) {
					delete obj;
				}
				else {
					obj->status &= ~Object::MARKED;
				}
			}
		});
	}

	////////

	union ObjectPlaceholder {
		char a[sizeof(ObjectArray)];
		char b[sizeof(ObjectBoolean)];
		char ba[sizeof(ObjectByteArray)];
		char c[sizeof(ObjectChar)];
		char e[sizeof(ObjectException)];
		char fi[sizeof(ObjectFile)];
		char fn[sizeof(ObjectFunction)];
		char i[sizeof(ObjectInteger)];
		char r[sizeof(ObjectReal)];
		char s[sizeof(ObjectString)];
		char sb[sizeof(ObjectStringBuilder)];
		char t[sizeof(ObjectThread)];
	};

	static Pool<sizeof(ObjectPlaceholder)> objPool;
	lint totalObjMem = 0;

	void * Object::operator new(__size size) {
		totalObjMem += objPool.size;
		return objPool.alloc(size);
	}

	void Object::operator delete(void *ptr) {
		return objPool.free(ptr);
	}

	//////

	class Clone : public ObjectBuiltIn {
	public:
		Object * run(Scope *scope) override;
		static Object *getInstance();
	};

	Object * Clone::run(Scope *scope) {
		Object *obj = new Object();
		scope->this_->clone(obj);
		return obj;
	}

	Object * Clone::getInstance() {
		static Clone __this;
		return &__this;
	}

	
	class InstanceOf : public ObjectBuiltIn {
	public:
		Object * run(Scope *scope) override;
		static Object *getInstance();
	};

	Object * InstanceOf::run(Scope *scope) {
		bool result = false;
		if (scope->arguments->vector.len() > 0) {
			Object *operand = scope->arguments->vector[0];
			result = scope->this_->instanceOf(operand);
		}
		return new ObjectBoolean(result);
	}

	Object * InstanceOf::getInstance() {
		static InstanceOf __this;
		return &__this;
	}


	class Inherit : public ObjectBuiltIn {
	protected:
		class StateImpl : public State {
		public:
			enum Step {
				CLONE,
				DONE
			};

			Inherit *expr;
			Step step;
			Object *cloned;
			Object *proto;

			StateImpl(State *_prev, Inherit *_expr) 
				: State(_prev), expr(_expr), step(CLONE), cloned(nullptr), proto(nullptr) {
			}
			State *next() override;
			void trace() override;
			void ret(Object *obj) override;
			Token * token() override;
		};

	public:
		State * createState(State *_prev) override;
		Object * run(Scope *scope) override;
		static Object *getInstance();
	};

	State * Inherit::createState(State *_prev) {
		return new StateImpl(_prev, this);
	}

	Object * Inherit::run(Scope *scope) {
		return nullptr;
	}

	Object * Inherit::getInstance() {
		static Inherit __this;
		return &__this;
	}

	State * Inherit::StateImpl::next() {
		switch (step) {
		case CLONE: {
			step = DONE;
			proto = scope->this_;
			Object *blank = scope->arguments->vector[0];
			Container *ctrClone = blank->find(Resource::i_clone());
			if (ctrClone->isPrimitive()) {
				// TODO: implement
			}
			else {
				Object *funcClone = ctrClone->data.obj;
				ObjectFunction *of = funcClone->toObjectFunction();
				if (of) {
					changeScope(of->context->clone());
					scope->arguments = nullptr;
					scope->this_ = blank;
					scope->proto.pushBack(scope->proto[0]);
					scope->proto[0] = blank;
					return of->function->createState(this);
				}
				ObjectBuiltIn * obi = funcClone->toObjectBuiltIn();
				if (obi) {
					cloneScope();
					scope->arguments = nullptr;
					scope->this_ = blank;
					return obi->createState(this);
				}
			}
			return throw_(new IsNotAFunction(Resource::s_clone));
		}
		case DONE: {
			ObjectArray *multi = proto->toObjectArray();
			if (multi) {
				cloned->proto.clean();
				multi->vector.clone(cloned->proto);
			}
			else {
				cloned->proto[0] = proto;
			}
			State *p = prev;
			p->ret(cloned);
			delete this;
			return p;
		}
		default:
			throw NotImplemented();
		}
	}

	void Inherit::StateImpl::trace() {
		if (cloned) {
			cloned->mark();
		}
	}

	Token * Inherit::StateImpl::token() {
		return nullptr;
	}

	void Inherit::StateImpl::ret(Object *obj) {
		cloned = obj;
	}


	class Flat : public ObjectBuiltIn {
	public:
		Object * run(Scope *scope) override;
		static Object *getInstance();
	};

	Object * Flat::run(Scope *scope) {
		Object *fobj = new Object();
		scope->this_->flat(fobj);
		return fobj;
	}

	Object * Flat::getInstance() {
		static Flat __this;
		return &__this;
	}


	class BaseEqual : public ObjectBuiltIn {
	public:
		Object * run(Scope *scope) override;
		static Object *getInstance();
	};

	Object * BaseEqual::run(Scope *scope) {
		Object *operand = scope->arguments->vector[0];
		if (!operand) {
			return new ObjectBoolean(false);
		}
		Container tmp(operand);
		return new ObjectBoolean(scope->this_->equals(&tmp));
	}

	Object * BaseEqual::getInstance() {
		static BaseEqual __this;
		return &__this;
	}


	class BaseNotEqual : public ObjectBuiltIn {
	public:
		Object * run(Scope *scope) override;
		static Object *getInstance();
	};

	Object * BaseNotEqual::run(Scope *scope) {
		Object *operand = scope->arguments->vector[0];
		if (!operand) {
			return new ObjectBoolean(true);
		}
		Container tmp(operand);
		return new ObjectBoolean(!scope->this_->equals(&tmp));
	}

	Object * BaseNotEqual::getInstance() {
		static BaseNotEqual __this;
		return &__this;
	}


	class BaseOr : public ObjectBuiltIn {
	public:
		Object * run(Scope *scope) override;
		static Object *getInstance();
	};

	Object * BaseOr::run(Scope *scope) {
		return scope->this_;
	}

	Object * BaseOr::getInstance() {
		static BaseOr __this;
		return &__this;
	}


	class BaseNot : public ObjectBuiltIn {
	public:
		Object * run(Scope *scope) override;
		static Object *getInstance();
	};

	Object * BaseNot::run(Scope *scope) {
		return new ObjectBoolean(false);
	}

	Object * BaseNot::getInstance() {
		static BaseNot __this;
		return &__this;
	}


	class Lock : public ObjectBuiltIn {
	protected:
		class StateImpl : public State {
		public:
			enum Step {
				CLONE,
				DONE
			};

			Lock *expr;
			Step step;
			Object *cloned;

			StateImpl(State *_prev, Lock *_expr)
				: State(_prev), expr(_expr), step(CLONE), cloned(nullptr) {
			}
			State *next() override;
			void trace() override;
			void ret(Object *obj) override;
			Token * token() override;
		};

	public:
		State * createState(State *_prev) override;
		Object * run(Scope *scope) override;
		static Object *getInstance();
	};

	State * Lock::createState(State *_prev) {
		return new StateImpl(_prev, this);
	}

	Object * Lock::run(Scope *scope) {
		return nullptr;
	}

	Object * Lock::getInstance() {
		static Lock __this;
		return &__this;
	}

	State * Lock::StateImpl::next() {
		switch (step) {
		case CLONE: {
			step = DONE;
			Object *blank = scope->this_;
			Container *ctrClone = scope->this_->find(Resource::i_clone());
			if (ctrClone->isPrimitive()) {
				// TODO: implement
			}
			else {
				Object *funcClone = ctrClone->data.obj;
				ObjectFunction *of = funcClone->toObjectFunction();
				if (of) {
					changeScope(of->context->clone());
					scope->arguments = nullptr;
					scope->this_ = blank;
					return of->function->createState(this);
				}
				ObjectBuiltIn * obi = funcClone->toObjectBuiltIn();
				if (obi) {
					cloneScope();
					scope->arguments = nullptr;
					scope->this_ = blank;
					return obi->createState(this);
				}
			}
			return throw_(new IsNotAFunction(Resource::s_clone));
		}
		case DONE: {
			State *p = prev;
			cloned->status |= Object::LOCKED;
			p->ret(cloned);
			delete this;
			return p;
		}
		default:
			throw NotImplemented();
		}
	}

	void Lock::StateImpl::trace() {
		if (cloned) {
			cloned->mark();
		}
	}

	Token * Lock::StateImpl::token() {
		return nullptr;
	}

	void Lock::StateImpl::ret(Object *obj) {
		cloned = obj;
	}


	SuperObject::SuperObject() : Object(true) {
		objects.insert(createIndex("clone"), Clone::getInstance());
		objects.insert(createIndex("flat"), Flat::getInstance());
		objects.insert(createIndex("instanceOf"), InstanceOf::getInstance());
		objects.insert(createIndex("->"), Inherit::getInstance());
		objects.insert(createIndex("=="), BaseEqual::getInstance());
		objects.insert(createIndex("!="), BaseNotEqual::getInstance());
		objects.insert(createIndex("||"), BaseOr::getInstance());
		objects.insert(createIndex("!"), BaseNot::getInstance());
		objects.insert(createIndex("@"), Lock::getInstance());
	}

	Object * SuperObject::getInstance() {
		static SuperObject __this;
		return &__this;
	}

	void SuperObject::flat(Object *fobj) {
		// do nothing
	}
}
