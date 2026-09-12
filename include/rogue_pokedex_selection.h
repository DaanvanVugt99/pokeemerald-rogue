#ifndef GUARD_ROGUE_POKEDEX_SELECTION_H
#define GUARD_ROGUE_POKEDEX_SELECTION_H

#include "global.h"

enum { DEX_POOL_REGIONAL, DEX_POOL_NATIONAL, DEX_POOL_SPECIAL, DEX_POOL_TYPE_COUNT };
enum { DEX_FIELD_TYPE, DEX_FIELD_REGION, DEX_FIELD_VARIANT, DEX_FIELD_GIMMICK, DEX_FIELD_DONE };
#define DEX_REGION_HISUI 10

struct RoguePokedexSelection
{
    u64 allowed;
    u16 nationalGimmick;
    u8 variant;
    bool8 intro;
};

u8 RogueDexSelection_GetType(u8 variant);
u8 RogueDexSelection_GetRegion(u8 variant);
const u8 *RogueDexSelection_GetTypeName(u8 type);
const u8 *RogueDexSelection_GetRegionName(u8 region);
const u8 *RogueDexSelection_GetGimmickName(u16 item);
bool8 RogueDexSelection_Init(struct RoguePokedexSelection *selection, u64 allowed, u8 variant, u16 gimmick, bool8 intro);
u8 RogueDexSelection_GetChoices(const struct RoguePokedexSelection *selection, u8 field, u8 *choices);
bool8 RogueDexSelection_Cycle(struct RoguePokedexSelection *selection, u8 field, s8 direction);
u16 RogueDexSelection_GetGimmick(const struct RoguePokedexSelection *selection);
void RogueDexSelection_Describe(const struct RoguePokedexSelection *selection, u8 *text);

#endif
