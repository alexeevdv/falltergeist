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
#include "../Audio/AudioMixer.h"
#include "../Game/Game.h"
#include "../Graphics/ActiveUI.h"
#include "../Graphics/Renderer.h"
#include "../ResourceManager.h"
#include "../Input/Mouse.h"
#include "../Game/DudeObject.h"
#include "../State/GameMenu.h"
#include "../State/Inventory.h"
#include "../State/PipBoy.h"
#include "../State/PlayerEdit.h"
#include "../State/PlayerPanel.h"
#include "../State/Skilldex.h"
#include "../State/WorldMap.h"
#include "../UI/Image.h"
#include "../UI/ImageButton.h"
#include "../UI/SmallCounter.h"
#include "../State/SaveGame.h"
#include "../State/LoadGame.h"
#include "../State/ExitConfirm.h"

// Third party includes

namespace Falltergeist
{
namespace State
{

PlayerPanel::PlayerPanel() : State()
{
}

PlayerPanel::~PlayerPanel()
{
}

void PlayerPanel::init()
{
    if (_initialized) return;
    State::init();

    setFullscreen(false);
    setModal(false);

    auto game = Game::getInstance();

    auto iface = new Image("art/intrface/iface.frm");
    setX((game->renderer()->width() - 640)*0.5);
    setY(game->renderer()->height() - iface->height());
    auto background = addUI("background", iface);
    background->addEventHandler("mouseleftdown", [this](Event* event){ this->onPanelMouseDown(dynamic_cast<MouseEvent*>(event)); });
    background->addEventHandler("mousein", [this](Event* event){ this->onPanelMouseIn(dynamic_cast<MouseEvent*>(event)); });
    background->addEventHandler("mouseout", [this](Event* event){ this->onPanelMouseOut(dynamic_cast<MouseEvent*>(event)); });
  
    addUI("change_hand_button", new ImageButton(ImageButton::TYPE_BIG_RED_CIRCLE, 218, 5));
    getActiveUI("change_hand_button")->addEventHandler("mouseleftclick", [this](Event* event){ this->onChangeHandButtonClick(dynamic_cast<MouseEvent*>(event)); });

    addUI("inventory_button", new ImageButton(ImageButton::TYPE_PANEL_INVENTORY, 211, 40));
    getActiveUI("inventory_button")->addEventHandler("mouseleftclick", [this](Event* event){ this->onInventoryButtonClick(dynamic_cast<MouseEvent*>(event)); });

    addUI("options_button", new ImageButton(ImageButton::TYPE_PANEL_OPTIONS, 210, 61));
    getActiveUI("options_button")->addEventHandler("mouseleftclick", [this](Event* event){ this->onOptionsButtonClick(dynamic_cast<MouseEvent*>(event)); });

    addUI("attack_button", new ImageButton(ImageButton::TYPE_PANEL_ATTACK, 267, 25));

    addUI("hit_points", new SmallCounter(471, 40));
    getSmallCounter("hit_points")->setNumber(game->player()->hitPoints());
    getSmallCounter("hit_points")->setType(SmallCounter::SIGNED);

    addUI("armor_class", new SmallCounter(472, 76));
    getSmallCounter("armor_class")->setNumber(game->player()->armorClass());
    getSmallCounter("armor_class")->setType(SmallCounter::SIGNED);

    addUI("skilldex_button", new ImageButton(ImageButton::TYPE_BIG_RED_CIRCLE, 523, 5));
    getActiveUI("skilldex_button")->addEventHandler("mouseleftclick", [this](Event* event){ this->onSkilldexButtonClick(dynamic_cast<MouseEvent*>(event)); });

    addUI("map_button", new ImageButton(ImageButton::TYPE_PANEL_MAP, 526, 39));
    getActiveUI("map_button")->addEventHandler("mouseleftclick", [this](Event* event){ this->onMapButtonClick(dynamic_cast<MouseEvent*>(event)); });

    addUI("cha_button", new ImageButton(ImageButton::TYPE_PANEL_CHA, 526, 58));
    getActiveUI("cha_button")->addEventHandler("mouseleftclick", [this](Event* event){ this->onChaButtonClick(dynamic_cast<MouseEvent*>(event)); });

    addUI("pip_button", new ImageButton(ImageButton::TYPE_PANEL_PIP, 526, 77));
    getActiveUI("pip_button")->addEventHandler("mouseleftclick", [this](Event* event){ this->onPipBoyButtonClick(dynamic_cast<MouseEvent*>(event)); });
}

void PlayerPanel::render()
{
    State::render();

    // object in hand
    if (auto item = Game::getInstance()->player()->currentHandSlot())
    {
        auto itemUi = item->inventoryDragUi();
        itemUi->setX(x() + 360 - itemUi->width()*0.5);
        itemUi->setY(y() + 60 - itemUi->height()*0.5);
        itemUi->render();
    }
}

void PlayerPanel::think()
{
    State::think();
    // object in hand
    if (auto item = Game::getInstance()->player()->currentHandSlot())
    {
        auto itemUi = item->inventoryDragUi();
        itemUi->think();
    }
}

void PlayerPanel::handle(Event* event)
{
    State::handle(event);
    // object in hand
    if (auto item = Game::getInstance()->player()->currentHandSlot())
    {
        auto itemUi = item->inventoryDragUi();
        itemUi->handle(event);
        if (event->handled()) return;
    }
}

void PlayerPanel::onPanelMouseDown(MouseEvent* event)
{
    event->setHandled(true);
}

void PlayerPanel::onInventoryButtonClick(MouseEvent* event)
{
    openInventory();
}

void PlayerPanel::onOptionsButtonClick(MouseEvent* event)
{
    openOptions();
}

void PlayerPanel::onSkilldexButtonClick(MouseEvent* event)
{
    openSkilldex();
}

void PlayerPanel::onMapButtonClick(MouseEvent* event)
{
    openMap();
}

void PlayerPanel::onChaButtonClick(MouseEvent* event)
{
    openCharacterScreen();
}

void PlayerPanel::onPipBoyButtonClick(MouseEvent* event)
{
    openPopBoy();
}

void PlayerPanel::onChangeHandButtonClick(MouseEvent* event)
{
    toggleActiveHand();
}

void PlayerPanel::onPanelMouseIn(MouseEvent* event)
{
    Game::getInstance()->mouse()->pushState(Mouse::BIG_ARROW);
}

void PlayerPanel::onPanelMouseOut(MouseEvent* event)
{
    auto mouse = Game::getInstance()->mouse();
    if (mouse->scrollState())
    {
        // this trick is needed for correct cursor type returning on scrolling
        auto state = mouse->state();
        mouse->popState();
        mouse->popState();
        mouse->pushState(state);
    }
    else
    {
        mouse->popState();
    }
}

void PlayerPanel::onPanelMouseUp(MouseEvent* event)
{
}

void PlayerPanel::onKeyDown(KeyboardEvent* event)
{
    switch (event->keyCode())
    {
        case SDLK_a:
            // @TODO: initiateCombat();
            break;
        case SDLK_c:
            openCharacterScreen();
            break;
        case SDLK_i:
            openInventory();
            break;
        case SDLK_p:
            if (event->controlPressed()) 
            {
                // @TODO: pause game
            }
            else 
            {
                openPopBoy();
            }
            break;
        case SDLK_z:
            openPopBoy(); // @TODO: go to clock
            break;
        case SDLK_ESCAPE:
        case SDLK_o:
            openOptions();
            break;
        case SDLK_b:
            toggleActiveHand();
            break;
        // M button is handled in State::Location
        case SDLK_n:
            // @TODO: toggleItemMode();
            break;
        case SDLK_s:
            if (event->controlPressed())
            {
                doSaveGame();
            }
            else
            {
                openSkilldex();
            }                
            break;
        case SDLK_l:
            if (event->controlPressed()) 
            {
                doLoadGame();
            }
            break;
        case SDLK_x:
            if (event->controlPressed())
            {
                Game::getInstance()->pushState(new ExitConfirm());
            }
        case SDLK_SLASH:
            // @TODO: printCurrentTime();
            break;
        case SDLK_TAB:
            openMap();
            break;
        case SDLK_F1:
            // @TODO: help screen
            break;
        case SDLK_F2:
            // @TODO: volume down
            break;
        case SDLK_F3:
            // @TODO: volume up
            break;
        case SDLK_F4:
            if (!event->altPressed())
                doSaveGame();
            break;
        case SDLK_F5:
            doLoadGame();
            break;
        case SDLK_F6:
            doSaveGame(true);
            break;
        case SDLK_F7:
            doLoadGame(true);
            break;
        case SDLK_F10:
            Game::getInstance()->pushState(new ExitConfirm());
            break;
        case SDLK_F12:
            // @TODO: save screenshot
            break;
    }
}

unsigned int PlayerPanel::height()
{
    return getActiveUI("background")->height();
}

void PlayerPanel::openInventory()
{
    Game::getInstance()->pushState(new Inventory());
}

void PlayerPanel::openMap()
{
    Game::getInstance()->pushState(new WorldMap());
}

void PlayerPanel::openOptions()
{
    Game::getInstance()->pushState(new GameMenu());
}

void PlayerPanel::openPopBoy()
{
    Game::getInstance()->pushState(new PipBoy());
}

void PlayerPanel::openSkilldex()
{
    Game::getInstance()->pushState(new Skilldex());
}

void PlayerPanel::toggleActiveHand()
{
    auto player = Game::getInstance()->player();
    player->setCurrentHand(player->currentHand() == Game::GameCritterObject::HAND_LEFT ? Game::GameCritterObject::HAND_RIGHT : Game::GameCritterObject::HAND_LEFT);
}

void PlayerPanel::openCharacterScreen()
{
    Game::getInstance()->pushState(new PlayerEdit());
}

void PlayerPanel::doLoadGame(bool quick)
{
    Game::getInstance()->pushState(new LoadGame());
    // @TODO: quick load logic
}

void PlayerPanel::doSaveGame(bool quick)
{
    Game::getInstance()->pushState(new SaveGame());
    // @TODO: quick save logic
}



}
}
