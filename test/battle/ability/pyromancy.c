#include <stdbool.h>
#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Pyromancy guarantees burn when base chance ×5 ≥ 100; Blaze burns ~30%")
{
    u16 ability;

    PARAMETRIZE { ability = ABILITY_PYROMANCY; }
    PARAMETRIZE
    {
        ability = ABILITY_BLAZE;
        PASSES_RANDOMLY(15, 50); // ~30%
    }

    GIVEN
    {
        ASSUME(gBattleMoves[MOVE_LAVA_PLUME].effect == EFFECT_BURN_HIT);
        ASSUME(gBattleMoves[MOVE_LAVA_PLUME].secondaryEffectChance == 30);

        PLAYER(SPECIES_CHARMANDER)
        {
            Ability(ability);
            Moves(MOVE_LAVA_PLUME);
        }

        OPPONENT(SPECIES_WOBBUFFET)
        {
            MaxHP(999);
            HP(999);
        }
    }

    WHEN
    {
        TURN { MOVE(player, MOVE_LAVA_PLUME); }
    }

    SCENE
    {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LAVA_PLUME, player);
        HP_BAR(opponent);

        if (ability == ABILITY_PYROMANCY)
            STATUS_ICON(opponent, burn : TRUE);
    }
}
