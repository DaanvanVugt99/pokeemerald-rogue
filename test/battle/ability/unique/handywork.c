#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Handywork uses 20 BP Covet and steals the target's item")
{
    GIVEN {
        PLAYER(SPECIES_AIPOM) { Level(50); Attack(100); Ability(ABILITY_RUN_AWAY); UniqueAbility(ABILITY_HANDYWORK); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(100); HP(400); MaxHP(400); Item(ITEM_LEFTOVERS); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_HANDYWORK);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_COVET, player);
        HP_BAR(opponent, damage: 10);
    } THEN {
        EXPECT_EQ(player->item, ITEM_LEFTOVERS);
        EXPECT_EQ(opponent->item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Handywork waits for contact and refreshes only after switching")
{
    GIVEN {
        PLAYER(SPECIES_AIPOM) { Speed(100); Attack(60); Ability(ABILITY_RUN_AWAY); UniqueAbility(ABILITY_HANDYWORK); Moves(MOVE_SCRATCH, MOVE_SWIFT); }
        PLAYER(SPECIES_ODDISH) { Speed(20); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); HP(1000); MaxHP(1000); Ability(ABILITY_TELEPATHY); Moves(MOVE_PROTECT, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWIFT); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_SCRATCH, hit: FALSE); }
        TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_HANDYWORK);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWIFT, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        NOT ABILITY_POPUP(player, ABILITY_HANDYWORK);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        ABILITY_POPUP(player, ABILITY_HANDYWORK);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_COVET, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        NOT ABILITY_POPUP(player, ABILITY_HANDYWORK);
        MESSAGE("Go! Oddish!");
        MESSAGE("Go! Aipom!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        ABILITY_POPUP(player, ABILITY_HANDYWORK);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_COVET, player);
    }
}

SINGLE_BATTLE_TEST("Handywork still triggers after the target's contact ability")
{
    GIVEN {
        PLAYER(SPECIES_AIPOM) { Ability(ABILITY_RUN_AWAY); UniqueAbility(ABILITY_HANDYWORK); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_BARBARACLE) { HP(1000); MaxHP(1000); Ability(ABILITY_TOUGH_CLAWS); UniqueAbility(ABILITY_BARNACLE_WALL); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        ABILITY_POPUP(opponent, ABILITY_BARNACLE_WALL);
        ABILITY_POPUP(player, ABILITY_HANDYWORK);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_COVET, player);
    } THEN {
        EXPECT(gStatuses4[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)] & STATUS4_SALT_CURE);
    }
}
