#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Thick Skull prevents a knockout from full HP")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_THICK_SKULL); MaxHP(100); HP(100); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SEISMIC_TOSS); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SEISMIC_TOSS, opponent);
        HP_BAR(player, hp: 1);
        ABILITY_POPUP(player, ABILITY_THICK_SKULL);
        MESSAGE("Wobbuffet endured the hit using Thick Skull!");
    }
}

SINGLE_BATTLE_TEST("Thick Skull does not prevent a knockout below full HP")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_THICK_SKULL); MaxHP(100); HP(99); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SEISMIC_TOSS); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SEISMIC_TOSS, opponent);
        HP_BAR(player, hp: 0);
    }
}

SINGLE_BATTLE_TEST("Thick Skull prevents OHKO moves")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_FISSURE].effect == EFFECT_OHKO);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_THICK_SKULL); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_FISSURE); }
    } SCENE {
        MESSAGE("Foe Wobbuffet used Fissure!");
        ABILITY_POPUP(player, ABILITY_THICK_SKULL);
        MESSAGE("Wobbuffet was protected by Thick Skull!");
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Thick Skull prevents flinching")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_THICK_SKULL); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FAKE_OUT); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FAKE_OUT, player);
        ABILITY_POPUP(opponent, ABILITY_THICK_SKULL);
        NONE_OF { MESSAGE("Foe Wobbuffet flinched!"); }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
    }
}

SINGLE_BATTLE_TEST("Thick Skull knockout prevention is ignored by Mold Breaker")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_THICK_SKULL); MaxHP(100); HP(100); }
        OPPONENT(SPECIES_PINSIR) { Ability(ABILITY_MOLD_BREAKER); Moves(MOVE_SEISMIC_TOSS); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SEISMIC_TOSS); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_MOLD_BREAKER);
        MESSAGE("Foe Pinsir breaks the mold!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SEISMIC_TOSS, opponent);
        HP_BAR(player, hp: 0);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_THICK_SKULL);
            MESSAGE("Wobbuffet endured the hit using Thick Skull!");
        }
    }
}

SINGLE_BATTLE_TEST("Thick Skull OHKO prevention is ignored by Mold Breaker")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_FISSURE].effect == EFFECT_OHKO);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_THICK_SKULL); }
        OPPONENT(SPECIES_PINSIR) { Ability(ABILITY_MOLD_BREAKER); Moves(MOVE_FISSURE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_FISSURE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_MOLD_BREAKER);
        MESSAGE("Foe Pinsir breaks the mold!");
        MESSAGE("Foe Pinsir used Fissure!");
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_THICK_SKULL);
            MESSAGE("Wobbuffet was protected by Thick Skull!");
        }
    }
}

SINGLE_BATTLE_TEST("Thick Skull flinch prevention is ignored by Mold Breaker")
{
    GIVEN {
        PLAYER(SPECIES_PINSIR) { Ability(ABILITY_MOLD_BREAKER); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_THICK_SKULL); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FAKE_OUT); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MOLD_BREAKER);
        MESSAGE("Pinsir breaks the mold!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FAKE_OUT, player);
        MESSAGE("Foe Wobbuffet flinched!");
        NONE_OF {
            ABILITY_POPUP(opponent, ABILITY_THICK_SKULL);
        }
    }
}
