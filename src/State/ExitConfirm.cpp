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
#include "../State/Location.h"
#include "../State/MainMenu.h"
#include "../UI/Image.h"
#include "../UI/ImageButton.h"
#include "../UI/TextArea.h"
#include "../Input/Mouse.h"

// Third party includes

namespace Falltergeist
{
namespace State
{

ExitConfirm::ExitConfirm() : State()
{
}

ExitConfirm::~ExitConfirm()
{
}

void ExitConfirm::init()
{
    if (_initialized) return;
    State::init();

    setModal(true);
    setFullscreen(false);

    auto background = std::make_shared<UI::Image>("art/intrface/lgdialog.frm");
    auto panelHeight = Game::getInstance()->locationState()->playerPanelState()->height();

    auto backgroundX = (Game::getInstance()->renderer()->width() - background->width())*0.5;
    auto backgroundY = (Game::getInstance()->renderer()->height() - background->height() - panelHeight)*0.5;

    auto box1 = std::make_shared<UI::Image>("art/intrface/donebox.frm");
    auto box2 = std::make_shared<UI::Image>("art/intrface/donebox.frm");
    box1->setX(backgroundX+38);
    box1->setY(backgroundY+98);
    box2->setX(backgroundX+170);
    box2->setY(backgroundY+98);

    auto yesButton = std::make_shared<UI::ImageButton>(UI::ImageButton::Type::SMALL_RED_CIRCLE, backgroundX+50, backgroundY+102);
    auto noButton = std::make_shared<UI::ImageButton>(UI::ImageButton::Type::SMALL_RED_CIRCLE, backgroundX+183, backgroundY+102);
    yesButton->addEventHandler("mouseleftclick", [this](Event::Event* event){ this->doYes(); });
    noButton->addEventHandler("mouseleftclick",  [this](Event::Event* event){ this->doNo(); });

    // label: Are you sure you want to quit?
    auto font = ResourceManager::getInstance()->font("font1.aaf", 0xb89c28ff);
    auto quitLabel = std::make_shared<UI::TextArea>(_t(MSG_MISC, 0), backgroundX+30, backgroundY+52);
    quitLabel->setFont(font);
    quitLabel->setWidth(244);
    quitLabel->setHorizontalAlign(UI::TextArea::HorizontalAlign::CENTER);

    // label: yes & no
    auto fontBig = ResourceManager::getInstance()->font("font3.aaf", 0xb89c28ff);
    // label: yes 101
    auto yesButtonLabel = std::make_shared<UI::TextArea>(_t(MSG_DIALOG_BOX, 101), backgroundX+74, backgroundY+101);
    yesButtonLabel->setFont(fontBig);
    // label: no 102
    auto noButtonLabel = std::make_shared<UI::TextArea>(_t(MSG_DIALOG_BOX, 102), backgroundX+204, backgroundY+101);
    noButtonLabel->setFont(fontBig);

    background->setX(backgroundX);
    background->setY(backgroundY);

    addUI(background);
    addUI(box1);
    addUI(box2);
    addUI(yesButton);
    addUI(noButton);
    addUI(quitLabel);
    addUI(yesButtonLabel);
    addUI(noButtonLabel);
}

void ExitConfirm::doYes()
{
    Game::getInstance()->setState(std::make_shared<MainMenu>());
}

void ExitConfirm::doNo()
{
    Game::getInstance()->popState();
}

void ExitConfirm::onKeyDown(Event::Keyboard* event)
{
    switch (event->keyCode())
    {
        case SDLK_ESCAPE:
        case SDLK_n:
            doNo();
            break;
        case SDLK_RETURN:
        case SDLK_y:
            doYes();
            break;
    }
}

void ExitConfirm::onStateActivate(Event::State* event)
{
    Game::getInstance()->mouse()->pushState(Input::Mouse::Cursor::BIG_ARROW);
}

void ExitConfirm::onStateDeactivate(Event::State* event)
{
    Game::getInstance()->mouse()->popState();
}

}
}
