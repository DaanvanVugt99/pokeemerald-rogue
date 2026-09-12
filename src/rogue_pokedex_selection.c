#include "global.h"
#include "rogue.h"
#include "rogue_pokedex.h"
#include "rogue_pokedex_selection.h"
#include "string_util.h"
#include "constants/items.h"

static const u8 sRegional[] = _("Regional");
static const u8 sNational[] = _("National");
static const u8 sSpecial[] = _("Special");
static const u8 sHisui[] = _("Hisui");
static const u8 sNone[] = _("None");
static const u8 sMega[] = _("Mega Evolution");
static const u8 sZMoves[] = _("Z-Moves");
static const u8 sDynamax[] = _("Dynamax");
static const u8 sTera[] = _("Terastallization");
static const u8 sRegionalDescription[] = _("From this game's regional Pokedex.");
static const u8 sNationalDescription[] = _("All Pokemon through Generation ");
static const u8 sSpecialDescription[] = _("Pokemon from this special roster.");
static const u16 sGimmicks[] = { ITEM_NONE, ITEM_MEGA_RING, ITEM_Z_POWER_RING, ITEM_DYNAMAX_BAND, ITEM_TERA_ORB };
static const u8 sRegionOrder[] = { 1, 2, 3, 4, DEX_REGION_HISUI, 5, 6, 7, 8, 9 };

STATIC_ASSERT(POKEDEX_VARIANT_COUNT <= 64, pokedexSelectionMaskCapacity);

u8 RogueDexSelection_GetType(u8 variant)
{
    if (variant >= POKEDEX_VARIANT_NATIONAL_GEN1 && variant <= POKEDEX_VARIANT_NATIONAL_GEN9)
        return DEX_POOL_NATIONAL;
    if (variant == POKEDEX_VARIANT_ROGUE_MODERN || variant == POKEDEX_VARIANT_ROGUE_CLASSICPLUS
     || variant == POKEDEX_VARIANT_EXTRAS_CONQUEST || variant == POKEDEX_VARIANT_EXTRAS_COLOSSEUM)
        return DEX_POOL_SPECIAL;
    return DEX_POOL_REGIONAL;
}

u8 RogueDexSelection_GetRegion(u8 variant)
{
    u8 region, i;
    if (variant == POKEDEX_VARIANT_EXTRAS_LEGENDSARCEUS) return DEX_REGION_HISUI;
    if (variant == POKEDEX_VARIANT_LEGENDS_ZA || variant == POKEDEX_VARIANT_LEGENDS_ZAFULLDLC) return POKEDEX_REGION_KALOS;
    for (region = POKEDEX_REGION_KANTO; region <= POKEDEX_REGION_PALDEA; ++region)
        for (i = 0; i < gPokedexRegions[region].variantCount; ++i)
            if (gPokedexRegions[region].variantList[i] == variant) return region;
    return POKEDEX_REGION_NONE;
}

const u8 *RogueDexSelection_GetTypeName(u8 type)
{
    return type == DEX_POOL_NATIONAL ? sNational : type == DEX_POOL_SPECIAL ? sSpecial : sRegional;
}

const u8 *RogueDexSelection_GetRegionName(u8 region)
{
    if (region == DEX_REGION_HISUI) return sHisui;
    return region < POKEDEX_REGION_COUNT ? gPokedexRegions[region].displayName : sNone;
}

const u8 *RogueDexSelection_GetGimmickName(u16 item)
{
    switch (item)
    {
    case ITEM_MEGA_RING: return sMega;
    case ITEM_Z_POWER_RING: return sZMoves;
    case ITEM_DYNAMAX_BAND: return sDynamax;
    case ITEM_TERA_ORB: return sTera;
    default: return sNone;
    }
}

static bool8 IsAllowed(const struct RoguePokedexSelection *selection, u8 variant)
{
    return variant < POKEDEX_VARIANT_COUNT && (selection->allowed & (1ULL << variant)) != 0;
}

bool8 RogueDexSelection_Init(struct RoguePokedexSelection *selection, u64 allowed, u8 variant, u16 gimmick, bool8 intro)
{
    u8 i;
    memset(selection, 0, sizeof(*selection));
    selection->allowed = allowed & (~0ULL >> (64 - POKEDEX_VARIANT_COUNT));
    if (intro)
        for (i = 0; i < POKEDEX_VARIANT_COUNT; ++i)
            if (RogueDexSelection_GetType(i) == DEX_POOL_SPECIAL
             || RogueDexSelection_GetRegion(i) == DEX_REGION_HISUI)
                selection->allowed &= ~(1ULL << i);
    selection->intro = intro;
    selection->variant = variant;
    for (i = 0; i < ARRAY_COUNT(sGimmicks); ++i)
        if (gimmick == sGimmicks[i]) selection->nationalGimmick = gimmick;
    if (!IsAllowed(selection, variant))
    {
        for (i = 0; i < POKEDEX_VARIANT_COUNT; ++i)
            if (IsAllowed(selection, i)) { selection->variant = i; return TRUE; }
        selection->variant = POKEDEX_VARIANT_NONE;
        return FALSE;
    }
    return TRUE;
}

u8 RogueDexSelection_GetChoices(const struct RoguePokedexSelection *selection, u8 field, u8 *choices)
{
    u8 count = 0, i, variant, type = RogueDexSelection_GetType(selection->variant);
    u8 region = RogueDexSelection_GetRegion(selection->variant);
    if (field == DEX_FIELD_GIMMICK)
    {
        if (selection->intro && type == DEX_POOL_NATIONAL)
            for (i = 0; i < ARRAY_COUNT(sGimmicks); ++i) choices[count++] = i;
    }
    else if (field == DEX_FIELD_TYPE || field == DEX_FIELD_REGION)
    {
        if (field == DEX_FIELD_REGION && type != DEX_POOL_REGIONAL) return 0;
        for (i = 0; i < (field == DEX_FIELD_TYPE ? DEX_POOL_TYPE_COUNT : ARRAY_COUNT(sRegionOrder)); ++i)
        {
            u8 value = field == DEX_FIELD_TYPE ? i : sRegionOrder[i];
            for (variant = 0; variant < POKEDEX_VARIANT_COUNT; ++variant)
                if (IsAllowed(selection, variant)
                 && (field == DEX_FIELD_TYPE ? RogueDexSelection_GetType(variant) == value
                  : RogueDexSelection_GetType(variant) == type && RogueDexSelection_GetRegion(variant) == value))
                { choices[count++] = value; break; }
        }
    }
    else if (field == DEX_FIELD_VARIANT)
    {
        for (variant = 0; variant < POKEDEX_VARIANT_COUNT; ++variant)
            if (IsAllowed(selection, variant) && RogueDexSelection_GetType(variant) == type
             && (type != DEX_POOL_REGIONAL || RogueDexSelection_GetRegion(variant) == region))
                choices[count++] = variant;
    }
    return count;
}

bool8 RogueDexSelection_Cycle(struct RoguePokedexSelection *selection, u8 field, s8 direction)
{
    u8 choices[POKEDEX_VARIANT_COUNT];
    u8 count = RogueDexSelection_GetChoices(selection, field, choices);
    u8 value, i, current = field == DEX_FIELD_TYPE ? RogueDexSelection_GetType(selection->variant)
        : field == DEX_FIELD_REGION ? RogueDexSelection_GetRegion(selection->variant) : selection->variant;
    if (count < 2) return FALSE;
    if (field == DEX_FIELD_GIMMICK)
        for (i = 0; i < ARRAY_COUNT(sGimmicks); ++i)
            if (selection->nationalGimmick == sGimmicks[i]) current = i;
    for (i = 0; i < count && choices[i] != current; ++i) {}
    value = choices[(i + count + (direction < 0 ? -1 : 1)) % count];
    if (field == DEX_FIELD_GIMMICK) selection->nationalGimmick = sGimmicks[value];
    else if (field == DEX_FIELD_VARIANT) selection->variant = value;
    else
        for (i = 0; i < POKEDEX_VARIANT_COUNT; ++i)
            if (IsAllowed(selection, i)
             && (field == DEX_FIELD_TYPE ? RogueDexSelection_GetType(i) == value
              : RogueDexSelection_GetType(i) == DEX_POOL_REGIONAL && RogueDexSelection_GetRegion(i) == value))
            { selection->variant = i; break; }
    return TRUE;
}

u16 RogueDexSelection_GetGimmick(const struct RoguePokedexSelection *selection)
{
    return RogueDexSelection_GetType(selection->variant) == DEX_POOL_NATIONAL
        ? selection->nationalGimmick : RoguePokedex_GetNativeGimmickItem(selection->variant);
}

void RogueDexSelection_Describe(const struct RoguePokedexSelection *selection, u8 *text)
{
    u8 type = RogueDexSelection_GetType(selection->variant);
    if (type == DEX_POOL_NATIONAL)
        ConvertIntToDecimalStringN(StringCopy(text, sNationalDescription),
            selection->variant - POKEDEX_VARIANT_NATIONAL_GEN1 + 1, STR_CONV_MODE_LEFT_ALIGN, 1);
    else StringCopy(text, type == DEX_POOL_REGIONAL ? sRegionalDescription : sSpecialDescription);
}
