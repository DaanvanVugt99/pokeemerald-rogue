#include "global.h"
#include "constants/abilities.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/region_map_sections.h"
#include "constants/rgb.h"
#include "event_data.h"
#include "random.h"

#include "rogue_baked.h"
#include "rogue_colour_utils.h"
#include "rogue_gifts.h"
#include "rogue_hub.h"
#include "rogue_pokedex.h"
#include "rogue_query.h"
#include "rogue_quest.h"
#include "rogue_save.h"

struct CustomTrainerData
{
    u8 const* name;
    u32 trainerId;
    u8 trainerColour;
};

struct CustomMonData
{
    u8 const* nickname;
    u16 const* moves;
    u16 const* abilities;
    u32 otId;
    u16 movesCount;
    u16 customTrainerId;
    u16 species;
    u16 pokeball;
    u16 heldItem;
    u16 isShiny : 1;
};

static u8 const sRarityToCustomTrainerIndex[] = 
{
    [UNIQUE_RARITY_COMMON]      = CUSTOM_TRAINER_COMMON,
    [UNIQUE_RARITY_RARE]        = CUSTOM_TRAINER_RARE,
    [UNIQUE_RARITY_EPIC]        = CUSTOM_TRAINER_EPIC,
    [UNIQUE_RARITY_EXOTIC]      = CUSTOM_TRAINER_EXOTIC,
    [UNIQUE_RARITY_LEGENDARY]   = CUSTOM_TRAINER_LEGEND,
};

static u16 const sDynamicCustomMonAbilities[] = 
{
#ifdef ROGUE_EXPANSION
    ABILITY_GOOD_AS_GOLD,
    ABILITY_WATER_ABSORB,
    ABILITY_VOLT_ABSORB,
    ABILITY_FLASH_FIRE,
    ABILITY_SPEED_BOOST,
    ABILITY_ADAPTABILITY,
    ABILITY_INTIMIDATE,
    ABILITY_DRIZZLE,
    ABILITY_DROUGHT,
    ABILITY_SNOW_WARNING,
    ABILITY_SAND_STREAM,
    ABILITY_QUICK_DRAW,
    ABILITY_NEUTRALIZING_GAS,
    ABILITY_ELECTRIC_SURGE,
    ABILITY_PSYCHIC_SURGE,
    ABILITY_MISTY_SURGE,
    ABILITY_GRASSY_SURGE,
    ABILITY_PROTEAN,
    ABILITY_MAGIC_BOUNCE,
    ABILITY_MOXIE,
    ABILITY_ILLUSION,
    ABILITY_REGENERATOR,
    ABILITY_SUPER_LUCK,
    ABILITY_CONTRARY,
    ABILITY_NO_GUARD,
    ABILITY_SKILL_LINK,
    ABILITY_SHELL_ARMOR,
    ABILITY_GUTS,
    ABILITY_THICK_FAT,
    ABILITY_HUGE_POWER,
    ABILITY_SERENE_GRACE,
    ABILITY_BATTLE_ARMOR,
    ABILITY_WEAK_ARMOR,
    ABILITY_TOXIC_DEBRIS,
    ABILITY_UNSEEN_FIST,
    ABILITY_GORILLA_TACTICS,
    ABILITY_TOUGH_CLAWS,
    ABILITY_FULL_METAL_BODY,
    ABILITY_TECHNICIAN,
    ABILITY_EMERGENCY_EXIT,
    ABILITY_PRANKSTER,
    ABILITY_POWER_OF_ALCHEMY,
    ABILITY_OPPORTUNIST,
    ABILITY_LEVITATE,
    ABILITY_MULTISCALE,
    ABILITY_COSTAR,
    ABILITY_POISON_HEAL,
    ABILITY_NEUROFORCE,
    ABILITY_MOODY,
    ABILITY_HOSPITALITY,
    ABILITY_COMATOSE,
    ABILITY_BERSERK,
    ABILITY_SIMPLE,
    ABILITY_UNAWARE,
    ABILITY_HARVEST,
    ABILITY_MOLD_BREAKER,
    ABILITY_SHEER_FORCE,
    ABILITY_OVERCOAT,
    ABILITY_IRON_BARBS,
    ABILITY_DEFIANT,
    ABILITY_STALL,
    ABILITY_SNIPER,
    ABILITY_MULTITYPE,
    ABILITY_SHARPNESS,
    ABILITY_PURIFYING_SALT,
    ABILITY_EARTH_EATER,
    ABILITY_WIND_RIDER,
    ABILITY_WELL_BAKED_BODY,
    ABILITY_THERMAL_EXCHANGE,
    ABILITY_ROCKY_PAYLOAD,
    ABILITY_DRAGONS_MAW,
    ABILITY_TRANSISTOR,
    ABILITY_PUNK_ROCK,
    ABILITY_MEGA_LAUNCHER,
    ABILITY_STEELWORKER,
    ABILITY_ARMOR_TAIL,
    ABILITY_QUEENLY_MAJESTY,
    ABILITY_GUARD_DOG,
    ABILITY_MAGIC_GUARD,
    ABILITY_TINTED_LENS,
    ABILITY_STRONG_JAW,
    ABILITY_IRON_FIST,
    ABILITY_RECKLESS,
    ABILITY_ANALYTIC,
    ABILITY_DOWNLOAD,
    ABILITY_SCRAPPY,
    ABILITY_FUR_COAT,
    ABILITY_ICE_SCALES,
    ABILITY_MIRROR_ARMOR,
    ABILITY_STORM_DRAIN,
    ABILITY_LIGHTNING_ROD,
    ABILITY_SAP_SIPPER,
    ABILITY_PIXILATE,
    ABILITY_AERILATE,
    ABILITY_REFRIGERATE,
    ABILITY_GALVANIZE,
    ABILITY_LIQUID_VOICE,
    ABILITY_DAUNTLESS_SHIELD,
    ABILITY_INTREPID_SWORD,
    ABILITY_CHILLING_NEIGH,
    ABILITY_GRIM_NEIGH,
    ABILITY_SUPREME_OVERLORD
#else
    ABILITY_DRIZZLE,
    ABILITY_SPEED_BOOST,
    ABILITY_BATTLE_ARMOR,
    ABILITY_LIMBER,
    ABILITY_SAND_VEIL,
    ABILITY_STATIC,
    ABILITY_VOLT_ABSORB,
    ABILITY_WATER_ABSORB,
    ABILITY_CLOUD_NINE,
    ABILITY_COMPOUND_EYES,
    ABILITY_COLOR_CHANGE,
    ABILITY_FLASH_FIRE,
    ABILITY_SHIELD_DUST,
    ABILITY_SUCTION_CUPS,
    ABILITY_INTIMIDATE,
    ABILITY_SHADOW_TAG,
    ABILITY_ROUGH_SKIN,
    ABILITY_LEVITATE,
    ABILITY_EFFECT_SPORE,
    ABILITY_CLEAR_BODY,
    ABILITY_SERENE_GRACE,
    ABILITY_SWIFT_SWIM,
    ABILITY_CHLOROPHYLL,
    ABILITY_HUGE_POWER,
    ABILITY_INNER_FOCUS,
    ABILITY_SOUNDPROOF,
    ABILITY_RAIN_DISH,
    ABILITY_SAND_STREAM,
    ABILITY_PRESSURE,
    ABILITY_THICK_FAT,
    ABILITY_FLAME_BODY,
    ABILITY_RUN_AWAY,
    ABILITY_PICKUP,
    ABILITY_HUSTLE,
    ABILITY_CUTE_CHARM,
    ABILITY_SHED_SKIN,
    ABILITY_GUTS,
    ABILITY_MARVEL_SCALE,
    ABILITY_ROCK_HEAD,
    ABILITY_DROUGHT,
    ABILITY_WHITE_SMOKE,
    ABILITY_PURE_POWER,
    ABILITY_SHELL_ARMOR,
    ABILITY_AIR_LOCK,
#endif
};

static u16 const sDynamicCustomMonUniqueAbilities[] =
{
    ABILITY_IMPACT,
    ABILITY_SILVER_LINING,
    ABILITY_THICK_SKULL,
    ABILITY_VAMPIRIC,
    ABILITY_STATIC_STASH,
    ABILITY_BRUTAL_CHARGE,
    ABILITY_SOLARBOOST,
    ABILITY_DAMPENING,
    ABILITY_PASTURIZED,
    ABILITY_LIGHTNING_FIELD,
    ABILITY_MISTY_MIRAGE,
    ABILITY_SECOND_WIND,
    ABILITY_BARNACLE_WALL,
    ABILITY_GRAVE_GROVE,
    ABILITY_LIFE_BLOSSOM,
    ABILITY_NIGHT_HUNTER,
    ABILITY_BEGUILE,
    ABILITY_FLOWER_CHAIN,
    ABILITY_BUBBLE_NET,
    ABILITY_ADAPTIVE_PLATING,
    ABILITY_BEAR_HUG,
    ABILITY_BRANCH_SWING,
    ABILITY_AFTERIMAGE,
    ABILITY_FORCEFIELD,
    ABILITY_SPOREFIELD,
    ABILITY_SNAREWIRE,
    ABILITY_DYNAMO,
    ABILITY_BATTLECRY,
    ABILITY_TAKE_AIM,
    ABILITY_DEADLY_SHOT,
    ABILITY_ROSE_GARDEN,
    ABILITY_FEEDING_FRENZY,
    ABILITY_VOLCANIC_RAGE,
    ABILITY_SMOLDERING_SHELL,
    ABILITY_WRECKAGE,
    ABILITY_DYNAMO_FISTS,
    ABILITY_BATTLE_TRANCE,
    ABILITY_BLOODHOUND,
    ABILITY_BRAMBLE_GUARD,
    ABILITY_BURNING_HEART,
    ABILITY_BURROW,
    ABILITY_CARNIVOROUS,
    ABILITY_COLD_READ,
    ABILITY_CORROSIVE_AMP,
    ABILITY_COUNTERPUNCH,
    ABILITY_CRACKED_SHELL,
    ABILITY_DEATHRATTLE,
    ABILITY_DEATH_ROLL,
    ABILITY_DEMOLITION,
    ABILITY_DESERT_SHROUD,
    ABILITY_DISGUISED,
    ABILITY_DIVINE_FAVOR,
    ABILITY_DREAD,
    ABILITY_DUELIST,
    ABILITY_AERODYNAMIC,
    ABILITY_AFTERSHOCK,
    ABILITY_AQUATIC_ARMOR,
    ABILITY_BASALT_SHELL,
    ABILITY_BEACON,
    ABILITY_BITING_COLD,
    ABILITY_BODY_OF_WATER,
    ABILITY_CLAIRVOYANT,
    ABILITY_CLUELESS,
    ABILITY_FORTIFIED_SPIN,
    ABILITY_FRIGHTMARE,
    ABILITY_FUNGAL_INFECTION,
    ABILITY_GLACIAL_MASS,
    ABILITY_GNAW_DOWN,
    ABILITY_HANDYWORK,
    ABILITY_HEADACHE,
    ABILITY_HEXCRAFT,
    ABILITY_HOT_PURSUIT,
    ABILITY_IMMOLATE,
    ABILITY_INTENSIVE_CARE,
    ABILITY_IONIZE,
    ABILITY_IRON_WILL,
    ABILITY_KNIGHTLY,
    ABILITY_LIVING_ROOTS,
    ABILITY_MARKSMAN,
    ABILITY_MATERNAL_INSTINCT,
    ABILITY_MISCHIEF,
    ABILITY_MOONLIGHT,
    ABILITY_NEUROTOXIN,
    ABILITY_OMNISENSE,
    ABILITY_OPEN_FIELD,
    ABILITY_PERMAFROST,
    ABILITY_PIERCING_JUDGMENT,
    ABILITY_PLAYMAKER,
    ABILITY_POLLEN_PUFF,
    ABILITY_PRECOGNITION,
    ABILITY_PRICKLY,
    ABILITY_RAILGUN_CHARGE,
    ABILITY_REDLINE,
    ABILITY_REGAL_DECREE,
    ABILITY_RESONANCE,
    ABILITY_RINGLEADER,
    ABILITY_ROCKET_FIST,
    ABILITY_ROLLING_START,
    ABILITY_ROYAL_CHARM,
    ABILITY_ROYAL_STORM,
    ABILITY_SANDMAN,
    ABILITY_SAND_SKIMMER,
    ABILITY_STARMOBILE,
};

static u16 const sDynamicCustomMonMoves[] = 
{
#ifdef ROGUE_EXPANSION
    MOVE_HYDRO_STEAM,
    MOVE_CHILLING_WATER,
    MOVE_TORCH_SONG,
    MOVE_JET_PUNCH,
    MOVE_STONE_AXE,
    MOVE_THUNDEROUS_KICK,
    MOVE_BOOMBURST,
    MOVE_SURGING_STRIKES,
    MOVE_PARTING_SHOT,
    MOVE_GRASSY_GLIDE,
    MOVE_DYNAMAX_CANNON,
    MOVE_STOMPING_TANTRUM,
    MOVE_BANEFUL_BUNKER,
    MOVE_DIAMOND_STORM,
    MOVE_PLAY_ROUGH,
    MOVE_FREEZE_DRY,
    MOVE_PHANTOM_FORCE,
    MOVE_V_CREATE,
    MOVE_SECRET_SWORD,
    MOVE_FOCUS_BLAST,
    MOVE_SPORE,
    MOVE_GUNK_SHOT,
    MOVE_BRAVE_BIRD,
    MOVE_CLOSE_COMBAT,
    MOVE_HURRICANE,
    MOVE_SLUDGE_BOMB,
    MOVE_HEADLONG_RUSH,
    MOVE_LEAF_BLADE,
    MOVE_METEOR_MASH,
    MOVE_EARTH_POWER,
    MOVE_SKILL_SWAP,
    MOVE_FAKE_OUT,
    MOVE_EXTREME_SPEED,
    MOVE_ROCK_SLIDE,
    MOVE_RAPID_SPIN,
    MOVE_MEGAHORN,
    MOVE_WISH,
    MOVE_THUNDER,
    MOVE_POWER_GEM,
    MOVE_THUNDERCLAP,
    MOVE_GIGATON_HAMMER,
    MOVE_SALT_CURE,
    MOVE_WICKED_BLOW,
    MOVE_POLTERGEIST,
    MOVE_METEOR_BEAM,
    MOVE_SPIRIT_BREAK,
    MOVE_RECOVER,
    MOVE_NO_RETREAT,
    MOVE_LUNGE,
    MOVE_METRONOME,
    MOVE_TOPSY_TURVY,
    MOVE_QUIVER_DANCE,
    MOVE_BATON_PASS,
    MOVE_MOONGEIST_BEAM,
    MOVE_MAGMA_STORM,
    MOVE_RUINATION,
    MOVE_REVELATION_DANCE,
    MOVE_SUCKER_PUNCH,
    MOVE_COLLISION_COURSE,
    MOVE_ROLLOUT,
    MOVE_FURY_CUTTER,
    MOVE_LAST_RESPECTS,
    MOVE_ACROBATICS,
    MOVE_BUG_BUZZ,
    MOVE_POWER_TRIP,
    MOVE_WEATHER_BALL,
    MOVE_BODY_PRESS,
    MOVE_POWER_UP_PUNCH,
    MOVE_FIRE_LASH,
    MOVE_MYSTICAL_FIRE,
    MOVE_FLAME_CHARGE,
    MOVE_SHADOW_BALL,
    MOVE_BRINE,
    MOVE_IRON_HEAD,
    MOVE_TRAILBLAZE,
    MOVE_POLLEN_PUFF,
    MOVE_NUZZLE,
    MOVE_FLASH_CANNON,
    MOVE_PSYSHOCK,
    MOVE_STORED_POWER,
    MOVE_ICY_WIND,
    MOVE_AVALANCHE,
    MOVE_OVERHEAT,
    MOVE_SCALE_SHOT,
    MOVE_SNARL,
    MOVE_LIQUIDATION,
    MOVE_MUDDY_WATER,
    MOVE_ACID_SPRAY,
    MOVE_CLEAR_SMOG,
    MOVE_BULLDOZE,
    MOVE_ROCK_TOMB,
    MOVE_POUNCE,
    MOVE_GIGA_DRAIN,
    MOVE_SUPERCELL_SLAM,
    MOVE_HEAVY_SLAM,
    MOVE_ELECTROWEB,
    MOVE_ENCORE,
    MOVE_ZEN_HEADBUTT,
    MOVE_DISABLE,
    MOVE_TORMENT,
    MOVE_TRIPLE_AXEL,
    MOVE_TOXIC_SPIKES,
    MOVE_DRAGON_RUSH,
    MOVE_DRACO_METEOR,
    MOVE_NIGHT_SLASH,
    MOVE_GRAVITY,
    MOVE_DARK_PULSE,
    MOVE_PAIN_SPLIT,
    MOVE_MOONBLAST,
    MOVE_MAGIC_COAT,
    MOVE_BURNING_JEALOUSY,
    MOVE_ALLURING_VOICE,
    MOVE_PSYCHIC_NOISE,
    MOVE_TEMPER_FLARE,
    MOVE_STRENGTH_SAP,
    MOVE_YAWN,
    MOVE_PERISH_SONG,
    MOVE_COURT_CHANGE,
    MOVE_METAL_BURST,
    MOVE_INFESTATION,
    MOVE_ZAP_CANNON,
    MOVE_ERUPTION,
    MOVE_SIGNAL_BEAM,
    MOVE_HORN_LEECH,
    MOVE_VACUUM_WAVE,
    MOVE_MIRROR_COAT,
    MOVE_BELLY_DRUM,
    MOVE_SCORCHING_SANDS,
    MOVE_TRICK_ROOM,
    MOVE_NASTY_PLOT,
    MOVE_U_TURN,
    MOVE_DRAGON_DANCE,
    MOVE_CALM_MIND,
    MOVE_KNOCK_OFF,
    MOVE_ANCIENT_POWER,
    MOVE_SWORDS_DANCE,
    MOVE_SUBSTITUTE,
    MOVE_BODY_SLAM,
    MOVE_FLIP_TURN,
    MOVE_APPLE_ACID,
    MOVE_VOLT_SWITCH,
    MOVE_BREAKING_SWIPE,
    MOVE_FOUL_PLAY,
    MOVE_DRAINING_KISS,
    MOVE_HEX,
    MOVE_GYRO_BALL,
    MOVE_TAUNT,
    MOVE_LEECH_SEED,
    MOVE_STICKY_WEB,
    MOVE_STEALTH_ROCK,
    MOVE_SPIKES,
    MOVE_HAZE,
    MOVE_DESTINY_BOND,
    MOVE_FELL_STINGER,
#else
    MOVE_PAY_DAY,
    MOVE_FIRE_PUNCH,
    MOVE_ICE_PUNCH,
    MOVE_THUNDER_PUNCH,
    MOVE_SWORDS_DANCE,
    MOVE_FLY,
    MOVE_BODY_SLAM,
    MOVE_SURF,
    MOVE_ICE_BEAM,
    MOVE_COUNTER,
    MOVE_DRAGON_RAGE,
    MOVE_EARTHQUAKE,
    MOVE_PSYCHIC,
    MOVE_METRONOME,
    MOVE_MIRROR_MOVE,
    MOVE_FIRE_BLAST,
    MOVE_SOFT_BOILED,
    MOVE_TRANSFORM,
    MOVE_SUBSTITUTE,
    MOVE_CURSE,
    MOVE_AEROBLAST,
    MOVE_MACH_PUNCH,
    MOVE_BELLY_DRUM,
    MOVE_ZAP_CANNON,
    MOVE_OUTRAGE,
    MOVE_GIGA_DRAIN,
    MOVE_ROLLOUT,
    MOVE_SACRED_FIRE,
    MOVE_MEGAHORN,
    MOVE_SYNTHESIS,
    MOVE_CRUNCH,
    MOVE_EXTREME_SPEED,
    MOVE_ANCIENT_POWER,
    MOVE_SHADOW_BALL,
    MOVE_FUTURE_SIGHT,
    MOVE_FAKE_OUT,
    MOVE_UPROAR,
    MOVE_WILL_O_WISP,
    MOVE_WISH,
    MOVE_ASSIST,
    MOVE_SUPERPOWER,
    MOVE_MAGIC_COAT,
    MOVE_BRICK_BREAK,
    MOVE_KNOCK_OFF,
    MOVE_SKILL_SWAP,
    MOVE_LUSTER_PURGE,
    MOVE_HYPER_VOICE,
    MOVE_BLAST_BURN,
    MOVE_HYDRO_CANNON,
    MOVE_OVERHEAT,
    MOVE_SIGNAL_BEAM,
    MOVE_AERIAL_ACE,
    MOVE_ICICLE_SPEAR,
    MOVE_FRENZY_PLANT,
    MOVE_BULK_UP,
    MOVE_VOLT_TACKLE,
    MOVE_CALM_MIND,
    MOVE_DRAGON_DANCE,
    MOVE_PSYCHO_BOOST,
    MOVE_ROCK_BLAST,
    MOVE_ERUPTION,
    MOVE_PERISH_SONG,
    MOVE_PAIN_SPLIT,
#endif
};

STATIC_ASSERT(ARRAY_COUNT(sDynamicCustomMonAbilities) <= 127, SizeOfDynamicCustomMonAbilities);
#define DYNAMIC_MOVE_POOL_CAPACITY 160
#define DYNAMIC_MOVE_SELECTION_CAPACITY (1 << 14)
#define DYNAMIC_MOVE_PAIR_COUNT ((DYNAMIC_MOVE_POOL_CAPACITY * (DYNAMIC_MOVE_POOL_CAPACITY - 1)) / 2)
#define DYNAMIC_MOVE_PAIR_CODE_START (DYNAMIC_MOVE_POOL_CAPACITY + 1)
#define DYNAMIC_TYPED_MOVE_SELECTION_STRIDE (DYNAMIC_MOVE_POOL_CAPACITY + 1)

STATIC_ASSERT(ARRAY_COUNT(sDynamicCustomMonMoves) <= DYNAMIC_MOVE_POOL_CAPACITY, SizeOfDynamicCustomMonMoves);
STATIC_ASSERT(DYNAMIC_MOVE_PAIR_CODE_START + DYNAMIC_MOVE_PAIR_COUNT <= DYNAMIC_MOVE_SELECTION_CAPACITY, DynamicMoveSelectionFits14Bits);
STATIC_ASSERT(NUMBER_OF_MON_TYPES * 4 * DYNAMIC_TYPED_MOVE_SELECTION_STRIDE <= DYNAMIC_MOVE_SELECTION_CAPACITY, TypedUniqueMoveSelectionFits14Bits);
STATIC_ASSERT(ARRAY_COUNT(sDynamicCustomMonUniqueAbilities) <= 127, SizeOfDynamicCustomMonUniqueAbilities);

const u16 sTypeTintColors[NUMBER_OF_MON_TYPES] =
{
    [TYPE_NORMAL] = RGB_RANGE_255_TO_31(255, 236, 206),
    [TYPE_FIGHTING] = RGB_RANGE_255_TO_31(179, 39, 27),
    [TYPE_FLYING] = RGB_RANGE_255_TO_31(154, 217, 232),
    [TYPE_POISON] = RGB_RANGE_255_TO_31(254, 20, 255),
    [TYPE_GROUND] = RGB_RANGE_255_TO_31(142, 102, 59),
    [TYPE_ROCK] = RGB_RANGE_255_TO_31(202, 162, 86),
    [TYPE_BUG] = RGB_RANGE_255_TO_31(188, 202, 121),
    [TYPE_GHOST] = RGB_WHITE,
    [TYPE_STEEL] = RGB_RANGE_255_TO_31(142, 142, 142),
    [TYPE_MYSTERY] = RGB_WHITE,
    [TYPE_FIRE] = RGB_RANGE_255_TO_31(255, 170, 71),
    [TYPE_WATER] = RGB_RANGE_255_TO_31(37, 178, 255),
    [TYPE_GRASS] = RGB_RANGE_255_TO_31(33, 255, 32),
    [TYPE_ELECTRIC] = RGB_RANGE_255_TO_31(255, 227, 20),
    [TYPE_PSYCHIC] = RGB_RANGE_255_TO_31(254, 80, 255),
    [TYPE_ICE] = RGB_RANGE_255_TO_31(163, 255, 253),
    [TYPE_DRAGON] = RGB_RANGE_255_TO_31(43, 40, 255),
    [TYPE_DARK] = RGB_RANGE_255_TO_31(91, 91, 91),
#ifdef ROGUE_EXPANSION
    [TYPE_FAIRY] = RGB_RANGE_255_TO_31(255, 192, 234),
    [TYPE_STELLAR] = RGB(10, 18, 27),
#endif
};

#include "data/rogue/custom_mons.h"

enum
{
    COMPRESSED_FORMAT_ORIGINAL = 0,
    COMPRESSED_FORMAT_MON_TYPE,
    COMPRESSED_FORMAT_ORIGINAL_UNIQUE_ABILITY,
    COMPRESSED_FORMAT_MON_TYPE_UNIQUE_ABILITY,
};

struct CompressedDynamicData
{
    u32 data1:21;
    u32 format:2;
    u32 data2:7;
    u32 reserved:2; // reserved for bitmask OTID_FLAG_CUSTOM_MON etc.
};

struct CompressedDynamicData_Original
{
    u32 moveSelection:14;
    u32 unused:7;
    u32 format:2;
    u32 ability:7; // 127 indices
    u32 reserved:2;
};

struct CompressedDynamicData_MonType
{
    u32 type:5;
    u32 typeSlot:1;
    u32 typeMoveFlip:1;
    u32 moveSelection:14;
    u32 format:2;
    u32 ability:7; // 127 indices
    u32 reserved:2;
};

struct CompressedDynamicData_OriginalUniqueAbility
{
    u32 moveSelection:14;
    u32 uniqueAbility:7; // 127 indices
    u32 format:2;
    u32 ability:7; // 127 indices
    u32 reserved:2;
};

struct CompressedDynamicData_MonTypeUniqueAbility
{
    u32 typeMoveSelection:14;
    u32 uniqueAbility:7; // 127 indices
    u32 format:2;
    u32 ability:7; // 127 indices
    u32 reserved:2;
};

struct DynamicMonData
{
    u16 moves[MAX_MON_MOVES];
    u16 movesCount;
    u8 types[2];
    u16 ability;
    u16 uniqueAbility;
};

STATIC_ASSERT(sizeof(struct CompressedDynamicData) == sizeof(struct CompressedDynamicData_Original), SizeOfCompressedDynamicData_Original);
STATIC_ASSERT(sizeof(struct CompressedDynamicData) == sizeof(struct CompressedDynamicData_MonType), SizeOfCompressedDynamicData_MonType);
STATIC_ASSERT(sizeof(struct CompressedDynamicData) == sizeof(struct CompressedDynamicData_OriginalUniqueAbility), SizeOfCompressedDynamicData_OriginalUniqueAbility);
STATIC_ASSERT(sizeof(struct CompressedDynamicData) == sizeof(struct CompressedDynamicData_MonTypeUniqueAbility), SizeOfCompressedDynamicData_MonTypeUniqueAbility);
STATIC_ASSERT(sizeof(struct CompressedDynamicData) == sizeof(u32), SizeOfDynamicCustomMonData);

static u16 SelectTypeBasedExtraMove(u8 type, u8 typeSlot, u8 typeMoveFlip)
{
#ifdef ROGUE_EXPANSION
    static const u16 sTypeBasedExtraMoves[NUMBER_OF_MON_TYPES][4] =
    {
        [TYPE_NORMAL]   = { MOVE_EXTREME_SPEED,  MOVE_GLARE,        MOVE_BOOMBURST,     MOVE_BODY_SLAM },
        [TYPE_FIGHTING] = { MOVE_CLOSE_COMBAT,   MOVE_BULK_UP,      MOVE_FOCUS_BLAST,   MOVE_DRAIN_PUNCH },
        [TYPE_FLYING]   = { MOVE_BRAVE_BIRD,     MOVE_ROOST,        MOVE_HURRICANE,     MOVE_AIR_SLASH },
        [TYPE_POISON]   = { MOVE_GUNK_SHOT,      MOVE_TOXIC,        MOVE_SLUDGE_BOMB,   MOVE_POISON_JAB },
        [TYPE_GROUND]   = { MOVE_HEADLONG_RUSH,  MOVE_SPIKES,       MOVE_EARTH_POWER,   MOVE_SCORCHING_SANDS },
        [TYPE_ROCK]     = { MOVE_ROCK_SLIDE,     MOVE_STEALTH_ROCK, MOVE_POWER_GEM,     MOVE_ANCIENT_POWER },
        [TYPE_BUG]      = { MOVE_LUNGE,          MOVE_STICKY_WEB,   MOVE_BUG_BUZZ,      MOVE_FELL_STINGER },
        [TYPE_GHOST]    = { MOVE_POLTERGEIST,    MOVE_DESTINY_BOND, MOVE_SHADOW_BALL,   MOVE_HEX },
        [TYPE_STEEL]    = { MOVE_IRON_HEAD,      MOVE_SHIFT_GEAR,   MOVE_FLASH_CANNON,  MOVE_GYRO_BALL },
        [TYPE_FIRE]     = { MOVE_FIRE_LASH,      MOVE_WILL_O_WISP,  MOVE_OVERHEAT,      MOVE_SACRED_FIRE },
        [TYPE_WATER]    = { MOVE_LIQUIDATION,    MOVE_RAIN_DANCE,   MOVE_MUDDY_WATER,   MOVE_FLIP_TURN },
        [TYPE_GRASS]    = { MOVE_LEAF_BLADE,     MOVE_LEECH_SEED,   MOVE_GIGA_DRAIN,    MOVE_APPLE_ACID },
        [TYPE_ELECTRIC] = { MOVE_SUPERCELL_SLAM, MOVE_THUNDER_WAVE, MOVE_THUNDER,       MOVE_VOLT_SWITCH },
        [TYPE_PSYCHIC]  = { MOVE_ZEN_HEADBUTT,   MOVE_TRICK_ROOM,   MOVE_PSYSHOCK,      MOVE_FUTURE_SIGHT },
        [TYPE_ICE]      = { MOVE_TRIPLE_AXEL,    MOVE_HAZE,         MOVE_FREEZE_DRY,    MOVE_FROST_BREATH },
        [TYPE_DRAGON]   = { MOVE_DRAGON_RUSH,    MOVE_DRAGON_DANCE, MOVE_DRACO_METEOR,  MOVE_BREAKING_SWIPE },
        [TYPE_DARK]     = { MOVE_NIGHT_SLASH,    MOVE_TAUNT,        MOVE_DARK_PULSE,    MOVE_FOUL_PLAY },
        [TYPE_FAIRY]    = { MOVE_PLAY_ROUGH,     MOVE_CHARM,        MOVE_MOONBLAST,     MOVE_DRAINING_KISS },
    };

    if (type < NUMBER_OF_MON_TYPES && sTypeBasedExtraMoves[type][0] != MOVE_NONE)
        return sTypeBasedExtraMoves[type][(typeSlot << 1) | typeMoveFlip];
#else
    u8 rng = typeMoveFlip;

    (void)typeSlot;

    switch (type)
    {
    case TYPE_NORMAL:
        return rng ? MOVE_QUICK_ATTACK : MOVE_BODY_SLAM;
    case TYPE_FIGHTING:
        return rng ? MOVE_MACH_PUNCH : MOVE_FOCUS_PUNCH;
    case TYPE_FLYING:
        return rng ? MOVE_FLY : MOVE_AEROBLAST;
    case TYPE_POISON:
        return rng ? MOVE_TOXIC : MOVE_SLUDGE_BOMB;
    case TYPE_GROUND:
        return rng ? MOVE_EARTHQUAKE : MOVE_DIG;
    case TYPE_ROCK:
        return rng ? MOVE_ROCK_SLIDE : MOVE_SANDSTORM;
    case TYPE_BUG:
        return rng ? MOVE_SIGNAL_BEAM : MOVE_PIN_MISSILE;
    case TYPE_GHOST:
        return rng ? MOVE_SHADOW_BALL : MOVE_SHADOW_PUNCH;
    case TYPE_STEEL:
        return rng ? MOVE_METEOR_MASH : MOVE_IRON_DEFENSE;
    case TYPE_FIRE:
        return rng ? MOVE_FLAMETHROWER : MOVE_WILL_O_WISP;
    case TYPE_WATER:
        return rng ? MOVE_SURF : MOVE_WATER_PULSE;
    case TYPE_GRASS:
        return rng ? MOVE_GIGA_DRAIN : MOVE_LEAF_BLADE;
    case TYPE_ELECTRIC:
        return rng ? MOVE_THUNDER : MOVE_VOLT_TACKLE;
    case TYPE_PSYCHIC:
        return rng ? MOVE_PSYCHIC : MOVE_CALM_MIND;
    case TYPE_ICE:
        return rng ? MOVE_ICE_BEAM : MOVE_BLIZZARD;
    case TYPE_DRAGON:
        return rng ? MOVE_DRAGON_BREATH : MOVE_DRAGON_CLAW;
    case TYPE_DARK:
        return rng ? MOVE_CRUNCH : MOVE_BITE;
    }
#endif

    return MOVE_RETURN;
}

static u16 EncodeDynamicMoveSelection(u16 move1, u16 move2)
{
    u16 first;
    u16 second;
    u32 pairRank;

    if(move1 == 0)
    {
        AGB_ASSERT(move2 <= ARRAY_COUNT(sDynamicCustomMonMoves));
        return move2;
    }
    if(move2 == 0)
    {
        AGB_ASSERT(move1 <= ARRAY_COUNT(sDynamicCustomMonMoves));
        return move1;
    }

    AGB_ASSERT(move1 <= ARRAY_COUNT(sDynamicCustomMonMoves));
    AGB_ASSERT(move2 <= ARRAY_COUNT(sDynamicCustomMonMoves));
    if(move1 == move2)
    {
        AGB_ASSERT(FALSE);
        return move1;
    }

    first = move1 - 1;
    second = move2 - 1;
    if(first > second)
    {
        u16 temp = first;
        first = second;
        second = temp;
    }

    pairRank = first * (2 * DYNAMIC_MOVE_POOL_CAPACITY - first - 1) / 2;
    pairRank += second - first - 1;
    return DYNAMIC_MOVE_PAIR_CODE_START + pairRank;
}

static void AppendDynamicMoveByIndex(struct DynamicMonData* outData, u16 moveIndex)
{
    if(moveIndex != 0 && moveIndex <= ARRAY_COUNT(sDynamicCustomMonMoves))
    {
        AGB_ASSERT(outData->movesCount < ARRAY_COUNT(outData->moves));
        outData->moves[outData->movesCount++] = sDynamicCustomMonMoves[moveIndex - 1];
    }
    else
    {
        AGB_ASSERT(FALSE);
    }
}

static void DecodeDynamicMoveSelection(u16 moveSelection, struct DynamicMonData* outData)
{
    u16 first;
    u32 pairRank;

    if(moveSelection == 0)
        return;

    if(moveSelection <= DYNAMIC_MOVE_POOL_CAPACITY)
    {
        AppendDynamicMoveByIndex(outData, moveSelection);
        return;
    }

    if(moveSelection < DYNAMIC_MOVE_PAIR_CODE_START)
    {
        AGB_ASSERT(FALSE);
        return;
    }

    pairRank = moveSelection - DYNAMIC_MOVE_PAIR_CODE_START;
    if(pairRank >= DYNAMIC_MOVE_PAIR_COUNT)
    {
        AGB_ASSERT(FALSE);
        return;
    }

    for(first = 0; first < DYNAMIC_MOVE_POOL_CAPACITY - 1; ++first)
    {
        u16 remainingPairs = DYNAMIC_MOVE_POOL_CAPACITY - first - 1;

        if(pairRank < remainingPairs)
        {
            AppendDynamicMoveByIndex(outData, first + 1);
            AppendDynamicMoveByIndex(outData, first + pairRank + 2);
            return;
        }

        pairRank -= remainingPairs;
    }

    AGB_ASSERT(FALSE);
}

static u16 EncodeTypedUniqueMoveSelection(u8 type, u8 typeSlot, u8 typeMoveFlip, u16 moveIndex)
{
    u16 typeSelection = ((type * 2 + typeSlot) * 2 + typeMoveFlip);

    AGB_ASSERT(type < NUMBER_OF_MON_TYPES);
    AGB_ASSERT(typeSlot < 2);
    AGB_ASSERT(typeMoveFlip < 2);
    AGB_ASSERT(moveIndex <= ARRAY_COUNT(sDynamicCustomMonMoves));
    return typeSelection * DYNAMIC_TYPED_MOVE_SELECTION_STRIDE + moveIndex;
}

static void DecodeTypedUniqueMoveSelection(u16 selection, u8* type, u8* typeSlot, u8* typeMoveFlip, u16* moveIndex)
{
    u16 typeSelection = selection / DYNAMIC_TYPED_MOVE_SELECTION_STRIDE;

    *moveIndex = selection % DYNAMIC_TYPED_MOVE_SELECTION_STRIDE;
    *typeMoveFlip = typeSelection % 2;
    typeSelection /= 2;
    *typeSlot = typeSelection % 2;
    *type = typeSelection / 2;

    AGB_ASSERT(*type < NUMBER_OF_MON_TYPES);
    AGB_ASSERT(*moveIndex <= ARRAY_COUNT(sDynamicCustomMonMoves));
}

static void UncompressDynamicMonData(u32 customMonId, struct DynamicMonData* outData)
{
    struct CompressedDynamicData* compressedUntyped = (struct CompressedDynamicData*)&customMonId;

    outData->ability = ABILITY_NONE;
    outData->uniqueAbility = ABILITY_NONE;
    outData->movesCount = 0;
    outData->types[0] = TYPE_NONE;
    outData->types[1] = TYPE_NONE;

    if(compressedUntyped->format == COMPRESSED_FORMAT_ORIGINAL)
    {
        struct CompressedDynamicData_Original* compressedData = (struct CompressedDynamicData_Original*)compressedUntyped;

        outData->ability = ((compressedData->ability - 1) < ARRAY_COUNT(sDynamicCustomMonAbilities)) ? sDynamicCustomMonAbilities[compressedData->ability - 1] : ABILITY_NONE;
        DecodeDynamicMoveSelection(compressedData->moveSelection, outData);
    }
    else if(compressedUntyped->format == COMPRESSED_FORMAT_ORIGINAL_UNIQUE_ABILITY)
    {
        struct CompressedDynamicData_OriginalUniqueAbility* compressedData = (struct CompressedDynamicData_OriginalUniqueAbility*)compressedUntyped;

        outData->ability = ((compressedData->ability - 1) < ARRAY_COUNT(sDynamicCustomMonAbilities)) ? sDynamicCustomMonAbilities[compressedData->ability - 1] : ABILITY_NONE;
        outData->uniqueAbility = ((compressedData->uniqueAbility - 1) < ARRAY_COUNT(sDynamicCustomMonUniqueAbilities)) ? sDynamicCustomMonUniqueAbilities[compressedData->uniqueAbility - 1] : ABILITY_NONE;
        DecodeDynamicMoveSelection(compressedData->moveSelection, outData);
    }
    else if(compressedUntyped->format == COMPRESSED_FORMAT_MON_TYPE)
    {
        struct CompressedDynamicData_MonType* compressedData = (struct CompressedDynamicData_MonType*)compressedUntyped;

        outData->ability = ((compressedData->ability - 1) < ARRAY_COUNT(sDynamicCustomMonAbilities)) ? sDynamicCustomMonAbilities[compressedData->ability - 1] : ABILITY_NONE;
        outData->types[compressedData->typeSlot] = compressedData->type;
        outData->moves[outData->movesCount++] = SelectTypeBasedExtraMove(compressedData->type, compressedData->typeSlot, compressedData->typeMoveFlip);
        DecodeDynamicMoveSelection(compressedData->moveSelection, outData);
    }
    else if(compressedUntyped->format == COMPRESSED_FORMAT_MON_TYPE_UNIQUE_ABILITY)
    {
        struct CompressedDynamicData_MonTypeUniqueAbility* compressedData = (struct CompressedDynamicData_MonTypeUniqueAbility*)compressedUntyped;
        u8 type;
        u8 typeSlot;
        u8 typeMoveFlip;
        u16 moveIndex;

        outData->ability = ((compressedData->ability - 1) < ARRAY_COUNT(sDynamicCustomMonAbilities)) ? sDynamicCustomMonAbilities[compressedData->ability - 1] : ABILITY_NONE;
        outData->uniqueAbility = ((compressedData->uniqueAbility - 1) < ARRAY_COUNT(sDynamicCustomMonUniqueAbilities)) ? sDynamicCustomMonUniqueAbilities[compressedData->uniqueAbility - 1] : ABILITY_NONE;
        DecodeTypedUniqueMoveSelection(compressedData->typeMoveSelection, &type, &typeSlot, &typeMoveFlip, &moveIndex);
        outData->types[typeSlot] = type;
        outData->moves[outData->movesCount++] = SelectTypeBasedExtraMove(type, typeSlot, typeMoveFlip);
        if(moveIndex != 0)
            AppendDynamicMoveByIndex(outData, moveIndex);
    }
    else
    {
        AGB_ASSERT(FALSE);
    }
};

static u32 CompressedDynamicDataToCustomMonId(struct CompressedDynamicData* inData)
{
    u32 id;
    id = *((u32*)inData);
    return id;
}

u32 RogueGift_GetCustomMonId(struct Pokemon* mon)
{
    return RogueGift_GetCustomMonIdBySpecies(GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_OT_ID));
}

u32 RogueGift_GetCustomBoxMonId(struct BoxPokemon* mon)
{
    return RogueGift_GetCustomMonIdBySpecies(GetBoxMonData(mon, MON_DATA_SPECIES), GetBoxMonData(mon, MON_DATA_OT_ID));
}

static u16 TransformCheckSpecies(u16 species)
{
#ifdef ROGUE_EXPANSION
    // Special case (probably should get a proper fix for this)
    if(species >= SPECIES_PIKACHU_COSPLAY && species <= SPECIES_PICHU_SPIKY_EARED)
        return species;

    if(!gSpeciesInfo[species].isAlolanForm && !gSpeciesInfo[species].isGalarianForm && !gSpeciesInfo[species].isHisuianForm && !gSpeciesInfo[species].isPaldeanForm)
    {
        // Fix for castform/other form changes
        return GET_BASE_SPECIES_ID(species);
    }
#endif

    return species;
}

u32 RogueGift_GetCustomMonIdBySpecies(u16 species, u32 otId)
{
    u32 i;

#ifdef ROGUE_EXPANSION
    // The mega itself has it's own ability so don't redirect
    if(species == SPECIES_PIKIN_MEGA)
        return CUSTOM_MON_NONE;
#endif

    species = TransformCheckSpecies(species);

    if((otId & OTID_FLAG_CUSTOM_MON) && (otId & OTID_FLAG_DYNAMIC_CUSTOM_MON))
    {
        // otId is the custom mon id, as it encodes the data into it
        return otId;
    }
    if(otId & OTID_FLAG_CUSTOM_MON)
    {
        species = Rogue_GetEggSpecies(species);

        for(i = CUSTOM_MON_NONE + 1; i < CUSTOM_MON_COUNT; ++i)
        {
            struct CustomMonData const* monData = &sCustomPokemon[i];
            
            if(monData->otId == otId && Rogue_GetEggSpecies(monData->species) == species)
                return i;
        }
    }

    return CUSTOM_MON_NONE;
}

u16 RogueGift_GetCustomMonMove(u32 id, u8 i)
{
    if(id & OTID_FLAG_DYNAMIC_CUSTOM_MON)
    {
        struct DynamicMonData dynamicData;
        UncompressDynamicMonData(id, &dynamicData);
        return i < dynamicData.movesCount ? dynamicData.moves[i] : MOVE_NONE;
    }
    else
    {
        AGB_ASSERT(id < CUSTOM_MON_COUNT);

        if(id != CUSTOM_MON_NONE)
        {
            struct CustomMonData const* monData = &sCustomPokemon[id];
            return i < monData->movesCount ? monData->moves[i] : MOVE_NONE;
        }
    }

    return MOVE_NONE;
}

u16 RogueGift_GetCustomMonMoveCount(u32 id)
{
    if(id & OTID_FLAG_DYNAMIC_CUSTOM_MON)
    {
        struct DynamicMonData dynamicData;
        UncompressDynamicMonData(id, &dynamicData);
        return dynamicData.movesCount;
    }
    else
    {
        AGB_ASSERT(id < CUSTOM_MON_COUNT);

        if(id != CUSTOM_MON_NONE)
        {
            struct CustomMonData const* monData = &sCustomPokemon[id];

            if(id == CUSTOM_MON_ABBIE_MAREEP && !FlagGet(FLAG_ROGUE_UNLOCKED_PIKIN_EASTER_EGG))
            {
                // Don't show last move until activated easter egg
                return monData->movesCount - 1;
            }

            return monData->movesCount;
        }
    }

    return 0;
}

u16 RogueGift_GetCustomMonAbility(u32 id, u8 i)
{
    if(id & OTID_FLAG_DYNAMIC_CUSTOM_MON)
    {
        struct DynamicMonData dynamicData;
        UncompressDynamicMonData(id, &dynamicData);
        return i == 0 ? dynamicData.ability : ABILITY_NONE;
    }
    else
    {
        AGB_ASSERT(id < CUSTOM_MON_COUNT);

        if(id != CUSTOM_MON_NONE)
        {
            struct CustomMonData const* monData = &sCustomPokemon[id];
            return (monData->abilities != NULL && i < NUM_ABILITY_SLOTS) ? monData->abilities[i] : ABILITY_NONE;
        }
    }

    return ABILITY_NONE;
}

u16 RogueGift_GetCustomMonAbilityCount(u32 id)
{
    if(id & OTID_FLAG_DYNAMIC_CUSTOM_MON)
    {
        struct DynamicMonData dynamicData;
        UncompressDynamicMonData(id, &dynamicData);
        return dynamicData.ability != ABILITY_NONE ? NUM_ABILITY_SLOTS : 0;
    }
    else
    {
        struct CustomMonData const* monData = &sCustomPokemon[id];
        AGB_ASSERT(id < CUSTOM_MON_COUNT);

        return (monData->abilities != NULL) ? NUM_ABILITY_SLOTS : 0;
    }
}

u16 RogueGift_GetCustomMonUniqueAbility(u32 id)
{
    if(id & OTID_FLAG_DYNAMIC_CUSTOM_MON)
    {
        struct DynamicMonData dynamicData;
        UncompressDynamicMonData(id, &dynamicData);
        return dynamicData.uniqueAbility;
    }

    return ABILITY_NONE;
}

u8 RogueGift_GetCustomMonType(u32 id, u8 i)
{
    if(id & OTID_FLAG_DYNAMIC_CUSTOM_MON)
    {
        struct DynamicMonData dynamicData;
        UncompressDynamicMonData(id, &dynamicData);
        return i < ARRAY_COUNT(dynamicData.types) ? dynamicData.types[i] : TYPE_NONE;
    }

    return TYPE_NONE;
}

bool8 RogueGift_CanRenameCustomMon(u32 id)
{
    if(id & OTID_FLAG_DYNAMIC_CUSTOM_MON)
    {
        return TRUE;
    }
    else
    {
        // We're allowed to rename exotics, as they're technically dynamic custom mons too
        struct CustomMonData const* monData = &sCustomPokemon[id];
        AGB_ASSERT(id < CUSTOM_MON_COUNT);

        return monData->customTrainerId == CUSTOM_TRAINER_EXOTIC;
    }
}

bool8 RogueGift_DisplayCustomMonRarity(u32 id)
{
    if(id & OTID_FLAG_DYNAMIC_CUSTOM_MON)
        return TRUE;
    else
    {
        struct CustomMonData const* monData = &sCustomPokemon[id]; 
        if(monData->customTrainerId == CUSTOM_TRAINER_EXOTIC || monData->customTrainerId == CUSTOM_TRAINER_LEGEND)
            return TRUE;
    }

    return FALSE;
}

u8 RogueGift_GetCustomMonRarity(u32 id)
{
    if(id & OTID_FLAG_DYNAMIC_CUSTOM_MON)
    {
        struct DynamicMonData dynamicData;
        struct CompressedDynamicData* compressedUntyped = (struct CompressedDynamicData*)&id;
        u16 moveCount;
        u16 abilityCount;

        UncompressDynamicMonData(id, &dynamicData);
        if(dynamicData.uniqueAbility != ABILITY_NONE)
            return UNIQUE_RARITY_LEGENDARY;

        moveCount = RogueGift_GetCustomMonMoveCount(id);
        abilityCount = RogueGift_GetCustomMonAbilityCount(id);

        if(compressedUntyped->format == COMPRESSED_FORMAT_MON_TYPE)
        {
            struct CompressedDynamicData_MonType* compressedData = (struct CompressedDynamicData_MonType*)compressedUntyped;

            if(compressedData->ability != 0)
                return UNIQUE_RARITY_EPIC;

            if(compressedData->moveSelection != 0)
                return UNIQUE_RARITY_RARE;

            return UNIQUE_RARITY_COMMON;
        }

        if(abilityCount != 0)
        {
            if(moveCount <= 1)
                return UNIQUE_RARITY_RARE;
            else
                return UNIQUE_RARITY_EPIC;
        }

        return UNIQUE_RARITY_COMMON;
    }
    else
    {
        struct CustomMonData const* monData = &sCustomPokemon[id]; 
        AGB_ASSERT(id < CUSTOM_MON_COUNT);

        switch (monData->customTrainerId)
        {
        case CUSTOM_TRAINER_COMMON:
            return UNIQUE_RARITY_COMMON;
        
        case CUSTOM_TRAINER_RARE:
            return UNIQUE_RARITY_RARE;
        
        case CUSTOM_TRAINER_EPIC:
            return UNIQUE_RARITY_EPIC;
        
        case CUSTOM_TRAINER_EXOTIC:
            return UNIQUE_RARITY_EXOTIC;

        case CUSTOM_TRAINER_LEGEND:
            return UNIQUE_RARITY_LEGENDARY;
        }

        // We expect most/all of these to be classed as exotic
        return UNIQUE_RARITY_EXOTIC;
    }
}

u8 const* RogueGift_GetRarityName(u8 rarity)
{
    switch (rarity)
    {
    case UNIQUE_RARITY_COMMON:
        return sCustomTrainers[CUSTOM_TRAINER_COMMON].name;
    
    case UNIQUE_RARITY_RARE:
        return sCustomTrainers[CUSTOM_TRAINER_RARE].name;
    
    case UNIQUE_RARITY_EPIC:
        return sCustomTrainers[CUSTOM_TRAINER_EPIC].name;
    
    case UNIQUE_RARITY_EXOTIC:
        return sCustomTrainers[CUSTOM_TRAINER_EXOTIC].name;

    case UNIQUE_RARITY_LEGENDARY:
        return sCustomTrainers[CUSTOM_TRAINER_LEGEND].name;
    }

    AGB_ASSERT(FALSE);
    return sCustomTrainers[CUSTOM_TRAINER_QUESTS].name;
}

void RogueGift_CreateMon(u32 customMonId, struct Pokemon* mon, u16 species, u8 level, u8 fixedIV)
{
    struct CustomTrainerData const* trainerData = NULL;
    u32 temp = 0;
    u16 customMoveCount = RogueGift_GetCustomMonMoveCount(customMonId);
    u16 customAbilityCount = RogueGift_GetCustomMonAbilityCount(customMonId);
    
    if(customMonId & OTID_FLAG_DYNAMIC_CUSTOM_MON)
    {
        u8 customTrainerId = sRarityToCustomTrainerIndex[RogueGift_GetCustomMonRarity(customMonId)];
        trainerData = &sCustomTrainers[customTrainerId];

        ZeroMonData(mon);
        CreateMon(mon, species, level, fixedIV, 0, 0, OT_ID_CUSTOM_MON, customMonId);

        // Assign pokeball
        temp = ITEM_ROGUE_BALL;
        SetMonData(mon, MON_DATA_POKEBALL, &temp);
    }
    else
    {
        struct CustomMonData const* monData = &sCustomPokemon[customMonId]; 
        trainerData = &sCustomTrainers[monData->customTrainerId];

        AGB_ASSERT(customMonId < CUSTOM_MON_COUNT);
        AGB_ASSERT(Rogue_GetEggSpecies(species) == Rogue_GetEggSpecies(monData->species));
        AGB_ASSERT(trainerData->trainerId & OTID_FLAG_CUSTOM_MON);
        AGB_ASSERT(!(trainerData->trainerId & OTID_FLAG_DYNAMIC_CUSTOM_MON));

        ZeroMonData(mon);
        CreateMon(mon, species, level, fixedIV, 0, 0, OT_ID_CUSTOM_MON, trainerData->trainerId);

        // Update nickname
        if(monData->nickname != NULL)
        {
            SetMonData(mon, MON_DATA_NICKNAME, monData->nickname);
        }

        // Assign pokeball
        temp = monData->pokeball;
        SetMonData(mon, MON_DATA_POKEBALL, &temp);

        // Set shiny state
        temp = monData->isShiny ? 1 : 0;
        SetMonData(mon, MON_DATA_IS_SHINY, &temp);
    }

    // Met location
    temp = METLOC_FATEFUL_ENCOUNTER;
    SetMonData(mon, MON_DATA_MET_LOCATION, &temp);

    // Update OT name
    SetMonData(mon, MON_DATA_OT_NAME, trainerData->name);

    // Update OT colour
    temp = trainerData->trainerColour;
    SetMonData(mon, MON_DATA_OT_GENDER, &temp);

    // Default moveset is first 4 moves from custom moveset
    if(customMoveCount)
    {
        u8 m, j;
        u16 moves[MAX_MON_MOVES];

        for(m = 0; m < MAX_MON_MOVES && m < customMoveCount; ++m)
        {
            moves[m] = RogueGift_GetCustomMonMove(customMonId, m);
        }

        // Fill the rest of the moves with default moves
        for(j = 0; m < MAX_MON_MOVES; ++m, ++j)
        {
            moves[m] = GetMonData(mon, MON_DATA_MOVE1 + j);
        }

        // Give back moves
        for(m = 0; m < MAX_MON_MOVES; ++m)
        {
            temp = moves[m];
            SetMonData(mon, MON_DATA_MOVE1 + m, &temp);
            SetMonData(mon, MON_DATA_PP1 + m, &gBattleMoves[temp].pp);
        }
    }

    // Default custom ability
    if(customAbilityCount)
    {
        temp = (RogueGift_GetCustomMonAbility(customMonId, 1) != ABILITY_NONE) ? (Random() % 2) : 0;
        SetMonData(mon, MON_DATA_ABILITY_NUM, &temp);
    }
}

static u32 SelectNextMoveIndex(u16 species)
{
    if(RogueMiscQuery_AnyActiveElements())
    {
        u32 i;
        u16 moveId = RogueMiscQuery_SelectRandomElement(Random());
        RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, moveId);

        for (i = 0; i < ARRAY_COUNT(sDynamicCustomMonMoves); i++)
        {
            if(sDynamicCustomMonMoves[i] == moveId)
                return 1 + i;
        }

        // Should never get here
        AGB_ASSERT(FALSE);
        return 1;
    }

    // Can get here if we've ran out of move options, as everything else is already known
    return 0;
}

static u16 SelectNextMoveSelection(u16 species, u8 moveCount)
{
    u16 move1 = 0;
    u16 move2 = 0;

    AGB_ASSERT(moveCount <= 2);
    if(moveCount >= 1)
        move1 = SelectNextMoveIndex(species);
    if(moveCount >= 2)
        move2 = SelectNextMoveIndex(species);

    return EncodeDynamicMoveSelection(move1, move2);
}

static u32 SelectNextAbilityIndex(u16 species)
{
    u8 i;

    // Give the mon a new ability for it
    while(TRUE)
    {
        u32 idx = (Random() % ARRAY_COUNT(sDynamicCustomMonAbilities));

        for(i = 0; i < NUM_ABILITY_SLOTS; ++i)
        {
            if(GetAbilityBySpecies(species, i, 0) == sDynamicCustomMonAbilities[idx])
            {
                idx = 10000;
                break;
            }
        }

        if(idx != 10000)
            return 1 + idx;
    }

    return 0;
}

static u32 SelectNextUniqueAbilityIndex(u16 species)
{
    u32 idx;
    u16 nativeUniqueAbility = GetUniqueAbilityBySpecies(species);

    do
    {
        idx = Random() % ARRAY_COUNT(sDynamicCustomMonUniqueAbilities);
    }
    while (sDynamicCustomMonUniqueAbilities[idx] == ABILITY_NONE
        || sDynamicCustomMonUniqueAbilities[idx] == nativeUniqueAbility);

    return 1 + idx;
}

static u32 SelectRandomType(u16 species, u8 index)
{
    u8 type;

    do
    {
        type = Random() % NUMBER_OF_MON_TYPES;
    }
    while (!IS_STANDARD_TYPE(type) || GetTypeBySpecies(species, 0, 0) == type || GetTypeBySpecies(species, 1, 0) == type);

    return type;
}

static u32 CreateDynamicMonId(u8 rarity, u16 species, bool8 ignoreTypingUnlockGate)
{
    u16 i;
    u32 temp;
    struct CompressedDynamicData compressedDataUntyped = {0};

    switch (Random() % 2)
    {
    case 0:
        compressedDataUntyped.format = COMPRESSED_FORMAT_ORIGINAL;
        break;

    case 1:
        compressedDataUntyped.format = COMPRESSED_FORMAT_MON_TYPE;
        break;
    }

    if(compressedDataUntyped.format == COMPRESSED_FORMAT_MON_TYPE && !ignoreTypingUnlockGate && !RogueHub_HasUpgrade(HUB_UPGRADE_LAB_UNIQUE_TYPINGS))
        compressedDataUntyped.format = COMPRESSED_FORMAT_ORIGINAL;

    if(rarity == UNIQUE_RARITY_LEGENDARY)
    {
        if(compressedDataUntyped.format == COMPRESSED_FORMAT_MON_TYPE)
            compressedDataUntyped.format = COMPRESSED_FORMAT_MON_TYPE_UNIQUE_ABILITY;
        else
            compressedDataUntyped.format = COMPRESSED_FORMAT_ORIGINAL_UNIQUE_ABILITY;
    }

    // Start query with moves which are valid
    RogueCustomQuery_Begin();

    for (i = 0; i < ARRAY_COUNT(sDynamicCustomMonMoves); i++)
    {
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, sDynamicCustomMonMoves[i]);
    }

    for (i = 0; gRoguePokemonProfiles[species].levelUpMoves[i].move != MOVE_NONE; i++)
    {
        RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, gRoguePokemonProfiles[species].levelUpMoves[i].move);
    }

    for (i = 0; gRoguePokemonProfiles[species].tutorMoves[i] != MOVE_NONE; i++)
    {
        RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, gRoguePokemonProfiles[species].tutorMoves[i]);
    }

    if(compressedDataUntyped.format == COMPRESSED_FORMAT_ORIGINAL)
    {
        struct CompressedDynamicData_Original* compressedData = (struct CompressedDynamicData_Original*)&compressedDataUntyped;

        switch (rarity)
        {
        case UNIQUE_RARITY_COMMON:
            compressedData->moveSelection = SelectNextMoveSelection(species, 2);
            break;

        case UNIQUE_RARITY_RARE:
            compressedData->moveSelection = SelectNextMoveSelection(species, 1);
            compressedData->ability = SelectNextAbilityIndex(species);
            break;

        case UNIQUE_RARITY_EPIC:
            compressedData->moveSelection = SelectNextMoveSelection(species, 2);
            compressedData->ability = SelectNextAbilityIndex(species);
            break;

        default:
            AGB_ASSERT(FALSE);
            break;
        }
    }
    else if(compressedDataUntyped.format == COMPRESSED_FORMAT_ORIGINAL_UNIQUE_ABILITY)
    {
        struct CompressedDynamicData_OriginalUniqueAbility* compressedData = (struct CompressedDynamicData_OriginalUniqueAbility*)&compressedDataUntyped;

        switch (rarity)
        {
        case UNIQUE_RARITY_LEGENDARY:
            compressedData->moveSelection = SelectNextMoveSelection(species, 2);
            compressedData->ability = SelectNextAbilityIndex(species);
            compressedData->uniqueAbility = SelectNextUniqueAbilityIndex(species);
            break;

        default:
            AGB_ASSERT(FALSE);
            break;
        }
    }
    else if(compressedDataUntyped.format == COMPRESSED_FORMAT_MON_TYPE)
    {
        struct CompressedDynamicData_MonType* compressedData = (struct CompressedDynamicData_MonType*)&compressedDataUntyped;
        compressedData->typeSlot = Random() % 2;
        compressedData->typeMoveFlip = Random() % 2;
        compressedData->type = SelectRandomType(species, compressedData->typeSlot);
        RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, SelectTypeBasedExtraMove(compressedData->type, compressedData->typeSlot, compressedData->typeMoveFlip));

        switch (rarity)
        {
        case UNIQUE_RARITY_COMMON:
            break;

        case UNIQUE_RARITY_RARE:
            compressedData->moveSelection = SelectNextMoveSelection(species, 2);
            break;

        case UNIQUE_RARITY_EPIC:
            compressedData->moveSelection = SelectNextMoveSelection(species, 1);
            compressedData->ability = SelectNextAbilityIndex(species);
            break;

        default:
            AGB_ASSERT(FALSE);
            break;
        }
    }
    else if(compressedDataUntyped.format == COMPRESSED_FORMAT_MON_TYPE_UNIQUE_ABILITY)
    {
        struct CompressedDynamicData_MonTypeUniqueAbility* compressedData = (struct CompressedDynamicData_MonTypeUniqueAbility*)&compressedDataUntyped;
        u8 typeSlot = Random() % 2;
        u8 typeMoveFlip = Random() % 2;
        u8 type = SelectRandomType(species, typeSlot);

        RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, SelectTypeBasedExtraMove(type, typeSlot, typeMoveFlip));

        switch (rarity)
        {
        case UNIQUE_RARITY_LEGENDARY:
            compressedData->typeMoveSelection = EncodeTypedUniqueMoveSelection(type, typeSlot, typeMoveFlip, SelectNextMoveIndex(species));
            compressedData->ability = SelectNextAbilityIndex(species);
            compressedData->uniqueAbility = SelectNextUniqueAbilityIndex(species);
            break;

        default:
            AGB_ASSERT(FALSE);
            break;
        }
    }
    else
    {
        AGB_ASSERT(FALSE);
    }

    RogueCustomQuery_End();

    temp = CompressedDynamicDataToCustomMonId(&compressedDataUntyped);
    temp |= (OTID_FLAG_CUSTOM_MON | OTID_FLAG_DYNAMIC_CUSTOM_MON);

#ifdef ROGUE_DEBUG
    // Ensure data (un)compresses correctly
    {
        struct DynamicMonData dynamicData = {0};

        UncompressDynamicMonData(temp, &dynamicData);
    }

#endif
    return temp;
}

u32 RogueGift_CreateDynamicMonId(u8 rarity, u16 species)
{
    return CreateDynamicMonId(rarity, species, FALSE);
}

u32 RogueGift_CreateDynamicMonIdRaw(u8 rarity, u16 species)
{
    return CreateDynamicMonId(rarity, species, TRUE);
}

static bool8 IsDynamicUniqueMonValid(struct UniqueMon* mon)
{
    return !(mon->species == SPECIES_NONE || mon->customMonId == 0 || mon->countDown == 0);
}

u8 RogueGift_RollDynamicUniqueRarity(bool8 ignoreUnlockGates)
{
    u8 rarity;

    switch (Random() % 8)
    {
    case 0:
        rarity = UNIQUE_RARITY_LEGENDARY;
        break;

    case 1:
    case 2:
    case 3:
        rarity = UNIQUE_RARITY_EPIC;
        break;

    case 4:
    case 5:
        rarity = UNIQUE_RARITY_RARE;
        break;
    
    case 6:
    case 7:
        rarity = UNIQUE_RARITY_COMMON;
        break;

    default:
        AGB_ASSERT(FALSE);
        rarity = UNIQUE_RARITY_COMMON;
        break;
    }

    if(ignoreUnlockGates)
        return rarity;

    if(rarity == UNIQUE_RARITY_LEGENDARY && !RogueHub_HasUpgrade(HUB_UPGRADE_LAB_UNIQUE_MON_RARITY_LEGENDARY))
        rarity = UNIQUE_RARITY_EPIC;

    if(rarity == UNIQUE_RARITY_EPIC && !RogueHub_HasUpgrade(HUB_UPGRADE_LAB_UNIQUE_MON_RARITY_EPIC))
        rarity = UNIQUE_RARITY_RARE;

    if(rarity == UNIQUE_RARITY_RARE && !RogueHub_HasUpgrade(HUB_UPGRADE_LAB_UNIQUE_MON_RARITY_RARE))
        rarity = UNIQUE_RARITY_COMMON;

    return rarity;
}

static bool8 IsSlotUnlocked(u8 slot)
{
    if(RogueHub_HasUpgrade(HUB_UPGRADE_LAB_UNIQUE_MON_LAB))
    {
        // 4 slots unlocked by default
        if(slot < 4)
            return TRUE;

        if(slot < 6)
            return RogueHub_HasUpgrade(HUB_UPGRADE_LAB_UNIQUE_MON_SLOTS0);

        if(slot < 8)
            return RogueHub_HasUpgrade(HUB_UPGRADE_LAB_UNIQUE_MON_SLOTS1);
    }

    return FALSE;
}

void RogueGift_EnsureDynamicCustomMonsAreValid()
{
    u8 i, write;
    u16 newSpecies[DYNAMIC_UNIQUE_MON_COUNT];

    // We use query below, so grab some new unique species now
    {
        // Override dex variant to our max nat dex
        u8 dexVariantToRestore = RoguePokedex_GetDexVariant();
        RoguePokedex_SetDexVariant(POKEDEX_VARIANT_DEFAULT);

        RogueMonQuery_Begin();
        RogueMonQuery_IsSpeciesActive();
        RogueMonQuery_TransformIntoEggSpecies();
        RogueMonQuery_IsLegendary(QUERY_FUNC_EXCLUDE);
        
        for(i = 0; i < DYNAMIC_UNIQUE_MON_COUNT; ++i)
        {
            if(IsDynamicUniqueMonValid(&gRogueSaveBlock->dynamicUniquePokemon[i]))
            {
                RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, gRogueSaveBlock->dynamicUniquePokemon[i].species);
            }
        }

        // Remove random entries until we can safely calcualte weights without going over
        while(RogueWeightQuery_IsOverSafeCapacity())
        {
            RogueMiscQuery_FilterByChance(Random(), QUERY_FUNC_INCLUDE, 50, DYNAMIC_UNIQUE_MON_COUNT);
        }

        RogueWeightQuery_Begin();
        RogueWeightQuery_FillWeights(1);

        for(i = 0; i < DYNAMIC_UNIQUE_MON_COUNT; ++i)
        {
            newSpecies[i] = RogueWeightQuery_SelectRandomFromWeightsWithUpdate(Random(), 0);
        }

        RogueWeightQuery_End();

        RogueMonQuery_End();

        RoguePokedex_SetDexVariant(dexVariantToRestore);
    }

    // Shift all the empty slots up 
    write = 0;
    for(i = 0; i < DYNAMIC_UNIQUE_MON_COUNT; ++i)
    {
        if(IsDynamicUniqueMonValid(&gRogueSaveBlock->dynamicUniquePokemon[i]))
        {
            if(write != i)
            {
                memcpy(&gRogueSaveBlock->dynamicUniquePokemon[write], &gRogueSaveBlock->dynamicUniquePokemon[i], sizeof(struct UniqueMon));
                memset(&gRogueSaveBlock->dynamicUniquePokemon[i], 0, sizeof(struct UniqueMon));
            }

            write++;
        }
        else
        {
            memset(&gRogueSaveBlock->dynamicUniquePokemon[i], 0, sizeof(struct UniqueMon));
        }
    }

    // Replace invalid mons
    for(i = 0; i < DYNAMIC_UNIQUE_MON_COUNT; ++i)
    {
        // We don't want to populate slots that aren't active
        if(!IsSlotUnlocked(i))
            continue;

        if(!IsDynamicUniqueMonValid(&gRogueSaveBlock->dynamicUniquePokemon[i]))
        {
            u8 rarity = RogueGift_RollDynamicUniqueRarity(FALSE);

            gRogueSaveBlock->dynamicUniquePokemon[i].countDown = 60 + 30 * i; // Time remaining is based on the slot

            gRogueSaveBlock->dynamicUniquePokemon[i].species = newSpecies[i];
            gRogueSaveBlock->dynamicUniquePokemon[i].customMonId = RogueGift_CreateDynamicMonId(rarity, newSpecies[i]);
        }
    }

}

void RogueGift_ClearDynamicCustomMons()
{
    memset(&gRogueSaveBlock->dynamicUniquePokemon[0], 0, sizeof(struct UniqueMon) * DYNAMIC_UNIQUE_MON_COUNT);
}

struct UniqueMon* RogueGift_GetDynamicUniqueMon(u8 slot)
{
    AGB_ASSERT(slot < DYNAMIC_UNIQUE_MON_COUNT);
    return &gRogueSaveBlock->dynamicUniquePokemon[slot];
}

void RogueGift_CountDownDynamicCustomMons()
{
    u8 i;

    for(i = 0; i < DYNAMIC_UNIQUE_MON_COUNT; ++i)
    {
        if(gRogueSaveBlock->dynamicUniquePokemon[i].countDown != 0)
            --gRogueSaveBlock->dynamicUniquePokemon[i].countDown;
    }
}

void RogueGift_RemoveDynamicCustomMon(u32 customMonId)
{
    u8 i;

    for(i = 0; i < DYNAMIC_UNIQUE_MON_COUNT; ++i)
    {
        if(gRogueSaveBlock->dynamicUniquePokemon[i].customMonId == customMonId)
        {
            memset(&gRogueSaveBlock->dynamicUniquePokemon[i], 0, sizeof(struct UniqueMon));
        }
    }
}

bool8 RogueGift_IsDynamicMonSlotEnabled(u8 slot)
{
    AGB_ASSERT(slot < DYNAMIC_UNIQUE_MON_COUNT);

    if(!IsSlotUnlocked(slot))
        return FALSE;

    return IsDynamicUniqueMonValid(&gRogueSaveBlock->dynamicUniquePokemon[slot]);
}

u32 RogueGift_TryFindEnabledDynamicCustomMonForSpecies(u16 species)
{
    u8 i;

    species = Rogue_GetEggSpecies(species);

    for(i = 0; i < DYNAMIC_UNIQUE_MON_COUNT; ++i)
    {
        if(RogueGift_IsDynamicMonSlotEnabled(i))
        {
            if(gRogueSaveBlock->dynamicUniquePokemon[i].species == species)
                return gRogueSaveBlock->dynamicUniquePokemon[i].customMonId;
        }
    }

    return 0;
}

static u8 ModifyShinyType(u8 type)
{
    if(type == TYPE_NONE)
        return type;

    do
    {
        type = (type + 5) % NUMBER_OF_MON_TYPES;
    } while (!IS_STANDARD_TYPE(type));

    return type;
}

bool8 RogueGift_TryApplyPaletteModify(u32 id, bool8 isShiny, u16 const* inputPal, u16 const* layerRefPal, u16* outputPal)
{
    u8 type1 = RogueGift_GetCustomMonType(id, 0);
    u8 type2 = RogueGift_GetCustomMonType(id, 1);

    if(type1 != TYPE_NONE || type2 != TYPE_NONE)
    {
        u16 layerPal[16];
        u16 layerWhitePoints[PALETTE_MODIFY_LAYER_COUNT];
        u16 colorsToApply[PALETTE_MODIFY_LAYER_COUNT];

        if(isShiny)
        {
            type1 = ModifyShinyType(type1);
            type2 = ModifyShinyType(type2);
        }

        colorsToApply[0] = RGB_ALPHA;
        colorsToApply[1] = RGB_ALPHA;
        colorsToApply[2] = RGB_ALPHA;

        if(type1 != TYPE_NONE)
            colorsToApply[0] = sTypeTintColors[type1];

        if(type2 != TYPE_NONE)
            colorsToApply[1] = sTypeTintColors[type2];

        if(layerRefPal == NULL)
        {
            Rogue_GenerateLayerPaletteByHue(inputPal, inputPal, layerPal);
            Rogue_GenerateWhitePointsPerLayers(inputPal, layerPal, layerWhitePoints, gDefaultPaletteLayerMasks);
        }
        else
        {
            Rogue_GenerateLayerPaletteByHue(layerRefPal, layerRefPal, layerPal);
            Rogue_GenerateWhitePointsPerLayers(layerRefPal, layerPal, layerWhitePoints, gDefaultPaletteLayerMasks);
            Rogue_GenerateLayerPaletteByHue(inputPal, layerRefPal, layerPal);
        }

        Rogue_ModifyPaletteByLayersHueShift(inputPal, layerPal, layerWhitePoints, outputPal, gDefaultPaletteLayerMasks, colorsToApply);
        return TRUE;
    }

    return FALSE;
}
