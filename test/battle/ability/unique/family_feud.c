#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BULLET_SEED].effect == EFFECT_MULTI_HIT);
    ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
}

SINGLE_BATTLE_TEST("Family Feud adds one hit to the next multi-hit move after being hit")
{
    bool32 wasHit;

    PARAMETRIZE { wasHit = TRUE; }
    PARAMETRIZE { wasHit = FALSE; }

    GIVEN {
        PLAYER(SPECIES_TANDEMAUS) { HP(1000); MaxHP(1000); Ability(ABILITY_SKILL_LINK); Moves(MOVE_CELEBRATE, MOVE_BULLET_SEED); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN {
        if (wasHit)
            TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
        else
            TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_BULLET_SEED); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        if (wasHit)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
            HP_BAR(player);
            ABILITY_POPUP(player, ABILITY_FAMILY_FEUD);
            MESSAGE("Tandemaus readied\nan extra hit!");
        }
        else
        {
            NONE_OF {
                ABILITY_POPUP(player, ABILITY_FAMILY_FEUD);
            }
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        if (wasHit)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
            MESSAGE("Hit 6 time(s)!");
        }
        else
        {
            MESSAGE("Hit 5 time(s)!");
        }
    }
}

SINGLE_BATTLE_TEST("Family Feud is not spent by a single-hit move before the next multi-hit move")
{
    GIVEN {
        PLAYER(SPECIES_TANDEMAUS) { HP(1000); MaxHP(1000); Ability(ABILITY_SKILL_LINK); Moves(MOVE_CELEBRATE, MOVE_TACKLE, MOVE_BULLET_SEED); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_BULLET_SEED); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_FAMILY_FEUD);
        MESSAGE("Tandemaus readied\nan extra hit!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        MESSAGE("Hit 6 time(s)!");
    }
}
