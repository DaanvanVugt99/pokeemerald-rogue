# Treasure Icon Catalog

This catalog tracks imported sprites for Divergence Treasure held items. Icons
marked `Implemented` have an allocated item ID and battle effect; the remaining
sprites are available for future design passes. This keeps visual exploration
separate from save-sensitive item definitions.

Each icon is a native 24x24 indexed PNG with transparency at palette index 0.
Its matching JASC palette contains 16 entries and uses CRLF line endings.
Imports preserve source pixels when already native-sized or exactly enlarged;
the adaptation notes document the few sprites that required background cleanup,
palette reduction, or resizing.

| Working name | Graphics symbol suffix | Design lane | Effect status | Source |
| --- | --- | --- | --- | --- |
| Chime Jewel | `BellJewel` | Sound moves raise the holder's Speed | Implemented | Tower of Ashes |
| Berserker's Greaves | `BerserkersGreaves` | Low-HP aggression, commitment, or Speed | TBD | Tower of Ashes |
| Impact Plating | `BufferShield` | Halves contact damage, but takes 50% more from non-contact moves | Implemented | Tower of Ashes |
| Champion's Band | `ChampionsBand` | Knockouts, streaks, or earned momentum | TBD | Tower of Ashes |
| Twin Goggles | `CompoundLens` | Doubles damaging-move accuracy, but prevents additional effects | Implemented | Tower of Ashes |
| Echo Scepter | `EchoScepter` | Repeated moves, copied effects, or echoes | Implemented | Tower of Ashes |
| Electric Tiki | `ElectricTiki` | Electric Terrain or Electric-type interaction | Implemented | Tower of Ashes |
| Fury Mantle | `FinalStand` | Contact hits randomly raise Attack or Speed | Implemented | Tower of Ashes |
| Precise Lens | `FocusLens` | Guaranteed critical hits when moving after the target | Implemented | Tower of Ashes |
| Gambler's Vest | `GamblersVest` | Volatility, random rewards, or risk protection | TBD | Tower of Ashes |
| Glass Sword | `GlassCannon` | Large offense-for-defense trade | Implemented | Tower of Ashes |
| Glyph Codex | `GlyphCodex` | Changes the holder to each move's type before use | Implemented | Tower of Ashes |
| Golden Egg | `GoldenEgg` | Gives healing moves priority at half HP or less | Implemented | Tower of Ashes |
| Golden Idol | `GoldenIdol` | Greed, sacrifice, or cursed rewards | TBD | Tower of Ashes |
| Grassy Tiki | `GrassyTiki` | Grassy Terrain or Grass-type interaction | Implemented | Tower of Ashes |
| Greedy Gloves | `GreedyGlove` | Contact moves steal the target's positive stat changes before damage | Implemented | Tower of Ashes |
| Snow Totem | `HailTotem` | Snow and Ice-type teams | Implemented | Tower of Ashes |
| Hexing Wand | `HexingWand` | Boosts damage against statused targets | Implemented | Tower of Ashes |
| Misty Tiki | `MistyTiki` | Misty Terrain or Fairy-type interaction | Implemented | Tower of Ashes |
| Psychic Tiki | `PsychicTiki` | Psychic Terrain or Psychic-type interaction | Implemented | Tower of Ashes |
| Fickle Hat | `MoodyHat` | Sharply raises one random stat and lowers another each turn | Implemented | Tower of Ashes |
| Ambush Talon | `QuickTalon` | Gives the holder's first move +1 priority once per battle | Implemented | Tower of Ashes |
| Rain Totem | `RainTotem` | Rain and Water-type teams | Implemented | Tower of Ashes |
| Acid Rain Totem | `AcidRainTotem` | Acid Rain and Poison-type teams | Implemented | Tower of Ashes |
| Ramping Staff | `RampingStaff` | Consecutive turns or escalating effects | TBD | Tower of Ashes |
| Sand Totem | `SandTotem` | Sandstorm and Rock/Ground/Steel teams | Implemented | Tower of Ashes |
| Purity Jewel | `SpecialistJewel` | Boosts damage when all known moves have the same type | Implemented | Tower of Ashes |
| Sun Totem | `SunTotem` | Sun and Fire/Grass-type interaction | Implemented | Tower of Ashes |
| Eclipse Totem | `EclipseTotem` | Eclipse Orb mechanics or inverted matchups | Implemented | Tower of Ashes |
| Wonder Shield | `WonderShield` | Type matchup defense or conditional immunity | Implemented | Tower of Ashes |
| Adaptive Specs | `ChampionsSpecs` | Damaging moves use whichever category deals more damage | Implemented | Tower of Ashes |
| Choice Jewel | `ChoiceJewel` | Open concept | TBD | Tower of Ashes |
| Clear Armor | `ClearArmor` | Open concept | TBD | Tower of Ashes |
| Crush | `Crush` | Open concept | TBD | Tower of Ashes |
| Drain Blade | `DrainBlade` | Open concept | TBD | Tower of Ashes |
| Grand Plaza | `GrandPlaza` | Open concept | TBD | Tower of Ashes |
| Guts Sword | `GutsSword` | Open concept | TBD | Tower of Ashes |
| Healing Lamp | `HealingLamp` | Open concept | TBD | Tower of Ashes |
| Ho-Oh Plume | `HoOhPlume` | Open concept | TBD | Tower of Ashes |
| Hourglass | `Hourglass` | Open concept | TBD | Tower of Ashes |
| Monotonous Armor | `MonotonousArmor` | Open concept | TBD | Tower of Ashes |
| Monotype | `Monotype` | Open concept | TBD | Tower of Ashes |
| No Guard Belt | `NoGuardBelt` | Open concept | TBD | Tower of Ashes |
| Renegade Orb | `RenegadeOrb` | Open concept | TBD | Tower of Ashes |
| Shaman Staff | `ShamanStaff` | Open concept | TBD | Tower of Ashes |
| Briar Bracer | `Thornmail` | Contact attackers are afflicted by Leech Seed | Implemented | Tower of Ashes |
| Tinted Shield | `TintedShield` | Open concept | TBD | Tower of Ashes |
| Tricky Box | `TrickyBox` | Answers opposing status moves with Copycat | Implemented | Tower of Ashes |
| Vest Jewel | `VestJewel` | Open concept | TBD | Tower of Ashes |
| Wood Cannon | `WoodCannon` | Open concept | TBD | Tower of Ashes |
| X Ancient | `XAncient` | Open concept | TBD | Tower of Ashes |
| Elred Berry | `ElredBerry` | Open concept | TBD | NocTurn |
| Honey Badge | `HoneyBadge` | Open concept | TBD | NocTurn |
| Item 692 | `Item692` | Open concept | TBD | NocTurn |
| Item 693 | `Item693` | Open concept | TBD | NocTurn |
| Megaphone | `Megaphone` | Open concept | TBD | NocTurn |
| Pinwheel | `Pinwheel` | Open concept | TBD | NocTurn |
| Pixie Dust | `PixieDust` | Open concept | TBD | NocTurn |
| Shock Bracer | `ShockBracer` | Open concept | TBD | NocTurn |
| Vital Herb | `VitalHerb` | Open concept | TBD | NocTurn |
| Charcoal Filter | `CharcoalFilter` | Open concept | TBD | How You Survive |
| EXP All | `ExpAll` | Open concept | TBD | How You Survive |
| Food Supply | `FoodSupply` | Open concept | TBD | How You Survive |
| Name Card | `NameCard` | Open concept | TBD | How You Survive |
| Poké Food Can | `PokeFoodCan` | Open concept | TBD | How You Survive |
| Raincoat | `Raincoat` | Grants immunity to Water-type moves during rain | Implemented | How You Survive |
| Sleeping Bag | `SleepingBag` | Open concept | TBD | How You Survive |
| STR Salve | `StrSalve` | Open concept | TBD | How You Survive |
| Tent Bag | `TentBag` | Open concept | TBD | How You Survive |
| Water Bottle | `WaterBottle` | Open concept | TBD | How You Survive |
| Wild Fruit Book | `WildFruitBook` | Open concept | TBD | How You Survive |

For a working name such as Bell Jewel, item data can use
`gItemIcon_TreasureBellJewel` and
`gItemIconPalette_TreasureBellJewel`. Source and credit details are recorded
in [`third_party/tower-of-ashes-item-icons.md`](third_party/tower-of-ashes-item-icons.md)
and [`third_party/additional-treasure-item-icons.md`](third_party/additional-treasure-item-icons.md).
