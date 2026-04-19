#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_PSYCHIC].split == SPLIT_SPECIAL);
    ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_PHYSICAL);
}

SINGLE_BATTLE_TEST("Floodplain reduces damage from special moves but not physical moves", s16 damage)
{
    u16 uniqueAbility;
    u16 move;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; move = MOVE_PSYCHIC; }
    PARAMETRIZE { uniqueAbility = ABILITY_FLOODPLAIN; move = MOVE_PSYCHIC; }
    PARAMETRIZE { uniqueAbility = ABILITY_NONE; move = MOVE_TACKLE; }
    PARAMETRIZE { uniqueAbility = ABILITY_FLOODPLAIN; move = MOVE_TACKLE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ALAKAZAM) { Moves(move); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.9), results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Floodplain restores 1/16 max HP in rain")
{
    GIVEN {
        PLAYER(SPECIES_SWAMPERT) { Ability(ABILITY_TORRENT); HP(200); MaxHP(400); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_RAIN_DANCE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_RAIN_DANCE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FLOODPLAIN);
        MESSAGE("Swampert's Floodplain restored its HP a little!");
        HP_BAR(player, damage: -25);
    }
}
