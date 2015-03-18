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
#include "../Audio/AudioMixer.h"
#include "../Graphics/Animation.h"
#include "../Graphics/AnimationQueue.h"
#include "../Logger.h"
#include "../Game/Game.h"
#include "../Game/DoorSceneryObject.h"
#include "../VM/VM.h"

// Third party includes

namespace Falltergeist
{
namespace Game
{

GameDoorSceneryObject::GameDoorSceneryObject() : GameSceneryObject()
{
    _subtype = TYPE_SCENERY_DOOR;
}

GameDoorSceneryObject::~GameDoorSceneryObject()
{
}

bool GameDoorSceneryObject::opened()
{
    return _opened;
}

void GameDoorSceneryObject::setOpened(bool value)
{
    _opened = value;
}

bool GameDoorSceneryObject::locked()
{
    return _locked;
}

void GameDoorSceneryObject::setLocked(bool value)
{
    _locked = value;
}

void GameDoorSceneryObject::use_p_proc()
{
    GameObject::use_p_proc();
    if (script() && script()->overrides()) return;

    if (!opened())
    {
        if (AnimationQueue* queue = dynamic_cast<AnimationQueue*>(this->ui()))
        {
            queue->start();
            queue->currentAnimation()->setReverse(false);
            queue->addEventHandler("animationEnded", std::bind(&GameDoorSceneryObject::onOpeningAnimationEnded, this, std::placeholders::_1));
            if (_soundId) Game::getInstance()->mixer()->playACMSound(std::string("sound/sfx/sodoors") + _soundId + ".acm");
        }
    }
    else
    {
        if (AnimationQueue* queue = dynamic_cast<AnimationQueue*>(this->ui()))
        {
            queue->start();
            queue->currentAnimation()->setReverse(true);
            queue->addEventHandler("animationEnded", std::bind(&GameDoorSceneryObject::onClosingAnimationEnded, this, std::placeholders::_1));
            if (_soundId) Game::getInstance()->mixer()->playACMSound(std::string("sound/sfx/scdoors") + _soundId + ".acm");
        }
    }
}

bool GameDoorSceneryObject::canWalkThru()
{
    return opened();
}

void GameDoorSceneryObject::onOpeningAnimationEnded(Event* event)
{
    auto queue = (AnimationQueue*)event->emitter();
    setOpened(true);
    queue->removeEventHandlers("animationEnded");
    queue->stop();
    Logger::info() << "Door opened: " << opened() << std::endl;
}

void GameDoorSceneryObject::onClosingAnimationEnded(Event* event)
{
    auto queue = (AnimationQueue*)event->emitter();
    setOpened(false);
    queue->removeEventHandlers("animationEnded");
    queue->stop();
    Logger::info() << "Door opened: " << opened() << std::endl;
}

}
}
