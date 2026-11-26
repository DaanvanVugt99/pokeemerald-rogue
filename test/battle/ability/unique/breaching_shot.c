#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Breaching Shot interrupts a slower foe once per switch-in and refreshes on re-entry")
{
    GIVEN {
        PLAYER(SPECIES_CLAWITZER) { Speed(100); SpAttack(80); Ability(ABILITY_MEGA_LAUNCHER); UniqueAbility(ABILITY_BREACHING_SHOT); Moves(MOVE_WATER_PULSE); }
        PLAYER(SPECIES_ODDISH) { Speed(20); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); HP(1000); MaxHP(1000); SpDefense(100); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(50); HP(1000); MaxHP(1000); SpDefense(100); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_PULSE, WITH_RNG(RNG_SECONDARY_EFFECT, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_WATER_PULSE, WITH_RNG(RNG_SECONDARY_EFFECT, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_WATER_PULSE, WITH_RNG(RNG_SECONDARY_EFFECT, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_WATER_PULSE, WITH_RNG(RNG_SECONDARY_EFFECT, FALSE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_PULSE, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_BREACHING_SHOT);
        MESSAGE("Foe Wynaut was dragged out!");
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_PULSE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_BREACHING_SHOT);
            MESSAGE("Foe Wobbuffet was dragged out!");
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_PULSE, player);
        ABILITY_POPUP(player, ABILITY_BREACHING_SHOT);
        MESSAGE("Foe Wobbuffet was dragged out!");
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_PULSE, player);
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_WOBBUFFET);
    }
}

SINGLE_BATTLE_TEST("Breaching Shot waits for a Water Pulse that damages the foe")
{
    GIVEN {
        PLAYER(SPECIES_CLAWITZER) { Speed(100); SpAttack(80); Ability(ABILITY_MEGA_LAUNCHER); UniqueAbility(ABILITY_BREACHING_SHOT); Moves(MOVE_WATER_PULSE, MOVE_DARK_PULSE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); HP(1000); MaxHP(1000); SpDefense(100); Moves(MOVE_PROTECT, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(60); HP(1000); MaxHP(1000); SpDefense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_DARK_PULSE, WITH_RNG(RNG_SECONDARY_EFFECT, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_WATER_PULSE, hit: FALSE); }
        TURN { MOVE(player, MOVE_WATER_PULSE, WITH_RNG(RNG_SECONDARY_EFFECT, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_WATER_PULSE, WITH_RNG(RNG_SECONDARY_EFFECT, FALSE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DARK_PULSE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        NOT ABILITY_POPUP(player, ABILITY_BREACHING_SHOT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_PULSE, player);
        ABILITY_POPUP(player, ABILITY_BREACHING_SHOT);
        MESSAGE("Foe Wynaut was dragged out!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_PULSE, player);
    }
}

SINGLE_BATTLE_TEST("Breaching Shot respects forced-switch immunity")
{
    GIVEN {
        PLAYER(SPECIES_CLAWITZER) { Speed(100); SpAttack(80); Ability(ABILITY_MEGA_LAUNCHER); UniqueAbility(ABILITY_BREACHING_SHOT); Moves(MOVE_WATER_PULSE); }
        OPPONENT(SPECIES_OCTILLERY) { Speed(50); Ability(ABILITY_SUCTION_CUPS); HP(1000); MaxHP(1000); SpDefense(100); }
        OPPONENT(SPECIES_WYNAUT) { Speed(60); HP(1000); MaxHP(1000); SpDefense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_PULSE, WITH_RNG(RNG_SECONDARY_EFFECT, FALSE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_PULSE, player);
        HP_BAR(opponent);
        ABILITY_POPUP(opponent, ABILITY_SUCTION_CUPS);
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_OCTILLERY);
    }
}
