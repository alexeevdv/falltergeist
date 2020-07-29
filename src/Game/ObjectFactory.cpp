#include "../Game/ObjectFactory.h"
#include "../Format/Int/File.h"
#include "../Format/Msg/File.h"
#include "../Format/Pro/File.h"
#include "../Game/AmmoItemObject.h"
#include "../Game/ArmorItemObject.h"
#include "../Game/ContainerItemObject.h"
#include "../Game/CritterObject.h"
#include "../Game/DoorSceneryObject.h"
#include "../Game/DrugItemObject.h"
#include "../Game/ElevatorSceneryObject.h"
#include "../Game/ExitMiscObject.h"
#include "../Game/GenericSceneryObject.h"
#include "../Game/KeyItemObject.h"
#include "../Game/LadderSceneryObject.h"
#include "../Game/MiscItemObject.h"
#include "../Game/StairsSceneryObject.h"
#include "../Game/WallObject.h"
#include "../Game/WeaponItemObject.h"
#include "../Exception.h"
#include "../ResourceManager.h"
#include "../VM/Script.h"

namespace Falltergeist
{
    namespace Game
    {
        ObjectFactory::ObjectFactory(std::shared_ptr<ILogger> logger)
        {
            this->logger = std::move(logger);
        }

        Object* ObjectFactory::createObjectByPID(unsigned int PID)
        {
            auto proto = ResourceManager::getInstance()->proFileType(PID);
            Object* object = 0;
            switch ((OBJECT_TYPE)proto->typeId())
            {
                case OBJECT_TYPE::ITEM:
                {
                    switch((ITEM_TYPE)proto->subtypeId())
                    {
                        case ITEM_TYPE::AMMO:
                        {
                            object = new AmmoItemObject();
                            break;
                        }
                        case ITEM_TYPE::ARMOR:
                        {
                            object = new ArmorItemObject();
                            for (unsigned i = (unsigned)DAMAGE::NORMAL; i != (unsigned)DAMAGE::POISON; ++i)
                            {
                                ((ArmorItemObject*)object)->setDamageResist((DAMAGE)i, proto->damageResist()->at(i));
                                ((ArmorItemObject*)object)->setDamageThreshold((DAMAGE)i, proto->damageThreshold()->at(i));
                            }
                            ((ArmorItemObject*)object)->setPerk(proto->perk());
                            ((ArmorItemObject*)object)->setMaleFID(proto->armorMaleFID());
                            ((ArmorItemObject*)object)->setFemaleFID(proto->armorFemaleFID());
                            ((ArmorItemObject*)object)->setArmorClass(proto->armorClass());
                            break;
                        }
                        case ITEM_TYPE::CONTAINER:
                        {
                            object = new ContainerItemObject();
                            break;
                        }
                        case ITEM_TYPE::DRUG:
                        {
                            object = new DrugItemObject();
                            break;
                        }
                        case ITEM_TYPE::KEY:
                        {
                            object = new KeyItemObject();
                            break;
                        }
                        case ITEM_TYPE::MISC:
                        {
                            object = new MiscItemObject();
                            break;
                        }
                        case ITEM_TYPE::WEAPON:
                        {
                            object = new WeaponItemObject();

                            ((WeaponItemObject*)object)->setPerk(proto->perk());
                            ((WeaponItemObject*)object)->setAnimationCode(proto->weaponAnimationCode());
                            ((WeaponItemObject*)object)->setDamageMin(proto->weaponDamageMin());
                            ((WeaponItemObject*)object)->setDamageMax(proto->weaponDamageMax());
                            ((WeaponItemObject*)object)->setDamageType(proto->weaponDamageType());
                            ((WeaponItemObject*)object)->setRangePrimary(proto->weaponRangePrimary());
                            ((WeaponItemObject*)object)->setRangeSecondary(proto->weaponRangeSecondary());
                            ((WeaponItemObject*)object)->setMinimumStrength(proto->weaponMinimumStrenght());
                            ((WeaponItemObject*)object)->setActionCostPrimary(proto->weaponActionCostPrimary());
                            ((WeaponItemObject*)object)->setActionCostSecondary(proto->weaponActionCostSecondary());
                            ((WeaponItemObject*)object)->setBurstRounds(proto->weaponBurstRounds());
                            ((WeaponItemObject*)object)->setAmmoType(proto->weaponAmmoType());
                            ((WeaponItemObject*)object)->setAmmoPID(proto->weaponAmmoPID());
                            ((WeaponItemObject*)object)->setAmmoCapacity(proto->weaponAmmoCapacity());
                            break;
                        }
                    }
                    ((ItemObject*)object)->setWeight(proto->weight());
                    // @TODO: ((GameItemObject*)object)->setVolume(proto->containerSize());
                    ((ItemObject*)object)->setInventoryFID(proto->inventoryFID());
                    ((ItemObject*)object)->setPrice(proto->basePrice());
                    ((ItemObject*)object)->setSoundId((char)proto->soundId());
                    auto msg = ResourceManager::getInstance()->msgFileType("text/english/game/pro_item.msg");
                    try
                    {
                        object->setName(msg->message(proto->messageId())->text());
                        object->setDescription(msg->message(proto->messageId() + 1)->text());
                    }
                    catch (const Exception&) {}
                    break;
                }
                case OBJECT_TYPE::CRITTER:
                {
                    object = new CritterObject();
                    auto msg = ResourceManager::getInstance()->msgFileType("text/english/game/pro_crit.msg");
                    try
                    {
                        object->setName(msg->message(proto->messageId())->text());
                        object->setDescription(msg->message(proto->messageId() + 1)->text());
                    }
                    catch (const Exception&) {}

                    for (unsigned i = (unsigned)STAT::STRENGTH; i <= (unsigned)STAT::LUCK; i++)
                    {
                        ((CritterObject*)object)->setStat((STAT)i, proto->critterStats()->at(i));
                        ((CritterObject*)object)->setStatBonus((STAT)i, proto->critterStatsBonus()->at(i));
                    }
                    for (unsigned i = (unsigned)SKILL::SMALL_GUNS; i <= (unsigned)SKILL::OUTDOORSMAN; i++)
                    {
                        ((CritterObject*)object)->setSkillTagged((SKILL)i, proto->critterSkills()->at(i));
                    }
                    ((CritterObject*)object)->setActionPoints(proto->critterActionPoints());
                    ((CritterObject*)object)->setActionPointsMax(proto->critterActionPoints());
                    ((CritterObject*)object)->setCritterFlags(proto->critterFlags());
                    ((CritterObject*)object)->setHitPointsMax(proto->critterHitPointsMax());
                    ((CritterObject*)object)->setArmorClass(proto->critterArmorClass());
                    ((CritterObject*)object)->setCarryWeightMax(proto->critterCarryWeightMax());
                    ((CritterObject*)object)->setMeleeDamage(proto->critterMeleeDamage());
                    ((CritterObject*)object)->setSequence(proto->critterSequence());
                    ((CritterObject*)object)->setCriticalChance(proto->critterCriticalChance());
                    ((CritterObject*)object)->setHealingRate(proto->critterHealingRate());
                    for (unsigned i = (unsigned)DAMAGE::NORMAL; i <= (unsigned)DAMAGE::POISON; i++)
                    {
                        ((CritterObject*)object)->setDamageResist((DAMAGE)i, proto->damageResist()->at(i));
                        ((CritterObject*)object)->setDamageThreshold((DAMAGE)i, proto->damageThreshold()->at(i));
                    }
                    ((CritterObject*)object)->setGender(proto->critterGender() ? GENDER::FEMALE : GENDER::MALE);
                    ((CritterObject*)object)->setAge(proto->critterAge());
                    break;
                }
                case OBJECT_TYPE::SCENERY:
                {
                    switch ((SCENERY_TYPE)proto->subtypeId())
                    {
                        case SCENERY_TYPE::DOOR:
                        {
                            object = new DoorSceneryObject(logger);
                            break;
                        }
                        case SCENERY_TYPE::ELEVATOR:
                        {
                            object = new ElevatorSceneryObject();
                            break;
                        }
                        case SCENERY_TYPE::GENERIC:
                        {
                            object = new GenericSceneryObject();
                            break;
                        }
                        case SCENERY_TYPE::LADDER_TOP:
                        case SCENERY_TYPE::LADDER_BOTTOM:
                        {
                            object = new LadderSceneryObject();
                            break;
                        }
                        case SCENERY_TYPE::STAIRS:
                        {
                            object = new StairsSceneryObject();
                            break;
                        }
                    }
                    auto msg = ResourceManager::getInstance()->msgFileType("text/english/game/pro_scen.msg");
                    try
                    {
                        object->setName(msg->message(proto->messageId())->text());
                        object->setDescription(msg->message(proto->messageId() + 1)->text());
                    }
                    catch (const Exception&) {}

                    ((SceneryObject*)object)->setSoundId((char)proto->soundId());

                    //first two bytes are orientation. second two - unknown
                    unsigned short orientation = proto->flagsExt() >> 16;
                    switch (orientation)
                    {
                        case 0x0000:
                            object->setLightOrientation(Orientation::NS);
                            break;
                        case 0x0800:
                            object->setLightOrientation(Orientation::EW);
                            break;
                        case 0x1000:
                            object->setLightOrientation(Orientation::NC);
                            break;
                        case 0x2000:
                            object->setLightOrientation(Orientation::SC);
                            break;
                        case 0x4000:
                            object->setLightOrientation(Orientation::EC);
                            break;
                        case 0x8000:
                            object->setLightOrientation(Orientation::WC);
                            break;
                    }
                    break;
                }
                case OBJECT_TYPE::WALL:
                {
                    object = new WallObject();
                    auto msg = ResourceManager::getInstance()->msgFileType("text/english/game/pro_wall.msg");
                    try
                    {
                        object->setName(msg->message(proto->messageId())->text());
                        object->setDescription(msg->message(proto->messageId() + 1)->text());
                    }
                    catch (const Exception&) {}

                    //first two bytes are orientation. second two - unknown
                    unsigned short orientation = proto->flagsExt() >> 16;
                    switch (orientation)
                    {
                        case 0x0000:
                            object->setLightOrientation(Orientation::NS);
                            break;
                        case 0x0800:
                            object->setLightOrientation(Orientation::EW);
                            break;
                        case 0x1000:
                            object->setLightOrientation(Orientation::NC);
                            break;
                        case 0x2000:
                            object->setLightOrientation(Orientation::SC);
                            break;
                        case 0x4000:
                            object->setLightOrientation(Orientation::EC);
                            break;
                        case 0x8000:
                            object->setLightOrientation(Orientation::WC);
                            break;
                    }
                    break;
                }
                case OBJECT_TYPE::TILE:
                {
                    throw Exception("ObjectFactory - unexpected tile object");
                }
                case OBJECT_TYPE::MISC:
                {
                    switch(PID& 0x00FFFFFF)
                    {
                        // Exit Grids
                        case 16:
                        case 17:
                        case 18:
                        case 19:
                        case 20:
                        case 21:
                        case 22:
                        case 23:
                            object = new ExitMiscObject();
                            break;
                        default:
                            object = new MiscObject();
                            break;
                    }

                    auto msg = ResourceManager::getInstance()->msgFileType("text/english/game/pro_misc.msg");
                    try
                    {
                        object->setName(msg->message(proto->messageId())->text());
                        object->setDescription(msg->message(proto->messageId() + 1)->text());
                    }
                    catch (const Exception&) {}
                    break;
                }
            }
            object->setPID(PID);
            object->setFID(proto->FID());
            object->setFlags(proto->flags());

            if (proto->scriptId() > 0)
            {
                auto intFile = ResourceManager::getInstance()->intFileType(proto->scriptId());
                if (intFile) object->setScript(new VM::Script(intFile, object));
            }

            return object;
        }
    }
}
