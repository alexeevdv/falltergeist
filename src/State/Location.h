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

#ifndef FALLTERGEIST_Location_H
#define FALLTERGEIST_Location_H

// C++ standard includes
#include <memory>

// Falltergeist includes
#include "State.h"
#include "../Input/Mouse.h"
#include "../State/PlayerPanel.h"
#include "../UI/ImageButton.h"

// Third party includes

namespace Falltergeist
{
namespace Game
{
    class Object;
}
namespace UI
{
    class Animation;
    class Image;
    class Tile;
    class TileMap;
}
class Hexagon;
class HexagonGrid;
class LocationCamera;
class VMStackValue;
class VM;

namespace State
{
    
/**
 * Location state works whenever player is on some map.
 * This state holds pointers to all object on current map, hexagon grid and other map-specific information.
 * 
 * pushed: when player starts new game or loads new game
 * popped: when player closes the game, exits to main menu or loads the game
 */
class Location : public State
{
protected:
    
    static const int KEYBOARD_SCROLL_STEP;
    static const int DROPDOWN_DELAY;

    // Timers
    unsigned int _scrollTicks = 0;
    unsigned int _scriptsTicks = 0;
    unsigned int _actionCursorTicks = 0;

    HexagonGrid* _hexagonGrid = 0;
    LocationCamera* _camera = 0;
    UI::TileMap* _floor = nullptr;
    UI::TileMap* _roof = nullptr;
    VM* _locationScript = 0;
    std::vector<int> _MVARS;
    std::map<std::string, VMStackValue> _EVARS;
    std::vector<UI::Base*> _floatMessages;

    bool _locationEnter = true;
    unsigned int _currentElevation = 0;
    unsigned int _lastClickedTile = 0;
    Game::Object* _objectUnderCursor = NULL;
    Game::Object* _actionCursorLastObject = NULL;
    bool _actionCursorButtonPressed = false;
    std::shared_ptr<PlayerPanel> _playerPanel;

    bool _scrollLeft = false;
    bool _scrollRight = false;
    bool _scrollTop = false;
    bool _scrollBottom = false;

    std::vector<Game::Object*> _objects;
    std::shared_ptr<UI::TextArea> _hexagonInfo;
    
    std::vector<Input::Mouse::Icon> getCursorIconsForObject(Game::Object* object);
public:
    Location();
    ~Location();

    void setLocation(const std::string& name);

    virtual void init();
    virtual void think();
    virtual void handle(Event::Event* event);
    virtual void render();

    HexagonGrid* hexagonGrid();
    LocationCamera* camera();

    void setMVAR(unsigned int number, int value);
    int MVAR(unsigned int number);

    std::map<std::string, VMStackValue>* EVARS();

    static void moveObjectToHexagon(Game::Object* object, Hexagon* hexagon);
    void destroyObject(Game::Object* object);
    void centerCameraAtHexagon(Hexagon* hexagon);
    void centerCameraAtHexagon(int tileNum);
    void handleAction(Game::Object* object, Input::Mouse::Icon action);
    void toggleCursorMode();
    PlayerPanel* playerPanelState();
    
    void displayMessage(const std::string& message);

    void onBackgroundClick(Event::Mouse* event);
    void onObjectMouseEvent(Event::Event* event, Game::Object* object);
    void onObjectHover(Event::Event* event, Game::Object* object);
    virtual void onKeyDown(Event::Keyboard* event);

    virtual void onStateActivate(Event::State* event);
    virtual void onStateDeactivate(Event::State* event);



};

}
}

#endif // FALLTERGEIST_Location_H
