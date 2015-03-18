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

#ifndef FALLTERGEIST_VMSTACK_H
#define FALLTERGEIST_VMSTACK_H

// C++ standard includes
#include <vector>
#include <memory>

// libfalltergeist includes

// Third party includes

namespace Falltergeist
{
class VMStackValue;

class VMStack
{
protected:
    std::vector<VMStackValue> _values;
public:
    VMStack();
    ~VMStack();
    void push(const VMStackValue &value);
    const VMStackValue& pop();
    const VMStackValue& top();
    std::vector<VMStackValue>* values();
    int size();
    void swap();
};

}
#endif // FALLTERGEIST_VMSTACK_H
