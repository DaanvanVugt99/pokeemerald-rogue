#ifndef GUARD_MOVE_RELEARNER_H
#define GUARD_MOVE_RELEARNER_H

#include "main.h"

void TeachMoveSetContextRelearnMove(void);
void TeachMoveSetContextEggMove(void);
void TeachMoveSetContextTutorMove(void);
void TeachMoveSetContextUnbound(void);
void TeachMoveFromContext(void);
void TeachMoveRelearnerMove(void);

void TeachMoveFromContextFromTask(u8 taskId);
void TeachMoveFromSummaryScreen(MainCallback returnCallback);
void CB2_InitLearnMove(void);

u8 GetNumberOfRelearnableMovesForContext(struct Pokemon* mon);
void MoveRelearnerShowHideHearts(s32);

#endif //GUARD_MOVE_RELEARNER_H
