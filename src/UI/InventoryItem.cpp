#include <memory>
#include "../Audio/Mixer.h"
#include "../Event/Event.h"
#include "../Event/Mouse.h"
#include "../Game/ArmorItemObject.h"
#include "../Game/DudeObject.h"
#include "../Game/Game.h"
#include "../Graphics/Rect.h"
#include "../Graphics/Renderer.h"
#include "../Input/Mouse.h"
#include "../State/CursorDropdown.h"
#include "../State/Inventory.h"
#include "../UI/InventoryItem.h"
#include "../UI/ItemsList.h"
#include "../UI/ResourceManager.h"

namespace Falltergeist
{
    namespace UI
    {
        using Graphics::Rect;

        InventoryItem::InventoryItem(Game::ItemObject *item, const Point& pos) : Falltergeist::UI::Base(pos)
        {
            resourceManager = std::make_shared<UI::ResourceManager>();
            _item = item;
            mouseDownHandler().add(std::bind(&InventoryItem::onMouseDown, this, std::placeholders::_1));
            mouseDragStartHandler().add(std::bind(&InventoryItem::onMouseDragStart, this, std::placeholders::_1));
            mouseDragHandler().add(std::bind(&InventoryItem::onMouseDrag, this, std::placeholders::_1));
            mouseDragStopHandler().add(std::bind(&InventoryItem::onMouseDragStop, this, std::placeholders::_1));
        }

        InventoryItem::~InventoryItem()
        {
        }

        InventoryItem::Type InventoryItem::type() const
        {
            return _type;
        }

        void InventoryItem::setType(Type value)
        {
            _type = value;
        }

        void InventoryItem::render(bool eggTransparency)
        {
            if (!_item) {
                return;
            }

            double widthRatio;
            double heightRatio;
            unsigned int newWidth;
            unsigned int newHeight;
            Size texSize;

            auto slotUi = _item->inventorySlotUi();
            auto dragUi = _item->inventoryDragUi();
            auto inventoryUi = _item->inventoryUi();

            switch (_type)
            {
                case Type::SLOT:
                    widthRatio = static_cast<double>(88) / static_cast<double>(slotUi->width());
                    heightRatio = static_cast<double>(58) / static_cast<double>(slotUi->height());

                    newWidth = static_cast<unsigned int>(static_cast<double>(slotUi->width()) * static_cast<double>(heightRatio));

                    if (newWidth <= 88) {
                        texSize = Size(newWidth, 58);
                        slotUi->setPosition(position() + (this->size() - texSize) / 2);
                        slotUi->render(Size(newWidth, 58), false);
                        return;
                    }
                    newHeight = static_cast<unsigned int>(static_cast<double>(slotUi->height()) * static_cast<double>(widthRatio));
                    texSize = Size(88, newHeight);
                    slotUi->setPosition(position() + (this->size() - texSize) / 2);
                    slotUi->render(Size(88, newHeight), false);
                    break;
                case Type::DRAG:
                    dragUi->setPosition(position());
                    dragUi->render();
                    break;
                default:
                    inventoryUi->setPosition(position());

                    widthRatio = static_cast<double>(57) / static_cast<double>(inventoryUi->width());
                    heightRatio = static_cast<double>(40) / static_cast<double>(inventoryUi->height());

                    newWidth = static_cast<unsigned int>(static_cast<double>(inventoryUi->width()) * static_cast<double>(heightRatio));

                    if (newWidth <= 57) {
                        texSize = Size(newWidth, 40);
                        inventoryUi->setPosition(position() + (this->size() - texSize) / 2);

                        inventoryUi->render(Size(newWidth, 40), false);
                        return;
                    }
                    newHeight = static_cast<unsigned int>(static_cast<double>(inventoryUi->height()) * static_cast<double>(widthRatio));
                    texSize = Size(57, newHeight);
                    inventoryUi->setPosition(position() + (this->size() - texSize) / 2);
                    inventoryUi->render(Size(57, newHeight), false);
                    break;
            }
        }

        Game::ItemObject* InventoryItem::item()
        {
            return _item;
        }

        void InventoryItem::setItem(Game::ItemObject* item)
        {
            _item = item;
        }

        void InventoryItem::onMouseDown(Event::Mouse* event)
        {
            if (event->leftButton()) {
                onMouseLeftDown(event);
            }

            if (event->rightButton()) {
                onMouseRightDown(event);
            }
        }

        void InventoryItem::onMouseLeftDown(Event::Mouse* event)
        {
            auto mouseState = Game::Game::getInstance()->mouse()->state();
            if (mouseState == Input::Mouse::Cursor::ACTION) {
                auto inventory = dynamic_cast<State::Inventory *>(Game::Game::getInstance()->topState(0));
                inventory->_item = _item;
                inventory->_actionCursorTimer.start();
                inventory->_actionCursorButtonPressed = true;
            }
        }

        void InventoryItem::onMouseRightDown(Event::Mouse* event)
        {
        }

        void InventoryItem::onMouseDragStart(Event::Mouse* event)
        {
            auto mouseState = Game::Game::getInstance()->mouse()->state();
            if (mouseState == Input::Mouse::Cursor::HAND) {
                Game::Game::getInstance()->mouse()->pushState(Input::Mouse::Cursor::NONE);
                Game::Game::getInstance()->mixer()->playACMSound("sound/sfx/ipickup1.acm");
                _oldType = type();
                setType(Type::DRAG);
                setOffset((event->position() - _position) - size() / 2);
            }
        }

        void InventoryItem::onMouseDrag(Event::Mouse* event)
        {
            if (type() == Type::DRAG) {
                setOffset(offset() + event->offset());
            }
        }

        void InventoryItem::onMouseDragStop(Event::Mouse* event)
        {
            if (type() == Type::DRAG) {
                Game::Game::getInstance()->mouse()->popState();
                Game::Game::getInstance()->mixer()->playACMSound("sound/sfx/iputdown.acm");
                setOffset({0, 0});
                setType(_oldType);

                auto itemevent = std::make_unique<Event::Mouse>(*event, "itemdragstop");
                itemevent->setPosition(event->position());
                itemevent->setTarget(this);
                emitEvent(std::move(itemevent), itemDragStopHandler());
            }
        }

        void InventoryItem::onArmorDragStop(Event::Mouse* event)
        {
            // Check if mouse is over this item
            if (!Rect::inRect(event->position(), position(), size())) {
                return;
            }

            if (ItemsList* itemsList = dynamic_cast<ItemsList*>(event->target())) {

                InventoryItem* draggedItem = itemsList->draggedItem();
                auto itemObject = draggedItem->item();
                if(itemObject->subtype() != Game::ItemObject::Subtype::ARMOR) return;
                itemsList->removeItem(draggedItem, 1);
                // place current armor back to inventory
                if (_item) {
                    itemsList->addItem(this, 1);
                }
                this->setItem(itemObject);
                if (auto armor = dynamic_cast<Game::ArmorItemObject*>(itemObject)) {
                    Game::Game::getInstance()->player()->setArmorSlot(armor);
                }
            }
        }

        void InventoryItem::onHandDragStop(Event::Mouse* event, HAND hand)
        {
            // Check if mouse is over this item
            if (!Rect::inRect(event->position(), position(), size()))
            {
                return;
            }

            if (ItemsList* itemsList = dynamic_cast<ItemsList*>(event->target()))
            {
                InventoryItem* itemUi = itemsList->draggedItem();
                auto item = itemUi->item();
                itemsList->removeItem(itemUi, 1);
                // place current weapon back to inventory
                if (_item) {
                    itemsList->addItem(this, 1);
                }
                this->setItem(item);
                auto player = Game::Game::getInstance()->player();
                if (hand == HAND::LEFT) {
                    player->setLeftHandSlot(item);
                } else {
                    player->setRightHandSlot(item);
                }
            }
        }

        Size InventoryItem::size() const
        {
            switch (_type)
            {
                case Type::INVENTORY:
                    return Size(70, 49);
                case Type::SLOT:
                    return Size(90, 63);
                default:
                    return Size(57, 40);
            }
        }

        Event::MouseHandler& InventoryItem::itemDragStopHandler()
        {
            return _itemDragStopHandler;
        }

        bool InventoryItem::opaque(const Point &pos)
        {
            if (!_item) {
                return false;
            }
            return Rect::inRect(pos, this->size());
        }
    }
}
