#include "global.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/rogue.h"
#include "constants/species.h"
#include "event_data.h"
#include "item.h"
#include "pokemon.h"
#include "rogue_controller.h"
#include "rogue_followmon.h"
#include "rogue_gifts.h"
#include "rogue_popup.h"
#include "rogue_ridemon.h"
#include "rogue_script.h"
#include "test/test.h"

static struct Pokemon sRideTestPartyBackup[PARTY_SIZE];
static struct BoxPokemon sRideTestDaycareBackup;

static void SetRideStealthTestState(u16 rideSpecies, bool8 isRiding, bool8 isUnlocked)
{
    struct RogueRideMonState *rideState;

    Rogue_RideMonInit();
    rideState = Rogue_GetPlayerRideMonStatePtr();
    rideState->monGfx = rideSpecies;

    if(isRiding)
        gPlayerAvatar.flags |= PLAYER_AVATAR_FLAG_RIDING;
    else
        gPlayerAvatar.flags &= ~PLAYER_AVATAR_FLAG_RIDING;

    if(isUnlocked)
        FlagSet(FLAG_SYS_RIDING_STEALTH);
    else
        FlagClear(FLAG_SYS_RIDING_STEALTH);
}

TEST("Ride Stealth requires its upgrade and an active compatible mount")
{
    SetRideStealthTestState(SPECIES_ZOROARK, TRUE, FALSE);
    EXPECT(!Rogue_IsRideMonStealthActive());

    SetRideStealthTestState(SPECIES_ZOROARK, FALSE, TRUE);
    EXPECT(!Rogue_IsRideMonStealthActive());

    SetRideStealthTestState(SPECIES_STANTLER, TRUE, TRUE);
    EXPECT(!Rogue_IsRideMonStealthActive());

    SetRideStealthTestState(SPECIES_ZOROARK, TRUE, TRUE);
    EXPECT(Rogue_IsRideMonStealthActive());

    FlagClear(FLAG_SYS_RIDING_STEALTH);
    gPlayerAvatar.flags &= ~PLAYER_AVATAR_FLAG_RIDING;
    Rogue_RideMonInit();
}

TEST("Ride Stealth handles shiny graphics and inherited ride forms")
{
    SetRideStealthTestState(SPECIES_ZOROARK + FOLLOWMON_SHINY_OFFSET, TRUE, TRUE);
    EXPECT(Rogue_IsRideMonStealthActive());

    EXPECT(Rogue_IsValidRideStealthSpecies(SPECIES_DECIDUEYE_HISUIAN));

    FlagClear(FLAG_SYS_RIDING_STEALTH);
    gPlayerAvatar.flags &= ~PLAYER_AVATAR_FLAG_RIDING;
    Rogue_RideMonInit();
}

TEST("Gold Whistle replaces Basic Whistle in its exact registered slot")
{
    u16 registeredItemsBackup[MAX_REGISTERED_ITEMS];
    bool8 hadBasicWhistle = CheckBagHasItem(ITEM_BASIC_RIDING_WHISTLE, 1);
    bool8 hadGoldWhistle = CheckBagHasItem(ITEM_GOLD_RIDING_WHISTLE, 1);

    memcpy(registeredItemsBackup, gSaveBlock1Ptr->registeredItems, sizeof(registeredItemsBackup));
    RemoveBagItem(ITEM_BASIC_RIDING_WHISTLE, 1);
    RemoveBagItem(ITEM_GOLD_RIDING_WHISTLE, 1);
    EXPECT(AddBagItem(ITEM_BASIC_RIDING_WHISTLE, 1));
    EXPECT(AddBagItem(ITEM_GOLD_RIDING_WHISTLE, 1));

    gSaveBlock1Ptr->registeredItems[0] = ITEM_HEALING_FLASK;
    gSaveBlock1Ptr->registeredItems[1] = ITEM_GOLD_RIDING_WHISTLE;
    gSaveBlock1Ptr->registeredItems[2] = ITEM_BASIC_RIDING_WHISTLE;
    gSaveBlock1Ptr->registeredItems[3] = ITEM_QUEST_LOG;

    Rogue_NormalizeRidingWhistles();

    EXPECT(!CheckBagHasItem(ITEM_BASIC_RIDING_WHISTLE, 1));
    EXPECT(CheckBagHasItem(ITEM_GOLD_RIDING_WHISTLE, 1));
    EXPECT_EQ(gSaveBlock1Ptr->registeredItems[0], ITEM_HEALING_FLASK);
    EXPECT_EQ(gSaveBlock1Ptr->registeredItems[1], ITEM_NONE);
    EXPECT_EQ(gSaveBlock1Ptr->registeredItems[2], ITEM_GOLD_RIDING_WHISTLE);
    EXPECT_EQ(gSaveBlock1Ptr->registeredItems[3], ITEM_QUEST_LOG);

    RemoveBagItem(ITEM_GOLD_RIDING_WHISTLE, 1);
    if(hadBasicWhistle)
        AddBagItem(ITEM_BASIC_RIDING_WHISTLE, 1);
    if(hadGoldWhistle)
        AddBagItem(ITEM_GOLD_RIDING_WHISTLE, 1);
    memcpy(gSaveBlock1Ptr->registeredItems, registeredItemsBackup, sizeof(registeredItemsBackup));
}

TEST("Gold Whistle combines party Day Care and park mounts")
{
    struct Pokemon daycareMon;
    u32 partyCustomMonId = OTID_FLAG_CUSTOM_MON | OTID_FLAG_DYNAMIC_CUSTOM_MON | 1;
    u32 daycareCustomMonId = OTID_FLAG_CUSTOM_MON | OTID_FLAG_DYNAMIC_CUSTOM_MON | 2;
    u32 registeredCustomMonId = OTID_FLAG_CUSTOM_MON | OTID_FLAG_DYNAMIC_CUSTOM_MON | 3;
    u32 registeredCustomMonBackup = gRogueSaveBlock->registeredRideMonCustomId;
    u8 daycareSlotCount = Rogue_GetCurrentDaycareSlotCount();
    u8 partyCountBackup = gPlayerPartyCount;
    u16 registeredRideBackup = VarGet(VAR_ROGUE_REGISTERED_RIDE_MON);

    memcpy(sRideTestPartyBackup, gPlayerParty, sizeof(sRideTestPartyBackup));
    sRideTestDaycareBackup = *Rogue_GetDaycareBoxMon(0);

    ZeroPlayerPartyMons();
    CreateMon(&gPlayerParty[0], SPECIES_STANTLER, 5, 0, FALSE, 0, OT_ID_PLAYER_ID, 0);
    SetMonData(&gPlayerParty[0], MON_DATA_OT_ID, &partyCustomMonId);
    gPlayerPartyCount = 1;
    CreateMon(&daycareMon, SPECIES_LAPRAS, 5, 0, FALSE, 0, OT_ID_PLAYER_ID, 0);
    SetMonData(&daycareMon, MON_DATA_OT_ID, &daycareCustomMonId);
    *Rogue_GetDaycareBoxMon(0) = daycareMon.box;
    VarSet(VAR_ROGUE_REGISTERED_RIDE_MON, SPECIES_ZOROARK);
    gRogueSaveBlock->registeredRideMonCustomId = registeredCustomMonId;

    EXPECT_EQ(RogueDebug_GetRideOptionCount(RIDE_WHISTLE_GOLD), 1 + daycareSlotCount + 1);
    EXPECT_EQ(RogueDebug_GetRideOptionGfx(RIDE_WHISTLE_GOLD, 0), FollowMon_GetMonGraphics(&gPlayerParty[0]));
    EXPECT_EQ(RogueDebug_GetRideOptionGfx(RIDE_WHISTLE_GOLD, 1), FollowMon_GetBoxMonGraphics(Rogue_GetDaycareBoxMon(0)));
    EXPECT_EQ(RogueDebug_GetRideOptionGfx(RIDE_WHISTLE_GOLD, 1 + daycareSlotCount), SPECIES_ZOROARK);
    EXPECT_EQ(RogueDebug_GetRideOptionCustomMonId(RIDE_WHISTLE_GOLD, 0), partyCustomMonId);
    EXPECT_EQ(RogueDebug_GetRideOptionCustomMonId(RIDE_WHISTLE_GOLD, 1), daycareCustomMonId);
    EXPECT_EQ(RogueDebug_GetRideOptionCustomMonId(RIDE_WHISTLE_GOLD, 1 + daycareSlotCount), registeredCustomMonId);

    memcpy(gPlayerParty, sRideTestPartyBackup, sizeof(sRideTestPartyBackup));
    gPlayerPartyCount = partyCountBackup;
    *Rogue_GetDaycareBoxMon(0) = sRideTestDaycareBackup;
    VarSet(VAR_ROGUE_REGISTERED_RIDE_MON, registeredRideBackup);
    gRogueSaveBlock->registeredRideMonCustomId = registeredCustomMonBackup;
}

TEST("Flight grants three takeoffs per Adventure room")
{
    bool8 wasRunActive = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    u16 chargesBackup = VarGet(VAR_ROGUE_REMAINING_FLIGHT_CHARGES);

    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    Rogue_ClearPopupQueue();
    Rogue_ResetFlightCharges();

    EXPECT_EQ(Rogue_GetMaxFlightCharges(), 3);
    EXPECT_EQ(Rogue_GetRemainingFlightCharges(), 3);
    Rogue_DecreaseFlightCharges();
    EXPECT_EQ(Rogue_GetRemainingFlightCharges(), 2);
    Rogue_DecreaseFlightCharges();
    Rogue_DecreaseFlightCharges();
    Rogue_DecreaseFlightCharges();
    EXPECT_EQ(Rogue_GetRemainingFlightCharges(), 0);

    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    EXPECT_EQ(Rogue_GetRemainingFlightCharges(), 999);

    VarSet(VAR_ROGUE_REMAINING_FLIGHT_CHARGES, chargesBackup);
    Rogue_ClearPopupQueue();
    if(wasRunActive)
        FlagSet(FLAG_ROGUE_RUN_ACTIVE);
}
