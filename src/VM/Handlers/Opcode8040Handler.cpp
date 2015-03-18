/*
 * Copyright 2012-2015 Falltergeist Developers.
 *
 * This file is part of Falltergeist.
 *
 * Falltergeist is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Falltergeist is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Falltergeist.  If not, see <http://www.gnu.org/licenses/>.
 */

// C++ standard includes

// Falltergeist includes
#include "../../Logger.h"
#include "../../VM/Handlers/Opcode8040Handler.h"
#include "../../VM/VM.h"
#include "../../Exception.h"

// Third party includes

namespace Falltergeist
{

Opcode8040Handler::Opcode8040Handler(VM* vm) : OpcodeHandler(vm)
{
}

void Opcode8040Handler::_run()
{
    Logger::debug("SCRIPT") << "[8040] [*] op_bwand" << std::endl;
    auto bValue = _vm->dataStack()->pop();
    auto aValue = _vm->dataStack()->pop();
    if (!aValue.isNumber() || !bValue.isNumber()) 
    {
        throw Exception(std::string("op_bwand: invalid argument types: ") + aValue.typeName() + " bwand " + bValue.typeName());
    }
    _vm->dataStack()->push(aValue.toInteger() & bValue.toInteger());
}

}
