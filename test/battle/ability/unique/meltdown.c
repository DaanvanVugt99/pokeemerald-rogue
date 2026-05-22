#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
}

#if B_SNOW_WARNING >= GEN_9
SINGLE_BATTLE_TEST("Meltdown summons Snow on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_VANILLUXE) { Ability(ABILITY_SOUNDPROOF); UniqueAbility(ABILITY_MELTDOWN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MELTDOWN);
        MESSAGE("It started to snow!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_SNOW_CONTINUES);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SNOW);
    }
}
#endif

SINGLE_BATTLE_TEST("Meltdown absorbs Fire moves, heals, and changes to Water type")
{
    GIVEN {
        PLAYER(SPECIES_VANILLUXE) { HP(50); MaxHP(100); Ability(ABILITY_SOUNDPROOF); UniqueAbility(ABILITY_MELTDOWN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CHARMANDER) { Speed(100); Moves(MOVE_EMBER); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_EMBER); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MELTDOWN);
        MESSAGE("It started to snow!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_SNOW_CONTINUES);
        MESSAGE("Foe Charmander used Ember!");
        ABILITY_POPUP(player, ABILITY_MELTDOWN);
        HP_BAR(player, damage: -25);
        MESSAGE("Vanilluxe restored HP using its Meltdown!");
        MESSAGE("Vanilluxe transformed into the Water type!");
    } THEN {
        EXPECT_EQ(player->hp, 75);
        EXPECT_EQ(player->type1, TYPE_WATER);
        EXPECT_EQ(player->type2, TYPE_WATER);
    }
}

SINGLE_BATTLE_TEST("Meltdown blocks Fire damage at full HP and changes to Water type")
{
    GIVEN {
        PLAYER(SPECIES_VANILLUXE) { HP(100); MaxHP(100); Ability(ABILITY_SOUNDPROOF); UniqueAbility(ABILITY_MELTDOWN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CHARMANDER) { Speed(100); Moves(MOVE_EMBER); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_EMBER); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MELTDOWN);
        MESSAGE("It started to snow!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_SNOW_CONTINUES);
        MESSAGE("Foe Charmander used Ember!");
        ABILITY_POPUP(player, ABILITY_MELTDOWN);
        MESSAGE("Vanilluxe's Meltdown made Ember useless!");
        MESSAGE("Vanilluxe transformed into the Water type!");
    } THEN {
        EXPECT_EQ(player->hp, 100);
        EXPECT_EQ(player->type1, TYPE_WATER);
        EXPECT_EQ(player->type2, TYPE_WATER);
    }
}

SINGLE_BATTLE_TEST("Meltdown does not absorb non-Fire moves")
{
    GIVEN {
        PLAYER(SPECIES_VANILLUXE) { HP(100); MaxHP(100); Ability(ABILITY_SOUNDPROOF); UniqueAbility(ABILITY_MELTDOWN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SQUIRTLE) { Speed(100); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_WATER_GUN); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MELTDOWN);
        MESSAGE("It started to snow!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_SNOW_CONTINUES);
        NONE_OF {
            MESSAGE("Vanilluxe restored HP using its Meltdown!");
            MESSAGE("Vanilluxe's Meltdown made Water Gun ineffective!");
            MESSAGE("Vanilluxe transformed into the Water type!");
        }
    } THEN {
        EXPECT_LT(player->hp, 100);
        EXPECT_NE(player->type1, TYPE_WATER);
    }
}
