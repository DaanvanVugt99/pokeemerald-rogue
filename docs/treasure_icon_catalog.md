# Treasure Icon Catalog

This catalog tracks imported sprites for Divergence Treasure held items. Icons
marked `Implemented` have an allocated item ID and battle effect; the remaining
sprites are available for future design passes. This keeps visual exploration
separate from save-sensitive item definitions.

Each icon is a native 24x24 indexed PNG with transparency at palette index 0.
Its matching JASC palette contains 16 entries and uses CRLF line endings. The
source pack stored these sprites at 48x48, but every imported image was an exact
2x nearest-neighbor enlargement; the port retains each logical source pixel.

| Working name | Graphics symbol suffix | Design lane | Effect status |
| --- | --- | --- | --- |
| Bell Jewel | `BellJewel` | Sound, move repetition, or delayed payoff | TBD |
| Berserker's Greaves | `BerserkersGreaves` | Low-HP aggression, commitment, or Speed | TBD |
| Buffer Shield | `BufferShield` | One-hit protection or damage smoothing | TBD |
| Champion's Band | `ChampionsBand` | Knockouts, streaks, or earned momentum | TBD |
| Compound Lens | `CompoundLens` | Accuracy, information, or multi-hit moves | TBD |
| Echo Scepter | `EchoScepter` | Repeated moves, copied effects, or echoes | Implemented |
| Electric Tiki | `ElectricTiki` | Electric Terrain or Electric-type interaction | Implemented |
| Final Stand | `FinalStand` | Last-Pokemon or low-HP comeback | TBD |
| Focus Lens | `FocusLens` | Critical hits, focus, or single-target moves | TBD |
| Gambler's Vest | `GamblersVest` | Volatility, random rewards, or risk protection | TBD |
| Glass Sword | `GlassCannon` | Large offense-for-defense trade | Implemented |
| Glyph Codex | `GlyphCodex` | Move types, categories, or move-slot interaction | TBD |
| Golden Egg | `GoldenEgg` | Growth, recovery, or run-level rewards | TBD |
| Golden Idol | `GoldenIdol` | Greed, sacrifice, or cursed rewards | TBD |
| Grassy Tiki | `GrassyTiki` | Grassy Terrain or Grass-type interaction | Implemented |
| Greedy Glove | `GreedyGlove` | Theft, held-item interaction, or extra rewards | TBD |
| Snow Totem | `HailTotem` | Snow and Ice-type teams | Implemented |
| Hexing Wand | `HexingWand` | Status conditions, curses, or reflected effects | TBD |
| Misty Tiki | `MistyTiki` | Misty Terrain or Fairy-type interaction | Implemented |
| Psychic Tiki | `PsychicTiki` | Psychic Terrain or Psychic-type interaction | Implemented |
| Moody Hat | `MoodyHat` | Stat-stage roulette or switching | TBD |
| Quick Talon | `QuickTalon` | Priority, Speed order, or first-turn effects | TBD |
| Rain Totem | `RainTotem` | Rain and Water-type teams | Implemented |
| Acid Rain Totem | `AcidRainTotem` | Acid Rain and Poison-type teams | Implemented |
| Ramping Staff | `RampingStaff` | Consecutive turns or escalating effects | TBD |
| Sand Totem | `SandTotem` | Sandstorm and Rock/Ground/Steel teams | Implemented |
| Specialist Jewel | `SpecialistJewel` | Move-category or single-type specialization | TBD |
| Sun Totem | `SunTotem` | Sun and Fire/Grass-type teams | Implemented |
| Eclipse Totem | `EclipseTotem` | Eclipse Orb mechanics or inverted matchups | Implemented |
| Wonder Shield | `WonderShield` | Type matchup defense or conditional immunity | Implemented |

For a working name such as Bell Jewel, item data can use
`gItemIcon_TreasureBellJewel` and
`gItemIconPalette_TreasureBellJewel`. Source and credit details are recorded in
[`third_party/tower-of-ashes-item-icons.md`](third_party/tower-of-ashes-item-icons.md).
