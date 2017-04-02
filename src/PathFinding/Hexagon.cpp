/*
 * Copyright 2012-2016 Falltergeist Developers.
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
#include <cmath>

// Falltergeist includes
#include "../Game/DoorSceneryObject.h"
#include "../Game/Game.h"
#include "../Game/Object.h"
#include "../Logger.h"
#include "../PathFinding/Hexagon.h"

// Third party includes

namespace Falltergeist
{

Hexagon::Hexagon(){}

Hexagon::Hexagon(unsigned int number)
{
    setNumber(number);
}

std::array<Hexagon*, 6> &Hexagon::neighbors()
{
    return _neighbors;
}

std::list<Game::Object*>* Hexagon::objects()
{
    return &_objects;
}

const Point& Hexagon::position() const
{
    return _position;
}

void Hexagon::setPosition(const Point& pos)
{
    _position = pos;
}

bool Hexagon::canWalkThru()
{
    bool canWalkThru = true;
    for (auto object : _objects)
    {
        if (auto door = dynamic_cast<Game::DoorSceneryObject*>(object))
        {
            return door->canWalkThru();
        }
        if (!object->canWalkThru()) canWalkThru = false;
    }
    return canWalkThru;
}



Game::Orientation Hexagon::orientationTo(Hexagon *hexagon)
{
    Point delta = hexagon->position() - _position;
    int dx = delta.x();
    int dy = delta.y();

    unsigned char result;

    if (dx)
    {
        // trigonometry magic.
        // basically, we try to find angle to second hex in circle, where first hex is center
        // and then find out to which of 60� slices it belongs

        double degree = atan2((double)-dy, (double)dx) * 180.0 * 0.3183098862851122; //  180 * 1/PI

        result = (360 - ((signed int)degree + 180) - 90 + ((char)-(360 - ((signed int)degree + 180) - 90 < 0) & 360)) / 60;
        if ( result > 5 )
        {
            result = 5;
        }
    }
    else if ( dy < 0 )
    {
        result = 0;
    }
    else
    {
        result = 2;
    }

    return Game::Orientation(result); // TODO: this is wrong. orientation!=direction
}

unsigned int Hexagon::addLight(unsigned int light)
{
    _light+=light;
    if (_light > 65536) _light = 65536;
    return _light;
}

unsigned int Hexagon::subLight(unsigned int light)
{
    _light-=light;
    if ((int)_light < 655) _light = 655;
    return _light;
}

unsigned int Hexagon::light()
{
    return _light;
}

unsigned int Hexagon::setLight(unsigned int light)
{
    _light = light;
    return _light;
}
}
