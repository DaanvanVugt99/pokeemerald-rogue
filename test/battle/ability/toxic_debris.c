#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Toxic Debris sets Toxic Spikes on the opposing side if hit by a physical attack")
{
    u32 move;

    PARAMETRIZE { move = MOVE_TACKLE;}
    PARAMETRIZE { move = MOVE_SWIFT;}

    GIVEN {
        PLAYER(SPECIES_GLIMMORA) { Ability(ABILITY_TOXIC_DEBRIS); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, move); }
    } SCENE {
        if (move == MOVE_TACKLE) {
            ABILITY_POPUP(player, ABILITY_TOXIC_DEBRIS);
            MESSAGE("Toxic Spikes were scattered all around the opposing side!");
        } else {
            NONE_OF {
                ABILITY_POPUP(player, ABILITY_TOXIC_DEBRIS);
                MESSAGE("Toxic Spikes were scattered all around the opposing side!");
            }
        }
    }
}

SINGLE_BATTLE_TEST("Toxic Debris does not activate if two layers of Toxic Spikes are already up")
{
    GIVEN {
        PLAYER(SPECIES_GLIMMORA) { Ability(ABILITY_TOXIC_DEBRIS); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ABILITY_POPUP(player, ABILITY_TOXIC_DEBRIS);
        MESSAGE("Toxic Spikes were scattered all around the opposing side!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ABILITY_POPUP(player, ABILITY_TOXIC_DEBRIS);
        MESSAGE("Toxic Spikes were scattered all around the opposing side!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_TOXIC_DEBRIS);
            MESSAGE("Toxic Spikes were scattered all around the opposing side!");
        }
    }
}

SINGLE_BATTLE_TEST("If a Substitute is hit, Toxic Debris does not set Toxic Spikes")
{
    GIVEN {
        PLAYER(SPECIES_GLIMMORA) { Ability(ABILITY_TOXIC_DEBRIS); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SUBSTITUTE); }
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUBSTITUTE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_TOXIC_DEBRIS);
            MESSAGE("Toxic Spikes were scattered all around the opposing side!");
        }
    }
}

SINGLE_BATTLE_TEST("Each hit of a Multi Hit move activates Toxic Debris")
{
    GIVEN {
        PLAYER(SPECIES_GLIMMORA) { Ability(ABILITY_TOXIC_DEBRIS); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_FURY_SWIPES); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FURY_SWIPES, opponent);
        ABILITY_POPUP(player, ABILITY_TOXIC_DEBRIS);
        MESSAGE("Toxic Spikes were scattered all around the opposing side!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FURY_SWIPES, opponent);
        ABILITY_POPUP(player, ABILITY_TOXIC_DEBRIS);
        MESSAGE("Toxic Spikes were scattered all around the opposing side!");
    }
}

SINGLE_BATTLE_TEST("Toxic Debris activates if user faints after physical hit")
{
    GIVEN {
        PLAYER(SPECIES_GLIMMORA) { HP(1); Ability(ABILITY_TOXIC_DEBRIS); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        MESSAGE("Glimmora fainted!");
        ABILITY_POPUP(player, ABILITY_TOXIC_DEBRIS);
        MESSAGE("Toxic Spikes were scattered all around the opposing side!");
    }
}

SINGLE_BATTLE_TEST("Air Balloon is popped after Toxic Debris activates")
{
    GIVEN {
        PLAYER(SPECIES_GLIMMORA) { Ability(ABILITY_TOXIC_DEBRIS); Item(ITEM_AIR_BALLOON); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ABILITY_POPUP(player, ABILITY_TOXIC_DEBRIS);
        MESSAGE("Toxic Spikes were scattered all around the opposing side!");
        MESSAGE("Glimmora's Air Balloon popped!");
    }
}

DOUBLE_BATTLE_TEST("Toxic Debris sets Toxic Spikes on the opposing side even when hit by an ally")
{
    struct BattlePokemon *user = NULL;

    PARAMETRIZE { user = opponentLeft; }
    PARAMETRIZE { user = opponentRight; }
    PARAMETRIZE { user = playerRight; }
    GIVEN {
        PLAYER(SPECIES_GLIMMORA) { Ability(ABILITY_TOXIC_DEBRIS); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(user, MOVE_TACKLE, target: playerLeft); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_TOXIC_DEBRIS);
        MESSAGE("Toxic Spikes were scattered all around the opposing side!");
    }
}

DOUBLE_BATTLE_TEST("Toxic Debris does not activate if two layers of Toxic Spikes are already up on target side")
{
    GIVEN {
        PLAYER(SPECIES_GLIMMORA) { Ability(ABILITY_TOXIC_DEBRIS); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TOXIC_SPIKES);
            MOVE(playerRight, MOVE_TOXIC_SPIKES);
        }
        TURN { MOVE(playerRight, MOVE_TACKLE, target: playerLeft); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TOXIC_SPIKES, playerLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TOXIC_SPIKES, playerRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, playerRight);
        NONE_OF {
            ABILITY_POPUP(playerLeft, ABILITY_TOXIC_DEBRIS);
            MESSAGE("Toxic Spikes were scattered all around the opposing side!");
        }
    }
}
