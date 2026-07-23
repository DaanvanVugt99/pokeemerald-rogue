#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_FLING].effect == EFFECT_FLING);
    ASSUME(gSpeciesInfo[SPECIES_DRATINI].types[0] == TYPE_DRAGON
        || gSpeciesInfo[SPECIES_DRATINI].types[1] == TYPE_DRAGON);
}

SINGLE_BATTLE_TEST("Snowball Fight makes a flung Snowball a 60 BP Ice move", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_LIMBER; }
    PARAMETRIZE { uniqueAbility = ABILITY_SNOWBALL_FIGHT; }

    GIVEN {
        PLAYER(SPECIES_BUNEARY) {
            Attack(100);
            Item(ITEM_SNOWBALL);
            Ability(ABILITY_RUN_AWAY);
            UniqueAbility(uniqueAbility);
            Moves(MOVE_FLING);
        }
        OPPONENT(SPECIES_DRATINI) { Defense(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_FLING); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(4.0), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Snowball Fight restores its Snowball and switches the user out")
{
    GIVEN {
        PLAYER(SPECIES_DARUMAKA_GALARIAN) {
            Item(ITEM_SNOWBALL);
            Ability(ABILITY_HUSTLE);
            UniqueAbility(ABILITY_SNOWBALL_FIGHT);
            Moves(MOVE_FLING);
        }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_FLING); SEND_OUT(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLING, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_SNOWBALL_FIGHT);
        MESSAGE("Go! Wynaut!");
    } THEN {
        EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_HELD_ITEM), ITEM_SNOWBALL);
    }
}

SINGLE_BATTLE_TEST("Snowball Fight does not restore its Snowball without switching")
{
    GIVEN {
        PLAYER(SPECIES_DARUMAKA_GALARIAN) {
            Item(ITEM_SNOWBALL);
            Ability(ABILITY_HUSTLE);
            UniqueAbility(ABILITY_SNOWBALL_FIGHT);
            Moves(MOVE_FLING);
        }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_FLING); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLING, player);
        HP_BAR(opponent);
        NOT ABILITY_POPUP(player, ABILITY_SNOWBALL_FIGHT);
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
    }
}
