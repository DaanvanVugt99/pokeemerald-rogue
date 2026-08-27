#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(ItemId_GetHoldEffect(ITEM_ECLIPSE_TOTEM) == HOLD_EFFECT_ECLIPSE_TOTEM);
    ASSUME(ItemId_GetHoldEffect(ITEM_SUN_TOTEM) == HOLD_EFFECT_SUN_TOTEM);
    ASSUME(ItemId_GetHoldEffect(ITEM_RAIN_TOTEM) == HOLD_EFFECT_RAIN_TOTEM);
    ASSUME(ItemId_GetHoldEffect(ITEM_SAND_TOTEM) == HOLD_EFFECT_SAND_TOTEM);
    ASSUME(ItemId_GetHoldEffect(ITEM_SNOW_TOTEM) == HOLD_EFFECT_SNOW_TOTEM);
    ASSUME(ItemId_GetHoldEffect(ITEM_ACID_RAIN_TOTEM) == HOLD_EFFECT_ACID_RAIN_TOTEM);
    ASSUME(ItemId_GetHoldEffect(ITEM_RAINCOAT) == HOLD_EFFECT_RAINCOAT);
    ASSUME(gBattleMoves[MOVE_ECLIPSE].effect == EFFECT_ECLIPSE);
    ASSUME(gBattleMoves[MOVE_ACID_RAIN].effect == EFFECT_CORROSIVE_CLOUDS);
    ASSUME(gBattleMoves[MOVE_SNOWSCAPE].effect == EFFECT_SNOWSCAPE);
    ASSUME(gBattleMoves[MOVE_SOAK].effect == EFFECT_SOAK);
    ASSUME(gBattleMoves[MOVE_DOUBLE_KICK].strikeCount == 2);
    ASSUME(gBattleMoves[MOVE_HYPER_VOICE].target == MOVE_TARGET_BOTH);
    ASSUME(gBattleMoves[MOVE_BITE].type == TYPE_DARK);
    ASSUME(gBattleMoves[MOVE_SWIFT].split == SPLIT_SPECIAL);
}

SINGLE_BATTLE_TEST("Weather Totem: Eclipse grants Dark STAB during Eclipse", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_ECLIPSE_TOTEM; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(120); Item(item); Moves(MOVE_ECLIPSE, MOVE_BITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_ECLIPSE); }
        TURN { MOVE(player, MOVE_BITE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Weather Totem: Eclipse raises Speed by 50 percent during Eclipse")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Item(ITEM_ECLIPSE_TOTEM); Moves(MOVE_CELEBRATE, MOVE_NIGHT_SLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(140); HP(1000); MaxHP(1000); Moves(MOVE_ECLIPSE, MOVE_POUND); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_ECLIPSE); }
        TURN { MOVE(player, MOVE_NIGHT_SLASH); MOVE(opponent, MOVE_POUND); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_NIGHT_SLASH, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_POUND, opponent);
    }
}

SINGLE_BATTLE_TEST("Weather Totem: Sun heals one third of damage dealt during sunlight")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1); MaxHP(100); Item(ITEM_SUN_TOTEM); Moves(MOVE_CELEBRATE, MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_SUNNY_DAY, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SUNNY_DAY); }
        TURN { MOVE(player, MOVE_DRAGON_RAGE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, damage: 40);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        MESSAGE("Wobbuffet's Sun Totem restored its HP a little!");
        HP_BAR(player, damage: -13);
    }
}

SINGLE_BATTLE_TEST("Weather Totem: Sun heals from every hit of a multi-hit move")
{
    s16 firstHit;
    s16 secondHit;
    s16 healing;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1); MaxHP(1000); Attack(500); Item(ITEM_SUN_TOTEM); Moves(MOVE_SUNNY_DAY, MOVE_DOUBLE_KICK); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Defense(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUNNY_DAY); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_DOUBLE_KICK, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &firstHit);
        HP_BAR(opponent, captureDamage: &secondHit);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        HP_BAR(player, captureDamage: &healing);
    } THEN {
        EXPECT_EQ(healing, -max(1, (firstHit + secondHit) / 3));
    }
}

DOUBLE_BATTLE_TEST("Weather Totem: Sun heals from damage dealt to every spread target")
{
    s16 leftDamage;
    s16 rightDamage;
    s16 healing;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1); MaxHP(1000); SpAttack(500); Item(ITEM_SUN_TOTEM); Moves(MOVE_SUNNY_DAY, MOVE_HYPER_VOICE); }
        PLAYER(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); SpDefense(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { HP(1000); MaxHP(1000); SpDefense(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_SUNNY_DAY); }
        TURN { MOVE(playerLeft, MOVE_HYPER_VOICE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(opponentLeft, captureDamage: &leftDamage);
        HP_BAR(opponentRight, captureDamage: &rightDamage);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, playerLeft);
        HP_BAR(playerLeft, captureDamage: &healing);
    } THEN {
        EXPECT_EQ(healing, -max(1, (leftDamage + rightDamage) / 3));
    }
}

SINGLE_BATTLE_TEST("Weather Totem: Rain adds Water typing during rain")
{
    u32 battler;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_RAIN_TOTEM); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_RAIN_DANCE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_RAIN_DANCE); }
    } THEN {
        battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        EXPECT_EQ(GetBattlerType(battler, 0, FALSE), TYPE_PSYCHIC);
        EXPECT_EQ(GetBattlerType(battler, 1, FALSE), TYPE_WATER);
        EXPECT_EQ(GetBattlerType(battler, 2, FALSE), TYPE_MYSTERY);
    }
}

SINGLE_BATTLE_TEST("Weather Totem: Rain restores one sixteenth HP at end of turn during rain")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1); MaxHP(160); Item(ITEM_RAIN_TOTEM); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_RAIN_DANCE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_RAIN_DANCE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        MESSAGE("Wobbuffet's Rain Totem restored its HP a little!");
        HP_BAR(player, damage: -10);
    }
}

SINGLE_BATTLE_TEST("Raincoat does not change Water damage outside rain", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_RAINCOAT; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Item(item); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { SpAttack(120); Moves(MOVE_CELEBRATE, MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[0].damage, 0);
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Raincoat blocks Water moves during rain")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Item(ITEM_RAINCOAT); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { SpAttack(120); Moves(MOVE_RAIN_DANCE, MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_RAIN_DANCE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        NONE_OF {
            HP_BAR(player);
        }
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Raincoat blocks Soak during rain")
{
    u32 battler;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_RAINCOAT); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_RAIN_DANCE, MOVE_SOAK); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_RAIN_DANCE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SOAK); }
    } SCENE {
        MESSAGE("But it failed!");
    } THEN {
        battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        EXPECT_EQ(GetBattlerType(battler, 0, FALSE), TYPE_PSYCHIC);
        EXPECT_EQ(GetBattlerType(battler, 1, FALSE), TYPE_PSYCHIC);
    }
}

SINGLE_BATTLE_TEST("Weather Totem: Sand raises both defenses by 50 percent during Sandstorm", s16 damage)
{
    u16 item;
    u16 setupMove;
    u16 attackMove;

    PARAMETRIZE { item = ITEM_NONE;       setupMove = MOVE_CELEBRATE; attackMove = MOVE_TACKLE; }
    PARAMETRIZE { item = ITEM_NONE;       setupMove = MOVE_SANDSTORM; attackMove = MOVE_TACKLE; }
    PARAMETRIZE { item = ITEM_SAND_TOTEM; setupMove = MOVE_SANDSTORM; attackMove = MOVE_TACKLE; }
    PARAMETRIZE { item = ITEM_NONE;       setupMove = MOVE_CELEBRATE; attackMove = MOVE_SWIFT; }
    PARAMETRIZE { item = ITEM_NONE;       setupMove = MOVE_SANDSTORM; attackMove = MOVE_SWIFT; }
    PARAMETRIZE { item = ITEM_SAND_TOTEM; setupMove = MOVE_SANDSTORM; attackMove = MOVE_SWIFT; }

    GIVEN {
        PLAYER(SPECIES_REGISTEEL) { Defense(120); SpDefense(120); HP(1000); MaxHP(1000); Item(item); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(120); SpAttack(120); Moves(setupMove, attackMove); }
    } WHEN {
        TURN { MOVE(opponent, setupMove); }
        TURN { MOVE(opponent, attackMove); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, UQ_4_12(1.5), results[1].damage);
        EXPECT_EQ(results[3].damage, results[4].damage);
        EXPECT_MUL_EQ(results[5].damage, UQ_4_12(1.5), results[4].damage);
    }
}

SINGLE_BATTLE_TEST("Weather Totem: Sand prevents Sandstorm damage")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_SAND_TOTEM); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SANDSTORM); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SANDSTORM); }
    } SCENE {
        NONE_OF {
            MESSAGE("Wobbuffet is buffeted by the sandstorm!");
            HP_BAR(player);
        }
    }
}

SINGLE_BATTLE_TEST("Weather Totem: Snow reduces supereffective damage by 35 percent during snow", s16 damage)
{
    u16 item;
    u16 setupMove;

    PARAMETRIZE { item = ITEM_NONE;       setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { item = ITEM_NONE;       setupMove = MOVE_SNOWSCAPE; }
    PARAMETRIZE { item = ITEM_SNOW_TOTEM; setupMove = MOVE_SNOWSCAPE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); Item(item); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(120); Moves(setupMove, MOVE_BITE); }
    } WHEN {
        TURN { MOVE(opponent, setupMove); }
        TURN { MOVE(opponent, MOVE_BITE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_MUL_EQ(results[1].damage, UQ_4_12(0.65), results[2].damage);
    }
}

SINGLE_BATTLE_TEST("Weather Totem: Acid Rain prevents Acid Rain damage")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_ACID_RAIN_TOTEM); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_ACID_RAIN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_ACID_RAIN); }
    } SCENE {
        NONE_OF {
            MESSAGE("Wobbuffet is scorched by acid rain!");
            HP_BAR(player);
        }
    }
}

SINGLE_BATTLE_TEST("Weather Totem: Acid Rain makes damaging moves poison during Acid Rain")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_ACID_RAIN_TOTEM); Moves(MOVE_ACID_RAIN, MOVE_SWIFT); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ACID_RAIN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SWIFT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWIFT, player);
        HP_BAR(opponent);
        MESSAGE("Foe Wobbuffet was poisoned!");
    } THEN {
        EXPECT_EQ(opponent->status1 & STATUS1_PSN_ANY, STATUS1_POISON);
    }
}

SINGLE_BATTLE_TEST("Weather Totem: Acid Rain does not poison outside Acid Rain")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_ACID_RAIN_TOTEM); Moves(MOVE_SWIFT); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWIFT); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->status1, 0);
    }
}
