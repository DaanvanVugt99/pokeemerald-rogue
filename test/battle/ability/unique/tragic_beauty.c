#include "global.h"
#include "test/battle.h"

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
