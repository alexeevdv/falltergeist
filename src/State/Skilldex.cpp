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
#include "../Game/Game.h"
#include "../Graphics/Renderer.h"
#include "../ResourceManager.h"
#include "../State/ExitConfirm.h"
#include "../State/Skilldex.h"
#include "../State/Location.h"
#include "../State/SettingsMenu.h"
#include "../UI/Image.h"
#include "../UI/ImageButton.h"
#include "../UI/TextArea.h"
#include "../Input/Mouse.h"

// Third party includes

namespace Falltergeist
{
namespace State
{

Skilldex::Skilldex()
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
    auto background = new Image("art/intrface/skldxbox.frm");
    auto backgroundX = (Game::getInstance()->renderer()->width() + 640 - 2*background->width())*0.5;
    auto backgroundY = (Game::getInstance()->renderer()->height() - 480 + 6);
    background->setX(backgroundX);
    background->setY(backgroundY);

    // buttons
    auto sneakButton = new ImageButton(ImageButton::TYPE_SKILLDEX_BUTTON, backgroundX+14, backgroundY+44);
    auto lockpickButton = new ImageButton(ImageButton::TYPE_SKILLDEX_BUTTON, backgroundX+14, backgroundY+44+36);
    auto stealButton = new ImageButton(ImageButton::TYPE_SKILLDEX_BUTTON, backgroundX+14, backgroundY+44+36*2);
    auto trapsButton = new ImageButton(ImageButton::TYPE_SKILLDEX_BUTTON, backgroundX+14, backgroundY+44+36*3);
    auto firstAidButton = new ImageButton(ImageButton::TYPE_SKILLDEX_BUTTON, backgroundX+14, backgroundY+44+36*4);
    auto doctorButton = new ImageButton(ImageButton::TYPE_SKILLDEX_BUTTON, backgroundX+14, backgroundY+44+36*5);
    auto scienceButton = new ImageButton(ImageButton::TYPE_SKILLDEX_BUTTON, backgroundX+14, backgroundY+44+36*6);
    auto repairButton = new ImageButton(ImageButton::TYPE_SKILLDEX_BUTTON, backgroundX+14, backgroundY+44+36*7);
    auto cancelButton = new ImageButton(ImageButton::TYPE_SMALL_RED_CIRCLE, backgroundX+48, backgroundY+338);

    // events
    cancelButton->addEventHandler("mouseleftclick", [this](Event* event){ this->onCancelButtonClick(dynamic_cast<MouseEvent*>(event)); });

    // LABELS
    auto font = ResourceManager::font("font3.aaf", 0xb89c28ff);

    // label: skilldex (100)
    auto skilldexLabel = new TextArea(_t(MSG_SKILLDEX, 100), backgroundX+56, backgroundY+14);
    skilldexLabel->setFont(font)->setWidth(76)->setHorizontalAlign(TextArea::HORIZONTAL_ALIGN_CENTER);

    // label: sneak (102)
    auto sneakLabel = new TextArea(_t(MSG_SKILLDEX, 102), backgroundX+17, backgroundY+52);
    sneakLabel->setFont(font)->setWidth(84)->setHorizontalAlign(TextArea::HORIZONTAL_ALIGN_CENTER);
    // label: lockpick (103)
    auto lockpickLabel = new TextArea(_t(MSG_SKILLDEX, 103), backgroundX+17, backgroundY+52+36);
    lockpickLabel->setFont(font)->setWidth(84)->setHorizontalAlign(TextArea::HORIZONTAL_ALIGN_CENTER);
    // label: steal (104)
    auto stealLabel = new TextArea(_t(MSG_SKILLDEX, 104), backgroundX+17, backgroundY+52+36*2);
    stealLabel->setFont(font)->setWidth(84)->setHorizontalAlign(TextArea::HORIZONTAL_ALIGN_CENTER);
    // label: traps (105)
    auto trapsLabel = new TextArea(_t(MSG_SKILLDEX, 105), backgroundX+17, backgroundY+52+36*3);
    trapsLabel->setFont(font)->setWidth(84)->setHorizontalAlign(TextArea::HORIZONTAL_ALIGN_CENTER);
    // label: first aid (106)
    auto firstAidLabel = new TextArea(_t(MSG_SKILLDEX, 106), backgroundX+17, backgroundY+52+36*4);
    firstAidLabel->setFont(font)->setWidth(84)->setHorizontalAlign(TextArea::HORIZONTAL_ALIGN_CENTER);
    // label: doctor (107)
    auto doctorLabel = new TextArea(_t(MSG_SKILLDEX, 107), backgroundX+17, backgroundY+52+36*5);
    doctorLabel->setFont(font)->setWidth(84)->setHorizontalAlign(TextArea::HORIZONTAL_ALIGN_CENTER);
    // label: science (108)
    auto scienceLabel = new TextArea(_t(MSG_SKILLDEX, 108), backgroundX+17, backgroundY+52+36*6);
    scienceLabel->setFont(font)->setWidth(84)->setHorizontalAlign(TextArea::HORIZONTAL_ALIGN_CENTER);
    // label: repair (109)
    auto repairLabel = new TextArea(_t(MSG_SKILLDEX, 109), backgroundX+17, backgroundY+52+36*7);
    repairLabel->setFont(font)->setWidth(84)->setHorizontalAlign(TextArea::HORIZONTAL_ALIGN_CENTER);

    // label: cancel (101)
    auto cancelButtonLabel = new TextArea(_t(MSG_SKILLDEX, 101), backgroundX+70, backgroundY+337);
    cancelButtonLabel->setFont(font);

    // add all buttons and labels
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
}

void Skilldex::onCancelButtonClick(MouseEvent* event)
{
    Game::getInstance()->popState();
}

void Skilldex::onKeyDown(KeyboardEvent* event)
{
    if (event->keyCode() == SDLK_ESCAPE)
    {
        Game::getInstance()->popState();
    }
}

void Skilldex::onStateActivate(StateEvent* event)
{
    Game::getInstance()->mouse()->pushState(Mouse::BIG_ARROW);
}

void Skilldex::onStateDeactivate(StateEvent* event)
{
    Game::getInstance()->mouse()->popState();
}


}
}
