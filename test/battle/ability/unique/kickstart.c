#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_MEGA_KICK].kickingMove);
    ASSUME(gBattleMoves[MOVE_MEGA_KICK].type == TYPE_FIGHTING);
    ASSUME(!gBattleMoves[MOVE_TACKLE].kickingMove);
}

SINGLE_BATTLE_TEST("Kickstart scales kicking damage by qualifying ally count", s16 damage)
{
    u16 ally1, ally2, ally3;
    PARAMETRIZE { ally1 = SPECIES_WOBBUFFET; ally2 = SPECIES_WOBBUFFET; ally3 = SPECIES_WOBBUFFET; } // 0 allies
    PARAMETRIZE { ally1 = SPECIES_ELECTRIKE; ally2 = SPECIES_WOBBUFFET; ally3 = SPECIES_WOBBUFFET; } // 1 ally
    PARAMETRIZE { ally1 = SPECIES_ELECTRIKE; ally2 = SPECIES_MACHOP;    ally3 = SPECIES_WOBBUFFET; } // 2 allies
    PARAMETRIZE { ally1 = SPECIES_ELECTRIKE; ally2 = SPECIES_MACHOP;    ally3 = SPECIES_NUMEL; }     // 3 allies

    GIVEN {
        PLAYER(SPECIES_BLAZIKEN) { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_FLASH_FIRESTORM); Moves(MOVE_MEGA_KICK); }
        PLAYER(ally1);
        PLAYER(ally2);
        PLAYER(ally3);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_MEGA_KICK); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.1), results[1].damage);
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.2), results[2].damage);
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.3), results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Kickstart burns the target on the first kicking move after switch-in at 3 allies")
{
    GIVEN {
        PLAYER(SPECIES_BLAZIKEN) { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_FLASH_FIRESTORM); Moves(MOVE_MEGA_KICK); }
        PLAYER(SPECIES_ELECTRIKE);
        PLAYER(SPECIES_MACHOP);
        PLAYER(SPECIES_NUMEL);
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(1000); HP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MEGA_KICK); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_MEGA_KICK); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FLASH_FIRESTORM);
        STATUS_ICON(opponent, burn: TRUE);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_FLASH_FIRESTORM);
        }
    } THEN {
        EXPECT(opponent->status1 & STATUS1_BURN);
    }
}

SINGLE_BATTLE_TEST("Kickstart does not burn at fewer than 3 qualifying allies")
{
    GIVEN {
        PLAYER(SPECIES_BLAZIKEN) { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_FLASH_FIRESTORM); Moves(MOVE_MEGA_KICK); }
        PLAYER(SPECIES_ELECTRIKE);
        PLAYER(SPECIES_MACHOP);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MEGA_KICK); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_FLASH_FIRESTORM);
            STATUS_ICON(opponent, burn: TRUE);
        }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Kickstart does not burn on non-kicking moves")
{
    GIVEN {
        PLAYER(SPECIES_BLAZIKEN) { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_FLASH_FIRESTORM); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_ELECTRIKE);
        PLAYER(SPECIES_MACHOP);
        PLAYER(SPECIES_NUMEL);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_FLASH_FIRESTORM);
            STATUS_ICON(opponent, burn: TRUE);
        }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}
