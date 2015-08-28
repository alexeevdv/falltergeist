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
#include "../functions.h"
#include "../Game/DudeObject.h"
#include "../Game/Game.h"
#include "../Graphics/Renderer.h"
#include "../ResourceManager.h"
#include "../State/ExitConfirm.h"
#include "../State/Skilldex.h"
#include "../State/Location.h"
#include "../State/SettingsMenu.h"
#include "../UI/BigCounter.h"
#include "../UI/Image.h"
#include "../UI/ImageButton.h"
#include "../UI/TextArea.h"
#include "../Input/Mouse.h"

// Third party includes
#include <libfalltergeist.h>

namespace Falltergeist
{
namespace State
{

Skilldex::Skilldex() : State()
{
}

Skilldex::~Skilldex()
{
}

void Skilldex::init()
{
    if (_initialized) return;
    State::init();

    setModal(true);
    setFullscreen(false);

    // original coordinates = 455x6
    // background size = 185x368
    auto background = std::make_shared<UI::Image>("art/intrface/skldxbox.frm");
    auto backgroundX = (Game::getInstance()->renderer()->width() + 640 - 2*background->width())*0.5;
    auto backgroundY = (Game::getInstance()->renderer()->height() - 480 + 6);
    background->setX(backgroundX);
    background->setY(backgroundY);

    // buttons
    auto sneakButton    = std::make_shared<UI::ImageButton>(UI::ImageButton::Type::SKILLDEX_BUTTON, backgroundX+14, backgroundY+44);
    auto lockpickButton = std::make_shared<UI::ImageButton>(UI::ImageButton::Type::SKILLDEX_BUTTON, backgroundX+14, backgroundY+44+36);
    auto stealButton    = std::make_shared<UI::ImageButton>(UI::ImageButton::Type::SKILLDEX_BUTTON, backgroundX+14, backgroundY+44+36*2);
    auto trapsButton    = std::make_shared<UI::ImageButton>(UI::ImageButton::Type::SKILLDEX_BUTTON, backgroundX+14, backgroundY+44+36*3);
    auto firstAidButton = std::make_shared<UI::ImageButton>(UI::ImageButton::Type::SKILLDEX_BUTTON, backgroundX+14, backgroundY+44+36*4);
    auto doctorButton   = std::make_shared<UI::ImageButton>(UI::ImageButton::Type::SKILLDEX_BUTTON, backgroundX+14, backgroundY+44+36*5);
    auto scienceButton  = std::make_shared<UI::ImageButton>(UI::ImageButton::Type::SKILLDEX_BUTTON, backgroundX+14, backgroundY+44+36*6);
    auto repairButton   = std::make_shared<UI::ImageButton>(UI::ImageButton::Type::SKILLDEX_BUTTON, backgroundX+14, backgroundY+44+36*7);
    auto cancelButton   = std::make_shared<UI::ImageButton>(UI::ImageButton::Type::SMALL_RED_CIRCLE, backgroundX+48, backgroundY+338);

    // counters
    auto sneakCounter    = std::make_shared<UI::BigCounter>(backgroundX + 111, backgroundY + 48, 3);
    auto lockpickCounter = std::make_shared<UI::BigCounter>(backgroundX + 111, backgroundY + 48 + 36, 3);
    auto stealCounter    = std::make_shared<UI::BigCounter>(backgroundX + 111, backgroundY + 48 + 36 * 2, 3);
    auto trapsCounter    = std::make_shared<UI::BigCounter>(backgroundX + 111, backgroundY + 48 + 36 * 3, 3);
    auto firstAidCounter = std::make_shared<UI::BigCounter>(backgroundX + 111, backgroundY + 48 + 36 * 4, 3);
    auto doctorCounter   = std::make_shared<UI::BigCounter>(backgroundX + 111, backgroundY + 48 + 36 * 5, 3);
    auto scienceCounter  = std::make_shared<UI::BigCounter>(backgroundX + 111, backgroundY + 48 + 36 * 6, 3);
    auto repairCounter   = std::make_shared<UI::BigCounter>(backgroundX + 111, backgroundY + 48 + 36 * 7, 3);

    //set counters with player's current skill values
    sneakCounter->setNumber(Game::getInstance()->player()->skillValue(SKILL::SNEAK));
    lockpickCounter->setNumber(Game::getInstance()->player()->skillValue(SKILL::LOCKPICK));
    stealCounter->setNumber(Game::getInstance()->player()->skillValue(SKILL::STEAL));
    trapsCounter->setNumber(Game::getInstance()->player()->skillValue(SKILL::TRAPS));
    firstAidCounter->setNumber(Game::getInstance()->player()->skillValue(SKILL::FIRST_AID));
    doctorCounter->setNumber(Game::getInstance()->player()->skillValue(SKILL::DOCTOR));
    scienceCounter->setNumber(Game::getInstance()->player()->skillValue(SKILL::SCIENCE));
    repairCounter->setNumber(Game::getInstance()->player()->skillValue(SKILL::REPAIR));

    // events
    cancelButton->addEventHandler("mouseleftclick", [this](Event::Event* event){ this->onCancelButtonClick(dynamic_cast<Event::Mouse*>(event)); });

    // LABELS
    auto font = ResourceManager::getInstance()->font("font3.aaf", 0xb89c28ff);

    // label: skilldex (100)
    auto skilldexLabel = std::make_shared<UI::TextArea>(_t(MSG_SKILLDEX, 100), backgroundX+56, backgroundY+14);
    skilldexLabel->setFont(font);
    skilldexLabel->setWidth(76);
    skilldexLabel->setHorizontalAlign(UI::TextArea::HorizontalAlign::CENTER);

    // label: sneak (102)
    auto sneakLabel = std::make_shared<UI::TextArea>(_t(MSG_SKILLDEX, 102), backgroundX+17, backgroundY+52);
    sneakLabel->setFont(font);
    sneakLabel->setWidth(84);
    sneakLabel->setHorizontalAlign(UI::TextArea::HorizontalAlign::CENTER);

    // label: lockpick (103)
    auto lockpickLabel = std::make_shared<UI::TextArea>(_t(MSG_SKILLDEX, 103), backgroundX+17, backgroundY+52+36);
    lockpickLabel->setFont(font);
    lockpickLabel->setWidth(84);
    lockpickLabel->setHorizontalAlign(UI::TextArea::HorizontalAlign::CENTER);

    // label: steal (104)
    auto stealLabel = std::make_shared<UI::TextArea>(_t(MSG_SKILLDEX, 104), backgroundX+17, backgroundY+52+36*2);
    stealLabel->setFont(font);
    stealLabel->setWidth(84);
    stealLabel->setHorizontalAlign(UI::TextArea::HorizontalAlign::CENTER);

    // label: traps (105)
    auto trapsLabel = std::make_shared<UI::TextArea>(_t(MSG_SKILLDEX, 105), backgroundX+17, backgroundY+52+36*3);
    trapsLabel->setFont(font);
    trapsLabel->setWidth(84);
    trapsLabel->setHorizontalAlign(UI::TextArea::HorizontalAlign::CENTER);

    // label: first aid (106)
    auto firstAidLabel = std::make_shared<UI::TextArea>(_t(MSG_SKILLDEX, 106), backgroundX+17, backgroundY+52+36*4);
    firstAidLabel->setFont(font);
    firstAidLabel->setWidth(84);
    firstAidLabel->setHorizontalAlign(UI::TextArea::HorizontalAlign::CENTER);

    // label: doctor (107)
    auto doctorLabel = std::make_shared<UI::TextArea>(_t(MSG_SKILLDEX, 107), backgroundX+17, backgroundY+52+36*5);
    doctorLabel->setFont(font);
    doctorLabel->setWidth(84);
    doctorLabel->setHorizontalAlign(UI::TextArea::HorizontalAlign::CENTER);

    // label: science (108)
    auto scienceLabel = std::make_shared<UI::TextArea>(_t(MSG_SKILLDEX, 108), backgroundX+17, backgroundY+52+36*6);
    scienceLabel->setFont(font);
    scienceLabel->setWidth(84);
    scienceLabel->setHorizontalAlign(UI::TextArea::HorizontalAlign::CENTER);

    // label: repair (109)
    auto repairLabel = std::make_shared<UI::TextArea>(_t(MSG_SKILLDEX, 109), backgroundX+17, backgroundY+52+36*7);
    repairLabel->setFont(font);
    repairLabel->setWidth(84);
    repairLabel->setHorizontalAlign(UI::TextArea::HorizontalAlign::CENTER);

    // label: cancel (101)
    auto cancelButtonLabel = std::make_shared<UI::TextArea>(_t(MSG_SKILLDEX, 101), backgroundX+70, backgroundY+337);
    cancelButtonLabel->setFont(font);

    // add all buttons and labels and counters
    addUI(background);
    addUI(sneakButton);
    addUI(lockpickButton);
    addUI(stealButton);
    addUI(trapsButton);
    addUI(firstAidButton);
    addUI(doctorButton);
    addUI(scienceButton);
    addUI(repairButton);
    addUI(cancelButton);
    addUI(skilldexLabel);
    addUI(sneakLabel);
    addUI(lockpickLabel);
    addUI(stealLabel);
    addUI(trapsLabel);
    addUI(firstAidLabel);
    addUI(doctorLabel);
    addUI(scienceLabel);
    addUI(repairLabel);
    addUI(cancelButtonLabel);
    addUI(sneakCounter);
    addUI(lockpickCounter);
    addUI(stealCounter);
    addUI(trapsCounter);
    addUI(firstAidCounter);
    addUI(doctorCounter);
    addUI(scienceCounter);
    addUI(repairCounter);

}

void Skilldex::onCancelButtonClick(Event::Mouse* event)
{
    Game::getInstance()->popState();
}

void Skilldex::onKeyDown(Event::Keyboard* event)
{
    if (event->keyCode() == SDLK_ESCAPE)
    {
        Game::getInstance()->popState();
    }
}

void Skilldex::onStateActivate(Event::State* event)
{
    Game::getInstance()->mouse()->pushState(Input::Mouse::Cursor::BIG_ARROW);
}

void Skilldex::onStateDeactivate(Event::State* event)
{
    Game::getInstance()->mouse()->popState();
}

}
}
