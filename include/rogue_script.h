#ifndef ROGUE_SCRIPT_H
#define ROGUE_SCRIPT_H

#include "global.h"

#define ROGUE_SAFARI_OFFER_MAX_COST_ITEMS 3

struct RogueSafariOfferCostItem
{
    u16 itemId;
    u16 requiredCount;
    u16 ownedCount;
    u8 type;
    bool8 isShinyCost;
};

struct RogueSafariOfferDetails
{
    u16 species;
    u16 picSpecies;
    u32 otId;
    u32 personality;
    bool8 isShiny;
    bool8 canPurchase;
    u8 gender;
    u8 type1;
    u8 type2;
    u8 costCount;
    u8 displayName[64];
    struct RogueSafariOfferCostItem costs[ROGUE_SAFARI_OFFER_MAX_COST_ITEMS];
};

bool8 Rogue_CheckPartyHasRoomForMon(void);
bool8 Rogue_CanPurchaseSafariMon(u16 safariIndex);
bool8 Rogue_GetSafariMonOfferDetails(u16 safariIndex, struct RogueSafariOfferDetails *details);

void Rogue_RandomisePartyMon(void);
void Rogue_AlterMonIVs(void);
void Rogue_ApplyStatusToMon(void);

u16 Rogue_GetMonEvoCount(void);
void Rogue_GetMonEvoParams(void);

void RogueDebug_FillGenPC(void);

#endif
