#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Chromatic Flux changes its holder and every move to the rolled type")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_HOUNDOOM) {
            Ability(ABILITY_FLASH_FIRE);
            UniqueAbility(ABILITY_CHROMATIC_FLUX);
            Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN, MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            SWITCH_WITH_RNG(player, 1, WITH_RNG(RNG_ROGUE_CHROMATIC_FLUX, TYPE_FIRE));
            MOVE(opponent, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(player, MOVE_CELEBRATE);
            MOVE(opponent, MOVE_WATER_GUN);
        }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_CHROMATIC_FLUX);
        MESSAGE("Houndoom transformed into the Fire type!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ABILITY_POPUP(player, ABILITY_CHROMATIC_FLUX);
        MESSAGE("Houndoom transformed into the Fire type!");
        ABILITY_POPUP(player, ABILITY_FLASH_FIRE);
    }
}

SINGLE_BATTLE_TEST("Chromatic Flux gives every attacking move STAB", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_FREESTYLE; }
    PARAMETRIZE { uniqueAbility = ABILITY_CHROMATIC_FLUX; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_MEW) {
            SpAttack(100);
            UniqueAbility(uniqueAbility);
            Moves(MOVE_WATER_GUN);
        }
        OPPONENT(SPECIES_WOBBUFFET) {
            HP(1000);
            MaxHP(1000);
            SpDefense(100);
            Moves(MOVE_CELEBRATE);
        }
    } WHEN {
        TURN {
            SWITCH_WITH_RNG(player, 1, WITH_RNG(RNG_ROGUE_CHROMATIC_FLUX, TYPE_FIRE));
            MOVE(opponent, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(player, MOVE_WATER_GUN, WITH_RNG(RNG_DAMAGE_MODIFIER, 100));
            MOVE(opponent, MOVE_CELEBRATE);
        }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Chromatic Flux rerolls between turns")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_MEW) {
            UniqueAbility(ABILITY_CHROMATIC_FLUX);
            Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            SWITCH_WITH_RNG(player, 1, WITH_RNG(RNG_ROGUE_CHROMATIC_FLUX, TYPE_FAIRY));
            MOVE(opponent, MOVE_CELEBRATE);
        }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_CHROMATIC_FLUX);
        MESSAGE("Mew transformed into the Fairy type!");
        ABILITY_POPUP(player, ABILITY_CHROMATIC_FLUX);
        MESSAGE("Mew transformed into the Fire type!");
    } THEN {
        EXPECT_EQ(GetBattlerType(B_POSITION_PLAYER_LEFT, 0, FALSE), TYPE_FIRE);
        EXPECT_EQ(GetBattlerType(B_POSITION_PLAYER_LEFT, 1, FALSE), TYPE_FIRE);
        EXPECT_EQ(GetBattlerType(B_POSITION_PLAYER_LEFT, 2, FALSE), TYPE_MYSTERY);
    }
}
