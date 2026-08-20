#include "global.h"
#include "pokemon_summary_screen.h"
#include "text.h"
#include "constants/moves.h"
#include "test/test.h"

#define MOVE_INFO_DESCRIPTION_WIDTH 136
#define MOVE_INFO_LEFT_EDGE 4
#define MOVE_INFO_RIGHT_EDGE 140
#define MOVE_INFO_CATEGORY_ICON_LEFT 34

TEST("Move Info header fits inside its horizontal margins")
{
    static const u8 sCategoryText[] = _("CAT: ");
    static const u8 sMaxPowerText[] = _("PWR: 999");
    static const u8 sMaxAccuracyText[] = _("ACC: 100");

    EXPECT_LE(MOVE_INFO_LEFT_EDGE + GetStringWidth(FONT_NARROW, sCategoryText, 0) + 3, MOVE_INFO_CATEGORY_ICON_LEFT);
    EXPECT_LE(56 + GetStringWidth(FONT_NARROW, sMaxPowerText, 0), MOVE_INFO_RIGHT_EDGE);
    EXPECT_LE(104 + GetStringWidth(FONT_NARROW, sMaxAccuracyText, 0), MOVE_INFO_RIGHT_EDGE);
}

TEST("Move Info has a fitting description for every move")
{
    u16 move;

    for (move = MOVE_POUND; move < MOVES_COUNT_ALL; move++)
    {
        const u8 *description = GetMoveDescription(move);
        u8 fontId;
        s32 width;

        if (description == NULL)
            Test_ExitWithResult(TEST_RESULT_FAIL, "Move %d has no Move Info description", move);
        if (move >= MOVES_COUNT && description == gNotDoneYetDescription)
            Test_ExitWithResult(TEST_RESULT_FAIL, "Special move %d uses the fallback Move Info description", move);

        fontId = GetFontIdToFit(description, FONT_NORMAL, 0, MOVE_INFO_DESCRIPTION_WIDTH);
        width = GetStringWidth(fontId, description, 0);
        if (width > MOVE_INFO_DESCRIPTION_WIDTH)
            Test_ExitWithResult(TEST_RESULT_FAIL, "Move %d description is %d pixels wide", move, width);
    }
}
