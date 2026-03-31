#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
}

SINGLE_BATTLE_TEST("Immolate converts Normal-type moves to Fire-type")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_IMMOLATE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_HOUNDOOM) { Ability(ABILITY_FLASH_FIRE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_FLASH_FIRE);
        NONE_OF {
            HP_BAR(opponent);
        }
    }
}

SINGLE_BATTLE_TEST("Immolate has no extra -ate damage boost", s16 damage)
{
    u16 ability;
    u16 uniqueAbility;
    PARAMETRIZE { ability = ABILITY_GALVANIZE; uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_SHADOW_TAG; uniqueAbility = ABILITY_IMMOLATE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); UniqueAbility(uniqueAbility); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(1.2), results[0].damage);
    }
}
