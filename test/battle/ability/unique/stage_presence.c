#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_TACKLE].effect != EFFECT_HIDDEN_POWER);
    ASSUME(gItems[ITEM_AIR_BALLOON].holdEffect == HOLD_EFFECT_AIR_BALLOON);
}

SINGLE_BATTLE_TEST("Stage Presence sets Psychic Terrain on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_INDEEDEE) { Ability(ABILITY_INNER_FOCUS); UniqueAbility(ABILITY_STAGE_PRESENCE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_STAGE_PRESENCE);
        MESSAGE("The battlefield got weird!");
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_PSYCHIC_TERRAIN);
    }
}

SINGLE_BATTLE_TEST("Stage Presence changes Normal moves to Psychic while affected by Psychic Terrain", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_AIR_BALLOON; }

    GIVEN {
        PLAYER(SPECIES_INDEEDEE) { Attack(100); Item(item); Ability(ABILITY_INNER_FOCUS); UniqueAbility(ABILITY_STAGE_PRESENCE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_MACHOP) { HP(500); MaxHP(500); Defense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_STAGE_PRESENCE);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Stage Presence moves Indeedee hidden abilities from Psychic Surge to Telepathy")
{
    GIVEN {
        PLAYER(SPECIES_INDEEDEE) { Ability(ABILITY_INNER_FOCUS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_INDEEDEE_MALE].abilities[2], ABILITY_TELEPATHY);
        EXPECT_EQ(gSpeciesInfo[SPECIES_INDEEDEE_FEMALE].abilities[2], ABILITY_TELEPATHY);
    }
}
