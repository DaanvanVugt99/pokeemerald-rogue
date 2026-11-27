#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Dream Sequence makes Hypnosis 80 percent accurate without a follow-up")
{
    PASSES_RANDOMLY(80, 100, RNG_ACCURACY);

    GIVEN {
        PLAYER(SPECIES_HYPNO) { Speed(50); Ability(ABILITY_INSOMNIA); UniqueAbility(ABILITY_DREAM_SEQUENCE); Moves(MOVE_HYPNOSIS); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_HYPNOSIS); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPNOSIS, player);
        STATUS_ICON(opponent, STATUS1_SLEEP);
        NOT HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Dream Sequence makes Dream Eater fully drain and wake the target")
{
    s16 damage;
    s16 healing;

    GIVEN {
        PLAYER(SPECIES_HYPNO) { Speed(100); HP(100); MaxHP(500); SpAttack(80); Ability(ABILITY_INSOMNIA); UniqueAbility(ABILITY_DREAM_SEQUENCE); Moves(MOVE_DREAM_EATER); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); HP(500); MaxHP(500); SpDefense(100); Status1(STATUS1_SLEEP_TURN(3)); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_DREAM_EATER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DREAM_EATER, player);
        HP_BAR(opponent, captureDamage: &damage);
        HP_BAR(player, captureDamage: &healing);
        ABILITY_POPUP(player, ABILITY_DREAM_SEQUENCE);
        MESSAGE("Foe Wobbuffet woke up!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    } THEN {
        EXPECT_GT(damage, 0);
        EXPECT_EQ(healing, -damage);
        EXPECT_EQ(opponent->status1 & STATUS1_SLEEP, 0);
    }
}

SINGLE_BATTLE_TEST("Dream Sequence does not let Dream Eater hit an awake foe")
{
    GIVEN {
        PLAYER(SPECIES_HYPNO) { Ability(ABILITY_INSOMNIA); UniqueAbility(ABILITY_DREAM_SEQUENCE); Moves(MOVE_DREAM_EATER); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DREAM_EATER, hit: FALSE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_DREAM_SEQUENCE);
            HP_BAR(opponent);
        }
    }
}
