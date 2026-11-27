#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Cross Training swaps both types for Lopunny and Mega Lopunny")
{
    bool32 mega;
    u16 move;
    PARAMETRIZE { mega = FALSE; move = MOVE_ICE_PUNCH; }
    PARAMETRIZE { mega = TRUE;  move = MOVE_ICE_PUNCH; }
    PARAMETRIZE { mega = FALSE; move = MOVE_BLAZE_KICK; }
    PARAMETRIZE { mega = TRUE;  move = MOVE_BLAZE_KICK; }
    GIVEN {
        PLAYER(SPECIES_LOPUNNY) { Item(ITEM_LOPUNNITE); UniqueAbility(ABILITY_CROSS_TRAINING); Moves(MOVE_ICE_PUNCH, MOVE_BLAZE_KICK, MOVE_RETURN, MOVE_AGILITY); }
        OPPONENT(SPECIES_FLAREON) { Ability(ABILITY_FLASH_FIRE); }
    } WHEN {
        TURN { MOVE(player, move, megaEvolve: mega); }
    } SCENE {
        if (move == MOVE_ICE_PUNCH)
            ABILITY_POPUP(opponent, ABILITY_FLASH_FIRE);
        else
            HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->species, mega ? SPECIES_LOPUNNY_MEGA : SPECIES_LOPUNNY);
    }
}

SINGLE_BATTLE_TEST("Cross Training grants STAB to both moves only with one of each", s16 damage)
{
    u16 move, extra;
    PARAMETRIZE { move = MOVE_ICE_PUNCH; extra = MOVE_CELEBRATE; }
    PARAMETRIZE { move = MOVE_ICE_PUNCH; extra = MOVE_DRAIN_PUNCH; }
    PARAMETRIZE { move = MOVE_BLAZE_KICK; extra = MOVE_CELEBRATE; }
    PARAMETRIZE { move = MOVE_BLAZE_KICK; extra = MOVE_DOUBLE_KICK; }
    GIVEN {
        PLAYER(SPECIES_LOPUNNY) { Ability(ABILITY_NO_GUARD); UniqueAbility(ABILITY_CROSS_TRAINING); Moves(MOVE_ICE_PUNCH, MOVE_BLAZE_KICK, extra); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, move, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(1.5), results[0].damage);
        EXPECT_MUL_EQ(results[3].damage, Q_4_12(1.5), results[2].damage);
    }
}
