#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].strikeCount < 2);
}

SINGLE_BATTLE_TEST("Parental Bond popup parity: Brutal and Toxic Tandem stay silent")
{
    u16 species, ability, uniqueAbility;
    bool32 blocked;

    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_PARENTAL_BOND; uniqueAbility = ABILITY_NONE; blocked = FALSE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_PARENTAL_BOND; uniqueAbility = ABILITY_NONE; blocked = TRUE; }
    PARAMETRIZE { species = SPECIES_HAXORUS; ability = ABILITY_MOLD_BREAKER; uniqueAbility = ABILITY_BRUTAL; blocked = FALSE; }
    PARAMETRIZE { species = SPECIES_HAXORUS; ability = ABILITY_MOLD_BREAKER; uniqueAbility = ABILITY_BRUTAL; blocked = TRUE; }
    PARAMETRIZE { species = SPECIES_BEEDRILL; ability = ABILITY_SWARM; uniqueAbility = ABILITY_TOXIC_TANDEM; blocked = FALSE; }
    PARAMETRIZE { species = SPECIES_BEEDRILL; ability = ABILITY_SWARM; uniqueAbility = ABILITY_TOXIC_TANDEM; blocked = TRUE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_POISON_STING].type == TYPE_POISON);
        ASSUME(gBattleMoves[MOVE_POISON_STING].strikeCount < 2);
        PLAYER(species) { Ability(ability); UniqueAbility(uniqueAbility); Moves(MOVE_POISON_STING); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Moves(MOVE_CELEBRATE, MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(player, MOVE_POISON_STING); MOVE(opponent, blocked ? MOVE_PROTECT : MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_PARENTAL_BOND);
            ABILITY_POPUP(player, ABILITY_BRUTAL);
            ABILITY_POPUP(player, ABILITY_TOXIC_TANDEM);
        }
    } THEN {
        if (blocked)
            EXPECT_EQ(opponent->hp, 400);
        else
            EXPECT_LT(opponent->hp, 400);
    }
}

SINGLE_BATTLE_TEST("Brutal makes damaging moves hit twice")
{
    GIVEN {
        PLAYER(SPECIES_HAXORUS) { Ability(ABILITY_MOLD_BREAKER); UniqueAbility(ABILITY_BRUTAL); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Brutal second hit uses 0.25x damage", s16 hit1, s16 hit2)
{
    u16 uniqueAbility;
    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_BRUTAL; }

    GIVEN {
        PLAYER(SPECIES_HAXORUS) { Ability(ABILITY_MOLD_BREAKER); UniqueAbility(uniqueAbility); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent, captureDamage: &results[i].hit1);
        if (uniqueAbility == ABILITY_BRUTAL)
            HP_BAR(opponent, captureDamage: &results[i].hit2);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].hit1, Q_4_12(1.25), results[1].hit1 + results[1].hit2);
    }
}
