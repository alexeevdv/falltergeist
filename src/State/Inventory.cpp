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
#include "../functions.h"
#include "../Game/ArmorItemObject.h"
#include "../Game/CritterObject.h"
#include "../Game/DudeObject.h"
#include "../Game/Game.h"
#include "../Game/Object.h"
#include "../Game/ObjectFactory.h"
#include "../Game/WeaponItemObject.h"
#include "../Graphics/Renderer.h"
#include "../Input/Mouse.h"
#include "../ResourceManager.h"
#include "../State/State.h"
#include "../State/GameMenu.h"
#include "../State/InventoryDragItem.h"
#include "../State/Inventory.h"
#include "../State/Location.h"
#include "../UI/Image.h"
#include "../UI/ImageButton.h"
#include "../UI/ItemsList.h"
#include "../UI/InventoryItem.h"
#include "../UI/TextArea.h"
#include "../UI/ImageList.h"

// Third party includes

namespace Falltergeist
{
namespace State
{

Inventory::Inventory() : State()
{
    Game::getInstance()->mouse()->pushState(Input::Mouse::Cursor::ACTION);
}

Inventory::~Inventory()
{
    // If hand cursor now
    if (Game::getInstance()->mouse()->state() == Input::Mouse::Cursor::HAND)
    {
        Game::getInstance()->mouse()->popState();
    }
    Game::getInstance()->mouse()->popState();
}

void Inventory::init()
{
    if (_initialized) return;
    State::init();

    setModal(true);
    setFullscreen(false);

    auto game = Game::getInstance();
    auto panelHeight = Game::getInstance()->locationState()->playerPanelState()->height();

    setX((game->renderer()->width()  - 499)/2); // 499x377 = art/intrface/invbox.frm
    setY((game->renderer()->height() - 377 - panelHeight)/2);

    addUI("background", std::make_shared<UI::Image>("art/intrface/invbox.frm"));
    getUI("background")->addEventHandler("mouserightclick", [this](Event::Event* event){ this->backgroundRightClick(dynamic_cast<Event::Mouse*>(event)); });

    addUI("button_up",   std::make_shared<UI::ImageButton>(UI::ImageButton::Type::INVENTORY_UP_ARROW,   128, 40));
    addUI("button_down", std::make_shared<UI::ImageButton>(UI::ImageButton::Type::INVENTORY_DOWN_ARROW, 128, 65));
    addUI("button_done", std::make_shared<UI::ImageButton>(UI::ImageButton::Type::SMALL_RED_CIRCLE, 438, 328));

    getUI("button_done")->addEventHandler("mouseleftclick", [this](Event::Event* event){ this->onDoneButtonClick(dynamic_cast<Event::Mouse*>(event)); });
    getUI("button_up")->addEventHandler("mouseleftclick",   [this](Event::Event* event){ this->onScrollUpButtonClick(dynamic_cast<Event::Mouse*>(event)); });
    getUI("button_down")->addEventHandler("mouseleftclick", [this](Event::Event* event){ this->onScrollDownButtonClick(dynamic_cast<Event::Mouse*>(event)); });

    // screen
    auto screenX = 300;
    auto screenY = 47;

    auto player = Game::getInstance()->player();

    addUI("player_name", std::make_shared<UI::TextArea>(player->name(), screenX, screenY));

    auto line1 = std::make_shared<UI::Image>(142, 1);
    line1->setX(screenX);
    line1->setY(screenY+16);
    line1->texture()->fill(0x3ff800ff); // default green color

    std::string statsLabels;
    for (unsigned i = (unsigned)STAT::STRENGTH; i <= (unsigned)STAT::LUCK; i++)
    {
        statsLabels += _t(MSG_INVENTORY, i) + "\n";
    }
    addUI("label_stats", std::make_shared<UI::TextArea>(statsLabels, screenX, screenY + 10*2));

    std::string statsValues;
    for (unsigned i = (unsigned)STAT::STRENGTH; i <= (unsigned)STAT::LUCK; i++)
    {
        statsValues += std::to_string(player->stat((STAT)i)) + "\n";
    }
    addUI("label_stats_values", std::make_shared<UI::TextArea>(statsValues, screenX + 22, screenY + 20));

    std::stringstream ss;
    for (unsigned int i=7; i<14; i++)
    {
        ss << _t(MSG_INVENTORY, i) << "\n";
    }
    auto textLabel = std::make_shared<UI::TextArea>(ss.str(), screenX+40, screenY+20);

    // label: hit points
    ss.str("");
    ss << player->hitPoints();
    ss << "/";
    ss << player->hitPointsMax();
    auto hitPointsLabel = std::make_shared<UI::TextArea>(ss.str(), screenX+94, screenY+20);
    hitPointsLabel->setWidth(46);
    hitPointsLabel->setHorizontalAlign(UI::TextArea::HorizontalAlign::RIGHT);
    // label: armor class
    ss.str("");
    ss << player->armorClass();
    auto armorClassLabel = std::make_shared<UI::TextArea>(ss.str(), screenX+94, screenY+30);
    armorClassLabel->setWidth(46);
    armorClassLabel->setHorizontalAlign(UI::TextArea::HorizontalAlign::RIGHT);

    // armorSlot, leftHand, rightHand
    Game::ArmorItemObject* armorSlot = player->armorSlot();
    Game::ItemObject* leftHand = player->leftHandSlot();
    Game::ItemObject* rightHand = player->rightHandSlot();


    // label: damage treshold levels
    ss.str("");
    if (armorSlot)
    {
        ss << player->damageThreshold(DAMAGE::NORMAL) + armorSlot->damageThreshold(DAMAGE::NORMAL) <<"/\n";
        ss << player->damageThreshold(DAMAGE::LASER) + armorSlot->damageThreshold(DAMAGE::LASER) <<"/\n";
        ss << player->damageThreshold(DAMAGE::FIRE) + armorSlot->damageThreshold(DAMAGE::FIRE) <<"/\n";
        ss << player->damageThreshold(DAMAGE::PLASMA) + armorSlot->damageThreshold(DAMAGE::PLASMA) <<"/\n";
        ss << player->damageThreshold(DAMAGE::EXPLOSIVE) + armorSlot->damageThreshold(DAMAGE::NORMAL) <<"/";
    }
    else
    {
        ss << player->damageThreshold(DAMAGE::NORMAL) <<"/\n";
        ss << player->damageThreshold(DAMAGE::LASER) <<"/\n";
        ss << player->damageThreshold(DAMAGE::FIRE) <<"/\n";
        ss << player->damageThreshold(DAMAGE::PLASMA) <<"/\n";
        ss << player->damageThreshold(DAMAGE::EXPLOSIVE) <<"/";
    }
    auto damageThresholdLabel = std::make_shared<UI::TextArea>(ss.str(), screenX+94, screenY+40);
    damageThresholdLabel->setWidth(26);
    damageThresholdLabel->setHorizontalAlign(UI::TextArea::HorizontalAlign::RIGHT);

    // label: damage resistance levels
    ss.str("");
    if (armorSlot)
    {
        ss << player->damageResist(DAMAGE::NORMAL) + armorSlot->damageResist(DAMAGE::NORMAL) <<"%\n";
        ss << player->damageResist(DAMAGE::LASER) + armorSlot->damageResist(DAMAGE::LASER) <<"%\n";
        ss << player->damageResist(DAMAGE::FIRE) + armorSlot->damageResist(DAMAGE::FIRE) <<"%\n";
        ss << player->damageResist(DAMAGE::PLASMA) + armorSlot->damageResist(DAMAGE::PLASMA) <<"%\n";
        ss << player->damageResist(DAMAGE::EXPLOSIVE) + armorSlot->damageResist(DAMAGE::NORMAL) <<"%";
    }
    else
    {
        ss << player->damageResist(DAMAGE::NORMAL) <<"%\n";
        ss << player->damageResist(DAMAGE::LASER) <<"%\n";
        ss << player->damageResist(DAMAGE::FIRE) <<"%\n";
        ss << player->damageResist(DAMAGE::PLASMA) <<"%\n";
        ss << player->damageResist(DAMAGE::EXPLOSIVE) <<"%";
    }
    auto damageResistanceLabel = std::make_shared<UI::TextArea>(ss.str(), screenX+120, screenY+40);

    auto line2 = std::make_shared<UI::Image>(142, 1);
    line2->setX(screenX);
    line2->setY(screenY+94);
    line2->texture()->fill(0x3ff800ff); // default green color

    auto line3 = std::make_shared<UI::Image>(142, 1);
    line3->setX(screenX);
    line3->setY(screenY+134);
    line3->texture()->fill(0x3ff800ff); // default green color

    // label: Total Wt: (20)
    auto weight = player->carryWeight();
    auto weightMax = player->carryWeightMax();

    ss.str("");
    ss << weight;
    auto totalWtLabel = std::make_shared<UI::TextArea>(_t(MSG_INVENTORY, 20), screenX+14, screenY+180);
    auto weightLabel = std::make_shared<UI::TextArea>(ss.str(), screenX+70, screenY+180);
    weightLabel->setWidth(24);
    weightLabel->setHorizontalAlign(UI::TextArea::HorizontalAlign::RIGHT);
    ss.str("");
    ss << "/" << weightMax;
    auto weightMaxLabel = std::make_shared<UI::TextArea>(ss.str(), screenX+94, screenY+180);
    if (weight>weightMax)
    {
        weightLabel->setFont(ResourceManager::getInstance()->font("font1.aaf", 0xff0000ff));
    }

    // label: left hand
    ss.str("");
    if (leftHand)
    {
        ss << _handItemSummary(leftHand);
    }
    auto leftHandLabel = std::make_shared<UI::TextArea>(ss.str(), screenX, screenY+100);

    // label: right hand
    ss.str("");
    if (rightHand)
    {
        ss << _handItemSummary(rightHand);
    }
    auto rightHandLabel = std::make_shared<UI::TextArea>(ss.str(), screenX, screenY+140);

    // screen info
    auto screenLabel = std::make_shared<UI::TextArea>("", screenX, screenY+20);
    screenLabel->setWidth(140); //screen size
    screenLabel->setHeight(168);
    screenLabel->setVisible(false);
    screenLabel->setWordWrap(true);


    addUI(line1);
    addUI("textLabel", textLabel);
    addUI("hitPointsLabel", hitPointsLabel);
    addUI("armorClassLabel", armorClassLabel);
    addUI("damageThresholdLabel", damageThresholdLabel);
    addUI("damageResistanceLabel", damageResistanceLabel);
    addUI("line2", line2);
    addUI("line3", line3);
    addUI("totalWtLabel", totalWtLabel);
    addUI("weightLabel", weightLabel);
    addUI("weightMaxLabel", weightMaxLabel);
    addUI("leftHandLabel", leftHandLabel);
    addUI("rightHandLabel", rightHandLabel);
    addUI("screenLabel", screenLabel);

    auto inventoryList = std::make_shared<UI::ItemsList>(40, 40);
    inventoryList->setItems(game->player()->inventory());
    addUI("inventory_list", inventoryList);

    // BIG ICONS
    // icon: armor
    {
        auto inventoryItem = std::make_shared<UI::InventoryItem>(armorSlot, 154, 183);
        inventoryItem->setType(UI::InventoryItem::Type::SLOT);
        inventoryItem->addEventHandler("itemdragstop", [inventoryList](Event::Event* event){ inventoryList->onItemDragStop(dynamic_cast<Event::Mouse*>(event)); });
        inventoryList->addEventHandler("itemdragstop", [inventoryItem](Event::Event* event){ inventoryItem->onArmorDragStop(dynamic_cast<Event::Mouse*>(event)); });
        addUI(inventoryItem);
    }

    // icon: left hand
    {
        auto inventoryItem = std::make_shared<UI::InventoryItem>(leftHand, 154, 286);
        inventoryItem->setType(UI::InventoryItem::Type::SLOT);
        inventoryItem->addEventHandler("itemdragstop", [inventoryList](Event::Event* event){ inventoryList->onItemDragStop(dynamic_cast<Event::Mouse*>(event)); });
        inventoryList->addEventHandler("itemdragstop", [inventoryItem](Event::Event* event){ inventoryItem->onHandDragStop(dynamic_cast<Event::Mouse*>(event)); });
        addUI(inventoryItem);
    }

    // icon: right hand
    {
        auto inventoryItem = std::make_shared<UI::InventoryItem>(rightHand, 247, 286);
        inventoryItem->setType(UI::InventoryItem::Type::SLOT);
        inventoryItem->addEventHandler("itemdragstop", [inventoryList](Event::Event* event){ inventoryList->onItemDragStop(dynamic_cast<Event::Mouse*>(event)); });
        inventoryList->addEventHandler("itemdragstop", [inventoryItem](Event::Event* event){ inventoryItem->onHandDragStop(dynamic_cast<Event::Mouse*>(event)); });
        addUI(inventoryItem);
    }

}

void Inventory::onDoneButtonClick(Event::Mouse* event)
{
    Game::getInstance()->popState();
}

void Inventory::onScrollUpButtonClick(Event::Mouse* event)
{
    auto inventory = dynamic_cast<UI::ItemsList*>(getUI("inventory_list"));
    if(inventory->canScrollUp())
    {
        inventory->scrollUp();
    }
}

void Inventory::onScrollDownButtonClick(Event::Mouse* event)
{
    auto inventory = dynamic_cast<UI::ItemsList*>(getUI("inventory_list"));
    if(inventory->canScrollDown())
    {
        inventory->scrollDown();
    }
}

void Inventory::onArmorSlotMouseDown(Event::Mouse* event)
{
    if (Game::getInstance()->mouse()->state() == Input::Mouse::Cursor::HAND)
    {
        auto itemUi = dynamic_cast<UI::ImageList*>(event->eventTarget());
        Game::getInstance()->pushState(std::make_shared<InventoryDragItem>(itemUi));
    }
    else
    {
        auto itemPID = Game::getInstance()->player()->armorSlot()->PID();
        _screenShow(itemPID);
    }
}

void Inventory::onLeftHandSlotMouseDown(Event::Mouse* event)
{
    if (Game::getInstance()->mouse()->state() == Input::Mouse::Cursor::HAND)
    {
        auto itemUi = dynamic_cast<UI::ImageList*>(event->eventTarget());
        Game::getInstance()->pushState(std::make_shared<InventoryDragItem>(itemUi));
    }
    else
    {
        auto itemPID = Game::getInstance()->player()->leftHandSlot()->PID();
        _screenShow(itemPID);
    }
}

void Inventory::onRightHandSlotMouseDown(Event::Mouse* event)
{
    if (Game::getInstance()->mouse()->state() == Input::Mouse::Cursor::HAND)
    {
        auto itemUi = dynamic_cast<UI::ImageList*>(event->eventTarget());
        Game::getInstance()->pushState(std::make_shared<InventoryDragItem>(itemUi));
    }
    else
    {
        auto itemPID = Game::getInstance()->player()->rightHandSlot()->PID();
        _screenShow(itemPID);
    }
}

//void Inventory::onSlotMouseDown(MouseEvent* event)
//{
//    auto state = dynamic_cast<Inventory*>(event->reciever());
//    auto itemUi = dynamic_cast<ImageList*>(event->eventTarget());
//    itemUi->setCurrentImage(1);
//    itemUi->setX(event->x() - itemUi->width()*0.5);
//    itemUi->setY(event->y() - itemUi->height()*0.5);
//}

//void Inventory::onSlotMouseUp(MouseEvent* event)
//{
//    auto itemUi = dynamic_cast<ImageList*>(event->eventTarget());
//    itemUi->setCurrentImage(0);
//    itemUi->setX(event->x() - itemUi->width()*0.5);
//    itemUi->setY(event->y() - itemUi->height()*0.5);
//}

//void Inventory::onSlotDrag(MouseEvent* event)
//{
//    //auto item = dynamic_cast<GameItemObject*>(event->reciever());
//    auto itemUi = dynamic_cast<ImageList*>(event->eventTarget());
//    //auto dragUi = item->inventoryDragUi();
//    itemUi->setX(itemUi->x() + event->xOffset());
//    itemUi->setY(itemUi->y() + event->yOffset());
//    //Game::getInstance()->states()->back()->ui()->push_back(dragUi);
//}

std::string Inventory::_handItemSummary (Game::ItemObject* hand)
{
    std::stringstream ss;
    if (hand)
    {
        ss << hand->name() << "\n";
        // is it weapon
        if (hand->subtype() == Game::ItemObject::Subtype::WEAPON)
        {
            auto weapon = dynamic_cast<Game::WeaponItemObject*>(hand);
            ss << "Dmg: " << weapon->damageMin() << "-" << weapon->damageMax() << " ";
            ss << "Rng: " << weapon->rangePrimary();
            // has it ammo?
            if (weapon->ammoType() != 0)
            {
                ss << "\nAmmo: /" << weapon->ammoCapacity() << " ";
                auto ammo = Game::ObjectFactory::getInstance()->createObject(weapon->ammoPID());
                ss << ammo->name();
            }
        }
    }
    else
    {
        ss << "No item\nUnarmed dmg:";
    }
    return ss.str();
}

void Inventory::backgroundRightClick(Event::Mouse* event)
{
    auto mouse = Game::getInstance()->mouse();
    if (mouse->state() == Input::Mouse::Cursor::ACTION)
    {
        mouse->pushState(Input::Mouse::Cursor::HAND);
    }
    else
    {
        mouse->popState();
        //state->_screenShow(1);
    }
    _screenShow(0);
}

void Inventory::_screenShow (unsigned int PID)
{
    auto player = Game::getInstance()->player();
    auto playerNameLabel = getTextArea("player_name");
    auto statsLabel = getTextArea("label_stats");
    auto statsValuesLabel = getTextArea("label_stats_values");
    auto textLabel = getTextArea("textLabel");
    auto hitPointsLabel = getTextArea("hitPointsLabel");
    auto armorClassLabel = getTextArea("armorClassLabel");
    auto damageThresholdLabel = getTextArea("damageThresholdLabel");
    auto damageResistanceLabel = getTextArea("damageResistanceLabel");
    auto line2 = dynamic_cast<UI::Image*>(getUI("line2"));
    auto line3 = dynamic_cast<UI::Image*>(getUI("line3"));
    auto totalWtLabel = getTextArea("totalWtLabel");
    auto weightLabel = getTextArea("weightLabel");
    auto weightMaxLabel = getTextArea("weightMaxLabel");
    auto leftHandLabel = getTextArea("leftHandLabel");
    auto rightHandLabel = getTextArea("rightHandLabel");
    auto screenLabel = getTextArea("screenLabel");

    if (PID == 0)
    {
        playerNameLabel->setText(player->name());
    }
    else
    {
        playerNameLabel->setText(_t(MSG_PROTO_ITEMS, PID*100)); // item name
        screenLabel->setText(_t(MSG_PROTO_ITEMS, PID*100 + 1)); // item dedcription
    }

    screenLabel->setVisible(PID != 0);
    statsLabel->setVisible(PID == 0);
    statsValuesLabel->setVisible(PID == 0);
    textLabel->setVisible(PID == 0);
    hitPointsLabel->setVisible(PID == 0);
    armorClassLabel->setVisible(PID == 0);
    damageThresholdLabel->setVisible(PID == 0);
    damageResistanceLabel->setVisible(PID == 0);
    line2->setVisible(PID == 0);
    line3->setVisible(PID == 0);
    totalWtLabel->setVisible(PID == 0);
    weightLabel->setVisible(PID == 0);
    weightMaxLabel->setVisible(PID == 0);
    leftHandLabel->setVisible(PID == 0);
    rightHandLabel->setVisible(PID == 0);
}

void Inventory::onKeyDown(Event::Keyboard* event)
{
    switch (event->keyCode())
    {
        case SDLK_ESCAPE:
            Game::getInstance()->popState();
            break;
    }
}


}
}
