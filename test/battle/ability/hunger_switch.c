#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Hunger Switch switches Morpeko's forms at the end of the turn")
{
    u16 species;
    PARAMETRIZE { species = SPECIES_MORPEKO_FULL_BELLY; }
    PARAMETRIZE { species = SPECIES_MORPEKO_HANGRY; }
    GIVEN {
        PLAYER(species) { Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Morpeko used Celebrate!");
        MESSAGE("Foe Wobbuffet used Celebrate!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_FORM_CHANGE, player);
    } THEN {
        if (species == SPECIES_MORPEKO_FULL_BELLY)
            EXPECT_EQ(player->species, SPECIES_MORPEKO_HANGRY);
        else
            EXPECT_EQ(player->species, SPECIES_MORPEKO_FULL_BELLY);
    }
}

SINGLE_BATTLE_TEST("Hunger Switch does not overwrite the next ability popup")
{
    GIVEN {
        PLAYER(SPECIES_MORPEKO_FULL_BELLY) { Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_GYARADOS) { Ability(ABILITY_INTIMIDATE); Speed(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); SWITCH(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_FORM_CHANGE, player);
        ABILITY_POPUP(opponent, ABILITY_INTIMIDATE);
    }
}
