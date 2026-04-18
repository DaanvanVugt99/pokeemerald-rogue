#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_RECOVER].healBlockBanned);
    ASSUME(!gBattleMoves[MOVE_TACKLE].healBlockBanned);
    ASSUME(gBattleMoves[MOVE_HEAL_PULSE].healBlockBanned);
}

SINGLE_BATTLE_TEST("Verdant Haven seeds the foe when the user uses a healing move")
{
    GIVEN {
        PLAYER(SPECIES_CHIKORITA)   { Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_VERDANT_HAVEN); Moves(MOVE_RECOVER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEECH_SEED, player);
    } THEN {
        EXPECT(gStatuses3[B_POSITION_OPPONENT_LEFT] & STATUS3_LEECHSEED);
    }
}

SINGLE_BATTLE_TEST("Verdant Haven does not seed the foe on non-healing moves")
{
    GIVEN {
        PLAYER(SPECIES_CHIKORITA)   { Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_VERDANT_HAVEN); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(!(gStatuses3[B_POSITION_OPPONENT_LEFT] & STATUS3_LEECHSEED));
    }
}

SINGLE_BATTLE_TEST("Verdant Haven does not seed Grass-type foes")
{
    GIVEN {
        PLAYER(SPECIES_CHIKORITA)   { Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_VERDANT_HAVEN); Moves(MOVE_RECOVER); }
        OPPONENT(SPECIES_BULBASAUR) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(!(gStatuses3[B_POSITION_OPPONENT_LEFT] & STATUS3_LEECHSEED));
    }
}

SINGLE_BATTLE_TEST("Verdant Haven does not reapply if the foe is already seeded")
{
    GIVEN {
        PLAYER(SPECIES_CHIKORITA)   { Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_VERDANT_HAVEN); Moves(MOVE_LEECH_SEED, MOVE_RECOVER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_LEECH_SEED); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gStatuses3[B_POSITION_OPPONENT_LEFT] & STATUS3_LEECHSEED);
    }
}

DOUBLE_BATTLE_TEST("Verdant Haven seeds a foe when the user uses Heal Pulse on its ally")
{
    GIVEN {
        PLAYER(SPECIES_MEGANIUM)     { Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_VERDANT_HAVEN); Moves(MOVE_HEAL_PULSE); }
        PLAYER(SPECIES_WOBBUFFET)    { HP(50); MaxHP(100); Moves(MOVE_CELEBRATE); }

        OPPONENT(SPECIES_WOBBUFFET)  { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET)  { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_HEAL_PULSE, target: playerRight); }
    } THEN {
        EXPECT(gStatuses3[B_POSITION_OPPONENT_LEFT] & STATUS3_LEECHSEED);
    }
}
