#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Fallen Skies switches out the first time this Pokemon drops below half HP")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SUPER_FANG); }
        OPPONENT(SPECIES_ARCHEN) { HP(262); MaxHP(263); Ability(ABILITY_DEFEATIST); UniqueAbility(ABILITY_FALLEN_SKIES); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SUPER_FANG); SEND_OUT(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUPER_FANG, player);
        HP_BAR(opponent);
        ABILITY_POPUP(opponent, ABILITY_FALLEN_SKIES);
    }
}

SINGLE_BATTLE_TEST("Fallen Skies uses a known move when this Pokemon faints")
{
    GIVEN {
        PLAYER(SPECIES_ARCHEN) { HP(40); MaxHP(100); Speed(50); Ability(ABILITY_DEFEATIST); UniqueAbility(ABILITY_FALLEN_SKIES); Moves(MOVE_TACKLE, MOVE_WING_ATTACK); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_DRAGON_RAGE, WITH_RNG(RNG_ROGUE_FALLEN_SKIES, 1)); }
    } SCENE {
        MESSAGE("Foe Wobbuffet used Dragon Rage!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_RAGE, opponent);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_FALLEN_SKIES);
        MESSAGE("Archen used Wing Attack!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WING_ATTACK, player);
        MESSAGE("Archen fainted!");
        NOT MESSAGE("Foe Wobbuffet fainted!");
    } THEN {
        EXPECT_EQ(player->hp, 0);
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}
