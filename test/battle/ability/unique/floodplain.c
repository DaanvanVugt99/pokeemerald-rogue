#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_PSYCHIC].split == SPLIT_SPECIAL);
    ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_PHYSICAL);
}

SINGLE_BATTLE_TEST("Floodplain reduces special damage based on qualifying ally count", s16 damage)
{
    u16 ally1, ally2, ally3;

    PARAMETRIZE { ally1 = SPECIES_WOBBUFFET; ally2 = SPECIES_WOBBUFFET; ally3 = SPECIES_WOBBUFFET; } // 0 allies
    PARAMETRIZE { ally1 = SPECIES_HORSEA;    ally2 = SPECIES_WOBBUFFET; ally3 = SPECIES_WOBBUFFET; } // 1 ally
    PARAMETRIZE { ally1 = SPECIES_HORSEA;    ally2 = SPECIES_SANDSHREW; ally3 = SPECIES_WOBBUFFET; } // 2 allies
    PARAMETRIZE { ally1 = SPECIES_HORSEA;    ally2 = SPECIES_SANDSHREW; ally3 = SPECIES_GEODUDE; }   // 3 allies

    GIVEN {
        PLAYER(SPECIES_SWAMPERT) { Ability(ABILITY_TORRENT); UniqueAbility(ABILITY_FLOODPLAIN); HP(400); MaxHP(400); Moves(MOVE_CELEBRATE); }
        PLAYER(ally1);
        PLAYER(ally2);
        PLAYER(ally3);
        OPPONENT(SPECIES_ALAKAZAM) { Moves(MOVE_PSYCHIC); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_PSYCHIC); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.9), results[1].damage);
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.8), results[2].damage);
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.7), results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Floodplain does not reduce physical damage", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_FLOODPLAIN; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); HP(400); MaxHP(400); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_HORSEA);
        PLAYER(SPECIES_SANDSHREW);
        PLAYER(SPECIES_GEODUDE);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Floodplain restores 1/16 max HP each turn at 3 qualifying allies")
{
    GIVEN {
        PLAYER(SPECIES_SWAMPERT) { Ability(ABILITY_TORRENT); UniqueAbility(ABILITY_FLOODPLAIN); HP(200); MaxHP(400); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_HORSEA);
        PLAYER(SPECIES_SANDSHREW);
        PLAYER(SPECIES_GEODUDE);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FLOODPLAIN);
        MESSAGE("Swampert's Floodplain restored its HP a little!");
        HP_BAR(player, damage: -25);
    }
}

SINGLE_BATTLE_TEST("Floodplain does not restore HP below full power")
{
    GIVEN {
        PLAYER(SPECIES_SWAMPERT) { Ability(ABILITY_TORRENT); UniqueAbility(ABILITY_FLOODPLAIN); HP(200); MaxHP(400); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_HORSEA);
        PLAYER(SPECIES_SANDSHREW);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_FLOODPLAIN);
        }
    } THEN {
        EXPECT_EQ(player->hp, 200);
    }
}
