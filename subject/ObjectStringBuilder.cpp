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
#include "ObjectInteger.h"

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
		status = PERMANENT | LOCKED;

		objects.insert(createIndex("clone"), Clone::getInstance()->toContainer());
		objects.insert(createIndex("toString"), ToString::getInstance()->toContainer());
		objects.insert(createIndex("<<"), Append::getInstance()->toContainer());
		objects.insert(createIndex("append"), Append::getInstance()->toContainer());
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
		unsigned int argsCount = scope->arguments->vector.len();
		if (argsCount == 0) {
			return new IllegalArgument();
		}
		WideString wstr = scope->arguments->vector[0].toWideString();
		if (argsCount == 1) {
			this_->builder << wstr;
		}
		else {
			lint lrepeat;
			if (scope->arguments->vector[1].getInteger(&lrepeat)) {
				if (lrepeat > uint32max)
					return new IllegalArgument();
				if (lrepeat > 0) {
					uint32 repeat = (uint32)lrepeat;
					this_->builder.reserve(wstr.len() * repeat);
					for (uint32 i = 0; i < repeat; i++) {
						this_->builder << wstr;
					}
				}
			}
			else {
				this_->builder << wstr;
			}
		}
		return this_;
	}

	Object * ObjectStringBuilder::Proto::Append::getInstance() {
		static Append __this;
		return &__this;
	}

}
