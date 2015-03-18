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
#include "../Game/Game.h"
#include "../Game/DudeObject.h"
#include "../Game/ItemObject.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/Texture.h"
#include "../Logger.h"
#include "../UI/InventoryItem.h"
#include "../UI/ItemsList.h"
#include "../UI/Image.h"

// Third party includes

namespace Falltergeist
{

InventoryItem::InventoryItem(Game::GameItemObject *item, int x, int y) : ActiveUI(x, y)
{
    _item = item;
    addEventHandler("mouseleftdown", [this](Event* event){ this->onMouseLeftDown(dynamic_cast<MouseEvent*>(event)); });
    addEventHandler("mousedragstart", [this](Event* event){ this->onMouseDragStart(dynamic_cast<MouseEvent*>(event)); });
    addEventHandler("mousedrag", [this](Event* event){ this->onMouseDrag(dynamic_cast<MouseEvent*>(event)); });
    addEventHandler("mousedragstop", [this](Event* event){ this->onMouseDragStop(dynamic_cast<MouseEvent*>(event)); });
}

unsigned int InventoryItem::type()
{
    return _type;
}

void InventoryItem::setType(unsigned int value)
{
    _type = value;
}

Texture* InventoryItem::texture()
{
    //if (!_texture)
    //{
    //    _texture = new Texture(this->width(), this->height());
    //    _texture->fill(0x000000ff);
    //}
    //return _texture;

    if (!_item) return 0;

    switch (_type)
    {
        case TYPE_SLOT:
            return _item->inventorySlotUi()->texture();
        case TYPE_DRAG:
            return _item->inventoryDragUi()->texture();
    }

    return _item->inventoryUi()->texture();
}

void InventoryItem::render()
{
    //return ActiveUI::render();
    if (!_item) return;
    auto game = Game::getInstance();
    game->renderer()->drawTexture(texture(), x() + (width() - texture()->width())/2, y() + (height() - texture()->height())/2);
}

unsigned int InventoryItem::pixel(unsigned int x, unsigned int y)
{
    if (!_item) return 0;
    return x < width() && y < height();
}

Game::GameItemObject* InventoryItem::item()
{
    return _item;
}

void InventoryItem::setItem(Game::GameItemObject* item)
{
    _item = item;
}

void InventoryItem::onMouseLeftDown(MouseEvent* event)
{
}

void InventoryItem::onMouseDragStart(MouseEvent* event)
{
    _oldType = type();
    setType(TYPE_DRAG);
}

void InventoryItem::onMouseDrag(MouseEvent* event)
{
    setXOffset(xOffset() + event->xOffset());
    setYOffset(yOffset() + event->yOffset());
}

void InventoryItem::onMouseDragStop(MouseEvent* event)
{
    setXOffset(0);
    setYOffset(0);
    setType(_oldType);

    auto itemevent = new MouseEvent("itemdragstop");
    itemevent->setX(event->x());
    itemevent->setY(event->y());
    itemevent->setEmitter(this);
    emitEvent(itemevent);
    delete itemevent;

}

void InventoryItem::onArmorDragStop(MouseEvent* event)
{
    // Check if mouse is over this item
    if (event->x() <= x() || event->x() >= x() + width()) return;
    if (event->y() <= y() || event->y() >= y() + height()) return;

    if (ItemsList* itemsList = dynamic_cast<ItemsList*>(event->emitter()))
    {
        InventoryItem* draggedItem = itemsList->draggedItem();
        auto itemObject = draggedItem->item();
        itemsList->removeItem(draggedItem, 1);
        // place current armor back to inventory
        if (_item)
        {
            itemsList->addItem(this, 1);
        }
        this->setItem(itemObject);
        if (auto armor = dynamic_cast<Game::GameArmorItemObject*>(itemObject))
        {
            Game::getInstance()->player()->setArmorSlot(armor);
        }
    }
}

void InventoryItem::onHandDragStop(MouseEvent* event)
{
    // Check if mouse is over this item
    if (event->x() <= x() || event->x() >= x() + width()) return;
    if (event->y() <= y() || event->y() >= y() + height()) return;

    if (ItemsList* itemsList = dynamic_cast<ItemsList*>(event->emitter()))
    {
        InventoryItem* item = itemsList->draggedItem();
        itemsList->removeItem(item, 1);
        // place current weapon back to inventory
        if (_item)
        {
            itemsList->addItem(this, 1);
        }
        this->setItem(item->item());
    }
}

unsigned int InventoryItem::width()
{
    return type() == TYPE_SLOT ? 90 : 70;
}

unsigned int InventoryItem::height()
{
    return type() == TYPE_SLOT ? 63 : 49;
}

}
