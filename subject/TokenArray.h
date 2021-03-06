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

#pragma once

#include "Expression.h"
#include "Brackets.h"

namespace goat {

	class TokenArray : public Expression {
	protected:
		class StateImpl : public State {
		public:
			TokenArray *expr;
			Token *item;
			ObjectArray *result;

			StateImpl(State *_prev, TokenArray *_expr);
			State *next() override;
			void ret(Object *obj) override;
			void trace() override;
			Token *token() override;
		};

	public:
		TokenList *items;

		TokenArray(Brackets *tokItems);
		~TokenArray();
		void trace() override;
		TokenArray *toTokenArray() override;
		State * createState(State *_prev) override;
		String toString() override;
	};

}
