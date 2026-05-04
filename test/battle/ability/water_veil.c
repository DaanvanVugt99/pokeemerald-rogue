#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Water Veil sets Aqua Ring on entry and heals at end of turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(150); MaxHP(160); Ability(ABILITY_WATER_VEIL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_WATER_VEIL);
        MESSAGE("Wobbuffet surrounded itself\nwith a veil of water!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_AQUA_RING_HEAL, player);
        MESSAGE("Aqua Ring restored\nWobbuffet's HP!");
        HP_BAR(player);
    } THEN {
        EXPECT_EQ(gStatuses3[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)] & STATUS3_AQUA_RING, STATUS3_AQUA_RING);
        EXPECT_EQ(player->hp, player->maxHP);
    }
}
