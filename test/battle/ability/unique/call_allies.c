#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gItems[ITEM_AIR_BALLOON].holdEffect == HOLD_EFFECT_AIR_BALLOON);
    ASSUME(gBattleMoves[MOVE_ECHOED_VOICE].soundMove);
    ASSUME(!IS_MOVE_STATUS(MOVE_ECHOED_VOICE));
    ASSUME(gBattleMoves[MOVE_HYPER_VOICE].soundMove);
    ASSUME(!IS_MOVE_STATUS(MOVE_HYPER_VOICE));
    ASSUME(!gBattleMoves[MOVE_TACKLE].soundMove);
    ASSUME(gBattleMoves[MOVE_INFESTED_TERRAIN].effect == EFFECT_INFESTED_TERRAIN);
}

SINGLE_BATTLE_TEST("Call Allies gives sound-based moves +1 priority")
{
    GIVEN {
        PLAYER(SPECIES_KRICKETUNE) { Speed(50); Ability(ABILITY_SWARM); UniqueAbility(ABILITY_CALL_ALLIES); Moves(MOVE_ECHOED_VOICE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ECHOED_VOICE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ECHOED_VOICE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("Call Allies sets Infested Terrain after a successful sound-based move")
{
    GIVEN {
        PLAYER(SPECIES_KRICKETUNE) { Ability(ABILITY_SWARM); UniqueAbility(ABILITY_CALL_ALLIES); Moves(MOVE_ECHOED_VOICE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ECHOED_VOICE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ECHOED_VOICE, player);
        ABILITY_POPUP(player, ABILITY_CALL_ALLIES);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_INFESTED_TERRAIN);
    }
}

SINGLE_BATTLE_TEST("Call Allies does not set Infested Terrain after a non-sound move")
{
    GIVEN {
        PLAYER(SPECIES_KRICKETUNE) { Ability(ABILITY_SWARM); UniqueAbility(ABILITY_CALL_ALLIES); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_CALL_ALLIES);
        }
    } THEN {
        EXPECT(!(gFieldStatuses & STATUS_FIELD_INFESTED_TERRAIN));
    }
}

SINGLE_BATTLE_TEST("Call Allies boosts sound-based move damage by 1.2x in Infested Terrain", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_CALL_ALLIES; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_INFESTED_TERRAIN, MOVE_HYPER_VOICE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); }
    } WHEN {
        TURN { MOVE(player, MOVE_INFESTED_TERRAIN); }
        TURN { MOVE(player, MOVE_HYPER_VOICE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.2), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Call Allies does not boost sound-based move damage if the user is not affected by Infested Terrain", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_AIR_BALLOON; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_CALL_ALLIES); Item(item); Moves(MOVE_INFESTED_TERRAIN, MOVE_HYPER_VOICE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); }
    } WHEN {
        TURN { MOVE(player, MOVE_INFESTED_TERRAIN); }
        TURN { MOVE(player, MOVE_HYPER_VOICE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[0].damage, results[1].damage);
    }
}
