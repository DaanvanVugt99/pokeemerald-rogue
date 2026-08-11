#include "global.h"
#include "rogue_popup.h"
#include "test/test.h"

TEST("Clearing popups cancels deferred queue consumption")
{
    EXPECT(RoguePopup_TestBeginDeferredConsume());
    EXPECT(RoguePopup_TestDeferredCloseWillConsume());

    Rogue_ClearPopupQueue();

    EXPECT(!RoguePopup_TestDeferredCloseWillConsume());
    EXPECT(RoguePopup_TestQueueIsEmpty());
    RoguePopup_TestEndDeferredConsume();
}
