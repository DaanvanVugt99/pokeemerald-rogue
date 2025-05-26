#ifndef GUARD_CONSTANTS_ABILITIES_H
#define GUARD_CONSTANTS_ABILITIES_H

#define ABILITY_NONE 0
#define ABILITY_STENCH 1         // 20% chance to switch out the attacker when hit by a contact move.
#define ABILITY_DRIZZLE 2        // The Pokémon makes it rain when it enters a battle.
#define ABILITY_SPEED_BOOST 3    // Its Speed stat is boosted every turn.
#define ABILITY_BATTLE_ARMOR 4   // Hard armor protects the Pokémon from critical hits. Takes 20% less damage from contact moves.
#define ABILITY_STURDY 5         // It cannot be knocked out with one hit. One-hit KO moves cannot knock it out, either.
#define ABILITY_DAMP 6           // Prevents the use of explosive moves, such as Self-Destruct, by dampening its surroundings.
#define ABILITY_LIMBER 7         // Its limber body protects the Pokémon from paralysis. Takes 50% less damage from crash damage.
#define ABILITY_SAND_VEIL 8      // Boosts the Pokémon's evasiveness in a sandstorm by 30%.
#define ABILITY_STATIC 9         // 1/3 chance to get paralyzed on contact against this Pokémon.
#define ABILITY_VOLT_ABSORB 10   // The Pokémon heals up to 1/4 of its maximum Hit Points when hit with Electric-type moves.
#define ABILITY_WATER_ABSORB 11  // The Pokémon heals up to 1/4 of its maximum Hit Points when hit with Water-type moves.
#define ABILITY_OBLIVIOUS 12     // The Pokémon is oblivious, and that keeps it from being infatuated, intimidated or falling for taunts.
#define ABILITY_CLOUD_NINE 13    // Eliminates the effects of weather.
#define ABILITY_COMPOUND_EYES 14 // Raises the Pokémon’s accuracy by 30%.
#define ABILITY_INSOMNIA 15      // The Pokémon cannot fall asleep and Speed cannot be lowered.
#define ABILITY_COLOR_CHANGE 16  // The Pokemon's type become the type that counters the move used on it before hit.
#define ABILITY_IMMUNITY 17      // The immune system of the Pokémon prevents it from getting poisoned or being hurt from acid rain. Halves damage from Poison-type moves.
#define ABILITY_FLASH_FIRE 18    // This ability makes the Pokémon immune to Fire-type moves and will activate when hit by one. When activated, the power of the Pokémon's Fire-type moves is increased by 50%.
#define ABILITY_SHIELD_DUST 19   // Damaging moves used by the opponent will not have an additional effect. Also avoids damage from entry hazards.
#define ABILITY_OWN_TEMPO 20     // Prevents it from becoming confused or being intimidated.
#define ABILITY_SUCTION_CUPS 21  // This Pokémon uses suction cups to stay in one spot to negate all moves and items that force switching out. Traps enemies for 2-3 turns on contact.
#define ABILITY_INTIMIDATE 22    // The Pokémon intimidates opposing Pokémon upon entering battle, lowering their Attack stat.
#define ABILITY_SHADOW_TAG 23    // This Pokémon steps on the opposing Pokémon's shadow to prevent it from  escaping. 1.2x damage for ghost-type moves.
#define ABILITY_ROUGH_SKIN 24    // This Pokémon inflicts 1/8 HP damage on contact. Deals 1/6 damage if the attacking move is Normal-, Fighting-, or Bug-type.
#define ABILITY_WONDER_GUARD 25  // *No innate* Its mysterious power only lets supereffective moves hit the  Pokémon.
#define ABILITY_LEVITATE 26      // By floating in the air, the Pokémon receives full immunity to all Ground-type moves. Flying moves gain STAB.
#define ABILITY_EFFECT_SPORE 27  // When a Pokémon with this Ability is hit by a move that makes contact, there is a 1/3 chance that the attacking Pokémon will become either  poisoned, paralyzed, or asleep
#define ABILITY_SYNCHRONIZE 28   // *No innate* The attacker will receive the same status condition if it inflicts a burn, poison, or paralysis to the Pokémon.
#define ABILITY_CLEAR_BODY 29    // Prevents other Pokémon's moves or Abilities from lowering the Pokémon's stats.
#define ABILITY_NATURAL_CURE 30  // All status conditions heal when the Pokémon switches out or the battle ends.
#define ABILITY_LIGHTNING_ROD 31 // The Pokémon draws in all Electric-type moves. Instead of being hit by Electric-type moves, it boosts its Attack or Sp. Attack stat (which is higher).
#define ABILITY_SERENE_GRACE 32  // Doubles the likelihood of additional effects occurring when attacking.
#define ABILITY_SWIFT_SWIM 33    // Doubles speed in rain.
#define ABILITY_CHLOROPHYLL 34   // Doubles speed in sunshine.
#define ABILITY_ILLUMINATE 35    // Prevents other Pokémon from lowering the accuracy stat stage of the Pokémon with this Ability, and also ignores the target's evasion stages. Lowers all opposing pokemons accuracy by 1 stage.
#define ABILITY_TRACE 36         // *No innate* When it enters a battle, the Pokémon copies an opposing Pokémon's Ability.
#define ABILITY_HUGE_POWER 37    // Doubles the Pokémon's highest offensive stat.
#define ABILITY_POISON_POINT 38  // Deals 1/16 damage and has 1/3 chance to get poisoned on contact against this Pokémon.
#define ABILITY_INNER_FOCUS 39   // The Pokémon's intensely focused, and that protects the Pokémon from flinching, Focus Blast has increased accuracy to 90%. Focus Punch halves damage instead of canceling on hit.
#define ABILITY_MAGMA_ARMOR 40   // Prevents the Pokémon from becoming frozen. Takes 50% less damage from Water- and Ice-type moves.
#define ABILITY_WATER_VEIL 41    // Prevents the Pokémon from getting a burn. Sets Aqua Ring on entry. Aqua Ring effect: Restore 1/8 HP every turn
#define ABILITY_MAGNET_PULL 42   // Traps Steel-type Pokémon. Steel moves gain 1.2x damage. (WIP, has to be good for magnezone)
#define ABILITY_SOUNDPROOF 43    // Soundproofing gives the Pokémon full immunity to all sound-based moves.
#define ABILITY_RAIN_DISH 44     // Restores 1/8 HP per turn under rain.
#define ABILITY_SAND_STREAM 45   // The Pokémon summons a sandstorm when it enters a battle.
#define ABILITY_PRESSURE 46      // When this Pokémon is hit by a move, all the opponent’s PP moves lower by 1.
#define ABILITY_THICK_FAT 47     // The Pokémon is protected by a layer of thick fat, which halves the  damage taken from Fire- and Ice-type moves. Also immune to burn and freeze/frostbite.
#define ABILITY_EARLY_BIRD 48    // The Pokémon awakens from sleep twice as fast as other Pokémon. Also, when the Pokémon wakes up, increases the Speed stat by one stage.
#define ABILITY_FLAME_BODY 49    // 30% chance to get burned on contact against this Pokémon.
#define ABILITY_RUN_AWAY 50      // Enables a sure getaway from wild Pokémon. Boosts the Pokémon's Speed  stat sharply when its stats are lowered.
#define ABILITY_KEEN_EYE 51      // Prevents loss of accuracy. Ups accuracy by 20%.
#define ABILITY_HYPER_CUTTER 52  // Prevents other Pokémon from lowering its Attack stat. Contact moves have its critical hit rate increased by 1.
#define ABILITY_PICKUP 53        // Removes all hazards on entry. Has a 10% chance of finding and holding an item after battle. Will not work if this Pokémon is already holding  an item. From Black & White, it allows for the user to pick up the item  of its opponent or ally if they used up the item in battle.
#define ABILITY_TRUANT 54        // The Pokémon can't use a move if it had used a move on the previous turn.
#define ABILITY_HUSTLE 55        // Boosts the Attack stat by 40%, but lowers accuracy by 10%.
#define ABILITY_CUTE_CHARM 56    // The opponent has a 30% chance of being induced with Attract when using an attack, that requires physical contact, against this Pokémon.
#define ABILITY_PLUS 57          // When battling with Plus, at the end of each turn, this pokemon recovers 1/8 HP. With Minus, this pokemon's status moves gain +1 priority.
#define ABILITY_MINUS 58         // When battling with Plus, Special Attack increases by 50%. With Minus, this pokemons attacks ignore target stat changes.
#define ABILITY_FORECAST 59      // This Pokémon’ changes its form and type if Sunlight, Rain, Acid Rain, Eclipse or Hail is in effect.
#define ABILITY_STICKY_HOLD 60   // This Pokémon’s item cannot be removed. Contact moves used against it have a 50% chance to lower the attacker’s Speed by 1 stage.
#define ABILITY_SHED_SKIN 61     // Every turn, it has a 1 in 3 chance of healing from a status condition  (BURN, PARALYZE, SLEEP, POISON, FREEZE).
#define ABILITY_GUTS 62          // Attack is increased by 50% when induced with a status (BURN, PARALYZE, SLEEP, POISON, FREEZE). Burn’s effect of lowering Attack is not applied.
#define ABILITY_MARVEL_SCALE 63  // The Pokémon's marvelous scales double Defense if it has a status condition.
#define ABILITY_LIQUID_OOZE 64   // The oozed liquid has a strong stench, which damages  attackers using any draining move. Also ups draining moves by 1.3x damage.
#define ABILITY_OVERGROW 65      // Now always provides a 20% damage boost to their move types. This  increases to 50% while the Pokémon is at or below 1/2 HP. These boosts  do not stack (the 50% boost replaces the 20% one while weakened).
#define ABILITY_BLAZE 66         // Now always provides a 20% damage boost to their move types. This  increases to 50% while the Pokémon is at or below 1/2 HP. These boosts do not stack (the 50% boost replaces the 20% one while weakened).
#define ABILITY_TORRENT 67       // Now always provides a 20% damage boost to their move types. This  increases to 50% while the Pokémon is at or below 1/2 HP. These boosts do not stack (the 50% boost replaces the 20% one while weakened).
#define ABILITY_SWARM 68         // Now always provides a 20% damage boost to their move types. This  increases to 50% while the Pokémon is at or below 1/2 HP. These boosts do not stack (the 50% boost replaces the 20% one while weakened).
#define ABILITY_ROCK_HEAD 69     // Prevents recoil damage.
#define ABILITY_DROUGHT 70       // Summons sunshine on entry.
#define ABILITY_ARENA_TRAP 71    // Prevents fleeing. 1.2x damage for ground-type moves.
#define ABILITY_VITAL_SPIRIT 72  // Prevents falling asleep and self lowering stat drops from attacks. (ideally add a popup for this)
#define ABILITY_WHITE_SMOKE 73   // The Pokémon is protected by its white smoke, which prevents other Pokémon from lowering its stats
#define ABILITY_PURE_POWER 74    // Doubles the Pokémon's highest offensive stat.
#define ABILITY_SHELL_ARMOR 75   // A hard shell protects the Pokémon from critical hits. Takes 20% less damage from non-contact moves.
#define ABILITY_AIR_LOCK 76      // Eliminates the effects of weather.

#define ABILITIES_COUNT_GEN3 77

// Gen 4
#define ABILITY_TANGLED_FEET 77  // *No innate* Pokémon’s evasion raises one level when Confused.
#define ABILITY_MOTOR_DRIVE 78   // Boosts its Speed stat if hit by an Electric-type move instead of taking damage.
#define ABILITY_RIVALRY 79       // Pokémon's power is increased by 25% if the foe is of the same gender.
#define ABILITY_STEADFAST 80     // *No innate* Raises speed by +1 when flinching.
#define ABILITY_SNOW_CLOAK 81    // Boosts the Pokémon's evasiveness in a hailstorm by 30%.
#define ABILITY_GLUTTONY 82      // Eats berries at 1/2 HP instead of 1/3 HP.
#define ABILITY_ANGER_POINT 83   // The Pokémon is angered when it takes a critical hit from physical move from opposing Pokémon, and that maxes its Attack stat. Boosts the Attack stat by one stage when hit by physical moves.
#define ABILITY_UNBURDEN 84      // *No innate* Doubles the Speed stat if the Pokémon's held item is used or lost.
#define ABILITY_HEATPROOF 85     // The heatproof body of the Pokémon halves the damage from Fire-type moves that hit it.
#define ABILITY_SIMPLE 86        // Doubles all stat changes.
#define ABILITY_DRY_SKIN 87      // Rain: HP will increase by 1/8 of its max HP each turn. Intense Sunlight: HP will decrease by 1/8 of its max HP each turn.
#define ABILITY_DOWNLOAD 88      // Compares an opposing Pokémon's Defense and Sp. Def stats before raising its own Attack or Sp. Atk stat—whichever will be more effective.
#define ABILITY_IRON_FIST 89     // The power of punching moves is increased by 30%.
#define ABILITY_POISON_HEAL 90   // Restores 1/8 HP per turn if the Pokémon is poisoned instead of losing HP.
#define ABILITY_ADAPTABILITY 91  // Increases the STAB Bonus from x1.5 to x2.
#define ABILITY_SKILL_LINK 92    // Maximizes the number of times multistrike moves hit.
#define ABILITY_HYDRATION 93     // Heals status conditions every turn if it's raining.
#define ABILITY_SOLAR_POWER 94   // Ups Sp. Atk in the sun by 1.5x. No longer decreases HP at end of every sun turn.
#define ABILITY_QUICK_FEET 95    // Boosts the Speed stat if the Pokémon has a status condition.
#define ABILITY_NORMALIZE 96     // All the Pokémon's moves become Normal type. The power of those moves is boosted 1.5x.
#define ABILITY_SNIPER 97        // Power of critical-hit moves is increased to 225% rather than 150%.
#define ABILITY_MAGIC_GUARD 98   // The Pokémon only takes damage from attacks.
#define ABILITY_NO_GUARD 99      // The Pokémon employs no-guard tactics to ensure incoming and outgoing attacks always land.
#define ABILITY_STALL 100        // Always moves last.
#define ABILITY_TECHNICIAN 101   // Powers up the Pokémon's weaker moves with BP 60 or less.
#define ABILITY_LEAF_GUARD 102   // Prevents status conditions in harsh sunlight.
#define ABILITY_KLUTZ 103        // *No innate* Can't use items.
#define ABILITY_MOLD_BREAKER 104 // Moves can be used on the target regardless of its Abilities. Hits Levitate with Ground moves.
#define ABILITY_SUPER_LUCK 105   // Raises the critical-hit ratio of moves by one level.
#define ABILITY_AFTERMATH 106    // The foe that dealt the final hit loses 1/4 of its maximum HP if the  move made physical contact.
#define ABILITY_ANTICIPATION 107 // Warns when the foe knows 1-hit KO or super-effective moves.
#define ABILITY_FOREWARN 108     // Reveals the foe’s strongest move.
#define ABILITY_UNAWARE 109      // Foe’s stat modifications are ignored.
#define ABILITY_TINTED_LENS 110  // The Pokémon can use "not very effective" moves to deal regular damage.
#define ABILITY_FILTER 111       // Reduces the power of supereffective attacks taken by 35%.
#define ABILITY_SLOW_START 112   // For five turns, the Pokémon's Attack and Speed stats are halved, but restores its stats after these five turns.
#define ABILITY_SCRAPPY 113      // The Pokémon can hit: Ghost-type Pokémon with Normal- and Fighting-type  moves.
#define ABILITY_STORM_DRAIN 114  // Water-type moves are drawn to this Pokémon. Water-type moves will do  no damage and the Pokémon's highest offensive stat is raised by 1 stage.
#define ABILITY_ICE_BODY 115     // Restores 1/8 HP every turn in hailstorm.
#define ABILITY_SOLID_ROCK 116   // Reduces the power of supereffective attacks taken by 35%.
#define ABILITY_SNOW_WARNING 117 // The Pokémon summons a hailstorm when it enters a battle.
#define ABILITY_HONEY_GATHER 118 // *No innate* The Pokémon may gather Honey after a battle.
#define ABILITY_FRISK 119        // *No innate* When it enters a battle, the Pokémon can check an opposing Pokémon's held item.
#define ABILITY_RECKLESS 120     // Powers up moves that have recoil and crash damage by 20%.
#define ABILITY_MULTITYPE 121    // *No innate* Changes the Pokémon's type to match the Plate or Z-Crystal  it holds.
#define ABILITY_FLOWER_GIFT 122  // *No innate* Ally’s Attack and Special Defence are multiplied by 1.5  during strong sunlight.
#define ABILITY_BAD_DREAMS 123   // Reduces 1/4 HP of sleeping opposing Pokémon every turn.

#define ABILITIES_COUNT_GEN4 124

// Gen 5
#define ABILITY_PICKPOCKET 124   // *No innate* When a Pokémon makes physical contact with the Pokémon, the item of the opponent is stolen.
#define ABILITY_SHEER_FORCE 125  // Moves with a secondary effect are increased in power by 30% but lost their secondary effect
#define ABILITY_CONTRARY 126     // Makes stat changes have an opposite effect.
#define ABILITY_UNNERVE 127      // *No innate* Unnerves opposing Pokémon and makes them unable to eat Berries and blocks healing moves.
#define ABILITY_DEFIANT 128      // Boosts the Pokémon's Attack stat sharply when its stats are lowered.
#define ABILITY_DEFEATIST 129    // *No innate* Halves Atk & SpAtk at 1/3 HP (instead of 1/2 HP).
#define ABILITY_CURSED_BODY 130  // May disable a move used on the Pokémon.
#define ABILITY_HEALER 131       // At the end of the turn, there is a 30% chance of either Ally being healed from a status condition
#define ABILITY_FRIEND_GUARD 132 // *No innate* The damage that ally Pokémon receive from attacks is reduced by 25%.
#define ABILITY_WEAK_ARMOR 133   // Physical attacks to the Pokémon lower its Defense and Sp. Defense stats but sharply raise its Attack, Sp. Attack and Speed stat.
#define ABILITY_HEAVY_METAL 134  // Doubles the Pokémon's weight.
#define ABILITY_LIGHT_METAL 135  // Halves the Pokémon's weight.
#define ABILITY_MULTISCALE 136   // Reduces damage afflicted to the Pokémon by 50% if at maximum HP
#define ABILITY_TOXIC_BOOST 137  // Attack stat increased by 50% when poisoned. Now immune to poison status damage.
#define ABILITY_FLARE_BOOST 138  // Special Attack stat increased by 50% when burned. Now immune to burn  status damage.
#define ABILITY_HARVEST 139      // The Pokémon has a 50% chance of restoring a consumed berry. During sunshine, this chance increased to 100%
#define ABILITY_TELEPATHY 140    // *No innate* Anticipates an ally's attack and dodges it.
#define ABILITY_MOODY 141        // Raises one stat sharply and lowers another every turn.
#define ABILITY_OVERCOAT 142     // Protects the Pokémon from things like sand, hail, and powder. The  Pokémon takes 20% less damage from special moves.
#define ABILITY_POISON_TOUCH 143 // 30% chance to badly poison a target when the Pokémon makes contact.
#define ABILITY_REGENERATOR 144  // Restores 1/3 HP when withdrawn from battle or when battle ends.
#define ABILITY_BIG_PECKS 145    // Powers up moves that make direct contact by 30%. Prevents other Pokémon from lowering the Pokémon's defense stat stage.
#define ABILITY_SAND_RUSH 146    // Doubles the Pokémon's Speed stat in a sandstorm.
#define ABILITY_WONDER_SKIN 147  // Makes non-damaging moves that are targeted at this Pokémon have the accuracy of 50%.
#define ABILITY_ANALYTIC 148     // Boosts move power by 50% when the Pokémon moves last.
#define ABILITY_ILLUSION 149     // 1.3x damage boost when in Illusion mode.
#define ABILITY_IMPOSTER 150     // *No innate* The Pokémon transforms itself into the Pokémon it's facing.
#define ABILITY_INFILTRATOR 151  // Passes through the opposing Pokémon's barrier, substitute, and the like and strikes.
#define ABILITY_MUMMY 152        // *No innate* Contact with the Pokémon changes the attacker's Ability to Mummy.
#define ABILITY_MOXIE 153        // Attack is raised by one stage when the Pokémon knocks out another Pokémon
#define ABILITY_JUSTIFIED 154    // Attack is raised by one stage when the Pokémon is hit by a Dark-type move
#define ABILITY_RATTLED 155      // Dark-, Ghost-, and Bug-type moves scare the Pokémon and boost its Speed stats.
#define ABILITY_MAGIC_BOUNCE 156 // Reflects status moves instead of getting hit by them.
#define ABILITY_SAP_SIPPER 157   // Immune to Grass-type moves. Instead of being hit by Grass-type moves, it boosts its Attack or Sp. Attack stat (which is higher).
#define ABILITY_PRANKSTER 158    // Gives priority to a status move.
#define ABILITY_SAND_FORCE 159   // Boosts the power of Rock-, Ground-, and Steel-type moves by 30% in a sandstorm.
#define ABILITY_IRON_BARBS 160   // Deals 1/8 HP damage on contact. Deals 1/6 damage if the attacking move is Steel-, Rock-, or Ground-type.
#define ABILITY_ZEN_MODE 161     // Changes form regardless of health (So, becomes Zen Mode as soon as Darmanitan is switched in)
#define ABILITY_VICTORY_STAR 162 // Boosts the accuracy of its allies and itself by 20%.
#define ABILITY_TURBOBLAZE 163   // Moves can be used on the target regardless of its Abilities. Adds the Fire-type to the Pokémon, in addition to the Pokémon's original type(s).
#define ABILITY_TERAVOLT 164     // Moves can be used on the target regardless of its Abilities. Adds the Electric-type to the Pokémon, in addition to the Pokémon's original type(s).

#define ABILITIES_COUNT_GEN5 165

// Gen 6
#define ABILITY_AROMA_VEIL 165     // Protects itself and its allies from attacks that limit their move choices.
#define ABILITY_FLOWER_VEIL 166    // Ally Grass-type Pokémon are protected from status conditions and the lowering of their stats.
#define ABILITY_CHEEK_POUCH 167    // Restores HP as well when the Pokémon eats a Berry.
#define ABILITY_PROTEAN 168        // Changes the Pokémon's type to the type of the move it's about to use.
#define ABILITY_FUR_COAT 169       // Halves the damage from physical moves.
#define ABILITY_MAGICIAN 170       // *No innate* The Pokémon steals the held item of a Pokémon it hits with a move.
#define ABILITY_BULLETPROOF 171    // Protects the Pokémon from some ball and bomb moves. Takes 20% less damage from special moves.
#define ABILITY_COMPETITIVE 172    // Boosts the Sp. Atk stat sharply when a stat is lowered.
#define ABILITY_STRONG_JAW 173     // The Pokémon's strong jaw boosts the power of its biting moves. [Now  affects Bug Bite, Super Fang, Deathroll]
#define ABILITY_REFRIGERATE 174    // All Normal-type moves used by the Pokémon to become Ice-type and receive a 10% power boost.
#define ABILITY_SWEET_VEIL 175     // *No innate* Prevents itself and ally Pokémon from falling asleep.
#define ABILITY_STANCE_CHANGE 176  // *No innate* The Pokémon changes its form to Blade Forme when it uses an attack move and changes to Shield Forme when it uses King's Shield.
#define ABILITY_GALE_WINGS 177     // Flying-type moves have +1 priority. Requires full HP.
#define ABILITY_MEGA_LAUNCHER 178  // Boosts all beam, pump, cannon, zooka, shot, gun, aura and pulse moves  by 50%.
#define ABILITY_GRASS_PELT 179     // Increases Defense by 50% when battling on Grass or if the move Grassy  Terrain is in effect
#define ABILITY_SYMBIOSIS 180      // May pass an item to an ally.
#define ABILITY_TOUGH_CLAWS 181    // Powers up moves by 30% that make direct contact.
#define ABILITY_PIXILATE 182       // All Normal-type moves used by the Pokémon to become Fairy-type and  receive a 10% power boost.
#define ABILITY_GOOEY 183          // Lowers speed on contact.
#define ABILITY_AERILATE 184       // All Normal-type moves used by the Pokémon to become Flying-type and  receive a 10% power boost.
#define ABILITY_PARENTAL_BOND 185  // Hits twice. Second hit deals 0.25x damage.
#define ABILITY_DARK_AURA 186      // Powers up each Pokémon's Dark-type moves by 50%.
#define ABILITY_FAIRY_AURA 187     // Powers up each Pokémon's Fairy-type moves by 50%.
#define ABILITY_AURA_BREAK 188     // *No innate* The effects of “Aura” Abilities are reversed to lower the  power of affected moves.
#define ABILITY_PRIMORDIAL_SEA 189 // The Pokémon changes the weather to nullify Fire-type attacks.
#define ABILITY_DESOLATE_LAND 190  // The Pokémon changes the weather to nullify Water-type attacks.
#define ABILITY_DELTA_STREAM 191   // Creates the Strong Winds weather. This lasts until the Pokémon exits battle, and weakens moves used on Flying-type Pokémon that would be Super Effective

#define ABILITIES_COUNT_GEN6 192

// Gen 7
#define ABILITY_STAMINA 192          // Boosts the Defense stat when hit by an attack. When it takes a critical hit, it maxes its Defense stat.
#define ABILITY_WIMP_OUT 193         // *No innate* The Pokémon cowardly switches out when its HP becomes half or less.
#define ABILITY_EMERGENCY_EXIT 194   // The Pokémon, sensing danger, switches out when its HP becomes half or less.
#define ABILITY_WATER_COMPACTION 195 // When hit by a Water-type move, its Defense rises by +2. Reduces Water type damage by 50%.
#define ABILITY_MERCILESS 196        // The Pokémon's attacks become critical hits if the target is poisoned or its Speed stat stage is -1 or lower.
#define ABILITY_SHIELDS_DOWN 197     // *No innate* Changes the Pokémon into its Core form when its Hit Points drop below 50%. Before then, it cannot be affected by Status Conditions such as Burn or Paralysis
#define ABILITY_STAKEOUT 198         // Doubles the damage dealt to the target's replacement if the target switches out.
#define ABILITY_WATER_BUBBLE 199     // Water Bubble halves the damage dealt to the Pokémon with this Ability by Fire-type attacks and doubles the power of Water-type moves used by the Pokémon with this Ability. It also prevents the Pokémon with the Ability from being burned.
#define ABILITY_STEELWORKER 200      // Powers up Steel-type moves by 30%.
#define ABILITY_BERSERK 201          // *No innate* Boosts the Pokémon's Sp. Atk stat when it takes a hit that causes its HP to become half or less.
#define ABILITY_SLUSH_RUSH 202       // Doubles speed in hail.
#define ABILITY_LONG_REACH 203       // The Pokémon uses its moves without making contact with the target. If a physical move already doesn't make contact before applying this  ability, boosts its damage by 20%.
#define ABILITY_LIQUID_VOICE 204     // All sound-based moves become Water-type moves and boosts them by 20%.
#define ABILITY_TRIAGE 205           // Gives +1 priority to healing moves.
#define ABILITY_GALVANIZE 206        // All Normal-type moves become Electric-type moves and boosts them by 10%.
#define ABILITY_SURGE_SURFER 207     // Doubles the Pokémon's Speed stat on Electric Terrain. Works while levitating.
#define ABILITY_SCHOOLING 208        // Changes the Pokémon's form when it is Level 20 and has more than 25% Hit Points. When Hit Points drop below 25%, it changes back to Solo Form
#define ABILITY_DISGUISE 209         // *No innate* Once per battle, the Pokémon can withstand one damaging attack without receiving any damage. Damage inflicted through entry hazards, weather or status conditions will not break the disguise. Damage from Confusion will break the Disguise. It causes 1/8th of max Hit Point damage when it breaks
#define ABILITY_BATTLE_BOND 210      // *No innate* Defeating an opposing Pokémon strengthens the Pokémon's bond with its Trainer, and it becomes Ash-Greninja. Water Shuriken gets more powerful.
#define ABILITY_POWER_CONSTRUCT 211  // *No innate* At the start of battle or at the end of a turn, if the HP of the Pokémon is below half, other cells gather to aid and it change into its Complete Forme. After the battle, if the Pokémon's HP is above half, it reverts to its original form.
#define ABILITY_CORROSION 212        // The Pokémon can poison or attack the target even if it's a Steel or Poison type. Poison-type moves deals "super-effective" damage against Steel-type Pokemon.
#define ABILITY_COMATOSE 213         // Always sleeping, but can use moves.
#define ABILITY_QUEENLY_MAJESTY 214  // Its majesty pressures the opposing Pokémon, making it unable to attack using priority moves.
#define ABILITY_INNARDS_OUT 215      // Damages the attacker landing the finishing hit by the amount equal to its last HP.
#define ABILITY_DANCER 216           // *No innate* When another Pokémon uses a dance move, it can use a dance move following it regardless of its Speed.
#define ABILITY_BATTERY 217          // Powers up ally Pokémon's special moves by 30%.
#define ABILITY_FLUFFY 218           // A Pokémon with Fluffy takes half damage from moves that make contact. It also takes double damage from Fire-type moves.
#define ABILITY_DAZZLING 219         // Surprises the opposing Pokémon, making it unable to attack using priority moves.
#define ABILITY_SOUL_HEART 220       // Boosts its Sp. Atk stat every time a Pokémon faints.
#define ABILITY_TANGLING_HAIR 221    // Contact with the Pokémon will lowers its Speed stat
#define ABILITY_RECEIVER 222         // *No innate* The Pokémon copies the Ability of a defeated ally.
#define ABILITY_POWER_OF_ALCHEMY 223 // *No innate* The Pokémon copies the Ability of a defeated ally.
#define ABILITY_BEAST_BOOST 224      // The Pokémon boosts its most proficient stat each time it knocks out a Pokémon.
#define ABILITY_RKS_SYSTEM 225       // *No innate* Changes the Pokémon's type to match the memory disc it holds.
#define ABILITY_ELECTRIC_SURGE 226   // Activates Electric Terrain for 5 turns when the Pokémon enters a battle.
#define ABILITY_PSYCHIC_SURGE 227    // Activates Psychic Terrain for 5 turns when the Pokémon enters a battle.
#define ABILITY_MISTY_SURGE 228      // Activates Misty Terrain for 5 turns when the Pokémon enters a battle.
#define ABILITY_GRASSY_SURGE 229     // Activates Grassy Terrain for 5 turns when the Pokémon enters a battle.
#define ABILITY_FULL_METAL_BODY 230  // Prevents other Pokémon's moves or abilities from lowering the Pokémon's stats.
#define ABILITY_SHADOW_SHIELD 231    // Reduces damage inflicted on this Pokémon when at max Hit Points by  50%. It is not affected by moves or abilities that would ignore the ability such as Mold Breaker, Moongeist Beam or Sunsteel Strike
#define ABILITY_PRISM_ARMOR 232      // Reduces the power of supereffective attacks taken by 35%.
#define ABILITY_NEUROFORCE 233       // Increases the power of Super Effective moves used by this Pokémon by 25%.

#define ABILITIES_COUNT_GEN7 234

// Gen 8
#define ABILITY_INTREPID_SWORD 234      // Boosts the Pokémon's Attack stat when the Pokémon enters a battle.
#define ABILITY_DAUNTLESS_SHIELD 235    // Boosts the Pokémon's Defense stat when the Pokémon enters a battle.
#define ABILITY_LIBERO 236              // Changes the Pokémon's type to the type of the move it's about to use.
#define ABILITY_BALL_FETCH 237          // *No innate* If the Pokémon is not holding an item, it will fetch the Poké Ball from the first failed throw of the battle.
#define ABILITY_COTTON_DOWN 238         // When the Pokémon is hit by an attack, it scatters cotton fluff around and lowers the Speed stat of all Pokémon except itself.
#define ABILITY_PROPELLER_TAIL 239      // *No innate* Ignores the effects of opposing Pokémon's Abilities and moves that draw in moves.
#define ABILITY_MIRROR_ARMOR 240        // Bounces back only the stat-lowering effects that the Pokémon receives.
#define ABILITY_GULP_MISSILE 241        // *No innate* When using Surf or Dive, it will change into Gulping Form if it has over 50% max HP or Gorging Form if it has less than 50% max HP. If hit afterwards, form reverts and it shoots out its prey to attack, doing 1/4th of opponent's maximum HP in damage. If the Cramorant was in Gulping Form, the opponent's Defense is lowered 1 stage. If the Cramorant was in Gorging Form, the opponent is paralyzed.
#define ABILITY_STALWART 242            // *No innate* Ignores the effects of opposing Pokémon's Abilities and moves that draw in moves.
#define ABILITY_STEAM_ENGINE 243        // Boosts the Pokémon's Speed stat drastically each time it is hit by a Fire- or Water-type move.
#define ABILITY_PUNK_ROCK 244           // Boosts the power of sound-based moves by 30%. The Pokémon also takes half the damage from these kinds of moves.
#define ABILITY_SAND_SPIT 245           // The Pokémon creates a sandstorm when it's hit by an attack.
#define ABILITY_ICE_SCALES 246          // The Pokémon is protected by ice scales, which halve the damage taken from special moves.
#define ABILITY_RIPEN 247               // Ripens Berries and doubles their effect.
#define ABILITY_ICE_FACE 248            // *No innate* Has a protective barrier that protects against Physical Attacks once and will change form to Noice Face after breaking. Confusion also breaks the barrier. Setting Hail or being sent out during hail will restore the Ice Face and change form to Ice Face
#define ABILITY_POWER_SPOT 249          // Increases the power of Ally Pokémon's attacks by 30%
#define ABILITY_MIMICRY 250             // *No innate* Changes the Pokémon's type depending on the terrain.
#define ABILITY_SCREEN_CLEANER 251      // *No innate* When the Pokémon enters a battle, the effects of Light Screen, Reflect, and Aurora Veil are nullified for both opposing and ally Pokémon.
#define ABILITY_STEELY_SPIRIT 252       // Increases the power of Steel-type moves used by the user or any Ally Pokémon by 30%
#define ABILITY_PERISH_BODY 253         // When hit by a move that makes direct contact, the Pokémon and the attacker will faint after three turns unless they switch out of battle.
#define ABILITY_WANDERING_SPIRIT 254    // The Pokémon exchanges Abilities with a Pokémon that hits it with a move that makes direct contact.
#define ABILITY_GORILLA_TACTICS 255     // Boosts the Pokémon's Attack stat but only allows the use of the first selected move.
#define ABILITY_NEUTRALIZING_GAS 256    // *No innate* If the Pokémon with Neutralizing Gas is in the battle, the effects of all Pokémon's abilities will be nullified or will not be triggered.
#define ABILITY_PASTEL_VEIL 257         // *No innate* Protects the Pokémon and its ally Pokémon from being poisoned.
#define ABILITY_HUNGER_SWITCH 258       // *No innate* Morpeko changes its form, alternating between its Full Belly Mode and Hangry Mode after the end of each turn.
#define ABILITY_QUICK_DRAW 259          // At the start of each turn, this Pokémon has a 30% chance of moving first in the Speed Priority Bracket
#define ABILITY_UNSEEN_FIST 260         // *No innate* If the Pokémon uses moves that make direct contact, it can attack the target even if the target protects itself.
#define ABILITY_CURIOUS_MEDICINE 261    // *No innate* When the Pokémon enters a battle, it scatters medicine from its shell, which removes all stat changes from allies.
#define ABILITY_TRANSISTOR 262          // Powers up Electric-type moves by 50%.
#define ABILITY_DRAGONS_MAW 263         // Powers up Dragon-type moves by 50%.
#define ABILITY_CHILLING_NEIGH 264      // *No innate* When the Pokémon knocks out a target, it utters a chilling neigh, which boosts its Attack stat.
#define ABILITY_GRIM_NEIGH 265          // *No innate* When the Pokémon knocks out a target, it utters a  terrifying neigh, which boosts its Sp. Atk stat.
#define ABILITY_AS_ONE_ICE_RIDER 266    // *No innate* This ability combines the effects of both Calyrex's Unnerve ability and Glastrier's Chilling Neigh ability/Spectrier's Grim Neigh ability.
#define ABILITY_AS_ONE_SHADOW_RIDER 267 // *No innate* This ability combines the effects of both Calyrex's Unnerve ability and Glastrier's Chilling Neigh ability/Spectrier's Grim Neigh ability.

#define ABILITIES_COUNT_LATEST_GEN 268

// Gen 9
#define ABILITY_LINGERING_AROMA 268
#define ABILITY_SEED_SOWER 269
#define ABILITY_THERMAL_EXCHANGE 270
#define ABILITY_ANGER_SHELL 271
#define ABILITY_PURIFYING_SALT 272
#define ABILITY_WELL_BAKED_BODY 273
#define ABILITY_WIND_RIDER 274
#define ABILITY_GUARD_DOG 275
#define ABILITY_ROCKY_PAYLOAD 276
#define ABILITY_WIND_POWER 277
#define ABILITY_ZERO_TO_HERO 278
#define ABILITY_COMMANDER 279
#define ABILITY_ELECTROMORPHOSIS 280
#define ABILITY_PROTOSYNTHESIS 281
#define ABILITY_QUARK_DRIVE 282
#define ABILITY_GOOD_AS_GOLD 283
#define ABILITY_VESSEL_OF_RUIN 284
#define ABILITY_SWORD_OF_RUIN 285
#define ABILITY_TABLETS_OF_RUIN 286
#define ABILITY_BEADS_OF_RUIN 287
#define ABILITY_ORICHALCUM_PULSE 288
#define ABILITY_HADRON_ENGINE 289
#define ABILITY_OPPORTUNIST 290
#define ABILITY_CUD_CHEW 291
#define ABILITY_SHARPNESS 292
#define ABILITY_SUPREME_OVERLORD 293
#define ABILITY_COSTAR 294
#define ABILITY_TOXIC_DEBRIS 295
#define ABILITY_ARMOR_TAIL 296
#define ABILITY_EARTH_EATER 297
#define ABILITY_MYCELIUM_MIGHT 298
#define ABILITY_HOSPITALITY 299
#define ABILITY_MINDS_EYE 300
#define ABILITY_EMBODY_ASPECT_TEAL 301
#define ABILITY_EMBODY_ASPECT_HEARTHFLAME 302
#define ABILITY_EMBODY_ASPECT_WELLSPRING 303
#define ABILITY_EMBODY_ASPECT_CORNERSTONE 304
#define ABILITY_TOXIC_CHAIN 305
#define ABILITY_SUPERSWEET_SYRUP 306
#define ABILITY_TERA_SHIFT 307
#define ABILITY_TERA_SHELL 308
#define ABILITY_TERAFORM_ZERO 309
#define ABILITY_POISON_PUPPETEER 310

#define ABILITIES_COUNT_GEN9 311

// Rogue
#define ABILITY_FORECAST_PRIORITY (ABILITIES_COUNT_GEN9)

// Unique Abilities
#define ABILITY_CHLOROPLAST (ABILITIES_COUNT_GEN9 + 1)       // Moves function as if it's sunny, regardless of weather // Bellossom
#define ABILITY_WHITEOUT (ABILITIES_COUNT_GEN9 + 2)          // Boosts the power of Ice attacks in Hail by 50%, 10% chance to freeze // Froslass
#define ABILITY_PYROMANCY (ABILITIES_COUNT_GEN9 + 3)         // Fire attacks are 5x more likely to inflict a burn // Chandelure
#define ABILITY_SPELL_FIST (ABILITIES_COUNT_GEN9 + 4)        // Punching moves deal 1.3x and do special damage // Golurk (spirit fist?)
#define ABILITY_BLITZ_BOXER (ABILITIES_COUNT_GEN9 + 5)       // Punching moves gain priority at full health // Hitmonchan
#define ABILITY_RAMPAGE (ABILITIES_COUNT_GEN9 + 6)           // If a recharge move KOs the foe, it doesn't need to recharge. // Rhyperior
#define ABILITY_VENGEANCE (ABILITIES_COUNT_GEN9 + 7)         // Provides a 20% damage boost to their move types. This increases to 50% while the Pokémon is at or below 1/2 HP. These boosts do not stack (the 50% boost replaces the 20% one while weakened). // rework 30 60 // spiritomb
#define ABILITY_SIGHTING_SYSTEM (ABILITIES_COUNT_GEN9 + 8)   // When the Pokémon uses a move with Accuracy of 80% or lower, the move will have 100% Accuracy, but have its priority decreased by 3. // Magnezone
#define ABILITY_BIG_WINGS (ABILITIES_COUNT_GEN9 + 9)         // Wind moves deal 1.3x and tailwind duration +1 // Braviary
#define ABILITY_POWER_CORE (ABILITIES_COUNT_GEN9 + 10)       // The Pokémon uses 25% of its Defense stat during physical moves and  Special Defense stat during special moves in addition to the default stat. (Defense and Special Defense stat stages still affects this percentage). // All def regis
#define ABILITY_ELECTROCYTES (ABILITIES_COUNT_GEN9 + 11)     // Electric type moves gain STAB, water type moves have 10% to paralyze  // Whiscash
#define ABILITY_AERODYNAMICS (ABILITIES_COUNT_GEN9 + 12)     // Immune to flying and gains +1 speed when hit with a flying type move. // Yanmega
#define ABILITY_EXPLOIT_WEAKNESS (ABILITIES_COUNT_GEN9 + 13) // Moves deal 1.5x and become dark type against pokemon with status. // Honchcrow
#define ABILITY_FIGHTING_SPIRIT (ABILITIES_COUNT_GEN9 + 14)  // Fighting moves become ghost type and gain a 1.5x boost // Gallade (got to change effect)
#define ABILITY_ANCIENT_IDOL (ABILITIES_COUNT_GEN9 + 15)     // Uses Def for Atk and SpDef for SpAtk // Claydol
#define ABILITY_SHORT_CIRCUIT (ABILITIES_COUNT_GEN9 + 16)    // Provides a 20% damage boost to their move types. This increases to 50% while the Pokémon is at or below 1/2 HP. These boosts do not stack (the 50% boost replaces the 20% one while weakened). // rework 30 60 // Rotom
#define ABILITY_ROUNDHOUSE (ABILITIES_COUNT_GEN9 + 17)       // Kicking moves can't miss and deal damage based on the lowest defensive type // Hitmonlee
#define ABILITY_SCAVENGER (ABILITIES_COUNT_GEN9 + 18)        // Heals 1/3 of health when any pokemon faints during battle. // Mandibuzz
#define ABILITY_FEAST (ABILITIES_COUNT_GEN9 + 19)            // Heals 1/2 of health when the pokemon knocks out another pokemon // Guzzlord
#define ABILITY_BRANCH_SWING (ABILITIES_COUNT_GEN9 + 20)     // Pokemon's speed is increased by 50% and becomes ungrounded on grassy terrain (but is still affected by grassy terrain) // Zarude
#define ABILITY_ARTILLERY (ABILITIES_COUNT_GEN9 + 21)        // Launcher moves hit both pokemon in double battles // Octillery
#define ABILITY_COIL_UP (ABILITIES_COUNT_GEN9 + 22)          // The Pokémon enters the battlefield coiled up. Coil: The next time the Pokémon uses a biting move, its priority is increased 1. // Arbok
#define ABILITY_MOLTEN_DOWN (ABILITIES_COUNT_GEN9 + 23)      // Fire moves deal super effective to rock types and lower their speed. // Magcargo
#define ABILITY_TOXISPHERE (ABILITIES_COUNT_GEN9 + 24)       // Set Acid Rain weather on switch in. // Weezing
#define ABILITY_LOW_BLOW (ABILITIES_COUNT_GEN9 + 25)         // Attacks with Feint Attack on switch-in. // Scrafty
#define ABILITY_VOLCANIC_RAGE (ABILITIES_COUNT_GEN9 + 26)    // Attacks with 50 BP Eruption after using a Fire-type move. // Camerupt
// #define ABILITY_COLD_REBOUND (ABILITIES_COUNT_GEN9 + 27) // Everytime when hit by a contact move, counter-attacks with Icy Wind. // Dewgong
// #define ABILITY_CHRISTMAS_SPIRIT (ABILITIES_COUNT_GEN9 + 28) // The Pokémon is protected by a mysterious force that reduces all incoming damage by 50% during a hailstorm. delibird
// #define ABILITY_INFLATABLE (ABILITIES_COUNT_GEN9 + 29) // When the Pokémon is hit by Fire-type or Flying-type moves, its Defense and Special Defense stats are increased by one stage. // drifblim
// #define ABILITY_DRAGONSLAYER (ABILITIES_COUNT_GEN9 + 30) // Increases the damage dealt against Dragon-type Pokémon by 50%. // Verizion, cobalion, terrakion
// #define ABILITY_LIQUIFIED (ABILITIES_COUNT_GEN9 + 31) // Takes half damage from contact moves. Takes 2x damage from Water-type moves. // Muk
// #define ABILITY_SELF_SUFFICIENT (ABILITIES_COUNT_GEN9 + 32) // Heals 1/8 at the end of every turn. // Torterra
// #define ABILITY_MYSTIC_POWER (ABILITIES_COUNT_GEN9 + 33) // All Pokémon's moves gain the same-type attack bonus (STAB) regardless the Pokémon's typing. // Unown (redo moveset like RR)
// #define ABILITY_DISTORTION (ABILITIES_COUNT_GEN9 + 34) // There is a 50% chance that the enemy will become confused (25%) or accuracy stat -1 (25%) when affected by the Pokémon's sound-based damaging moves. // Exploud
// #define ABILITY_LEAD_COAT (ABILITIES_COUNT_GEN9 + 35) // Triples the Pokémon's weight. Increases the Defense stat by 40%, but decreases Speed stat by 10%. // Aggron
// #define ABILITY_MAGICAL_DUST (ABILITIES_COUNT_GEN9 + 36) // When is hit by contact move, it leaves a Magic Powder that makes the target psychic type. // Rimbombee
// #define ABILITY_NOCTURNAL (ABILITIES_COUNT_GEN9 + 37) // At night or during Eclipse, Ghost-type moves deal 1.3x damage and the Pokémon can't fall asleep // Noctowl (add dark type moves)
// #define ABILITY_CHAMPION (ABILITIES_COUNT_GEN9 + 38) // Punch moves hit twice, second hit deals 0.3x. // Machamp
// #define ABILITY_STRONG_WINDS (ABILITIES_COUNT_GEN9 + 39) // Sets Tailwind for 5 turns when entering battle. // Pidgeot
// #define ABILITY_JUGGERNAUT (ABILITIES_COUNT_GEN9 + 40) // Contact moves use 20% of its Defense stat additionally. Also, the Pokémon cannot be paralyzed. // Tyrantrum
// #define ABILITY_HYPNOTIST (ABILITIES_COUNT_GEN9 + 41) // Increases Hypnosis accuracy by 50%. // Hypno
// #define ABILITY_FRIGHTEN (ABILITIES_COUNT_GEN9 + 42) // Special Intimidate. // Banette
// #define ABILITY_BAD_LUCK (ABILITIES_COUNT_GEN9 + 43) // Enemies can't critically hit and have decreased accuracy of 20%. // Absol
// #define ABILITY_SOLENOGLYPHS (ABILITIES_COUNT_GEN9 + 44) // Biting moves have a 50% chance of badly poisoning the target. // Seviper
// #define ABILITY_SPIDER_LAIR (ABILITIES_COUNT_GEN9 + 45) // Sets up Sticky Web on entry. // Ariados
// #define ABILITY_IMPENETRABLE (ABILITIES_COUNT_GEN9 + 46) // Boosts the Pokémon's Defense stat by +3 when its stats are lowered. // Bastiodon
// #define ABILITY_POISON_ABSORB (ABILITIES_COUNT_GEN9 + 47) // Restore 1/4 HP when hit by Poison-type moves. // Swalot
// #define ABILITY_TWISTED_DIMENSION (ABILITIES_COUNT_GEN9 + 48) // Summons Trick Room on entry. // Reuniculus
// #define ABILITY_AURORA_BOREALIS (ABILITIES_COUNT_GEN9 + 49) // Sets Aurora Veil on entry. Immune to Hail damage. // Aurorus
// #define ABILITY_BRUTAL_CHARGE (ABILITIES_COUNT_GEN9 + 50) // Boosts Speed by 1.5x, Attack by 1.2x on first turn (same activation requirements as Fake Out) // Granbull
// #define ABILITY_SAGE_POWER (ABILITIES_COUNT_GEN9 + 51) // Automatically locked into the first move chosen, but boosts Special Attack by 1.5x (Gorilla Tactics for Special moves) // Sage monkeys
// #define ABILITY_BONE_ZONE (ABILITIES_COUNT_GEN9 + 52) // Bone-Based moves ignore immunities and also deal double damage on not very effective. // Marowak
// #define ABILITY_HYPER_AGGRO (ABILITIES_COUNT_GEN9 + 53) // Hits twice. Second hit does 25% damage. // Haxorus
// #define ABILITY_FLOCK (ABILITIES_COUNT_GEN9 + 55) // Flying moves deal 1.3x damage and 1.6x damage at less than 1/2 HP // Fearow
// #define ABILITY_STRIKER (ABILITIES_COUNT_GEN9 + 56) // Boosts kicking moves by 1.3x. Moves affected by Striker: Blaze Kick, Jump Kick, High Jump Kick, Triple Axel, Triple Kick, Stomp, Mega Kick, Low Kick, Rolling Kick, Trop Kick, Pyro Ball, High Horsepower, Double Kick, Thunderous Kick. // Tsareena
// #define ABILITY_LUNAR_ECLIPSE (ABILITIES_COUNT_GEN9 + 57) // The Pokémon's Fairy- and Dark-type moves gain same-type attack bonus regardless the Pokémon's typing. // Cresselia
// #define ABILITY_GRIP_PINCER (ABILITIES_COUNT_GEN9 + 57) // Contact moves have 50% chance to trap the foe for two to three turns. If the target is trapped, contact moves bypass target's Defense and accuracy check. // Pinsir
// #define ABILITY_FURIOUS_FIST (ABILITIES_COUNT_GEN9 + 57) // Punching moves have critical rate increased by +1 and have double the chance of secondary effects. // Primeape
// #define ABILITY_DEADEYE (ABILITIES_COUNT_GEN9 + 57) // Never misses. // Decidueye
// #define ABILITY_AMPLIFIER (ABILITIES_COUNT_GEN9 + 57) // Sound-based moves now target all foes [Affected moves: Hyper Voice, Boomburst, Bug Buzz, Chatter, Confide, Disarming Voice, Echoed Voice, Eerie Spell, Grass Whistle, Metal Sound, Noble Roar, Parting Shot, Roar, Round, Screech, Sing, Snore, Supersonic, Uproar etc.]. // Toxtricity
// #define ABILITY_FRIGID_CREST (ABILITIES_COUNT_GEN9 + 57) // Immune to Ice-type moves. Instead of being hit by Ice-type moves, it boosts its Attack or Sp. Attack stat (which is higher). // Kingdra
// #define ABILITY_SUN_TOTEM (ABILITIES_COUNT_GEN9 + 57) // Ups highest stat on entry when it's sunny, has Fire STAB in sun. // Solrock
// #define ABILITY_MOON_TOTEM (ABILITIES_COUNT_GEN9 + 57) // Ups highest stat on entry during an eclipse, has Dark STAB in sun. // Lunatone
// #define ABILITY_VAMPIRIC (ABILITIES_COUNT_GEN9 + 57) // Contact Moves do 1.25x damage and heal the user for 1/4 of the damage dealt. // Crobat
// #define ABILITY_DISCIPLINE (ABILITIES_COUNT_GEN9 + 57) // Moves like Outrage no longer trap you. Immunity to Confusion, Intimidate and Frighten. // Arcanine
// #define ABILITY_THUNDERCALL (ABILITIES_COUNT_GEN9 + 57) // When using an Electric-type move, follows up with Smite at 20% of its power. // Thundurus
// #define ABILITY_STORMCALL (ABILITIES_COUNT_GEN9 + 57) // When using a Flying-type move, follows up with Smite at 20% of its power. // Tornadus
// #define ABILITY_EARTHCALL (ABILITIES_COUNT_GEN9 + 57) // When using a Ground-type move, follows up with Smite at 20% of its power. // Landorus
// #define ABILITY_ARCTIC_FUR (ABILITIES_COUNT_GEN9 + 57) // Takes 20% less damage from incoming physical & special moves. // Beartic
// #define ABILITY_LETHARGY (ABILITIES_COUNT_GEN9 + 57) // Atk stage drops sharply at the end of each turn. // Slaking
// #define ABILITY_FUNGAL_INFECTION (ABILITIES_COUNT_GEN9 + 57) // Every attacking move inflicts Leech Seed on the target. // Parasect
// #define ABILITY_PARRY (ABILITIES_COUNT_GEN9 + 57) // Takes 80% from Contact, then counters with Mach Punch. // Sawk
// #define ABILITY_CLINCH (ABILITIES_COUNT_GEN9 + 57) // Takes 80% from Contact, then counters by making the enemy repeat it's move for 2-3 turns. // Throh
// #define ABILITY_SHARP_QUILLS (ABILITIES_COUNT_GEN9 + 57) // when being hit by any physical move, trigger Spikes // Sandslash
// #define ABILITY_STONE_SHRAPNEL (ABILITIES_COUNT_GEN9 + 57) // when being hit by any physical move, trigger Stealth Rock // Gigalith
// #define ABILITY_SPINNING_TOP (ABILITIES_COUNT_GEN9 + 57) // Fighting-type moves raise speed by +1 and clear hazards. // Hitmontop
// #define ABILITY_KINGS_WRATH (ABILITIES_COUNT_LATEST_GEN + 141) // When a stat is lowered, boost Atk and Def by +1. Includes Ally Drops.
// #define ABILITY_QUEENS_MOURNING (ABILITIES_COUNT_LATEST_GEN + 141) // When a stat is lowered, boost Sp.Atk and Sp.Def by +1. Includes Ally Drops.
// #define ABILITY_SWEEPING_EDGE (ABILITIES_COUNT_LATEST_GEN + 153) // Slicing moves cannot miss and single-target moves hit both foes. // Kingambit
// #define ABILITY_GIFTED_MIND (ABILITIES_COUNT_LATEST_GEN + 154) // Immune to Psychic Weaknesses but take super effective from Fighting, status moves can't miss // Slowking
// #define ABILITY_HYDRO_CIRCUIT (ABILITIES_COUNT_LATEST_GEN + 155) // Electric moves deal 1.3x damage and water moves drain 1/5 of the damage dealt. // Lanturn
// #define ABILITY_CELESTIAL_SYNC (ABILITIES_COUNT_LATEST_GEN + 156) // The user Attack and Special Attack are equal to the higher of the two. // Mineor
// #define ABILITY_EXPOSED_CORE (ABILITIES_COUNT_LATEST_GEN + 156) // Same as Shields down but also gives psychic moves STAB // Mineor
// #define ABILITY_CLUELESS (ABILITIES_COUNT_LATEST_GEN + 158)         // When this Pokemon is on the field, all weather and terrains are negated. // Quagsire
// #define ABILITY_CHEAT_DEATH (ABILITIES_COUNT_LATEST_GEN + 159) // The first attack against this pokemon deals no damage.
// #define ABILITY_COWARD (ABILITIES_COUNT_LATEST_GEN + 161) // Sets up Protect on switch-in. Only works once.
// #define ABILITY_DUNE_TERROR (ABILITIES_COUNT_LATEST_GEN + 163) // Sandstorm reduces damage by 25% and boosts Ground moves by 20%. // Krookodile
// #define ABILITY_INFERNAL_RAGE (ABILITIES_COUNT_LATEST_GEN + 164)    // Fire-type moves are boosted by 30% with 5% recoil. // Houndoom
// #define ABILITY_ELEMENTALIST (ABILITIES_COUNT_LATEST_GEN + 166) // 20% chance to BRN/FRZ/PARA with respective types. // Drampa
// #define ABILITY_AMBUSH (ABILITIES_COUNT_LATEST_GEN + 167) // Guaranteed critical hit on first turn. // Zoroark
// #define ABILITY_GRAVITY_WELL (ABILITIES_COUNT_LATEST_GEN + 168) // Sets Gravity on entry for 5 turns. // Probopass
// #define ABILITY_RADIANCE (ABILITIES_COUNT_LATEST_GEN + 169) // +20% accuracy; Dark moves fail when user is on the field. // Gardevoir
// #define ABILITY_EMPATH (ABILITIES_COUNT_LATEST_GEN + 169) // Mirrors positive enemy stat changes. // Ralts, Kirlia
// #define ABILITY_EVAPORATE (ABILITIES_COUNT_LATEST_GEN + 176) // Takes no damage and sets Mist if hit by water // Torkoal
// #define ABILITY_CHLOROFUMES (ABILITIES_COUNT_LATEST_GEN + 177) // Grass-type moves lower the targets attack stage by 1 and poison if sunny // Venusaur
// #define ABILITY_DRACONIC (ABILITIES_COUNT_LATEST_GEN + 178) // Dragon type moves gain STAB. // Charizard
// #define ABILITY_PRESSURE_SHELL (ABILITIES_COUNT_LATEST_GEN + 178) // Moves that hit this pokemon use up 2 more PP and it is immune to multi hit moves. // Blastoise
// #define ABILITY_MAGIC_POWDER (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon is hit by a move, 30% chance to sleep the attacker. Flying-type and Magic Guard Pokémon are immune. // Butterfree
// #define ABILITY_VENOM_RUSH (ABILITIES_COUNT_GEN9 + XX) // If the target is poisoned or slowed, this Pokémon’s Speed is doubled and moves have +1 Crit. // Beedrill
// #define ABILITY_GNAWING_INSTINCT (ABILITIES_COUNT_GEN9 + XX) // biting moves lower the target's Defense. // Raticate
// #define ABILITY_SPARK_SURGE (ABILITIES_COUNT_GEN9 + XX) // On switch-in, sets Electric Terrain. Boosts Speed by 1.5x in Electric Terrain. // Raichu
// #define ABILITY_STARFIELD (ABILITIES_COUNT_GEN9 + XX) // Fairy-type moves hit both opponents and sound-based moves become Fairy-type. // Clefairy
// #define ABILITY_FROSTFLARE_GUARD (ABILITIES_COUNT_GEN9 + XX) // On switch in: If Sun is active, sets Light Screen, if Hail is active, sets Reflect. // Ninetales
// #define ABILITY_VOCAL_RANGE (ABILITIES_COUNT_GEN9 + XX) // Sound-based moves are Normal/Fairy dual-typed and have +1 priority if the target is statused. // Wigglytuff
// #define ABILITY_TOXIC_BLOOM (ABILITIES_COUNT_GEN9 + XX) // At the end of each turn, all foes are poisoned. // Vileplume
// #define ABILITY_NEUROTOXIN (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon hits with a Bug or Poison move, there is a 30% chance to reduce the target’s Speed and Special Defense by 1 stage. // Venomoth
// #define ABILITY_SINKHOLE (ABILITIES_COUNT_GEN9 + XX) // On switch-in, lowers the Speed of all grounded enemies by 1 stage. // Dugtrio
// #define ABILITY_TIDAL_FOCUS (ABILITIES_COUNT_GEN9 + XX) // While Rain is active, this Pokémon's moves always land critical hits. // Golduck
// #define ABILITY_FATAL_GRACE (ABILITIES_COUNT_GEN9 + XX) // This Pokémon's moves have +1 priority if the target is at or below 1/2 HP. // Persian
// #define ABILITY_UNDERCURRENT (ABILITIES_COUNT_GEN9 + XX) // This Pokémon's Water-type moves lower the target’s Speed by 1 stage. // Polywrath
// #define ABILITY_PSYCH_OVERFLOW (ABILITIES_COUNT_GEN9 + XX) // If this Pokémon’s Special Attack is raised, its Speed is also raised by the same amount. // Alakazam
// #define ABILITY_CARNIVOROUS (ABILITIES_COUNT_GEN9 + XX) // Flying- and Bug-type moves that would be “supereffective” Grass-type Pokémon will be only normally effective against the Pokémon instead. Grass-type moves used by the Pokémon will deal “supereffective” damage against Dragon-, Flying- and Bug-type. // Victreebel
// #define ABILITY_ACIDIC_MUCUS (ABILITIES_COUNT_GEN9 + XX) // Contact moves lower the attacker’s Special Defense by 2 stages. // Tentacruel
// #define ABILITY_STONEHEART (ABILITIES_COUNT_GEN9 + XX) // Takes 50% less damage from super effective moves, but status conditions last twice as long. // Golem
// #define ABILITY_HOT_PURSUIT (ABILITIES_COUNT_GEN9 + XX) // When an opponent switches out, they are hit by Flame Charge and are burned. // Rapidash
// #define ABILITY_REACTIVE (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon takes a hit, it raises the stat corresponding to the damage taken (Atk for physical, Sp.Atk for special). // Slowbro
// #define ABILITY_COMBAT_FOCUS (ABILITIES_COUNT_GEN9 + XX) // If this Pokémon's accuracy is lowered or it misses, its next move is guaranteed to crit. // Sirfetch'd
// #define ABILITY_SPLIT_INSTINCT (ABILITIES_COUNT_GEN9 + XX) // At the end of each turn, this Pokémon gains a random stat boost (excluding Accuracy/Evasion). // Dodrio
// #define ABILITY_SHELLSHOCK (ABILITIES_COUNT_GEN9 + XX) // // Opponents that hit this Pokémon with a contact move have a 20% chance to be frozen. Cannot be frozen itself.
// #define ABILITY_FRIGHTMARE (ABILITIES_COUNT_GEN9 + XX) // When an opponent switches in, their highest stat is lowered by 1 stage. // Gengar
// #define ABILITY_SUBTERRANEAN (ABILITIES_COUNT_GEN9 + XX) // 50% less damage from super effective moves when underground or in sandstorm. Cant Flinch // Steelix
// #define ABILITY_AQUATIC_ARMOR (ABILITIES_COUNT_GEN9 + XX) // After using a Water-type move, takes 50% less damage until the end of the turn. // Kingler
// #define ABILITY_VOLT_BREAK (ABILITIES_COUNT_GEN9 + XX) // On switch-in disables the enemies ability for 3 turns. Sound-based moves have +2 priority. // Electrode
// #define ABILITY_SOLARDREAM (ABILITIES_COUNT_GEN9 + XX) This Pokémon’s Grass-, Dragon-, and Psychic-type moves deal 1.3x damage and never miss in sunny weather.// Exeggutor
// #define ABILITY_SLOBBER (ABILITIES_COUNT_GEN9 + XX) // Contact moves used by this Pokémon lower the target’s Speed and Accuracy by 1 stage. // Lickitung
// #define ABILITY_NURSING (ABILITIES_COUNT_GEN9 + XX) // All allies (including the user) restore 1/10 of their HP at the end of each turn. // Blissey
// #define ABILITY_SPRAWLING_ROOTS (ABILITIES_COUNT_GEN9 + XX) // On switch-in, sets Grassy Terrain. While Grassy Terrain is active, opposing grounded Pokémon have their Speed halved. // Tangrowth
// #define ABILITY_MOTHERLODE (ABILITIES_COUNT_GEN9 + XX) // Normal-type moves used by this Pokémon heal it for 1/3 of the damage dealt. // Kangaskhan
// #define ABILITY_HORN_FORWARD (ABILITIES_COUNT_GEN9 + XX) // If this Pokémon uses a Horn-based move, its Speed is raised by 1 stage and the move bypasses Protect/Detect. // Seaking
// #define ABILITY_SUPERNOVA (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon uses a Psychic-type move, all opposing Pokémon lose 1/10 HP and their screens are removed.
// #define ABILITY_STAGECRAFT (ABILITIES_COUNT_GEN9 + XX) // After using a status move, create a random screen effect (Light Screen, Reflect or Aurora Veil if snow is in effect).
// #define ABILITY_CUT_THROUGH (ABILITIES_COUNT_GEN9 + XX) // Slicing moves ignore resistances (but not immunities).
// #define ABILITY_FEMME_FATALE (ABILITIES_COUNT_GEN9 + XX) // If this Pokémon uses a status move, its next Ice- or Psychic-type move has +1 priority and a 30% chance to freeze.
// #define ABILITY_STATIC_FURY (ABILITIES_COUNT_GEN9 + XX) // When the pokemon uses a physical Electric-type move, it gains +1 Speed. If the move KO’s a target, also gain +1 Attack.
// #define ABILITY_SMOKING_GUN (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon uses a special Fire-type move, the opposing Pokémon has their Sp. Def lowered by 1 stage. If the move KO's a target, gain +1 Special Attack.
// #define ABILITY_BULL_RUSH (ABILITIES_COUNT_GEN9 + XX) // First Normal-type move used each battle has +1 Priority and 1.3× power. Afterwards, Speed drops by 1 stage. // Tauros
// #define ABILITY_DREAD (ABILITIES_COUNT_GEN9 + XX) // At the end of each turn, lower a random stat of the opponent by 1 stage. // Gyarados
// #define ABILITY_STILL_WATER (ABILITIES_COUNT_GEN9 + XX) // At the end of each turn, if the pokemon did not take damage, it heals 1/6 of its max HP // Lapras
// #define ABILITY_COMBAT_TRANCE (ABILITIES_COUNT_GEN9 + XX) // This Pokémon is unaffected by flinching, Intimidate, and stat drops caused by opponents. // Tauros Paldean
// #define ABILITY_STABILIZE (ABILITIES_COUNT_GEN9 + XX) // On switch-in, this Pokémon removes all stat stage changes from both sides. // Ditto
// #define ABILITY_PRECOGNITION (ABILITIES_COUNT_GEN9 + XX) // On switch-in, this Pokémon identifies and disables the move with the highest base power in each opposing Pokémon’s current moveset for 3 turns. // Espeon
// #define ABILITY_SHADOW_CARAPACE (ABILITIES_COUNT_GEN9 + XX) // This Pokémon takes 0.8x from physical moves and lowers the target's Special Defense by 1 stage when hit by a special move. // Umbreon
// #define ABILITY_VERDANT_RUSH (ABILITIES_COUNT_GEN9 + XX) // While Grassy Terrain is active, this Pokémon gains 1.5x Speed and grass moves ignore resistances. // Leafeon
// #define ABILITY_GLACIAL_DUST (ABILITIES_COUNT_GEN9 + XX) // While Snow is active, this Pokémon is immune to critical hits and takes 30% less from super-effective moves. // Glaceon
// #define ABILITY_SMOLDER (ABILITIES_COUNT_GEN9 + XX) // While this Pokémon is active, all opposing Pokémon are considered burned for the purpose of damage calculation. // Flareon
// #define ABILITY_IONIZE (ABILITIES_COUNT_GEN9 + XX) // All Pokémon on the field lose 1/8 of their HP at the end of each turn if they are not Electric-type. // Jolteon
// #define ABILITY_BODY_OF_WATER (ABILITIES_COUNT_GEN9 + XX) // This Pokémon takes 0.5x damage on the first turn it's out. // Vaporeon
// #define ABILITY_BUG_FIX (ABILITIES_COUNT_GEN9 + XX) // Bug-type moves used by this Pokémon heal it for 1/2 of the damage dealt and remove the pokemon's status conditions. // Porygon
// #define ABILITY_ANCIENT_HUNGER (ABILITIES_COUNT_GEN9 + XX) // Opposing Pokémon cannot restore HP while this Pokémon is active. If Rain is active, this Pokémon’s water moves heal it for 1/2 of the damage dealt // Omastar
// #define ABILITY_ANCIENT_INSTINCT (ABILITIES_COUNT_GEN9 + XX) // This Pokémon’s slicing moves bypass protection moves (like Protect, Detect, Spiky Shield, etc.), and deal 1.3x damage // Kabutops
// #define ABILITY_SKY_CARRIER (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon knocks out a foe, Tailwind is set for 3 turns. If Tailwind is already active, its duration is extended by 1 turn instead. // Aerodactyl
// #define ABILITY_IMPASSIVE (ABILITIES_COUNT_GEN9 + XX) // While this Pokémon is active, priority moves always fail for grounded pokemon. In addition, other grounded Pokémon cannot switch out. // Snorlax
// #define ABILITY_GLACIAL_PRESENCE (ABILITIES_COUNT_GEN9 + XX) // If Snow is active, all opposing pokemon lose 1 stage of Special Attack at the end of turn. // Arcticuno
// #define ABILITY_THUNDERDOME (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon enters battle, it sets Electric Terrain for 3 turns. While Electric Terrain is active, it cannot be afflicted by status conditions. // Zapdos
// #define ABILITY_ASHEN_WINGS (ABILITIES_COUNT_GEN9 + XX) // Once per battle, when this Pokémon falls below 50% HP, it fully heals and gains 1 stage of Sp.Atk. // Ho-oh
// define ABILITY_VOLCANIC_HEART (ABILITIES_COUNT_GEN9 + XX) // This Pokémon is immune to entry hazards. When a Rock-type move is used against it, burn the user instead. // Moltres
// #define ABILITY_DRAGON_MAJESTY (ABILITIES_COUNT_GEN9 + XX) // While this Pokémon is active, opposing Pokémon’s abilities are suppressed. // dragonite
// #define ABILITY_ONE_MIND (ABILITIES_COUNT_GEN9 + XX) // This Pokémon always moves first if it uses a Psychic-type move, regardless of priority. // Mewtwo
// #define ABILITY_INFINITE_CODE (ABILITIES_COUNT_GEN9 + XX) // At the start of battle, this Pokémon randomly copies the unique ability of any Pokémon in your party.
// #define ABILITY_RESTORATIVE_AURA (ABILITIES_COUNT_GEN9 + XX) // At the end of each turn, all Pokémon on the field have their status conditions removed and heal 1/16 of their max HP. // Meganium
// #define ABILITY_ERUPTION (ABILITIES_COUNT_GEN9 + XX) // The first time this Pokémon takes damage it sets Sunlight for 5 turns and boosts its Speed by 1 stage. // Typhlosion
// #define ABILITY_DEEP_FRENZY (ABILITIES_COUNT_GEN9 + XX) // The first time this Pokémon switches in, it immediately uses a move chosen at random from its current moveset. // Feraligatr
// #define ABILITY_MANOEUVRE (ABILITIES_COUNT_GEN9 + XX) // The first time this Pokémon is hit in battle, it gains +2 Speed and +1 Attack. // Sentret
// #define ABILITY_FLUTTER (ABILITIES_COUNT_GEN9 + XX) // At the end of each turn, this pokemon gains +1 Evasion if it did not take damage. // Ledian
// #define ABILITY_DIVINE_FAVOR (ABILITIES_COUNT_GEN9 + XX) // Once per battle, it survives a hit that would KO it with 1 HP. // Togekiss
// #define ABILITY_OMNISENSE (ABILITIES_COUNT_GEN9 + XX) // This Pokémon reflects stat-lowering effects back at the source. On switch-in, it also reveals the held item of opposing Pokémon. // Xatu
// #define ABILITY_MEGA_FLUX (ABILITIES_COUNT_GEN9 + XX) // Each time this Pokémon uses an Electric-type move, it gains +1 Special Attack. If it is Mega-Evolved, this effect also boosts Speed by +1. // Ampharos
// #define ABILITY_BUBBLEBASH (ABILITIES_COUNT_GEN9 + XX) // This Pokémon’s Water- and Fairy-type moves lower the target’s Speed by 1 stage. If moving before the target, the move deals 1.2x damage. // Azumarill
// #define ABILITY_DISGUISED (ABILITIES_COUNT_GEN9 + XX) // After being hit by a move changes to Rock-type and gains +1 Attack and Defense. // Sudowoodo
// #define ABILITY_ROYAL_STORM (ABILITIES_COUNT_GEN9 + XX) // Sets rain on switch-in. While rain is active, all priority moves will fail. // Politoed
// #define ABILITY_POLLEN_PUFF (ABILITIES_COUNT_GEN9 + XX) // This Pokémon is immune to entry hazards and terrain effects. On switch-in, it lowers the Speed of all opposing Pokémon by 1 stage. // Jumpluff
// #define ABILITY_MANY_HANDS (ABILITIES_COUNT_GEN9 + XX) // After using a contact move, use Feint. // Ambipom
// #define ABILITY_SOLARBOOST (ABILITIES_COUNT_GEN9 + XX) // Sets harsh sunlight on switch-in. The first turn this Pokémon is out Grass- or Fire-type moves gain +1 priority. // Sunflora
// #define ABILITY_WITCHLIGHT (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon uses a status move, the opposing Pokémon is afflicted with either Curse, Torment, or Taunt. // Mismagius
// #define ABILITY_MIRROR_SKIN (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon is hit by a damaging move, the attacker’s highest stat is lowered by 1 stage. // Wobbufet
// #define ABILITY_BRAINWARP (ABILITIES_COUNT_GEN9 + XX) // While Trick Room is active, this Pokémon’s Special Attack and Defense are increased by 50%. // Farigiraf
// #define ABILITY_FORTIFIED_SPIN (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon uses protect, remove entry hazards on its side of the field and heal 1/8 HP. // Forretress
// #define ABILITY_BURROW (ABILITIES_COUNT_GEN9 + XX) // While grounded, this Pokémon is immune to Flying-type moves and can't be crit. // Dudunsparce
// #define ABILITY_SANDSTALK (ABILITIES_COUNT_GEN9 + XX) // While Sandstorm is active, this Pokémon’s damaging moves can't miss and ignore Protect and Substitute. // Gliscor
// #define ABILITY_TOXIC_SPINES (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon is hit by a contact move, the attacker is poisoned and set Spikes or Toxic Spikes randomly.
// #define ABILITY_SILVER_LINING (ABILITIES_COUNT_GEN9 + XX) // This Pokémon is immune to status effects. Every time it takes a hit, it restores 1/16 HP afterwards. // Shuckle
// #define ABILITY_DUALIST_INSTINCT (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon hits with a contact move, it gains a stack of Battle Instinct (max 3). Each stack boosts the power of its contact moves by 10%. At max stacks, gainst +1 Speed. // heracross
// #define ABILITY_VICTORY_FLARE (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon lands a critical hit, all stats gain a boost. // Victini
// #define ABILITY_RUSHDOWN (ABILITIES_COUNT_GEN9 + XX) // Normal- and Flying-type moves used by this Pokémon deal 1.3x damage if it moves before the target. // Staraptor
// #define ABILITY_NOXIOUS_AROMA (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon uses a status move, the target also becomes badly poisoned. // Roserade
// #define ABILITY_BONECRUSHER (ABILITIES_COUNT_GEN9 + XX) // This Pokémon’s moves deal 1.3x damage if the target has higher Defense than Special Defense. // Rampardos
// #define ABILITY_ENVIRONMENTAL (ABILITIES_COUNT_GEN9 + XX) // While terrain or weather is active, this Pokémon takes 25% less damage and its moves gain STAB for its according type. // Wormadam
// #define ABILITY_WINDWORKS (ABILITIES_COUNT_GEN9 + XX) // This Pokémon’s first damaging move has +1 priority if it is super effective. // Mothim
// #define ABILITY_ROYAL_SWARM (ABILITIES_COUNT_GEN9 + XX) // If this Pokémon used a status move, it uses Attack Order on the opposing Pokémon (50 BP, Bug-type). // Vespiquen
// #define ABILITY_MERCILESS_EDGE (ABILITIES_COUNT_GEN9 + XX) // This Pokémon’s contact moves deal 1.5x damage to targets that are Paralyzed or have lowered Speed. // Weavile
// #define ABILITY_UNYIELDING (ABILITIES_COUNT_GEN9 + XX) // This Pokémon’s stat stages cannot be lowered by opponents. Additionally, it takes 30% less damage from the first hit it receives each time it enters battle. // Ursaluna
// #define ABILITY_MOONS_JUDGMENT (ABILITIES_COUNT_GEN9 + XX) // Blood Moon ignores Protect/Detect and lowers the target’s Sp. Def by 1 stage during an Eclipse. // Ursaluna-Bloodmoon
// #define ABILITY ICEBOUND_COLOSSUS (ABILITIES_COUNT_GEN9 + XX) // While Snow is active, this Pokémon's physical moves deal 1.3× damage and it cannot flinch. // Mamoswine
// #define ABILITY_REEF_WARD (ABILITIES_COUNT_GEN9 + XX) // This Pokémon takes 0.5x damage from entry hazards and heals 1/16 HP when Terrain is active. // Corsola
// #define ABILITY_SOOTHING_SURGE (ABILITIES_COUNT_GEN9 + XX) // All ally Pokémon restore 1/16 HP when this Pokémon uses a non-damaging move. // Mantine
// #define ABILITY_RAZOR_PLATING (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon is hit by a contact move, the attacker takes 1/16 HP damage and has their Speed lowered by 1 stage. // Skarmory
// #define ABILITY_RUMBLE_ROLL (ABILITIES_COUNT_GEN9 + XX) // After using a damaging Ground-type move, this Pokémon's Defense increases by 1 stage and hazards are cleared from its side. // Donphan line
// #define ABILITY_MIND_HAZE (ABILITIES_COUNT_GEN9 + XX) // On switch-in, lowers the Special Attack of all foes by 1 stage. If a foe raises their stats, confuse them. // Wyrdeer
// #define ABILITY_PAINT_SPLATTER (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon uses a status move, its Speed is raised by 1 stage. If it uses a damaging move, its Accuracy is boosted by 1 stage. // Smeargle
// #define ABILITY_COMBAT_POTENTIAL (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon enters battle, it gains +1 in either Attack or Defense—whichever is lower. // Tyrogue
// #define ABILITY_PASTURIZED (ABILITIES_COUNT_GEN9 + XX) // This Pokémon restores its health fully the first time it uses Milk Drink or Heal Bell each battle. // Miltank
// #define ABILITY_SCORCHED_REIGN (ABILITIES_COUNT_GEN9 + XX) // Sets harsh sunlight on switch-in. In Sunlight, this Pokémon’s Defense is halved. // Entei
// #define ABILITY_LIGHTNING_FIELD (ABILITIES_COUNT_GEN9 + XX) // Sets Electric Terrain on switch-in. While on Electric terrain, this Pokémon cannot gain stat boosts. // Raikou
// #define ABILITY_MISTCALL (ABILITIES_COUNT_GEN9 + XX) // Sets Rain on switch-in. While in rain, this Pokémon’s moves deal 0.70x damage. // Suicune
// #define ABILITY_DOMINION (ABILITIES_COUNT_GEN9 + XX) // If this Pokémon is the last one on your team: It becomes immune to status conditions and its damaging moves deal 1.3× damage. // Tyranitar
// #define ABILITY_TIMESKIP (ABILITIES_COUNT_GEN9 + XX) // Once per battle, after using a move, it immediately uses the same move again. // Celebi
// #define ABILITY_MAELSTROM (ABILITIES_COUNT_GEN9 + XX) // While Rain is active, this Pokémon takes 30% less damage and its Water-type moves gain STAB. // Lugia
// #define ABILITY_BLADE_DANCE (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon uses a slicing move, it gains +1 Evasion. // Sceptile
// #define ABILITY_INFERNO (ABILITIES_COUNT_GEN9 + XX) // Contact moves have a 20% chance to burn. Burn chance is doubled if the move is a kick. // Blaziken line
// #define ABILITY_MUD_VEIL (ABILITIES_COUNT_GEN9 + XX) // While this Pokémon is grounded, it takes 0.75x damage from special moves. // Swampert line
// #define ABILITY_FULL_MOON (ABILITIES_COUNT_GEN9 + XX) // While Eclipse is active, this Pokémon has 20% evasion and always lower the target's Speed by 1 stage. // Mightyena line
// #define ABILITY_ZIGZAG (ABILITIES_COUNT_GEN9 + XX) // This Pokémon uses a random move from its known moveset instead of the selected one—but the move deals 1.5× damage. // Linoone
// #define ABILITY_ALLURE (ABILITIES_COUNT_GEN9 + XX) // This pokemon has 10% evasion. Contact moves lower the attacker’s Speed by 1 stage. // Beautifly line
// #define ABILITY_REPELLANT (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon is hit by a contact move, the attacker is badly poisoned. // Dustox line
// #define ABILITY_PERMAFROST (ABILITIES_COUNT_GEN9 + XX) // Takes half damage from Fighting, Rock, and Steel. // Avalugg
// #define ABILITY_POSITIVE_FEEDBACK (ABILITIES_COUNT_GEN9 + XX) // When an ally with Minus faints, this Pokémon immediately uses Nuzzle on the opposing enemy. // Plusle
// #define ABILITY_NEGATIVE_REACTION (ABILITIES_COUNT_GEN9 + XX) // When an ally with Plus faints, this Pokémon’s Electric-type moves hit both foes. // Minun
// #define ABILITY_FIESTA (ABILITIES_COUNT_GEN9 + XX) // When Rain or grassy terrain is active, this Pokémon gains +1 Speed when it uses a sound-based move. // Ludicolo line
// #define ABILITY_FOREST_AMBUSH (ABILITIES_COUNT_GEN9 + XX) // During an eclipse or on grassy terrain, the first time this Pokémon uses a status move in battle, it gains +1 Evasion and flinches the target. // Shiftry line
// #define ABILITY_AERIAL_BRAVERY (ABILITIES_COUNT_GEN9 + XX) // If this Pokémon uses a Flying-type move while damaged, it gains +1 Attack. // Swellow line
// #define ABILITY_MONSOON (ABILITIES_COUNT_GEN9 + XX) // Sets Rain on switch-in. While Rain is active, this Pokémon’s moves deal 0.9x damage but can't miss. // Pelipper line
// #define ABILITY_WATER_GLIDE (ABILITIES_COUNT_GEN9 + XX) // While Rain is active, this Pokémon's Flying- and Water-type moves have STAB and lower the target’s Special Defense by 1 stage. // Masquerain line
// #define ABILITY_SPORECLOUD (ABILITIES_COUNT_GEN9 + XX) // At the end of each turn, all other Pokémon have a 10% chance to fall asleep. // Breloom
// #define ABILITY_INVISIBLE_SCOUT (ABILITIES_COUNT_GEN9 + XX) // Once per battle on switch-in, this Pokémon Flies into the air. If it attacks while in this state, its move has +1 priority and 1.3× power. // Ninjask
// #define ABILITY_UNMOVABLE (ABILITIES_COUNT_GEN9 + XX) // This Pokémon cannot be forced to switch out. Takes 30% less damage from the first hit it receives each time it enters battle. // Hariyama
// #define ABILITY_HEARTBREAK (ABILITIES_COUNT_GEN9 + XX) // This Pokémon’s damaging moves have 30% chance to infatuate the target which works regardless of gender. Infatuated targets lose 1/8 HP each turn. // Delcatty
// #define ABILITY_SHADOW_TRICKS (ABILITIES_COUNT_GEN9 + XX) // If this Pokémon uses a status move, its next damaging move has +1 priority and deals 1.3x damage. // Sableye
// #define ABILITY_MEGAJAW (ABILITIES_COUNT_GEN9 + XX) // Biting moves always hit and ignore defensive stat boosts. If the target is Fairy-type, damage is doubled. // Mawile
// #define ABILITY_CHAKRA (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon uses a Status move, it heals 1/8 max HP and gains +1 Speed. // Medicham
// #define ABILITY_ARCLASH (ABILITIES_COUNT_GEN9 + XX) // If this Pokémon moves before the target, its Electric-type moves deal 1.2× damage. If it moves after, the move always paralyzes. // Manectric
// #define ABILITY_LUMINESCENCE (ABILITIES_COUNT_GEN9 + XX) // While Eclipse is active, gain +1 Evasion when this Pokémon uses a status move. // Volbeat
// #define ABILITY_TWILIGHT_VEIL (ABILITIES_COUNT_GEN9 + XX) // While Eclipse is active, all allied Pokémon take 30% less damage from special moves.
// #define ABILITY_FEEDING_FRENZY (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon uses a biting move, it the opponents Defense is lowered by 1. If Rain is active, it also loses 1 Speed. // Sharpedo
// #define ABILITY_TIDAL_FLOOD (ABILITIES_COUNT_GEN9 + XX) // On switch-in, clears hazards and restores 1/4 HP. // Wailord
// #define ABILITY_PSYCHIC_SPRING (ABILITIES_COUNT_GEN9 + XX) // On switch-in, this Pokémon sets Psychic Terrain. While Psychic Terrain is active, all moves have a 10% chance to confuse the target. // Grumpig
// #define ABILITY_GETTING_DIZZY (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon is hit by a move, the attacker has a 50% chance to become confused. // Spinda
// #define ABILITY_DESERT_PHANTOM (ABILITIES_COUNT_GEN9 + XX) // While Sandstorm is active, sound-based moves become Ground-type deal and 1.5× damage.
// #define ABILITY_NEEDLEBURST (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon uses a contact move, it also sets a layer of Spikes. Immune to Sandstorm damage. // Cacturne
// #define ABILITY_CELESTIA (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon enters battle, it gains +1 Sp.Def. If it is hit by a status move, it heals 1/8 of its HP. // Altaria
// #define ABILITY_ANTIVENOM (ABILITIES_COUNT_GEN9 + XX) // This Pokémon’s moves always crit against Poison-type targets. If it KOs a Poison-type, it heals 1/4 of its max HP. // Zangoose
// #define ABILITY_ROGUE_CURRENT (ABILITIES_COUNT_GEN9 + XX) // Water- and Dark-type moves used by this Pokémon ignore stat changes on the target.
// #define ABILITY_PETRIFYING_ROOTS (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon is hit by a contact move, the attacker’s Speed is lowered by 1 stage. If Grassy Terrain is active, also lower Attack. // Cradily
// #define ABILITY_RAZOR_CARAPACE (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon is hit by a contact move, it retaliates with a 40 BP physical Bug-type attack. // Armaldo
// #define ABILITY_NARCISSISM (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon is targeted by a status move, Defense and Sp. Defense are raised by 1 stage. // Milotic
// #define ABILITY_CLIMATIC_BURST (ABILITIES_COUNT_GEN9 + XX) // On switch-in, this Pokémon sets a random weather condition if there is none (Sun, Rain, Snow, Acid Rain or Eclipse).
// #define ABILITY_CHAMELEON_CURSE (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon is hit by a super-effective move, it faints. // Kecleon
// #define ABILITY_GRAVEKEEPER (ABILITIES_COUNT_GEN9 + XX) // On switch-in, sets Haunted Grounds for 5 turns. While Haunted Grounds is active, opposing Pokémon cannot restore HP // Dusknoir
// #define ABILITY_RIPE_FRUIT (ABILITIES_COUNT_GEN9 + XX) // At the end of each turn, this Pokémon restores 1/16 of its HP. If Sunlight is active, restore 1/8 instead. // Tropius
// #define ABILITY_RESONANCE_FIELD (ABILITIES_COUNT_GEN9 + XX) // On switch-in, this Pokémon grants +1 Sp. Def to all allies. // Chimecho
// #define ABILITY_FROSTLOCK (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon is hit by a contact move, the attacker’s held item becomes unusable for 3 turns. // Glalie
// #define ABILITY_HYPOTHERMIA (ABILITIES_COUNT_GEN9 + XX) // Opposing Pokémon that are statused deal 30% less damage to this Pokémon. // Walrein
// #define ABILITY_DEEP_SEA_PRESSURE (ABILITIES_COUNT_GEN9 + XX) // While this Pokémon is active, opposing Pokémon cannot raise their stats. // Huntail
// #define ABILITY_SIRENS_CALL (ABILITIES_COUNT_GEN9 + XX) // Opposing Pokémon lose 1/4 HP at the end of each turn if they have raised stats. // Gorebyss
// #define ABILITY_LAST_SURVIVOR (ABILITIES_COUNT_GEN9 + XX) // Inherits stat boosts when a Pokémon faints. // Relicanth
// #define ABILITY_HEARTSTRING (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon uses a status move, its Speed is raised by 1 stage. // Luvdisc
// #define ABILITY_DOMINANCE (ABILITIES_COUNT_GEN9 + XX) // If it moves first, this Pokémon’s moves can't miss. If it moves last, deals 1.2x damage. // Salamence
// #define ABILITY_NEURAL_OVERRIDE (ABILITIES_COUNT_GEN9 + XX) // This Pokémon’s moves ignore protection. // Metagross
// #define ABILITY_SOULGUARD (ABILITIES_COUNT_GEN9 + XX) // If an ally is below half HP, this Pokémon gains +1 Defense and Special Defense at the end of each turn. // Latias
// #define ABILITY_MINDFANG (ABILITIES_COUNT_GEN9 + XX) // If the target’s HP is above half, this Pokémon’s damaging moves deal 1.3× damage. // Latios
// #define ABILITY_ABYSSAL_CROWN (ABILITIES_COUNT_GEN9 + XX) // On switch-in: Sets Rain and Misty Terrain. // Kyogre
// #define ABILITY_TECTONIC_THRONE (ABILITIES_COUNT_GEN9 + XX) // On switch-in: Sets Sun and Grassy Terrain // Groudon
// #define ABILITY_SKY_SOVEREIGN (ABILITIES_COUNT_GEN9 + XX) // On switch-in: Sets Tailwind. // Rayquaza
// #define ABILITY_ASTRAL_PRAYER (ABILITIES_COUNT_GEN9 + XX) // Wish heals a Pokémon to full health // Jirachi
// #define ABILITY_STELLAR_MUTATION (ABILITIES_COUNT_GEN9 + XX) // When this Pokémon is hit by a damaging move, randomly boosts one stat by +1 and lowers another by -1.

#define ABILITIES_COUNT_CUSTOM (ABILITY_VOLCANIC_RAGE + 1)

#define IS_FORECAST_ABILITY(a) (a == ABILITY_FORECAST || a == ABILITY_FORECAST_PRIORITY)
#define ABILITIES_COUNT ABILITIES_COUNT_CUSTOM

#endif // GUARD_CONSTANTS_ABILITIES_H
