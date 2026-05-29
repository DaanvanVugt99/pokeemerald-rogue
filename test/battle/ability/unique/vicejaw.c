#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BITE].power > 0);
    ASSUME(gBattleMoves[MOVE_BITE].makesContact);
    ASSUME(gBattleMoves[MOVE_CRUNCH].power > 0);
    ASSUME(gBattleMoves[MOVE_FIRE_FANG].power > 0);
    ASSUME(gBattleMoves[MOVE_JAW_LOCK].bitingMove);
}

SINGLE_BATTLE_TEST("Vicejaw uses a random reduced-power biting move after Bite or Crunch")
{
    u16 triggerMove;
    u16 calledMove;
    s16 damage;

    PARAMETRIZE { triggerMove = MOVE_BITE; calledMove = MOVE_CRUNCH; }
    PARAMETRIZE { triggerMove = MOVE_CRUNCH; calledMove = MOVE_FIRE_FANG; }

    GIVEN {
        PLAYER(SPECIES_MAWILE) { Ability(ABILITY_HYPER_CUTTER); UniqueAbility(ABILITY_VICEJAW); Moves(triggerMove); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Ability(ABILITY_SHIELD_DUST); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, triggerMove, WITH_RNG(RNG_ROGUE_VICEJAW, calledMove)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, triggerMove, player);
        ABILITY_POPUP(player, ABILITY_VICEJAW);
        ANIMATION(ANIM_TYPE_MOVE, calledMove, player);
        HP_BAR(opponent, captureDamage: &damage);
    } THEN {
        EXPECT_GT(damage, 0);
    }
}

SINGLE_BATTLE_TEST("Vicejaw does not trigger after Vise Grip")
{
    GIVEN {
        PLAYER(SPECIES_MAWILE) { Ability(ABILITY_HYPER_CUTTER); UniqueAbility(ABILITY_VICEJAW); Moves(MOVE_VISE_GRIP); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_VISE_GRIP, WITH_RNG(RNG_ROGUE_VICEJAW, MOVE_FIRE_FANG)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_VISE_GRIP, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_VICEJAW);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FIRE_FANG, player);
        }
    } THEN {
        EXPECT(gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].hp < gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].maxHP);
    }
}

SINGLE_BATTLE_TEST("Vicejaw still triggers after the target's contact ability")
{
    GIVEN {
        PLAYER(SPECIES_MAWILE) { Ability(ABILITY_HYPER_CUTTER); UniqueAbility(ABILITY_VICEJAW); Moves(MOVE_BITE); }
        OPPONENT(SPECIES_BARBARACLE) { HP(1000); MaxHP(1000); Ability(ABILITY_TOUGH_CLAWS); UniqueAbility(ABILITY_BARNACLE_WALL); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE, WITH_RNG(RNG_ROGUE_VICEJAW, MOVE_FIRE_FANG)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, player);
        ABILITY_POPUP(opponent, ABILITY_BARNACLE_WALL);
        ABILITY_POPUP(player, ABILITY_VICEJAW);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FIRE_FANG, player);
    } THEN {
        EXPECT(gStatuses4[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)] & STATUS4_SALT_CURE);
        EXPECT(gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].hp < gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].maxHP);
    }
}

SINGLE_BATTLE_TEST("Vicejaw follows up with one-quarter of the selected biting move's base power", s16 damage)
{
    bool32 vicejaw;

    PARAMETRIZE { vicejaw = FALSE; }
    PARAMETRIZE { vicejaw = TRUE; }

    GIVEN {
        if (vicejaw)
            PLAYER(SPECIES_MAWILE) { Level(50); Attack(100); Ability(ABILITY_HYPER_CUTTER); UniqueAbility(ABILITY_VICEJAW); Moves(MOVE_FIRE_FANG, MOVE_BITE); }
        else
            PLAYER(SPECIES_WOBBUFFET) { Level(50); Attack(100); Ability(ABILITY_HYPER_CUTTER); Moves(MOVE_FIRE_FANG, MOVE_BITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); HP(1000); MaxHP(1000); Defense(100); Ability(ABILITY_SHIELD_DUST); Moves(MOVE_CELEBRATE); }
    } WHEN {
        if (vicejaw)
            TURN { MOVE(player, MOVE_BITE, WITH_RNG(RNG_ROGUE_VICEJAW, MOVE_FIRE_FANG)); MOVE(opponent, MOVE_CELEBRATE); }
        else
            TURN { MOVE(player, MOVE_FIRE_FANG); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        if (vicejaw)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, player);
            HP_BAR(opponent);
            ABILITY_POPUP(player, ABILITY_VICEJAW);
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FIRE_FANG, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_LT(results[1].damage, results[0].damage);
        EXPECT_GT(results[1].damage, 0);
    }
}
