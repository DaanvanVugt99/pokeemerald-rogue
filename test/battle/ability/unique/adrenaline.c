#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_MEGA_PUNCH].punchingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].punchingMove);
}

SINGLE_BATTLE_TEST("Adrenaline heals for one-fourth of damage when poisoned and using a punching move")
{
    s16 damage;
    s16 healed;

    GIVEN {
        PLAYER(SPECIES_BRELOOM) { HP(100); MaxHP(200); Status1(STATUS1_POISON); Ability(ABILITY_EFFECT_SPORE); UniqueAbility(ABILITY_ADRENALINE); Moves(MOVE_MEGA_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MEGA_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MEGA_PUNCH, player);
        HP_BAR(opponent, captureDamage: &damage);
        ABILITY_POPUP(player, ABILITY_ADRENALINE);
        HP_BAR(player, captureDamage: &healed);
        MESSAGE("Foe Wobbuffet had its energy drained!");
    } THEN {
        EXPECT_MUL_EQ(damage, Q_4_12(-1.0 / 4.0), healed);
    }
}

SINGLE_BATTLE_TEST("Adrenaline does not heal when not poisoned")
{
    GIVEN {
        PLAYER(SPECIES_BRELOOM) { HP(100); MaxHP(200); Ability(ABILITY_EFFECT_SPORE); UniqueAbility(ABILITY_ADRENALINE); Moves(MOVE_MEGA_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MEGA_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_ADRENALINE);
            HP_BAR(player);
            MESSAGE("Foe Wobbuffet had its energy drained!");
        }
    }
}

SINGLE_BATTLE_TEST("Adrenaline blocks poison damage without curing poison")
{
    u32 status;

    PARAMETRIZE { status = STATUS1_POISON; }
    PARAMETRIZE { status = STATUS1_TOXIC_POISON; }

    GIVEN {
        PLAYER(SPECIES_BRELOOM) { HP(100); MaxHP(200); Status1(status); Ability(ABILITY_EFFECT_SPORE); UniqueAbility(ABILITY_ADRENALINE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_ADRENALINE);
            MESSAGE("Foe Wobbuffet had its energy drained!");
        }
    } THEN {
        EXPECT_EQ(player->hp, 100);
        EXPECT(player->status1 & status);
    }
}

SINGLE_BATTLE_TEST("Poison Heal still heals a poisoned Pokemon with Adrenaline")
{
    GIVEN {
        PLAYER(SPECIES_BRELOOM) { HP(100); MaxHP(200); Status1(STATUS1_POISON); Ability(ABILITY_POISON_HEAL); UniqueAbility(ABILITY_ADRENALINE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_POISON_HEAL);
        MESSAGE("The poisoning healed Breloom a little bit!");
        HP_BAR(player, damage: -25);
    } THEN {
        EXPECT_EQ(player->hp, 125);
        EXPECT(player->status1 & STATUS1_POISON);
    }
}
