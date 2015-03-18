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

#ifndef FALLTERGEIST_GAMEITEMOBJECT_H
#define FALLTERGEIST_GAMEITEMOBJECT_H

// C++ standard includes

// Falltergeist includes
#include "Object.h"

// Third party includes

namespace Falltergeist
{
namespace Game
{
    
/**
 * Item. Can be placed inside other object inventories (critters and containers)
 */
class GameItemObject : public GameObject
{
protected:
    unsigned int _amount = 1;
    unsigned int _weight = 0;
    unsigned int _volume = 0;
    int _inventoryFID = -1;
    Image* _inventoryUi = 0;
    Image* _inventorySlotUi = 0;
    Image* _inventoryDragUi = 0;
    virtual void _generateUi();
public:
    GameItemObject();
    virtual ~GameItemObject();

    // item stack size in inventory or on the ground
    unsigned int amount() const;
    void setAmount(unsigned int value);

    // item weight, in pounds
    unsigned int weight() const;
    void setWeight(unsigned int value);
    
    // item volume
    unsigned int volume() const;
    void setVolume(unsigned int volume);

    // item inventory frame ID
    int inventoryFID() const;
    void setInventoryFID(int value);

    Image* inventoryUi() const;
    Image* inventorySlotUi() const;
    Image* inventoryDragUi() const;
};

}
}

#endif // FALLTERGEIST_GAMEITEMOBJECT_H
