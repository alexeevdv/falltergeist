#include "../Game/Game.h"
#include "../Game/Location.h"
#include "../Helpers/GameLocationHelper.h"
#include "../Helpers/StateLocationHelper.h"
#include "../State/Location.h"

namespace Falltergeist
{
    namespace Helpers
    {
        State::Location* StateLocationHelper::getInitialLocationState() const
        {
            GameLocationHelper gameLocationHelper;
            auto initialLocation = gameLocationHelper.getInitialLocation();

            auto game = Game::getInstance();

            auto locationState = new State::Location(
                game->player(),
                game->mixer(),
                game->mouse(),
                game->settings(),
                game->gameTime(),
                game->renderer()
            );
            locationState->setElevation(initialLocation->defaultElevationIndex());
            locationState->setLocation(initialLocation);
            return locationState;
        }
    }
}
