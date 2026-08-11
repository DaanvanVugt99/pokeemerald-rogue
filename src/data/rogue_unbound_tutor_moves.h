#ifndef GUARD_DATA_ROGUE_UNBOUND_TUTOR_MOVES_H
#define GUARD_DATA_ROGUE_UNBOUND_TUTOR_MOVES_H

// Each lesson offers one move from each role. Tiers follow Adventure
// progression; universal compatibility makes even familiar moves create new
// builds, so unsafe signatures and extreme setup moves are deliberately
// absent.

enum
{
    UNBOUND_TUTOR_ROLE_COVERAGE,
    UNBOUND_TUTOR_ROLE_UTILITY,
    UNBOUND_TUTOR_ROLE_BUILD_AROUND,
    UNBOUND_TUTOR_ROLE_COUNT,
};

enum
{
    UNBOUND_TUTOR_TIER_EARLY,
    UNBOUND_TUTOR_TIER_MID,
    UNBOUND_TUTOR_TIER_LATE,
    UNBOUND_TUTOR_TIER_COUNT,
};

struct RogueUnboundTutorMovePool
{
    const u16 *moves;
    u8 count;
};

#define UNBOUND_MOVE_POOL(pool) {pool, ARRAY_COUNT(pool)}

static const u16 sUnboundEarlyCoverage[] =
{
    MOVE_AERIAL_ACE,
    MOVE_BRICK_BREAK,
    MOVE_GIGA_DRAIN,
    MOVE_ICY_WIND,
    MOVE_ROCK_TOMB,
    MOVE_SHOCK_WAVE,
};

static const u16 sUnboundEarlyUtility[] =
{
    MOVE_ENCORE,
    MOVE_HAZE,
    MOVE_LIGHT_SCREEN,
    MOVE_REFLECT,
    MOVE_TAUNT,
    MOVE_THUNDER_WAVE,
    MOVE_WILL_O_WISP,
};

static const u16 sUnboundEarlyBuildAround[] =
{
    MOVE_AGILITY,
    MOVE_BULK_UP,
    MOVE_CALM_MIND,
    MOVE_ENDURE,
    MOVE_REST,
    MOVE_SLEEP_TALK,
    MOVE_SUBSTITUTE,
};

static const u16 sUnboundMidCoverage[] =
{
    MOVE_BODY_PRESS,
    MOVE_DRAIN_PUNCH,
    MOVE_EARTH_POWER,
    MOVE_FLASH_CANNON,
    MOVE_ICE_SPINNER,
    MOVE_KNOCK_OFF,
    MOVE_PLAY_ROUGH,
    MOVE_PSYCHIC_FANGS,
};

static const u16 sUnboundMidUtility[] =
{
    MOVE_PARTING_SHOT,
    MOVE_SPIKES,
    MOVE_STEALTH_ROCK,
    MOVE_TAILWIND,
    MOVE_TRICK,
    MOVE_TRICK_ROOM,
    MOVE_U_TURN,
};

static const u16 sUnboundMidBuildAround[] =
{
    MOVE_AQUA_JET,
    MOVE_BULLET_PUNCH,
    MOVE_DRAGON_DANCE,
    MOVE_NASTY_PLOT,
    MOVE_RECOVER,
    MOVE_ROOST,
    MOVE_SWORDS_DANCE,
    MOVE_VACUUM_WAVE,
};

static const u16 sUnboundLateCoverage[] =
{
    MOVE_EXTREME_SPEED,
    MOVE_FLIP_TURN,
    MOVE_KNOCK_OFF,
    MOVE_POLLEN_PUFF,
    MOVE_SUCKER_PUNCH,
    MOVE_VOLT_SWITCH,
};

static const u16 sUnboundLateUtility[] =
{
    MOVE_AURORA_VEIL,
    MOVE_DEFOG,
    MOVE_PARTING_SHOT,
    MOVE_STICKY_WEB,
    MOVE_TAILWIND,
    MOVE_TRICK,
};

static const u16 sUnboundLateBuildAround[] =
{
    MOVE_DRAGON_DANCE,
    MOVE_NASTY_PLOT,
    MOVE_QUIVER_DANCE,
    MOVE_RECOVER,
    MOVE_ROOST,
    MOVE_SHIFT_GEAR,
    MOVE_SWORDS_DANCE,
};

static const struct RogueUnboundTutorMovePool sUnboundTutorMovePools[UNBOUND_TUTOR_TIER_COUNT][UNBOUND_TUTOR_ROLE_COUNT] =
{
    [UNBOUND_TUTOR_TIER_EARLY] =
    {
        UNBOUND_MOVE_POOL(sUnboundEarlyCoverage),
        UNBOUND_MOVE_POOL(sUnboundEarlyUtility),
        UNBOUND_MOVE_POOL(sUnboundEarlyBuildAround),
    },
    [UNBOUND_TUTOR_TIER_MID] =
    {
        UNBOUND_MOVE_POOL(sUnboundMidCoverage),
        UNBOUND_MOVE_POOL(sUnboundMidUtility),
        UNBOUND_MOVE_POOL(sUnboundMidBuildAround),
    },
    [UNBOUND_TUTOR_TIER_LATE] =
    {
        UNBOUND_MOVE_POOL(sUnboundLateCoverage),
        UNBOUND_MOVE_POOL(sUnboundLateUtility),
        UNBOUND_MOVE_POOL(sUnboundLateBuildAround),
    },
};

#undef UNBOUND_MOVE_POOL

#endif // GUARD_DATA_ROGUE_UNBOUND_TUTOR_MOVES_H
