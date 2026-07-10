#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_HYPNOSIS].accuracy < 100);
    ASSUME(gBattleMoves[MOVE_BITE].type == TYPE_DARK);
    ASSUME(gSpeciesInfo[SPECIES_NOCTOWL].types[0] == TYPE_PSYCHIC);
    ASSUME(gSpeciesInfo[SPECIES_NOCTOWL].types[1] == TYPE_FLYING);
}

SINGLE_BATTLE_TEST("Nocturnal makes Hypnosis perfectly accurate during Eclipse")
{
    GIVEN {
        PLAYER(SPECIES_NOCTOWL)     { Speed(50); Ability(ABILITY_INSOMNIA); UniqueAbility(ABILITY_NOCTURNAL); Moves(MOVE_HYPNOSIS, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_ECLIPSE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_ECLIPSE); }
        TURN { MOVE(player, MOVE_HYPNOSIS, WITH_RNG(RNG_ACCURACY, 99)); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(opponent->status1 & STATUS1_SLEEP);
    }
}

SINGLE_BATTLE_TEST("Nocturnal grants Dark immunity during Eclipse")
{
    GIVEN {
        PLAYER(SPECIES_NOCTOWL)     { HP(200); MaxHP(200); Ability(ABILITY_INSOMNIA); UniqueAbility(ABILITY_NOCTURNAL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_ECLIPSE, MOVE_BITE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_ECLIPSE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_BITE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_NOCTURNAL);
        NONE_OF { HP_BAR(player); }
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Nocturnal does not grant Dark immunity outside Eclipse")
{
    GIVEN {
        PLAYER(SPECIES_NOCTOWL)     { HP(200); MaxHP(200); Ability(ABILITY_INSOMNIA); UniqueAbility(ABILITY_NOCTURNAL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_BITE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_BITE); }
    } SCENE {
        HP_BAR(player);
    } THEN {
        EXPECT_LT(player->hp, player->maxHP);
    }
}
