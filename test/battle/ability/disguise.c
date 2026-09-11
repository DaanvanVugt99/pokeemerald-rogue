#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Disguise shields the spider line without changing species")
{
    u16 species, move;
    PARAMETRIZE { species = SPECIES_TAROUNTULA; move = MOVE_TACKLE; }
    PARAMETRIZE { species = SPECIES_SPIDOPS; move = MOVE_TACKLE; }
    PARAMETRIZE { species = SPECIES_TAROUNTULA; move = MOVE_WATER_GUN; }
    PARAMETRIZE { species = SPECIES_SPIDOPS; move = MOVE_WATER_GUN; }
    GIVEN {
        PLAYER(species) { Ability(ABILITY_DISGUISE); HP(400); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, move); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DISGUISE);
        HP_BAR(player, damage: 50);
    } THEN {
        EXPECT_EQ(player->species, species);
        EXPECT_EQ(player->hp, 350);
        EXPECT(!IsBattlerDisguiseIntact(B_POSITION_PLAYER_LEFT));
        EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_SPECIES), species);
    }
}

SINGLE_BATTLE_TEST("Disguise on spiders stays broken after switching, with independent party shields")
{
    GIVEN {
        PLAYER(SPECIES_SPIDOPS) { Ability(ABILITY_DISGUISE); HP(400); MaxHP(400); }
        PLAYER(SPECIES_TAROUNTULA) { Ability(ABILITY_DISGUISE); HP(400); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_TACKLE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DISGUISE);
        HP_BAR(player, damage: 50);
        ABILITY_POPUP(player, ABILITY_DISGUISE);
        HP_BAR(player, damage: 50);
        NOT ABILITY_POPUP(player, ABILITY_DISGUISE);
        HP_BAR(player);
    } THEN {
        EXPECT_LT(player->hp, 350);
        EXPECT_EQ(player->species, SPECIES_SPIDOPS);
        EXPECT_EQ(GetMonData(&gPlayerParty[1], MON_DATA_SPECIES), SPECIES_TAROUNTULA);
    }
}

SINGLE_BATTLE_TEST("Disguise on spiders shares party storage without consuming other transformation abilities")
{
    GIVEN {
        PLAYER(SPECIES_PALAFIN_ZERO) { Ability(ABILITY_ZERO_TO_HERO); }
        PLAYER(SPECIES_SPIDOPS) { Ability(ABILITY_DISGUISE); HP(400); MaxHP(400); }
        PLAYER(SPECIES_DITTO) { Ability(ABILITY_LIMBER); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_TACKLE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 2); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TRANSFORM); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DISGUISE);
        HP_BAR(player, damage: 50);
        ABILITY_POPUP(player, ABILITY_ZERO_TO_HERO);
        ABILITY_POPUP(player, ABILITY_RAPID_REPLICA);
        NOT ABILITY_POPUP(player, ABILITY_DISGUISE);
        HP_BAR(player);
    } THEN {
        EXPECT(!IsBattlerDisguiseIntact(B_POSITION_PLAYER_LEFT));
    }
}

SINGLE_BATTLE_TEST("Disguise on spiders cannot shield a transformed Ditto")
{
    GIVEN {
        PLAYER(SPECIES_DITTO) { Ability(ABILITY_LIMBER); }
        OPPONENT(SPECIES_SPIDOPS) { Ability(ABILITY_DISGUISE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TRANSFORM); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_RAPID_REPLICA);
        NOT ABILITY_POPUP(player, ABILITY_DISGUISE);
        HP_BAR(player);
    } THEN {
        EXPECT(!IsBattlerDisguiseIntact(B_POSITION_PLAYER_LEFT));
        EXPECT(IsBattlerDisguiseIntact(B_POSITION_OPPONENT_LEFT));
    }
}

SINGLE_BATTLE_TEST("Disguise on spiders blocks only the first hit of a multi-hit move")
{
    GIVEN {
        PLAYER(SPECIES_SPIDOPS) { Ability(ABILITY_DISGUISE); HP(400); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_DOUBLE_KICK); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DISGUISE);
        HP_BAR(player, damage: 50);
        NOT ABILITY_POPUP(player, ABILITY_DISGUISE);
        HP_BAR(player);
    } THEN {
        EXPECT_LT(player->hp, 350);
        EXPECT_EQ(player->species, SPECIES_SPIDOPS);
    }
}

SINGLE_BATTLE_TEST("Disguise on spiders survives hazards and passive damage")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_SPIDOPS) { Ability(ABILITY_DISGUISE); HP(400); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_STEALTH_ROCK); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_WILL_O_WISP); }
    } SCENE {
        HP_BAR(player);
        NOT ABILITY_POPUP(player, ABILITY_DISGUISE);
    } THEN {
        EXPECT_LT(player->hp, 400);
        EXPECT(player->status1 & STATUS1_BURN);
        EXPECT(IsBattlerDisguiseIntact(B_POSITION_PLAYER_LEFT));
    }
}

SINGLE_BATTLE_TEST("Disguise on spiders is bypassed but not consumed by Mold Breaker")
{
    GIVEN {
        PLAYER(SPECIES_SPIDOPS) { Ability(ABILITY_DISGUISE); HP(400); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_MOLD_BREAKER); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_DISGUISE);
        HP_BAR(player);
    } THEN {
        EXPECT_LT(player->hp, 400);
        EXPECT(IsBattlerDisguiseIntact(B_POSITION_PLAYER_LEFT));
    }
}

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_AERIAL_ACE].split == SPLIT_PHYSICAL);
}

SINGLE_BATTLE_TEST("Disguised Mimikyu will lose 1/8 of its max HP upon changing to its busted form")
{
    s16 disguiseDamage;

    GIVEN {
        PLAYER(SPECIES_MIMIKYU_DISGUISED) { Ability(ABILITY_DISGUISE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_AERIAL_ACE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, opponent);
        ABILITY_POPUP(player, ABILITY_DISGUISE);
        HP_BAR(player, captureDamage: &disguiseDamage);
    } THEN {
        EXPECT_EQ(player->species, SPECIES_MIMIKYU_BUSTED);
        EXPECT_EQ(disguiseDamage, player->maxHP / 8);
    }
}

SINGLE_BATTLE_TEST("Disguised Mimikyu takes no damage from a confusion hit and changes to its busted form")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_CONFUSE_RAY].effect == EFFECT_CONFUSE);
        PLAYER(SPECIES_MIMIKYU_DISGUISED) { Ability(ABILITY_DISGUISE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_CONFUSE_RAY); }
        TURN { }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CONFUSE_RAY, opponent);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_CONFUSION, player);
        MESSAGE("Mimikyu became confused!");
        MESSAGE("Mimikyu is confused!");
        MESSAGE("It hurt itself in its confusion!");
        NOT HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_DISGUISE);
    } THEN {
        EXPECT_EQ(player->species, SPECIES_MIMIKYU_BUSTED);
    }
}

SINGLE_BATTLE_TEST("Disguised Mimikyu's Air Balloon will pop upon changing to its busted form")
{
    GIVEN {
        ASSUME(gItems[ITEM_AIR_BALLOON].holdEffect == HOLD_EFFECT_AIR_BALLOON);
        PLAYER(SPECIES_MIMIKYU_DISGUISED) { Ability(ABILITY_DISGUISE); Item(ITEM_AIR_BALLOON); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_AERIAL_ACE); }
    } SCENE {
        MESSAGE("Mimikyu floats in the air with its Air Balloon!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, opponent);
        NOT HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_DISGUISE);
        MESSAGE("Mimikyu's Air Balloon popped!");
    } THEN {
        EXPECT_EQ(player->species, SPECIES_MIMIKYU_BUSTED);
    }
}

SINGLE_BATTLE_TEST("Disguised Mimikyu takes damage from secondary damage without breaking the disguise")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_STEALTH_ROCK].effect == EFFECT_STEALTH_ROCK);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_MIMIKYU_DISGUISED) { Ability(ABILITY_DISGUISE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_STEALTH_ROCK); }
        TURN { SWITCH(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STEALTH_ROCK, opponent);
        HP_BAR(player);
        MESSAGE("Pointed stones dug into Mimikyu!");
    } THEN {
        EXPECT_EQ(player->species, SPECIES_MIMIKYU_DISGUISED);
    }
}

SINGLE_BATTLE_TEST("Disguised Mimikyu takes damage from Rocky Helmet without breaking the disguise")
{
    GIVEN {
        ASSUME(gItems[ITEM_ROCKY_HELMET].holdEffect == HOLD_EFFECT_ROCKY_HELMET);
        PLAYER(SPECIES_MIMIKYU_DISGUISED) { Ability(ABILITY_DISGUISE); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_ROCKY_HELMET); }
    } WHEN {
        TURN { MOVE(player, MOVE_AERIAL_ACE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, opponent);
        HP_BAR(player);
        MESSAGE("Mimikyu was hurt by Foe Wobbuffet's Rocky Helmet!");
    } THEN {
        EXPECT_EQ(player->species, SPECIES_MIMIKYU_DISGUISED);
    }
}

SINGLE_BATTLE_TEST("Disguised Mimikyu takes damage from Rough Skin without breaking the disguise")
{
    GIVEN {
        PLAYER(SPECIES_MIMIKYU_DISGUISED) { Ability(ABILITY_DISGUISE); }
        OPPONENT(SPECIES_CARVANHA) { Ability(ABILITY_ROUGH_SKIN); }
    } WHEN {
        TURN { MOVE(player, MOVE_AERIAL_ACE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, player);
        HP_BAR(opponent);
        ABILITY_POPUP(opponent, ABILITY_ROUGH_SKIN);
        HP_BAR(player);
        MESSAGE("Mimikyu was hurt by Foe Carvanha's Rough Skin!");
    } THEN {
        EXPECT_EQ(player->species, SPECIES_MIMIKYU_DISGUISED);
    }
}
