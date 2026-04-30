#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Vengeful Force activates after an allied faint and boosts only the next damaging move", s16 firstDamage, s16 secondDamage)
{
    bool32 allyFainted;
    PARAMETRIZE { allyFainted = FALSE; }
    PARAMETRIZE { allyFainted = TRUE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].power != 0);
        PLAYER(SPECIES_WYNAUT) { HP(allyFainted ? 1 : 100); MaxHP(100); Speed(1); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_TERRAKION) { Ability(ABILITY_JUSTIFIED); UniqueAbility(ABILITY_VENGEFUL_FORCE); Attack(100); Speed(100); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(100); HP(1000); MaxHP(1000); Speed(1); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN {
        if (allyFainted) {
            TURN { MOVE(opponent, MOVE_TACKLE); SEND_OUT(player, 1); }
        } else {
            TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        if (allyFainted) {
            ABILITY_POPUP(player, ABILITY_VENGEFUL_FORCE);
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
            MESSAGE("Terrakion's Vengeful Force raised its Attack!");
        } else {
            NONE_OF {
                ABILITY_POPUP(player, ABILITY_VENGEFUL_FORCE);
            }
        }
        HP_BAR(opponent, captureDamage: &results[i].firstDamage);
        HP_BAR(opponent, captureDamage: &results[i].secondDamage);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], allyFainted ? DEFAULT_STAT_STAGE + 1 : DEFAULT_STAT_STAGE);
    } FINALLY {
        EXPECT_EQ(results[0].firstDamage, results[0].secondDamage);
        EXPECT_MUL_EQ(results[1].secondDamage, Q_4_12(1.5), results[1].firstDamage);
        EXPECT(results[1].secondDamage > results[0].secondDamage);
    }
}
