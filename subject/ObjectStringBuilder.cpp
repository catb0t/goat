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

#include "ObjectStringBuilder.h"
#include "ObjectException.h"
#include "ObjectString.h"

namespace goat {

	ObjectStringBuilder::ObjectStringBuilder() {
		proto[0] = Proto::getInstance();
	}

	ObjectStringBuilder * ObjectStringBuilder::toObjectStringBuilder() {
		return this;
	}

	WideString ObjectStringBuilder::toWideString() {
		return builder.toWideString();
	}

	ObjectStringBuilder::Proto::Proto() {
		status = PERMANENT;

		objects.insert("clone", Clone::getInstance());
		objects.insert("toString", ToString::getInstance());
		objects.insert("<<", Append::getInstance());
		objects.insert("append", Append::getInstance());
	}

	Object * ObjectStringBuilder::Proto::getInstance() {
		static Proto __this;
		return &__this;
	}


	Object * ObjectStringBuilder::Proto::Clone::run(Scope *scope) {
		ObjectStringBuilder *this_ = scope->this_->toObjectStringBuilder();
		ObjectStringBuilder *obj = new ObjectStringBuilder();
		obj->builder << this_->builder.toWideString();
		this_->clone(obj);
		return obj;
	}

	Object * ObjectStringBuilder::Proto::Clone::getInstance() {
		static Clone __this;
		return &__this;
	}


	Object * ObjectStringBuilder::Proto::ToString::run(Scope *scope) {
		ObjectStringBuilder *this_ = scope->this_->toObjectStringBuilder();
		return new ObjectString(this_->builder.toWideString());
	}

	Object * ObjectStringBuilder::Proto::ToString::getInstance() {
		static ToString __this;
		return &__this;
	}


	Object * ObjectStringBuilder::Proto::Append::run(Scope *scope) {
		ObjectStringBuilder *this_ = scope->this_->toObjectStringBuilder();
		Object *operand = scope->arguments->vector[0];
		if (!operand) {
			return new IllegalArgument();
		}
		this_->builder << operand->toWideString();
		return this_;
	}

	Object * ObjectStringBuilder::Proto::Append::getInstance() {
		static Append __this;
		return &__this;
	}

}