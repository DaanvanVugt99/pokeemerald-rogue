#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Fairy Absorb heals 25 percent when hit by Fairy-type moves")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_FAIRY_WIND].type == TYPE_FAIRY);
        PLAYER(SPECIES_SPIRITOMB) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_FAIRY_ABSORB); HP(1); MaxHP(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_FAIRY_WIND); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_FAIRY_WIND); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FAIRY_ABSORB);
        HP_BAR(player, damage: -25);
    }
}

SINGLE_BATTLE_TEST("Fairy Absorb activates on Fairy-type status moves")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_BABY_DOLL_EYES].type == TYPE_FAIRY);
        ASSUME(gBattleMoves[MOVE_BABY_DOLL_EYES].split == SPLIT_STATUS);
        PLAYER(SPECIES_SPIRITOMB) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_FAIRY_ABSORB); HP(1); MaxHP(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_BABY_DOLL_EYES); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_BABY_DOLL_EYES); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FAIRY_ABSORB);
        HP_BAR(player, damage: -25);
        NONE_OF { MESSAGE("Spiritomb's Attack fell!"); }
    }
}

SINGLE_BATTLE_TEST("Fairy Absorb does not activate if protected")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_FAIRY_WIND].type == TYPE_FAIRY);
        PLAYER(SPECIES_SPIRITOMB) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_FAIRY_ABSORB); HP(1); MaxHP(100); Moves(MOVE_PROTECT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_FAIRY_WIND); }
    } WHEN {
        TURN { MOVE(player, MOVE_PROTECT); MOVE(opponent, MOVE_FAIRY_WIND); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_FAIRY_ABSORB);
            HP_BAR(player);
        }
    }
}

SINGLE_BATTLE_TEST("Fairy Absorb is assigned to Spiritomb")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SPIRITOMB) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_SPIRITOMB), ABILITY_FAIRY_ABSORB);
    }
}
