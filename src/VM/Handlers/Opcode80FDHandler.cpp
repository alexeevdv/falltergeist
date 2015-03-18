/*
 * Copyright 2012-2014 Falltergeist Developers.
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
#include "../../Game/DudeObject.h"
#include "../../VM/Handlers/Opcode80FDHandler.h"
#include "../../VM/VM.h"

// Third party includes

namespace Falltergeist
{

Opcode80FDHandler::Opcode80FDHandler(VM* vm) : OpcodeHandler(vm)
{
}

void Opcode80FDHandler::_run()
{
    auto &debug = Logger::debug("SCRIPT");
    debug << "[80FD] [+] void radiation_inc(GameObject* who, int amount)" << std::endl;
    int amount = _vm->dataStack()->popInteger();
    debug << "    amount = " << amount << std::endl;
    auto object = _vm->dataStack()->popObject();
    if (!object)
    {
        _error("radiation_inc - object is NULL");
    }
    auto critter = dynamic_cast<Game::GameCritterObject*>(object);
    if (critter)
    {
        critter->setRadiationLevel(critter->radiationLevel() + amount);
    }
    else 
    {
        _warning("radiation_inc - object is not critter");
    }
}

}
