#pragma once

#include "../Game/Object.h"
#include "../UI/Image.h"

namespace Falltergeist
{
    namespace Game
    {
        /**
         * Item. Can be placed inside other object inventories (critters and containers)
         */
        class ItemObject : public Object
        {
            public:
                enum class Subtype
                {
                    ARMOR = 0,
                    CONTAINER,
                    DRUG,
                    WEAPON,
                    AMMO,
                    MISC,
                    KEY
                };

                ItemObject();
                ~ItemObject() = default;

                Subtype subtype() const;

                // item stack size in inventory or on the ground
                unsigned int amount() const;
                void setAmount(unsigned int value);

                // item weight, in pounds
                unsigned int weight() const;
                void setWeight(unsigned int value);

                // item price without any modifiers
                unsigned int price() const;
                void setPrice(unsigned price);

                // item volume
                unsigned int volume() const;
                void setVolume(unsigned int volume);

                // item inventory frame ID
                int inventoryFID() const;
                void setInventoryFID(int value);

                // item sound ID
                char soundId() const;
                void setSoundId(char soundId);

                UI::Base* inventoryUi() const;
                UI::Base* inventorySlotUi() const;
                UI::Base* inventoryDragUi() const;
                std::unique_ptr<UI::TextArea>& inventoryAmountUi();

            protected:
                char _soundId = 0;
                Subtype _subtype;
                unsigned int _amount = 1;
                unsigned int _weight = 0;
                unsigned int _price = 0;
                unsigned int _volume = 0;
                int _inventoryFID = -1;
                std::unique_ptr<UI::TextArea> _inventoryAmountUi;
                std::unique_ptr<UI::Base> _inventoryUi, _inventorySlotUi, _inventoryDragUi;
                void _generateUi() override;
        };
    }
}
