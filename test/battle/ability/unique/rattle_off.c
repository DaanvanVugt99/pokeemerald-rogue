#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Rattle Off uses Roar when Spiky Shield blocks contact")
{
    u16 ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_MAGIC_GUARD; }
    GIVEN {
        PLAYER(SPECIES_MARACTUS) { Ability(ABILITY_ROUGH_SKIN); UniqueAbility(ABILITY_RATTLE_OFF); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(player, MOVE_SPIKY_SHIELD); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        if (ability != ABILITY_MAGIC_GUARD)
            HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_RATTLE_OFF);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROAR, player);
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_WYNAUT);
    }
}

SINGLE_BATTLE_TEST("Rattle Off requires Spiky Shield to block a contact hit from a surviving foe")
{
    u16 shield, move, ability, hp;
    PARAMETRIZE { shield = MOVE_PROTECT; move = MOVE_TACKLE; ability = ABILITY_NONE; hp = 100; }
    PARAMETRIZE { shield = MOVE_SPIKY_SHIELD; move = MOVE_WATER_GUN; ability = ABILITY_NONE; hp = 100; }
    PARAMETRIZE { shield = MOVE_SPIKY_SHIELD; move = MOVE_TACKLE; ability = ABILITY_UNSEEN_FIST; hp = 100; }
    PARAMETRIZE { shield = MOVE_SPIKY_SHIELD; move = MOVE_TACKLE; ability = ABILITY_NONE; hp = 1; }
    GIVEN {
        PLAYER(SPECIES_MARACTUS) { UniqueAbility(ABILITY_RATTLE_OFF); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); HP(hp); MaxHP(100); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN {
            MOVE(player, shield);
            MOVE(opponent, move);
            if (hp == 1)
                SEND_OUT(opponent, 1);
        }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_RATTLE_OFF);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ROAR, player);
        }
    }
}

DOUBLE_BATTLE_TEST("Rattle Off can answer both foes behind the same Spiky Shield")
{
    GIVEN {
        PLAYER(SPECIES_MARACTUS) { UniqueAbility(ABILITY_RATTLE_OFF); Speed(20); }
        PLAYER(SPECIES_WYNAUT) { Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(80); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); }
        OPPONENT(SPECIES_WYNAUT) { Speed(40); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SPIKY_SHIELD);
            MOVE(opponentLeft, MOVE_TACKLE, target: playerLeft);
            MOVE(opponentRight, MOVE_TACKLE, target: playerLeft);
        }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_RATTLE_OFF);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROAR, playerLeft);
        ABILITY_POPUP(playerLeft, ABILITY_RATTLE_OFF);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROAR, playerLeft);
    }
}
