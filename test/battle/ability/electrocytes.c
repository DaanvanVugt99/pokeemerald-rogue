#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Electrocytes gives STAB to Electric-type moves", s16 damage)
{
    u32 move;
    u16 ability;

    ASSUME(gBattleMoves[MOVE_THUNDERBOLT].type == TYPE_ELECTRIC); // Ensure move is Electric-type
    PARAMETRIZE
    {
        move = MOVE_THUNDERBOLT;
        ability = ABILITY_ELECTROCYTES;
    }
    PARAMETRIZE
    {
        move = MOVE_THUNDERBOLT;
        ability = ABILITY_OBLIVIOUS;
    }
    GIVEN
    {
        PLAYER(SPECIES_WHISCASH)
        {
            Ability(ability);
        }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(player, move); }
    }
    SCENE
    {
        HP_BAR(opponent, captureDamage : &results[i].damage);
    }
    FINALLY
    {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage); // Electrocytes grants STAB
    }
}

SINGLE_BATTLE_TEST("Electrocytes gives Water-type moves a 20% chance to paralyze")
{
    ASSUME(gBattleMoves[MOVE_WATER_PULSE].type == TYPE_WATER);
    GIVEN
    {
        PLAYER(SPECIES_WHISCASH) { Ability(ABILITY_ELECTROCYTES); }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(player, MOVE_WATER_PULSE); }
    }
    SCENE
    {
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PRZ, player);
        MESSAGE("Wobbuffet was paralyzed by Foe Whiscash's Water Pulse!");
        STATUS_ICON(player, paralysis : TRUE);
    }
}
