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
#include "../../VM/Handlers/Opcode8041Handler.h"
#include "../../VM/VM.h"

// Third party includes

namespace Falltergeist
{

Opcode8041Handler::Opcode8041Handler(VM* vm) : OpcodeHandler(vm)
{
}

void Opcode8041Handler::_run()
{
    Logger::debug("SCRIPT") << "[8041] [*] op_bwor" << std::endl;
    auto bValue = _vm->dataStack()->pop();
    auto aValue = _vm->dataStack()->pop();
    if (!aValue.isNumber() || !bValue.isNumber()) 
    {
        _error(std::string("op_bwor: invalid argument types: ") + aValue.typeName() + " bwor " + bValue.typeName());
    }
    _vm->dataStack()->push(aValue.toInteger() | bValue.toInteger());
}

}
