#include "global.h"
#include "test/battle.h"
#include "battle_ai_util.h"

AI_SINGLE_BATTLE_TEST("AI treats status moves as unusable with Vow of Silence")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_VOW_OF_SILENCE); Moves(MOVE_RECOVER, MOVE_TACKLE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_TACKLE); }
    }
}

AI_SINGLE_BATTLE_TEST("AI KO prediction recognizes Blood Oath survival")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { HP(20); MaxHP(100); Item(ITEM_BLOOD_OATH); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE, MOVE_SPLASH); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_DRAGON_RAGE, MOVE_SPLASH); }
    } THEN {
        gBattleResources->aiData->holdEffects[B_POSITION_PLAYER_LEFT] = HOLD_EFFECT_BLOOD_OATH;
        EXPECT(!CanAIFaintTarget(B_POSITION_OPPONENT_LEFT, B_POSITION_PLAYER_LEFT, 0));
    }
}

AI_SINGLE_BATTLE_TEST("AI understands Hollow Sun type inversion")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_TANGELA) { Item(ITEM_HOLLOW_SUN); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_EMBER, MOVE_WATER_GUN); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_WATER_GUN, MOVE_EMBER); }
    }
}

AI_SINGLE_BATTLE_TEST("AI uses Twin Goggles accuracy when comparing damaging moves")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_DYNAMIC_PUNCH].accuracy == 50);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_COMPOUND_GOGGLES); Moves(MOVE_DYNAMIC_PUNCH, MOVE_STRENGTH); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_DYNAMIC_PUNCH, MOVE_STRENGTH); }
    }
}

AI_SINGLE_BATTLE_TEST("AI accounts for Impact Plating after the item is revealed")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].power == gBattleMoves[MOVE_WATER_GUN].power);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Item(ITEM_IMPACT_PLATING); Moves(MOVE_DRAGON_RAGE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(100); HP(1); MaxHP(1); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TACKLE, MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RAGE); EXPECT_MOVE(opponent, MOVE_TACKLE); EXPECT_SEND_OUT(opponent, 1); }
        TURN { MOVE(player, MOVE_CELEBRATE); SCORE_GT(opponent, MOVE_WATER_GUN, MOVE_TACKLE); }
    }
}

AI_SINGLE_BATTLE_TEST("AI is less eager to set up against revealed Greedy Gloves")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Item(ITEM_GREEDY_GLOVES); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(100); HP(1); MaxHP(1); Moves(MOVE_DEFENSE_CURL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_SWORDS_DANCE, MOVE_TACKLE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_DEFENSE_CURL); MOVE(player, MOVE_TACKLE); EXPECT_SEND_OUT(opponent, 1); }
        TURN { MOVE(player, MOVE_CELEBRATE); SCORE_GT(opponent, MOVE_TACKLE, MOVE_SWORDS_DANCE); }
    }
}

AI_SINGLE_BATTLE_TEST("AI reduces setup value against known Spectral Thief like Greedy Gloves")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_MARSHADOW) { Speed(50); Moves(MOVE_SPECTRAL_THIEF, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(100); HP(1); MaxHP(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_SWORDS_DANCE, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPECTRAL_THIEF); EXPECT_MOVE(opponent, MOVE_CELEBRATE); EXPECT_SEND_OUT(opponent, 1); }
        TURN { MOVE(player, MOVE_CELEBRATE); SCORE_LT_VAL(opponent, MOVE_SWORDS_DANCE, AI_SCORE_DEFAULT); }
    }
}

AI_SINGLE_BATTLE_TEST("Treasure AI audit: Wonder Shield blocks super-effective damage predictions while unused")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_BUG_BITE].type == TYPE_BUG);
        ASSUME(gBattleMoves[MOVE_BUG_BITE].power > gBattleMoves[MOVE_TACKLE].power);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_WONDER_SHIELD); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_BUG_BITE, MOVE_TACKLE); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_TACKLE, MOVE_BUG_BITE); }
    }
}

AI_SINGLE_BATTLE_TEST("Treasure AI audit: weather totems encourage their matching weather")
{
    u32 item, weatherMove;

    PARAMETRIZE { item = ITEM_RAIN_TOTEM; weatherMove = MOVE_RAIN_DANCE; }
    PARAMETRIZE { item = ITEM_ACID_RAIN_TOTEM; weatherMove = MOVE_ACID_RAIN; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_ACID_RAIN].effect == EFFECT_CORROSIVE_CLOUDS);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(item); Moves(weatherMove, MOVE_SPLASH); }
    } WHEN {
        TURN { SCORE_GT(opponent, weatherMove, MOVE_SPLASH); }
    }
}

AI_SINGLE_BATTLE_TEST("Treasure AI audit: Electric Tiki adds value to its priority trigger")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_QUICK_ATTACK].power == gBattleMoves[MOVE_TACKLE].power);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_ELECTRIC_TIKI); Moves(MOVE_QUICK_ATTACK, MOVE_TACKLE); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_QUICK_ATTACK, MOVE_TACKLE); }
    }
}

AI_SINGLE_BATTLE_TEST("Treasure AI audit: Witch's Thread accounts for reflected status")
{
    GIVEN {
        ASSUME(CanBePoisoned(B_POSITION_OPPONENT_LEFT, B_POSITION_OPPONENT_LEFT));
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_WITCHS_THREAD); Moves(MOVE_TOXIC, MOVE_TACKLE); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_TACKLE, MOVE_TOXIC); }
    }
}

AI_SINGLE_BATTLE_TEST("AI gets baited by Protect Switch tactics") // This behavior is to be fixed.
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_SWITCHING);
        PLAYER(SPECIES_STUNFISK);
        PLAYER(SPECIES_PELIPPER);
        OPPONENT(SPECIES_DARKRAI) { Moves(MOVE_TACKLE, MOVE_PECK, MOVE_EARTHQUAKE, MOVE_THUNDERBOLT); }
        OPPONENT(SPECIES_SCIZOR) { Moves(MOVE_HYPER_BEAM, MOVE_FACADE, MOVE_GIGA_IMPACT, MOVE_EXTREME_SPEED); }
    } WHEN {

        TURN { MOVE(player, MOVE_PROTECT);  EXPECT_MOVE(opponent, MOVE_EARTHQUAKE); } // E-quake
        TURN { SWITCH(player, 1);           EXPECT_MOVE(opponent, MOVE_EARTHQUAKE); } // E-quake
        TURN { MOVE(player, MOVE_PROTECT);  EXPECT_MOVE(opponent, MOVE_THUNDERBOLT); } // T-Bolt
        TURN { SWITCH(player, 0);           EXPECT_MOVE(opponent, MOVE_THUNDERBOLT); } // T-Bolt
        TURN { MOVE(player, MOVE_PROTECT);  EXPECT_MOVE(opponent, MOVE_EARTHQUAKE); } // E-quake
        TURN { SWITCH(player, 1);           EXPECT_MOVE(opponent, MOVE_EARTHQUAKE);} // E-quake
        TURN { MOVE(player, MOVE_PROTECT);  EXPECT_MOVE(opponent, MOVE_THUNDERBOLT); } // T-Bolt
    }
}

AI_SINGLE_BATTLE_TEST("AI prefers Bubble over Water Gun if it's slower")
{
    u32 speedPlayer, speedAi;

    PARAMETRIZE { speedPlayer = 200; speedAi = 10; }
    PARAMETRIZE { speedPlayer = 10; speedAi = 200; }

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_SCIZOR) { Speed(speedPlayer); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN, MOVE_BUBBLE); Speed(speedAi); }
    } WHEN {
        if (speedPlayer > speedAi)
        {
            TURN { SCORE_GT(opponent, MOVE_BUBBLE, MOVE_WATER_GUN); }
            TURN { SCORE_GT(opponent, MOVE_BUBBLE, MOVE_WATER_GUN); }
        }
        else
        {
            TURN { SCORE_EQ(opponent, MOVE_BUBBLE, MOVE_WATER_GUN); }
            TURN { SCORE_EQ(opponent, MOVE_BUBBLE, MOVE_WATER_GUN); }
        }
    }
}

AI_SINGLE_BATTLE_TEST("AI prefers Water Gun over Bubble if it knows that foe has Contrary")
{
    u32 abilityAI;

    PARAMETRIZE { abilityAI = ABILITY_MOXIE; }
    PARAMETRIZE { abilityAI = ABILITY_MOLD_BREAKER; } // Mold Breaker ignores Contrary.
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_SHUCKLE) { Ability(ABILITY_CONTRARY); }
        OPPONENT(SPECIES_PINSIR) { Moves(MOVE_WATER_GUN, MOVE_BUBBLE); Ability(abilityAI); }
    } WHEN {
            TURN { MOVE(player, MOVE_DEFENSE_CURL); }
            TURN { MOVE(player, MOVE_DEFENSE_CURL);
                   if (abilityAI == ABILITY_MOLD_BREAKER) { SCORE_EQ(opponent, MOVE_WATER_GUN, MOVE_BUBBLE); }
                   else { SCORE_GT(opponent, MOVE_WATER_GUN, MOVE_BUBBLE); }}
    } SCENE {
        MESSAGE("Shuckle's Defense fell!"); // Contrary activates
    } THEN {
        EXPECT(gBattleResources->aiData->abilities[B_POSITION_PLAYER_LEFT] == ABILITY_CONTRARY);
    }
}

AI_SINGLE_BATTLE_TEST("AI does not use Rest if it has Insomnia")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_WOBBUFFET) { HP(200); }
        OPPONENT(SPECIES_DROWZEE) { Ability(ABILITY_INSOMNIA); HP(40); MaxHP(120); Moves(MOVE_REST, MOVE_TACKLE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_TACKLE); }
    }
}

AI_SINGLE_BATTLE_TEST("AI does not use Rest if it has unique Insomnia")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_WOBBUFFET) { HP(200); }
        OPPONENT(SPECIES_WOBBUFFET) { UniqueAbility(ABILITY_INSOMNIA); HP(40); MaxHP(120); Moves(MOVE_REST, MOVE_TACKLE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_TACKLE); }
    }
}

AI_SINGLE_BATTLE_TEST("AI does not choose healing moves against Bog Body")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_CLODSIRE) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(40); MaxHP(120); Moves(MOVE_RECOVER, MOVE_TACKLE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_TACKLE); }
    }
}

AI_SINGLE_BATTLE_TEST("AI does not use Power Trick with Ancient Idol")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_POWER_TRICK].effect == EFFECT_POWER_TRICK);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_WOBBUFFET) { HP(200); }
        OPPONENT(SPECIES_WOBBUFFET) {
            Attack(50); Defense(100); UniqueAbility(ABILITY_ANCIENT_IDOL); Moves(MOVE_POWER_TRICK, MOVE_TACKLE);
        }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_TACKLE); }
    }
}

AI_SINGLE_BATTLE_TEST("AI scores Confuse Ray below Tackle against revealed Own Tempo")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_CONFUSE_RAY].effect == EFFECT_CONFUSE);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_OWN_TEMPO); }
        OPPONENT(SPECIES_ARBOK) { Ability(ABILITY_INTIMIDATE); Moves(MOVE_CONFUSE_RAY, MOVE_TACKLE); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_TACKLE, MOVE_CONFUSE_RAY); }
        TURN { SCORE_GT(opponent, MOVE_TACKLE, MOVE_CONFUSE_RAY); }
    }
}

AI_SINGLE_BATTLE_TEST("AI scores Confuse Ray below Tackle against revealed unique Own Tempo")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_CONFUSE_RAY].effect == EFFECT_CONFUSE);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_WOBBUFFET) { UniqueAbility(ABILITY_OWN_TEMPO); }
        OPPONENT(SPECIES_ARBOK) { Ability(ABILITY_INTIMIDATE); Moves(MOVE_CONFUSE_RAY, MOVE_TACKLE); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_TACKLE, MOVE_CONFUSE_RAY); }
        TURN { SCORE_GT(opponent, MOVE_TACKLE, MOVE_CONFUSE_RAY); }
    }
}

AI_SINGLE_BATTLE_TEST("AI scores Knock Off below Tackle against known Sticky Hold")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_STICKY_HOLD); Item(ITEM_LEFTOVERS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MACHAMP) { Moves(MOVE_KNOCK_OFF, MOVE_TACKLE); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_TACKLE, MOVE_KNOCK_OFF); }
    }
}

AI_SINGLE_BATTLE_TEST("AI scores Knock Off below Tackle against revealed unique Sticky Hold")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_WOBBUFFET) { UniqueAbility(ABILITY_STICKY_HOLD); Item(ITEM_LEFTOVERS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MACHAMP) { Moves(MOVE_KNOCK_OFF, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_KNOCK_OFF); }
        TURN { SCORE_GT(opponent, MOVE_TACKLE, MOVE_KNOCK_OFF); }
    }
}

AI_SINGLE_BATTLE_TEST("AI scores absorbed moves below safe moves against known unique absorbers")
{
    u16 species, ability, absorbedMove, safeMove;

    PARAMETRIZE { species = SPECIES_PALOSSAND; ability = ABILITY_LOW_TIDE; absorbedMove = MOVE_WATER_GUN; safeMove = MOVE_TACKLE; }
    PARAMETRIZE { species = SPECIES_TURTONATOR; ability = ABILITY_MORTAR_SHELL; absorbedMove = MOVE_ROCK_THROW; safeMove = MOVE_TACKLE; }
    PARAMETRIZE { species = SPECIES_HEATMOR; ability = ABILITY_INSECTIVORE; absorbedMove = MOVE_FURY_CUTTER; safeMove = MOVE_TACKLE; }
    PARAMETRIZE { species = SPECIES_YANMEGA; ability = ABILITY_AERODYNAMIC; absorbedMove = MOVE_PECK; safeMove = MOVE_TACKLE; }
    PARAMETRIZE { species = SPECIES_CRYOGONAL; ability = ABILITY_FLASH_FREEZE; absorbedMove = MOVE_EMBER; safeMove = MOVE_TACKLE; }

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(species) { UniqueAbility(ability); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(absorbedMove, safeMove); }
    } WHEN {
        TURN { SCORE_GT(opponent, safeMove, absorbedMove); }
    }
}

AI_SINGLE_BATTLE_TEST("AI scores reflected status moves below safe moves against known Omnisense")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_XATU) { Ability(ABILITY_SYNCHRONIZE); UniqueAbility(ABILITY_OMNISENSE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TOXIC, MOVE_TACKLE); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_TACKLE, MOVE_TOXIC); }
    }
}

AI_SINGLE_BATTLE_TEST("AI scores priority moves below safe moves against known Gridlock in Electric Terrain")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_QUICK_ATTACK].priority > 0);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_REGIELEKI) { Speed(100); Ability(ABILITY_TRANSISTOR); Moves(MOVE_ELECTRIC_TERRAIN, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_QUICK_ATTACK, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ELECTRIC_TERRAIN); EXPECT_MOVES(opponent, MOVE_TACKLE, MOVE_QUICK_ATTACK); }
        TURN { SCORE_GT(opponent, MOVE_TACKLE, MOVE_QUICK_ATTACK); }
    }
}

AI_SINGLE_BATTLE_TEST("AI scores priority moves below safe moves against Royal Storm and Authority")
{
    u16 ability;
    u16 species;

    PARAMETRIZE { species = SPECIES_POLITOED; ability = ABILITY_ROYAL_STORM; }
    PARAMETRIZE { species = SPECIES_HONEDGE; ability = ABILITY_AUTHORITY; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_AQUA_JET].priority > 0);
        ASSUME(gBattleMoves[MOVE_AQUA_JET].power == gBattleMoves[MOVE_WATER_GUN].power);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(species) { Ability(ABILITY_DAMP); UniqueAbility(ability); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_AQUA_JET, MOVE_WATER_GUN); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_WATER_GUN, MOVE_AQUA_JET); }
    }
}

AI_SINGLE_BATTLE_TEST("AI scores status moves below safe moves against last-mon Dominion")
{
    u16 statusMove;

    PARAMETRIZE { statusMove = MOVE_SPORE; }
    PARAMETRIZE { statusMove = MOVE_TOXIC; }
    PARAMETRIZE { statusMove = MOVE_WILL_O_WISP; }
    PARAMETRIZE { statusMove = MOVE_THUNDER_WAVE; }

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_TYRANITAR) { UniqueAbility(ABILITY_DOMINION); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(statusMove, MOVE_TACKLE); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_TACKLE, statusMove); }
    }
}

AI_SINGLE_BATTLE_TEST("AI scores stat drops below default against Crystal Armor and active Sand Skimmer")
{
    u16 ability;
    u16 species;
    u16 setupMove;

    PARAMETRIZE { species = SPECIES_CLAMPERL; ability = ABILITY_CRYSTAL_ARMOR; setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { species = SPECIES_GLIGAR; ability = ABILITY_SAND_SKIMMER; setupMove = MOVE_SANDSTORM; }

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(species) { UniqueAbility(ability); Moves(setupMove); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SCREECH, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, setupMove); EXPECT_MOVES(opponent, MOVE_SCREECH, MOVE_TACKLE); }
        TURN { MOVE(player, setupMove); SCORE_LT_VAL(opponent, MOVE_SCREECH, AI_SCORE_DEFAULT); }
    }
}

AI_SINGLE_BATTLE_TEST("AI scores phazing below default against Rooted Shrine in Grassy Terrain")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_ROAR].effect == EFFECT_ROAR);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_TAPU_BULU) { UniqueAbility(ABILITY_ROOTED_SHRINE); Moves(MOVE_GRASSY_TERRAIN); }
        PLAYER(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_ROAR, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GRASSY_TERRAIN); EXPECT_MOVES(opponent, MOVE_ROAR, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_GRASSY_TERRAIN); SCORE_LT_VAL(opponent, MOVE_ROAR, AI_SCORE_DEFAULT); }
    }
}

AI_SINGLE_BATTLE_TEST("AI does not value blocked secondary effects against Toxisphere and Smog Refinery")
{
    u16 ability;
    u16 species;

    PARAMETRIZE { species = SPECIES_WEEZING; ability = ABILITY_TOXISPHERE; }
    PARAMETRIZE { species = SPECIES_WEEZING_GALARIAN; ability = ABILITY_SMOG_REFINERY; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_BUBBLE].power == gBattleMoves[MOVE_WATER_GUN].power);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(species) { Speed(200); UniqueAbility(ability); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); Moves(MOVE_BUBBLE, MOVE_WATER_GUN); }
    } WHEN {
        TURN { SCORE_EQ(opponent, MOVE_BUBBLE, MOVE_WATER_GUN); }
    }
}

AI_SINGLE_BATTLE_TEST("AI does not value blocked flinches against Toxisphere and Smog Refinery")
{
    u16 ability;
    u16 species;

    PARAMETRIZE { species = SPECIES_WEEZING; ability = ABILITY_TOXISPHERE; }
    PARAMETRIZE { species = SPECIES_WEEZING_GALARIAN; ability = ABILITY_SMOG_REFINERY; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_BITE].power == gBattleMoves[MOVE_FEINT_ATTACK].power);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(species) { Speed(10); UniqueAbility(ability); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(200); Moves(MOVE_BITE, MOVE_FEINT_ATTACK); }
    } WHEN {
        TURN { SCORE_EQ(opponent, MOVE_BITE, MOVE_FEINT_ATTACK); }
    }
}

AI_SINGLE_BATTLE_TEST("AI penalizes status moves against Precognition and Counterspell")
{
    u16 ability;
    u16 species;

    PARAMETRIZE { species = SPECIES_ESPEON; ability = ABILITY_PRECOGNITION; }
    PARAMETRIZE { species = SPECIES_SLOWKING_GALARIAN; ability = ABILITY_COUNTERSPELL; }

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_VIABILITY);
        PLAYER(species) { UniqueAbility(ability); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { SCORE_LT_VAL(opponent, MOVE_SPLASH, AI_SCORE_DEFAULT); }
    }
}

AI_SINGLE_BATTLE_TEST("AI scores priority and switching moves below safe moves against known Lock Protocol")
{
    u16 blockedMove;

    PARAMETRIZE { blockedMove = MOVE_QUICK_ATTACK; }
    PARAMETRIZE { blockedMove = MOVE_U_TURN; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_QUICK_ATTACK].priority > 0);
        ASSUME(gBattleMoves[MOVE_U_TURN].effect == EFFECT_HIT_ESCAPE);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_IRON_JUGULIS) { Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(blockedMove, MOVE_TACKLE); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_TACKLE, blockedMove); }
    }
}

AI_SINGLE_BATTLE_TEST("AI avoids moves blocked by Blast Shield and Flak Shield")
{
    u16 shieldAbility;
    u16 blockedMove;
    u16 safeMove;

    PARAMETRIZE { shieldAbility = ABILITY_BLAST_SHIELD; blockedMove = MOVE_CLOSE_COMBAT; safeMove = MOVE_PSYCHIC; }
    PARAMETRIZE { shieldAbility = ABILITY_FLAK_SHIELD; blockedMove = MOVE_TACKLE; safeMove = MOVE_SWIFT; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_CLOSE_COMBAT].power == BLAST_SHIELD_MIN_POWER);
        ASSUME(gBattleMoves[MOVE_PSYCHIC].power < BLAST_SHIELD_MIN_POWER);
        ASSUME(gBattleMoves[MOVE_TACKLE].power == FLAK_SHIELD_MAX_POWER);
        ASSUME(gBattleMoves[MOVE_SWIFT].power > FLAK_SHIELD_MAX_POWER);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_WOBBUFFET) { UniqueAbility(shieldAbility); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(blockedMove, safeMove); }
    } WHEN {
        TURN { SCORE_GT(opponent, safeMove, blockedMove); }
    }
}

AI_SINGLE_BATTLE_TEST("AI scores phazing moves below default against known Strange Guest")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_ROAR].effect == EFFECT_ROAR);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_OGERPON_TEAL_MASK) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_ROAR, MOVE_CELEBRATE); }
    } WHEN {
        TURN { SCORE_LT_VAL(opponent, MOVE_ROAR, AI_SCORE_DEFAULT); }
    }
}

AI_SINGLE_BATTLE_TEST("AI scores Whirlwind below default against known Unmovable")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_WHIRLWIND].effect == EFFECT_ROAR);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_HARIYAMA) { UniqueAbility(ABILITY_UNMOVABLE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_WHIRLWIND, MOVE_CELEBRATE); }
    } WHEN {
        TURN { SCORE_LT_VAL(opponent, MOVE_WHIRLWIND, AI_SCORE_DEFAULT); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI scores partner-targeted moves higher when a unique ability absorbs them")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_HP_AWARE);
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_WATER_GUN, MOVE_TACKLE); }
        OPPONENT(SPECIES_PALOSSAND) { Speed(50); HP(40); MaxHP(100); UniqueAbility(ABILITY_LOW_TIDE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SCORE_GT(opponentLeft, MOVE_WATER_GUN, MOVE_TACKLE, target:opponentRight); }
    }
}

AI_SINGLE_BATTLE_TEST("AI scores Thunder Wave below Tackle against known Limber")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_LIMBER); }
        OPPONENT(SPECIES_PIKACHU) { Moves(MOVE_THUNDER_WAVE, MOVE_TACKLE); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_TACKLE, MOVE_THUNDER_WAVE); }
    }
}

AI_SINGLE_BATTLE_TEST("AI scores Thunder Wave below Tackle against revealed unique Limber")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_WOBBUFFET) { UniqueAbility(ABILITY_LIMBER); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PIKACHU) { Moves(MOVE_THUNDER_WAVE, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_THUNDER_WAVE); }
        TURN { SCORE_GT(opponent, MOVE_TACKLE, MOVE_THUNDER_WAVE); }
    }
}

AI_SINGLE_BATTLE_TEST("AI scores harmful status moves below Tackle against Unknown Biology")
{
    u16 statusMove;

    PARAMETRIZE { statusMove = MOVE_THUNDER_WAVE; }
    PARAMETRIZE { statusMove = MOVE_TOXIC; }
    PARAMETRIZE { statusMove = MOVE_WILL_O_WISP; }

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_DEOXYS_NORMAL) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_UNKNOWN_BIOLOGY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PIKACHU) { Moves(statusMove, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_TACKLE); }
        TURN { SCORE_GT(opponent, MOVE_TACKLE, statusMove); }
    }
}

AI_SINGLE_BATTLE_TEST("AI prefers moves with better accuracy, but only if they both require the same number of hits to ko")
{
    u16 move1 = MOVE_NONE, move2 = MOVE_NONE, move3 = MOVE_NONE, move4 = MOVE_NONE;
    u16 hp, expectedMove, turns, abilityAtk, expectedMove2;

    abilityAtk = ABILITY_NONE;
    expectedMove2 = MOVE_NONE;

    // Here it's a simple test, both Slam and Extreme Speed deal the same damage, but Extreme Speed is more accurate.
    PARAMETRIZE { move1 = MOVE_SLAM; move2 = MOVE_EXTREME_SPEED; move3 = MOVE_TACKLE; hp = 490; expectedMove = MOVE_EXTREME_SPEED; turns = 4; }
    PARAMETRIZE { move1 = MOVE_SLAM; move2 = MOVE_EXTREME_SPEED; move3 = MOVE_SWIFT; move4 = MOVE_TACKLE; hp = 365; expectedMove = MOVE_EXTREME_SPEED; turns = 3; }
    PARAMETRIZE { move1 = MOVE_SLAM; move2 = MOVE_EXTREME_SPEED; move3 = MOVE_SWIFT; move4 = MOVE_TACKLE; hp = 245; expectedMove = MOVE_EXTREME_SPEED; turns = 2; }
    PARAMETRIZE { move1 = MOVE_SLAM; move2 = MOVE_EXTREME_SPEED; move3 = MOVE_SWIFT; move4 = MOVE_TACKLE; hp = 125; expectedMove = MOVE_EXTREME_SPEED; turns = 1; }
    // Take Down deals more damage, but can miss more often. Here, AI should choose Take Down if it can faint target in less number of turns than Extreme Speed. Otherwise, it should use Extreme Speed.
    PARAMETRIZE { move1 = MOVE_TAKE_DOWN; move2 = MOVE_EXTREME_SPEED; move3 = MOVE_SWIFT; move4 = MOVE_TACKLE; hp = 140; expectedMove = MOVE_TAKE_DOWN; turns = 1; }
    PARAMETRIZE { move1 = MOVE_TAKE_DOWN; move2 = MOVE_EXTREME_SPEED; move3 = MOVE_SWIFT; move4 = MOVE_TACKLE; hp = 245; expectedMove = MOVE_EXTREME_SPEED; turns = 2; }
    // Swift always hits and Guts has accuracy of 100%. Hustle lowers accuracy of all physical moves.
    PARAMETRIZE { abilityAtk = ABILITY_HUSTLE; move1 = MOVE_MEGA_KICK; move2 = MOVE_EXTREME_SPEED; move3 = MOVE_SWIFT; move4 = MOVE_TACKLE; hp = 5; expectedMove = MOVE_SWIFT; turns = 1; }
    PARAMETRIZE { abilityAtk = ABILITY_HUSTLE; move1 = MOVE_MEGA_KICK; move2 = MOVE_EXTREME_SPEED; move3 = MOVE_GUST; move4 = MOVE_TACKLE; hp = 5; expectedMove = MOVE_GUST; turns = 1; }
    // Mega Kick and Slam both have lower accuracy. Gust and Tackle both have 100, so AI can choose either of them.
    PARAMETRIZE { move1 = MOVE_MEGA_KICK; move2 = MOVE_SLAM; move3 = MOVE_TACKLE; move4 = MOVE_GUST; hp = 5; expectedMove = MOVE_GUST; expectedMove2 = MOVE_TACKLE; turns = 1; }
    // All moves hit with No guard ability
    PARAMETRIZE { move1 = MOVE_MEGA_KICK; move2 = MOVE_GUST; hp = 5; expectedMove = MOVE_MEGA_KICK; expectedMove2 = MOVE_GUST; turns = 1; }
    // Tests to compare move that always hits and a beneficial effect. A move with higher acc should be chosen in this case.
    PARAMETRIZE { move1 = MOVE_SHOCK_WAVE; move2 = MOVE_ICY_WIND; hp = 5; expectedMove = MOVE_SHOCK_WAVE; turns = 1; }
    PARAMETRIZE { move1 = MOVE_SHOCK_WAVE; move2 = MOVE_ICY_WIND; move3 = MOVE_THUNDERBOLT; hp = 5; expectedMove = MOVE_SHOCK_WAVE; expectedMove2 = MOVE_THUNDERBOLT; turns = 1; }

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { HP(hp); }
        PLAYER(SPECIES_WOBBUFFET);
        ASSUME(gBattleMoves[MOVE_SWIFT].accuracy == 0);
        ASSUME(gBattleMoves[MOVE_SLAM].power == gBattleMoves[MOVE_EXTREME_SPEED].power);
        ASSUME(gBattleMoves[MOVE_TAKE_DOWN].power > gBattleMoves[MOVE_EXTREME_SPEED].power);
        ASSUME(gBattleMoves[MOVE_SLAM].accuracy < gBattleMoves[MOVE_EXTREME_SPEED].accuracy);
        ASSUME(gBattleMoves[MOVE_TAKE_DOWN].accuracy < gBattleMoves[MOVE_EXTREME_SPEED].accuracy);
        ASSUME(gBattleMoves[MOVE_TACKLE].accuracy == 100);
        ASSUME(gBattleMoves[MOVE_GUST].accuracy == 100);
        ASSUME(gBattleMoves[MOVE_SHOCK_WAVE].accuracy == 0);
        ASSUME(gBattleMoves[MOVE_THUNDERBOLT].accuracy == 100);
        ASSUME(gBattleMoves[MOVE_ICY_WIND].accuracy != 100);
        OPPONENT(SPECIES_EXPLOUD) { Moves(move1, move2, move3, move4); Ability(abilityAtk); SpAttack(50); } // Low Sp.Atk, so Swift deals less damage than Extreme Speed.
    } WHEN {
            switch (turns)
            {
            case 1:
                if (expectedMove2 != MOVE_NONE) {
                    TURN { EXPECT_MOVES(opponent, expectedMove, expectedMove2); SEND_OUT(player, 1); }
                }
                else {
                    TURN { EXPECT_MOVE(opponent, expectedMove); SEND_OUT(player, 1); }
                }
                break;
            case 2:
                TURN { EXPECT_MOVE(opponent, expectedMove); }
                TURN { EXPECT_MOVE(opponent, expectedMove); SEND_OUT(player, 1); }
                break;
            case 3:
                TURN { EXPECT_MOVE(opponent, expectedMove); }
                TURN { EXPECT_MOVE(opponent, expectedMove); }
                TURN { EXPECT_MOVE(opponent, expectedMove); SEND_OUT(player, 1); }
                break;
            case 4:
                TURN { EXPECT_MOVE(opponent, expectedMove); }
                TURN { EXPECT_MOVE(opponent, expectedMove); }
                TURN { EXPECT_MOVE(opponent, expectedMove); }
                TURN { EXPECT_MOVE(opponent, expectedMove); SEND_OUT(player, 1); }
                break;
            }
    } SCENE {
        MESSAGE("Wobbuffet fainted!");
    }
}

AI_SINGLE_BATTLE_TEST("AI prefers moves which deal more damage instead of moves which are super-effective but deal less damage")
{
    u8 turns = 0;
    u16 move1 = MOVE_NONE, move2 = MOVE_NONE, move3 = MOVE_NONE, move4 = MOVE_NONE;
    u16 expectedMove, abilityAtk, abilityDef;

    abilityAtk = ABILITY_NONE;

    // Scald and Poison Jab take 3 hits, Waterfall takes 2.
    PARAMETRIZE { move1 = MOVE_WATERFALL; move2 = MOVE_SCALD; move3 = MOVE_POISON_JAB; move4 = MOVE_WATER_GUN; expectedMove = MOVE_WATERFALL; turns = 2; }
    // Poison Jab takes 3 hits, Water gun 5. Shield Dust prevents poison chip damage.
    PARAMETRIZE { move1 = MOVE_POISON_JAB; move2 = MOVE_WATER_GUN; expectedMove = MOVE_POISON_JAB; abilityDef = ABILITY_SHIELD_DUST; turns = 3; }

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_TYPHLOSION) { Ability(abilityDef); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_NIDOQUEEN) { Moves(move1, move2, move3, move4); Ability(abilityAtk); }
    } WHEN {
            switch (turns)
            {
            case 2:
                TURN { EXPECT_MOVE(opponent, expectedMove); }
                TURN { EXPECT_MOVE(opponent, expectedMove); SEND_OUT(player, 1); }
                break;
            case 3:
                TURN { EXPECT_MOVE(opponent, expectedMove); }
                TURN { EXPECT_MOVE(opponent, expectedMove); }
                TURN { EXPECT_MOVE(opponent, expectedMove); SEND_OUT(player, 1); }
                break;
            }
    } SCENE {
        MESSAGE("Typhlosion fainted!");
    }
}

AI_SINGLE_BATTLE_TEST("AI prefers Earthquake over Drill Run if both require the same number of hits to ko")
{
    // Drill Run has less accuracy than E-quake, but can score a higher crit. However the chance is too small, so AI should ignore it.
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_GEODUDE) { Ability(ABILITY_STURDY); Moves(MOVE_EARTHQUAKE, MOVE_DRILL_RUN); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_EARTHQUAKE); }
    }
    SCENE {}
}

AI_SINGLE_BATTLE_TEST("AI prefers a weaker move over a one with a downside effect if both require the same number of hits to ko")
{
    u16 move1 = MOVE_NONE, move2 = MOVE_NONE, move3 = MOVE_NONE, move4 = MOVE_NONE;
    u16 hp, expectedMove, turns;

    // Both moves require the same number of turns but Flamethrower will be chosen over Overheat (powerful effect)
    PARAMETRIZE { move1 = MOVE_OVERHEAT; move2 = MOVE_FLAMETHROWER; hp = 200; expectedMove = MOVE_FLAMETHROWER; turns = 2; }
    // Overheat kill in least amount of turns
    PARAMETRIZE { move1 = MOVE_OVERHEAT; move2 = MOVE_FLAMETHROWER; hp = 150; expectedMove = MOVE_OVERHEAT; turns = 1; }

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { HP(hp); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { SpAttack(200); Ability(ABILITY_NONE); Moves(move1, move2, move3, move4); }
    } WHEN {
        switch (turns)
        {
        case 1:
            TURN { EXPECT_MOVE(opponent, expectedMove); SEND_OUT(player, 1); }
            break;
        case 2:
            TURN { EXPECT_MOVE(opponent, expectedMove); }
            TURN { EXPECT_MOVE(opponent, expectedMove); SEND_OUT(player, 1); }
            break;
        }
    } SCENE {
        MESSAGE("Wobbuffet fainted!");
    }
}

AI_SINGLE_BATTLE_TEST("AI prefers moves with the best possible score, chosen randomly if tied")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { HP(5); };
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_THUNDERBOLT, MOVE_SLUDGE_BOMB, MOVE_TAKE_DOWN); }
    } WHEN {
        TURN { EXPECT_MOVES(opponent, MOVE_THUNDERBOLT, MOVE_SLUDGE_BOMB); SEND_OUT(player, 1); }
    }
    SCENE {
        MESSAGE("Wobbuffet fainted!");
    }
}

AI_SINGLE_BATTLE_TEST("AI can choose a status move that boosts the attack by two")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { HP(277); };
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_KANGASKHAN) { Moves(MOVE_BODY_SLAM, MOVE_HORN_ATTACK, MOVE_SWORDS_DANCE); }
    } WHEN {
        TURN { EXPECT_MOVES(opponent, MOVE_BODY_SLAM, MOVE_SWORDS_DANCE); }
        TURN { EXPECT_MOVE(opponent, MOVE_BODY_SLAM); SEND_OUT(player, 1); }
    }
}

AI_SINGLE_BATTLE_TEST("AI chooses the safest option to faint the target, taking into account accuracy and move effect")
{
    u16 move1 = MOVE_NONE, move2 = MOVE_NONE, move3 = MOVE_NONE, move4 = MOVE_NONE;
    u16 expectedMove, expectedMove2 = MOVE_NONE;
    u16 abilityAtk = ABILITY_NONE, holdItemAtk = ITEM_NONE;

    // Psychic is not very effective, but always hits. Solarbeam requires a charging turn, Double Edge has recoil and Focus Blast can miss;
    PARAMETRIZE { abilityAtk = ABILITY_STURDY; move1 = MOVE_FOCUS_BLAST; move2 = MOVE_SOLAR_BEAM; move3 = MOVE_PSYCHIC; move4 = MOVE_DOUBLE_EDGE; expectedMove = MOVE_PSYCHIC; }
    // Same as above, but ai mon has rock head ability, so it can use Double Edge without taking recoil damage. Psychic can also lower Special Defense,
    // but because it faints the target it doesn't matter.
    PARAMETRIZE { abilityAtk = ABILITY_ROCK_HEAD; move1 = MOVE_FOCUS_BLAST; move2 = MOVE_SOLAR_BEAM; move3 = MOVE_PSYCHIC; move4 = MOVE_DOUBLE_EDGE;
                  expectedMove = MOVE_PSYCHIC; expectedMove2 = MOVE_DOUBLE_EDGE; }
    // This time it's Solarbeam + Psychic, because the weather is sunny.
    PARAMETRIZE { abilityAtk = ABILITY_DROUGHT; move1 = MOVE_FOCUS_BLAST; move2 = MOVE_SOLAR_BEAM; move3 = MOVE_PSYCHIC; move4 = MOVE_DOUBLE_EDGE;
                  expectedMove = MOVE_PSYCHIC; expectedMove2 = MOVE_SOLAR_BEAM; }
    // Psychic and Solar Beam are chosen because user is holding Power Herb
    PARAMETRIZE { abilityAtk = ABILITY_STURDY; holdItemAtk = ITEM_POWER_HERB; move1 = MOVE_FOCUS_BLAST; move2 = MOVE_SOLAR_BEAM; move3 = MOVE_PSYCHIC; move4 = MOVE_DOUBLE_EDGE;
                  expectedMove = MOVE_PSYCHIC; expectedMove2 = MOVE_SOLAR_BEAM; }
    // Psychic and Skull Bash are chosen because user is holding Power Herb
    PARAMETRIZE { abilityAtk = ABILITY_STURDY; holdItemAtk = ITEM_POWER_HERB; move1 = MOVE_FOCUS_BLAST; move2 = MOVE_SKULL_BASH; move3 = MOVE_PSYCHIC; move4 = MOVE_DOUBLE_EDGE;
                  expectedMove = MOVE_PSYCHIC; expectedMove2 = MOVE_SKULL_BASH; }
    // Skull Bash is chosen because it's the most accurate and is holding Power Herb
    PARAMETRIZE { abilityAtk = ABILITY_STURDY; holdItemAtk = ITEM_POWER_HERB; move1 = MOVE_FOCUS_BLAST; move2 = MOVE_SKULL_BASH; move3 = MOVE_SLAM; move4 = MOVE_DOUBLE_EDGE;
                  expectedMove = MOVE_SKULL_BASH; }
    // Crabhammer is chosen even if Skull Bash is more accurate, the user has no Power Herb
    PARAMETRIZE { abilityAtk = ABILITY_STURDY; move1 = MOVE_FOCUS_BLAST; move2 = MOVE_SKULL_BASH; move3 = MOVE_SLAM; move4 = MOVE_CRABHAMMER;
                  expectedMove = MOVE_CRABHAMMER; }

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { HP(5); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_GEODUDE) { Moves(move1, move2, move3, move4); Ability(abilityAtk); Item(holdItemAtk); }
    } WHEN {
        TURN {  if (expectedMove2 == MOVE_NONE) { EXPECT_MOVE(opponent, expectedMove); SEND_OUT(player, 1); }
                else {EXPECT_MOVES(opponent, expectedMove, expectedMove2); SCORE_EQ(opponent, expectedMove, expectedMove2); SEND_OUT(player, 1);}
             }
    }
    SCENE {
        MESSAGE("Wobbuffet fainted!");
    }
}

AI_SINGLE_BATTLE_TEST("AI won't use Solar Beam if there is no Sun up or the user is not holding Power Herb")
{
    u16 abilityAtk = ABILITY_NONE;
    u16 holdItemAtk = ITEM_NONE;

    PARAMETRIZE { abilityAtk = ABILITY_DROUGHT; }
    PARAMETRIZE { holdItemAtk = ITEM_POWER_HERB; }
    PARAMETRIZE { }

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { HP(211); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_TYPHLOSION) { Moves(MOVE_SOLAR_BEAM, MOVE_GRASS_PLEDGE); Ability(abilityAtk); Item(holdItemAtk); }
    } WHEN {
        if (abilityAtk == ABILITY_DROUGHT) {
            TURN { EXPECT_MOVES(opponent, MOVE_SOLAR_BEAM, MOVE_GRASS_PLEDGE); }
            TURN { EXPECT_MOVES(opponent, MOVE_SOLAR_BEAM, MOVE_GRASS_PLEDGE); SEND_OUT(player, 1); }
        } else if (holdItemAtk == ITEM_POWER_HERB) {
            TURN { EXPECT_MOVES(opponent, MOVE_SOLAR_BEAM, MOVE_GRASS_PLEDGE); MOVE(player, MOVE_KNOCK_OFF); }
            TURN { EXPECT_MOVE(opponent, MOVE_GRASS_PLEDGE); SEND_OUT(player, 1); }
        } else {
            TURN { EXPECT_MOVE(opponent, MOVE_GRASS_PLEDGE); }
            TURN { EXPECT_MOVE(opponent, MOVE_GRASS_PLEDGE); SEND_OUT(player, 1); }
        }
    } SCENE {
        MESSAGE("Wobbuffet fainted!");
    }
}

AI_SINGLE_BATTLE_TEST("AI won't use ground type attacks against flying type Pokemon unless Gravity is in effect")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_CROBAT);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_NIDOQUEEN) { Moves(MOVE_EARTHQUAKE, MOVE_TACKLE, MOVE_POISON_STING, MOVE_GUST); }
    } WHEN {
            TURN { NOT_EXPECT_MOVE(opponent, MOVE_EARTHQUAKE); }
            TURN { MOVE(player, MOVE_GRAVITY); NOT_EXPECT_MOVE(opponent, MOVE_EARTHQUAKE); }
            TURN { EXPECT_MOVE(opponent, MOVE_EARTHQUAKE); SEND_OUT(player, 1); }
    } SCENE {
        MESSAGE("Gravity intensified!");
    }
}

AI_DOUBLE_BATTLE_TEST("AI won't use a Weather changing move if partner already chose such move")
{
    u32 j, k;
    static const u16 weatherMoves[] = {MOVE_SUNNY_DAY, MOVE_HAIL, MOVE_RAIN_DANCE, MOVE_SANDSTORM, MOVE_SNOWSCAPE};
    u16 weatherMoveLeft = MOVE_NONE, weatherMoveRight = MOVE_NONE;

    for (j = 0; j < ARRAY_COUNT(weatherMoves); j++)
    {
        for (k = 0; k < ARRAY_COUNT(weatherMoves); k++)
        {
            PARAMETRIZE { weatherMoveLeft = weatherMoves[j]; weatherMoveRight = weatherMoves[k]; }
        }
    }

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(weatherMoveLeft); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE, weatherMoveRight); }
    } WHEN {
            TURN {  NOT_EXPECT_MOVE(opponentRight, weatherMoveRight);
                    SCORE_LT_VAL(opponentRight, weatherMoveRight, AI_SCORE_DEFAULT, target:playerLeft);
                    SCORE_LT_VAL(opponentRight, weatherMoveRight, AI_SCORE_DEFAULT, target:playerRight);
                    SCORE_LT_VAL(opponentRight, weatherMoveRight, AI_SCORE_DEFAULT, target:opponentLeft);
                 }
    }
}

AI_DOUBLE_BATTLE_TEST("AI will not use Helping Hand if partner does not have any damage moves")
{
    u16 move1 = MOVE_NONE, move2 = MOVE_NONE, move3 = MOVE_NONE, move4 = MOVE_NONE;

    PARAMETRIZE{ move1 = MOVE_LEER; move2 = MOVE_TOXIC; }
    PARAMETRIZE{ move1 = MOVE_HELPING_HAND; move2 = MOVE_PROTECT; }
    PARAMETRIZE{ move1 = MOVE_ACUPRESSURE; move2 = MOVE_DOUBLE_TEAM; move3 = MOVE_TOXIC; move4 = MOVE_PROTECT; }

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_HELPING_HAND, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(move1, move2, move3, move4); }
    } WHEN {
            TURN {  NOT_EXPECT_MOVE(opponentLeft, MOVE_HELPING_HAND);
                    SCORE_LT_VAL(opponentLeft, MOVE_HELPING_HAND, AI_SCORE_DEFAULT, target:playerLeft);
                    SCORE_LT_VAL(opponentLeft, MOVE_HELPING_HAND, AI_SCORE_DEFAULT, target:playerRight);
                    SCORE_LT_VAL(opponentLeft, MOVE_HELPING_HAND, AI_SCORE_DEFAULT, target:opponentLeft);
                 }
    } SCENE {
        NOT MESSAGE("Foe Wobbuffet used Helping Hand!");
    }
}

AI_DOUBLE_BATTLE_TEST("AI will not use a status move if partner already chose Helping Hand")
{
    s32 j;
    u32 statusMove = MOVE_NONE;

    for (j = MOVE_NONE + 1; j < MOVES_COUNT; j++)
    {
        if (gBattleMoves[j].split == SPLIT_STATUS) {
            PARAMETRIZE{ statusMove = j; }
        }
    }

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_HELPING_HAND); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE, statusMove); }
    } WHEN {
            TURN {  NOT_EXPECT_MOVE(opponentRight, statusMove);
                    SCORE_LT_VAL(opponentRight, statusMove, AI_SCORE_DEFAULT, target:playerLeft);
                    SCORE_LT_VAL(opponentRight, statusMove, AI_SCORE_DEFAULT, target:playerRight);
                    SCORE_LT_VAL(opponentRight, statusMove, AI_SCORE_DEFAULT, target:opponentLeft);
                 }
    } SCENE {
        MESSAGE("Foe Wobbuffet used Helping Hand!");
    }
}

AI_SINGLE_BATTLE_TEST("AI without any flags chooses moves at random - singles")
{
    GIVEN {
        AI_FLAGS(0);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_NIDOQUEEN) { Moves(MOVE_SPLASH, MOVE_EXPLOSION, MOVE_RAGE, MOVE_HELPING_HAND); }
    } WHEN {
            TURN { EXPECT_MOVES(opponent, MOVE_SPLASH, MOVE_EXPLOSION, MOVE_RAGE, MOVE_HELPING_HAND);
                   SCORE_EQ_VAL(opponent, MOVE_SPLASH, AI_SCORE_DEFAULT);
                   SCORE_EQ_VAL(opponent, MOVE_EXPLOSION, AI_SCORE_DEFAULT);
                   SCORE_EQ_VAL(opponent, MOVE_RAGE, AI_SCORE_DEFAULT);
                   SCORE_EQ_VAL(opponent, MOVE_HELPING_HAND, AI_SCORE_DEFAULT);
                }
    }
}

AI_DOUBLE_BATTLE_TEST("AI without any flags chooses moves at random - doubles")
{
    GIVEN {
        AI_FLAGS(0);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_NIDOQUEEN) { Moves(MOVE_SPLASH, MOVE_EXPLOSION, MOVE_RAGE, MOVE_HELPING_HAND); }
        OPPONENT(SPECIES_NIDOQUEEN) { Moves(MOVE_SPLASH, MOVE_EXPLOSION, MOVE_RAGE, MOVE_HELPING_HAND); }
    } WHEN {
            TURN { EXPECT_MOVES(opponentLeft, MOVE_SPLASH, MOVE_EXPLOSION, MOVE_RAGE, MOVE_HELPING_HAND);
                   EXPECT_MOVES(opponentRight, MOVE_SPLASH, MOVE_EXPLOSION, MOVE_RAGE, MOVE_HELPING_HAND);
                   SCORE_EQ_VAL(opponentLeft, MOVE_SPLASH, AI_SCORE_DEFAULT, target:playerLeft);
                   SCORE_EQ_VAL(opponentLeft, MOVE_EXPLOSION, AI_SCORE_DEFAULT, target:playerLeft);
                   SCORE_EQ_VAL(opponentLeft, MOVE_RAGE, AI_SCORE_DEFAULT, target:playerLeft);
                   SCORE_EQ_VAL(opponentLeft, MOVE_HELPING_HAND, AI_SCORE_DEFAULT, target:playerLeft);
                   SCORE_EQ_VAL(opponentRight, MOVE_SPLASH, AI_SCORE_DEFAULT, target:playerLeft);
                   SCORE_EQ_VAL(opponentRight, MOVE_EXPLOSION, AI_SCORE_DEFAULT, target:playerLeft);
                   SCORE_EQ_VAL(opponentRight, MOVE_RAGE, AI_SCORE_DEFAULT, target:playerLeft);
                   SCORE_EQ_VAL(opponentRight, MOVE_HELPING_HAND, AI_SCORE_DEFAULT, target:playerLeft);
                }
    }
}

AI_SINGLE_BATTLE_TEST("AI will choose either Rock Tomb or Bulldoze if Stat drop effect will activate and they kill with the same number of hits")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { HP(46); Speed(20); }
        PLAYER(SPECIES_WYNAUT) { Speed(20); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); Moves(MOVE_BULLDOZE, MOVE_ROCK_TOMB); }
    } WHEN {
            TURN { EXPECT_MOVES(opponent, MOVE_BULLDOZE, MOVE_ROCK_TOMB); }
            TURN { EXPECT_MOVES(opponent, MOVE_BULLDOZE, MOVE_ROCK_TOMB); SEND_OUT(player, 1); }
    } SCENE {
        MESSAGE("Wobbuffet fainted!");
    }
}

AI_SINGLE_BATTLE_TEST("AI_FLAG_SMART_MON_CHOICES: AI will not switch in a Pokemon which is slower and gets 1HKOed after fainting")
{
    bool32 alakazamFirst;
    u32 speedAlakazm;
    u32 aiSmartSwitchFlags = 0;

    PARAMETRIZE{ speedAlakazm = 200; alakazamFirst = TRUE; } // Without smart switch flags, AI prioritizes the stronger attacker.
    PARAMETRIZE{ speedAlakazm = 400; alakazamFirst = TRUE; aiSmartSwitchFlags = AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES; } // With smart switch flags, AI still brings in the faster KO option.

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | aiSmartSwitchFlags);
        PLAYER(SPECIES_WEAVILE) { Speed(300); HP(300); SpDefense(200); Attack(300); Moves(MOVE_NIGHT_SLASH); }
        OPPONENT(SPECIES_PONYTA) { Level(1); Speed(1); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(speedAlakazm); HP(50); MaxHP(50); Defense(1); SpAttack(400); Moves(MOVE_FOCUS_BLAST, MOVE_PSYCHIC); } // Can OHKO Weavile, but if slower should be considered too fragile.
        OPPONENT(SPECIES_WYNAUT) { Speed(200); HP(300); MaxHP(300); Defense(200); Moves(MOVE_BUBBLE_BEAM, MOVE_WATER_GUN, MOVE_LEER, MOVE_STRENGTH); } // Can't OHKO, but survives a hit from Weavile's Night Slash.
    } WHEN {
            if (aiSmartSwitchFlags)
                TURN { MOVE(player, MOVE_NIGHT_SLASH); EXPECT_SWITCH(opponent, alakazamFirst ? 1 : 2); }
            else
                TURN { MOVE(player, MOVE_NIGHT_SLASH); EXPECT_SEND_OUT(opponent, alakazamFirst ? 1 : 2); }
    } SCENE {
        if (!aiSmartSwitchFlags)
            MESSAGE("Foe Ponyta fainted!");
        if (alakazamFirst) {
            MESSAGE("{PKMN} TRAINER LEAF sent out Wobbuffet!");
        } else {
            MESSAGE("{PKMN} TRAINER LEAF sent out Wynaut!");
        }
    }
}

AI_SINGLE_BATTLE_TEST("AI switches if Perish Song is about to kill")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) {Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_CROBAT) {Moves(MOVE_TACKLE); }
    } WHEN {
            TURN { MOVE(player, MOVE_PERISH_SONG); }
            TURN { ; }
            TURN { ; }
            TURN { EXPECT_SWITCH(opponent, 1); }
    } SCENE {
        MESSAGE("{PKMN} TRAINER LEAF sent out Crobat!");
    }
}

AI_SINGLE_BATTLE_TEST("AI_FLAG_SMART_MON_CHOICES: AI will not switch in a Pokemon which is slower and gets 1HKOed after fainting")
{
    bool32 alakazamFaster;
    u32 speedAlakazm;

    PARAMETRIZE{ speedAlakazm = 200; alakazamFaster = FALSE; }
    PARAMETRIZE{ speedAlakazm = 400; alakazamFaster = TRUE; }

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_MON_CHOICES);
        PLAYER(SPECIES_WEAVILE) { Speed(300); HP(300); SpDefense(200); Attack(300); Moves(MOVE_NIGHT_SLASH); }
        OPPONENT(SPECIES_PONYTA) { Level(1); Speed(1); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(speedAlakazm); HP(50); MaxHP(50); Defense(1); SpAttack(400); Moves(MOVE_FOCUS_BLAST, MOVE_PSYCHIC); } // Can OHKO Weavile, but if slower should be considered too fragile.
        OPPONENT(SPECIES_WYNAUT) { Speed(200); HP(300); MaxHP(300); Defense(200); Moves(MOVE_BUBBLE_BEAM, MOVE_WATER_GUN, MOVE_LEER, MOVE_STRENGTH); } // Can't OHKO, but survives a hit from Weavile's Night Slash.
    } WHEN {
            TURN { MOVE(player, MOVE_NIGHT_SLASH); EXPECT_SEND_OUT(opponent, alakazamFaster ? 1 : 2); }
    } SCENE {
        MESSAGE("Foe Ponyta fainted!");
        if (alakazamFaster) {
            MESSAGE("{PKMN} TRAINER LEAF sent out Wobbuffet!");
        } else {
            MESSAGE("{PKMN} TRAINER LEAF sent out Wynaut!");
        }
    }
}

AI_SINGLE_BATTLE_TEST("AI_FLAG_SMART_MON_CHOICES: AI considers hazard damage when choosing which Pokemon to switch in")
{
    u32 aiIsSmart = 0;
    u32 aiSmartSwitchFlags = 0;

    PARAMETRIZE{ aiIsSmart = 0; aiSmartSwitchFlags = 0; } // AI doesn't care about hazard damage resulting in Pokemon being KO'd
    PARAMETRIZE{ aiIsSmart = 1; aiSmartSwitchFlags = AI_FLAG_SMART_MON_CHOICES; } // AI_FLAG_SMART_MON_CHOICES avoids being KO'd as a result of hazards damage

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | aiSmartSwitchFlags);
        PLAYER(SPECIES_MEGANIUM) { Speed(100); SpDefense(328); SpAttack(265); Moves(MOVE_STEALTH_ROCK, MOVE_SURF); } // Meganium does ~56% minimum ~66% maximum, enough to KO Charizard after rocks and never KO Typhlosion after rocks
        OPPONENT(SPECIES_PONYTA) { Level(5); Speed(5); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_CHARIZARD) { Speed(200); Moves(MOVE_FLAMETHROWER); SpAttack(317); SpDefense(207); MaxHP(297); } // Outspeends and 2HKOs Meganium
        OPPONENT(SPECIES_TYPHLOSION) { Speed(200); Moves(MOVE_FLAMETHROWER); SpAttack(317); SpDefense(207); MaxHP(297); } // Outspeends and 2HKOs Meganium
    } WHEN {
            TURN { MOVE(player, MOVE_STEALTH_ROCK) ;}
            TURN { MOVE(player, MOVE_SURF) ; EXPECT_SEND_OUT(opponent, aiIsSmart ? 2 : 1); } // AI sends out Typhlosion to get the KO with the flag rather than Charizard
    }
}

AI_SINGLE_BATTLE_TEST("AI_FLAG_SMART_MON_CHOICES: Mid-battle switches prioritize type matchup + SE move, then type matchup")
{
    u32 aiSmartSwitchFlags = 0;
    u32 move1;
    u32 move2;
    u32 expectedIndex;

    PARAMETRIZE{ expectedIndex = 3; move1 = MOVE_TACKLE; move2 = MOVE_TACKLE; aiSmartSwitchFlags = 0; } // When not smart, AI will only switch in a defensive mon if it has a SE move, otherwise will just default to damage
    PARAMETRIZE{ expectedIndex = 1; move1 = MOVE_GIGA_DRAIN; move2 = MOVE_TACKLE; aiSmartSwitchFlags = 0; }
    PARAMETRIZE{ expectedIndex = 2; move1 = MOVE_TACKLE; move2 = MOVE_TACKLE; aiSmartSwitchFlags = AI_FLAG_SMART_MON_CHOICES; } // When smart, AI will prioritize SE move, but still switch in good type matchup without SE move
    PARAMETRIZE{ expectedIndex = 1; move1 = MOVE_GIGA_DRAIN; move2 = MOVE_TACKLE; aiSmartSwitchFlags = AI_FLAG_SMART_MON_CHOICES; }

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | aiSmartSwitchFlags);
        PLAYER(SPECIES_MARSHTOMP) { Level(30); Moves(MOVE_MUD_BOMB, MOVE_WATER_GUN, MOVE_GROWL, MOVE_MUD_SHOT); Speed(5); }
        OPPONENT(SPECIES_PONYTA) { Level(1); Moves(MOVE_NONE); Speed(6); } // Forces switchout
        OPPONENT(SPECIES_TANGELA) { Level(30); Moves(move1); Speed(4); }
        OPPONENT(SPECIES_LOMBRE) { Level(30); Moves(move2); Speed(4); }
        OPPONENT(SPECIES_HARIYAMA) { Level(30); Moves(MOVE_VITAL_THROW); Speed(4); }
    } WHEN {
            TURN { MOVE(player, MOVE_GROWL) ; EXPECT_SWITCH(opponent, expectedIndex); }
    }
}

AI_SINGLE_BATTLE_TEST("AI_FLAG_SMART_MON_CHOICES: Mid-battle switches prioritize defensive options")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_MON_CHOICES);
        PLAYER(SPECIES_SWELLOW) { Level(30); Moves(MOVE_WING_ATTACK, MOVE_BOOMBURST); Speed(5); }
        OPPONENT(SPECIES_PONYTA) { Level(1); Moves(MOVE_NONE); Speed(4); } // Forces switchout
        OPPONENT(SPECIES_ARON) { Level(30); Moves(MOVE_HEADBUTT); Speed(4); } // Mid battle, AI sends out Aron
        OPPONENT(SPECIES_ELECTRODE) { Level(30); Moves(MOVE_CHARGE_BEAM); Speed(6); }
    } WHEN {
            TURN { MOVE(player, MOVE_WING_ATTACK) ; EXPECT_SWITCH(opponent, 1); }
    }
}

AI_SINGLE_BATTLE_TEST("AI_FLAG_SMART_MON_CHOICES: Post-KO switches prioritize offensive options")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_MON_CHOICES);
        PLAYER(SPECIES_SWELLOW) { Level(30); Moves(MOVE_WING_ATTACK, MOVE_BOOMBURST); Speed(5); }
        OPPONENT(SPECIES_PONYTA) { Level(1); Moves(MOVE_TACKLE); Speed(4); }
        OPPONENT(SPECIES_ARON) { Level(30); Moves(MOVE_HEADBUTT); Speed(4); } // Mid battle, AI sends out Aron
        OPPONENT(SPECIES_ELECTRODE) { Level(30); Moves(MOVE_CHARGE_BEAM); Speed(6); }
    } WHEN {
            TURN { MOVE(player, MOVE_WING_ATTACK) ; EXPECT_SEND_OUT(opponent, 2); }
    }
}

AI_SINGLE_BATTLE_TEST("AI_FLAG_SMART_SWITCHING: AI switches out after sufficient stat drops")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_SWITCHING);
        PLAYER(SPECIES_HITMONTOP) { Level(30); Moves(MOVE_CHARM, MOVE_TACKLE); Ability(ABILITY_INTIMIDATE); Speed(5); }
        OPPONENT(SPECIES_GRIMER) { Level(30); Moves(MOVE_TACKLE); Ability(ABILITY_STENCH); Speed(4); }
        OPPONENT(SPECIES_PONYTA) { Level(30); Moves(MOVE_HEADBUTT); Speed(4); }
    } WHEN {
            TURN { MOVE(player, MOVE_CHARM) ;}
            TURN { MOVE(player, MOVE_TACKLE) ; EXPECT_SWITCH(opponent, 1); }
    }
}

AI_SINGLE_BATTLE_TEST("AI_FLAG_SMART_SWITCHING: AI will not switch out if Pokemon would faint to hazards unless party member can clear them")
{
    u32 move1;

    PARAMETRIZE{move1 = MOVE_TACKLE; }
    PARAMETRIZE{move1 = MOVE_RAPID_SPIN; }

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_SWITCHING);
        PLAYER(SPECIES_HITMONTOP) { Level(30); Moves(MOVE_CHARM, MOVE_TACKLE, MOVE_STEALTH_ROCK, MOVE_EARTHQUAKE); Ability(ABILITY_INTIMIDATE); Speed(5); }
        OPPONENT(SPECIES_GRIMER) { Level(30); Moves(MOVE_TACKLE); Ability(ABILITY_STENCH); Item(ITEM_FOCUS_SASH); Speed(4); }
        OPPONENT(SPECIES_PONYTA) { Level(30); Moves(MOVE_HEADBUTT, move1); Speed(4); }
    } WHEN {
            TURN { MOVE(player, MOVE_STEALTH_ROCK) ;}
            TURN { MOVE(player, MOVE_EARTHQUAKE) ;}
            TURN { MOVE(player, MOVE_CHARM) ;}
            TURN { // If the AI has a mon that can remove hazards, don't prevent them switching out
                MOVE(player, MOVE_CHARM);
                if (move1 == MOVE_RAPID_SPIN)
                    EXPECT_SWITCH(opponent, 1);
                else if (move1 == MOVE_TACKLE)
                    EXPECT_SWITCH(opponent, 1);
            }
    }
}

AI_DOUBLE_BATTLE_TEST("AI will not try to switch for the same pokemon for 2 spots in a double battle")
{
    u32 flags;

    PARAMETRIZE {flags = AI_FLAG_SMART_SWITCHING; }
    PARAMETRIZE {flags = 0; }

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | flags);
        PLAYER(SPECIES_RATTATA);
        PLAYER(SPECIES_RATTATA);
        // No moves to damage player.
        OPPONENT(SPECIES_GENGAR) { Moves(MOVE_SHADOW_BALL); }
        OPPONENT(SPECIES_HAUNTER) { Moves(MOVE_SHADOW_BALL); }
        OPPONENT(SPECIES_GENGAR) { Moves(MOVE_SHADOW_BALL); }
        OPPONENT(SPECIES_RATICATE) { Moves(MOVE_HEADBUTT); }
    } WHEN {
        TURN { EXPECT_SWITCH(opponentLeft, 3); };
    } SCENE {
        MESSAGE("{PKMN} TRAINER LEAF withdrew Gengar!");
        MESSAGE("{PKMN} TRAINER LEAF sent out Raticate!");
        NONE_OF {
            MESSAGE("{PKMN} TRAINER LEAF withdrew Haunter!");
            MESSAGE("{PKMN} TRAINER LEAF sent out Raticate!");
        }
    }
}
