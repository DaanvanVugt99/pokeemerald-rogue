#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gItems[ITEM_LIFE_ORB].holdEffect == HOLD_EFFECT_LIFE_ORB);
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_FIRE_SPIN].effect == EFFECT_TRAP);
}

SINGLE_BATTLE_TEST("Singularity Reactor uses Fire Spin after Iron Moth's first Fire move if it is the only Paradox")
{
    bool32 hasOtherParadox;

    PARAMETRIZE { hasOtherParadox = FALSE; }
    PARAMETRIZE { hasOtherParadox = TRUE; }

    GIVEN {
        PLAYER(SPECIES_IRON_MOTH) { Speed(100); Ability(ABILITY_NO_GUARD); Moves(MOVE_EMBER); }
        if (hasOtherParadox)
            PLAYER(SPECIES_IRON_JUGULIS) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        else
            PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        HP_BAR(opponent);
        if (!hasOtherParadox)
        {
            ABILITY_POPUP(player, ABILITY_SINGULARITY_REACTOR);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FIRE_SPIN, player);
            HP_BAR(opponent);
        }
        else
        {
            NONE_OF {
                ABILITY_POPUP(player, ABILITY_SINGULARITY_REACTOR);
                ANIMATION(ANIM_TYPE_MOVE, MOVE_FIRE_SPIN, player);
            }
        }
    } THEN {
        if (hasOtherParadox)
        {
            EXPECT_EQ(opponent->status2 & STATUS2_WRAPPED, 0);
            EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
        }
        else
        {
            EXPECT_NE(opponent->status2 & STATUS2_WRAPPED, 0);
            EXPECT_EQ(gBattleStruct->wrappedMove[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)], MOVE_FIRE_SPIN);
            EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
        }
    }
}

SINGLE_BATTLE_TEST("Singularity Reactor only triggers once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_IRON_MOTH) { Speed(100); Ability(ABILITY_NO_GUARD); Moves(MOVE_EMBER); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_SINGULARITY_REACTOR);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FIRE_SPIN, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SINGULARITY_REACTOR);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FIRE_SPIN, player);
        }
    } THEN {
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Singularity Reactor does not trigger after non-Fire moves")
{
    GIVEN {
        PLAYER(SPECIES_IRON_MOTH) { Speed(100); Ability(ABILITY_NO_GUARD); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SINGULARITY_REACTOR);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FIRE_SPIN, player);
        }
    } THEN {
        EXPECT_EQ(opponent->status2 & STATUS2_WRAPPED, 0);
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Singularity Reactor does not trigger if pending Life Orb damage would faint the user")
{
    GIVEN {
        PLAYER(SPECIES_IRON_MOTH) { HP(1); MaxHP(100); Speed(100); Ability(ABILITY_NO_GUARD); Item(ITEM_LIFE_ORB); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(200); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        HP_BAR(opponent);
        NOT ABILITY_POPUP(player, ABILITY_SINGULARITY_REACTOR);
        HP_BAR(player);
    } THEN {
        EXPECT_EQ(player->hp, 0);
        EXPECT_EQ(opponent->status2 & STATUS2_WRAPPED, 0);
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}
