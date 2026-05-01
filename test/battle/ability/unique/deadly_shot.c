#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Deadly Shot increases super-effective move damage by 35 percent", s16 damage)
{
    u16 uniqueAbility;
    u32 move;

    PARAMETRIZE { uniqueAbility = ABILITY_DEADLY_SHOT; move = MOVE_DARK_PULSE; }
    PARAMETRIZE { uniqueAbility = ABILITY_ALGAE_BLOOM; move = MOVE_DARK_PULSE; }
    PARAMETRIZE { uniqueAbility = ABILITY_DEADLY_SHOT; move = MOVE_TACKLE; }
    PARAMETRIZE { uniqueAbility = ABILITY_ALGAE_BLOOM; move = MOVE_TACKLE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_DARK_PULSE].type == TYPE_DARK);
        ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
        PLAYER(SPECIES_CLAWITZER) { Ability(ABILITY_MEGA_LAUNCHER); UniqueAbility(uniqueAbility); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, UQ_4_12(1.35), results[0].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}
