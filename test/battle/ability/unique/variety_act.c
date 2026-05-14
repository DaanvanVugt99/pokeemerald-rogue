#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_GROWL));
    ASSUME(!IS_MOVE_STATUS(MOVE_TACKLE));
    ASSUME(gBattleMoves[MOVE_HYPER_VOICE].soundMove);
    ASSUME(gBattleMoves[MOVE_SWORDS_DANCE].danceMove);
}

SINGLE_BATTLE_TEST("Variety Act uses a random sound move after a status move")
{
    GIVEN {
        PLAYER(SPECIES_MR_RIME) { Ability(ABILITY_TANGLED_FEET); UniqueAbility(ABILITY_VARIETY_ACT); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL, WITH_RNG(RNG_ROGUE_VARIETY_ACT, MOVE_HYPER_VOICE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        ABILITY_POPUP(player, ABILITY_VARIETY_ACT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT(gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].hp < gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].maxHP);
    }
}

SINGLE_BATTLE_TEST("Variety Act can choose a dance move after a status move")
{
    GIVEN {
        PLAYER(SPECIES_MR_RIME) { Ability(ABILITY_TANGLED_FEET); UniqueAbility(ABILITY_VARIETY_ACT); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL, WITH_RNG(RNG_ROGUE_VARIETY_ACT, MOVE_SWORDS_DANCE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        ABILITY_POPUP(player, ABILITY_VARIETY_ACT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWORDS_DANCE, player);
    } THEN {
        EXPECT_EQ(gBattleMons[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Variety Act does not trigger after damaging moves")
{
    GIVEN {
        PLAYER(SPECIES_MR_RIME) { Ability(ABILITY_TANGLED_FEET); UniqueAbility(ABILITY_VARIETY_ACT); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_VARIETY_ACT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, player);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SWORDS_DANCE, player);
        }
    }
}
