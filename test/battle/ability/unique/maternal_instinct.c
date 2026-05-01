#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SAFEGUARD].effect == EFFECT_SAFEGUARD);
    ASSUME(gBattleMoves[MOVE_DRAGON_RAGE].effect == EFFECT_DRAGON_RAGE);
}

SINGLE_BATTLE_TEST("Maternal Instinct sets Safeguard once when Kangaskhan falls below half HP")
{
    GIVEN {
        PLAYER(SPECIES_KANGASKHAN) { Ability(ABILITY_EARLY_BIRD); UniqueAbility(ABILITY_MATERNAL_INSTINCT); HP(81); MaxHP(160); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_RAGE, opponent);
        ABILITY_POPUP(player, ABILITY_MATERNAL_INSTINCT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SAFEGUARD, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_RAGE, opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_MATERNAL_INSTINCT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SAFEGUARD, player);
        }
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_SAFEGUARD);
    }
}

SINGLE_BATTLE_TEST("Maternal Instinct does not trigger if Kangaskhan faints")
{
    GIVEN {
        PLAYER(SPECIES_KANGASKHAN) { Ability(ABILITY_EARLY_BIRD); UniqueAbility(ABILITY_MATERNAL_INSTINCT); HP(40); MaxHP(160); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_RAGE, opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_MATERNAL_INSTINCT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SAFEGUARD, player);
        }
    }
}
