#ifndef ROGUE_BATTLEHUD_H
#define ROGUE_BATTLEHUD_H

void RogueBH_CreateBattleOverlay();
void RogueBH_RemoveBattleOverlay(bool32 fromResetSprites);

u16 RogueBH_GetInspectButtons(void);
void RogueBH_ShowInspectHint(void);
void RogueBH_HideInspectHint(void);
void RogueBH_ToggleStatView();
bool8 RogueBH_IsStatViewActive();
void RogueBH_HandleStatViewUpdate(u32 battler);
void RogueBH_PrintStatView();

#endif