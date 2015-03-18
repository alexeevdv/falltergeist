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
#include <string>

// Falltergeist includes
#include "../VM/VMStack.h"
#include "../VM/VMStackValue.h"
#include "../Exception.h"

// Third party includes

namespace Falltergeist
{

VMStack::VMStack()
{
}

VMStack::~VMStack()
{
}

void VMStack::push(const VMStackValue& value)
{
    _values.push_back(value);
}

const VMStackValue VMStack::pop()
{
    if (_values.size() == 0) throw Exception("VMStack::pop() - stack is empty");
    auto value = _values.back();
    _values.pop_back();
    return value;
}

int VMStack::size()
{
    return _values.size();
}

void VMStack::swap()
{
    if (_values.size() < 2) throw Exception("VMStack::swap() - size is < 2");

    auto value1 = _values.back(); _values.pop_back();
    auto value2 = _values.back(); _values.pop_back();
    _values.push_back(value1);
    _values.push_back(value2);
}

std::vector<VMStackValue>* VMStack::values()
{
    return &_values;
}

const VMStackValue VMStack::top()
{
    return _values.back();
}

int VMStack::popInteger()
{
    return pop().integerValue();
}

void VMStack::push(unsigned int value)
{
    push((int)value);
}

void VMStack::push(int value)
{
    push(VMStackValue(value));
}

float VMStack::popFloat()
{
    return pop().floatValue();
}

void VMStack::push(float value)
{
    push(VMStackValue(value));
}

Game::GameObject* VMStack::popObject()
{
    return pop().objectValue();
}

void VMStack::push(Game::GameObject* value)
{
    push(VMStackValue(value));
}

std::string VMStack::popString()
{
    return pop().stringValue();
}

void VMStack::push(const std::string &value)
{
    push(VMStackValue(value));
}

bool VMStack::popLogical()
{
    return pop().toBoolean();
}

}
