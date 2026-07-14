#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SWORDS_DANCE].effect == EFFECT_ATTACK_UP_2);
    ASSUME(gBattleMoves[MOVE_SCREECH].effect == EFFECT_DEFENSE_DOWN_2);
}

SINGLE_BATTLE_TEST("Natural Order moves every active battler's stat stages one step toward neutral")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_SCREECH); }
        OPPONENT(SPECIES_ZYGARDE) { Speed(1); Ability(ABILITY_AURA_BREAK); Moves(MOVE_SWORDS_DANCE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCREECH); MOVE(opponent, MOVE_SWORDS_DANCE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_NATURAL_ORDER);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
        MESSAGE("Foe Zygarde's Attack fell!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
        MESSAGE("Foe Zygarde's Defense rose!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Natural Order resolves after other end-turn abilities")
{
    GIVEN {
        PLAYER(SPECIES_NINJASK) { Ability(ABILITY_SPEED_BOOST); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ZYGARDE) { Ability(ABILITY_AURA_BREAK); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SPEED_BOOST);
        ABILITY_POPUP(opponent, ABILITY_NATURAL_ORDER);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        MESSAGE("Ninjask's Speed fell!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Natural Order does not activate when all stat stages are neutral")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ZYGARDE) { Ability(ABILITY_AURA_BREAK); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(opponent, ABILITY_NATURAL_ORDER);
    }
}
