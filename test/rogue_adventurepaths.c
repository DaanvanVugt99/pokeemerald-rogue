#include "global.h"
#include "constants/rogue.h"
#include "rogue.h"
#include "rogue_adventurepaths.h"
#include "rogue_controller.h"
#include "test/test.h"

TEST("A portal-pregenerated initial path is reused on map entry")
{
    u8 originalRoomCount = gRogueAdvPath.roomCount;
    bool8 originalJustGenerated = gRogueAdvPath.justGenerated;
    u8 originalRoomId = gRogueRun.adventureRoomId;

    gRogueAdvPath.roomCount = 1;
    gRogueAdvPath.justGenerated = TRUE;
    gRogueRun.adventureRoomId = ADVPATH_INVALID_ROOM_ID;

    EXPECT(RogueAdv_GenerateAdventurePathsIfRequired());
    EXPECT_EQ(gRogueAdvPath.roomCount, 1);
    EXPECT(gRogueAdvPath.justGenerated);

    gRogueAdvPath.roomCount = originalRoomCount;
    gRogueAdvPath.justGenerated = originalJustGenerated;
    gRogueRun.adventureRoomId = originalRoomId;
}
