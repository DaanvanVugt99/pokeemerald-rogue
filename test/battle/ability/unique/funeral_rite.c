#include "global.h"
#include "test/battle.h"

#if B_EXPANDED_MOVE_NAMES
#define REVIVAL_BLESSING "Revival Blessing"
#else
#define REVIVAL_BLESSING "RevivlBlesng"
#endif

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_REVIVAL_BLESSING].effect == EFFECT_REVIVAL_BLESSING);
    ASSUME(gBattleMoves[MOVE_TRICK_ROOM].effect == EFFECT_TRICK_ROOM);
}

SINGLE_BATTLE_TEST("Funeral Rite uses Trick Room after Revival Blessing succeeds")
{
    bool32 hasFaintedPartyMember;

    PARAMETRIZE { hasFaintedPartyMember = TRUE; }
    PARAMETRIZE { hasFaintedPartyMember = FALSE; }

    GIVEN {
        PLAYER(SPECIES_RABSCA) { Moves(MOVE_REVIVAL_BLESSING); }
        PLAYER(SPECIES_WOBBUFFET) { if (hasFaintedPartyMember) HP(0); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_ROOM_SERVICE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        if (hasFaintedPartyMember)
            TURN { MOVE(player, MOVE_REVIVAL_BLESSING); SEND_OUT(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        else
            TURN { MOVE(player, MOVE_REVIVAL_BLESSING); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Rabsca used " REVIVAL_BLESSING "!");
        if (hasFaintedPartyMember)
        {
            MESSAGE("Wobbuffet was revived and is ready to fight again!");
            ABILITY_POPUP(player, ABILITY_FUNERAL_RITE);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TRICK_ROOM, player);
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, opponent);
        }
        else
        {
            MESSAGE("But it failed!");
            NONE_OF {
                ABILITY_POPUP(player, ABILITY_FUNERAL_RITE);
                ANIMATION(ANIM_TYPE_MOVE, MOVE_TRICK_ROOM, player);
            }
        }
    } THEN {
        if (hasFaintedPartyMember)
        {
            EXPECT(gFieldStatuses & STATUS_FIELD_TRICK_ROOM);
            EXPECT_EQ(opponent->item, ITEM_NONE);
            EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
        }
        else
        {
            EXPECT(!(gFieldStatuses & STATUS_FIELD_TRICK_ROOM));
        }
    }
}
