#include "graphics.h"

extern const u8 gItemDesc_EvolutionItem[];
extern const u8 gItemDesc_QuestLog[];
extern const u8 gItemDesc_HealingFlask[];
extern const u8 gItemDesc_RidingWhistle[];
extern const u8 gItemDesc_GoldRidingWhistle[];
extern const u8 gItemDesc_CGear[];
extern const u8 gItemDesc_DaycarePhone[];
extern const u8 gItemDesc_BuildingSupplies[];
extern const u8 gItemDesc_PokeblockBundle[];
extern const u8 gItemDesc_BigPokeblockBundle[];
extern const u8 gItemDesc_TradeCase[];
extern const u8 gItemDesc_FieldShovel[];
extern const u8 gItemDesc_SmallCoinCase[];
extern const u8 gItemDesc_LargeCoinCase[];
extern const u8 gItemDesc_GoldenSeed[];
extern const u8 gItemDesc_UniqueScanner[];

extern const u8 gItemDesc_NormalPokeblock[];
extern const u8 gItemDesc_FightingPokeblock[];
extern const u8 gItemDesc_FlyingPokeblock[];
extern const u8 gItemDesc_PoisonPokeblock[];
extern const u8 gItemDesc_GroundPokeblock[];
extern const u8 gItemDesc_RockPokeblock[];
extern const u8 gItemDesc_BugPokeblock[];
extern const u8 gItemDesc_GhostPokeblock[];
extern const u8 gItemDesc_SteelPokeblock[];
extern const u8 gItemDesc_FirePokeblock[];
extern const u8 gItemDesc_WaterPokeblock[];
extern const u8 gItemDesc_GrassPokeblock[];
extern const u8 gItemDesc_ElectricPokeblock[];
extern const u8 gItemDesc_PsychicPokeblock[];
extern const u8 gItemDesc_IcePokeblock[];
extern const u8 gItemDesc_DragonPokeblock[];
extern const u8 gItemDesc_DarkPokeblock[];
extern const u8 gItemDesc_FairyPokeblock[];
extern const u8 gItemDesc_ShinyPokeblock[];
extern const u8 gItemDesc_StatPokeblock[];

extern const u8 gItemDesc_ShopPriceCharm[];
extern const u8 gItemDesc_ShopPriceCurse[];
extern const u8 gItemDesc_FlinchCharm[];
extern const u8 gItemDesc_FlinchCurse[];
extern const u8 gItemDesc_CritCharm[];
extern const u8 gItemDesc_CritCurse[];
extern const u8 gItemDesc_ShedSkinCharm[];
extern const u8 gItemDesc_ShedSkinCurse[];
extern const u8 gItemDesc_WildIVCharm[];
extern const u8 gItemDesc_WildIVCurse[];
extern const u8 gItemDesc_CatchingCharm[];
extern const u8 gItemDesc_CatchingCurse[];
extern const u8 gItemDesc_GraceCharm[];
extern const u8 gItemDesc_GraceCurse[];
extern const u8 gItemDesc_WildCharm[];
extern const u8 gItemDesc_WildCurse[];
extern const u8 gItemDesc_PriorityCharm[];
extern const u8 gItemDesc_PriorityCurse[];
extern const u8 gItemDesc_EndureCharm[];
extern const u8 gItemDesc_EndureCurse[];
extern const u8 gItemDesc_TormentCharm[];
extern const u8 gItemDesc_TormentCurse[];
extern const u8 gItemDesc_PressureCharm[];
extern const u8 gItemDesc_PressureCurse[];
extern const u8 gItemDesc_UnawareCharm[];
extern const u8 gItemDesc_UnawareCurse[];
extern const u8 gItemDesc_AdaptabilityCharm[];
extern const u8 gItemDesc_AdaptabilityCurse[];
extern const u8 gItemDesc_ExtraLifeCharm[];
extern const u8 gItemDesc_AllowSaveScumCharm[];
extern const u8 gItemDesc_TechnicianCharm[];
extern const u8 gItemDesc_TintedCharm[];
extern const u8 gItemDesc_IronFistCharm[];
extern const u8 gItemDesc_SharpnessCharm[];
extern const u8 gItemDesc_StrongJawCharm[];
extern const u8 gItemDesc_SkillCharm[];
extern const u8 gItemDesc_ReachCharm[];
extern const u8 gItemDesc_AccuracyCharm[];
extern const u8 gItemDesc_RecoveryCharm[];
extern const u8 gItemDesc_RecoilCharm[];
extern const u8 gItemDesc_GuardCharm[];
extern const u8 gItemDesc_ConversionCharm[];
extern const u8 gItemDesc_RetaliateCharm[];
extern const u8 gItemDesc_MomentumCharm[];
extern const u8 gItemDesc_StandCharm[];
extern const u8 gItemDesc_PrepCharm[];
extern const u8 gItemDesc_ProteanCharm[];
extern const u8 gItemDesc_LevelCharm[];
extern const u8 gItemDesc_RegenCharm[];
extern const u8 gItemDesc_MoodyCharm[];
extern const u8 gItemDesc_EvioliteCharm[];
extern const u8 gItemDesc_LevelCurse[];
extern const u8 gItemDesc_TintedCurse[];
extern const u8 gItemDesc_ReachCurse[];
extern const u8 gItemDesc_AccuracyCurse[];
extern const u8 gItemDesc_RetaliateCurse[];
extern const u8 gItemDesc_StandCurse[];
extern const u8 gItemDesc_CursedLens[];
extern const u8 gItemDesc_VowOfSilence[];
extern const u8 gItemDesc_BloodOath[];
extern const u8 gItemDesc_HollowSun[];
extern const u8 gItemDesc_MaliceOrb[];
extern const u8 gItemDesc_Graveglass[];
extern const u8 gItemDesc_AshenCrown[];
extern const u8 gItemDesc_WitchsThread[];
extern const u8 gItemDesc_PetrifiedHeart[];
extern const u8 gItemDesc_FalseIdol[];
extern const u8 gItemDesc_RustedAnchor[];
extern const u8 gItemDesc_GamblersClaw[];
extern const u8 gItemDesc_TempoDial[];
extern const u8 gItemDesc_TurnaboutTotem[];
extern const u8 gItemDesc_JesterSwitch[];
extern const u8 gItemDesc_WaywardIncense[];
extern const u8 gItemDesc_ChaosCharm[];
extern const u8 gItemDesc_MischiefQuill[];
extern const u8 gItemDesc_FinaleBell[];
extern const u8 gItemDesc_EclipseTotem[];
extern const u8 gItemDesc_SunTotem[];
extern const u8 gItemDesc_RainTotem[];
extern const u8 gItemDesc_SandTotem[];
extern const u8 gItemDesc_SnowTotem[];
extern const u8 gItemDesc_AcidRainTotem[];
extern const u8 gItemDesc_ElectricTiki[];
extern const u8 gItemDesc_GrassyTiki[];
extern const u8 gItemDesc_MistyTiki[];
extern const u8 gItemDesc_PsychicTiki[];
extern const u8 gItemDesc_GlassSword[];
extern const u8 gItemDesc_WonderShield[];
extern const u8 gItemDesc_EchoScepter[];
extern const u8 gItemDesc_GlyphCodex[];
extern const u8 gItemDesc_PreciseLens[];
extern const u8 gItemDesc_FuryMantle[];
extern const u8 gItemDesc_CompoundGoggles[];
extern const u8 gItemDesc_GreedyGloves[];
extern const u8 gItemDesc_ImpactPlating[];
extern const u8 gItemDesc_ChimeJewel[];
extern const u8 gItemDesc_AmbushTalon[];
extern const u8 gItemDesc_PurityJewel[];
extern const u8 gItemDesc_HexingWand[];
extern const u8 gItemDesc_FickleHat[];
extern const u8 gItemDesc_GoldenEgg[];
extern const u8 gItemDesc_BriarBracer[];
extern const u8 gItemDesc_TrickyBox[];
extern const u8 gItemDesc_AdaptiveSpecs[];
extern const u8 gItemDesc_Raincoat[];
extern const u8 gItemDesc_MonotypeSerum[];
extern const u8 gItemDesc_DecoyDoll[];
extern const u8 gItemDesc_WoodenSword[];
extern const u8 gItemDesc_PartyCurse[];
extern const u8 gItemDesc_EverstoneCurse[];
extern const u8 gItemDesc_BattleItemCurse[];
extern const u8 gItemDesc_SpeciesClauseCurse[];
extern const u8 gItemDesc_ItemShuffleCurse[];
extern const u8 gItemDesc_SnowballCurse[];
extern const u8 gItemDesc_RandomanRouteSpawnCurse[];
extern const u8 gItemDesc_RandomanAlwaysSpawnCurse[];
extern const u8 gItemDesc_AutoMoveSelectCurse[];
extern const u8 gItemDesc_OneHitCurse[];
extern const u8 gItemDesc_SnagCurse[];
extern const u8 gItemDesc_WildEggSpeciesCurse[];

const struct RogueItem gRogueItems[ITEM_ROGUE_ITEM_COUNT] =
{
    [ITEM_LINK_CABLE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Link Cable"),
#else
        .name = _("LINK CABLE"),
#endif
        .itemId = ITEM_LINK_CABLE,
        .price = 2100,
        .description = gItemDesc_EvolutionItem,
        .pocket = POCKET_HELD_ITEMS,
        .iconImage = gItemIcon_ExpShare,
        .iconPalette = gItemIconPalette_ExpShare,
    },

    [ITEM_QUEST_LOG - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Quest Book"),
#else
        .name = _("QUEST BOOK"),
#endif
        .itemId = ITEM_QUEST_LOG,
        .price = 0,
        .description = gItemDesc_QuestLog,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_QuestLog,
        .iconImage = gItemIcon_QuestBook,
        .iconPalette = gItemIconPalette_QuestBook,
    },

    [ITEM_HEALING_FLASK - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Healing Flask"),
#else
        .name = _("HEALING FLASK"),
#endif
        .itemId = ITEM_HEALING_FLASK,
        .price = 0,
        .description = gItemDesc_HealingFlask,
        .pocket = POCKET_KEY_ITEMS,
        .registrability = TRUE,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_HealingFlask,
        .iconImage = gItemIcon_Potion,
        .iconPalette = gItemIconPalette_HealingFlask,
    },

    [ITEM_BASIC_RIDING_WHISTLE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Poké Whistle"),
#else
        .name = _("POKé WHISTLE"),
#endif
        .itemId = ITEM_BASIC_RIDING_WHISTLE, 
        .price = 0,
        .description = gItemDesc_RidingWhistle,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_RideMon,
        .secondaryId = RIDE_WHISTLE_BASIC,
        .registrability = TRUE,
        .iconImage = gItemIcon_PokeWhistle,
        .iconPalette = gItemIconPalette_PokeWhistleBasic,
    },

    [ITEM_GOLD_RIDING_WHISTLE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Gold Whistle"),
#else
        .name = _("GOLD WHISTLE"),
#endif
        .itemId = ITEM_GOLD_RIDING_WHISTLE,
        .price = 0,
        .description = gItemDesc_GoldRidingWhistle,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_RideMon,
        .secondaryId = RIDE_WHISTLE_GOLD,
        .registrability = TRUE,
        .iconImage = gItemIcon_PokeWhistle,
        .iconPalette = gItemIconPalette_PokeWhistleGold,
    },

    [ITEM_C_GEAR - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("C-Gear"),
#else
        .name = _("C-GEAR"),
#endif
        .itemId = ITEM_C_GEAR,
        .price = 0,
        .description = gItemDesc_CGear,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_CGear,
        .secondaryId = 0,
        .registrability = TRUE,
        .iconImage = gItemIcon_CGear,
        .iconPalette = gItemIconPalette_CGear,
    },

    [ITEM_DAYCARE_PHONE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Pokégear"),
#else
        .name = _("POKéGEAR"),
#endif
        .itemId = ITEM_DAYCARE_PHONE,
        .price = 0,
        .description = gItemDesc_DaycarePhone,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_DayCarePhone,
        .secondaryId = 0,
        .registrability = TRUE,
        .iconImage = gItemIcon_PokeGear,
        .iconPalette = gItemIconPalette_PokeGear,
    },
    
    [ITEM_BUILDING_SUPPLIES - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Build Parts"),
#else
        .name = _("BUILD PARTS"),
#endif
        .itemId = ITEM_BUILDING_SUPPLIES,
        .price = 0,
        .description = gItemDesc_BuildingSupplies,
        .pocket = POCKET_KEY_ITEMS,
        .registrability = FALSE,
        .iconImage = gItemIcon_BuildResources,
        .iconPalette = gItemIconPalette_BuildResources,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        //.iconImage = gItemIcon_OaksParcel,
        //.iconPalette = gItemIconPalette_OaksParcel,
    },

    [ITEM_POKEBLOCK_BUNDLE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Small Bundle"),
#else
        .name = _("SMALL BUNDLE"),
#endif
        .itemId = ITEM_POKEBLOCK_BUNDLE,
        .price = 0,
        .description = gItemDesc_PokeblockBundle,
        .pocket = POCKET_KEY_ITEMS,
        .registrability = FALSE,
        .iconImage = gItemIcon_Parcel,
        .iconPalette = gItemIconPalette_Parcel,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
    },

    [ITEM_BIG_POKEBLOCK_BUNDLE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Large Bundle"),
#else
        .name = _("LARGE BUNDLE"),
#endif
        .itemId = ITEM_BIG_POKEBLOCK_BUNDLE,
        .price = 0,
        .description = gItemDesc_BigPokeblockBundle,
        .pocket = POCKET_KEY_ITEMS,
        .registrability = FALSE,
        .iconImage = gItemIcon_Parcel,
        .iconPalette = gItemIconPalette_Parcel,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
    },

    [ITEM_TRADE_CASE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Trade Case"),
#else
        .name = _("TRADE CASE"),
#endif
        .itemId = ITEM_TRADE_CASE,
        .price = 0,
        .description = gItemDesc_TradeCase,
        .pocket = POCKET_KEY_ITEMS,
        .registrability = FALSE,
        .iconImage = gItemIcon_Parcel,
        .iconPalette = gItemIconPalette_Parcel,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
    },

    [ITEM_FIELD_SHOVEL - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Field Shovel"),
#else
        .name = _("FIELD SHOVEL"),
#endif
        .itemId = ITEM_FIELD_SHOVEL,
        .price = 0,
        .description = gItemDesc_FieldShovel,
        .pocket = POCKET_KEY_ITEMS,
        .registrability = FALSE,
        .iconImage = gItemIcon_FieldShovel,
        .iconPalette = gItemIconPalette_FieldShovel,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
    },

#ifdef ROGUE_EXPANSION
    [ITEM_ALOLA_STONE - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Alola Artefact"),
        .itemId = ITEM_ALOLA_STONE,
        .price = 2100,
        .description = gItemDesc_EvolutionItem,
        .pocket = POCKET_ITEMS,
        .iconImage = gItemIcon_AlolaStone,
        .iconPalette = gItemIconPalette_AlolaStone,
    },
    [ITEM_GALAR_STONE - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Galar Artefact"),
        .itemId = ITEM_GALAR_STONE,
        .price = 2100,
        .description = gItemDesc_EvolutionItem,
        .pocket = POCKET_ITEMS,
        .iconImage = gItemIcon_GalarStone,
        .iconPalette = gItemIconPalette_GalarStone,
    },
    [ITEM_HISUI_STONE - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Hisui Artefact"),
        .itemId = ITEM_HISUI_STONE,
        .price = 2100,
        .description = gItemDesc_EvolutionItem,
        .pocket = POCKET_ITEMS,
        .iconImage = gItemIcon_HisuiStone,
        .iconPalette = gItemIconPalette_HisuiStone,
    },
#endif
    [ITEM_SMALL_COIN_CASE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Small Coin Case"),
#else
        .name = _("SMALL COIN CASE"),
#endif
        .itemId = ITEM_SMALL_COIN_CASE,
        .price = 1300,
        .description = gItemDesc_SmallCoinCase,
        .pocket = POCKET_ITEMS,
        .iconImage = gItemIcon_CoinCase,
        .iconPalette = gItemIconPalette_CoinCase,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
    },
    [ITEM_LARGE_COIN_CASE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Large Coin Case"),
#else
        .name = _("LARGE COIN CASE"),
#endif
        .itemId = ITEM_LARGE_COIN_CASE,
        .price = 12500,
        .description = gItemDesc_LargeCoinCase,
        .pocket = POCKET_ITEMS,
        .iconImage = gItemIcon_CoinCase,
        .iconPalette = gItemIconPalette_CoinCase,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
    },

    [ITEM_GOLDEN_SEED - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Golden Seed"),
#else
        .name = _("GOLDEN SEED"),
#endif
        .itemId = ITEM_GOLDEN_SEED,
        .price = 0,
        .description = gItemDesc_GoldenSeed,
        .pocket = POCKET_KEY_ITEMS,
        .iconImage = gItemIcon_GoldenSeed,
        .iconPalette = gItemIconPalette_GoldenSeed,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_GoldenSeed,
    },

    [ITEM_UNIQUE_SCANNER - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Unique Scanner"),
#else
        .name = _("UNIQUE SCANNER"),
#endif
        .itemId = ITEM_UNIQUE_SCANNER,
        .price = 0,
        .description = gItemDesc_UniqueScanner,
        .pocket = POCKET_KEY_ITEMS,
        .iconImage = gItemIcon_Scanner,
        .iconPalette = gItemIconPalette_Scanner,
        .type = ITEM_USE_FIELD,
        .registrability = TRUE,
        .fieldUseFunc = ItemUseOutOfBattle_UniqueScanner,
    },

    // Pokeblock
    //
    [ITEM_POKEBLOCK_NORMAL - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Normal {POKEBLOCK}"),
#else
        .name = _("NORMAL {POKEBLOCK}"),
#endif
        .itemId = ITEM_POKEBLOCK_NORMAL,
        .secondaryId = TYPE_NORMAL,
        .price = 0,
        .description = gItemDesc_NormalPokeblock,
        .pocket = POCKET_POKEBLOCK,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_Pokeblock,
        .registrability = FALSE,
        .iconImage = gItemIcon_Pokeblock,
        .iconPalette = gItemIconPalette_PokeblockNormal,
    },
    [ITEM_POKEBLOCK_FIGHTING - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Fight. {POKEBLOCK}"),
#else
        .name = _("FIGHT. {POKEBLOCK}"),
#endif
        .itemId = ITEM_POKEBLOCK_FIGHTING,
        .secondaryId = TYPE_FIGHTING,
        .price = 0,
        .description = gItemDesc_FightingPokeblock,
        .pocket = POCKET_POKEBLOCK,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_Pokeblock,
        .registrability = FALSE,
        .iconImage = gItemIcon_Pokeblock,
        .iconPalette = gItemIconPalette_PokeblockFighting,
    },
    [ITEM_POKEBLOCK_FLYING - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Flying {POKEBLOCK}"),
#else
        .name = _("FLYING {POKEBLOCK}"),
#endif
        .itemId = ITEM_POKEBLOCK_FLYING,
        .secondaryId = TYPE_FLYING,
        .price = 0,
        .description = gItemDesc_FlyingPokeblock,
        .pocket = POCKET_POKEBLOCK,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_Pokeblock,
        .registrability = FALSE,
        .iconImage = gItemIcon_Pokeblock,
        .iconPalette = gItemIconPalette_PokeblockFlying,
    },
    [ITEM_POKEBLOCK_POISON - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Poison {POKEBLOCK}"),
#else
        .name = _("POISON {POKEBLOCK}"),
#endif
        .itemId = ITEM_POKEBLOCK_POISON,
        .secondaryId = TYPE_POISON,
        .price = 0,
        .description = gItemDesc_PoisonPokeblock,
        .pocket = POCKET_POKEBLOCK,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_Pokeblock,
        .registrability = FALSE,
        .iconImage = gItemIcon_Pokeblock,
        .iconPalette = gItemIconPalette_PokeblockPoison,
    },
    [ITEM_POKEBLOCK_GROUND - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Ground {POKEBLOCK}"),
#else
        .name = _("GROUND {POKEBLOCK}"),
#endif
        .itemId = ITEM_POKEBLOCK_GROUND,
        .secondaryId = TYPE_GROUND,
        .price = 0,
        .description = gItemDesc_GroundPokeblock,
        .pocket = POCKET_POKEBLOCK,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_Pokeblock,
        .registrability = FALSE,
        .iconImage = gItemIcon_Pokeblock,
        .iconPalette = gItemIconPalette_PokeblockGround,
    },
    [ITEM_POKEBLOCK_ROCK - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Rock {POKEBLOCK}"),
#else
        .name = _("ROCK {POKEBLOCK}"),
#endif
        .itemId = ITEM_POKEBLOCK_ROCK,
        .secondaryId = TYPE_ROCK,
        .price = 0,
        .description = gItemDesc_RockPokeblock,
        .pocket = POCKET_POKEBLOCK,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_Pokeblock,
        .registrability = FALSE,
        .iconImage = gItemIcon_Pokeblock,
        .iconPalette = gItemIconPalette_PokeblockRock,
    },
    [ITEM_POKEBLOCK_BUG - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Bug {POKEBLOCK}"),
#else
        .name = _("BUG {POKEBLOCK}"),
#endif
        .itemId = ITEM_POKEBLOCK_BUG,
        .secondaryId = TYPE_BUG,
        .price = 0,
        .description = gItemDesc_BugPokeblock,
        .pocket = POCKET_POKEBLOCK,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_Pokeblock,
        .registrability = FALSE,
        .iconImage = gItemIcon_Pokeblock,
        .iconPalette = gItemIconPalette_PokeblockBug,
    },
    [ITEM_POKEBLOCK_GHOST - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Ghost {POKEBLOCK}"),
#else
        .name = _("GHOST {POKEBLOCK}"),
#endif
        .itemId = ITEM_POKEBLOCK_GHOST,
        .secondaryId = TYPE_GHOST,
        .price = 0,
        .description = gItemDesc_GhostPokeblock,
        .pocket = POCKET_POKEBLOCK,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_Pokeblock,
        .registrability = FALSE,
        .iconImage = gItemIcon_Pokeblock,
        .iconPalette = gItemIconPalette_PokeblockGhost,
    },
    [ITEM_POKEBLOCK_STEEL - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Steel {POKEBLOCK}"),
#else
        .name = _("STEEL {POKEBLOCK}"),
#endif
        .itemId = ITEM_POKEBLOCK_STEEL,
        .secondaryId = TYPE_STEEL,
        .price = 0,
        .description = gItemDesc_SteelPokeblock,
        .pocket = POCKET_POKEBLOCK,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_Pokeblock,
        .registrability = FALSE,
        .iconImage = gItemIcon_Pokeblock,
        .iconPalette = gItemIconPalette_PokeblockSteel,
    },
    [ITEM_POKEBLOCK_FIRE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Fire {POKEBLOCK}"),
#else
        .name = _("FIRE {POKEBLOCK}"),
#endif
        .itemId = ITEM_POKEBLOCK_FIRE,
        .secondaryId = TYPE_FIRE,
        .price = 0,
        .description = gItemDesc_FirePokeblock,
        .pocket = POCKET_POKEBLOCK,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_Pokeblock,
        .registrability = FALSE,
        .iconImage = gItemIcon_Pokeblock,
        .iconPalette = gItemIconPalette_PokeblockFire,
    },
    [ITEM_POKEBLOCK_WATER - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Water {POKEBLOCK}"),
#else
        .name = _("WATER {POKEBLOCK}"),
#endif
        .itemId = ITEM_POKEBLOCK_WATER,
        .secondaryId = TYPE_WATER,
        .price = 0,
        .description = gItemDesc_WaterPokeblock,
        .pocket = POCKET_POKEBLOCK,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_Pokeblock,
        .registrability = FALSE,
        .iconImage = gItemIcon_Pokeblock,
        .iconPalette = gItemIconPalette_PokeblockWater,
    },
    [ITEM_POKEBLOCK_GRASS - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Grass {POKEBLOCK}"),
#else
        .name = _("GRASS {POKEBLOCK}"),
#endif
        .itemId = ITEM_POKEBLOCK_GRASS,
        .secondaryId = TYPE_GRASS,
        .price = 0,
        .description = gItemDesc_GrassPokeblock,
        .pocket = POCKET_POKEBLOCK,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_Pokeblock,
        .registrability = FALSE,
        .iconImage = gItemIcon_Pokeblock,
        .iconPalette = gItemIconPalette_PokeblockGrass,
    },
    [ITEM_POKEBLOCK_ELECTRIC - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Elec. {POKEBLOCK}"),
#else
        .name = _("ELEC. {POKEBLOCK}"),
#endif
        .itemId = ITEM_POKEBLOCK_ELECTRIC,
        .secondaryId = TYPE_ELECTRIC,
        .price = 0,
        .description = gItemDesc_ElectricPokeblock,
        .pocket = POCKET_POKEBLOCK,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_Pokeblock,
        .registrability = FALSE,
        .iconImage = gItemIcon_Pokeblock,
        .iconPalette = gItemIconPalette_PokeblockElectric,
    },
    [ITEM_POKEBLOCK_PSYCHIC - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Psychic {POKEBLOCK}"),
#else
        .name = _("PSYCHIC {POKEBLOCK}"),
#endif
        .itemId = ITEM_POKEBLOCK_PSYCHIC,
        .secondaryId = TYPE_PSYCHIC,
        .price = 0,
        .description = gItemDesc_PsychicPokeblock,
        .pocket = POCKET_POKEBLOCK,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_Pokeblock,
        .registrability = FALSE,
        .iconImage = gItemIcon_Pokeblock,
        .iconPalette = gItemIconPalette_PokeblockPsychic,
    },
    [ITEM_POKEBLOCK_ICE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Ice {POKEBLOCK}"),
#else
        .name = _("ICE {POKEBLOCK}"),
#endif
        .itemId = ITEM_POKEBLOCK_ICE,
        .secondaryId = TYPE_ICE,
        .price = 0,
        .description = gItemDesc_IcePokeblock,
        .pocket = POCKET_POKEBLOCK,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_Pokeblock,
        .registrability = FALSE,
        .iconImage = gItemIcon_Pokeblock,
        .iconPalette = gItemIconPalette_PokeblockIce,
    },
    [ITEM_POKEBLOCK_DRAGON - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Dragon {POKEBLOCK}"),
#else
        .name = _("DRAGON {POKEBLOCK}"),
#endif
        .itemId = ITEM_POKEBLOCK_DRAGON,
        .secondaryId = TYPE_DRAGON,
        .price = 0,
        .description = gItemDesc_DragonPokeblock,
        .pocket = POCKET_POKEBLOCK,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_Pokeblock,
        .registrability = FALSE,
        .iconImage = gItemIcon_Pokeblock,
        .iconPalette = gItemIconPalette_PokeblockDragon,
    },
    [ITEM_POKEBLOCK_DARK - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Dark {POKEBLOCK}"),
#else
        .name = _("DARK {POKEBLOCK}"),
#endif
        .itemId = ITEM_POKEBLOCK_DARK,
        .secondaryId = TYPE_DARK,
        .price = 0,
        .description = gItemDesc_DarkPokeblock,
        .pocket = POCKET_POKEBLOCK,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_Pokeblock,
        .registrability = FALSE,
        .iconImage = gItemIcon_Pokeblock,
        .iconPalette = gItemIconPalette_PokeblockDark,
    },
#ifdef ROGUE_EXPANSION
    [ITEM_POKEBLOCK_FAIRY - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Fairy {POKEBLOCK}"),
        .itemId = ITEM_POKEBLOCK_FAIRY,
        .secondaryId = TYPE_FAIRY,
        .price = 0,
        .description = gItemDesc_FairyPokeblock,
        .pocket = POCKET_POKEBLOCK,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_Pokeblock,
        .registrability = FALSE,
        .iconImage = gItemIcon_Pokeblock,
        .iconPalette = gItemIconPalette_PokeblockFairy,
    },
#endif
    [ITEM_POKEBLOCK_SHINY - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Shiny {POKEBLOCK}"),
#else
        .name = _("SHINY {POKEBLOCK}"),
#endif
        .itemId = ITEM_POKEBLOCK_SHINY,
        .secondaryId = TYPE_MYSTERY, // just fill in with a placeholder
        .price = 0,
        .description = gItemDesc_ShinyPokeblock,
        .pocket = POCKET_POKEBLOCK,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_Pokeblock,
        .registrability = FALSE,
        .iconImage = gItemIcon_PokeblockShiny,
        .iconPalette = gItemIconPalette_PokeblockShiny,
    },
    [ITEM_POKEBLOCK_HP - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("HP {POKEBLOCK}"),
        .itemId = ITEM_POKEBLOCK_HP,
        .secondaryId = TYPE_NONE,
        .price = 0,
        .description = gItemDesc_StatPokeblock,
        .pocket = POCKET_POKEBLOCK,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_Pokeblock,
        .registrability = FALSE,
        .iconImage = gItemIcon_Pokeblock,
        .iconPalette = gItemIconPalette_PokeblockStat,
    },
    [ITEM_POKEBLOCK_ATK - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Atk {POKEBLOCK}"),
#else
        .name = _("ATK {POKEBLOCK}"),
#endif
        .itemId = ITEM_POKEBLOCK_ATK,
        .secondaryId = TYPE_NONE,
        .price = 0,
        .description = gItemDesc_StatPokeblock,
        .pocket = POCKET_POKEBLOCK,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_Pokeblock,
        .registrability = FALSE,
        .iconImage = gItemIcon_Pokeblock,
        .iconPalette = gItemIconPalette_PokeblockStat,
    },
    [ITEM_POKEBLOCK_DEF - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Def {POKEBLOCK}"),
#else
        .name = _("DEF {POKEBLOCK}"),
#endif
        .itemId = ITEM_POKEBLOCK_DEF,
        .secondaryId = TYPE_NONE,
        .price = 0,
        .description = gItemDesc_StatPokeblock,
        .pocket = POCKET_POKEBLOCK,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_Pokeblock,
        .registrability = FALSE,
        .iconImage = gItemIcon_Pokeblock,
        .iconPalette = gItemIconPalette_PokeblockStatAlt,
    },
    [ITEM_POKEBLOCK_SPEED - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Speed {POKEBLOCK}"),
#else
        .name = _("SPEED {POKEBLOCK}"),
#endif
        .itemId = ITEM_POKEBLOCK_SPEED,
        .secondaryId = TYPE_NONE,
        .price = 0,
        .description = gItemDesc_StatPokeblock,
        .pocket = POCKET_POKEBLOCK,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_Pokeblock,
        .registrability = FALSE,
        .iconImage = gItemIcon_Pokeblock,
        .iconPalette = gItemIconPalette_PokeblockStatAlt,
    },
    [ITEM_POKEBLOCK_SPATK - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Sp.Atk {POKEBLOCK}"),
#else
        .name = _("SP.ATK {POKEBLOCK}"),
#endif
        .itemId = ITEM_POKEBLOCK_SPATK,
        .secondaryId = TYPE_NONE,
        .price = 0,
        .description = gItemDesc_StatPokeblock,
        .pocket = POCKET_POKEBLOCK,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_Pokeblock,
        .registrability = FALSE,
        .iconImage = gItemIcon_Pokeblock,
        .iconPalette = gItemIconPalette_PokeblockStat,
    },
    [ITEM_POKEBLOCK_SPDEF - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Sp.Def {POKEBLOCK}"),
#else
        .name = _("SP.DEF {POKEBLOCK}"),
#endif
        .itemId = ITEM_POKEBLOCK_SPDEF,
        .secondaryId = TYPE_NONE,
        .price = 0,
        .description = gItemDesc_StatPokeblock,
        .pocket = POCKET_POKEBLOCK,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_Pokeblock,
        .registrability = FALSE,
        .iconImage = gItemIcon_Pokeblock,
        .iconPalette = gItemIconPalette_PokeblockStatAlt,
    },


    // Charms
    //
    [ITEM_SHOP_PRICE_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Discount Charm"),
#else
        .name = _("DISCOUNT CHARM"),
#endif
        .itemId = ITEM_SHOP_PRICE_CHARM,
        .price = 0,
        .description = gItemDesc_ShopPriceCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_FLINCH_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Flinch Charm"),
#else
        .name = _("FLINCH CHARM"),
#endif
        .itemId = ITEM_FLINCH_CHARM,
        .price = 0,
        .description = gItemDesc_FlinchCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_CRIT_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Crit Charm"),
#else
        .name = _("CRIT CHARM"),
#endif
        .itemId = ITEM_CRIT_CHARM,
        .price = 0,
        .description = gItemDesc_CritCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_SHED_SKIN_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Shed Skin Charm"),
#else
        .name = _("SHED SKIN CHARM"),
#endif
        .itemId = ITEM_SHED_SKIN_CHARM,
        .price = 0,
        .description = gItemDesc_ShedSkinCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_WILD_IV_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Strength Charm"),
#else
        .name = _("STRENGTH CHARM"),
#endif
        .itemId = ITEM_WILD_IV_CHARM,
        .price = 0,
        .description = gItemDesc_WildIVCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_CATCHING_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Catching Charm"),
#else
        .name = _("CATCHING CHARM"),
#endif
        .itemId = ITEM_CATCHING_CHARM,
        .price = 0,
        .description = gItemDesc_CatchingCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_GRACE_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Grace Charm"),
#else
        .name = _("GRACE CHARM"),
#endif
        .itemId = ITEM_GRACE_CHARM,
        .price = 0,
        .description = gItemDesc_GraceCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_WILD_ENCOUNTER_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Wild Charm"),
#else
        .name = _("WILD CHARM"),
#endif
        .itemId = ITEM_WILD_ENCOUNTER_CHARM,
        .price = 0,
        .description = gItemDesc_WildCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_MOVE_PRIORITY_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Priority Charm"),
#else
        .name = _("PRIORITY CHARM"),
#endif
        .itemId = ITEM_MOVE_PRIORITY_CHARM,
        .price = 0,
        .description = gItemDesc_PriorityCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_ENDURE_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Endure Charm"),
#else
        .name = _("ENDURE CHARM"),
#endif
        .itemId = ITEM_ENDURE_CHARM,
        .price = 0,
        .description = gItemDesc_EndureCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_TORMENT_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Torment Charm"),
#else
        .name = _("TORMENT CHARM"),
#endif
        .itemId = ITEM_TORMENT_CHARM,
        .price = 0,
        .description = gItemDesc_TormentCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_PRESSURE_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Pressure Charm"),
#else
        .name = _("PRESSURE CHARM"),
#endif
        .itemId = ITEM_PRESSURE_CHARM,
        .price = 0,
        .description = gItemDesc_PressureCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_UNAWARE_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Unaware Charm"),
#else
        .name = _("UNAWARE CHARM"),
#endif
        .itemId = ITEM_UNAWARE_CHARM,
        .price = 0,
        .description = gItemDesc_UnawareCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_ADAPTABILITY_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("AdaptbltyCharm"),
#else
        .name = _("ADAPTBLTYCHARM"),
#endif
        .itemId = ITEM_ADAPTABILITY_CHARM,
        .price = 0,
        .description = gItemDesc_AdaptabilityCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_INFINITE_EXTRA_LIFE_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Retry Charm"),
#else
        .name = _("RETRY CHARM"),
#endif
        .itemId = ITEM_INFINITE_EXTRA_LIFE_CHARM,
        .price = 0,
        .description = gItemDesc_ExtraLifeCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_ALLOW_SAVE_SCUM_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Save Charm"),
#else
        .name = _("SAVE CHARM"),
#endif
        .itemId = ITEM_ALLOW_SAVE_SCUM_CHARM,
        .price = 0,
        .description = gItemDesc_AllowSaveScumCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_TECHNICIAN_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Tech Charm"),
#else
        .name = _("TECH CHARM"),
#endif
        .itemId = ITEM_TECHNICIAN_CHARM,
        .price = 0,
        .description = gItemDesc_TechnicianCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_TINTED_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Tinted Charm"),
#else
        .name = _("TINTED CHARM"),
#endif
        .itemId = ITEM_TINTED_CHARM,
        .price = 0,
        .description = gItemDesc_TintedCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_IRON_FIST_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Iron Fist Charm"),
#else
        .name = _("IRON FIST CHARM"),
#endif
        .itemId = ITEM_IRON_FIST_CHARM,
        .price = 0,
        .description = gItemDesc_IronFistCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_SHARPNESS_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Sharpness Charm"),
#else
        .name = _("SHARPNESS CHARM"),
#endif
        .itemId = ITEM_SHARPNESS_CHARM,
        .price = 0,
        .description = gItemDesc_SharpnessCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_STRONG_JAW_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Jaw Charm"),
#else
        .name = _("JAW CHARM"),
#endif
        .itemId = ITEM_STRONG_JAW_CHARM,
        .price = 0,
        .description = gItemDesc_StrongJawCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_SKILL_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Skill Charm"),
#else
        .name = _("SKILL CHARM"),
#endif
        .itemId = ITEM_SKILL_CHARM,
        .price = 0,
        .description = gItemDesc_SkillCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_REACH_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Reach Charm"),
#else
        .name = _("REACH CHARM"),
#endif
        .itemId = ITEM_REACH_CHARM,
        .price = 0,
        .description = gItemDesc_ReachCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_ACCURACY_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Accuracy Charm"),
#else
        .name = _("ACCURACY CHARM"),
#endif
        .itemId = ITEM_ACCURACY_CHARM,
        .price = 0,
        .description = gItemDesc_AccuracyCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_RECOVERY_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Recovery Charm"),
#else
        .name = _("RECOVERY CHARM"),
#endif
        .itemId = ITEM_RECOVERY_CHARM,
        .price = 0,
        .description = gItemDesc_RecoveryCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_RECOIL_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Recoil Charm"),
#else
        .name = _("RECOIL CHARM"),
#endif
        .itemId = ITEM_RECOIL_CHARM,
        .price = 0,
        .description = gItemDesc_RecoilCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_GUARD_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Guard Charm"),
#else
        .name = _("GUARD CHARM"),
#endif
        .itemId = ITEM_GUARD_CHARM,
        .price = 0,
        .description = gItemDesc_GuardCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_CONVERSION_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Convert Charm"),
#else
        .name = _("CONVERT CHARM"),
#endif
        .itemId = ITEM_CONVERSION_CHARM,
        .price = 0,
        .description = gItemDesc_ConversionCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_RETALIATE_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Retaliate Charm"),
#else
        .name = _("RETALIATE CHARM"),
#endif
        .itemId = ITEM_RETALIATE_CHARM,
        .price = 0,
        .description = gItemDesc_RetaliateCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_MOMENTUM_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Momentum Charm"),
#else
        .name = _("MOMENTUM CHARM"),
#endif
        .itemId = ITEM_MOMENTUM_CHARM,
        .price = 0,
        .description = gItemDesc_MomentumCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_STAND_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Stand Charm"),
#else
        .name = _("STAND CHARM"),
#endif
        .itemId = ITEM_STAND_CHARM,
        .price = 0,
        .description = gItemDesc_StandCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_PREP_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Prep Charm"),
#else
        .name = _("PREP CHARM"),
#endif
        .itemId = ITEM_PREP_CHARM,
        .price = 0,
        .description = gItemDesc_PrepCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_PROTEAN_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Protean Charm"),
#else
        .name = _("PROTEAN CHARM"),
#endif
        .itemId = ITEM_PROTEAN_CHARM,
        .price = 0,
        .description = gItemDesc_ProteanCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_LEVEL_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Level Charm"),
#else
        .name = _("LEVEL CHARM"),
#endif
        .itemId = ITEM_LEVEL_CHARM,
        .price = 0,
        .description = gItemDesc_LevelCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_REGEN_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Regen Charm"),
#else
        .name = _("REGEN CHARM"),
#endif
        .itemId = ITEM_REGEN_CHARM,
        .price = 0,
        .description = gItemDesc_RegenCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_MOODY_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Moody Charm"),
#else
        .name = _("MOODY CHARM"),
#endif
        .itemId = ITEM_MOODY_CHARM,
        .price = 0,
        .description = gItemDesc_MoodyCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_EVIOLITE_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Eviolite Charm"),
#else
        .name = _("EVIOLITE CHARM"),
#endif
        .itemId = ITEM_EVIOLITE_CHARM,
        .price = 0,
        .description = gItemDesc_EvioliteCharm,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_SHOP_PRICE_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Discount Curse"),
#else
        .name = _("DISCOUND CURSE"),
#endif
        .itemId = ITEM_SHOP_PRICE_CURSE,
        .price = 0,
        .description = gItemDesc_ShopPriceCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_FLINCH_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Flinch Curse"),
#else
        .name = _("FLINCH CURSE"),
#endif
        .itemId = ITEM_FLINCH_CURSE,
        .price = 0,
        .description = gItemDesc_FlinchCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_CRIT_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Crit Curse"),
#else
        .name = _("CRIT CURSE"),
#endif
        .itemId = ITEM_CRIT_CURSE,
        .price = 0,
        .description = gItemDesc_CritCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_SHED_SKIN_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Shed Skin Curse"),
#else
        .name = _("SHED SKIN CURSE"),
#endif
        .itemId = ITEM_SHED_SKIN_CURSE,
        .price = 0,
        .description = gItemDesc_ShedSkinCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_WILD_IV_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Strength Curse"),
#else
        .name = _("STRENGTH CURSE"),
#endif
        .itemId = ITEM_WILD_IV_CURSE,
        .price = 0,
        .description = gItemDesc_WildIVCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_CATCHING_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Catching Curse"),
#else
        .name = _("CATCHING CURSE"),
#endif
        .itemId = ITEM_CATCHING_CURSE,
        .price = 0,
        .description = gItemDesc_CatchingCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_GRACE_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Grace Curse"),
#else
        .name = _("GRACE CURSE"),
#endif
        .itemId = ITEM_GRACE_CURSE,
        .price = 0,
        .description = gItemDesc_GraceCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_WILD_ENCOUNTER_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Wild Curse"),
#else
        .name = _("WILD CURSE"),
#endif
        .itemId = ITEM_WILD_ENCOUNTER_CURSE,
        .price = 0,
        .description = gItemDesc_WildCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_PARTY_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Capacity Curse"),
#else
        .name = _("CAPACITY CURSE"),
#endif
        .itemId = ITEM_PARTY_CURSE,
        .price = 0,
        .description = gItemDesc_PartyCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_EVERSTONE_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Everstone Curse"),
#else
        .name = _("EVERSTONE CURSE"),
#endif
        .itemId = ITEM_EVERSTONE_CURSE,
        .price = 0,
        .description = gItemDesc_EverstoneCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_BATTLE_ITEM_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Embargo Curse"),
#else
        .name = _("EMBARGO CURSE"),
#endif
        .itemId = ITEM_BATTLE_ITEM_CURSE,
        .price = 0,
        .description = gItemDesc_BattleItemCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_SPECIES_CLAUSE_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Species Curse"),
#else
        .name = _("SPECIES CURSE"),
#endif
        .itemId = ITEM_SPECIES_CLAUSE_CURSE,
        .price = 0,
        .description = gItemDesc_SpeciesClauseCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_SHUFFLE_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Shuffle Curse"),
#else
        .name = _("SHUFFLE CURSE"),
#endif
        .itemId = ITEM_SHUFFLE_CURSE,
        .price = 0,
        .description = gItemDesc_ItemShuffleCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_SNOWBALL_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Snowball Curse"),
#else
        .name = _("SNOWBALL CURSE"),
#endif
        .itemId = ITEM_SNOWBALL_CURSE,
        .price = 0,
        .description = gItemDesc_SnowballCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_RANDOMAN_ROUTE_SPAWN_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Wahey Curse"),
#else
        .name = _("WAHEY CURSE"),
#endif
        .itemId = ITEM_RANDOMAN_ROUTE_SPAWN_CURSE,
        .price = 0,
        .description = gItemDesc_RandomanRouteSpawnCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_RANDOMAN_ALWAYS_SPAWN_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Wahey+ Curse"),
#else
        .name = _("WAHEY+ CURSE"),
#endif
        .itemId = ITEM_RANDOMAN_ALWAYS_SPAWN_CURSE,
        .price = 0,
        .description = gItemDesc_RandomanAlwaysSpawnCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_AUTO_MOVE_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Auto Move Curse"),
#else
        .name = _("AUTO MOVE CURSE"),
#endif
        .itemId = ITEM_AUTO_MOVE_CURSE,
        .price = 0,
        .description = gItemDesc_AutoMoveSelectCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_ONE_HIT_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("OHKO Curse"),
#else
        .name = _("OHKO CURSE"),
#endif
        .itemId = ITEM_ONE_HIT_CURSE,
        .price = 0,
        .description = gItemDesc_OneHitCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_SNAG_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Snag Curse"),
#else
        .name = _("SNAG CURSE"),
#endif
        .itemId = ITEM_SNAG_CURSE,
        .price = 0,
        .description = gItemDesc_SnagCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_WILD_EGG_SPECIES_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Baby Curse"),
#else
        .name = _("BABY CURSE"),
#endif
        .itemId = ITEM_WILD_EGG_SPECIES_CURSE,
        .price = 0,
        .description = gItemDesc_WildEggSpeciesCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_MOVE_PRIORITY_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Priority Curse"),
#else
        .name = _("PRIORITY CURSE"),
#endif
        .itemId = ITEM_MOVE_PRIORITY_CURSE,
        .price = 0,
        .description = gItemDesc_PriorityCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_ENDURE_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Endure Curse"),
#else
        .name = _("ENDURE CURSE"),
#endif
        .itemId = ITEM_ENDURE_CURSE,
        .price = 0,
        .description = gItemDesc_EndureCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_TORMENT_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Torment Curse"),
#else
        .name = _("TORMENT CURSE"),
#endif
        .itemId = ITEM_TORMENT_CURSE,
        .price = 0,
        .description = gItemDesc_TormentCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_PRESSURE_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Pressure Curse"),
#else
        .name = _("PRESSURE CURSE"),
#endif
        .itemId = ITEM_PRESSURE_CURSE,
        .price = 0,
        .description = gItemDesc_PressureCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_UNAWARE_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Unaware Curse"),
#else
        .name = _("UNAWARE CURSE"),
#endif
        .itemId = ITEM_UNAWARE_CURSE,
        .price = 0,
        .description = gItemDesc_UnawareCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_ADAPTABILITY_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("AdaptbltyCurse"),
#else
        .name = _("ADAPTBLTYCURSE"),
#endif
        .itemId = ITEM_ADAPTABILITY_CURSE,
        .price = 0,
        .description = gItemDesc_AdaptabilityCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_LEVEL_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Level Curse"),
#else
        .name = _("LEVEL CURSE"),
#endif
        .itemId = ITEM_LEVEL_CURSE,
        .price = 0,
        .description = gItemDesc_LevelCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_TINTED_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Tinted Curse"),
#else
        .name = _("TINTED CURSE"),
#endif
        .itemId = ITEM_TINTED_CURSE,
        .price = 0,
        .description = gItemDesc_TintedCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_REACH_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Reach Curse"),
#else
        .name = _("REACH CURSE"),
#endif
        .itemId = ITEM_REACH_CURSE,
        .price = 0,
        .description = gItemDesc_ReachCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_ACCURACY_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Accuracy Curse"),
#else
        .name = _("ACCURACY CURSE"),
#endif
        .itemId = ITEM_ACCURACY_CURSE,
        .price = 0,
        .description = gItemDesc_AccuracyCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_RETALIATE_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Retaliate Curse"),
#else
        .name = _("RETALIATE CURSE"),
#endif
        .itemId = ITEM_RETALIATE_CURSE,
        .price = 0,
        .description = gItemDesc_RetaliateCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_STAND_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Stand Curse"),
#else
        .name = _("STAND CURSE"),
#endif
        .itemId = ITEM_STAND_CURSE,
        .price = 0,
        .description = gItemDesc_StandCurse,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },
    
    [ITEM_TR01 - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("TR"),
        .itemId = ITEM_TR01,
        .price = 1000,
        .description = gText_DexEmptyString,
        .pocket = POCKET_TM_HM,
        .type = ITEM_USE_PARTY_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_TMHM,
        .iconImage = gItemIcon_TM,
        .iconPalette = gItemIconPalette_FightingTMHM,
    },
    [ITEM_TR50 - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("TR"),
        .itemId = ITEM_TR50,
        .price = 1000,
        .description = gText_DexEmptyString,
        .pocket = POCKET_TM_HM,
        .type = ITEM_USE_PARTY_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_TMHM,
        .iconImage = gItemIcon_TM,
        .iconPalette = gItemIconPalette_FightingTMHM,
    },

    [ITEM_CURSED_LENS - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Bane Lens"),
        .itemId = ITEM_CURSED_LENS,
        .price = 0,
        .description = gItemDesc_CursedLens,
        .holdEffect = HOLD_EFFECT_CURSED_LENS,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_ScopeLens,
        .iconPalette = gItemIconPalette_CursedLens,
    },

    [ITEM_VOW_OF_SILENCE - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Hush Band"),
        .itemId = ITEM_VOW_OF_SILENCE,
        .price = 0,
        .description = gItemDesc_VowOfSilence,
        .holdEffect = HOLD_EFFECT_VOW_OF_SILENCE,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_ChoiceBand,
        .iconPalette = gItemIconPalette_VowOfSilence,
    },

    [ITEM_BLOOD_OATH - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Fate Sash"),
        .itemId = ITEM_BLOOD_OATH,
        .price = 0,
        .description = gItemDesc_BloodOath,
        .holdEffect = HOLD_EFFECT_BLOOD_OATH,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_FocusSash,
        .iconPalette = gItemIconPalette_BloodOath,
    },

    [ITEM_HOLLOW_SUN - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Eclipse Orb"),
        .itemId = ITEM_HOLLOW_SUN,
        .price = 0,
        .description = gItemDesc_HollowSun,
        .holdEffect = HOLD_EFFECT_HOLLOW_SUN,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_LightBall,
        .iconPalette = gItemIconPalette_HollowSun,
    },

    [ITEM_MALICE_ORB - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Grudge Orb"),
        .itemId = ITEM_MALICE_ORB,
        .price = 0,
        .description = gItemDesc_MaliceOrb,
        .holdEffect = HOLD_EFFECT_MALICE_ORB,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_LifeOrb,
        .iconPalette = gItemIconPalette_MaliceOrb,
    },

    [ITEM_GRAVEGLASS - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Phantom Stone"),
        .itemId = ITEM_GRAVEGLASS,
        .price = 0,
        .description = gItemDesc_Graveglass,
        .holdEffect = HOLD_EFFECT_GRAVEGLASS,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_DimRock,
        .iconPalette = gItemIconPalette_Graveglass,
    },

    [ITEM_ASHEN_CROWN - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Fated Crown"),
        .itemId = ITEM_ASHEN_CROWN,
        .price = 0,
        .description = gItemDesc_AshenCrown,
        .holdEffect = HOLD_EFFECT_ASHEN_CROWN,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_KingsRock,
        .iconPalette = gItemIconPalette_AshenCrown,
    },

    [ITEM_WITCHS_THREAD - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Hex Thread"),
        .itemId = ITEM_WITCHS_THREAD,
        .price = 0,
        .description = gItemDesc_WitchsThread,
        .holdEffect = HOLD_EFFECT_WITCHS_THREAD,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_DestinyKnot,
        .iconPalette = gItemIconPalette_WitchsThread,
    },

    [ITEM_PETRIFIED_HEART - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Stone Heart"),
        .itemId = ITEM_PETRIFIED_HEART,
        .price = 0,
        .description = gItemDesc_PetrifiedHeart,
        .holdEffect = HOLD_EFFECT_PETRIFIED_HEART,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_HeartScale,
        .iconPalette = gItemIconPalette_PetrifiedHeart,
    },

    [ITEM_FALSE_IDOL - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Null Idol"),
        .itemId = ITEM_FALSE_IDOL,
        .price = 0,
        .description = gItemDesc_FalseIdol,
        .holdEffect = HOLD_EFFECT_FALSE_IDOL,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_OddKeystone,
        .iconPalette = gItemIconPalette_FalseIdol,
    },

    [ITEM_RUSTED_ANCHOR - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Rusted Anchor"),
        .itemId = ITEM_RUSTED_ANCHOR,
        .price = 0,
        .description = gItemDesc_RustedAnchor,
        .holdEffect = HOLD_EFFECT_RUSTED_ANCHOR,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_IronBall,
        .iconPalette = gItemIconPalette_RustedAnchor,
    },

    [ITEM_GAMBLERS_CLAW - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Gambit Claw"),
        .itemId = ITEM_GAMBLERS_CLAW,
        .price = 0,
        .description = gItemDesc_GamblersClaw,
        .holdEffect = HOLD_EFFECT_GAMBLERS_CLAW,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_QuickClaw,
        .iconPalette = gItemIconPalette_GamblersClaw,
    },

    [ITEM_TEMPO_DIAL - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Tempo Dial"),
        .itemId = ITEM_TEMPO_DIAL,
        .price = 0,
        .description = gItemDesc_TempoDial,
        .holdEffect = HOLD_EFFECT_TEMPO_DIAL,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_Metronome,
        .iconPalette = gItemIconPalette_TempoDial,
    },

    [ITEM_TURNABOUT_TOTEM - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Turnabout Totem"),
        .itemId = ITEM_TURNABOUT_TOTEM,
        .price = 0,
        .description = gItemDesc_TurnaboutTotem,
        .holdEffect = HOLD_EFFECT_TURNABOUT_TOTEM,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_StrangeSouvenir,
        .iconPalette = gItemIconPalette_TurnaboutTotem,
    },

    [ITEM_JESTER_SWITCH - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Jester Switch"),
        .itemId = ITEM_JESTER_SWITCH,
        .price = 0,
        .description = gItemDesc_JesterSwitch,
        .holdEffect = HOLD_EFFECT_JESTER_SWITCH,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_EjectButton,
        .iconPalette = gItemIconPalette_JesterSwitch,
    },

    [ITEM_WAYWARD_INCENSE - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Wayward Incense"),
        .itemId = ITEM_WAYWARD_INCENSE,
        .price = 0,
        .description = gItemDesc_WaywardIncense,
        .holdEffect = HOLD_EFFECT_WAYWARD_INCENSE,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_OddIncense,
        .iconPalette = gItemIconPalette_WaywardIncense,
    },

    [ITEM_CHAOS_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Chaos Charm"),
        .itemId = ITEM_CHAOS_CHARM,
        .price = 0,
        .description = gItemDesc_ChaosCharm,
        .holdEffect = HOLD_EFFECT_CHAOS_CHARM,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_ShinyCharm,
        .iconPalette = gItemIconPalette_ChaosCharm,
    },

    [ITEM_MISCHIEF_QUILL - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Mischief Quill"),
        .itemId = ITEM_MISCHIEF_QUILL,
        .price = 0,
        .description = gItemDesc_MischiefQuill,
        .holdEffect = HOLD_EFFECT_MISCHIEF_QUILL,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_CleverFeather,
        .iconPalette = gItemIconPalette_MischiefQuill,
    },

    [ITEM_FINALE_BELL - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Finale Bell"),
        .itemId = ITEM_FINALE_BELL,
        .price = 0,
        .description = gItemDesc_FinaleBell,
        .holdEffect = HOLD_EFFECT_FINALE_BELL,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_SootheBell,
        .iconPalette = gItemIconPalette_FinaleBell,
    },

    [ITEM_ECLIPSE_TOTEM - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Eclipse Totem"),
        .itemId = ITEM_ECLIPSE_TOTEM,
        .price = 0,
        .description = gItemDesc_EclipseTotem,
        .holdEffect = HOLD_EFFECT_ECLIPSE_TOTEM,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureEclipseTotem,
        .iconPalette = gItemIconPalette_TreasureEclipseTotem,
    },

    [ITEM_SUN_TOTEM - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Sun Totem"),
        .itemId = ITEM_SUN_TOTEM,
        .price = 0,
        .description = gItemDesc_SunTotem,
        .holdEffect = HOLD_EFFECT_SUN_TOTEM,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureSunTotem,
        .iconPalette = gItemIconPalette_TreasureSunTotem,
    },

    [ITEM_RAIN_TOTEM - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Rain Totem"),
        .itemId = ITEM_RAIN_TOTEM,
        .price = 0,
        .description = gItemDesc_RainTotem,
        .holdEffect = HOLD_EFFECT_RAIN_TOTEM,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureRainTotem,
        .iconPalette = gItemIconPalette_TreasureRainTotem,
    },

    [ITEM_SAND_TOTEM - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Sand Totem"),
        .itemId = ITEM_SAND_TOTEM,
        .price = 0,
        .description = gItemDesc_SandTotem,
        .holdEffect = HOLD_EFFECT_SAND_TOTEM,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureSandTotem,
        .iconPalette = gItemIconPalette_TreasureSandTotem,
    },

    [ITEM_SNOW_TOTEM - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Snow Totem"),
        .itemId = ITEM_SNOW_TOTEM,
        .price = 0,
        .description = gItemDesc_SnowTotem,
        .holdEffect = HOLD_EFFECT_SNOW_TOTEM,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureHailTotem,
        .iconPalette = gItemIconPalette_TreasureHailTotem,
    },

    [ITEM_ACID_RAIN_TOTEM - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Acid Rain Totem"),
        .itemId = ITEM_ACID_RAIN_TOTEM,
        .price = 0,
        .description = gItemDesc_AcidRainTotem,
        .holdEffect = HOLD_EFFECT_ACID_RAIN_TOTEM,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureAcidRainTotem,
        .iconPalette = gItemIconPalette_TreasureAcidRainTotem,
    },

    [ITEM_ELECTRIC_TIKI - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Electric Tiki"),
        .itemId = ITEM_ELECTRIC_TIKI,
        .price = 0,
        .description = gItemDesc_ElectricTiki,
        .holdEffect = HOLD_EFFECT_ELECTRIC_TIKI,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureElectricTiki,
        .iconPalette = gItemIconPalette_TreasureElectricTiki,
    },

    [ITEM_GRASSY_TIKI - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Grassy Tiki"),
        .itemId = ITEM_GRASSY_TIKI,
        .price = 0,
        .description = gItemDesc_GrassyTiki,
        .holdEffect = HOLD_EFFECT_GRASSY_TIKI,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureGrassyTiki,
        .iconPalette = gItemIconPalette_TreasureGrassyTiki,
    },

    [ITEM_MISTY_TIKI - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Misty Tiki"),
        .itemId = ITEM_MISTY_TIKI,
        .price = 0,
        .description = gItemDesc_MistyTiki,
        .holdEffect = HOLD_EFFECT_MISTY_TIKI,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureMistyTiki,
        .iconPalette = gItemIconPalette_TreasureMistyTiki,
    },

    [ITEM_PSYCHIC_TIKI - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Psychic Tiki"),
        .itemId = ITEM_PSYCHIC_TIKI,
        .price = 0,
        .description = gItemDesc_PsychicTiki,
        .holdEffect = HOLD_EFFECT_PSYCHIC_TIKI,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasurePsychicTiki,
        .iconPalette = gItemIconPalette_TreasurePsychicTiki,
    },

    [ITEM_GLASS_SWORD - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Glass Sword"),
        .itemId = ITEM_GLASS_SWORD,
        .price = 0,
        .description = gItemDesc_GlassSword,
        .holdEffect = HOLD_EFFECT_GLASS_SWORD,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureGlassCannon,
        .iconPalette = gItemIconPalette_TreasureGlassCannon,
    },

    [ITEM_WONDER_SHIELD - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Wonder Shield"),
        .itemId = ITEM_WONDER_SHIELD,
        .price = 0,
        .description = gItemDesc_WonderShield,
        .holdEffect = HOLD_EFFECT_WONDER_SHIELD,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureWonderShield,
        .iconPalette = gItemIconPalette_TreasureWonderShield,
    },

    [ITEM_ECHO_SCEPTER - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Echo Scepter"),
        .itemId = ITEM_ECHO_SCEPTER,
        .price = 0,
        .description = gItemDesc_EchoScepter,
        .holdEffect = HOLD_EFFECT_ECHO_SCEPTER,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureEchoScepter,
        .iconPalette = gItemIconPalette_TreasureEchoScepter,
    },

    [ITEM_GLYPH_CODEX - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Glyph Codex"),
        .itemId = ITEM_GLYPH_CODEX,
        .price = 0,
        .description = gItemDesc_GlyphCodex,
        .holdEffect = HOLD_EFFECT_GLYPH_CODEX,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureGlyphCodex,
        .iconPalette = gItemIconPalette_TreasureGlyphCodex,
    },

    [ITEM_PRECISE_LENS - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Precise Lens"),
        .itemId = ITEM_PRECISE_LENS,
        .price = 0,
        .description = gItemDesc_PreciseLens,
        .holdEffect = HOLD_EFFECT_PRECISE_LENS,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureFocusLens,
        .iconPalette = gItemIconPalette_TreasureFocusLens,
    },

    [ITEM_FURY_MANTLE - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Fury Mantle"),
        .itemId = ITEM_FURY_MANTLE,
        .price = 0,
        .description = gItemDesc_FuryMantle,
        .holdEffect = HOLD_EFFECT_FURY_MANTLE,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureFinalStand,
        .iconPalette = gItemIconPalette_TreasureFinalStand,
    },

    [ITEM_COMPOUND_GOGGLES - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Twin Goggles"),
        .itemId = ITEM_COMPOUND_GOGGLES,
        .price = 0,
        .description = gItemDesc_CompoundGoggles,
        .holdEffect = HOLD_EFFECT_COMPOUND_GOGGLES,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureCompoundLens,
        .iconPalette = gItemIconPalette_TreasureCompoundLens,
    },

    [ITEM_GREEDY_GLOVES - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Greedy Gloves"),
        .itemId = ITEM_GREEDY_GLOVES,
        .price = 0,
        .description = gItemDesc_GreedyGloves,
        .holdEffect = HOLD_EFFECT_GREEDY_GLOVES,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureGreedyGlove,
        .iconPalette = gItemIconPalette_TreasureGreedyGlove,
    },

    [ITEM_IMPACT_PLATING - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Impact Plating"),
        .itemId = ITEM_IMPACT_PLATING,
        .price = 0,
        .description = gItemDesc_ImpactPlating,
        .holdEffect = HOLD_EFFECT_IMPACT_PLATING,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureBufferShield,
        .iconPalette = gItemIconPalette_TreasureBufferShield,
    },

    [ITEM_CHIME_JEWEL - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Chime Jewel"),
        .itemId = ITEM_CHIME_JEWEL,
        .price = 0,
        .description = gItemDesc_ChimeJewel,
        .holdEffect = HOLD_EFFECT_CHIME_JEWEL,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureBellJewel,
        .iconPalette = gItemIconPalette_TreasureBellJewel,
    },

    [ITEM_AMBUSH_TALON - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Ambush Talon"),
        .itemId = ITEM_AMBUSH_TALON,
        .price = 0,
        .description = gItemDesc_AmbushTalon,
        .holdEffect = HOLD_EFFECT_AMBUSH_TALON,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureQuickTalon,
        .iconPalette = gItemIconPalette_TreasureQuickTalon,
    },

    [ITEM_PURITY_JEWEL - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Purity Jewel"),
        .itemId = ITEM_PURITY_JEWEL,
        .price = 0,
        .description = gItemDesc_PurityJewel,
        .holdEffect = HOLD_EFFECT_PURITY_JEWEL,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureSpecialistJewel,
        .iconPalette = gItemIconPalette_TreasureSpecialistJewel,
    },

    [ITEM_HEXING_WAND - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Hexing Wand"),
        .itemId = ITEM_HEXING_WAND,
        .price = 0,
        .description = gItemDesc_HexingWand,
        .holdEffect = HOLD_EFFECT_HEXING_WAND,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureHexingWand,
        .iconPalette = gItemIconPalette_TreasureHexingWand,
    },

    [ITEM_FICKLE_HAT - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Fickle Hat"),
        .itemId = ITEM_FICKLE_HAT,
        .price = 0,
        .description = gItemDesc_FickleHat,
        .holdEffect = HOLD_EFFECT_FICKLE_HAT,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureMoodyHat,
        .iconPalette = gItemIconPalette_TreasureMoodyHat,
    },

    [ITEM_GOLDEN_EGG - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Golden Egg"),
        .itemId = ITEM_GOLDEN_EGG,
        .price = 0,
        .description = gItemDesc_GoldenEgg,
        .holdEffect = HOLD_EFFECT_GOLDEN_EGG,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureGoldenEgg,
        .iconPalette = gItemIconPalette_TreasureGoldenEgg,
    },

    [ITEM_BRIAR_BRACER - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Briar Bracer"),
        .itemId = ITEM_BRIAR_BRACER,
        .price = 0,
        .description = gItemDesc_BriarBracer,
        .holdEffect = HOLD_EFFECT_BRIAR_BRACER,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureThornmail,
        .iconPalette = gItemIconPalette_TreasureThornmail,
    },

    [ITEM_TRICKY_BOX - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Tricky Box"),
        .itemId = ITEM_TRICKY_BOX,
        .price = 0,
        .description = gItemDesc_TrickyBox,
        .holdEffect = HOLD_EFFECT_TRICKY_BOX,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureTrickyBox,
        .iconPalette = gItemIconPalette_TreasureTrickyBox,
    },

    [ITEM_ADAPTIVE_SPECS - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Adaptive Specs"),
        .itemId = ITEM_ADAPTIVE_SPECS,
        .price = 0,
        .description = gItemDesc_AdaptiveSpecs,
        .holdEffect = HOLD_EFFECT_ADAPTIVE_SPECS,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureChampionsSpecs,
        .iconPalette = gItemIconPalette_TreasureChampionsSpecs,
    },

    [ITEM_RAINCOAT - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Raincoat"),
        .itemId = ITEM_RAINCOAT,
        .price = 0,
        .description = gItemDesc_Raincoat,
        .holdEffect = HOLD_EFFECT_RAINCOAT,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureRaincoat,
        .iconPalette = gItemIconPalette_TreasureRaincoat,
    },

    [ITEM_MONOTYPE_SERUM - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Monotype Serum"),
        .itemId = ITEM_MONOTYPE_SERUM,
        .price = 0,
        .description = gItemDesc_MonotypeSerum,
        .holdEffect = HOLD_EFFECT_MONOTYPE_SERUM,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureMonotype,
        .iconPalette = gItemIconPalette_TreasureMonotype,
    },

    [ITEM_DECOY_DOLL - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Decoy Doll"),
        .itemId = ITEM_DECOY_DOLL,
        .price = 0,
        .description = gItemDesc_DecoyDoll,
        .holdEffect = HOLD_EFFECT_DECOY_DOLL,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureDecoyDoll,
        .iconPalette = gItemIconPalette_TreasureDecoyDoll,
    },

    [ITEM_WOODEN_SWORD - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("Wooden Sword"),
        .itemId = ITEM_WOODEN_SWORD,
        .price = 0,
        .description = gItemDesc_WoodenSword,
        .holdEffect = HOLD_EFFECT_WOODEN_SWORD,
        .pocket = POCKET_HELD_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
        .iconImage = gItemIcon_TreasureWoodCannon,
        .iconPalette = gItemIconPalette_TreasureWoodCannon,
    },

#ifdef ROGUE_DEBUG
    [ITEM_DEBUG_MINT_HARDY - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("DEBUG H MINT"),
        .itemId = ITEM_DEBUG_MINT_HARDY,
        .price = 0,
        .description = gText_DexEmptyString,
        .pocket = POCKET_ITEMS,
        .type = ITEM_USE_PARTY_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_NatureMint,
        .iconImage = gItemIcon_EnigmaBerry,
        .iconPalette = gItemIconPalette_EnigmaBerry,
    },
    [ITEM_DEBUG_MINT_ADAMANT - ITEM_ROGUE_ITEM_FIRST] =
    {
        .name = _("DEBUG A MINT"),
        .itemId = ITEM_DEBUG_MINT_ADAMANT,
        .price = 0,
        .description = gText_DexEmptyString,
        .pocket = POCKET_ITEMS,
        .type = ITEM_USE_PARTY_MENU,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_NatureMint,
        .iconImage = gItemIcon_EnigmaBerry,
        .iconPalette = gItemIconPalette_EnigmaBerry,
    },
#endif
};
