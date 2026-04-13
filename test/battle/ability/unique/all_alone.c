#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
    ASSUME(gBattleMoves[MOVE_BONE_CLUB].power > 0);
}

SINGLE_BATTLE_TEST("All Alone uses Bone Club after a contact hit when it is the only alive party mon")
{
    s16 boneClubDamage;

    GIVEN {
        PLAYER(SPECIES_MAROWAK) { Ability(ABILITY_ROCK_HEAD); UniqueAbility(ABILITY_ALL_ALONE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(0); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_ALL_ALONE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BONE_CLUB, player);
        HP_BAR(opponent, captureDamage: &boneClubDamage);
    } THEN {
        EXPECT_GT(boneClubDamage, 0);
    }
}

SINGLE_BATTLE_TEST("All Alone does not trigger while another party Pokemon is still alive")
{
    GIVEN {
        PLAYER(SPECIES_MAROWAK) { Ability(ABILITY_ROCK_HEAD); UniqueAbility(ABILITY_ALL_ALONE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}
