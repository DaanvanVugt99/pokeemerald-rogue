#include "global.h"
#include "test/battle.h"

#if B_EXPANDED_MOVE_NAMES
#define CORROSIVE_GAS_NAME "Corrosive Gas"
#else
#define CORROSIVE_GAS_NAME "CorrosiveGas"
#endif

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ROUND].split == SPLIT_SPECIAL);
    ASSUME(gBattleMoves[MOVE_TRICK].effect == EFFECT_TRICK);
    ASSUME(gBattleMoves[MOVE_FLING].effect == EFFECT_FLING);
    ASSUME(gBattleMoves[MOVE_KNOCK_OFF].effect == EFFECT_KNOCK_OFF);
    ASSUME(gBattleMoves[MOVE_CORROSIVE_GAS].effect == EFFECT_CORROSIVE_GAS);
    ASSUME(gBattleMoves[MOVE_RECYCLE].effect == EFFECT_RECYCLE);
}

SINGLE_BATTLE_TEST("Booster Energy activates Protosynthesis and Quark Drive outside their field condition")
{
    u16 ability;
    PARAMETRIZE { ability = ABILITY_PROTOSYNTHESIS; }
    PARAMETRIZE { ability = ABILITY_QUARK_DRIVE; }

    GIVEN {
        PLAYER(SPECIES_ABRA) { Ability(ability); Item(ITEM_BOOSTER_ENERGY); Moves(MOVE_ROUND); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_ROUND); }
    } SCENE {
        if (ability == ABILITY_PROTOSYNTHESIS) {
            MESSAGE("Abra's Booster Energy activated its Protosynthesis!");
            ABILITY_POPUP(player, ABILITY_PROTOSYNTHESIS);
        } else {
            MESSAGE("Abra's Booster Energy activated its Quark Drive!");
            ABILITY_POPUP(player, ABILITY_QUARK_DRIVE);
        }
        MESSAGE("Abra's Sp. Atk was heightened!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROUND, player);
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Booster Energy is not consumed again after Recycle while already active")
{
    GIVEN {
        PLAYER(SPECIES_ABRA) { Ability(ABILITY_PROTOSYNTHESIS); Item(ITEM_BOOSTER_ENERGY); Moves(MOVE_ROUND, MOVE_RECYCLE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_ROUND); }
        TURN { MOVE(player, MOVE_RECYCLE); }
    } SCENE {
        MESSAGE("Abra's Booster Energy activated its Protosynthesis!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROUND, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECYCLE, player);
        NONE_OF {
            MESSAGE("Abra's Booster Energy activated its Protosynthesis!");
        }
    } THEN {
        EXPECT_EQ(player->item, ITEM_BOOSTER_ENERGY);
    }
}

SINGLE_BATTLE_TEST("Booster Energy gives Protosynthesis its damage boost", s16 damage)
{
    u16 item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_BOOSTER_ENERGY; }

    GIVEN {
        PLAYER(SPECIES_ABRA) { Ability(ABILITY_PROTOSYNTHESIS); Item(item); Moves(MOVE_ROUND); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_ROUND); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROUND, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.3), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Booster Energy is not consumed while the field condition activates the ability")
{
    GIVEN {
        PLAYER(SPECIES_ABRA) { Ability(ABILITY_PROTOSYNTHESIS); Item(ITEM_BOOSTER_ENERGY); Moves(MOVE_ROUND); }
        OPPONENT(SPECIES_NINETALES) { Ability(ABILITY_DROUGHT); }
    } WHEN {
        TURN { MOVE(player, MOVE_ROUND); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_DROUGHT);
        ABILITY_POPUP(player, ABILITY_PROTOSYNTHESIS);
        MESSAGE("The harsh sunlight activated Abra's Protosynthesis!");
        MESSAGE("Abra's Sp. Atk was heightened!");
        NONE_OF {
            MESSAGE("Abra's Booster Energy activated its Protosynthesis!");
        }
    } THEN {
        EXPECT_EQ(player->item, ITEM_BOOSTER_ENERGY);
    }
}

SINGLE_BATTLE_TEST("Booster Energy activates after its field condition ends")
{
    u16 ability, opponentAbility, turnLimit, turns;
    PARAMETRIZE { ability = ABILITY_PROTOSYNTHESIS; opponentAbility = ABILITY_DROUGHT; turnLimit = WEATHER_DURATION_TURNS; }
    PARAMETRIZE { ability = ABILITY_QUARK_DRIVE; opponentAbility = ABILITY_ELECTRIC_SURGE; turnLimit = TERRAIN_DURATION_TURNS; }

    GIVEN {
        PLAYER(SPECIES_ABRA) { Ability(ability); Item(ITEM_BOOSTER_ENERGY); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(opponentAbility); }
    } WHEN {
        for (turns = 0; turns < turnLimit; turns++)
            TURN {}
    } SCENE {
        ABILITY_POPUP(opponent, opponentAbility);
        if (ability == ABILITY_PROTOSYNTHESIS) {
            ABILITY_POPUP(player, ABILITY_PROTOSYNTHESIS);
            MESSAGE("The harsh sunlight activated Abra's Protosynthesis!");
            MESSAGE("Abra's Sp. Atk was heightened!");
            MESSAGE("The sunlight faded.");
            MESSAGE("Abra's Booster Energy activated its Protosynthesis!");
            ABILITY_POPUP(player, ABILITY_PROTOSYNTHESIS);
        } else {
            ABILITY_POPUP(player, ABILITY_QUARK_DRIVE);
            MESSAGE("The Electric Terrain activated Abra's Quark Drive!");
            MESSAGE("Abra's Sp. Atk was heightened!");
            MESSAGE("The electricity disappeared from the battlefield.");
            MESSAGE("Abra's Booster Energy activated its Quark Drive!");
            ABILITY_POPUP(player, ABILITY_QUARK_DRIVE);
        }
        MESSAGE("Abra's Sp. Atk was heightened!");
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Booster Energy cannot be transferred to or from Paradox Pokemon with Trick")
{
    u16 playerSpecies, playerItem, opponentSpecies, opponentItem;
    PARAMETRIZE { playerSpecies = SPECIES_GREAT_TUSK; playerItem = ITEM_BOOSTER_ENERGY; opponentSpecies = SPECIES_WOBBUFFET; opponentItem = ITEM_ORAN_BERRY; }
    PARAMETRIZE { playerSpecies = SPECIES_WOBBUFFET; playerItem = ITEM_ORAN_BERRY; opponentSpecies = SPECIES_GREAT_TUSK; opponentItem = ITEM_BOOSTER_ENERGY; }

    GIVEN {
        PLAYER(playerSpecies) { Ability(ABILITY_OVERGROW); Item(playerItem); Moves(MOVE_TRICK); }
        OPPONENT(opponentSpecies) { Ability(ABILITY_OVERGROW); Item(opponentItem); }
    } WHEN {
        TURN { MOVE(player, MOVE_TRICK); }
    } THEN {
        EXPECT_EQ(player->item, playerItem);
        EXPECT_EQ(opponent->item, opponentItem);
    }
}

SINGLE_BATTLE_TEST("Booster Energy can be transferred by Koraidon and Miraidon with Trick")
{
    u16 species;
    PARAMETRIZE { species = SPECIES_KORAIDON; }
    PARAMETRIZE { species = SPECIES_MIRAIDON; }

    GIVEN {
        PLAYER(species) { Ability(ABILITY_OVERGROW); Item(ITEM_BOOSTER_ENERGY); Moves(MOVE_TRICK); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_ORAN_BERRY); }
    } WHEN {
        TURN { MOVE(player, MOVE_TRICK); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_ORAN_BERRY);
        EXPECT_EQ(opponent->item, ITEM_BOOSTER_ENERGY);
    }
}

SINGLE_BATTLE_TEST("Booster Energy cannot be used with Fling by Paradox Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_GREAT_TUSK) { Ability(ABILITY_OVERGROW); Item(ITEM_BOOSTER_ENERGY); Moves(MOVE_FLING); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_FLING); }
    } SCENE {
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FLING, player);
        }
    } THEN {
        EXPECT_EQ(player->item, ITEM_BOOSTER_ENERGY);
    }
}

SINGLE_BATTLE_TEST("Booster Energy cannot be removed from Paradox Pokemon by item-removal moves")
{
    u16 move;
    PARAMETRIZE { move = MOVE_KNOCK_OFF; }
    PARAMETRIZE { move = MOVE_CORROSIVE_GAS; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(move); }
        OPPONENT(SPECIES_GREAT_TUSK) { Ability(ABILITY_OVERGROW); Item(ITEM_BOOSTER_ENERGY); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        if (move == MOVE_KNOCK_OFF)
            ANIMATION(ANIM_TYPE_MOVE, MOVE_KNOCK_OFF, player);
        else
            MESSAGE("Wobbuffet used " CORROSIVE_GAS_NAME "!");
        NONE_OF {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_ITEM_KNOCKOFF);
            MESSAGE("Wobbuffet knocked off Foe Great Tusk's Booster Energy!");
            MESSAGE("Wobbuffet corroded Foe Great Tusk's Booster Energy!");
        }
    } THEN {
        EXPECT_EQ(opponent->item, ITEM_BOOSTER_ENERGY);
    }
}

SINGLE_BATTLE_TEST("Booster Energy does not boost Knock Off when locked to Paradox Pokemon", s16 damage)
{
    u16 item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_BOOSTER_ENERGY; }

    GIVEN {
        PLAYER(SPECIES_GREAT_TUSK) { Ability(ABILITY_OVERGROW); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_KNOCK_OFF); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_KNOCK_OFF); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_KNOCK_OFF, opponent);
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}
