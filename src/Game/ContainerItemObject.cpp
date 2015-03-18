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
#include "../Game/ContainerItemObject.h"
#include "../Game/Game.h"
#include "../State/Container.h"

// Third party includes

namespace Falltergeist
{
namespace Game
{

GameContainerItemObject::GameContainerItemObject() : GameItemObject()
{
    _subtype = TYPE_ITEM_CONTAINER;
}

GameContainerItemObject::~GameContainerItemObject()
{
}

std::vector<GameItemObject*>* GameContainerItemObject::inventory()
{
    return &_inventory;
}

void GameContainerItemObject::use_p_proc()
{
    auto state = new State::Container();
    state->setObject(this);
    Game::getInstance()->pushState(state);
}

void GameContainerItemObject::setLocked(bool _locked)
{
    this->_locked = _locked;
}

bool GameContainerItemObject::locked() const
{
    return _locked;
}

void GameContainerItemObject::setOpened(bool _opened)
{
    this->_opened = _opened;
}

bool GameContainerItemObject::opened() const
{
    return _opened;
}

}
}
