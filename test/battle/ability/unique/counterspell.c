#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_SWORDS_DANCE));
    ASSUME(!IS_MOVE_STATUS(MOVE_TACKLE));
    ASSUME(gBattleMoves[MOVE_EERIE_SPELL].effect == EFFECT_EERIE_SPELL);
    ASSUME(gBattleMoves[MOVE_EERIE_SPELL].power == 80);
}

SINGLE_BATTLE_TEST("Counterspell answers a successful opposing status move with a 20 BP Eerie Spell")
{
    s16 damage;

    GIVEN {
        PLAYER(SPECIES_SLOWKING_GALARIAN) { Speed(1); Ability(ABILITY_OWN_TEMPO); UniqueAbility(ABILITY_COUNTERSPELL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(100); MovesWithPP({MOVE_SWORDS_DANCE, 20}); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SWORDS_DANCE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWORDS_DANCE, opponent);
        ABILITY_POPUP(player, ABILITY_COUNTERSPELL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EERIE_SPELL, player);
        HP_BAR(opponent, captureDamage: &damage);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    } THEN {
        EXPECT_EQ(gBattleMovePower, 20);
        EXPECT_GT(damage, 0);
        EXPECT_EQ(opponent->pp[0], 16);
    }
}

SINGLE_BATTLE_TEST("Counterspell does not answer an opposing damaging move")
{
    GIVEN {
        PLAYER(SPECIES_SLOWKING_GALARIAN) { Ability(ABILITY_OWN_TEMPO); UniqueAbility(ABILITY_COUNTERSPELL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_COUNTERSPELL);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_EERIE_SPELL, player);
        }
    }
}

SINGLE_BATTLE_TEST("Counterspell does not answer a failed opposing status move")
{
    GIVEN {
        PLAYER(SPECIES_SKARMORY) { Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_COUNTERSPELL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TOXIC); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TOXIC); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_COUNTERSPELL);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_EERIE_SPELL, player);
        }
    }
}

SINGLE_BATTLE_TEST("Protect blocks Counterspell's Eerie Spell")
{
    GIVEN {
        PLAYER(SPECIES_SLOWKING_GALARIAN) { Ability(ABILITY_OWN_TEMPO); UniqueAbility(ABILITY_COUNTERSPELL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); MovesWithPP({MOVE_PROTECT, 10}); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_PROTECT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        ABILITY_POPUP(player, ABILITY_COUNTERSPELL);
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_EERIE_SPELL, player);
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP);
        EXPECT_EQ(opponent->pp[0], 9);
    }
}
