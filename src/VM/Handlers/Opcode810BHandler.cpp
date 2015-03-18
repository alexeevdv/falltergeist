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
#include "../../Exception.h"
#include "../../Logger.h"
#include "../../VM/Handlers/Opcode810BHandler.h"
#include "../../VM/VM.h"

// Third party includes

namespace Falltergeist
{

Opcode810BHandler::Opcode810BHandler(VM* vm) : OpcodeHandler(vm)
{
}

void Opcode810BHandler::_run()
{
    Logger::debug("SCRIPT") << "[810B] [*] int metarule(int type, value)" << std::endl;
    auto value = _vm->dataStack()->pop();
    auto type = _vm->dataStack()->popInteger();
    int result;

    switch(type)
    {
        case 14: // METARULE_TEST_FIRSTRUN
            result = 1;
            break;
        case 16: // METARULE_PARTY_COUNT
            result = 0;
            break;
        case 17: //  METARULE_AREA_KNOWN
            result = 1;
            break;
        case 18: // METARULE_WHO_ON_DRUGS
            result = 0;
            break;
        case 19: // METARULE_MAP_KNOWN
            result = 1;
            break;
        case 22: // METARULE_IS_LOADGAME
            result = 0;
            break;
        case 30: // METARULE_CAR_CURRENT_TOWN
            result = 0;
            break;
        case 40: // METARULE_SKILL_CHECK_TAG
            result = 0;
            break;
        case 44: // METARULE_GET_WORLDMAP_XPOS
            result = 300;
            break;
        case 45: // METARULE_GET_WORLDMAP_YPOS
            result = 300;
            break;
        case 46: // METARULE_CURRENT_TOWN
            result = 0;
            break;
        default:
            throw Exception("Opcode810BHandler - unimplemented type: " + std::to_string(type));
    }

    _vm->dataStack()->push(result);
}

}


