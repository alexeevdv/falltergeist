#pragma once

#include <vector>
#include "../Game/ItemObject.h"

namespace Falltergeist
{
    namespace Game
    {
        /**
         * Items able to hold other items in their inventory.
         */
        class ContainerItemObject : public ItemObject
        {
            public:
                ContainerItemObject();
                ~ContainerItemObject() = default;

                std::vector<ItemObject*>* inventory();

                void use_p_proc(CritterObject* usedBy) override;

            protected:
                std::vector<ItemObject*> _inventory;
        };
    }
}
