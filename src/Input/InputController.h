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

#ifndef FALLTERGEIST_INPUT_INPUTCONTROLLER_H
#define FALLTERGEIST_INPUT_INPUTCONTROLLER_H

// C++ standard includes

// Falltergeist includes

// Third party includes

namespace Falltergeist
{

namespace Lua
{
    class Script;
}

class Mouse;

class InputController
{
public:
    static void exportToLuaScript(Lua::Script* script);

    InputController(Mouse* mouse);

    unsigned int mouseCursorType() const;
    void setMouseCursorType(unsigned int type);

private:
    InputController(const InputController&) = delete;
    void operator=(const InputController&) = delete;

    Mouse* const _mouse;
};

}

#endif // FALLTERGEIST_INPUT_INPUTCONTROLLER_H