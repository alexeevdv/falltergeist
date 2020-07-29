#include <memory>
#include "../Game/ItemObject.h"
#include "../Graphics/ObjectUIFactory.h"
#include "../ResourceManager.h"
#include "../UI/TextArea.h"
#include "../VM/Script.h"

namespace Falltergeist
{
    namespace Game
    {
        using namespace Base;

        ItemObject::ItemObject() : Object()
        {
            _type = Type::ITEM;
        }

        unsigned int ItemObject::amount() const
        {
            return _amount;
        }

        void ItemObject::setAmount(unsigned int value)
        {
            _amount = value;
            _inventoryAmountUi->setText("x" + std::to_string(value));
        }

        unsigned int ItemObject::weight() const
        {
            return _weight;
        }

        void ItemObject::setWeight(unsigned int value)
        {
            _weight = value;
        }

        unsigned int ItemObject::price() const
        {
            return _price;
        }

        void ItemObject::setPrice(unsigned price)
        {
            _price = price;
        }

        int ItemObject::inventoryFID() const
        {
            return _inventoryFID;
        }

        void ItemObject::setInventoryFID(int value)
        {
            _inventoryFID = value;
        }

        UI::Base* ItemObject::inventoryDragUi() const
        {
            return _inventoryDragUi.get();
        }

        std::unique_ptr<UI::TextArea>& ItemObject::inventoryAmountUi()
        {
            return _inventoryAmountUi;
        }

        void ItemObject::setVolume(unsigned int volume)
        {
            _volume = volume;
        }

        unsigned int ItemObject::volume() const
        {
            return _volume;
        }

        UI::Base* ItemObject::inventoryUi() const
        {
            return _inventoryUi.get();
        }

        UI::Base* ItemObject::inventorySlotUi() const
        {
            return _inventorySlotUi.get();
        }

        void ItemObject::_generateUi()
        {
            Object::_generateUi();

            if (inventoryFID() == -1) {
                return;
            }

            Graphics::ObjectUIFactory uiFactory;

            // Big unscaled image of item
            _inventoryDragUi = uiFactory.buildByFID(inventoryFID());
            _inventoryUi = uiFactory.buildByFID(inventoryFID());
            _inventorySlotUi = uiFactory.buildByFID(inventoryFID());
            _inventoryAmountUi = std::make_unique<UI::TextArea>("x" + std::to_string(_amount), inventorySlotUi()->position());
            _inventoryAmountUi->setColor({ 255, 255, 255, 0 });
        }

        ItemObject::Subtype ItemObject::subtype() const
        {
            return _subtype;
        }

        void ItemObject::setSoundId(char soundId)
        {
            this->_soundId = soundId;
        }

        char ItemObject::soundId() const
        {
            return _soundId;
        }
    }
}
