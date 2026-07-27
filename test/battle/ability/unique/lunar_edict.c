#include "global.h"
#include "pokemon.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_HEALING_WISH].effect == EFFECT_HEALING_WISH);
    ASSUME(gBattleMoves[MOVE_LUNAR_DANCE].effect == EFFECT_HEALING_WISH);
    ASSUME(gBattleMoves[MOVE_MISTY_TERRAIN].effect == EFFECT_MISTY_TERRAIN);
}

SINGLE_BATTLE_TEST("Lunar Edict makes Misty Terrain take Lunar Dance's sacrifice")
{
    GIVEN {
        ASSUME(B_HEALING_WISH_SWITCH >= GEN_5);
        PLAYER(SPECIES_CRESSELIA) {
            Speed(50);
            Ability(ABILITY_LEVITATE);
            UniqueAbility(ABILITY_LUNAR_EDICT);
            HP(100);
            MaxHP(100);
            Moves(MOVE_CELEBRATE, MOVE_LUNAR_DANCE);
        }
        PLAYER(SPECIES_WYNAUT) {
            Speed(25);
            HP(1);
            MaxHP(100);
            Status1(STATUS1_POISON);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_MISTY_TERRAIN, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_MISTY_TERRAIN); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_LUNAR_DANCE); MOVE(opponent, MOVE_CELEBRATE); SEND_OUT(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LUNAR_DANCE, player);
        ABILITY_POPUP(player, ABILITY_LUNAR_EDICT);
        MESSAGE("The mist disappeared from the battlefield.");
        NOT MESSAGE("Cresselia fainted!");
        MESSAGE("Wynaut became cloaked in mystical moonlight!");
        HP_BAR(player, hp: 100);
        STATUS_ICON(player, none: TRUE);
        MESSAGE("Wynaut regained health!");
    } THEN {
        EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_HP), 100);
        EXPECT(!(gFieldStatuses & STATUS_FIELD_MISTY_TERRAIN));
    }
}
