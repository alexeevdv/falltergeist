#include <cmath>
#include <functional>
#include "../Format/Gcd/File.h"
#include "../Game/DudeObject.h"
#include "../Game/Game.h"
#include "../Game/WeaponItemObject.h"
#include "../ResourceManager.h"
#include "../State/Location.h"
#include "../UI/Animation.h"
#include "../UI/AnimationQueue.h"
#include "../UI/TextArea.h"

namespace Falltergeist
{
    namespace Game
    {
        DudeObject::DudeObject() : CritterObject()
        {
            _type = Type::DUDE;
            setLightIntensity(65536);
            setLightRadius(4);
        }

        void DudeObject::loadFromGCDFile(Format::Gcd::File* gcd)
        {
            for (unsigned i = (unsigned)STAT::STRENGTH; i <= (unsigned)STAT::LUCK; i++) {
                setStat((STAT)i, gcd->stat((STAT)i));
                setStatBonus((STAT)i, gcd->statBonus((STAT)i));
            }

            _statsPoints = gcd->characterPoints();
            _name = gcd->name();
            _age = gcd->age();

            _hitPointsMax = gcd->hitPoints() + gcd->hitPointsBonus();
            _hitPoints = _hitPointsMax;

            if ((signed)gcd->firstTrait() >= 0) {
                setTraitTagged(gcd->firstTrait(), 1);
            }
            if ((signed)gcd->secondTrait() >= 0) {
                setTraitTagged(gcd->secondTrait(), 1);
            }

            if ((signed)gcd->firstTaggedSkill() >= 0) {
                setSkillTagged(gcd->firstTaggedSkill(), 1);
                _skillsPoints--;
            }
            if ((signed)gcd->secondTaggedSkill() >= 0) {
                setSkillTagged(gcd->secondTaggedSkill(), 1);
                _skillsPoints--;
            }
            if ((signed)gcd->thirdTaggedSkill() >= 0) {
                setSkillTagged(gcd->thirdTaggedSkill(), 1);
                _skillsPoints--;
            }

            setGender(gcd->gender());
            setActionPoints(actionPointsMax());
        }

        int DudeObject::experience() const
        {
            return _experience;
        }

        void DudeObject::setExperience(int value)
        {
            _experience = value;
        }

        std::string DudeObject::biography() const
        {
            return _biography;
        }

        void DudeObject::setBiography(const std::string& value)
        {
            _biography = value;
        }

        int DudeObject::statsPoints() const
        {
            return _statsPoints;
        }

        void DudeObject::setStatsPoints(int value)
        {
            _statsPoints = value;
        }

        int DudeObject::skillsPoints() const
        {
            return _skillsPoints;
        }

        void DudeObject::setSkillsPoints(int value)
        {
            _skillsPoints = value;
        }

        /**
         * @return current rank of the \p perk or zero if the player doesn't have the perk
         */
        int DudeObject::perk(PERK perk) const
        {
            return _perks.at(static_cast<unsigned>(perk));
        }

        void DudeObject::addPerk(PERK perk)
        {
            _perks[static_cast<unsigned>(perk)]++;
        }

        int DudeObject::hitPointsMax() const
        {
            int value = 15;
            value += statTotal(STAT::ENDURANCE) * 2;
            value += statTotal(STAT::STRENGTH);
            value += (2 + static_cast<int>(ceil(statTotal(STAT::ENDURANCE)/2))) * (level() - 1);
            return value;
        }

        int DudeObject::level() const
        {
            return _level;
        }

        void DudeObject::setLevel(int value)
        {
            _level = value;
        }

        int DudeObject::armorClass() const
        {
            unsigned int value = 0;
            if (!traitTagged(TRAIT::KAMIKAZE)) {
                value += statTotal(STAT::AGILITY) > 10 ? 10 : statTotal(STAT::AGILITY);
            }
            return value;

        }

        int DudeObject::actionPointsMax() const
        {
            unsigned int value = 0;
            value += 5 + static_cast<int>(ceil(statTotal(STAT::AGILITY) / 2));
            if (traitTagged(TRAIT::BRUISER)) {
                value -= 2;
            }
            return value;
        }

        unsigned int DudeObject::carryWeightMax() const
        {
            unsigned int value = 0;
            unsigned int st = statTotal(STAT::STRENGTH);

            if (traitTagged(TRAIT::SMALL_FRAME)) {
                value += 25 + 15*(st > 10 ? 10 : st);
                if (traitTagged(TRAIT::GIFTED) && st <= 10) {
                    value += 10;
                }
            } else {
                value += 25 + 25*(st > 10 ? 10 : st);
            }
            return value;
        }

        int DudeObject::meleeDamage() const
        {
            unsigned int value = 0;
            unsigned int st = statTotal(STAT::STRENGTH);
            if (st > 10) {
                st = 10;
            }
            value += st > 5 ? st - 5 : 1;
            if (traitTagged(TRAIT::HEAVY_HANDED)) {
                value += 4;
            }
            return value;
        }

        int DudeObject::damageResistance() const
        {
            return 0;
        }

        int DudeObject::radiationResistance() const
        {
            return 0;
        }

        int DudeObject::poisonResistance() const
        {
            int value = 0;
            if (!traitTagged(TRAIT::FAST_METABOLISM)) {
                value += 5*statTotal(STAT::ENDURANCE);
            }
            return value;
        }

        int DudeObject::sequence() const
        {
            unsigned int value = 0;
            unsigned int pe = statTotal(STAT::PERCEPTION);
            value += 2*(pe > 10 ? 10 : pe);
            if (traitTagged(TRAIT::KAMIKAZE)) {
                value += 5;
            }
            return value;
        }

        int DudeObject::healingRate() const
        {
            int value = 0;
            int en = statTotal(STAT::ENDURANCE);
            value += static_cast<int>(ceil((en > 10 ? 10 : en) / 3));
            if (value == 0) {
                value = 1;
            }

            if (traitTagged(TRAIT::FAST_METABOLISM)) {
                value += 2;
            }
            return value;
        }

        int DudeObject::criticalChance() const
        {
            unsigned int value = 0;
            unsigned int lk = statTotal(STAT::LUCK);
            value += lk > 10 ? 10 : lk;
            if (traitTagged(TRAIT::FINESSE)) {
                value += 10;
            }
            return value;
        }

        void DudeObject::_generateUi()
        {
            CritterObject::_generateUi();
        }

        Point DudeObject::eggOffset()
        {
            return animation()->frameOffset() + animation()->shift() - Point(63, 78);
        }

        std::string DudeObject::scrName() const
        {
            return _name;
        }
    }
}
