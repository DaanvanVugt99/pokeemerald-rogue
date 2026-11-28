#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Toxic Deluge sets Acid Rain on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_MUK) { Ability(ABILITY_STICKY_HOLD); UniqueAbility(ABILITY_TOXIC_DELUGE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_ACID_RAIN);
    }
}

SINGLE_BATTLE_TEST("Toxic Deluge heals a quarter of Poison move damage only in Acid Rain", s16 damage, s16 healing)
{
    u16 move;
    bool32 rain;
    PARAMETRIZE { move = MOVE_SLUDGE_BOMB; rain = TRUE; }
    PARAMETRIZE { move = MOVE_TACKLE; rain = TRUE; }
    PARAMETRIZE { move = MOVE_SLUDGE_BOMB; rain = FALSE; }

    GIVEN {
        PLAYER(SPECIES_MUK) { HP(50); MaxHP(200); Speed(50); Moves(MOVE_SLUDGE_BOMB, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE, MOVE_SUNNY_DAY); }
    } WHEN {
        TURN { MOVE(opponent, rain ? MOVE_CELEBRATE : MOVE_SUNNY_DAY); MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
        if (rain && move == MOVE_SLUDGE_BOMB) {
            ABILITY_POPUP(player, ABILITY_TOXIC_DELUGE);
            HP_BAR(player, captureDamage: &results[i].healing);
        } else {
            NOT ABILITY_POPUP(player, ABILITY_TOXIC_DELUGE);
        }
    } THEN {
        if (rain && move == MOVE_SLUDGE_BOMB)
            EXPECT_EQ(-results[i].healing, max(1, results[i].damage / 4));
    }
}

SINGLE_BATTLE_TEST("Toxic Deluge combines both hits into one heal")
{
    s16 first, second, healing;
    GIVEN {
        PLAYER(SPECIES_MUK) { Ability(ABILITY_PARENTAL_BOND); UniqueAbility(ABILITY_TOXIC_DELUGE); HP(50); MaxHP(200); Moves(MOVE_SLUDGE_BOMB); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SLUDGE_BOMB); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SLUDGE_BOMB, player);
        HP_BAR(opponent, captureDamage: &first);
        HP_BAR(opponent, captureDamage: &second);
        ABILITY_POPUP(player, ABILITY_TOXIC_DELUGE);
        HP_BAR(player, captureDamage: &healing);
    } THEN {
        EXPECT_EQ(-healing, max(1, (first + second) / 4));
    }
}
