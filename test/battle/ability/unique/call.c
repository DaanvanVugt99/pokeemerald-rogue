#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_GROWL].split == SPLIT_STATUS);
}

SINGLE_BATTLE_TEST("Call gives +1 priority to status moves if the party contains a Fairy-type Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_VOLBEAT) { Speed(50); Ability(ABILITY_ILLUMINATE); UniqueAbility(ABILITY_CALL); Moves(MOVE_GROWL); }
        PLAYER(SPECIES_CLEFAIRY) { Speed(1); Ability(ABILITY_CUTE_CHARM); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("Call does not give +1 priority without a Fairy-type Pokemon in the party")
{
    GIVEN {
        PLAYER(SPECIES_VOLBEAT) { Speed(50); Ability(ABILITY_ILLUMINATE); UniqueAbility(ABILITY_CALL); Moves(MOVE_GROWL); }
        PLAYER(SPECIES_PIKACHU) { Speed(1); Ability(ABILITY_STATIC); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
    }
}

SINGLE_BATTLE_TEST("Call still affects Dark-type targets")
{
    GIVEN {
        PLAYER(SPECIES_VOLBEAT) { Speed(50); Ability(ABILITY_ILLUMINATE); UniqueAbility(ABILITY_CALL); Moves(MOVE_GROWL); }
        PLAYER(SPECIES_CLEFAIRY) { Speed(1); Ability(ABILITY_CUTE_CHARM); }
        OPPONENT(SPECIES_UMBREON) { Speed(60); Ability(ABILITY_SYNCHRONIZE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    } THEN {
        EXPECT_LT(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}
