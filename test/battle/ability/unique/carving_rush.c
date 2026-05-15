#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_FILLET_AWAY].effect == EFFECT_FILLET_AWAY);
    ASSUME(gBattleMoves[MOVE_LEAF_BLADE].slicingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].slicingMove);
}

SINGLE_BATTLE_TEST("Carving Rush makes slicing moves restore 1/4 damage dealt after Fillet Away")
{
    s16 damage;
    s16 healed;

    GIVEN {
        PLAYER(SPECIES_VELUZA) { HP(160); MaxHP(200); Speed(100); Moves(MOVE_FILLET_AWAY, MOVE_LEAF_BLADE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FILLET_AWAY); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_LEAF_BLADE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FILLET_AWAY, player);
        ABILITY_POPUP(player, ABILITY_CARVING_RUSH);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEAF_BLADE, player);
        HP_BAR(opponent, captureDamage: &damage);
        ABILITY_POPUP(player, ABILITY_CARVING_RUSH);
        HP_BAR(player, captureDamage: &healed);
        MESSAGE("Foe Wobbuffet had its energy drained!");
    } THEN {
        EXPECT_MUL_EQ(damage, Q_4_12(-1.0 / 4.0), healed);
    }
}

SINGLE_BATTLE_TEST("Carving Rush does not restore HP from non-slicing moves after Fillet Away")
{
    GIVEN {
        PLAYER(SPECIES_VELUZA) { HP(160); MaxHP(200); Speed(100); Moves(MOVE_FILLET_AWAY, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FILLET_AWAY); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->hp, 60);
    }
}

SINGLE_BATTLE_TEST("Carving Rush effect ends when Veluza switches out")
{
    GIVEN {
        PLAYER(SPECIES_VELUZA) { HP(160); MaxHP(200); Speed(100); Moves(MOVE_FILLET_AWAY, MOVE_LEAF_BLADE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_TENTACRUEL) { Ability(ABILITY_LIQUID_OOZE); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FILLET_AWAY); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_LEAF_BLADE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->hp, 150);
    }
}
