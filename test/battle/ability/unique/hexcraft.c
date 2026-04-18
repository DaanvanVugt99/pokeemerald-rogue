#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_GROWL));
    ASSUME(IS_MOVE_STATUS(MOVE_AGILITY));
    ASSUME(gBattleMoves[MOVE_HEX].power > 30);
}

SINGLE_BATTLE_TEST("Hexcraft uses 30 BP Hex after status moves targeting foes")
{
    s16 damage;

    GIVEN {
        PLAYER(SPECIES_MISDREAVUS) { Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_HEXCRAFT); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        ABILITY_POPUP(player, ABILITY_HEXCRAFT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HEX, player);
        HP_BAR(opponent, captureDamage: &damage);
    } THEN {
        EXPECT_GT(damage, 0);
    }
}

SINGLE_BATTLE_TEST("Hexcraft does not trigger on self-targeting status moves")
{
    GIVEN {
        PLAYER(SPECIES_MISDREAVUS) { Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_HEXCRAFT); Moves(MOVE_AGILITY); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_AGILITY); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AGILITY, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_HEXCRAFT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HEX, player);
            HP_BAR(opponent);
        }
    }
}
