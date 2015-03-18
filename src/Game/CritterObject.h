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

#ifndef FALLTERGEIST_GAMECRITTEROBJECT_H
#define FALLTERGEIST_GAMECRITTEROBJECT_H

// C++ standard includes
#include <vector>

// Falltergeist includes
#include "Object.h"
#include "ArmorItemObject.h"
#include "../PathFinding/Hexagon.h"

// Third party includes

namespace Falltergeist
{
class Animation;

namespace Game
{
class ItemObject;

/**
 * Critter refers to player, all NPCs, creatures, robots, etc - all movable and shootable objects.
 */
class GameCritterObject : public GameObject
{
protected:
    bool _moving  = false;
    bool _running = false;

    int _gender = GENDER_MALE;
    int _poisonLevel = 0;
    int _radiationLevel = 0;
    int _hitPoints = 0;
    int _hitPointsMax = 0;
    int _healingRate = 0;
    int _armorClass = 0;
    int _actionPoints = 0;
    int _actionPointsMax = 0;
    int _meleeDamage = 0;
    int _sequence = 0;
    int _criticalChance = 0;
    
    unsigned int _nextIdleAnim = 0;

    unsigned int _currentHand = HAND_RIGHT;
    unsigned int _carryWeightMax = 0;

    std::vector<int> _stats = {0, 0, 0, 0, 0, 0, 0};
    std::vector<int> _statsBonus = {0, 0, 0, 0, 0, 0, 0};
    std::vector<int> _skillsTagged = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<int> _skillsGainedValue = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<int> _traitsTagged = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<int> _damageResist = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<int> _damageThreshold = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<GameItemObject*> _inventory;
    std::vector<Hexagon*> _movementQueue;
    GameArmorItemObject* _armorSlot = 0;
    GameItemObject* _leftHandSlot = 0;
    GameItemObject* _rightHandSlot = 0;

    virtual Animation* _generateMovementAnimation();
    virtual std::string _generateArmorFrmString();
    virtual std::string _generateWeaponFrmString();
    void _setupNextIdleAnim();
public:
    enum { HAND_RIGHT = 0, HAND_LEFT };
    enum { GENDER_MALE = 0, GENDER_FEMALE };
    enum 
    { 
        STAT_STRENGTH = 0, 
        STAT_PERCEPTION, 
        STAT_ENDURANCE, 
        STAT_CHARISMA,
        STAT_INTELLIGENCE, 
        STAT_AGILITY, 
        STAT_LUCK 
    };
    enum 
    { 
        DAMAGE_NORMAL = 0, 
        DAMAGE_LASER, 
        DAMAGE_FIRE, 
        DAMAGE_PLASMA, 
        DAMAGE_ELECTRICAL, 
        DAMAGE_EMP, 
        DAMAGE_EXPLOSION, 
        DAMAGE_RADIATION, 
        DAMAGE_POISON 
    };
    enum 
    {
        TRAIT_FAST_METABOLISM = 0,
        TRAIT_BRUISER,
        TRAIT_SMALL_FRAME,
        TRAIT_ONE_HANDED,
        TRAIT_FINESSE,
        TRAIT_KAMIKAZE,
        TRAIT_HEAVY_HANDED,
        TRAIT_FAST_SHOT,
        TRAIT_BLOODY_MESS,
        TRAIT_JINXED,
        TRAIT_GOOD_NATURED,
        TRAIT_CHEM_RELIANT,
        TRAIT_CHEM_RESISTANT,
        TRAIT_SEX_APPEAL,
        TRAIT_SKILLED,
        TRAIT_GIFTED
    };
    enum
    {
        SKILL_SMALL_GUNS = 0,
        SKILL_BIG_GUNS,
        SKILL_ENERGY_WEAPONS,
        SKILL_UNARMED,
        SKILL_MELEE_WEAPONS,
        SKILL_THROWING,
        SKILL_FIRST_AID,
        SKILL_DOCTOR,
        SKILL_SNEAK,
        SKILL_LOCKPICK,
        SKILL_STEAL,
        SKILL_TRAPS,
        SKILL_SCIENCE,
        SKILL_REPAIR,
        SKILL_SPEECH,
        SKILL_BARTER,
        SKILL_GAMBLING,
        SKILL_OUTDOORSMAN
    };

    GameCritterObject();
    virtual ~GameCritterObject();

    // critter's own inventory
    std::vector<GameItemObject*>* inventory();
    virtual void setOrientation(int value);

    std::vector<Hexagon*>* movementQueue();

    GameArmorItemObject* armorSlot() const;
    void setArmorSlot(GameArmorItemObject* object);

    GameItemObject* leftHandSlot() const;
    void setLeftHandSlot(GameItemObject* object);

    GameItemObject* rightHandSlot() const;
    void setRightHandSlot(GameItemObject* object);

    GameItemObject* currentHandSlot() const;

    int gender() const;
    void setGender(unsigned int value);

    int stat(unsigned int stat) const;
    void setStat(unsigned int stat, int value);

    int statBonus(unsigned int stat) const;
    void setStatBonus(unsigned int stat, int value);

    // returns total stat value (with bonuses)
    int statTotal(unsigned int num) const;

    int skillTagged(unsigned int skill) const;
    void setSkillTagged(unsigned int skill, int value);

    int skillBaseValue(unsigned int skill) const;
    int skillGainedValue(unsigned int skill) const;
    void setSkillGainedValue(unsigned int skill, int value);
    int skillValue(unsigned int skill) const;

    int traitTagged(unsigned int num) const;
    void setTraitTagged(unsigned int num, int value);

    int hitPoints() const;
    void setHitPoints(int value);

    virtual int hitPointsMax() const;
    void setHitPointsMax(int value);

    virtual int armorClass() const;
    void setArmorClass(int value);

    virtual int actionPoints() const;
    void setActionPoints(int value);

    virtual int actionPointsMax() const;
    void setActionPointsMax(int value);

    virtual unsigned int carryWeight() const;
    virtual unsigned int carryWeightMax() const;
    void setCarryWeightMax(unsigned int value);

    virtual int meleeDamage() const;
    void setMeleeDamage(int value);

    virtual int sequence() const;
    void setSequence(int value);

    virtual int criticalChance() const;
    void setCriticalChance(int value);

    virtual int healingRate() const;
    void setHealingRate(int value);

    int poisonLevel() const;
    void setPoisonLevel(int value);
    
    int radiationLevel() const;
    void setRadiationLevel(int radiationLevel);

    virtual int damageResist(unsigned int type) const;
    void setDamageResist(unsigned int type, int value);

    virtual int damageThreshold(unsigned int type) const;
    void setDamageThreshold(unsigned int type, int value);

    unsigned int currentHand() const;
    void setCurrentHand(unsigned int value);

    virtual void damage_p_proc();
    virtual void combat_p_proc();
    virtual void critter_p_proc();
    virtual void talk_p_proc();
    virtual void use_skill_on_p_proc();
    virtual void is_dropping_p_proc();

    virtual void think();
    virtual void onMovementAnimationEnded(Event* event);

    virtual bool running() const;
    virtual void setRunning(bool value);
    
    virtual void stopMovement();

    virtual Animation* setActionAnimation(std::string action);
};

}
}

#endif // FALLTERGEIST_GAMECRITTEROBJECT_H
