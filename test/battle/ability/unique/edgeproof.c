#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_LEAF_BLADE].slicingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].slicingMove);
}

SINGLE_BATTLE_TEST("Edgeproof blocks slicing moves")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_LEAF_BLADE); }
        OPPONENT(SPECIES_ZYGARDE) { Ability(ABILITY_AURA_BREAK); UniqueAbility(ABILITY_EDGEPROOF); }
    } WHEN {
        TURN { MOVE(player, MOVE_LEAF_BLADE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_EDGEPROOF);
        MESSAGE("Foe Zygarde's Edgeproof blocks Leaf Blade!");
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_LEAF_BLADE, player);
            HP_BAR(opponent);
        }
    }
}

SINGLE_BATTLE_TEST("Edgeproof does not block non-slicing moves")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_ZYGARDE) { Ability(ABILITY_AURA_BREAK); UniqueAbility(ABILITY_EDGEPROOF); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        NOT ABILITY_POPUP(opponent, ABILITY_EDGEPROOF);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Edgeproof is active on all Zygarde forms")
{
    u16 species;

    PARAMETRIZE { species = SPECIES_ZYGARDE_10_AURA_BREAK; }
    PARAMETRIZE { species = SPECIES_ZYGARDE_10_POWER_CONSTRUCT; }
    PARAMETRIZE { species = SPECIES_ZYGARDE_50_AURA_BREAK; }
    PARAMETRIZE { species = SPECIES_ZYGARDE_50_POWER_CONSTRUCT; }
    PARAMETRIZE { species = SPECIES_ZYGARDE_COMPLETE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_LEAF_BLADE); }
        OPPONENT(species) { Ability(ABILITY_AURA_BREAK); }
    } WHEN {
        TURN { MOVE(player, MOVE_LEAF_BLADE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_EDGEPROOF);
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_LEAF_BLADE, player);
            HP_BAR(opponent);
        }
    }
}
