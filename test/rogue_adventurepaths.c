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

TEST("An exhausted path is replaced after its boss")
{
    struct RogueAdvPath originalPath = gRogueAdvPath;
    u8 originalRoomId = gRogueRun.adventureRoomId;

    gRogueAdvPath.roomCount = 1;
    gRogueAdvPath.justGenerated = FALSE;
    gRogueAdvPath.rooms[0].roomType = ADVPATH_ROOM_BOSS;
    gRogueRun.adventureRoomId = ADVPATH_INVALID_ROOM_ID;

    EXPECT(RogueAdv_GenerateAdventurePathsIfRequired());
    EXPECT(gRogueAdvPath.justGenerated);
    EXPECT_GT(gRogueAdvPath.roomCount, 1);

    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
}
