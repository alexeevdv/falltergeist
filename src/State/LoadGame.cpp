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
#include <sstream>

// Falltergeist includes
#include "../Event/Event.h"
#include "../Event/Mouse.h"
#include "../Event/State.h"
#include "../functions.h"
#include "../Game/Game.h"
#include "../Graphics/Renderer.h"
#include "../Input/Mouse.h"
#include "../ResourceManager.h"
#include "../State/State.h"
#include "../State/LoadGame.h"
#include "../UI/Image.h"
#include "../UI/ImageButton.h"
#include "../UI/TextArea.h"
#include "../UI/ImageList.h"

// Third party includes

namespace Falltergeist
{
namespace State
{

LoadGame::LoadGame() : State()
{
}

LoadGame::~LoadGame()
{
}

void LoadGame::init()
{
    if (_initialized) return;
    State::init();

    setModal(true);
    setFullscreen(true);

    auto game = Game::getInstance();
    //auto player = Game::getInstance()->player();

    // background
    auto bg = std::make_shared<UI::Image>("art/intrface/lsgame.frm");
    auto bgX = (game->renderer()->width() - bg->width())*0.5;
    auto bgY = (game->renderer()->height() - bg->height())*0.5;
    bg->setX(bgX);
    bg->setY(bgY);
    addUI(bg);

    // BUTTONS

    // button: up arrow
    addUI("button_up", std::make_shared<UI::ImageButton>(UI::ImageButton::Type::SMALL_UP_ARROW, bgX+35, bgY+58));
    // button: down arrow
    addUI("button_down", std::make_shared<UI::ImageButton>(UI::ImageButton::Type::SMALL_DOWN_ARROW, bgX+35, bgY+72));

    // button: Done
    auto doneButton = std::make_shared<UI::ImageButton>(UI::ImageButton::Type::SMALL_RED_CIRCLE, bgX+391, bgY+349);
    doneButton->addEventHandler("mouseleftclick", [this](Event::Event* event){ this->onDoneButtonClick(dynamic_cast<Event::Mouse*>(event)); });
    addUI(doneButton);

    // button: Cancel
    auto cancelButton = std::make_shared<UI::ImageButton>(UI::ImageButton::Type::SMALL_RED_CIRCLE, bgX+495, bgY+349);
    cancelButton->addEventHandler("mouseleftclick", [this](Event::Event* event){ this->doCancel(); });
    addUI(cancelButton);

    // LABELS

    auto font3_907824ff = ResourceManager::getInstance()->font("font3.aaf", 0x907824ff);

    // LOAD GAME LABEL
    auto saveGameLabel = std::make_shared<UI::TextArea>(_t(MSG_LOAD_SAVE, 110), bgX+48, bgY+27);
    saveGameLabel->setFont(font3_907824ff);
    addUI(saveGameLabel);

    // DONE BUTTON LABEL
    auto doneButtonLabel = std::make_shared<UI::TextArea>(_t(MSG_OPTIONS, 300), bgX+410, bgY+348);
    doneButtonLabel->setFont(font3_907824ff);
    addUI(doneButtonLabel);

    // CANCEL BUTTON LABEL
    auto cancelButtonLabel = std::make_shared<UI::TextArea>(_t(MSG_OPTIONS, 121), bgX+515, bgY+348);
    cancelButtonLabel->setFont(font3_907824ff);
    addUI(cancelButtonLabel);
}

void LoadGame::onDoneButtonClick(Event::Mouse* event)
{
    Game::getInstance()->popState();
}

void LoadGame::doCancel()
{
    if (!Game::getInstance()->locationState())
    {
        removeEventHandlers("fadedone");
        addEventHandler("fadedone", [this](Event::Event* event){ this->onCancelFadeDone(dynamic_cast<Event::State*>(event)); });
        Game::getInstance()->renderer()->fadeOut(255,255,255,1000);
    }
    else
    {
        Game::getInstance()->popState();
    }
}

void LoadGame::onCancelFadeDone(Event::State* event)
{
    removeEventHandlers("fadedone");
    Game::getInstance()->popState();
}

void LoadGame::onStateActivate(Event::State* event)
{
    if (!Game::getInstance()->locationState())
        Game::getInstance()->renderer()->fadeIn(0,0,0,1000);
    Game::getInstance()->mouse()->pushState(Input::Mouse::Cursor::BIG_ARROW);
}

void LoadGame::onStateDeactivate(Event::State* event)
{
    Game::getInstance()->mouse()->popState();
}

void LoadGame::onKeyDown(Event::Keyboard* event)
{
    switch (event->keyCode())
    {
        case SDLK_ESCAPE:
            doCancel();
            break;
    }
}


}
}
