/*
 * Copyright 2012-2018 Falltergeist Developers.
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

#ifndef FALLTERGEIST_GAME_LOCATIONELEVATION_H
#define FALLTERGEIST_GAME_LOCATIONELEVATION_H

// C++ standard includes
#include <memory>
#include <vector>

// Falltergeist includes


// Third party includes

namespace Falltergeist
{
    namespace UI
    {
        class TileMap;
    }
    namespace Game
    {
        class Object;

        /**
         * @brief Location elevation class
         *
         * This class represents each elevation of game location
         * It contains information about tiles and game objects
         */
        class LocationElevation
        {
            public:
                LocationElevation();
                ~LocationElevation();

                bool canRestHere() const;
                void setCanRestHere(bool value);

                std::shared_ptr<UI::TileMap> floor();
                std::shared_ptr<UI::TileMap> roof();

                std::vector<Object*>* objects();

            protected:
                /**
                 * @brief Can player rest here?
                 * As defined by `can_rest_here` in MAPS.TXT
                 */
                bool _canRestHere = true;

                std::shared_ptr<UI::TileMap> _floor;

                std::shared_ptr<UI::TileMap> _roof;

                /**
                 * @brief Array of game objects
                 */
                std::vector<Object*> _objects;
        };
    }
}
#endif // FALLTERGEIST_GAME_LOCATIONELEVATION_H
