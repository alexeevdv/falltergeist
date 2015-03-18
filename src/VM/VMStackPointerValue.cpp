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
#include "../VM/VMStackPointerValue.h"

// Third party includes

namespace Falltergeist
{

VMStackPointerValue::VMStackPointerValue(void* value) : VMStackValue(TYPE_OBJECT)
{
    _value = value;
}

VMStackPointerValue::~VMStackPointerValue()
{
}

void VMStackPointerValue::setValue(void* value)
{
    _value = value;
}

void* VMStackPointerValue::value()
{
    return _value;
}

void VMStackPointerValue::setPointerType(unsigned int type)
{
    _pointerType = type;
}

unsigned int VMStackPointerValue::pointerType()
{
    return _pointerType;
}

}
