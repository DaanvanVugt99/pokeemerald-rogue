#include "global.h"
#include "battle_util.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("King's Domain prevents loafing if the party shares a type")
{
    GIVEN {
        PLAYER(SPECIES_SLAKING) { Ability(ABILITY_TRUANT); Moves(MOVE_SONIC_BOOM, MOVE_SONIC_BOOM); }
        PLAYER(SPECIES_SLAKOTH) { Ability(ABILITY_TRUANT); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SONIC_BOOM); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SONIC_BOOM); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->hp, 60);
    }
}

SINGLE_BATTLE_TEST("King's Domain does not prevent loafing if the party does not share a type")
{
    GIVEN {
        PLAYER(SPECIES_SLAKING) { Ability(ABILITY_TRUANT); Moves(MOVE_SONIC_BOOM, MOVE_SONIC_BOOM); }
        PLAYER(SPECIES_ODDISH) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SONIC_BOOM); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SONIC_BOOM); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TRUANT);
        MESSAGE("Slaking is loafing around!");
    } THEN {
        EXPECT_EQ(opponent->hp, 80);
    }
}

SINGLE_BATTLE_TEST("King's Domain suppresses Slow Start Attack and Speed only on shared-type teams", s16 damage)
{
    bool32 sharedType;
    PARAMETRIZE { sharedType = FALSE; }
    PARAMETRIZE { sharedType = TRUE; }

    GIVEN {
        PLAYER(SPECIES_SLAKING) { Ability(ABILITY_SLOW_START); Attack(100); Speed(100); Moves(MOVE_TACKLE); }
        PLAYER(sharedType ? SPECIES_SLAKOTH : SPECIES_ODDISH) { Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(75); Defense(100); MaxHP(500); HP(500); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        if (!sharedType) {
            ABILITY_POPUP(player, ABILITY_SLOW_START);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
        if (sharedType) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        }
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_POSITION_PLAYER_LEFT), sharedType ? ABILITY_NONE : ABILITY_SLOW_START);
        EXPECT_EQ(GetBattlerUniqueAbility(B_POSITION_PLAYER_LEFT), ABILITY_KINGS_DOMAIN);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(2.0), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("King's Domain suppresses the regular slot rather than just its native drawbacks", s16 damage)
{
    u16 uniqueAbility;
    PARAMETRIZE { uniqueAbility = ABILITY_KEEN_EYE; }
    PARAMETRIZE { uniqueAbility = ABILITY_KINGS_DOMAIN; }

    GIVEN {
        PLAYER(SPECIES_SLAKING) { Ability(ABILITY_HUGE_POWER); UniqueAbility(uniqueAbility); Attack(100); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_SLAKOTH) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(100); MaxHP(500); HP(500); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, UQ_4_12(2.0), results[0].damage);
    }
}
