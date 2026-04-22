#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ATTRACT].effect == EFFECT_ATTRACT);
}

SINGLE_BATTLE_TEST("Allure uses Attract on switch-in when possible")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Gender(MON_MALE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_BEAUTIFLY) { Gender(MON_FEMALE); Ability(ABILITY_SWARM); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_ALLURE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ATTRACT, opponent);
    } THEN {
        EXPECT(player->status2 & STATUS2_INFATUATION);
    }
}

SINGLE_BATTLE_TEST("Allure uses Attract when sent out at battle start")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Gender(MON_MALE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_BEAUTIFLY) { Gender(MON_FEMALE); Ability(ABILITY_SWARM); UniqueAbility(ABILITY_ALLURE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_ALLURE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ATTRACT, opponent);
    } THEN {
        EXPECT(player->status2 & STATUS2_INFATUATION);
    }
}

SINGLE_BATTLE_TEST("Allure infatuates the foe at battle start")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Gender(MON_MALE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_BEAUTIFLY) { Gender(MON_FEMALE); Ability(ABILITY_SWARM); UniqueAbility(ABILITY_ALLURE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(IsBattlerAlive(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)));
        EXPECT(IsBattlerAlive(GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)));
        EXPECT_EQ(gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].species, SPECIES_BEAUTIFLY);
        EXPECT_EQ(GetBattlerUniqueAbility(GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)), ABILITY_ALLURE);
        EXPECT_EQ((u32)gProtectStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].extraMoveUsed, FALSE);
        EXPECT(player->status2 & STATUS2_INFATUATION);
    }
}

SINGLE_BATTLE_TEST("Allure does nothing if infatuation is not possible")
{
    GIVEN {
        PLAYER(SPECIES_SLOWPOKE) { Ability(ABILITY_OBLIVIOUS); Gender(MON_MALE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_BEAUTIFLY) { Gender(MON_FEMALE); Ability(ABILITY_SWARM); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(opponent, ABILITY_ALLURE);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ATTRACT, opponent);
        }
    } THEN {
        EXPECT(!(player->status2 & STATUS2_INFATUATION));
    }
}
