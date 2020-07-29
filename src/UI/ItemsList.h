#pragma once

#include <vector>
#include "../UI/Base.h"
#include "../UI/InventoryItem.h"

namespace Falltergeist
{
    namespace Event
    {
        class Mouse;
    }
    namespace Game
    {
        class ItemObject;
    }
    namespace UI
    {
        class ItemsList : public Falltergeist::UI::Base
        {
            public:
                ItemsList(const Point& pos);

                void setItems(std::vector<Game::ItemObject*>* items);
                std::vector<Game::ItemObject*>* items();

                std::vector<std::unique_ptr<InventoryItem>>& inventoryItems();
                InventoryItem* draggedItem();

                void update();

                void scrollUp();
                void scrollDown();

                bool canScrollUp();
                bool canScrollDown();

                unsigned int slotsNumber() const;
                void setSlotsNumber(unsigned int value);

                unsigned int slotOffset() const;
                // void setSlotOffset(unsigned int value);

                virtual void render(bool eggTransparency = false) override;

                void onMouseDown(Event::Mouse* event);
                void onMouseLeftDown(Event::Mouse* event);
                void onMouseRightDown(Event::Mouse* event);
                void onMouseDragStart(Event::Mouse* event);
                void onMouseDrag(Event::Mouse* event);
                void onMouseDragStop(Event::Mouse* event);
                void onItemDragStop(Event::Mouse* event, HAND hand);
                void onItemDragStop(Event::Mouse* event);

                void addItem(InventoryItem* item, unsigned int ammount);
                void removeItem(InventoryItem* item, unsigned int ammount);

                Event::MouseHandler& itemDragStopHandler();
                Event::Handler& itemsListModifiedHandler();


                virtual bool opaque(const Point &pos) override;

            private:
                std::vector<Game::ItemObject*>* _items = nullptr;
                InventoryItem* _draggedItem = nullptr;
                std::vector<std::unique_ptr<InventoryItem>> _inventoryItems;
                InventoryItem::Type _type = InventoryItem::Type::INVENTORY;
                unsigned int _slotsNumber = 7; // Height of widget in items
                unsigned int _slotOffset = 0;
                unsigned int _slotWidth = 77;
                unsigned int _slotHeight = 40;

                Event::MouseHandler _itemDragStopHandler;
                Event::Handler _itemsListModifiedHandler;
        };
    }
}
