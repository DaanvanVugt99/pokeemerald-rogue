#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_RECOVER].effect == EFFECT_RESTORE_HP);
    ASSUME(gBattleMoves[MOVE_GRASSY_TERRAIN].effect == EFFECT_GRASSY_TERRAIN);
    ASSUME(gBattleMoves[MOVE_TEATIME].effect == EFFECT_TEATIME);
    ASSUME(gItems[ITEM_ORAN_BERRY].holdEffect == HOLD_EFFECT_RESTORE_HP);
    ASSUME(gItems[ITEM_LIECHI_BERRY].holdEffect == HOLD_EFFECT_ATTACK_UP);
}

SINGLE_BATTLE_TEST("Last Pour uses Teatime after restoring HP")
{
    GIVEN {
        PLAYER(SPECIES_SINISTCHA) { HP(50); MaxHP(100); Ability(ABILITY_HEATPROOF); UniqueAbility(ABILITY_LAST_POUR); Moves(MOVE_RECOVER); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_LIECHI_BERRY); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, player);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_LAST_POUR);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TEATIME, player);
        MESSAGE("Using Liechi Berry, the Attack of Foe Wobbuffet rose!");
    } THEN {
        EXPECT_EQ(opponent->item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Last Pour triggers from end-turn terrain healing")
{
    GIVEN {
        PLAYER(SPECIES_SINISTCHA) { HP(50); MaxHP(100); Ability(ABILITY_HEATPROOF); UniqueAbility(ABILITY_LAST_POUR); Moves(MOVE_GRASSY_TERRAIN); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_LIECHI_BERRY); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GRASSY_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Sinistcha is healed by the grassy terrain!");
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_LAST_POUR);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TEATIME, player);
    } THEN {
        EXPECT_EQ(opponent->item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Last Pour does not trigger when healing at full HP")
{
    GIVEN {
        PLAYER(SPECIES_SINISTCHA) { HP(100); MaxHP(100); Ability(ABILITY_HEATPROOF); UniqueAbility(ABILITY_LAST_POUR); Moves(MOVE_RECOVER); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_LIECHI_BERRY); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_LAST_POUR);
    } THEN {
        EXPECT_EQ(opponent->item, ITEM_LIECHI_BERRY);
    }
}

SINGLE_BATTLE_TEST("Last Pour waits for a healing Berry to be consumed before using Teatime")
{
    GIVEN {
        PLAYER(SPECIES_SINISTCHA) { HP(40); MaxHP(100); Ability(ABILITY_HEATPROOF); UniqueAbility(ABILITY_LAST_POUR); Item(ITEM_ORAN_BERRY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_LIECHI_BERRY); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_LAST_POUR);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TEATIME, player);
        MESSAGE("Using Liechi Berry, the Attack of Foe Wobbuffet rose!");
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(opponent->item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Last Pour does not activate from Regenerator after leaving battle")
{
    GIVEN {
        PLAYER(SPECIES_SINISTCHA) { HP(40); MaxHP(100); Ability(ABILITY_REGENERATOR); UniqueAbility(ABILITY_LAST_POUR); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_LIECHI_BERRY); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_LAST_POUR);
    } THEN {
        EXPECT_EQ(opponent->item, ITEM_LIECHI_BERRY);
    }
}

SINGLE_BATTLE_TEST("Last Pour activates only once per battle")
{
    GIVEN {
        PLAYER(SPECIES_SINISTCHA) { HP(1); MaxHP(100); Ability(ABILITY_HEATPROOF); UniqueAbility(ABILITY_LAST_POUR); Moves(MOVE_RECOVER); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(1); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_LAST_POUR);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_LAST_POUR);
        }
    }
}
