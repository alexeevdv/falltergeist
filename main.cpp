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

#include "src/Game/Game.h"
#include "src/Exception.h"
#include "src/Logger.h"
#include "src/Settings.h"
#include "State/GameBootingState.h"

using namespace Falltergeist;

int main(int argc, char* argv[])
{
    try
    {
        auto game = Game::Game::getInstance();

        game->init(std::make_unique<Settings>());
        game->setState(new State::GameBootingState());
        game->run();
        game->shutdown();

        return 0;
    }
    catch(const Exception &e)
    {
        Logger::critical() << e.what() << std::endl;
        return 1;
    }
}

