#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SLEEP_TALK].effect == EFFECT_SLEEP_TALK);
    ASSUME(gBattleMoves[MOVE_SNORE].effect == EFFECT_SNORE);
}

SINGLE_BATTLE_TEST("Dozing triggers every other turn")
{
    GIVEN {
        PLAYER(SPECIES_KOMALA) { Speed(100); Ability(ABILITY_COMATOSE); UniqueAbility(ABILITY_DOZING); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE, WITH_RNG(RNG_ROGUE_DOZING, 0)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);

        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ABILITY_POPUP(player, ABILITY_DOZING);

        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("Dozing can use Snore on its active turn")
{
    GIVEN {
        PLAYER(SPECIES_KOMALA) { Speed(100); Ability(ABILITY_COMATOSE); UniqueAbility(ABILITY_DOZING); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE, WITH_RNG(RNG_ROGUE_DOZING, 1)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DOZING);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SNORE, player);
    }
}
