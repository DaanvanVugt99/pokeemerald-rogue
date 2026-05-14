#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ODOR_SLEUTH].effect == EFFECT_FORESIGHT);
    ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_PHYSICAL);
}

SINGLE_BATTLE_TEST("Bloodhound uses Odor Sleuth on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_BOLTUND) { Ability(ABILITY_STRONG_JAW); UniqueAbility(ABILITY_BLOODHOUND); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BLOODHOUND);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ODOR_SLEUTH, player);
    } THEN {
        EXPECT(opponent->status2 & STATUS2_FORESIGHT);
    }
}

SINGLE_BATTLE_TEST("Bloodhound heals one-quarter of damage dealt to identified targets")
{
    s16 damage;
    s16 healed;

    GIVEN {
        PLAYER(SPECIES_BOLTUND) { HP(120); MaxHP(200); Ability(ABILITY_STRONG_JAW); UniqueAbility(ABILITY_BLOODHOUND); Moves(MOVE_ODOR_SLEUTH, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ODOR_SLEUTH); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent, captureDamage: &damage);
        ABILITY_POPUP(player, ABILITY_BLOODHOUND);
        HP_BAR(player, captureDamage: &healed);
        MESSAGE("Foe Wobbuffet had its energy drained!");
    } THEN {
        EXPECT_MUL_EQ(damage, Q_4_12(-1.0 / 4.0), healed);
    }
}
