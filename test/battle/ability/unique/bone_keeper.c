#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BONE_CLUB].accuracy < 100);
    ASSUME(gBattleMoves[MOVE_BONE_CLUB].priority == 0);
}

SINGLE_BATTLE_TEST("Bone Keeper makes bone moves never miss")
{
    GIVEN {
        PLAYER(SPECIES_MAROWAK) { Ability(ABILITY_ROCK_HEAD); UniqueAbility(ABILITY_BONE_KEEPER); Moves(MOVE_BONE_CLUB); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BONE_CLUB, WITH_RNG(RNG_ACCURACY, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BONE_CLUB, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Bone Keeper gives bone moves priority if no ally remains")
{
    bool32 hasAlly;

    PARAMETRIZE { hasAlly = FALSE; }
    PARAMETRIZE { hasAlly = TRUE; }

    GIVEN {
        PLAYER(SPECIES_MAROWAK) { Speed(1); Ability(ABILITY_ROCK_HEAD); UniqueAbility(ABILITY_BONE_KEEPER); Moves(MOVE_SHADOW_BONE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(hasAlly ? 1 : 0); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHADOW_BONE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        if (hasAlly)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SHADOW_BONE, player);
        }
        else
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SHADOW_BONE, player);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        }
    }
}
