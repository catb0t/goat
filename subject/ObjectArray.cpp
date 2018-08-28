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

#include "ObjectArray.h"
#include "ObjectString.h"
#include "ObjectInteger.h"
#include "WideStringBuilder.h"

namespace goat {

	ObjectArray::ObjectArray() {
		proto[0] = Proto::getInstance();
	}

	ObjectArray * ObjectArray::toObjectArray() {
		return this;
	}

	void ObjectArray::trace() {
		vector.forEach([](Container &ctr) {
			ctr.mark();
		});
	}

	WideString ObjectArray::toWideString() {
		WideStringBuilder b;
		b << (wchar)'[';
		int i = 0;
		vector.forEach([&](Container &ctr) {
			if (i) {
				b << (wchar)',';
			}
			i++;
			b << ctr.toWideStringNotation();
		});
		b << (wchar)']';
		return b.toWideString();
	}


	class ArrayClone : public ObjectBuiltIn {
	public:
		Object * run(Scope *scope) override;
		static Object *getInstance();
	};

	class ArrayLength : public ObjectBuiltIn {
	public:
		Object * run(Scope *scope) override;
		static Object *getInstance();
	};

	class ArrayPush : public ObjectBuiltIn {
	public:
		Object * run(Scope *scope) override;
		static Object *getInstance();
	};

	ObjectArray::Proto::Proto() {
		status = PERMANENT | LOCKED;

		objects.insert(createIndex("clone"), ArrayClone::getInstance()->toContainer());
		objects.insert(createIndex("length"), ArrayLength::getInstance()->toContainer());
		objects.insert(createIndex("push"), ArrayPush::getInstance()->toContainer());
	}

	Object * ObjectArray::Proto::getInstance() {
		static Proto __this;
		return &__this;
	}


	Object * ArrayClone::run(Scope *scope) {
		ObjectArray *this_ = scope->this_->toObjectArray();
		ObjectArray *obj = new ObjectArray();
		this_->vector.clone(obj->vector);
		this_->clone(obj);
		return obj;
	}

	Object *ArrayClone::getInstance() {
		static ArrayClone __this;
		return &__this;
	}


	Object * ArrayLength::run(Scope *scope) {
		ObjectArray *this_ = scope->this_->toObjectArray();
		return new ObjectInteger(this_->vector.len());
	}

	Object *ArrayLength::getInstance() {
		static ArrayLength __this;
		return &__this;
	}


	Object * ArrayPush::run(Scope *scope) {
		ObjectArray *this_ = scope->this_->toObjectArray();
		unsigned int i, l = scope->arguments->vector.len();
		for (i = 0; i < l; i++) {
			this_->vector.pushBack(scope->arguments->vector[i]);
		}
		return nullptr;
	}

	Object *ArrayPush::getInstance() {
		static ArrayPush __this;
		return &__this;
	}
}
