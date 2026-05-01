#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TECHNO_BLAST].effect == EFFECT_CHANGE_TYPE_ON_ITEM);
    ASSUME(gBattleMoves[MOVE_TECHNO_BLAST].argument == HOLD_EFFECT_DRIVE);
    ASSUME(gBattleMoves[MOVE_TACKLE].effect == EFFECT_HIT);
}

SINGLE_BATTLE_TEST("Modular changes only Genesect's primary type to match its Drive")
{
    GIVEN {
        PLAYER(SPECIES_GENESECT) { Ability(ABILITY_DOWNLOAD); UniqueAbility(ABILITY_MODULAR); Item(ITEM_DOUSE_DRIVE); Moves(MOVE_TECHNO_BLAST); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TECHNO_BLAST); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MODULAR);
        MESSAGE("Genesect's main type changed to Water!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TECHNO_BLAST, player);
    } THEN {
        EXPECT_EQ(player->type1, TYPE_WATER);
        EXPECT_EQ(player->type2, TYPE_STEEL);
        EXPECT_EQ(player->type3, TYPE_MYSTERY);
    }
}

SINGLE_BATTLE_TEST("Modular supports every Drive type")
{
    u16 item;
    u8 type;

    PARAMETRIZE { item = ITEM_DOUSE_DRIVE; type = TYPE_WATER; }
    PARAMETRIZE { item = ITEM_SHOCK_DRIVE; type = TYPE_ELECTRIC; }
    PARAMETRIZE { item = ITEM_BURN_DRIVE; type = TYPE_FIRE; }
    PARAMETRIZE { item = ITEM_CHILL_DRIVE; type = TYPE_ICE; }

    GIVEN {
        PLAYER(SPECIES_GENESECT) { Ability(ABILITY_DOWNLOAD); UniqueAbility(ABILITY_MODULAR); Item(item); Moves(MOVE_TECHNO_BLAST); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TECHNO_BLAST); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->type1, type);
        EXPECT_EQ(player->type2, TYPE_STEEL);
        EXPECT_EQ(player->type3, TYPE_MYSTERY);
    }
}

SINGLE_BATTLE_TEST("Modular does not change type when using moves other than Techno Blast")
{
    GIVEN {
        PLAYER(SPECIES_GENESECT) { Ability(ABILITY_DOWNLOAD); UniqueAbility(ABILITY_MODULAR); Item(ITEM_DOUSE_DRIVE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_MODULAR);
        }
    } THEN {
        EXPECT_EQ(player->type1, TYPE_BUG);
        EXPECT_EQ(player->type2, TYPE_STEEL);
        EXPECT_EQ(player->type3, TYPE_MYSTERY);
    }
}

SINGLE_BATTLE_TEST("Modular type change resets after switching out")
{
    GIVEN {
        PLAYER(SPECIES_GENESECT) { Ability(ABILITY_DOWNLOAD); UniqueAbility(ABILITY_MODULAR); Item(ITEM_DOUSE_DRIVE); Moves(MOVE_TECHNO_BLAST); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TECHNO_BLAST); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->type1, TYPE_BUG);
        EXPECT_EQ(player->type2, TYPE_STEEL);
        EXPECT_EQ(player->type3, TYPE_MYSTERY);
    }
}
