#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_GROWL));
    ASSUME(!IS_MOVE_STATUS(MOVE_TACKLE));
    ASSUME(gSpeciesInfo[SPECIES_KLEFKI].types[0] == TYPE_STEEL || gSpeciesInfo[SPECIES_KLEFKI].types[1] == TYPE_STEEL);
    ASSUME(gSpeciesInfo[SPECIES_KLEFKI].types[0] == TYPE_FAIRY || gSpeciesInfo[SPECIES_KLEFKI].types[1] == TYPE_FAIRY);
    ASSUME(gSpeciesInfo[SPECIES_MAWILE].types[0] == TYPE_STEEL || gSpeciesInfo[SPECIES_MAWILE].types[1] == TYPE_STEEL);
    ASSUME(gSpeciesInfo[SPECIES_MAWILE].types[0] == TYPE_FAIRY || gSpeciesInfo[SPECIES_MAWILE].types[1] == TYPE_FAIRY);
}

SINGLE_BATTLE_TEST("Keyring disables the target's last used move for 2 turns after the first status move")
{
    GIVEN {
        PLAYER(SPECIES_KLEFKI) { Speed(1); Ability(ABILITY_MAGIC_GUARD); UniqueAbility(ABILITY_KEYRING); Moves(MOVE_GROWL); }
        PLAYER(SPECIES_PIKACHU) { Speed(25); }
        PLAYER(SPECIES_CHARMANDER) { Speed(25); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); MOVE(player, MOVE_GROWL); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        ABILITY_POPUP(player, ABILITY_KEYRING);
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disabledMove, MOVE_TACKLE);
        EXPECT_EQ((u32)gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disableTimer, 2);
    }
}

SINGLE_BATTLE_TEST("Keyring does not trigger if party type combinations are duplicated")
{
    GIVEN {
        PLAYER(SPECIES_KLEFKI) { Speed(1); Ability(ABILITY_MAGIC_GUARD); UniqueAbility(ABILITY_KEYRING); Moves(MOVE_GROWL); }
        PLAYER(SPECIES_MAWILE) { Speed(25); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); MOVE(player, MOVE_GROWL); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_KEYRING);
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disabledMove, MOVE_NONE);
        EXPECT_EQ((u32)gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disableTimer, 0);
    }
}

SINGLE_BATTLE_TEST("Keyring only triggers once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_KLEFKI) { Speed(1); Ability(ABILITY_MAGIC_GUARD); UniqueAbility(ABILITY_KEYRING); Moves(MOVE_GROWL); }
        PLAYER(SPECIES_PIKACHU) { Speed(25); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); MOVE(player, MOVE_GROWL); }
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_GROWL); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_KEYRING);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_KEYRING);
        }
    }
}

SINGLE_BATTLE_TEST("Keyring refreshes after switching out and back in")
{
    GIVEN {
        PLAYER(SPECIES_KLEFKI) { Speed(1); Ability(ABILITY_MAGIC_GUARD); UniqueAbility(ABILITY_KEYRING); Moves(MOVE_GROWL); }
        PLAYER(SPECIES_PIKACHU) { Speed(25); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); MOVE(player, MOVE_GROWL); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_TACKLE); MOVE(player, MOVE_GROWL); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_KEYRING);
        ABILITY_POPUP(player, ABILITY_KEYRING);
    }
}
