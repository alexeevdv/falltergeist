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

// Related headers
#include "../Event/Event.h"

// C++ standard includes

// Falltergeist includes

// Third party includes

namespace Falltergeist
{
namespace Event
{

Event::Event(const std::string& name)
{
    _name = name;
}

Event::~Event()
{
}

std::string Event::name() const
{
    return _name;
}

void Event::setName(const std::string& name)
{
    _name = name;
}

EventTarget* Event::eventTarget() const
{
    return _eventTarget;
}

void Event::setEventTarget(EventTarget* value)
{
    _eventTarget = value;
}

bool Event::handled() const
{
    return _handled;
}

void Event::setHandled(bool value)
{
    _handled = value;
}

}
}
