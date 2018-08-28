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

#include "ObjectString.h"
#include "ObjectInteger.h"
#include "ObjectBoolean.h"
#include "ObjectChar.h"
#include "ObjectException.h"
#include "WideStringBuilder.h"
#include "Resource.h"

namespace goat {

	ObjectString::ObjectString(WideString _value) {
		value = _value;
		proto[0] = Proto::getInstance();
	}

	ObjectString * ObjectString::toObjectString() {
		return this;
	}

	WideString ObjectString::toWideString() {
		return value;
	}

	WideString ObjectString::toWideStringNotation() {
		WideStringBuilder b;
		b << (wchar)'\"';
		for (unsigned int i = 0, l = value.len(); i < l; i++) {
			wchar c = value[i];
			switch (c) {
				case '\r':
					b << "\r"; break;
				case '\n':
					b << "\n"; break;
				case '\t':
					b << "\t"; break;
				case '\"':
					b << "\\\""; break;
				case '\\':
					b << "\\\\"; break;
				default:
					b << c;
			}
		}
		b << (wchar)'\"';
		return b.toWideString();
	}

	bool ObjectString::equals(Container *ctr) {
		if (ctr->isPrimitive())
			return false;
		ObjectString *obj = ctr->data.obj->toObjectString();
		return obj && obj->value == value;
	}

	ObjectString::Proto::Proto() {
		status = PERMANENT | LOCKED;

		objects.insert(createIndex("+"), OperatorPlus::getInstance()->toContainer());
		objects.insert(createIndex("+="), OperatorPlus::getInstance()->toContainer());
		objects.insert(createIndex("<"), OperatorLess::getInstance()->toContainer());
		objects.insert(createIndex("<="), OperatorLessEqual::getInstance()->toContainer());
		objects.insert(createIndex(">"), OperatorGreater::getInstance()->toContainer());
		objects.insert(createIndex(">="), OperatorGreaterEqual::getInstance()->toContainer());
		objects.insert(createIndex("!"), OperatorNot::getInstance()->toContainer());
		objects.insert(createIndex("length"), Length::getInstance()->toContainer());
		objects.insert(createIndex("clone"), Clone::getInstance()->toContainer());
		objects.insert(createIndex("valueOf"), ValueOf::getInstance()->toContainer());
		objects.insert(createIndex("subString"), SubString::getInstance()->toContainer());
		objects.insert(createIndex("split"), Split::getInstance()->toContainer());
	}

	Object * ObjectString::Proto::getInstance() {
		static Proto __this;
		return &__this;
	}


	Object * ObjectString::Proto::OperatorPlus::run(Scope *scope) {
		ObjectString *this_ = scope->this_->toObjectString();
		return new ObjectString(this_->value + scope->arguments->vector[0].toWideString());
	}

	Object * ObjectString::Proto::OperatorPlus::getInstance() {
		static OperatorPlus __this;
		return &__this;
	}


	Object * ObjectString::Proto::OperatorLess::run(Scope *scope) {
		ObjectString *this_ = scope->this_->toObjectString();
		WideString operand;
		if (!scope->arguments->vector[0].getString(&operand)) {
			return new IllegalArgument();
		}
		return new ObjectBoolean(this_->value < operand);
	}

	Object * ObjectString::Proto::OperatorLess::getInstance() {
		static OperatorLess __this;
		return &__this;
	}


	Object * ObjectString::Proto::OperatorLessEqual::run(Scope *scope) {
		ObjectString *this_ = scope->this_->toObjectString();
		WideString operand;
		if (!scope->arguments->vector[0].getString(&operand)) {
			return new IllegalArgument();
		}
		return new ObjectBoolean(this_->value <= operand);
	}

	Object * ObjectString::Proto::OperatorLessEqual::getInstance() {
		static OperatorLessEqual __this;
		return &__this;
	}


	Object * ObjectString::Proto::OperatorGreater::run(Scope *scope) {
		ObjectString *this_ = scope->this_->toObjectString();
		WideString operand;
		if (!scope->arguments->vector[0].getString(&operand)) {
			return new IllegalArgument();
		}
		return new ObjectBoolean(this_->value > operand);
	}

	Object * ObjectString::Proto::OperatorGreater::getInstance() {
		static OperatorGreater __this;
		return &__this;
	}


	Object * ObjectString::Proto::OperatorGreaterEqual::run(Scope *scope) {
		ObjectString *this_ = scope->this_->toObjectString();
		WideString operand;
		if (!scope->arguments->vector[0].getString(&operand)) {
			return new IllegalArgument();
		}
		return new ObjectBoolean(this_->value >= operand);
	}

	Object * ObjectString::Proto::OperatorGreaterEqual::getInstance() {
		static OperatorGreaterEqual __this;
		return &__this;
	}


	Object * ObjectString::Proto::OperatorNot::run(Scope *scope) {
		ObjectString *this_ = scope->this_->toObjectString();
		return new ObjectBoolean(this_->value.len() == 0);
	}

	Object * ObjectString::Proto::OperatorNot::getInstance() {
		static OperatorNot __this;
		return &__this;
	}


	Object * ObjectString::Proto::Length::run(Scope *scope) {
		ObjectString *this_ = scope->this_->toObjectString();
		return new ObjectInteger(this_->value.len());
	}

	Object * ObjectString::Proto::Length::getInstance() {
		static Length __this;
		return &__this;
	}


	Object * ObjectString::Proto::Clone::run(Scope *scope) {
		ObjectString *this_ = scope->this_->toObjectString();
		ObjectString *obj = new ObjectString(this_->value);
		this_->clone(obj);
		return obj;
	}

	Object * ObjectString::Proto::Clone::getInstance() {
		static Clone __this;
		return &__this;
	}


	Object * ObjectString::Proto::ValueOf::run(Scope *scope) {
		return new ObjectString(scope->arguments->vector[0].toWideString());
	}

	Object * ObjectString::Proto::ValueOf::getInstance() {
		static ValueOf __this;
		return &__this;
	}


	Object * ObjectString::Proto::SubString::run(Scope *scope) {
		ObjectString *this_ = scope->this_->toObjectString();
		if (scope->arguments->vector.len() < 1) {
			return new IllegalArgument();
		}
		lint start,
		 	 count;
		if (!scope->arguments->vector[0].getInteger(&start) || start < 0 || start > uint32max) {
			return new IllegalArgument();
		}
		uint32 start_ = (uint32)start,
			count_;
		if (scope->arguments->vector.len() > 1) {
			if (!scope->arguments->vector[1].getInteger(&count) || count < 0 || count > uint32max) {
				return new IllegalArgument();
			}
			count_ = (uint32)count;
		}
		else {
			count_ = this_->value.len() - start_;
		}
		return new ObjectString(this_->value.subString(start_, count_));
	}

	Object * ObjectString::Proto::SubString::getInstance() {
		static SubString __this;
		return &__this;
	}


	Object * ObjectString::Proto::Split::run(Scope *scope) {
		ObjectString *this_ = scope->this_->toObjectString();
		if (scope->arguments->vector.len() < 1) {
			return new IllegalArgument();
		}
		wchar chSeparator;
		if (scope->arguments->vector[0].getChar(&chSeparator)) {
			ObjectArray *result = new ObjectArray();
			uint32 len = this_->value.len(),
				end = 0,
				begin = 0;
			while (end < len) {
				if (this_->value[end] == chSeparator) {
					result->vector.pushBack((new ObjectString((this_->value.subString(begin, end - begin))))->toContainer());
					begin = end + 1;
				}
				end++;
			}
			result->vector.pushBack((new ObjectString((this_->value.subString(begin, end - begin))))->toContainer());
			return result;
		}

		return new IllegalArgument();
	}

	Object * ObjectString::Proto::Split::getInstance() {
		static Split __this;
		return &__this;
	}
}
