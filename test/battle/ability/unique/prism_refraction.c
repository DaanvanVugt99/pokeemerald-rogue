#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_PHOTON_GEYSER].power > 0);
    ASSUME(gBattleMoves[MOVE_ECLIPSE].split == SPLIT_STATUS);
    ASSUME(gBattleMoves[MOVE_ECLIPSE].type == TYPE_DARK);
}

SINGLE_BATTLE_TEST("Prism Refraction gives Photon Geyser the type of the immediately preceding status move")
{
    GIVEN {
        PLAYER(SPECIES_NECROZMA) { Speed(100); Ability(ABILITY_PRISM_ARMOR); UniqueAbility(ABILITY_PRISM_REFRACTION); Moves(MOVE_ECLIPSE, MOVE_PHOTON_GEYSER); }
        OPPONENT(SPECIES_UMBREON) { HP(500); MaxHP(500); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ECLIPSE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_PHOTON_GEYSER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_PRISM_REFRACTION);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PHOTON_GEYSER, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Prism Refraction does not persist through an intervening move")
{
    GIVEN {
        PLAYER(SPECIES_NECROZMA) { Speed(100); Ability(ABILITY_PRISM_ARMOR); UniqueAbility(ABILITY_PRISM_REFRACTION); Moves(MOVE_ECLIPSE, MOVE_TACKLE, MOVE_PHOTON_GEYSER); }
        OPPONENT(SPECIES_SPIRITOMB) { HP(500); MaxHP(500); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ECLIPSE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_PHOTON_GEYSER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_PRISM_REFRACTION);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_PHOTON_GEYSER, player);
            HP_BAR(opponent);
        }
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}
