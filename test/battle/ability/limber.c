#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Limber prevents paralysis")
{
    GIVEN {
        PLAYER(SPECIES_PERSIAN) { Ability(ABILITY_LIMBER); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_THUNDER_SHOCK); }
    } SCENE {
        HP_BAR(player);
        NONE_OF {
            ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PRZ, player);
            STATUS_ICON(player, paralysis: TRUE);
        }
    }
}

SINGLE_BATTLE_TEST("Unique Limber prevents paralysis")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { UniqueAbility(ABILITY_LIMBER); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_THUNDER_WAVE); }
    } SCENE {
        NONE_OF {
            ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PRZ, player);
            STATUS_ICON(player, paralysis: TRUE);
        }
    }
}

SINGLE_BATTLE_TEST("Limber ignores Speed stage changes when determining move order")
{
    u32 ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_LIMBER; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Speed(100); }
        OPPONENT(SPECIES_WYNAUT) { Ability(ABILITY_PRANKSTER); Speed(60); Moves(MOVE_SCARY_FACE, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCARY_FACE); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("Foe Wynaut used Scary Face!");
        MESSAGE("Wobbuffet used Celebrate!");
        if (ability == ABILITY_NONE) {
            MESSAGE("Foe Wynaut used Tackle!");
            MESSAGE("Wobbuffet used Celebrate!");
        } else {
            MESSAGE("Wobbuffet used Celebrate!");
            MESSAGE("Foe Wynaut used Tackle!");
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 2);
    }
}
