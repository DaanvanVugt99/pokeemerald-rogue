#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gSpeciesInfo[SPECIES_ZACIAN_HERO_OF_MANY_BATTLES].types[0] == TYPE_FAIRY || gSpeciesInfo[SPECIES_ZACIAN_HERO_OF_MANY_BATTLES].types[1] == TYPE_FAIRY);
    ASSUME(gSpeciesInfo[SPECIES_ZAMAZENTA_HERO_OF_MANY_BATTLES].types[0] == TYPE_FIGHTING || gSpeciesInfo[SPECIES_ZAMAZENTA_HERO_OF_MANY_BATTLES].types[1] == TYPE_FIGHTING);
    ASSUME(gSpeciesInfo[SPECIES_CLEFAIRY].types[0] == TYPE_FAIRY || gSpeciesInfo[SPECIES_CLEFAIRY].types[1] == TYPE_FAIRY);
    ASSUME(gSpeciesInfo[SPECIES_MACHOP].types[0] == TYPE_FIGHTING || gSpeciesInfo[SPECIES_MACHOP].types[1] == TYPE_FIGHTING);
    ASSUME(gBattleMoves[MOVE_DISARMING_VOICE].type == TYPE_FAIRY);
    ASSUME(gBattleMoves[MOVE_DISARMING_VOICE].power > 0);
    ASSUME(gBattleMoves[MOVE_PSYCHIC].type == TYPE_PSYCHIC);
    ASSUME(gBattleMoves[MOVE_PSYCHIC].power > 0);
}

SINGLE_BATTLE_TEST("Duelist's Law doubles resisted damage when Zacian's party shares a type", s16 damage)
{
    bool32 sharedParty;

    PARAMETRIZE { sharedParty = FALSE; }
    PARAMETRIZE { sharedParty = TRUE; }

    GIVEN {
        PLAYER(SPECIES_ZACIAN_HERO_OF_MANY_BATTLES) { Speed(100); Ability(ABILITY_INTREPID_SWORD); Moves(MOVE_DISARMING_VOICE); }
        PLAYER(sharedParty ? SPECIES_CLEFAIRY : SPECIES_CHARMANDER) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CHARMANDER) { HP(300); MaxHP(300); Speed(50); }
    } WHEN {
        TURN { MOVE(player, MOVE_DISARMING_VOICE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(2.0), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Shield Wall caps damage at half HP when Zamazenta's party shares a type", s16 damage)
{
    bool32 sharedParty;

    PARAMETRIZE { sharedParty = FALSE; }
    PARAMETRIZE { sharedParty = TRUE; }

    GIVEN {
        PLAYER(SPECIES_ZAMAZENTA_HERO_OF_MANY_BATTLES) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_DAUNTLESS_SHIELD); Moves(MOVE_CELEBRATE); }
        PLAYER(sharedParty ? SPECIES_MACHOP : SPECIES_CHARMANDER) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MEWTWO) { Speed(100); SpAttack(999); Moves(MOVE_PSYCHIC); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PSYCHIC); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[0].damage, 500);
        EXPECT_EQ(results[1].damage, 500);
    }
}
