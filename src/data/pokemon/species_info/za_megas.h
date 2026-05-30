// Legends Z-A and Mega Dimension Mega Evolutions.
// Imported after Rogue custom species IDs to preserve existing save/data numbering.

#if P_MEGA_EVOLUTIONS
    [SPECIES_CLEFABLE_MEGA] =
    {
        .baseHP        = 95,
        .baseAttack    = 80,
        .baseDefense   = 93,
        .baseSpeed     = 70,
        .baseSpAttack  = 135,
        .baseSpDefense = 110,
        .types = { TYPE_FAIRY, TYPE_FLYING },
        .catchRate = 25,
    #if GEN_LATEST >= GEN_8
        .expYield = 242,
    #elif GEN_LATEST >= GEN_7
        .expYield = 217,
    #elif GEN_LATEST >= GEN_5
        .expYield = 213,
    #else
        .expYield = 129,
    #endif
        .evYield_HP = 3,
        .itemRare = ITEM_MOON_STONE,
        .genderRatio = PERCENT_FEMALE(75),
        .eggCycles = 10,
        .friendship = 140,
        .growthRate = GROWTH_FAST,
        .eggGroups = { EGG_GROUP_FAIRY, EGG_GROUP_FAIRY },
        .abilities = { ABILITY_MAGIC_BOUNCE, ABILITY_MAGIC_BOUNCE, ABILITY_MAGIC_BOUNCE },
        .bodyColor = BODY_COLOR_PINK,
        .speciesName = _("Clefable"),
        .cryId = CRY_CLEFABLE_MEGA,
        .natDexNum = NATIONAL_DEX_CLEFABLE,
        .categoryName = _("Fairy"),
        .height = 17,
        .weight = 423,
        .description = POKEDEX_DESC_STRING(
            "It flies by using the power of\n"
            "moonlight to control gravity within\n"
            "a radius of over 32 feet around it."),
        FRONT_PIC(ClefableMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(ClefableMega, 64, 64),
        .backPicYOffset = 3,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(ClefableMega),
        ICON(ClefableMega, 0),
        .footprint = gMonFootprint_Clefable,
        .formSpeciesIdTable = sClefableFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Clefable),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_VICTREEBEL_MEGA] =
    {
        .baseHP        = 80,
        .baseAttack    = 125,
        .baseDefense   = 85,
        .baseSpeed     = 70,
        .baseSpAttack  = 135,
        .baseSpDefense = 95,
        .types = { TYPE_GRASS, TYPE_POISON },
        .catchRate = 45,
    #if GEN_LATEST >= GEN_8
        .expYield = 245,
    #elif GEN_LATEST >= GEN_7
        .expYield = 221,
    #elif GEN_LATEST >= GEN_5
        .expYield = 216,
    #else
        .expYield = 191,
    #endif
        .evYield_Attack = 3,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = { EGG_GROUP_GRASS, EGG_GROUP_GRASS },
        .abilities = { ABILITY_INNARDS_OUT, ABILITY_NONE, ABILITY_INNARDS_OUT },
        .bodyColor = BODY_COLOR_GREEN,
        .speciesName = _("Victreebel"),
        .cryId = CRY_VICTREEBEL_MEGA,
        .natDexNum = NATIONAL_DEX_VICTREEBEL,
        .categoryName = _("Flycatcher"),
        .height = 45,
        .weight = 1255,
        .description = POKEDEX_DESC_STRING(
            "The volume of this Pokémon's acid\n"
            "has increased due to Mega Evolution,\n"
            "filling its mouth. If not careful,\n"
            "the acid will overflow and spill out."),
        FRONT_PIC(VictreebelMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(VictreebelMega, 64, 64),
        .backPicYOffset = 0,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(VictreebelMega),
        ICON(VictreebelMega, 1),
        .footprint = gMonFootprint_Victreebel,
        .formSpeciesIdTable = sVictreebelFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Victreebel),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_STARMIE_MEGA] =
    {
        .baseHP        = 60,
        .baseAttack    = 140,
        .baseDefense   = 105,
        .baseSpeed     = 120,
        .baseSpAttack  = 130,
        .baseSpDefense = 105,
        .types = { TYPE_WATER, TYPE_PSYCHIC },
        .catchRate = 60,
        .expYield = (GEN_LATEST >= GEN_5) ? 182 : 207,
        .evYield_Speed = 2,
        .itemCommon = ITEM_STARDUST,
        .itemRare = ITEM_STAR_PIECE,
        .genderRatio = MON_GENDERLESS,
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_SLOW,
        .eggGroups = { EGG_GROUP_WATER_3, EGG_GROUP_WATER_3 },
        .abilities = { ABILITY_HUGE_POWER, ABILITY_HUGE_POWER, ABILITY_HUGE_POWER },
        .bodyColor = BODY_COLOR_PURPLE,
        .speciesName = _("Starmie"),
        .cryId = CRY_STARMIE_MEGA,
        .natDexNum = NATIONAL_DEX_STARMIE,
        .categoryName = _("Mysterious"),
        .height = 23,
        .weight = 800,
        .description = POKEDEX_DESC_STRING(
            "Its movements have become more\n"
            "humanlike. Whether it's simply\n"
            "trying to communicate or wants to\n"
            "supplant humanity is unclear."),
        FRONT_PIC(StarmieMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(StarmieMega, 64, 64),
        .backPicYOffset = 0,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(StarmieMega),
        ICON(StarmieMega, 2),
        .footprint = gMonFootprint_Starmie,
        .formSpeciesIdTable = sStarmieFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Starmie),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_DRAGONITE_MEGA] =
    {
        .baseHP        = 91,
        .baseAttack    = 124,
        .baseDefense   = 115,
        .baseSpeed     = 100,
        .baseSpAttack  = 145,
        .baseSpDefense = 125,
        .types = { TYPE_DRAGON, TYPE_FLYING },
        .catchRate = 45,
    #if GEN_LATEST >= GEN_8
        .expYield = 300,
    #elif GEN_LATEST >= GEN_5
        .expYield = 270,
    #else
        .expYield = 218,
    #endif
        .evYield_Attack = 3,
        .itemRare = ITEM_DRAGON_SCALE,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 40,
        .friendship = 35,
        .growthRate = GROWTH_SLOW,
        .eggGroups = { EGG_GROUP_WATER_1, EGG_GROUP_DRAGON },
        .abilities = { ABILITY_MULTISCALE, ABILITY_NONE, ABILITY_MULTISCALE },
        .bodyColor = BODY_COLOR_BROWN,
        .speciesName = _("Dragonite"),
        .cryId = CRY_DRAGONITE_MEGA,
        .natDexNum = NATIONAL_DEX_DRAGONITE,
        .categoryName = _("Dragon"),
        .height = 22,
        .weight = 2900,
        .description = POKEDEX_DESC_STRING(
            "Mega Evolution has excessively\n"
            "powered up this Pokémon's feelings\n"
            "of kindness. It finishes off its\n"
            "opponents with mercy in its heart."),
        FRONT_PIC(DragoniteMega, 64, 64),
        .frontPicYOffset = 1,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(DragoniteMega, 64, 64),
        .backPicYOffset = 3,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(DragoniteMega),
        ICON(DragoniteMega, 2),
        .footprint = gMonFootprint_Dragonite,
        .formSpeciesIdTable = sDragoniteFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Dragonite),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_MEGANIUM_MEGA] =
    {
        .baseHP        = 80,
        .baseAttack    = 92,
        .baseDefense   = 115,
        .baseSpeed     = 80,
        .baseSpAttack  = 143,
        .baseSpDefense = 115,
        .types = { TYPE_GRASS, TYPE_FAIRY },
        .catchRate = 45,
    #if GEN_LATEST >= GEN_8
        .expYield = 263,
    #elif GEN_LATEST >= GEN_5
        .expYield = 236,
    #else
        .expYield = 208,
    #endif
        .evYield_Defense = 1,
        .evYield_SpDefense = 2,
        .genderRatio = PERCENT_FEMALE(12.5),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = { EGG_GROUP_MONSTER, EGG_GROUP_GRASS },
        .abilities = { ABILITY_MEGA_SOL, ABILITY_NONE, ABILITY_MEGA_SOL },
        .bodyColor = BODY_COLOR_GREEN,
        .speciesName = _("Meganium"),
        .cryId = CRY_MEGANIUM_MEGA,
        .natDexNum = NATIONAL_DEX_MEGANIUM,
        .categoryName = _("Herb"),
        .height = 24,
        .weight = 2010,
        .description = POKEDEX_DESC_STRING(
            "This Pokémon can fire a tremendously\n"
            "powerful Solar Beam from its four\n"
            "flowers. Another name for this is\n"
            "Mega Sol Cannon."),
        FRONT_PIC(MeganiumMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(MeganiumMega, 64, 64),
        .backPicYOffset = 0,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(MeganiumMega),
        ICON(MeganiumMega, 1),
        .footprint = gMonFootprint_Meganium,
        .formSpeciesIdTable = sMeganiumFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Meganium),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_FERALIGATR_MEGA] =
    {
        .baseHP        = 85,
        .baseAttack    = 160,
        .baseDefense   = 125,
        .baseSpeed     = 78,
        .baseSpAttack  = 89,
        .baseSpDefense = 93,
        .types = { TYPE_WATER, TYPE_DRAGON },
        .catchRate = 45,
    #if GEN_LATEST >= GEN_8
        .expYield = 265,
    #elif GEN_LATEST >= GEN_5
        .expYield = 239,
    #else
        .expYield = 210,
    #endif
        .evYield_Attack = 2,
        .evYield_Defense = 1,
        .genderRatio = PERCENT_FEMALE(12.5),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = { EGG_GROUP_MONSTER, EGG_GROUP_WATER_1 },
        .abilities = { ABILITY_DRAGONIZE, ABILITY_NONE, ABILITY_DRAGONIZE },
        .bodyColor = BODY_COLOR_BLUE,
        .speciesName = _("Feraligatr"),
        .cryId = CRY_FERALIGATR_MEGA,
        .natDexNum = NATIONAL_DEX_FERALIGATR,
        .categoryName = _("Double Jaw"),
        .height = 23,
        .weight = 1088,
        .description = POKEDEX_DESC_STRING(
            "With its arms and hoodlike fin, this\n"
            "Pokémon forms a gigantic set of jaws\n"
            "with a bite 10 times as powerful\n"
            "as Mega Feraligatr's actual jaws."),
        FRONT_PIC(FeraligatrMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(FeraligatrMega, 64, 64),
        .backPicYOffset = 0,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(FeraligatrMega),
        ICON(FeraligatrMega, 0),
        .footprint = gMonFootprint_Feraligatr,
        .formSpeciesIdTable = sFeraligatrFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Feraligatr),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_SKARMORY_MEGA] =
    {
        .baseHP        = 65,
        .baseAttack    = 140,
        .baseDefense   = 110,
        .baseSpeed     = 110,
        .baseSpAttack  = 40,
        .baseSpDefense = 100,
        .types = { TYPE_STEEL, TYPE_FLYING },
        .catchRate = 25,
        .expYield = (GEN_LATEST >= GEN_5) ? 163 : 168,
        .evYield_Defense = 2,
        .itemRare = ITEM_METAL_COAT,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 25,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_SLOW,
        .eggGroups = { EGG_GROUP_FLYING, EGG_GROUP_FLYING },
        .abilities = { ABILITY_STALWART, ABILITY_STALWART, ABILITY_STALWART },
        .bodyColor = BODY_COLOR_GRAY,
        .speciesName = _("Skarmory"),
        .cryId = CRY_SKARMORY_MEGA,
        .natDexNum = NATIONAL_DEX_SKARMORY,
        .categoryName = _("Armor Bird"),
        .height = 17,
        .weight = 404,
        .description = POKEDEX_DESC_STRING(
            "Due to the effects of Mega Evolution,\n"
            "its pincers have taken a more\n"
            "diabolical form, ripping anything\n"
            "they pierce to shreds."),
        FRONT_PIC(SkarmoryMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(SkarmoryMega, 64, 64),
        .backPicYOffset = 0,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(SkarmoryMega),
        ICON(SkarmoryMega, 0),
        .footprint = gMonFootprint_Skarmory,
        .formSpeciesIdTable = sSkarmoryFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Skarmory),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_FROSLASS_MEGA] =
    {
        .baseHP        = 70,
        .baseAttack    = 80,
        .baseDefense   = 70,
        .baseSpeed     = 120,
        .baseSpAttack  = 140,
        .baseSpDefense = 100,
        .types = { TYPE_ICE, TYPE_GHOST },
        .catchRate = 75,
        .expYield = (GEN_LATEST >= GEN_5) ? 168 : 187,
        .evYield_Speed = 2,
        .genderRatio = MON_FEMALE,
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = { EGG_GROUP_FAIRY, EGG_GROUP_MINERAL },
        .abilities = { ABILITY_SNOW_WARNING, ABILITY_NONE, ABILITY_SNOW_WARNING },
        .bodyColor = BODY_COLOR_WHITE,
        .speciesName = _("Froslass"),
        .cryId = CRY_FROSLASS_MEGA,
        .natDexNum = NATIONAL_DEX_FROSLASS,
        .categoryName = _("Snow Land"),
        .height = 26,
        .weight = 296,
        .description = POKEDEX_DESC_STRING(
            "This Pokémon can use eerie cold\n"
            "air imbued with ghost energy to\n"
            "freeze even insubstantial things,\n"
            "such as flames or the wind."),
        FRONT_PIC(FroslassMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(FroslassMega, 64, 64),
        .backPicYOffset = 1,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(FroslassMega),
        ICON(FroslassMega, 0),
        .footprint = gMonFootprint_Froslass,
        .formSpeciesIdTable = sFroslassFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Froslass),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_EMBOAR_MEGA] =
    {
        .baseHP        = 110,
        .baseAttack    = 148,
        .baseDefense   = 75,
        .baseSpeed     = 75,
        .baseSpAttack  = 110,
        .baseSpDefense = 110,
        .types = { TYPE_FIRE, TYPE_FIGHTING },
        .catchRate = 45,
        .expYield = (GEN_LATEST >= GEN_8) ? 264 : 238,
        .evYield_Attack = 3,
        .genderRatio = PERCENT_FEMALE(12.5),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = { EGG_GROUP_FIELD, EGG_GROUP_FIELD },
        .abilities = { ABILITY_MOLD_BREAKER, ABILITY_NONE, ABILITY_MOLD_BREAKER },
        .bodyColor = BODY_COLOR_RED,
        .noFlip = TRUE,
        .speciesName = _("Emboar"),
        .cryId = CRY_EMBOAR_MEGA,
        .natDexNum = NATIONAL_DEX_EMBOAR,
        .categoryName = _("Fire Pig"),
        .height = 18,
        .weight = 1803,
        .description = POKEDEX_DESC_STRING(
            "Brandishing a blazing flame\n"
            "shaped like a serpentine spear,\n"
            "it rushes in to save its\n"
            "imperiled allies."),
        FRONT_PIC(EmboarMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(EmboarMega, 64, 64),
        .backPicYOffset = 1,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(EmboarMega),
        ICON(EmboarMega, 5),
        .footprint = gMonFootprint_Emboar,
        .formSpeciesIdTable = sEmboarFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Emboar),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_EXCADRILL_MEGA] =
    {
        .baseHP        = 110,
        .baseAttack    = 165,
        .baseDefense   = 100,
        .baseSpeed     = 103,
        .baseSpAttack  = 65,
        .baseSpDefense = 65,
        .types = { TYPE_GROUND, TYPE_STEEL },
        .catchRate = 60,
        .expYield = 178,
        .evYield_Attack = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = { EGG_GROUP_FIELD, EGG_GROUP_FIELD },
        .abilities = { ABILITY_PIERCING_DRILL, ABILITY_PIERCING_DRILL, ABILITY_PIERCING_DRILL },
        .bodyColor = BODY_COLOR_GRAY,
        .noFlip = TRUE,
        .speciesName = _("Excadrill"),
        .cryId = CRY_EXCADRILL_MEGA,
        .natDexNum = NATIONAL_DEX_EXCADRILL,
        .categoryName = _("Subterrene"),
        .height = 9,
        .weight = 600,
        .description = POKEDEX_DESC_STRING(
            "If this Pokémon brings its arms and\n"
            "head together to form a streamlined\n"
            "shape and spins at high speeds,\n"
            "it can destroy anything."),
        FRONT_PIC(ExcadrillMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(ExcadrillMega, 64, 64),
        .backPicYOffset = 0,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(ExcadrillMega),
        ICON(ExcadrillMega, 0),
        .footprint = gMonFootprint_Excadrill,
        .formSpeciesIdTable = sExcadrillFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Excadrill),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_SCOLIPEDE_MEGA] =
    {
        .baseHP        = 60,
        .baseAttack    = 140,
        .baseDefense   = 149,
        .baseSpeed     = 62,
        .baseSpAttack  = 75,
        .baseSpDefense = 99,
        .types = { TYPE_BUG, TYPE_POISON },
        .catchRate = 45,
    #if GEN_LATEST >= GEN_8
        .expYield = 243,
    #elif GEN_LATEST >= GEN_7
        .expYield = 218,
    #else
        .expYield = 214,
    #endif
        .evYield_Speed = 3,
        .itemRare = ITEM_POISON_BARB,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = { EGG_GROUP_BUG, EGG_GROUP_BUG },
        .abilities = { ABILITY_POISON_POINT, ABILITY_SWARM, ABILITY_SPEED_BOOST },
        .bodyColor = BODY_COLOR_RED,
        .speciesName = _("Scolipede"),
        .cryId = CRY_SCOLIPEDE_MEGA,
        .natDexNum = NATIONAL_DEX_SCOLIPEDE,
        .categoryName = _("Megapede"),
        .height = 32,
        .weight = 2305,
        .description = POKEDEX_DESC_STRING(
            "Its deadly venom gives off a faint\n"
            "glow. The venom affects Scolipede's\n"
            "mind, honing its viciousness."),
        FRONT_PIC(ScolipedeMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(ScolipedeMega, 64, 64),
        .backPicYOffset = 0,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(ScolipedeMega),
        ICON(ScolipedeMega, 0),
        .footprint = gMonFootprint_Scolipede,
        .formSpeciesIdTable = sScolipedeFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Scolipede),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_SCRAFTY_MEGA] =
    {
        .baseHP        = 65,
        .baseAttack    = 130,
        .baseDefense   = 135,
        .baseSpeed     = 68,
        .baseSpAttack  = 55,
        .baseSpDefense = 135,
        .types = { TYPE_DARK, TYPE_FIGHTING },
        .catchRate = 90,
        .expYield = 171,
        .evYield_Defense = 1,
        .evYield_SpDefense = 1,
        .itemRare = ITEM_SHED_SHELL,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 15,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = { EGG_GROUP_FIELD, EGG_GROUP_DRAGON },
        .abilities = { ABILITY_SHED_SKIN, ABILITY_MOXIE, ABILITY_INTIMIDATE },
        .bodyColor = BODY_COLOR_RED,
        .speciesName = _("Scrafty"),
        .cryId = CRY_SCRAFTY_MEGA,.natDexNum = NATIONAL_DEX_SCRAFTY,
        .categoryName = _("Hoodlum"),
        .height = 11,
        .weight = 310,
        .description = POKEDEX_DESC_STRING(
            "Mega Evolution has caused Scrafty's\n"
            "shed skin to turn white, growing\n"
            "tough and supple. Of course, this\n"
            "Pokémon is still as feisty as ever."),
        FRONT_PIC(ScraftyMega, 64, 64),
        .frontPicYOffset = 1,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(ScraftyMega, 64, 64),
        .backPicYOffset = 5,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(ScraftyMega),
        ICON(ScraftyMega, 0),
        .footprint = gMonFootprint_Scrafty,
        .formSpeciesIdTable = sScraftyFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Scrafty),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_EELEKTROSS_MEGA] =
    {
        .baseHP        = 85,
        .baseAttack    = 145,
        .baseDefense   = 80,
        .baseSpeed     = 80,
        .baseSpAttack  = 135,
        .baseSpDefense = 90,
        .types = { TYPE_ELECTRIC, TYPE_ELECTRIC },
        .catchRate = 30,
        .expYield = (GEN_LATEST >= GEN_8) ? 258 : 232,
        .evYield_Attack = 3,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_SLOW,
        .eggGroups = { EGG_GROUP_AMORPHOUS, EGG_GROUP_AMORPHOUS },
        .abilities = { ABILITY_LEVITATE, ABILITY_NONE, ABILITY_NONE },
        .bodyColor = BODY_COLOR_BLUE,
        .speciesName = _("Eelektross"),
        .cryId = CRY_EELEKTROSS_MEGA,
        .natDexNum = NATIONAL_DEX_EELEKTROSS,
        .categoryName = _("EleFish"),
        .height = 30,
        .weight = 1800,
        .description = POKEDEX_DESC_STRING(
            "It now generates 10 times the\n"
            "electricity it did before Mega\n"
            "Evolving. It discharges this energy\n"
            "from its false Eelektrik made of mucus."),
        FRONT_PIC(EelektrossMega, 64, 64),
        .frontPicYOffset = 1,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(EelektrossMega, 64, 64),
        .backPicYOffset = 0,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(EelektrossMega),
        ICON(EelektrossMega, 0),
        .footprint = gMonFootprint_Eelektross,
        .formSpeciesIdTable = sEelektrossFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Eelektross),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_CHANDELURE_MEGA] =
    {
        .baseHP        = 60,
        .baseAttack    = 75,
        .baseDefense   = 110,
        .baseSpeed     = 90,
        .baseSpAttack  = 175,
        .baseSpDefense = 110,
        .types = { TYPE_GHOST, TYPE_FIRE },
        .catchRate = 45,
        .expYield = (GEN_LATEST >= GEN_8) ? 260 : 234,
        .evYield_SpAttack = 3,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = { EGG_GROUP_AMORPHOUS, EGG_GROUP_AMORPHOUS },
        .abilities = { ABILITY_INFILTRATOR, ABILITY_INFILTRATOR, ABILITY_INFILTRATOR },
        .bodyColor = BODY_COLOR_BLACK,
        .speciesName = _("Chandelure"),
        .cryId = CRY_CHANDELURE_MEGA,
        .natDexNum = NATIONAL_DEX_CHANDELURE,
        .categoryName = _("Luring"),
        .height = 25,
        .weight = 696,
        .description = POKEDEX_DESC_STRING(
            "One of its eyes is a window linking\n"
            "our world with the afterlife.\n"
            "This Pokémon draws in hatred and\n"
            "converts it into power."),
        FRONT_PIC(ChandelureMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(ChandelureMega, 64, 64),
        .backPicYOffset = 0,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(ChandelureMega),
        ICON(ChandelureMega, 0),
        .footprint = gMonFootprint_Chandelure,
        .formSpeciesIdTable = sChandelureFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Chandelure),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_CHESNAUGHT_MEGA] =
    {
        .baseHP        = 88,
        .baseAttack    = 137,
        .baseDefense   = 172,
        .baseSpeed     = 44,
        .baseSpAttack  = 74,
        .baseSpDefense = 115,
        .types = { TYPE_GRASS, TYPE_FIGHTING },
        .catchRate = 45,
        .expYield = (GEN_LATEST >= GEN_8) ? 265 : 239,
        .evYield_Defense = 3,
        .genderRatio = PERCENT_FEMALE(12.5),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = { EGG_GROUP_FIELD, EGG_GROUP_FIELD },
        .abilities = { ABILITY_BULLETPROOF, ABILITY_NONE, ABILITY_BULLETPROOF },
        .bodyColor = BODY_COLOR_GREEN,
        .speciesName = _("Chesnaught"),
        .cryId = CRY_CHESNAUGHT_MEGA,.natDexNum = NATIONAL_DEX_CHESNAUGHT,
        .categoryName = _("Spiny Armor"),
        .height = 16,
        .weight = 900,
        .description = POKEDEX_DESC_STRING(
            "It has fortified armor and a\n"
            "will to defend at all costs.\n"
            "Both are absurdly strong."),
        FRONT_PIC(ChesnaughtMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(ChesnaughtMega, 64, 64),
        .backPicYOffset = 0,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(ChesnaughtMega),
        ICON(ChesnaughtMega, 0),
        .footprint = gMonFootprint_Chesnaught,
        .formSpeciesIdTable = sChesnaughtFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Chesnaught),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_DELPHOX_MEGA] =
    {
        .baseHP        = 75,
        .baseAttack    = 69,
        .baseDefense   = 72,
        .baseSpeed     = 134,
        .baseSpAttack  = 159,
        .baseSpDefense = 125,
        .types = { TYPE_FIRE, TYPE_PSYCHIC },
        .catchRate = 45,
        .expYield = (GEN_LATEST >= GEN_8) ? 267 : 240,
        .evYield_SpAttack = 3,
        .genderRatio = PERCENT_FEMALE(12.5),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = { EGG_GROUP_FIELD, EGG_GROUP_FIELD },
        .abilities = { ABILITY_LEVITATE, ABILITY_NONE, ABILITY_LEVITATE },
        .bodyColor = BODY_COLOR_RED,
        .speciesName = _("Delphox"),
        .cryId = CRY_DELPHOX_MEGA,
        .natDexNum = NATIONAL_DEX_DELPHOX,
        .categoryName = _("Fox"),
        .height = 15,
        .weight = 390,
        .description = POKEDEX_DESC_STRING(
            "It wields flaming branches to\n"
            "dazzle its opponents before\n"
            "incinerating them with a\n"
            "huge fireball."),
        FRONT_PIC(DelphoxMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(DelphoxMega, 64, 64),
        .backPicYOffset = 0,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(DelphoxMega),
        ICON(DelphoxMega, 0),
        .footprint = gMonFootprint_Delphox,
        .formSpeciesIdTable = sDelphoxFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Delphox),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_GRENINJA_MEGA] =
    {
        .baseHP        = 72,
        .baseAttack    = 125,
        .baseDefense   = 77,
        .baseSpeed     = 142,
        .baseSpAttack  = 133,
        .baseSpDefense = 81,
        .types = { TYPE_WATER, TYPE_DARK },
        .catchRate = 45,
        .expYield = (GEN_LATEST >= GEN_8) ? 265 : 239,
        .evYield_Speed = 3,
        .genderRatio = PERCENT_FEMALE(12.5),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = { EGG_GROUP_WATER_1, EGG_GROUP_WATER_1 },
        .abilities = { ABILITY_PROTEAN, ABILITY_NONE, ABILITY_PROTEAN },
        .bodyColor = BODY_COLOR_BLUE,
        .noFlip = TRUE,
        .speciesName = _("Greninja"),
        .cryId = CRY_GRENINJA_MEGA,
        .natDexNum = NATIONAL_DEX_GRENINJA,
        .categoryName = _("Ninja"),
        .height = 15,
        .weight = 400,
        .description = POKEDEX_DESC_STRING(
            "This Pokémon spins a giant\n"
            "shuriken at high speed to make it\n"
            "float, then clings to it upside\n"
            "down to catch opponents unawares."),
        FRONT_PIC(GreninjaMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(GreninjaMega, 64, 64),
        .backPicYOffset = 0,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(GreninjaMega),
        ICON(GreninjaMega, 0),
        .footprint = gMonFootprint_Greninja,
        .formSpeciesIdTable = sGreninjaFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Greninja),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_PYROAR_MEGA] =
    {
        .baseHP        = 86,
        .baseAttack    = 88,
        .baseDefense   = 92,
        .baseSpeed     = 126,
        .baseSpAttack  = 129,
        .baseSpDefense = 86,
        .types = { TYPE_FIRE, TYPE_NORMAL },
        .catchRate = 65,
        .expYield = 177,
        .evYield_SpAttack = 2,
        .genderRatio = PERCENT_FEMALE(87.5),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = { EGG_GROUP_FIELD, EGG_GROUP_FIELD },
        .abilities = { ABILITY_RIVALRY, ABILITY_UNNERVE, ABILITY_MOXIE },
        .bodyColor = BODY_COLOR_BROWN,
        .speciesName = _("Pyroar"),
        .cryId = CRY_PYROAR_MEGA,
        .natDexNum = NATIONAL_DEX_PYROAR,
        .categoryName = _("Royal"),
        .height = 15,
        .weight = 933,
        .description = POKEDEX_DESC_STRING(
            "This Pokémon spews flames hotter\n"
            "than 18,000 degrees Fahrenheit.\n"
            "It swings around its grand, blazing\n"
            "mane as it protects its allies."),
        FRONT_PIC(PyroarMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(PyroarMega, 64, 64),
        .backPicYOffset = 0,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(PyroarMega),
        ICON(PyroarMega, 0),
        .footprint = gMonFootprint_Pyroar,
        .formSpeciesIdTable = sPyroarFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Pyroar),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_MALAMAR_MEGA] =
    {
        .baseHP        = 86,
        .baseAttack    = 102,
        .baseDefense   = 88,
        .baseSpeed     = 88,
        .baseSpAttack  = 98,
        .baseSpDefense = 120,
        .types = { TYPE_DARK, TYPE_PSYCHIC },
        .catchRate = 80,
        .expYield = 169,
        .evYield_Attack = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = { EGG_GROUP_WATER_1, EGG_GROUP_WATER_2 },
        .abilities = { ABILITY_CONTRARY, ABILITY_SUCTION_CUPS, ABILITY_INFILTRATOR },
        .bodyColor = BODY_COLOR_BLUE,
        .speciesName = _("Malamar"),
        .cryId = CRY_MALAMAR_MEGA,
        .natDexNum = NATIONAL_DEX_MALAMAR,
        .categoryName = _("Overturning"),
        .height = 29,
        .weight = 698,
        .description = POKEDEX_DESC_STRING(
            "It uses its colorful lights to\n"
            "overwrite the personality and\n"
            "memories of others-and to\n"
            "control them."),
        FRONT_PIC(MalamarMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(MalamarMega, 64, 64),
        .backPicYOffset = 8,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(MalamarMega),
        ICON(MalamarMega, 0),
        .footprint = gMonFootprint_Malamar,
        .formSpeciesIdTable = sMalamarFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Malamar),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_DRAGALGE_MEGA] =
    {
        .baseHP        = 65,
        .baseAttack    = 85,
        .baseDefense   = 105,
        .baseSpeed     = 44,
        .baseSpAttack  = 132,
        .baseSpDefense = 163,
        .types = { TYPE_POISON, TYPE_DRAGON },
        .catchRate = 55,
        .expYield = 173,
        .evYield_SpDefense = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = { EGG_GROUP_WATER_1, EGG_GROUP_DRAGON },
        .abilities = { ABILITY_POISON_POINT, ABILITY_POISON_TOUCH, ABILITY_ADAPTABILITY },
        .bodyColor = BODY_COLOR_BROWN,
        .speciesName = _("Dragalge"),
        .cryId = CRY_DRAGALGE_MEGA,
        .natDexNum = NATIONAL_DEX_DRAGALGE,
        .categoryName = _("Mock Kelp"),
        .height = 21,
        .weight = 1003,
        .description = POKEDEX_DESC_STRING(
            "It spits a liquid that causes the\n"
            "regenerative power of cells to run\n"
            "wild. The liquid is deadly poison\n"
            "to everything other than itself."),
        FRONT_PIC(DragalgeMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(DragalgeMega, 64, 64),
        .backPicYOffset = 0,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(DragalgeMega),
        ICON(DragalgeMega, 0),
        .footprint = gMonFootprint_Dragalge,
        .formSpeciesIdTable = sDragalgeFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Dragalge),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_HAWLUCHA_MEGA] =
    {
        .baseHP        = 78,
        .baseAttack    = 137,
        .baseDefense   = 100,
        .baseSpeed     = 118,
        .baseSpAttack  = 74,
        .baseSpDefense = 93,
        .types = { TYPE_FIGHTING, TYPE_FLYING },
        .catchRate = 100,
        .expYield = 175,
        .evYield_Attack = 2,
        .itemRare = ITEM_KINGS_ROCK,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = { EGG_GROUP_FLYING, EGG_GROUP_HUMAN_LIKE },
        .abilities = { ABILITY_NO_GUARD, ABILITY_NO_GUARD, ABILITY_NO_GUARD },
        .bodyColor = BODY_COLOR_GREEN,
        .speciesName = _("Hawlucha"),
        .cryId = CRY_HAWLUCHA_MEGA,
        .natDexNum = NATIONAL_DEX_HAWLUCHA,
        .categoryName = _("Wrestling"),
        .height = 10,
        .weight = 250,
        .description = POKEDEX_DESC_STRING(
            "Mega Evolution has pumped up all\n"
            "its muscles. Hawlucha flexes to\n"
            "show off its strength."),
        FRONT_PIC(HawluchaMega, 64, 64),
        .frontPicYOffset = 2,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(HawluchaMega, 64, 64),
        .backPicYOffset = 4,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(HawluchaMega),
        ICON(HawluchaMega, 0),
        .footprint = gMonFootprint_Hawlucha,
        .formSpeciesIdTable = sHawluchaFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Hawlucha),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_FLOETTE_MEGA] =
    {
        .baseHP        = 74,
        .baseAttack    = 85,
        .baseDefense   = 87,
        .baseSpeed     = 102,
        .baseSpAttack  = 155,
        .baseSpDefense = 148,
        .types = { TYPE_FAIRY, TYPE_FAIRY },
        .catchRate = 120,
        .expYield = 1,
        .evYield_SpDefense = 2,
        .genderRatio = MON_FEMALE,
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = { EGG_GROUP_NO_EGGS_DISCOVERED, EGG_GROUP_NO_EGGS_DISCOVERED },
        .abilities = { ABILITY_FAIRY_AURA, ABILITY_NONE, ABILITY_FAIRY_AURA },
        .bodyColor = BODY_COLOR_WHITE,
        .speciesName = _("Floette"),
        .cryId = CRY_FLOETTE_MEGA,
        .natDexNum = NATIONAL_DEX_FLOETTE,
        .categoryName = _("Single Bloom"),
        // height
        // weight
        .description = POKEDEX_DESC_STRING(
            "The Eternal Flower has absorbed\n"
            "all the energy from Mega\n"
            "Evolution. The flower now attacks\n"
            "enemies on its own."),
        FRONT_PIC(FloetteMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(FloetteMega, 64, 64),
        .backPicYOffset = 6,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(FloetteMega),
        ICON(FloetteMega, 0),
        .footprint = gMonFootprint_Floette,
        .formSpeciesIdTable = sFloetteFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(FloetteEternal),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_BARBARACLE_MEGA] =
    {
        .baseHP        = 72,
        .baseAttack    = 140,
        .baseDefense   = 130,
        .baseSpeed     = 88,
        .baseSpAttack  = 64,
        .baseSpDefense = 106,
        .types = { TYPE_ROCK, TYPE_FIGHTING },
        .catchRate = 45,
        .expYield = 175,
        .evYield_Attack = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = { EGG_GROUP_WATER_3, EGG_GROUP_WATER_3 },
        .abilities = { ABILITY_TOUGH_CLAWS, ABILITY_SNIPER, ABILITY_PICKPOCKET },
        .bodyColor = BODY_COLOR_BROWN,
        .noFlip = TRUE,
        .speciesName = _("Barbaracle"),
        .cryId = CRY_BARBARACLE_MEGA,
        .natDexNum = NATIONAL_DEX_BARBARACLE,
        .categoryName = _("Collective"),
        .height = 22,
        .weight = 1000,
        .description = POKEDEX_DESC_STRING(
            "It uses its many arms to toy\n"
            "with its opponents. This\n"
            "keeps the head extremely busy."),
        FRONT_PIC(BarbaracleMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(BarbaracleMega, 64, 64),
        .backPicYOffset = 0,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(BarbaracleMega),
        ICON(BarbaracleMega, 2),
        .footprint = gMonFootprint_Barbaracle,
        .formSpeciesIdTable = sBarbaracleFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Barbaracle),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_ZYGARDE_MEGA] =
    {
        .baseHP        = 216,
        .baseAttack    = 70,
        .baseDefense   = 91,
        .baseSpeed     = 100,
        .baseSpAttack  = 216,
        .baseSpDefense = 85,
        .types = { TYPE_DRAGON, TYPE_GROUND },
        .catchRate = 3,
        .expYield = (GEN_LATEST >= GEN_8) ? 354 : 319,
        .evYield_HP = 3,
        .genderRatio = MON_GENDERLESS,
        .eggCycles = 120,
        .friendship = 0,
        .growthRate = GROWTH_SLOW,
        .eggGroups = { EGG_GROUP_NO_EGGS_DISCOVERED, EGG_GROUP_NO_EGGS_DISCOVERED },
        .abilities = { ABILITY_AURA_BREAK, ABILITY_NONE, ABILITY_NONE },
        .bodyColor = BODY_COLOR_BLACK,
        .noFlip = TRUE,
        .speciesName = _("Zygarde"),
        .cryId = CRY_ZYGARDE_MEGA,
        .natDexNum = NATIONAL_DEX_ZYGARDE,
        .categoryName = _("Order"),
        .height = 77,
        .weight = 6100,
        .description = POKEDEX_DESC_STRING(
            "In response to people's emotions\n"
            "during an unprecedented crisis,\n"
            "Zygarde Mega Evolves and calms the\n"
            "situation with its unmatched power."),
        FRONT_PIC(ZygardeMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(ZygardeMega, 64, 64),
        .backPicYOffset = 15,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(ZygardeMega),
        ICON(ZygardeMega, 1),
        .footprint = gMonFootprint_Zygarde,
        .formSpeciesIdTable = sZygardeFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        .isLegendary = TRUE,
        LEARNSETS(Zygarde),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_DRAMPA_MEGA] =
    {
        .baseHP        = 78,
        .baseAttack    = 85,
        .baseDefense   = 110,
        .baseSpeed     = 36,
        .baseSpAttack  = 160,
        .baseSpDefense = 116,
        .types = { TYPE_NORMAL, TYPE_DRAGON },
        .catchRate = 70,
        .expYield = 170,
        .evYield_SpAttack = 2,
        .itemRare = ITEM_PERSIM_BERRY,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = { EGG_GROUP_MONSTER, EGG_GROUP_DRAGON },
        .abilities = { ABILITY_BERSERK, ABILITY_BERSERK, ABILITY_BERSERK },
        .bodyColor = BODY_COLOR_WHITE,
        .speciesName = _("Drampa"),
        .cryId = CRY_DRAMPA_MEGA,
        .natDexNum = NATIONAL_DEX_DRAMPA,
        .categoryName = _("Imposing"),
        .height = 3,
        .weight = 2405,
        .description = POKEDEX_DESC_STRING(
            "Drampa's cells have been\n"
            "invigorated, allowing it to regain\n"
            "its youth. It manipulates the\n"
            "atmosphere to summon storms."),
        FRONT_PIC(DrampaMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(DrampaMega, 64, 64),
        .backPicYOffset = 0,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(DrampaMega),
        ICON(DrampaMega, 0),
        .footprint = gMonFootprint_Drampa,
        .formSpeciesIdTable = sDrampaFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Drampa),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_FALINKS_MEGA] =
    {
        .baseHP        = 65,
        .baseAttack    = 135,
        .baseDefense   = 135,
        .baseSpeed     = 100,
        .baseSpAttack  = 70,
        .baseSpDefense = 65,
        .types = { TYPE_FIGHTING, TYPE_FIGHTING },
        .catchRate = 45,
        .expYield = 165,
        .evYield_Attack = 2,
        .evYield_SpDefense = 1,
        .genderRatio = MON_GENDERLESS,
        .eggCycles = 25,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = { EGG_GROUP_FAIRY, EGG_GROUP_MINERAL },
        .abilities = { ABILITY_BATTLE_ARMOR, ABILITY_NONE, ABILITY_DEFIANT },
        .bodyColor = BODY_COLOR_YELLOW,
        .speciesName = _("Falinks"),
        .cryId = CRY_FALINKS_MEGA,.natDexNum = NATIONAL_DEX_FALINKS,
        .categoryName = _("Formation"),
        .height = 16,
        .weight = 990,
        .description = POKEDEX_DESC_STRING(
            "Mega Falinks has taken on the\n"
            "ultimate battle formation, which\n"
            "can be achieved only if the troopers\n"
            "and brass have the strongest of bonds."),
        FRONT_PIC(FalinksMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(FalinksMega, 64, 64),
        .backPicYOffset = 0,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(FalinksMega),
        ICON(FalinksMega, 0),
        .footprint = gMonFootprint_Falinks,
        .formSpeciesIdTable = sFalinksFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Falinks),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_HEATRAN_MEGA] =
    {
        .baseHP        = 91,
        .baseAttack    = 120,
        .baseDefense   = 106,
        .baseSpeed     = 67,
        .baseSpAttack  = 175,
        .baseSpDefense = 141,
        .types = { TYPE_FIRE, TYPE_STEEL },
        .catchRate = 3,
    #if GEN_LATEST >= GEN_8
        .expYield = 300,
    #elif GEN_LATEST >= GEN_5
        .expYield = 270,
    #else
        .expYield = 215,
    #endif
        .evYield_SpAttack = 3,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 10,
        .friendship = 100,
        .growthRate = GROWTH_SLOW,
        .eggGroups = { EGG_GROUP_NO_EGGS_DISCOVERED, EGG_GROUP_NO_EGGS_DISCOVERED },
        .abilities = { ABILITY_FLASH_FIRE, ABILITY_NONE, ABILITY_FLAME_BODY },
        .bodyColor = BODY_COLOR_BROWN,
        .speciesName = _("Heatran"),
        .cryId = CRY_HEATRAN_MEGA,
        .natDexNum = NATIONAL_DEX_HEATRAN,
        .categoryName = _("Lava Dome"),
        .height = 28,
        .weight = 5700,
        .description = POKEDEX_DESC_STRING(
            "It's said that if it goes all out, it\n"
            "can heat its body up to temperatures\n"
            "over 1.8 million degrees Fahrenheit.\n"
            "This heat keeps enemies at bay."),
        FRONT_PIC(HeatranMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(HeatranMega, 64, 64),
        .backPicYOffset = 15,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(HeatranMega),
        ICON(HeatranMega, 0),
        .footprint = gMonFootprint_Heatran,
        .formSpeciesIdTable = sHeatranFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        .isLegendary = TRUE,
        LEARNSETS(Heatran),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_DARKRAI_MEGA] =
    {
        .baseHP        = 70,
        .baseAttack    = 120,
        .baseDefense   = 130,
        .baseSpeed     = 85,
        .baseSpAttack  = 165,
        .baseSpDefense = 130,
        .types = { TYPE_DARK, TYPE_DARK },
        .catchRate = 3,
    #if GEN_LATEST >= GEN_8
        .expYield = 300,
    #elif GEN_LATEST >= GEN_5
        .expYield = 270,
    #else
        .expYield = 210,
    #endif
        .evYield_Speed = 1,
        .evYield_SpAttack = 2,
        .genderRatio = MON_GENDERLESS,
        .eggCycles = 120,
        .friendship = 0,
        .growthRate = GROWTH_SLOW,
        .eggGroups = { EGG_GROUP_NO_EGGS_DISCOVERED, EGG_GROUP_NO_EGGS_DISCOVERED },
        .abilities = { ABILITY_BAD_DREAMS, ABILITY_NONE, ABILITY_NONE },
        .bodyColor = BODY_COLOR_BLACK,
        .speciesName = _("Darkrai"),
        .cryId = CRY_DARKRAI_MEGA,
        .natDexNum = NATIONAL_DEX_DARKRAI,
        .categoryName = _("Pitch-Black"),
        .height = 30,
        .weight = 2400,
        .description = POKEDEX_DESC_STRING(
            "Its dark power blocks out the sun,\n"
            "plunging the surrounding area into\n"
            "darkness. There is no escaping its\n"
            "evil eye."),
        FRONT_PIC(DarkraiMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(DarkraiMega, 64, 64),
        .backPicYOffset = 3,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(DarkraiMega),
        ICON(DarkraiMega, 0),
        .footprint = gMonFootprint_Darkrai,
        .formSpeciesIdTable = sDarkraiFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        .isMythical = TRUE,
        LEARNSETS(Darkrai),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_ZERAORA_MEGA] =
    {
        .baseHP        = 88,
        .baseAttack    = 157,
        .baseDefense   = 75,
        .baseSpeed     = 153,
        .baseSpAttack  = 147,
        .baseSpDefense = 80,
        .types = { TYPE_ELECTRIC, TYPE_ELECTRIC },
        .catchRate = 3,
        .expYield = (GEN_LATEST >= GEN_8) ? 300 : 270,
        .evYield_Speed = 3,
        .genderRatio = MON_GENDERLESS,
        .eggCycles = 120,
        .friendship = 0,
        .growthRate = GROWTH_SLOW,
        .eggGroups = { EGG_GROUP_NO_EGGS_DISCOVERED, EGG_GROUP_NO_EGGS_DISCOVERED },
        .abilities = { ABILITY_VOLT_ABSORB, ABILITY_NONE, ABILITY_NONE },
        .bodyColor = BODY_COLOR_YELLOW,
        .speciesName = _("Zeraora"),
        .cryId = CRY_ZERAORA_MEGA,
        .natDexNum = NATIONAL_DEX_ZERAORA,
        .categoryName = _("Thunderclap"),
        .height = 15,
        .weight = 445,
        .description = POKEDEX_DESC_STRING(
            "It stores up 10 lightning strikes' worth\n"
            "of electricity. When it stops limiting\n"
            "itself, it's in the strongest class of\n"
            "electric Pokémon."),
        FRONT_PIC(ZeraoraMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(ZeraoraMega, 64, 64),
        .backPicYOffset = 9,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(ZeraoraMega),
        ICON(ZeraoraMega, 0),
        .footprint = gMonFootprint_Zeraora,
        .formSpeciesIdTable = sZeraoraFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        .isMythical = TRUE,
        LEARNSETS(Zeraora),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_RAICHU_MEGA_X] =
    {
        .baseHP        = 60,
        .baseAttack    = 135,
        .baseDefense   = 95,
        .baseSpeed     = 110,
        .baseSpAttack  = 90,
        .baseSpDefense = 95,
        .types = { TYPE_ELECTRIC, TYPE_ELECTRIC },
        .catchRate = 75,
        .expYield = 218,
        .evYield_Speed = 3,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 10,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = { EGG_GROUP_FIELD, EGG_GROUP_FAIRY },
        .abilities = { ABILITY_STATIC, ABILITY_NONE, ABILITY_LIGHTNING_ROD },
        .bodyColor = BODY_COLOR_YELLOW,
        .speciesName = _("Raichu"),
        .cryId = CRY_RAICHU_MEGA_X,
        .natDexNum = NATIONAL_DEX_RAICHU,
        .categoryName = _("Mouse"),
        .height = 12,
        .weight = 380,
        .description = POKEDEX_DESC_STRING(
            "It resembles an X as it flies through\n"
            "the air with 50 million volts of\n"
            "electricity sparking from its ears and\n"
            "forked tail."),
        FRONT_PIC(RaichuMegaX, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(RaichuMegaX, 64, 64),
        .backPicYOffset = 0,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(RaichuMegaX),
        ICON(RaichuMegaX, 0),
        .footprint = gMonFootprint_Raichu,
        .formSpeciesIdTable = sRaichuFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Raichu),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_RAICHU_MEGA_Y] =
    {
        .baseHP        = 60,
        .baseAttack    = 100,
        .baseDefense   = 55,
        .baseSpeed     = 130,
        .baseSpAttack  = 160,
        .baseSpDefense = 80,
        .types = { TYPE_ELECTRIC, TYPE_ELECTRIC },
        .catchRate = 75,
        .expYield = 218,
        .evYield_Speed = 3,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 10,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = { EGG_GROUP_FIELD, EGG_GROUP_FAIRY },
        .abilities = { ABILITY_STATIC, ABILITY_NONE, ABILITY_LIGHTNING_ROD },
        .bodyColor = BODY_COLOR_YELLOW,
        .speciesName = _("Raichu"),
        .cryId = CRY_RAICHU_MEGA_Y,
        .natDexNum = NATIONAL_DEX_RAICHU,
        .categoryName = _("Mouse"),
        .height = 10,
        .weight = 260,
        .description = POKEDEX_DESC_STRING(
            "It fires bolts of electricity from the\n"
            "tip of its tail and from the spiky tufts\n"
            "of fur growing out of its temples. This\n"
            "electricity forms the letter Y."),
        FRONT_PIC(RaichuMegaY, 64, 64),
        .frontPicYOffset = 4,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(RaichuMegaY, 64, 64),
        .backPicYOffset = 2,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(RaichuMegaY),
        ICON(RaichuMegaY, 0),
        .footprint = gMonFootprint_Raichu,
        .formSpeciesIdTable = sRaichuFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Raichu),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_CHIMECHO_MEGA] =
    {
        .baseHP        = 75,
        .baseAttack    = 50,
        .baseDefense   = 110,
        .baseSpeed     = 65,
        .baseSpAttack  = 135,
        .baseSpDefense = 120,
        .types = { TYPE_PSYCHIC, TYPE_STEEL },
        .catchRate = 45,
    #if GEN_LATEST >= GEN_7
        .expYield = 159,
    #elif GEN_LATEST >= GEN_5
        .expYield = 149,
    #else
        .expYield = 147,
    #endif
        .evYield_SpAttack = 1,
        .evYield_SpDefense = 1,
        .itemRare = ITEM_CLEANSE_TAG,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 25,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_FAST,
        .eggGroups = { EGG_GROUP_AMORPHOUS, EGG_GROUP_AMORPHOUS },
        .abilities = { ABILITY_LEVITATE, ABILITY_NONE, ABILITY_NONE },
        .bodyColor = BODY_COLOR_BLUE,
        .speciesName = _("Chimecho"),
        .cryId = CRY_CHIMECHO_MEGA,
        .natDexNum = NATIONAL_DEX_CHIMECHO,
        .categoryName = _("Wind Chime"),
        .height = 12,
        .weight = 80,
        .description = POKEDEX_DESC_STRING(
            "It vibrates its entire body to attack in\n"
            "all directions using sound waves. Any who\n"
            "hear the vibrations get so confused, they\n"
            "forget who they are."),
        FRONT_PIC(ChimechoMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(ChimechoMega, 64, 64),
        .backPicYOffset = 0,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(ChimechoMega),
        ICON(ChimechoMega, 0),
        .footprint = gMonFootprint_Chimecho,
        .formSpeciesIdTable = sChimechoFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Chimecho),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_ABSOL_MEGA_Z] =
    {
        .baseHP        = 65,
        .baseAttack    = 154,
        .baseDefense   = 60,
        .baseSpeed     = 151,
        .baseSpAttack  = 75,
        .baseSpDefense = 60,
        .types = { TYPE_DARK, TYPE_GHOST },
        .catchRate = 30,
        .expYield = 198,
        .evYield_Attack = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 25,
        .friendship = 35,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = { EGG_GROUP_FIELD, EGG_GROUP_FIELD },
        .abilities = { ABILITY_MAGIC_BOUNCE, ABILITY_MAGIC_BOUNCE, ABILITY_MAGIC_BOUNCE },
        .bodyColor = BODY_COLOR_WHITE,
        .noFlip = TRUE,
        .speciesName = _("Absol"),
        .cryId = CRY_ABSOL_MEGA_Z,
        .natDexNum = NATIONAL_DEX_ABSOL,
        .categoryName = _("Disaster"),
        .height = 12,
        .weight = 490,
        .description = POKEDEX_DESC_STRING(
            "Using fur that it has made into sharp,\n"
            "clawlike shapes, it cuts down foes with\n"
            "a single blow. This is an act of kindness\n"
            "to keep them from suffering."),
        FRONT_PIC(AbsolMegaZ, 64, 64),
        .frontPicYOffset = 1,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(AbsolMegaZ, 64, 64),
        .backPicYOffset = 0,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(AbsolMegaZ),
        ICON(AbsolMegaZ, 0),
        .footprint = gMonFootprint_Absol,
        .formSpeciesIdTable = sAbsolFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Absol),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_STARAPTOR_MEGA] =
    {
        .baseHP        = 85,
        .baseAttack    = 140,
        .baseDefense   = 100,
        .baseSpeed     = 110,
        .baseSpAttack  = 60,
        .baseSpDefense = 90,
        .types = { TYPE_FIGHTING, TYPE_FLYING },
        .catchRate = 45,
    #if GEN_LATEST >= GEN_8
        .expYield = 243,
    #elif GEN_LATEST >= GEN_7
        .expYield = 218,
    #elif GEN_LATEST >= GEN_5
        .expYield = 214,
    #else
        .expYield = 172,
    #endif
        .evYield_Attack = 3,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 15,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = { EGG_GROUP_FLYING, EGG_GROUP_FLYING },
        .abilities = { ABILITY_INTIMIDATE, ABILITY_NONE, ABILITY_RECKLESS },
        .bodyColor = BODY_COLOR_BROWN,
        .speciesName = _("Staraptor"),
        .cryId = CRY_STARAPTOR_MEGA,
        .natDexNum = NATIONAL_DEX_STARAPTOR,
        .categoryName = _("Predator"),
        .height = 19,
        .weight = 500,
        .description = POKEDEX_DESC_STRING(
            "Mega Staraptor is a top-class flier.\n"
            "It can easily soar through the sky while\n"
            "gripping a Steelix that weighs more than\n"
            "880 lbs."),
        FRONT_PIC(StaraptorMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(StaraptorMega, 64, 64),
        .backPicYOffset = 0,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(StaraptorMega),
        ICON(Staraptor, 0),
        .footprint = gMonFootprint_Staraptor,
        .formSpeciesIdTable = sStaraptorFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Staraptor),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_GARCHOMP_MEGA_Z] =
    {
        .baseHP        = 108,
        .baseAttack    = 130,
        .baseDefense   = 85,
        .baseSpeed     = 151,
        .baseSpAttack  = 141,
        .baseSpDefense = 85,
        .types = { TYPE_DRAGON, TYPE_DRAGON },
        .catchRate = 45,
    #if GEN_LATEST >= GEN_8
        .expYield = 300,
    #elif GEN_LATEST >= GEN_5
        .expYield = 270,
    #else
        .expYield = 218,
    #endif
        .evYield_Attack = 3,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 40,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_SLOW,
        .eggGroups = { EGG_GROUP_MONSTER, EGG_GROUP_DRAGON },
        .abilities = { ABILITY_SAND_VEIL, ABILITY_NONE, ABILITY_ROUGH_SKIN },
        .bodyColor = BODY_COLOR_BLUE,
        .speciesName = _("Garchomp"),
        .cryId = CRY_GARCHOMP, //CRY_GARCHOMP_MEGA_Z
        .natDexNum = NATIONAL_DEX_GARCHOMP,
        .categoryName = _("Mach"),
        .height = 19,
        .weight = 990,
        .description = POKEDEX_DESC_STRING(
            "Garchomp has gained a new Mega-Evolved\n"
            "form. It flies around foes at Mach speed\n"
            "and cuts them to shreds with its sinister\n"
            "wing claws."),
        FRONT_PIC(GarchompMegaZ, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(GarchompMegaZ, 64, 64),
        .backPicYOffset = 0,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(GarchompMegaZ),
        ICON(GarchompMegaZ, 0),
        .footprint = gMonFootprint_Garchomp,
        .formSpeciesIdTable = sGarchompFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Garchomp),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_LUCARIO_MEGA_Z] =
    {
        .baseHP        = 70,
        .baseAttack    = 100,
        .baseDefense   = 70,
        .baseSpeed     = 151,
        .baseSpAttack  = 164,
        .baseSpDefense = 70,
        .types = { TYPE_FIGHTING, TYPE_STEEL },
        .catchRate = 45,
        .expYield = (GEN_LATEST >= GEN_5) ? 184 : 204,
        .evYield_Attack = 1,
        .evYield_SpAttack = 1,
        .genderRatio = PERCENT_FEMALE(12.5),
        .eggCycles = 25,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = { EGG_GROUP_FIELD, EGG_GROUP_HUMAN_LIKE },
        .abilities = { ABILITY_STEADFAST, ABILITY_INNER_FOCUS, ABILITY_JUSTIFIED },
        .bodyColor = BODY_COLOR_BLUE,
        .speciesName = _("Lucario"),
        .cryId = CRY_LUCARIO_MEGA_Z,.natDexNum = NATIONAL_DEX_LUCARIO,
        .categoryName = _("Aura"),
        .height = 13,
        .weight = 494,
        .description = POKEDEX_DESC_STRING(
            "By completely cloaking itself in its aura,\n"
            "Mega Lucario Z can parry all manner of\n"
            "attacks, battling as if it were gracefully\n"
            "dancing."),
        FRONT_PIC(LucarioMegaZ, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(LucarioMegaZ, 64, 64),
        .backPicYOffset = 0,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(LucarioMegaZ),
        ICON(LucarioMegaZ, 0),
        .footprint = gMonFootprint_Lucario,
        .formSpeciesIdTable = sLucarioFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Lucario),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_GOLURK_MEGA] =
    {
        .baseHP        = 89,
        .baseAttack    = 159,
        .baseDefense   = 105,
        .baseSpeed     = 55,
        .baseSpAttack  = 70,
        .baseSpDefense = 105,
        .types = { TYPE_GROUND, TYPE_GHOST },
        .catchRate = 90,
        .expYield = 169,
        .evYield_Attack = 2,
        .itemRare = ITEM_LIGHT_CLAY,
        .genderRatio = MON_GENDERLESS,
        .eggCycles = 25,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = { EGG_GROUP_MINERAL, EGG_GROUP_MINERAL },
        .abilities = { ABILITY_UNSEEN_FIST, ABILITY_UNSEEN_FIST, ABILITY_UNSEEN_FIST },
        .bodyColor = BODY_COLOR_GREEN,
        .noFlip = TRUE,
        .speciesName = _("Golurk"),
        .cryId = CRY_GOLURK_MEGA,
        .natDexNum = NATIONAL_DEX_GOLURK,
        .categoryName = _("Automaton"),
        .height = 40,
        .weight = 3300,
        .description = POKEDEX_DESC_STRING(
            "The energy within Golurk has been\n"
            "stimulated by Mega Evolution.\n"
            "The Pokémon could explode at any moment."),
        FRONT_PIC(GolurkMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(GolurkMega, 64, 64),
        .backPicYOffset = 0,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(GolurkMega),
        ICON(GolurkMega, 2),
        .footprint = gMonFootprint_Golurk,
        .formSpeciesIdTable = sGolurkFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Golurk),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_MEOWSTIC_M_MEGA] =
    {
        .baseHP        = 74,
        .baseAttack    = 48,
        .baseDefense   = 76,
        .baseSpeed     = 124,
        .baseSpAttack  = 143,
        .baseSpDefense = 101,
        .types = { TYPE_PSYCHIC, TYPE_PSYCHIC },
        .catchRate = 75,
        .expYield = 163,
        .evYield_Speed = 2,
        .genderRatio = MON_MALE,
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = { EGG_GROUP_FIELD, EGG_GROUP_FIELD },
        .abilities = { ABILITY_KEEN_EYE, ABILITY_INFILTRATOR, ABILITY_PRANKSTER },
        .bodyColor = BODY_COLOR_BLUE,
        .speciesName = _("Meowstic"),
        .cryId = CRY_MEOWSTIC_MEGA,
        .natDexNum = NATIONAL_DEX_MEOWSTIC,
        .categoryName = _("Constraint"),
        .height = 8,
        .weight = 101,
        .description = POKEDEX_DESC_STRING(
            "Mega Meowstic can use its psychic power\n"
            "to compress or expand anything. It\n"
            "overwhelms foes by contorting space\n"
            "itself."),
        FRONT_PIC(MeowsticMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(MeowsticMega, 64, 64),
        .backPicYOffset = 3,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(MeowsticMega),
        ICON(MeowsticMega, 0),
        .footprint = gMonFootprint_Meowstic,
        .formSpeciesIdTable = sMeowsticFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(MeowsticM),
    },
#endif

#if P_MEGA_EVOLUTIONS

    [SPECIES_MEOWSTIC_F_MEGA] =
    {
        .baseHP        = 74,
        .baseAttack    = 48,
        .baseDefense   = 76,
        .baseSpeed     = 124,
        .baseSpAttack  = 143,
        .baseSpDefense = 101,
        .types = { TYPE_PSYCHIC, TYPE_PSYCHIC },
        .catchRate = 75,
        .expYield = 163,
        .evYield_Speed = 2,
        .genderRatio = MON_FEMALE,
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = { EGG_GROUP_FIELD, EGG_GROUP_FIELD },
        .abilities = { ABILITY_KEEN_EYE, ABILITY_INFILTRATOR, ABILITY_COMPETITIVE },
        .bodyColor = BODY_COLOR_WHITE,
        .speciesName = _("Meowstic"),
        .cryId = CRY_MEOWSTIC_MEGA,
        .natDexNum = NATIONAL_DEX_MEOWSTIC,
        .categoryName = _("Constraint"),
        .height = 8,
        .weight = 101,
        .description = POKEDEX_DESC_STRING(
            "Mega Meowstic can use its psychic power\n"
            "to compress or expand anything. It\n"
            "overwhelms foes by contorting space\n"
            "itself."),
        FRONT_PIC(MeowsticMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(MeowsticMega, 64, 64),
        .backPicYOffset = 3,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(MeowsticMega),
        ICON(MeowsticMega, 0),
        .footprint = gMonFootprint_Meowstic,
        .formSpeciesIdTable = sMeowsticFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(MeowsticF),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_CRABOMINABLE_MEGA] =
    {
        .baseHP        = 97,
        .baseAttack    = 157,
        .baseDefense   = 122,
        .baseSpeed     = 33,
        .baseSpAttack  = 62,
        .baseSpDefense = 107,
        .types = { TYPE_FIGHTING, TYPE_ICE },
        .catchRate = 60,
        .expYield = 167,
        .evYield_Attack = 2,
        .itemRare = ITEM_CHERI_BERRY,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = { EGG_GROUP_WATER_3, EGG_GROUP_WATER_3 },
        .abilities = { ABILITY_IRON_FIST, ABILITY_IRON_FIST, ABILITY_IRON_FIST },
        .bodyColor = BODY_COLOR_WHITE,
        .speciesName = _("Crabminabl"),
        .cryId = CRY_CRABOMINABLE_MEGA,
        .natDexNum = NATIONAL_DEX_CRABOMINABLE,
        .categoryName = _("Woolly Crab"),
        .height = 26,
        .weight = 2528,
        .description = POKEDEX_DESC_STRING(
            "It can pulverize reinforced concrete with\n"
            "a light swing of one of its fists, each of\n"
            "which is covered in a thick layer of ice."),
        FRONT_PIC(CrabominableMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(CrabominableMega, 64, 64),
        .backPicYOffset = 3,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(CrabominableMega),
        ICON(CrabominableMega, 2),
        .footprint = gMonFootprint_Crabominable,
        .formSpeciesIdTable = sCrabominableFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Crabominable),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_GOLISOPOD_MEGA] =
    {
        .baseHP        = 75,
        .baseAttack    = 150,
        .baseDefense   = 175,
        .baseSpeed     = 40,
        .baseSpAttack  = 70,
        .baseSpDefense = 120,
        .types = { TYPE_BUG, TYPE_STEEL },
        .catchRate = 45,
        .expYield = 186,
        .evYield_Defense = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = { EGG_GROUP_BUG, EGG_GROUP_WATER_3 },
        .abilities = { ABILITY_EMERGENCY_EXIT, ABILITY_NONE, ABILITY_NONE },
        .bodyColor = BODY_COLOR_GRAY,
        .speciesName = _("Golisopod"),
        .cryId = CRY_GOLISOPOD_MEGA,
        .natDexNum = NATIONAL_DEX_GOLISOPOD,
        .categoryName = _("Hard Scale"),
        .height = 23,
        .weight = 1480,
        .description = POKEDEX_DESC_STRING(
            "It uses four of its arms to fiercely\n"
            "assail its foes. Once they've been pushed\n"
            "to the brink of defeat, it finishes them\n"
            "off with the arms it kept hidden."),
        FRONT_PIC(GolisopodMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(GolisopodMega, 64, 64),
        .backPicYOffset = 4,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(GolisopodMega),
        ICON(GolisopodMega, 2),
        .footprint = gMonFootprint_Golisopod,
        .formSpeciesIdTable = sGolisopodFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Golisopod),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_MAGEARNA_MEGA] =
    {
        .baseHP        = 80,
        .baseAttack    = 125,
        .baseDefense   = 115,
        .baseSpeed     = 95,
        .baseSpAttack  = 170,
        .baseSpDefense = 115,
        .types = { TYPE_STEEL, TYPE_FAIRY },
        .catchRate = 3,
        .expYield = (GEN_LATEST >= GEN_8) ? 300 : 270,
        .evYield_SpAttack = 3,
        .genderRatio = MON_GENDERLESS,
        .eggCycles = 120,
        .friendship = 0,
        .growthRate = GROWTH_SLOW,
        .eggGroups = { EGG_GROUP_NO_EGGS_DISCOVERED, EGG_GROUP_NO_EGGS_DISCOVERED },
        .abilities = { ABILITY_SOUL_HEART, ABILITY_NONE, ABILITY_NONE },
        .bodyColor = BODY_COLOR_GRAY,
        .speciesName = _("Magearna"),
        .cryId = CRY_MAGEARNA_MEGA,
        .natDexNum = NATIONAL_DEX_MAGEARNA,
        .categoryName = _("Artificial"),
        .height = 13,
        .weight = 2481,
        .description = POKEDEX_DESC_STRING(
            "This artificial Pokémon, constructed more\n"
            "than 500 years ago, can understand human\n"
            "speech but cannot itself speak. Its true\n"
            "self is its Soul-Heart, an artificial soul."),
        FRONT_PIC(MagearnaMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(MagearnaMega, 64, 64),
        .backPicYOffset = 3,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(MagearnaMega),
        ICON(MagearnaMega, 0),
        .footprint = gMonFootprint_Magearna,
        .formSpeciesIdTable = sMagearnaFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        .isMythical = TRUE,
        LEARNSETS(Magearna),
    },
#endif

#if P_MEGA_EVOLUTIONS

    [SPECIES_MAGEARNA_ORIGINAL_MEGA] =
    {
        .baseHP        = 80,
        .baseAttack    = 125,
        .baseDefense   = 115,
        .baseSpeed     = 95,
        .baseSpAttack  = 170,
        .baseSpDefense = 115,
        .types = { TYPE_STEEL, TYPE_FAIRY },
        .catchRate = 3,
        .expYield = (GEN_LATEST >= GEN_8) ? 300 : 270,
        .evYield_SpAttack = 3,
        .genderRatio = MON_GENDERLESS,
        .eggCycles = 120,
        .friendship = 0,
        .growthRate = GROWTH_SLOW,
        .eggGroups = { EGG_GROUP_NO_EGGS_DISCOVERED, EGG_GROUP_NO_EGGS_DISCOVERED },
        .abilities = { ABILITY_SOUL_HEART, ABILITY_NONE, ABILITY_NONE },
        .bodyColor = BODY_COLOR_GRAY,
        .speciesName = _("Magearna"),
        .cryId = CRY_MAGEARNA_MEGA,
        .natDexNum = NATIONAL_DEX_MAGEARNA,
        .categoryName = _("Artificial"),
        .height = 13,
        .weight = 2481,
        .description = POKEDEX_DESC_STRING(
            "A mechanism to remove Magearna's\n"
            "limitations has lain secretly within\n"
            "Magearna for 500 years. This mechanism\n"
            "is triggered by a Mega Stone."),
        FRONT_PIC(MagearnaOriginalMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(MagearnaOriginalMega, 64, 64),
        .backPicYOffset = 3,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(MagearnaOriginalMega),
        ICON(MagearnaOriginalMega, 0),
        .footprint = gMonFootprint_Magearna,
        .formSpeciesIdTable = sMagearnaFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        .isMythical = TRUE,
        LEARNSETS(Magearna),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_SCOVILLAIN_MEGA] =
    {
        .baseHP        = 65,
        .baseAttack    = 138,
        .baseDefense   = 85,
        .baseSpeed     = 75,
        .baseSpAttack  = 138,
        .baseSpDefense = 85,
        .types = { TYPE_GRASS, TYPE_FIRE },
        .catchRate = 75,
        .expYield = 170,
        .evYield_Attack = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = { EGG_GROUP_GRASS, EGG_GROUP_GRASS },
        .abilities = { ABILITY_SPICY_SPRAY, ABILITY_SPICY_SPRAY, ABILITY_SPICY_SPRAY },
        .bodyColor = BODY_COLOR_GREEN,
        .speciesName = _("Scovillain"),
        .cryId = CRY_SCOVILLAIN_MEGA,
        .natDexNum = NATIONAL_DEX_SCOVILLAIN,
        .categoryName = _("Spicy Pepper"),
        .height = 12,
        .weight = 220,
        .description = POKEDEX_DESC_STRING(
            "Mega Evolution has dialed up this\n"
            "Pokémon's spiciness. It swings its\n"
            "“necktie” around to wallop its foes."),
        FRONT_PIC(ScovillainMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(ScovillainMega, 64, 64),
        .backPicYOffset = 8,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(ScovillainMega),
        ICON(ScovillainMega, 1),
        .formSpeciesIdTable = sScovillainFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Scovillain),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_BAXCALIBUR_MEGA] =
    {
        .baseHP        = 115,
        .baseAttack    = 175,
        .baseDefense   = 117,
        .baseSpeed     = 87,
        .baseSpAttack  = 105,
        .baseSpDefense = 101,
        .types = { TYPE_DRAGON, TYPE_ICE },
        .catchRate = 10,
        .expYield = 300,
        .evYield_Attack = 3,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 40,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_SLOW,
        .eggGroups = { EGG_GROUP_DRAGON, EGG_GROUP_MINERAL },
        .abilities = { ABILITY_THERMAL_EXCHANGE, ABILITY_NONE, ABILITY_ICE_BODY },
        .bodyColor = BODY_COLOR_BLUE,
        .speciesName = _("Baxcalibur"),
        .cryId = CRY_BAXCALIBUR_MEGA,
        .natDexNum = NATIONAL_DEX_BAXCALIBUR,
        .categoryName = _("Ice Dragon"),
        .height = 21,
        .weight = 3150,
        .description = POKEDEX_DESC_STRING(
            "Baxcalibur's dorsal blade has grown even\n"
            "more massive thanks to Mega Evolution.\n"
            "This Pokémon fires beams from the hilt\n"
            "at its solar plexus."),
        FRONT_PIC(BaxcaliburMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(BaxcaliburMega, 64, 64),
        .backPicYOffset = 5,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(BaxcaliburMega),
        ICON(BaxcaliburMega, 0),
        .formSpeciesIdTable = sBaxcaliburFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Baxcalibur),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_TATSUGIRI_CURLY_MEGA] =
    {
        .baseHP        = 68,
        .baseAttack    = 65,
        .baseDefense   = 90,
        .baseSpeed     = 92,
        .baseSpAttack  = 135,
        .baseSpDefense = 125,
        .types = { TYPE_DRAGON, TYPE_WATER },
        .catchRate = 100,
        .expYield = 166,
        .evYield_SpAttack = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 35,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = { EGG_GROUP_WATER_2, EGG_GROUP_WATER_2 },
        .abilities = { ABILITY_STORM_DRAIN, ABILITY_NONE, ABILITY_STORM_DRAIN },
        .bodyColor = BODY_COLOR_RED,
        .speciesName = _("Tatsugiri"),
        .cryId = CRY_TATSUGIRI_MEGA,
        .natDexNum = NATIONAL_DEX_TATSUGIRI,
        .categoryName = _("Mimicry"),
        .height = 6,
        .weight = 240,
        .description = POKEDEX_DESC_STRING(
            "Tatsugiri's brain has been invigorated\n"
            "by Mega Evolution, making it even\n"
            "wilier. It can create and command\n"
            "copies of itself."),
        FRONT_PIC(TatsugiriCurlyMega, 64, 64),
        .frontPicYOffset = 11,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(TatsugiriCurlyMega, 64, 64),
        .backPicYOffset = 17,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(TatsugiriMega),
        ICON(TatsugiriCurlyMega, 0),
        .formSpeciesIdTable = sTatsugiriFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Tatsugiri),
    },
#endif

#if P_MEGA_EVOLUTIONS

    [SPECIES_TATSUGIRI_DROOPY_MEGA] =
    {
        .baseHP        = 68,
        .baseAttack    = 65,
        .baseDefense   = 90,
        .baseSpeed     = 92,
        .baseSpAttack  = 135,
        .baseSpDefense = 125,
        .types = { TYPE_DRAGON, TYPE_WATER },
        .catchRate = 100,
        .expYield = 166,
        .evYield_SpAttack = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 35,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = { EGG_GROUP_WATER_2, EGG_GROUP_WATER_2 },
        .abilities = { ABILITY_STORM_DRAIN, ABILITY_NONE, ABILITY_STORM_DRAIN },
        .bodyColor = BODY_COLOR_RED,
        .speciesName = _("Tatsugiri"),
        .cryId = CRY_TATSUGIRI_MEGA,
        .natDexNum = NATIONAL_DEX_TATSUGIRI,
        .categoryName = _("Mimicry"),
        .height = 6,
        .weight = 240,
        .description = POKEDEX_DESC_STRING(
            "It solidifies the energy of Mega Evolution,\n"
            "building up an overflowing pile to launch\n"
            "as projectiles. These projectiles explode\n"
            "on contact."),
        FRONT_PIC(TatsugiriDroopyMega, 64, 64),
        .frontPicYOffset = 11,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(TatsugiriDroopyMega, 64, 64),
        .backPicYOffset = 17,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(TatsugiriMega),
        ICON(TatsugiriDroopyMega, 0),
        .formSpeciesIdTable = sTatsugiriFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Tatsugiri),
    },
#endif

#if P_MEGA_EVOLUTIONS

    [SPECIES_TATSUGIRI_STRETCHY_MEGA] =
    {
        .baseHP        = 68,
        .baseAttack    = 65,
        .baseDefense   = 90,
        .baseSpeed     = 92,
        .baseSpAttack  = 135,
        .baseSpDefense = 125,
        .types = { TYPE_DRAGON, TYPE_WATER },
        .catchRate = 100,
        .expYield = 166,
        .evYield_SpAttack = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 35,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = { EGG_GROUP_WATER_2, EGG_GROUP_WATER_2 },
        .abilities = { ABILITY_STORM_DRAIN, ABILITY_NONE, ABILITY_STORM_DRAIN },
        .bodyColor = BODY_COLOR_RED,
        .speciesName = _("Tatsugiri"),
        .cryId = CRY_TATSUGIRI_MEGA,
        .natDexNum = NATIONAL_DEX_TATSUGIRI,
        .categoryName = _("Mimicry"),
        .height = 6,
        .weight = 240,
        .description = POKEDEX_DESC_STRING(
            "Using the energy of Mega Evolution, it\n"
            "creates a dish to ride upon, allowing it\n"
            "to move with total freedom-even through\n"
            "the air."),
        FRONT_PIC(TatsugiriStretchyMega, 40, 40),
        .frontPicYOffset = 11,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(TatsugiriStretchyMega, 64, 64),
        .backPicYOffset = 17,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(TatsugiriMega),
        ICON(TatsugiriStretchyMega, 0),
        .formSpeciesIdTable = sTatsugiriFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Tatsugiri),
    },
#endif

#if P_MEGA_EVOLUTIONS
    [SPECIES_GLIMMORA_MEGA] =
    {
        .baseHP        = 83,
        .baseAttack    = 90,
        .baseDefense   = 105,
        .baseSpeed     = 101,
        .baseSpAttack  = 150,
        .baseSpDefense = 96,
        .types = { TYPE_ROCK, TYPE_POISON },
        .catchRate = 25,
        .expYield = 184,
        .evYield_SpAttack = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 30,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = { EGG_GROUP_MINERAL, EGG_GROUP_MINERAL },
        .abilities = { ABILITY_ADAPTABILITY, ABILITY_NONE, ABILITY_ADAPTABILITY },
        .bodyColor = BODY_COLOR_BLUE,
        .speciesName = _("Glimmora"),
        .cryId = CRY_GLIMMORA_MEGA,
        .natDexNum = NATIONAL_DEX_GLIMMORA,
        .categoryName = _("Ore"),
        .height = 15,
        .weight = 450,
        .description = POKEDEX_DESC_STRING(
            "Glimmora's petals-now larger and\n"
            "separated from its main body-rotate\n"
            "around it to provide defense while\n"
            "cattering poisonous fragments."),
        FRONT_PIC(GlimmoraMega, 64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        BACK_PIC(GlimmoraMega, 64, 64),
        .backPicYOffset = 0,
        .backAnimId = BACK_ANIM_NONE,
        PALETTES(GlimmoraMega),
        ICON(GlimmoraMega, 0),
        .formSpeciesIdTable = sGlimmoraFormSpeciesIdTable,
        .isMegaEvolution = TRUE,
        LEARNSETS(Glimmora),
    },
#endif
