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

#include "ObjectChar.h"
#include "ObjectBoolean.h"
#include "ObjectInteger.h"
#include "ObjectException.h"
#include "WideStringBuilder.h"

namespace goat {

	ObjectChar::ObjectChar(wchar _value) {
		value = _value;
		proto[0] = Proto::getInstance();
	}

	ObjectChar * ObjectChar::toObjectChar() {
		return this;
	}

	WideString ObjectChar::toWideString() {
		return WideString(&value, 1);
	}

	WideString ObjectChar::toWideStringNotation() {
		return (WideStringBuilder() << L'\'' << value << L'\'').toWideString();
	}

	bool ObjectChar::equals(Container *ctr) {
		if (ctr->handler != nullptr) {
			if (ctr->handler == getHandler())
				return ctr->data.C == value;
			return false;
		}
		ObjectChar *obj = ctr->data.obj->toObjectChar();
		return obj && obj->value == value;
	}

	ObjectChar::Proto::Proto() {
		status = PERMANENT | LOCKED;

		objects.insert(createIndex("clone"), Clone::getInstance()->toContainer());
		objects.insert(createIndex("valueOf"), ValueOf::getInstance()->toContainer());
		objects.insert(createIndex("++"), OperatorIncrement::getInstance()->toContainer());
		objects.insert(createIndex("--"), OperatorDecrement::getInstance()->toContainer());
		objects.insert(createIndex("!"), OperatorNot::getInstance()->toContainer());
		objects.insert(createIndex("<"), OperatorLess::getInstance()->toContainer());
		objects.insert(createIndex("<="), OperatorLessEqual::getInstance()->toContainer());
		objects.insert(createIndex(">"), OperatorGreater::getInstance()->toContainer());
		objects.insert(createIndex(">="), OperatorGreaterEqual::getInstance()->toContainer());
	}

	Object * ObjectChar::Proto::getInstance() {
		static Proto __this;
		return &__this;
	}


	Object * ObjectChar::Proto::OperatorLess::run(Scope *scope) {
		ObjectChar *this_ = scope->this_->toObjectChar();
		wchar operand;
		if (!scope->arguments->vector[0].getChar(&operand)) {
			return new IllegalArgument();
		}
		return new ObjectBoolean(this_->value < operand);
	}

	Object * ObjectChar::Proto::OperatorLess::getInstance() {
		static OperatorLess __this;
		return &__this;
	}


	Object * ObjectChar::Proto::OperatorLessEqual::run(Scope *scope) {
		ObjectChar *this_ = scope->this_->toObjectChar();
		wchar operand;
		if (!scope->arguments->vector[0].getChar(&operand)) {
			return new IllegalArgument();
		}
		return new ObjectBoolean(this_->value <= operand);
	}

	Object * ObjectChar::Proto::OperatorLessEqual::getInstance() {
		static OperatorLessEqual __this;
		return &__this;
	}


	Object * ObjectChar::Proto::OperatorGreater::run(Scope *scope) {
		ObjectChar *this_ = scope->this_->toObjectChar();
		wchar operand;
		if (!scope->arguments->vector[0].getChar(&operand)) {
			return new IllegalArgument();
		}
		return new ObjectBoolean(this_->value > operand);
	}

	Object * ObjectChar::Proto::OperatorGreater::getInstance() {
		static OperatorGreater __this;
		return &__this;
	}


	Object * ObjectChar::Proto::OperatorGreaterEqual::run(Scope *scope) {
		ObjectChar *this_ = scope->this_->toObjectChar();
		wchar operand;
		if (!scope->arguments->vector[0].getChar(&operand)) {
			return new IllegalArgument();
		}
		return new ObjectBoolean(this_->value >= operand);
	}

	Object * ObjectChar::Proto::OperatorGreaterEqual::getInstance() {
		static OperatorGreaterEqual __this;
		return &__this;
	}


	Object * ObjectChar::Proto::OperatorIncrement::run(Scope *scope) {
		ObjectChar *this_ = scope->this_->toObjectChar();
		return new ObjectChar(this_->value + 1);
	}

	Object * ObjectChar::Proto::OperatorIncrement::getInstance() {
		static OperatorIncrement __this;
		return &__this;
	}


	Object * ObjectChar::Proto::OperatorDecrement::run(Scope *scope) {
		ObjectChar *this_ = scope->this_->toObjectChar();
		return new ObjectChar(this_->value - 1);
	}

	Object * ObjectChar::Proto::OperatorDecrement::getInstance() {
		static OperatorDecrement __this;
		return &__this;
	}


	Object * ObjectChar::Proto::OperatorNot::run(Scope *scope) {
		ObjectChar *this_ = scope->this_->toObjectChar();
		return new ObjectBoolean(this_->value == 0);
	}

	Object * ObjectChar::Proto::OperatorNot::getInstance() {
		static OperatorNot __this;
		return &__this;
	}


	Object * ObjectChar::Proto::Clone::run(Scope *scope) {
		ObjectChar *this_ = scope->this_->toObjectChar();
		ObjectChar *obj = new ObjectChar(this_->value);
		this_->clone(obj);
		return obj;
	}

	Object * ObjectChar::Proto::Clone::getInstance() {
		static Clone __this;
		return &__this;
	}


	Object * ObjectChar::Proto::ValueOf::run(Scope *scope) {
		Container &arg = scope->arguments->vector[0];
		wchar argChar ;
		if (arg.getChar(&argChar)) {
			return new ObjectChar(argChar);
		}
		lint argInt;
		if (arg.getInteger(&argInt) && argInt > 0
#ifdef WCHAR_32
			&& argInt < 0x10FFFF
#endif
#ifdef WCHAR_16
			&& argInt < 0xFFFF
#endif
			) {
			return new ObjectChar((wchar)argInt);
		}
		return new ObjectChar(L'\0');
	}

	Object * ObjectChar::Proto::ValueOf::getInstance() {
		static ValueOf __this;
		return &__this;
	}

	class CharHandler : public PrimitiveHandler {
	public:
		static PrimitiveHandler *getInstance() {
			static CharHandler __this;
			return &__this;
		}

		Object * toObject(Container *ctr) override {
			return new ObjectChar(ctr->data.C);
		}

		bool equals(Container *left, Container *right) {
			if (right->handler != nullptr) {
				if (right->handler == this)
					return left->data.C == right->data.C;
				return false;
			}
			else {
				ObjectChar *robj = right->data.obj->toObjectChar();
				return robj && left->data.C == robj->value;
			}
		}

		WideString toWideString(Container *ctr) override {
			return WideString(&ctr->data.C, 1);
		}

		WideString toWideStringNotation(Container *ctr) override {
			return (WideStringBuilder() << L'\'' << ctr->data.C << L'\'').toWideString();;
		}
	};

	PrimitiveHandler * ObjectChar::getHandler() {
		return CharHandler::getInstance();
	}
}
