#ifndef ROGUE_GIFTS_H
#define ROGUE_GIFTS_H

#include "global.h"
#include "constants/generated/custom_mons.h"

enum
{
    UNIQUE_RARITY_COMMON,       // 2 exotic moves, or a new type + 1 typed move
    UNIQUE_RARITY_RARE,         // 1 exotic move + standard ability, or new type + typed move + 2 exotic moves
    UNIQUE_RARITY_EPIC,         // 2 exotic moves + standard ability, or new type + typed move + 1 exotic move + standard ability
    UNIQUE_RARITY_EXOTIC,       // bespoke made mons for Quest rewards
    UNIQUE_RARITY_LEGENDARY,    // 2 total custom moves + standard and unique abilities
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
bool8 RogueGift_IsAnomalousUniqueAbility(u16 ability);
u16 RogueGift_GetDynamicUniqueAbilityPoolCount(void);

#ifdef ROGUE_DEBUG
u16 RogueGift_DebugGetDynamicSynergyProfileId(u16 ability);
u16 RogueGift_DebugGetDynamicSynergyMove(u16 ability, u8 choice);
u8 RogueGift_DebugSelectDynamicSynergyChoice(u16 species, u16 ability, u16 suppliedMove);
u16 RogueGift_DebugSelectDynamicSynergyMove(u16 species, u16 ability, u16 move1, u16 move2);
u16 RogueGift_DebugSelectCreationSynergyMove(u16 species, u8 type, u16 suppliedMove);
bool8 RogueGift_DebugDoesMoveMatchDynamicSynergy(u16 ability, u16 move);
bool8 RogueGift_DebugDoesMoveMatchCreationSynergy(u8 type, u16 move);
u16 RogueGift_DebugGetDynamicMovePoolCount(void);
u16 RogueGift_DebugGetDynamicExoticMoveCount(void);
u16 RogueGift_DebugGetDynamicMoveByIndex(u16 index);
bool8 RogueGift_DebugIsMoveInDynamicPool(u16 move);
bool8 RogueGift_DebugIsMoveExotic(u16 move);
bool8 RogueGift_DebugIsMoveNativeToEvolutionFamily(u16 species, u16 move);
bool8 RogueGift_DebugIsStandardAbilityNativeToEvolutionFamily(u16 species, u16 ability);
bool8 RogueGift_DebugIsStandardAbilityEligible(u16 ability);
u16 RogueGift_DebugGetStandardAbilityGroupCount(void);
u16 RogueGift_DebugGetStandardAbilityGroupRepresentative(u16 ability);
u8 RogueGift_DebugGetStandardAbilityFlavorCount(u16 ability);
u16 RogueGift_DebugGetStandardAbilityFlavor(u16 ability, u8 flavor);
bool8 RogueGift_DebugIsUniqueAbilityNativeToEvolutionFamily(u16 species, u16 ability);
u16 RogueGift_DebugGetEvolutionFamilyExoticMoveCount(u16 species);
bool8 RogueGift_DebugIsSpeciesInDynamicUniquePool(u16 species);
bool8 RogueGift_DebugAllDynamicSpeciesHaveExoticMoves(u8 requiredCount);
u16 RogueGift_DebugSelectRandomDynamicSpecies(void);
u32 RogueGift_DebugCreateAnomalousMonId(u16 species);
#endif

u8 RogueGift_GetCustomMonType(u32 id, u8 i);

bool8 RogueGift_CanRenameCustomMon(u32 id);

bool8 RogueGift_DisplayCustomMonRarity(u32 id);
u8 RogueGift_GetCustomMonRarity(u32 id);
u8 const* RogueGift_GetRarityName(u8 rarity);

void RogueGift_CreateMon(u32 customMonId, struct Pokemon* mon, u16 species, u8 level, u8 fixedIV);
u32 RogueGift_CreateDynamicMonId(u8 rarity, u16 species);
u32 RogueGift_CreateDynamicMonIdRaw(u8 rarity, u16 species);
u32 RogueGift_CreateDynamicMonIdRawWithTypingChance(u8 rarity, u16 species, u8 typingChance);
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
