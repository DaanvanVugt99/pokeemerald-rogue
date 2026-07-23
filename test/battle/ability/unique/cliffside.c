#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Cliffside uses Rock Polish the first time Klawf drops below half HP each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_KLAWF)
        {
            Ability(ABILITY_SHELL_ARMOR);
            HP(GetMonData(&PLAYER_PARTY[0], MON_DATA_MAX_HP) / 2 + 20);
            Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_RAGE, opponent);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_CLIFFSIDE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROCK_POLISH, player);
        NOT ABILITY_POPUP(player, ABILITY_CLIFFSIDE);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Cliffside does not trigger from confusion damage or a later hit while below half HP")
{
    s16 confusionDamage;

    PASSES_RANDOMLY(1, 3, RNG_CONFUSION);

    GIVEN {
        PLAYER(SPECIES_KLAWF) {
            Ability(ABILITY_SHELL_ARMOR);
            UniqueAbility(ABILITY_CLIFFSIDE);
            HP(60);
            MaxHP(100);
            Attack(150);
            Defense(255);
            Speed(100);
            Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(1); Speed(1); Moves(MOVE_CONFUSE_RAY, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CONFUSE_RAY); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("It hurt itself in its confusion!");
        HP_BAR(player, captureDamage: &confusionDamage);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        HP_BAR(player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_CLIFFSIDE);
        }
    } THEN {
        EXPECT_GT(confusionDamage, 10);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}
