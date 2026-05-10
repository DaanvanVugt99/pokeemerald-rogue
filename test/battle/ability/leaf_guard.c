#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Leaf Guard prevents non-volatile status conditions in sun")
{
    u32 move;
    u16 status;
    PARAMETRIZE { move = MOVE_WILL_O_WISP; status = STATUS1_BURN; }
    PARAMETRIZE { move = MOVE_HYPNOSIS; status = STATUS1_SLEEP; }
    PARAMETRIZE { move = MOVE_THUNDER_WAVE; status = STATUS1_PARALYSIS; }
    PARAMETRIZE { move = MOVE_TOXIC; status = STATUS1_TOXIC_POISON; }
    PARAMETRIZE { move = MOVE_POWDER_SNOW; status = STATUS1_FREEZE; }
    GIVEN {
        ASSUME(gBattleMoves[MOVE_WILL_O_WISP].effect == EFFECT_WILL_O_WISP);
        ASSUME(gBattleMoves[MOVE_HYPNOSIS].effect == EFFECT_SLEEP);
        ASSUME(gBattleMoves[MOVE_THUNDER_WAVE].effect == EFFECT_PARALYZE);
        ASSUME(gBattleMoves[MOVE_TOXIC].effect == EFFECT_TOXIC);
        ASSUME(gBattleMoves[MOVE_POWDER_SNOW].effect == EFFECT_FREEZE_HIT);
        PLAYER(SPECIES_LEAFEON) { Ability(ABILITY_LEAF_GUARD); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SUNNY_DAY); MOVE(opponent, move); }
    } SCENE {
        if (move != MOVE_POWDER_SNOW) {
            NOT ANIMATION(ANIM_TYPE_MOVE, move, opponent);
            ABILITY_POPUP(player, ABILITY_LEAF_GUARD);
            MESSAGE("It doesn't affect Leafeon…");
            NOT STATUS_ICON(player, status);
        } else {
            NONE_OF {
                ABILITY_POPUP(player, ABILITY_LEAF_GUARD);
                STATUS_ICON(player, status);
            }
        }
    }
}

SINGLE_BATTLE_TEST("Unique Leaf Guard prevents non-volatile status conditions in sun")
{
    u32 move;
    u16 status;
    PARAMETRIZE { move = MOVE_WILL_O_WISP; status = STATUS1_BURN; }
    PARAMETRIZE { move = MOVE_HYPNOSIS; status = STATUS1_SLEEP; }
    PARAMETRIZE { move = MOVE_THUNDER_WAVE; status = STATUS1_PARALYSIS; }
    PARAMETRIZE { move = MOVE_TOXIC; status = STATUS1_TOXIC_POISON; }
    PARAMETRIZE { move = MOVE_POWDER_SNOW; status = STATUS1_FREEZE; }
    GIVEN {
        ASSUME(gBattleMoves[MOVE_WILL_O_WISP].effect == EFFECT_WILL_O_WISP);
        ASSUME(gBattleMoves[MOVE_HYPNOSIS].effect == EFFECT_SLEEP);
        ASSUME(gBattleMoves[MOVE_THUNDER_WAVE].effect == EFFECT_PARALYZE);
        ASSUME(gBattleMoves[MOVE_TOXIC].effect == EFFECT_TOXIC);
        ASSUME(gBattleMoves[MOVE_POWDER_SNOW].effect == EFFECT_FREEZE_HIT);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); UniqueAbility(ABILITY_LEAF_GUARD); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUNNY_DAY); MOVE(opponent, move); }
    } SCENE {
        if (move != MOVE_POWDER_SNOW) {
            NOT ANIMATION(ANIM_TYPE_MOVE, move, opponent);
            ABILITY_POPUP(player, ABILITY_LEAF_GUARD);
            MESSAGE("It doesn't affect Wobbuffet…");
            NOT STATUS_ICON(player, status);
        } else {
            NONE_OF {
                ABILITY_POPUP(player, ABILITY_LEAF_GUARD);
                STATUS_ICON(player, status);
            }
        }
    }
}

SINGLE_BATTLE_TEST("Leaf Guard prevents status conditions from Flame Orb and Toxic Orb")
{
    u32 item;
    PARAMETRIZE { item = ITEM_FLAME_ORB; }
    PARAMETRIZE { item = ITEM_TOXIC_ORB; }
    GIVEN {
        ASSUME(gItems[ITEM_FLAME_ORB].holdEffect == HOLD_EFFECT_FLAME_ORB);
        ASSUME(gItems[ITEM_TOXIC_ORB].holdEffect == HOLD_EFFECT_TOXIC_ORB);
        PLAYER(SPECIES_LEAFEON) { Ability(ABILITY_LEAF_GUARD); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SUNNY_DAY); }
    } SCENE {
        if (item == ITEM_FLAME_ORB) {
            NONE_OF { MESSAGE("Leafeon was burned!"); STATUS_ICON(player, burn: TRUE); }
        }
        else {
            NONE_OF { MESSAGE("Leafeon is badly poisoned!"); STATUS_ICON(player, poison: TRUE); }
        }
    }
}

SINGLE_BATTLE_TEST("Leaf Guard prevents Rest during sun")
{
    GIVEN {
        ASSUME(B_LEAF_GUARD_PREVENTS_REST >= GEN_5);
        ASSUME(gBattleMoves[MOVE_REST].effect == EFFECT_REST);
        PLAYER(SPECIES_LEAFEON) { Ability(ABILITY_LEAF_GUARD); HP(100); MaxHP(200); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUNNY_DAY); MOVE(player, MOVE_REST); }
    } SCENE {
        MESSAGE("But it failed!");
        NONE_OF {
            STATUS_ICON(player, sleep: TRUE);
            HP_BAR(player);
        }
    }
}

SINGLE_BATTLE_TEST("Leaf Guard boosts Defense and Sp. Defense during sun", s16 damage)
{
    bool32 sun;
    u32 ability;
    u32 move;

    PARAMETRIZE { sun = FALSE; ability = ABILITY_LEAF_GUARD; move = MOVE_TACKLE; }
    PARAMETRIZE { sun = TRUE;  ability = ABILITY_NONE;       move = MOVE_TACKLE; }
    PARAMETRIZE { sun = TRUE;  ability = ABILITY_LEAF_GUARD; move = MOVE_TACKLE; }
    PARAMETRIZE { sun = FALSE; ability = ABILITY_LEAF_GUARD; move = MOVE_POWER_GEM; }
    PARAMETRIZE { sun = TRUE;  ability = ABILITY_NONE;       move = MOVE_POWER_GEM; }
    PARAMETRIZE { sun = TRUE;  ability = ABILITY_LEAF_GUARD; move = MOVE_POWER_GEM; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_PHYSICAL);
        ASSUME(gBattleMoves[MOVE_POWER_GEM].split == SPLIT_SPECIAL);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Ability(ability); Moves(MOVE_SUNNY_DAY, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_TACKLE, MOVE_POWER_GEM); }
    } WHEN {
        if (sun)
            TURN { MOVE(player, MOVE_SUNNY_DAY); MOVE(opponent, move); }
        else
            TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, opponent);
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(0.8), results[2].damage);
        EXPECT_EQ(results[3].damage, results[4].damage);
        EXPECT_MUL_EQ(results[4].damage, Q_4_12(0.8), results[5].damage);
    }
}
