#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_HYPNOSIS].effect == EFFECT_SLEEP);
    ASSUME(gBattleMoves[MOVE_HYPNOSIS].accuracy > 0);
    ASSUME(gBattleMoves[MOVE_DREAM_EATER].power > 30);
    ASSUME(IS_MOVE_STATUS(MOVE_GROWL));
}

SINGLE_BATTLE_TEST("Dreamweaver uses 30 BP Dream Eater after status moves")
{
    s16 damage;
    PASSES_RANDOMLY(gBattleMoves[MOVE_HYPNOSIS].accuracy, 100, RNG_ACCURACY);

    GIVEN {
        PLAYER(SPECIES_HYPNO) { Ability(ABILITY_INSOMNIA); UniqueAbility(ABILITY_DREAMWEAVER); Moves(MOVE_HYPNOSIS); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPNOSIS); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPNOSIS, player);
        ABILITY_POPUP(player, ABILITY_DREAMWEAVER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DREAM_EATER, player);
        HP_BAR(opponent, captureDamage: &damage);
    } THEN {
        EXPECT_GT(damage, 0);
    }
}

SINGLE_BATTLE_TEST("Dreamweaver's Dream Eater fails if target is not asleep")
{
    GIVEN {
        PLAYER(SPECIES_HYPNO) { Ability(ABILITY_INSOMNIA); UniqueAbility(ABILITY_DREAMWEAVER); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        ABILITY_POPUP(player, ABILITY_DREAMWEAVER);
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}
