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
#include <iostream>

// Falltergeist includes
#include "../Engine/Game.h"
#include "../Engine/Graphics/Renderer.h"
#include "../Engine/Input/Mouse.h"
#include "../Engine/ResourceManager.h"
#include "../States/WorldMapState.h"
#include "../States/LocationState.h"
#include "../States/MainMenuState.h"
#include "../UI/Image.h"
#include "../UI/ImageButton.h"
#include "../UI/ImageList.h"
#include "../UI/TextArea.h"

// Third party includes

namespace Falltergeist
{

WorldMapState::WorldMapState()
{
}

void WorldMapState::init()
{
    if (_initialized) return;
    State::init();

    setModal(true);
    setFullscreen(true);

    unsigned int renderWidth = Game::getInstance()->renderer()->width();
    unsigned int renderHeight = Game::getInstance()->renderer()->height();

    // loading map tiles
    std::vector<std::string> imagelist = std::vector<std::string>{
        "art/intrface/wrldmp00.frm",
        "art/intrface/wrldmp01.frm",
        "art/intrface/wrldmp02.frm",
        "art/intrface/wrldmp03.frm",
        "art/intrface/wrldmp04.frm",
        "art/intrface/wrldmp05.frm",
        "art/intrface/wrldmp06.frm",
        "art/intrface/wrldmp07.frm",
        "art/intrface/wrldmp08.frm",
        "art/intrface/wrldmp09.frm",
        "art/intrface/wrldmp10.frm",
        "art/intrface/wrldmp11.frm",
        "art/intrface/wrldmp12.frm",
        "art/intrface/wrldmp13.frm",
        "art/intrface/wrldmp14.frm",
        "art/intrface/wrldmp15.frm",
        "art/intrface/wrldmp16.frm",
        "art/intrface/wrldmp17.frm",
        "art/intrface/wrldmp18.frm",
        "art/intrface/wrldmp19.frm"};
    _tiles = new ImageList(imagelist, 0, 0);

    //auto cross = new Image("art/intrface/wmaploc.frm");
    _hotspot = new ImageButton(ImageButton::TYPE_MAP_HOTSPOT, 0, 0);
    //addUI(_hotspot);

    // creating screen
    if (WorldMapFullscreen)
    {
        _panel = new Image("art/intrface/wminfce2.frm"); // panel small
        mapWidth = renderWidth - 168;
        mapHeight = renderHeight;
        mapMinX = 0;
        mapMinY = 0;
    }
    else
    {
        _panel = new Image("art/intrface/wmapbox.frm"); // panel full
        mapWidth = 450;   // fallout 2 map screen width
        mapHeight = 442;  // fallout 2 map screen height
        mapMinX = (renderWidth - 640)*0.5 + 22;
        mapMinY = (renderHeight - 480)*0.5 + 21;
    }
    _screenMap = new Image (mapWidth, mapHeight);
    _screenMap->setX(mapMinX);
    _screenMap->setY(mapMinY);
}

void WorldMapState::render()
{
    // calculating render size, screen size, etc
    unsigned int renderWidth = Game::getInstance()->renderer()->width();
    unsigned int renderHeight = Game::getInstance()->renderer()->height();

    // MAP SHOW
    // calculating delta (shift of map to fit to screen)
    deltaX = worldMapX - mapWidth*0.5;
    deltaY = worldMapY - mapHeight*0.5;

    unsigned int worldMapSizeX = tilesNumberX*tileWidth;
    unsigned int worldMapSizeY = tilesNumberY*tileHeight;

    // correcting delta
    if (deltaX<0)
    {
        deltaX = 0;
    }
    if (deltaY<0)
    {
        deltaY = 0;
    }
    if (worldMapSizeX-deltaX < mapWidth)
    {
        deltaX = worldMapSizeX - mapWidth;
    }
    if (worldMapSizeY-deltaY < mapHeight)
    {
        deltaY = worldMapSizeY - mapHeight;
    }

    signed int worldTileMinX; // start X coordinate of current tile on world map
    signed int worldTileMinY; // start Y coordinate of current tile on world map
    // NB: can be unsigned, but it compared with signed deltaX and deltaY, so...

    // copy tiles to screen if needed
    for (unsigned int y=0; y<tilesNumberY; y++)
    {
        for (unsigned int x=0; x<tilesNumberX; x++)
        {
            _tiles->setCurrentImage(y*tilesNumberX+x);
            worldTileMinX = x*tileWidth;
            worldTileMinY = y*tileHeight;
            // checking if tile is visible on screenMap
            // checking current tile borders
            // either xmin or xmax SHOULD belongs to map area AND
            // either ymin or ymax SHOULD belongs to map area
            if ( (((deltaX<=worldTileMinX) && (worldTileMinX<=deltaX+(signed int)mapWidth)) ||
                  ((deltaX<=worldTileMinX+(signed int)tileWidth) && (worldTileMinX+(signed int)tileWidth<=deltaX+(signed int)mapWidth))) &&
                 (((deltaY<=worldTileMinY) && (worldTileMinY<=deltaY+(signed int)mapHeight)) ||
                  ((deltaY<=worldTileMinY+(signed int)tileHeight) && (worldTileMinY+(signed int)tileHeight<=deltaY+(signed int)mapHeight))) )
            {
                _tiles->texture()->copyTo(_screenMap->texture(), x*tileWidth-deltaX, y*tileHeight-deltaY, 0, 0, tileWidth, tileHeight);
            }
        }
    }
    _screenMap->render();

    // hostpot show
    _hotspot->setX(mapMinX + worldMapX - deltaX);
    _hotspot->setY(mapMinY + worldMapY - deltaY);
    _hotspot->render();

    // panel
    unsigned int panelX;
    unsigned int panelY;

    if (WorldMapFullscreen)
    {
        panelX = renderWidth - 168; // only panel right
    }
    else
    {
        panelX = (renderWidth - _panel->width())*0.5;
    }
    panelY = (renderHeight - _panel->height())*0.5;

    _panel->setX(panelX);
    _panel->setY(panelY);
    _panel->render();

}

void WorldMapState::handle(Event* event)
{
    auto game = Game::getInstance();

    if (auto mouseEvent = dynamic_cast<MouseEvent*>(event))
    {
        auto mouse = game->mouse();

        // Left button down
        if (mouseEvent->name() == "mousedown" && mouseEvent->leftButton())
        {
            // check if point clicked belongs to the screen
            if ((mapMinX<=(unsigned int)mouse->x()) && ((unsigned int)mouse->x()<=(mapMinX+mapWidth)) &&
                (mapMinY<=(unsigned int)mouse->y()) && ((unsigned int)mouse->y()<=(mapMinY+mapHeight)))
            {
                // change destination point
                worldMapX = mouse->x()+deltaX-mapMinX;
                worldMapY = mouse->y()+deltaY-mapMinY;
            }
        }
    }

    if (auto keyboardEvent = dynamic_cast<KeyboardEvent*>(event))
    {
        switch (keyboardEvent->keyCode())
        {
            case SDLK_ESCAPE:
            {
                game->popState();
            }
        }
    }
}

}

