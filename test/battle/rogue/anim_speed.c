#include "global.h"
#include "battle.h"
#include "battle_controllers.h"
#include "test/battle.h"

WILD_BATTLE_TEST("(ROGUE) Catching speed completes independently of wild battle speed")
{
    u8 opt;

    for (opt = 0; opt < OPTIONS_ANIM_SPEED_COUNT; opt++)
    {
        PARAMETRIZE
        {
            gSaveBlock2Ptr->optionsCatchingSpeed = i;
            gSaveBlock2Ptr->optionsWildBattleScene = OPTIONS_BATTLE_SCENE_4X - i;
        }
    }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CATERPIE) { HP(1); }
    } WHEN {
        TURN { USE_ITEM(player, ITEM_MASTER_BALL); }
    } SCENE {
        MESSAGE("You used Master Ball!");
        ANIMATION(ANIM_TYPE_SPECIAL, B_ANIM_BALL_THROW, player);
    } THEN {
        EXPECT(!BtlController_IsBallThrowAnimActive());
    } FINALLY {
        gSaveBlock2Ptr->optionsCatchingSpeed = OPTIONS_ANIM_SPEED_1X;
        gSaveBlock2Ptr->optionsWildBattleScene = OPTIONS_BATTLE_SCENE_2X;
    }
}

WILD_BATTLE_TEST("(ROGUE) Catching speed clears after a failed capture")
{
    PARAMETRIZE
    {
        gSaveBlock2Ptr->optionsCatchingSpeed = OPTIONS_ANIM_SPEED_4X;
        gSaveBlock2Ptr->optionsWildBattleScene = OPTIONS_BATTLE_SCENE_1X;
    }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ARCEUS) { Level(100); }
    } WHEN {
        TURN { USE_ITEM(player, ITEM_POKE_BALL); }
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("You used Poké Ball!");
        ANIMATION(ANIM_TYPE_SPECIAL, B_ANIM_BALL_THROW, player);
        MESSAGE("Wobbuffet used Celebrate!");
    } THEN {
        EXPECT(!BtlController_IsBallThrowAnimActive());
    } FINALLY {
        gSaveBlock2Ptr->optionsCatchingSpeed = OPTIONS_ANIM_SPEED_1X;
        gSaveBlock2Ptr->optionsWildBattleScene = OPTIONS_BATTLE_SCENE_2X;
    }
}

SINGLE_BATTLE_TEST("(ROGUE) Background fade doesn't stall: Acrobatics")
{
    u8 opt;

    for(opt = 0; opt < OPTIONS_BATTLE_SCENE_COUNT; ++opt)
    {
        PARAMETRIZE { gSaveBlock2Ptr->optionsTrainerBattleScene = i; }
    }
    GIVEN {
        PLAYER(SPECIES_CHANSEY);
        OPPONENT(SPECIES_CHANSEY);
    } WHEN {
        TURN { MOVE(player, MOVE_ACROBATICS); MOVE(opponent, MOVE_ACROBATICS); }
    } SCENE {
        MESSAGE("Chansey used Acrobatics!");
        //ANIMATION(ANIM_TYPE_MOVE, MOVE_ACROBATICS, player);
        MESSAGE("Foe Chansey used Acrobatics!");
        //ANIMATION(ANIM_TYPE_MOVE, MOVE_ACROBATICS, opponent);
    }
    FINALLY
    {
        gSaveBlock2Ptr->optionsTrainerBattleScene = OPTIONS_BATTLE_SCENE_1X;
    }
}

SINGLE_BATTLE_TEST("(ROGUE) Background fade doesn't stall: Psychic")
{
    u8 opt;

    for(opt = 0; opt < OPTIONS_BATTLE_SCENE_COUNT; ++opt)
    {
        PARAMETRIZE { gSaveBlock2Ptr->optionsTrainerBattleScene = i; }
    }
    GIVEN {
        PLAYER(SPECIES_CHANSEY);
        OPPONENT(SPECIES_CHANSEY);
    } WHEN {
        TURN { MOVE(player, MOVE_PSYCHIC); MOVE(opponent, MOVE_PSYCHIC); }
    } SCENE {
        MESSAGE("Chansey used Psychic!");
        //ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC, player);
        MESSAGE("Foe Chansey used Psychic!");
        //ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC, opponent);
    }
    FINALLY
    {
        gSaveBlock2Ptr->optionsTrainerBattleScene = OPTIONS_BATTLE_SCENE_1X;
    }
}
