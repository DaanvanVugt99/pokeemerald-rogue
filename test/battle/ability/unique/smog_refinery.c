#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_DYNAMIC_PUNCH].effect == EFFECT_CONFUSE_HIT);
    ASSUME(gBattleMoves[MOVE_DYNAMIC_PUNCH].secondaryEffectChance == 100);
    ASSUME(gBattleMoves[MOVE_ACID_SPRAY].effect == EFFECT_SPECIAL_DEFENSE_DOWN_HIT_2);
    ASSUME(gBattleMoves[MOVE_THUNDER_WAVE].effect == EFFECT_PARALYZE);
}

SINGLE_BATTLE_TEST("Smog Refinery blocks a secondary effect and retaliates with Strange Steam")
{
    GIVEN {
        PLAYER(SPECIES_WEEZING_GALARIAN) { HP(500); Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_SMOG_REFINERY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MACHAMP) { HP(500); Ability(ABILITY_NO_GUARD); Moves(MOVE_DYNAMIC_PUNCH); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_DYNAMIC_PUNCH); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DYNAMIC_PUNCH, opponent);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_SMOG_REFINERY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STRANGE_STEAM, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT((player->status2 & STATUS2_CONFUSION) == 0);
        EXPECT(opponent->hp < opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Smog Refinery blocks a secondary stat drop and retaliates with Strange Steam")
{
    GIVEN {
        PLAYER(SPECIES_WEEZING_GALARIAN) { HP(500); Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_SMOG_REFINERY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); Speed(100); Moves(MOVE_ACID_SPRAY); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ACID_SPRAY); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ACID_SPRAY, opponent);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_SMOG_REFINERY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STRANGE_STEAM, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
        EXPECT(opponent->hp < opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Neutralizing Gas suppresses its holder's Smog Refinery")
{
    GIVEN {
        PLAYER(SPECIES_WEEZING_GALARIAN) { HP(500); Ability(ABILITY_NEUTRALIZING_GAS); UniqueAbility(ABILITY_SMOG_REFINERY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); Speed(100); Moves(MOVE_ACID_SPRAY); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ACID_SPRAY); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SMOG_REFINERY);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_STRANGE_STEAM, player);
        }
    } THEN {
        EXPECT_LT(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Smog Refinery does not block primary effects from status moves")
{
    GIVEN {
        PLAYER(SPECIES_WEEZING_GALARIAN) { Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_SMOG_REFINERY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_THUNDER_WAVE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_THUNDER_WAVE); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SMOG_REFINERY);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_STRANGE_STEAM, player);
        }
    } THEN {
        EXPECT(player->status1 & STATUS1_PARALYSIS);
    }
}
