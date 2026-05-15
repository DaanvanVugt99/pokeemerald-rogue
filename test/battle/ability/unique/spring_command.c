#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Spring Command sets Misty Terrain after a KO with other Flying and Fairy allies")
{
    bool32 hasFairyAlly;

    PARAMETRIZE { hasFairyAlly = TRUE; }
    PARAMETRIZE { hasFairyAlly = FALSE; }

    GIVEN {
        PLAYER(SPECIES_ENAMORUS) { Speed(100); Ability(ABILITY_CUTE_CHARM); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_PIDGEY) { Speed(50); Ability(ABILITY_KEEN_EYE); }
        PLAYER(hasFairyAlly ? SPECIES_CLEFAIRY : SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_CUTE_CHARM); }
        OPPONENT(SPECIES_CATERPIE) { HP(1); MaxHP(400); Speed(1); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_SPLASH); SEND_OUT(opponent, 1); }
    } SCENE {
        if (hasFairyAlly)
            ABILITY_POPUP(player, ABILITY_SPRING_COMMAND);
        else
            NOT ABILITY_POPUP(player, ABILITY_SPRING_COMMAND);
    } THEN {
        if (hasFairyAlly)
            EXPECT(gFieldStatuses & STATUS_FIELD_MISTY_TERRAIN);
        else
            EXPECT(!(gFieldStatuses & STATUS_FIELD_MISTY_TERRAIN));
    }
}
