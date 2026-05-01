#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_HYPER_VOICE].soundMove);
    ASSUME(!IS_MOVE_STATUS(MOVE_HYPER_VOICE));
    ASSUME(!gBattleMoves[MOVE_TACKLE].soundMove);
    ASSUME(gBattleMoves[MOVE_MISTY_TERRAIN].effect == EFFECT_MISTY_TERRAIN);
}

SINGLE_BATTLE_TEST("Serene Voice heals for one-eighth of damage dealt by sound moves")
{
    s16 damage;
    s16 healed;

    GIVEN {
        PLAYER(SPECIES_SYLVEON) { HP(100); MaxHP(400); Speed(100); Ability(ABILITY_CUTE_CHARM); UniqueAbility(ABILITY_SERENE_VOICE); Moves(MOVE_HYPER_VOICE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); SpDefense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPER_VOICE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, player);
        HP_BAR(opponent, captureDamage: &damage);
        ABILITY_POPUP(player, ABILITY_SERENE_VOICE);
        HP_BAR(player, captureDamage: &healed);
        MESSAGE("Foe Wobbuffet had its energy drained!");
    } THEN {
        EXPECT_MUL_EQ(damage, Q_4_12(-1.0 / 8.0), healed);
    }
}

SINGLE_BATTLE_TEST("Serene Voice heals for one-quarter of damage dealt by sound moves in Misty Terrain")
{
    s16 damage;
    s16 healed;

    GIVEN {
        PLAYER(SPECIES_SYLVEON) { HP(100); MaxHP(400); Speed(50); Ability(ABILITY_CUTE_CHARM); UniqueAbility(ABILITY_SERENE_VOICE); Moves(MOVE_HYPER_VOICE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); SpDefense(100); Moves(MOVE_MISTY_TERRAIN); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_MISTY_TERRAIN); MOVE(player, MOVE_HYPER_VOICE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MISTY_TERRAIN, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, player);
        HP_BAR(opponent, captureDamage: &damage);
        ABILITY_POPUP(player, ABILITY_SERENE_VOICE);
        HP_BAR(player, captureDamage: &healed);
        MESSAGE("Foe Wobbuffet had its energy drained!");
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_MISTY_TERRAIN);
        EXPECT_MUL_EQ(damage, Q_4_12(-1.0 / 4.0), healed);
    }
}

SINGLE_BATTLE_TEST("Serene Voice does not heal from non-sound moves")
{
    GIVEN {
        PLAYER(SPECIES_SYLVEON) { HP(100); MaxHP(400); Speed(100); Ability(ABILITY_CUTE_CHARM); UniqueAbility(ABILITY_SERENE_VOICE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SERENE_VOICE);
            HP_BAR(player);
            MESSAGE("Foe Wobbuffet had its energy drained!");
        }
    }
}
