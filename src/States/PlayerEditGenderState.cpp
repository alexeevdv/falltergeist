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
 *
 */

// C++ standard includes

// Falltergeist includes
#include "../Engine/Game.h"
#include "../Engine/Graphics/Renderer.h"
#include "../Engine/ResourceManager.h"
#include "../Game/GameDudeObject.h"
#include "../States/PlayerEditGenderState.h"
#include "../UI/ImageList.h"
#include "../UI/Image.h"
#include "../UI/TextArea.h"
#include "../UI/ImageButton.h"

// Third party includes

namespace Falltergeist
{

PlayerEditGenderState::PlayerEditGenderState() : State()
{
}

void PlayerEditGenderState::init()
{
    State::init();
    setFullscreen(false);
    setModal(true);

    auto bgX = (Game::getInstance()->renderer()->width() - 640)*0.5;
    auto bgY = (Game::getInstance()->renderer()->height() - 480)*0.5;

    auto bg = new Image("art/intrface/charwin.frm");
    bg->setX(bgX+236);
    bg->setY(bgY+0);

    std::vector<std::string> maleImage = std::vector<std::string>{
        "art/intrface/maleoff.frm",
        "art/intrface/maleon.frm" };
    _maleImage = new ImageList(maleImage, bgX+260, bgY+2);
    _maleImage->addEventHandler("mouseleftclick", this, (EventRecieverMethod) &PlayerEditGenderState::onMaleButtonPress);
    if (Game::getInstance()->player()->gender() == 0) _maleImage->setCurrentImage(1); // 0 - male

    std::vector<std::string> femaleImage = std::vector<std::string>{
        "art/intrface/femoff.frm",
        "art/intrface/femon.frm" };
    _femaleImage = new ImageList(femaleImage, bgX + 310, bgY + 2);
    _femaleImage->addEventHandler("mouseleftclick", this, (EventRecieverMethod) &PlayerEditGenderState::onFemaleButtonPress);
    if (Game::getInstance()->player()->gender() == 1) _femaleImage->setCurrentImage(1); // 1 - female

    auto doneBox = new Image("art/intrface/donebox.frm");
    doneBox->setX(bgX+250);
    doneBox->setY(bgY+42);

    auto msg = ResourceManager::msgFileType("text/english/game/editor.msg");
    auto doneLabel = new TextArea(msg->message(100), bgX+281, bgY+45);
    auto font3_b89c28ff = ResourceManager::font("font3.aaf", 0xb89c28ff);
    doneLabel->setFont(font3_b89c28ff);

    auto doneButton = new ImageButton(ImageButton::TYPE_SMALL_RED_CIRCLE, bgX+260, bgY+45);
    doneButton->addEventHandler("mouseleftclick", this, (EventRecieverMethod) &PlayerEditGenderState::onDoneButtonClick);

    addUI(bg);
    addUI(doneBox);
    addUI(doneButton);
    addUI(doneLabel);
    addUI(_maleImage);
    addUI(_femaleImage);
}

void PlayerEditGenderState::onDoneButtonClick(MouseEvent* event)
{
    Game::getInstance()->popState();
}

void PlayerEditGenderState::onFemaleButtonPress(MouseEvent* event)
{
    auto state = dynamic_cast<PlayerEditGenderState*>(event->reciever());
    Game::getInstance()->player()->setGender(1); // 1 - female
    state->_maleImage->setCurrentImage(0);
    state->_femaleImage->setCurrentImage(1);
}

void PlayerEditGenderState::onMaleButtonPress(MouseEvent* event)
{
    auto state = dynamic_cast<PlayerEditGenderState*>(event->reciever());
    Game::getInstance()->player()->setGender(0); // 0 - male
    state->_maleImage->setCurrentImage(1);
    state->_femaleImage->setCurrentImage(0);
}

}
