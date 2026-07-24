#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
    ASSUME(!gBattleMoves[MOVE_WATER_GUN].makesContact);
    ASSUME(gBattleMoves[MOVE_RECOVER].effect == EFFECT_RESTORE_HP);
}

SINGLE_BATTLE_TEST("Numbing Spines applies Heal Block to contact attackers")
{
    GIVEN {
        PLAYER(SPECIES_TOXAPEX) { Ability(ABILITY_MERCILESS); UniqueAbility(ABILITY_NUMBING_SPINES); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(50); MaxHP(100); Moves(MOVE_TACKLE, MOVE_RECOVER); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ABILITY_POPUP(player, ABILITY_NUMBING_SPINES);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HEAL_BLOCK, opponent);
        MESSAGE("Foe Wobbuffet was prevented\nfrom healing!");
    } THEN {
        EXPECT(gStatuses3[B_POSITION_OPPONENT_LEFT] & STATUS3_HEAL_BLOCK);
        EXPECT_EQ(opponent->hp, 50);
    }
}

SINGLE_BATTLE_TEST("Numbing Spines does not apply Heal Block to non-contact attackers")
{
    GIVEN {
        PLAYER(SPECIES_TOXAPEX) { Ability(ABILITY_MERCILESS); UniqueAbility(ABILITY_NUMBING_SPINES); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_NUMBING_SPINES);
            MESSAGE("Foe Wobbuffet was prevented\nfrom healing!");
        }
    } THEN {
        EXPECT(!(gStatuses3[B_POSITION_OPPONENT_LEFT] & STATUS3_HEAL_BLOCK));
    }
}
