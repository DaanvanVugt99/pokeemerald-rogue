#ifndef ROGUE_GIFTS_H
#define ROGUE_GIFTS_H

#include "global.h"
#include "constants/generated/custom_mons.h"

enum
{
    UNIQUE_RARITY_COMMON,       // 2 moves, or type + 1 type move
    UNIQUE_RARITY_RARE,         // 1 move & ability, or type + type move + 2 moves
    UNIQUE_RARITY_EPIC,         // 2 moves & ability, or type + type move + 1 move & ability
    UNIQUE_RARITY_EXOTIC,       // bespoke made mons for Quest rewards
    UNIQUE_RARITY_LEGENDARY,    // Epic payload plus unique ability
};

u32 RogueGift_GetCustomMonId(struct Pokemon* mon);
u32 RogueGift_GetCustomBoxMonId(struct BoxPokemon* mon);
u32 RogueGift_GetCustomMonIdBySpecies(u16 species, u32 otId);

u16 RogueGift_GetCustomMonMove(u32 id, u8 i);
u16 RogueGift_GetCustomMonMoveCount(u32 id);

u16 RogueGift_GetCustomMonAbility(u32 id, u8 i);
u16 RogueGift_GetCustomMonAbilityCount(u32 id);
u16 RogueGift_GetCustomMonUniqueAbility(u32 id);
bool8 RogueGift_IsDynamicUniqueAbilityEligible(u16 ability);
u16 RogueGift_GetDynamicUniqueAbilityPoolCount(void);

#ifdef ROGUE_DEBUG
u16 RogueGift_DebugGetDynamicSynergyProfileId(u16 ability);
u16 RogueGift_DebugGetDynamicSynergyMove(u16 ability, u8 choice);
u8 RogueGift_DebugGetDynamicSynergyPolicy(u16 ability);
u8 RogueGift_DebugSelectDynamicSynergyChoice(u16 species, u16 ability, u16 suppliedMove);
u16 RogueGift_DebugSelectCreationSynergyMove(u16 species, u8 type, u16 suppliedMove);
#endif

u8 RogueGift_GetCustomMonType(u32 id, u8 i);

bool8 RogueGift_CanRenameCustomMon(u32 id);

bool8 RogueGift_DisplayCustomMonRarity(u32 id);
u8 RogueGift_GetCustomMonRarity(u32 id);
u8 const* RogueGift_GetRarityName(u8 rarity);

void RogueGift_CreateMon(u32 customMonId, struct Pokemon* mon, u16 species, u8 level, u8 fixedIV);
u32 RogueGift_CreateDynamicMonId(u8 rarity, u16 species);
u32 RogueGift_CreateDynamicMonIdRaw(u8 rarity, u16 species);
u8 RogueGift_RollDynamicUniqueRarity(bool8 ignoreUnlockGates);

void RogueGift_EnsureDynamicCustomMonsAreValid();
void RogueGift_ClearDynamicCustomMons();
void RogueGift_CountDownDynamicCustomMons();
void RogueGift_RemoveDynamicCustomMon(u32 customMonId);
struct UniqueMon* RogueGift_GetDynamicUniqueMon(u8 slot);
bool8 RogueGift_IsDynamicMonSlotEnabled(u8 slot);

u32 RogueGift_TryFindEnabledDynamicCustomMonForSpecies(u16 species);

bool8 RogueGift_TryApplyPaletteModify(u32 id, bool8 isShiny, u16 const* inputPal, u16 const* layerRefPal, u16* outputPal);

#endif // ROGUE_GIFTS_H
