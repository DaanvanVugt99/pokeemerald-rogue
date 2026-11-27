#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Abyssal Maw uses the other bite once, preserving Psychic Fangs' screen removal")
{
    u16 opener, followUp;
    PARAMETRIZE { opener = MOVE_CRUNCH; followUp = MOVE_PSYCHIC_FANGS; }
    PARAMETRIZE { opener = MOVE_PSYCHIC_FANGS; followUp = MOVE_CRUNCH; }
    GIVEN {
        PLAYER(SPECIES_HUNTAIL) { Speed(1); Moves(MOVE_PSYCHIC_FANGS, MOVE_COIL, MOVE_CRUNCH, MOVE_SURF); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(800); MaxHP(800); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_REFLECT); MOVE(player, opener); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, opener, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_ABYSSAL_MAW);
        ANIMATION(ANIM_TYPE_MOVE, followUp, player);
        HP_BAR(opponent);
        NOT ABILITY_POPUP(player, ABILITY_ABYSSAL_MAW);
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_REFLECT));
    }
}

SINGLE_BATTLE_TEST("Abyssal Maw uses 40 percent of the partner bite's base power", s16 damage)
{
    bool32 combo;
    PARAMETRIZE { combo = FALSE; }
    PARAMETRIZE { combo = TRUE; }
    GIVEN {
        PLAYER(SPECIES_HUNTAIL) { Moves(MOVE_ICE_FANG, MOVE_CRUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); }
    } WHEN {
        TURN { MOVE(player, combo ? MOVE_ICE_FANG : MOVE_CRUNCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CRUNCH, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.4), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Abyssal Maw requires exactly two bites and a landed biting opener")
{
    u16 opener, second, third, reply;
    PARAMETRIZE { opener = MOVE_BITE; second = MOVE_SURF; third = MOVE_COIL; reply = MOVE_CELEBRATE; }
    PARAMETRIZE { opener = MOVE_BITE; second = MOVE_CRUNCH; third = MOVE_ICE_FANG; reply = MOVE_CELEBRATE; }
    PARAMETRIZE { opener = MOVE_SURF; second = MOVE_CRUNCH; third = MOVE_ICE_FANG; reply = MOVE_CELEBRATE; }
    PARAMETRIZE { opener = MOVE_BITE; second = MOVE_CRUNCH; third = MOVE_COIL; reply = MOVE_PROTECT; }
    PARAMETRIZE { opener = MOVE_PSYCHIC_FANGS; second = MOVE_CRUNCH; third = MOVE_COIL; reply = MOVE_CELEBRATE; }
    GIVEN {
        PLAYER(SPECIES_HUNTAIL) { Moves(opener, second, third); }
        OPPONENT(SPECIES_UMBREON) { HP(800); MaxHP(800); }
    } WHEN {
        TURN { MOVE(player, opener); MOVE(opponent, reply); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_ABYSSAL_MAW);
    }
}
