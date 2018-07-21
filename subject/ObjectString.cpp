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

	bool ObjectString::equals(Object *_obj) {
		ObjectString *obj = _obj->toObjectString();
		return obj && obj->value == value;
	}

	ObjectString::Proto::Proto() {
		status = PERMANENT | LOCKED;

		objects.insert("+", OperatorPlus::getInstance());
		objects.insert("+=", OperatorPlus::getInstance());
		objects.insert("<", OperatorLess::getInstance());
		objects.insert("<=", OperatorLessEqual::getInstance());
		objects.insert(">", OperatorGreater::getInstance());
		objects.insert(">=", OperatorGreaterEqual::getInstance());
		objects.insert("!", OperatorNot::getInstance());
		objects.insert("length", Length::getInstance());
		objects.insert("clone", Clone::getInstance());
		objects.insert("valueOf", ValueOf::getInstance());
		objects.insert("subString", SubString::getInstance());
		objects.insert("split", Split::getInstance());
	}

	Object * ObjectString::Proto::getInstance() {
		static Proto __this;
		return &__this;
	}


	Object * ObjectString::Proto::OperatorPlus::run(Scope *scope) {
		ObjectString *this_ = scope->this_->toObjectString();
		Object *arg = scope->arguments->vector[0];
		if (arg) {
			return new ObjectString(this_->value + arg->toWideString());
		}
		// exception
		return nullptr;
	}

	Object * ObjectString::Proto::OperatorPlus::getInstance() {
		static OperatorPlus __this;
		return &__this;
	}


	Object * ObjectString::Proto::OperatorLess::run(Scope *scope) {
		ObjectString *this_ = scope->this_->toObjectString();
		ObjectString *operand = scope->arguments->vector[0]->toObjectString();
		if (!operand) {
			return new IllegalArgument();
		}
		return new ObjectBoolean(this_->value < operand->value);
	}

	Object * ObjectString::Proto::OperatorLess::getInstance() {
		static OperatorLess __this;
		return &__this;
	}


	Object * ObjectString::Proto::OperatorLessEqual::run(Scope *scope) {
		ObjectString *this_ = scope->this_->toObjectString();
		ObjectString *operand = scope->arguments->vector[0]->toObjectString();
		if (!operand) {
			return new IllegalArgument();
		}
		return new ObjectBoolean(this_->value <= operand->value);
	}

	Object * ObjectString::Proto::OperatorLessEqual::getInstance() {
		static OperatorLessEqual __this;
		return &__this;
	}


	Object * ObjectString::Proto::OperatorGreater::run(Scope *scope) {
		ObjectString *this_ = scope->this_->toObjectString();
		ObjectString *operand = scope->arguments->vector[0]->toObjectString();
		if (!operand) {
			return new IllegalArgument();
		}
		return new ObjectBoolean(this_->value > operand->value);
	}

	Object * ObjectString::Proto::OperatorGreater::getInstance() {
		static OperatorGreater __this;
		return &__this;
	}


	Object * ObjectString::Proto::OperatorGreaterEqual::run(Scope *scope) {
		ObjectString *this_ = scope->this_->toObjectString();
		ObjectString *operand = scope->arguments->vector[0]->toObjectString();
		if (!operand) {
			return new IllegalArgument();
		}
		return new ObjectBoolean(this_->value >= operand->value);
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
		Object *arg = scope->arguments->vector[0];
		if (!arg) {
			return new ObjectString(Resource::w_undefined);
		}
		return new ObjectString(arg->toWideString());
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
		ObjectInteger *start = scope->arguments->vector[0]->toObjectInteger();
		if (!start || start->value < 0) {
			return new IllegalArgument();
		}
		ObjectInteger *count = nullptr;
		if (scope->arguments->vector.len() > 1) {
			count = scope->arguments->vector[1]->toObjectInteger();
		}
		unsigned int start_ = (unsigned int)start->value,
			count_;
		if (count) {
			if (count->value < 0) {
				return new IllegalArgument();
			}
			count_ = (unsigned int)count->value;
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
		ObjectChar *chSeparator = scope->arguments->vector[0]->toObjectChar();
		if (chSeparator) {
			ObjectArray *result = new ObjectArray();
			unsigned int len = this_->value.len(),
				end = 0,
				begin = 0;
			while (end < len) {
				if (this_->value[end] == chSeparator->value) {
					result->vector.pushBack(new ObjectString((this_->value.subString(begin, end - begin))));
					begin = end + 1;
				}
				end++;
			}
			result->vector.pushBack(new ObjectString((this_->value.subString(begin, end - begin))));
			return result;
		}

		return new IllegalArgument();
	}

	Object * ObjectString::Proto::Split::getInstance() {
		static Split __this;
		return &__this;
	}
}
