#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_STICKY_WEB].effect == EFFECT_STICKY_WEB);
    ASSUME(gItems[ITEM_WHITE_HERB].holdEffect == HOLD_EFFECT_RESTORE_STATS);
}

SINGLE_BATTLE_TEST("Tragic Beauty gives the user burn, poison, or paralysis on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_MILOTIC) { Ability(ABILITY_MARVEL_SCALE); UniqueAbility(ABILITY_TRAGIC_BEAUTY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TRAGIC_BEAUTY);
    } THEN {
        EXPECT(player->status1 == STATUS1_BURN
            || player->status1 == STATUS1_POISON
            || player->status1 == STATUS1_PARALYSIS);
    }
}

SINGLE_BATTLE_TEST("Tragic Beauty continues switch-in effects after statusing the user")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_STICKY_WEB, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MILOTIC) { Ability(ABILITY_MARVEL_SCALE); UniqueAbility(ABILITY_TRAGIC_BEAUTY); Item(ITEM_WHITE_HERB); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_STICKY_WEB); }
        TURN { SWITCH(opponent, 1); }
        TURN {}
    } SCENE {
        MESSAGE("Foe Milotic was caught in a Sticky Web!");
        ABILITY_POPUP(opponent, ABILITY_TRAGIC_BEAUTY);
        MESSAGE("Foe Milotic's White Herb restored its status!");
    } THEN {
        EXPECT_EQ(opponent->item, ITEM_NONE);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT(opponent->status1 == STATUS1_BURN
            || opponent->status1 == STATUS1_POISON
            || opponent->status1 == STATUS1_PARALYSIS);
    }
}

SINGLE_BATTLE_TEST("Tragic Beauty restores 1/8 max HP at end of turn")
{
    GIVEN {
        PLAYER(SPECIES_MILOTIC) { Ability(ABILITY_MARVEL_SCALE); UniqueAbility(ABILITY_TRAGIC_BEAUTY); HP(350); MaxHP(400); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TRAGIC_BEAUTY);
        MESSAGE("Milotic's Tragic Beauty restored its HP a little!");
        HP_BAR(player, damage: -50);
    }
}
