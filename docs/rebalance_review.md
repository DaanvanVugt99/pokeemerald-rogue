# Upstream 2.2 Rebalance Review

This ledger records design decisions made while comparing Divergence with
upstream 2.2's experimental Revised Mode. Decisions remain pending unless
explicitly marked implemented below; approving a row does not by itself change
runtime data.

Upstream comparison commit: `c0ea02119f`

## Implemented targeted Ability decisions — 2026-09-08

- [x] Huntail: Swift Swim / Intimidate / Water Veil.
- [x] Gorebyss: Swift Swim / Dazzling / Hydration.
- [x] Gigalith: verified the existing Sturdy / Stamina / Sand Force arrangement
  and its competitive-set remap; no additional change needed.
- [x] Volbeat: Illuminate / Tinted Lens / Swarm.
- [x] Illumise: Oblivious / Tinted Lens / Swarm.
- [x] Shieldon and Bastiodon: Sturdy / Solid Rock / Soundproof; Impenetrable
  now negates super-effective special moves. Physical attacks remain unaffected
  by Impenetrable, and Mold Breaker can bypass it.
- [x] Slakoth and Slaking: Truant / empty / Slow Start; King's Domain suppresses
  the regular Ability when the party shares a type, including both drawbacks.
  Vigoroth is unchanged in this implementation batch.
- [x] Regenerated species reports and baked data; regenerated Volbeat/Illumise
  competitive sets to remove Prankster, with no unrelated profile changes.

Only these Ability decisions are crossed off. Their separately approved stats,
typing, and learnsets remain pending, including Volbeat/Illumise's 465-BST
spreads. The starter and Eeveelution audit tables below are also still approval
records, not implementation status.

Validation passed: four Impenetrable tests, four King's Domain tests, competitive
profile Ability legality, species/bake verification, deterministic profile
verification, the normal debug ROM build, and `git diff --check`. The legacy
profile project did not compile with the installed Mono toolset; refresh and
verification used the same pipeline sources compiled as a profile-only runner.

## Move-distribution audit — approved revisions

Approved on 2026-09-08, including the tentative cut recommendations. These
revise the pending decisions below; they do not mark learnsets implemented.
Remove the newly proposed additions rather than replacing each with another
coverage move. Preserve existing move access and unrelated approved changes.

- Mystical Fire: skip Meowstic (both genders and Megas), Cursola, and Sunflora.
  Sunflora also skips Flamethrower; retain Fiery Dance and Heat Wave, with Heat
  Wave serving its doubles option.
- Power Gem: skip Clefable, Elgyem, Beheeyem, and Delcatty.
- Hurricane: skip Beautifly, Dustox, Venomoth, Swoobat, and Jumpluff. Other
  approved recipients and evolutionary access extensions remain approved.
- Coverage packages: skip Energy Ball and Flash Cannon on Ampharos and the
  Voltorb line, plus Aura Sphere on Electrode. Retain Bug Buzz on Voltorb and
  Electrode, but skip it on Chatot and the Magnemite line.
- Other cuts: Apple Acid on Bellossom; Fiery Dance on Lilligant; Scale Shot on
  Chewtle/Drednaw; Mach Punch on Tarountula/Spidops.
- Retain the approved Head Smash additions. Cubone/Marowak was an optional
  reconsideration, not a recommended cut.

## Learnset accumulation audit — approved corrections

Approved on 2026-09-08. Move access should normally accumulate through evolution,
including regional branches, even when level-up timing differs. Preserve direct
learning access as well as the ability to carry a learned move through evolution.
Apply approved access consistently to associated Mega and Gigantamax forms.

The individual rows below now include the identified missing moves: Headbutt on
Metang/Metagross; Mystical Fire on Quilava/Hisuian Typhlosion; Low Sweep/Revenge
on Hisuian Samurott; Dual Chop on Electivire; Natural Gift on both Exeggutor forms;
Grudge/Hypnosis on Mismagius; Sucker Punch on Pangoro; Hurricane on Tranquill;
Sing on Prinplup; Steel Wing on Hisuian Braviary; Air Cutter on Drizzile/Togekiss;
Poison Fang on Ariados; and Bubble on Grapploct.

Reverse the conflicting rejections of Crunch on Purugly, Lunge on Parasect, and
Head Smash on Alolan Marowak. Add Tearful Look to Hisuian Sliggoo. Withdraw the
proposed Recover addition from shared pre-evolution Goomy; retain it on regular
Sliggoo/Goodra only, preserving the rejection on the Hisuian branch.

These are pending learnset decisions, not implemented changes or a certification
that every learnset is gap-free. During implementation, check cumulative access
automatically and document any intentional exceptions explicitly.

## Typing audit — approved revisions

Approved on 2026-09-08. Typing should express a specific identity, not merely
reward access to matching moves or supply convenient resistances. These revise
pending design decisions; no runtime typing changes are marked implemented here.
Previously approved stats, Abilities, and moves remain unchanged by this audit.

| Pokemon | Final approved typing after audit |
|---|---|
| Ampharos | Electric; Mega remains Electric/Dragon |
| Beartic | Ice |
| Bellossom | Grass |
| Feraligatr | Water; Mega remains Water/Dragon |
| Skiddo / Gogoat | Grass |
| Goodra | Dragon, reverting Divergence's Poison addition; Hisuian Goodra remains Dragon/Steel |
| Gothita / Gothorita / Gothitelle | Psychic |
| Snubbull / Granbull | Fairy |
| Lycanroc (Midday) | Rock |
| Meganium | Grass; Mega remains Grass/Fairy |
| Sceptile | Grass; Mega remains Grass/Dragon |
| Ursaring | Normal; Ursaluna forms remain unchanged |

The user explicitly retains Gorebyss's Water/Fairy, Huntail's Water/Dragon,
Mega Banette's approved Ghost/Normal, Dodrio's Ground/Flying, and Seviper's
approved Poison/Dark. Do not apply the suggested reversions to these five.

Retain all other positively reviewed typings, including the lake trio's
Psychic/Fairy, Ninetales's Fire/Fairy, Mismagius's Ghost/Fairy, Luvdisc's
Water/Fairy, and the existing Illumise/Volbeat and Furfrou design systems.
Pignite/Emboar's deliberately chosen Fire/Ground design remains approved, with
Mega Emboar Fire/Fighting: the audit flagged this for reconsideration but did not
recommend a definite reversal. No replacement typing or move removals are implied.

## Remaining Upstream Revised Pokemon

This list covers upstream Revised Mode profiles not included in the initial
Divergence-design review.

Profiles containing only generic legacy compatibility additions such as Toxic,
Return, Frustration, and Hidden Power are automatically skipped and keep their
Divergence design; they do not consume a review-batch slot.

| Pokemon | Decision |
|---|---|
| Absol | Keep Divergence as-is for now; reassess only after Mega Absol Z's strength is established. Skip upstream's stats and tutor additions. |
| Accelgor | Change to Bug/Dark; replace Hydration with Infiltrator; add Dark Pulse and Me First by level; retain all existing moves removed by upstream and Divergence's existing additions. |
| Aegislash | Port upstream's Swirling Blade move and add it at level 1; retain Bitter Blade, Authority, and Eclipse. |
| Aerodactyl | Add Brave Bird as a late-level move; retain Stone Edge, Accelerock, and Jetstream; leave the elemental fangs and Sky Drop as tutors rather than duplicating upstream's level placements. |
| Aggron | Replace Heavy Metal with Earth Eater; retain current stats for Aggron and Mega Aggron, skipping upstream's +0/+0/+0/+0/+20/+0 changes; retain Iron Shards. |
| Aipom | Add Quick Attack at level 1; retain Upper Hand, Plain Terrain, Handywork, and current stats. |
| Alakazam | Add Aura Sphere, Miracle Eye, and Telekinesis by level plus Charge Beam as a tutor; retain Psyshock, Role Play, Clairvoyant, Brainstorm, and Eclipse; leave Trick as tutor-only. |
| Alcremie | Keep Divergence as-is; skip Toxic, Return, and Frustration; retain Sugar Coat and Alluring Voice. |
| Alomomola | Keep Divergence as-is; skip Toxic; retain Healing Current and Alluring Voice. |
| Altaria | Keep Divergence as-is for both forms; skip upstream's +0/+20/+0/+20/+0/+0 changes, Astonish, and Natural Gift; leave Brave Bird, Dragon Dance, and Refresh as tutors; retain Sparkling Aria and Cotton Guarding. |
| Ambipom | Add Arm Thrust, Dual Chop, and Quick Attack by level plus Pursuit as a tutor; skip upstream's +0/+5/+0/+8/+0/+0 stats; retain Upper Hand, Plain Terrain, and Handywork. |
| Ampharos | Keep base Ampharos pure Electric, reserving Electric/Dragon for Mega; use Static / Overcoat / Cotton Down; retain Mold Breaker on Mega Ampharos; add Signal Beam by level plus Draco Meteor as a tutor; retain Dazzling Gleam, Ion Deluge, Beacon, Buzzy Buzz, and Static Burst; skip Energy Ball, Flash Cannon, Tail Glow, Rock Climb, Transistor, and upstream's +5/+5/+5/+0/+0/+0 stats for both forms. |
| Annihilape | Use Defiant / Anger Point / Vital Spirit, replacing Inner Focus; retain Tantrum, Eclipse, and current stats. |
| Anorith | Add Lunge as a tutor; skip Bug Buzz and Rock Climb; retain Fossil Drill, Acid Rain, Infested Terrain, and current stats. |
| Appletun | Use upstream's +0/+0/+5/+5/+5/+0 stats for Appletun and its Gigantamax form; add Superpower as a tutor; skip Toxic, Return, and Frustration; retain Orchard and Wild Growth. |
| Applin | Add Leaf Storm as a tutor; retain Orchard, Wild Growth, and current stats. |
| Arcanine | Add Hone Claws as a tutor to regular Arcanine; retain Immolate and current stats. |
| Arceus | Add Punishment, Roar of Time, Shadow Force, and Spacial Rend by level; retain Healing Wish, Creation, and Divergence's existing tutors; skip Natural Gift and leave Refresh as tutor-only. |
| Arctovish | Keep Divergence as-is; skip Toxic, Return, Frustration, and Hidden Power; retain Fossil Memory. |
| Arctozolt | Keep Divergence as-is; skip Toxic, Return, Frustration, and Hidden Power; retain Fossil Memory. |
| Armaldo | Add First Impression by level plus Lunge and Rock Climb as tutors; skip Bug Buzz, Mud Sport, and Scratch; retain Fossil Drill, Acid Rain, Infested Terrain, and current stats. |
| Aron | Replace Heavy Metal with Earth Eater; retain Headlong Rush, Iron Shards, and current stats. |
| Arrokuda | Keep Divergence as-is; skip Toxic, Return, and Frustration; retain Red Wake. |
| Articuno | Add Extrasensory as a tutor; retain Frost Call and current stats. |
| Articuno (Galarian) | Keep Divergence as-is; skip Extrasensory, Toxic, Return, and Frustration; retain Psychic Noise and Cold Premonition. |
| Axew | Keep Divergence as-is; skip the legacy Rock Climb and Work Up tutors; retain Brutal and current stats. |
| Azelf | Change to Psychic/Fairy; add Moonblast by level; leave Play Rough as tutor-only; skip Natural Gift; retain Psychic, Psybeam, Mystical Power, Power Shift, Iron Will, and current stats. |
| Azumarill | Use +0/+0/+0/+20/+0/+0, skipping upstream's additional 10 HP; skip Drain Punch; retain Buoyancy and Alluring Voice. |
| Azurill | Use +0/+0/+0/+20/+0/+0; retain Buoyancy and Alluring Voice. |
| Bagon | Add Wish as a tutor; skip Work Up; retain Predator and current stats. |
| Baltoy | Add Recover and Heal Block by level plus Power Gem as a tutor; skip Nasty Plot; retain Mystical Power, Eclipse, Ancient Idol, and current stats. |
| Barboach | Replace Oblivious with Simple; add Breaking Swipe as a tutor; retain Magnitude, Electrocytes, and current stats. |
| Barraskewda | Keep Divergence as-is; skip Toxic, Return, and Frustration; retain Red Wake. |
| Basculin (Red-Striped) | Keep Divergence as-is; skip upstream's +20/+0/+0/+10/+0/+0 stats, Aqua Tail, Chip Away, and Uproar; leave Psychic Fangs as tutor-only; retain Wave Crash and Death Current. |
| Basculin (Blue-Striped) | Keep Divergence as-is; skip upstream's +20/+0/+0/+10/+0/+0 stats; retain Wave Crash and Death Current. |
| Basculin (White-Striped) | Keep Divergence as-is; skip upstream's +20/+0/+0/+10/+0/+0 stats; retain Death Current and Eclipse. |
| Bastiodon | Add Solid Rock in slot 2 and Head Smash by level; redesign Impenetrable so super-effective special moves are completely negated; retain Soundproof, Power Shift, every existing move, and current stats; skip upstream's +0/+20/+0/-20/+0/+0 redistribution, Slack Off, and Toxic. |
| Bayleef | Use Overgrow / Filter / Triage; add Aromatherapy by level; skip Draining Kiss, Moonblast, and Natural Gift; retain Giga Drain, Leech Seed, Jungle Healing, Flower Field, and current stats. |
| Beartic | Keep pure Ice, skipping the Fighting addition; use Swift Swim / Slush Rush / Tough Claws; add Ice Shard by level plus Triple Axel and Waterfall as tutors; skip upstream's +10/+0/+0/+0/+0/+5 stats, Bide, Rock Smash, Hail, Low Sweep, and Work Up; retain moves removed by upstream, Ice Floe, and Divergence's custom moves. |
| Beedrill | Add Drill Peck and Megahorn by level plus Dual Wingbeat as a tutor; leave Drill Run and X-Scissor as tutor-only and skip Skitter Smack; skip base Beedrill's +0/+25/+0/-5/+15/+20 and Mega Beedrill's +0/+25/+15/+15/+30/+25 stats; retain Toxic Tandem and Divergence's existing additions. |
| Beldum | Add Rock Head in slot 2 and Headbutt as a tutor; retain Hive Mind and current stats. |
| Bellossom | Keep pure Grass, skipping the Fairy addition; use Chlorophyll / Cloud Nine / Healer; add Draining Kiss, Fiery Dance, and Leaf Blade by level; retain Moonblast, Moonlight, all moves removed by upstream, Festival, and current stats; skip Apple Acid, Earth Power, and the remaining level reshuffle. |
| Bibarel | Use upstream's +15/+15/+5/+0/+5/+0 stats; skip Rock Climb; retain Spillway. |
| Bisharp | Keep Divergence: retain Retaliate, Power Play, Eclipse, the existing learnset, and current stats; skip Leaf Blade, Psycho Cut, Air Slash, Embargo, and Feint Attack. This prevents an evolved Bisharp from carrying extra coverage into the already exceptional Kingambit. |
| Blastoise | Use Torrent / Rain Dish / Shell Armor on regular and Gigantamax Blastoise while retaining Mega Launcher on Mega Blastoise; skip upstream's +0/+0/+0/+5/+0/+0 stats for all forms, Scald, and Toxic; retain Shell Formation. |
| Blaziken | Add Early Bird in slot 2; skip upstream's +0/+0/+5/+0/+0/+0 stats for both forms; retain Blaze, Speed Boost, Flash Firestorm, and Divergence's kicking moves. |
| Blitzle | Add Flame Wheel, Flare Blitz, High Horsepower, and Nuzzle by level; skip Bounce, Lunge, Pursuit, Flamethrower, and Low Sweep; retain Zippy Zap, Volt Tackle, Redline, the custom tutors, and current stats. |
| Boldore | Keep Divergence as-is; skip upstream's +0/-55/+0/+55/+0/+0 redistribution, Solar Power, and wholesale special movepool; retain Sand Force, Diamond Storm, Geode Heart, and the physical Rock moves upstream removes. |
| Bonsly | Use upstream's +10/+5/+0/+0/+10/+0 stats; retain Divergence's Grass typing and Fauxliage. |
| Bouffalant | Use upstream's +0/+0/+0/+0/+0/+10 stats; add Thrash by level plus Body Press, Flame Charge, Head Smash, and Heat Crash as tutors; skip Rage; leave Body Slam and Pursuit as tutor-only; retain Tackle, Throat Chop, Plain Terrain, and Battle Fury. |
| Braixen | Use Blaze / Magician / Magic Guard; retain Spellbook, Torch Song, and current stats. Reassess consistency when reviewing Delphox. |
| Braviary | Add Quash and Steel Wing as tutors; retain Warpath, the existing learnset, and current stats. |
| Braviary (Hisuian) | Add Steel Wing as a tutor to preserve Rufflet's approved access; otherwise retain existing Abilities, moves, typing, and stats. |
| Breloom | Add Mind Reader, Sky Uppercut, and Spore by level; retain Growth, Poison Powder, Toxic, Worry Seed, Brick Break, Focus Punch, Adrenaline, and current stats. |
| Bronzong | Add Teleport and Fly as tutors; retain Doom Desire, Psyshield Bash, Brainstorm, Eclipse, Equilibrium, and current stats. |
| Bronzor | Add Teleport as a tutor; retain Doom Desire, Psyshield Bash, Brainstorm, Eclipse, Equilibrium, and current stats. |
| Bruxish | Use upstream's +0/+0/+0/-10/+10/+10 stats; retain Dazzling, Strong Jaw, Wonder Skin, and Psychic Maw. |
| Buizel | Add Tail Slap as a tutor; retain Swift Swim / empty slot 2 / Water Veil, Undertow, Tackle, Soak, Liquidation, Wave Crash, Aqua Step, Surging Strikes, and current stats; skip Technician and upstream's other move additions. |
| Bulbasaur | Add Sweet Veil in slot 2; retain Overgrow, Chlorophyll, Vine Lash, Acid Rain, and current stats. |
| Buneary | Add Frustration at level 1 plus Jump Kick and Dizzy Punch by level; retain Double Kick, Headbutt, Flatter, Bunny Ears, Plain Terrain, and current stats; skip Foresight, Endure, and Double-Edge. |
| Bunnelby | Use upstream's +7/+2/+2/-2/+4/+0 stats; retain Work Crew and Plain Terrain. |
| Burmy (all cloaks) | Add Battle Armor in slot 2; retain current stats, skipping upstream's +0/-9/+0/+16/+0/-6 redistribution; retain Shed Skin, Overcoat, Environmental, and the custom tutors. |
| Butterfree | Use upstream's +5/+0/+10/+10/+10/+20 stats for regular and Gigantamax Butterfree; retain Compound Eyes, Tinted Lens, Sleep Dust, and the custom terrain tutors. |
| Cacnea | Add Rough Skin in slot 2; make Needle Arm a level-up move; add Scorching Sands as a tutor; skip Feint Attack and Rock Climb while retaining Power Trip, Needleburst, Ceaseless Edge, Eclipse, Nightfall, Wild Growth, and current stats. |
| Calyrex | Keep Divergence as-is; skip Toxic, Return, and Frustration; retain Bountiful Reign and current stats. |
| Calyrex (Ice Rider) | Keep Divergence as-is; skip Toxic, Return, and Frustration; retain Tundra Reign, Aromatherapy, and current stats. |
| Calyrex (Shadow Rider) | Keep Divergence as-is; skip Toxic, Return, and Frustration; retain Astral Reign and current stats. |
| Camerupt | Use Magma Armor / Simple / Solid Rock on regular Camerupt and retain Sheer Force on Mega Camerupt; add Slack Off by level and Power Gem as a tutor; retain Bulldoze, Incinerate, Searing Shot, Volcanic Rage, and current stats, skipping upstream's +20/+10/+0/+0/+0/+0 base stats and +20/+0/+0/+10/+0/+0 Mega stats; skip Flame Burst, Growth, and Rock Climb. |
| Capsakid | Keep Divergence as-is; skip Solar Blade and upstream's +0/+0/+0/-12/+0/+12 redistribution; retain Capsaicin Craze. |
| Carkol | Add Lava Plume, Power Gem, and Burn Up by level; retain Stone Edge, Skyfire, Acid Rain, and current stats; skip Toxic, Return, and Frustration. |
| Centiskorch | Use upstream's +0/+0/+0/-25/+0/+25 stats for regular and Gigantamax Centiskorch; retain Sizzly Slide at Divergence's level 40 placement, Molten Burrow, Coil, Fire Lash, Lunge, and the custom terrain tutors; skip Toxic, Return, Frustration, and Hidden Power. |
| Chandelure | Add Mystical Fire and Dazzling Gleam as tutors; retain Pyromancy, Eerie Spell, Eclipse, Nightfall, and current stats. |
| Charizard | Use Blaze / Defiant / Solar Power on regular and Gigantamax Charizard; add Dual Wingbeat as a tutor; retain Mega Charizard's existing Abilities, Thermal Lift, Tailwind, and current stats, skipping upstream's +0/+0/+0/+1/+0/+0 regular and Gigantamax stats, +0/+1/+0/+0/+0/+0 Mega X stats, and +0/+0/+0/+1/+0/+0 Mega Y stats. |
| Charjabug | Use +0/+20/+0/+0/+0/+0, skipping upstream's 10 HP and 20 Speed increases; retain Battery, Dynamo, and the custom terrain tutors. Keeping only Attack preserves its physical cocoon niche without making evolution into unchanged Vikavolt lose Speed. |
| Charmander | Add Defiant in slot 2; retain Blaze, Solar Power, Thermal Lift, and current stats. |
| Charmeleon | Add Defiant in slot 2; retain Blaze, Solar Power, Thermal Lift, and current stats. |
| Chatot | Use Soundproof / Tangled Feet / Punk Rock; add Air Slash, Role Play, and Boomburst by level; retain Relic Song, Chatterbox, and current stats; skip Bug Buzz and upstream's +0/+0/+5/+12/+8/+5 stats. |
| Cherrim | Use upstream's +0/+40/+0/+0/+5/+5 stats for both forms; add Sleep Powder, Stun Spore, and Solar Blade by level; retain Flower Shield, Leafage, and Bloom Burst; skip Lucky Chant and X-Scissor. |
| Cherubi | Use upstream's +10/+0/+15/+0/+0/+5 stats; add Sleep Powder, Stun Spore, and Sunny Day by level; retain Leafage and Bloom Burst; skip Solar Blade, Lucky Chant, and Play Rough. |
| Chesnaught | Use upstream's +5/+0/+0/+0/+0/+0 stats for regular and Mega Chesnaught, producing 93/137/172/74/115/44 (BST 635) on Mega; add Iron Barbs in slot 2 to regular Chesnaught while retaining Mega's Bulletproof; make Needle Arm a level-up move; add Horn Leech as a tutor; retain Bramble Guard and Spiky Shield; skip Belly Drum and Rock Climb. |
| Chespin | Add Iron Barbs in slot 2 plus Needle Arm and Bulk Up by level; retain Bramble Guard and current stats; skip Stone Edge, Horn Leech, and Rock Climb. |
| Chewtle | Add Aqua Jet and Shell Smash by level; retain Ripjaw, the existing level moves, and current stats; skip Scale Shot and upstream's remaining tutors. |
| Chikorita | Use Overgrow / Filter / Triage; add Aromatherapy by level; retain Leech Seed, Giga Drain, Jungle Healing, Flower Field, and current stats; skip Natural Gift, Draining Kiss, Moonblast, Dazzling Gleam, and Play Rough. |
| Chimchar | Add Flame Body in slot 2; retain Blaze, Iron Fist, Spellfist, and current stats; skip Rock Climb. |
| Chinchou | Add Signal Beam and Parabolic Charge by level; retain Take Down, Splishy Splash, Abyssal Light, and current stats; leave Ion Deluge as tutor-only and skip Bubble. |
| Chingling | Use upstream's +0/+0/+0/+15/+10/+0 stats; add Extrasensory and Disarming Voice by level; retain Psychic Noise, Eclipse, and Wind Chimes; skip Weather Ball. |
| Cinccino | Use upstream's +5/+5/+0/+0/+0/+0 stats; retain Technician, Skill Link, Spring Clean, Alluring Voice, and Plain Terrain. |
| Cinderace | Add Receiver in slot 2 for regular and Gigantamax Cinderace; retain Blaze, Libero, Playmaker, and current stats, skipping upstream's +0/+0/+0/+5/+0/+0 stats and generic legacy tutors. |
| Clamperl | Keep Divergence as-is; skip Battle Armor because Crystal Armor already prevents critical hits; retain Shell Armor, Rattled, Crystal Armor, and current stats. |
| Claydol | Add Heal Block and Recover by level plus Fly, Power Gem, and Tri Attack as tutors; retain Mystical Power, Eclipse, Ancient Idol, and current stats, skipping upstream's +0/+0/+0/+10/+0/+0 stats. |
| Clefable | Add Spotlight at level 1; retain Meteor Mash, Life Dew, Moonblast's current placement, Alluring Voice, Air Slash, Moonlight, and current stats; skip Power Gem and the other duplicated level-1 moves. |
| Clefairy | Add Spotlight at level 1 and Draining Kiss by level; retain Splash, Sweet Kiss, Charm, Copycat, Life Dew, Alluring Voice, Moonlight, and current stats; skip Double Slap, Bestow, Wake-Up Slap, Minimize, Lucky Chant, and Body Slam. |
| Cleffa | Add Magical Leaf and Draining Kiss by level; retain Splash, Alluring Voice, Moonlight, and current stats. |
| Clobbopus | Change to Fighting/Water; use Limber / Sticky Hold / Technician; add Bubble, Aqua Jet, and Mach Punch by level plus Flip Turn as a tutor; retain Octolock and current stats; skip the generic legacy tutors. |
| Coalossal | Add Lava Plume, Power Gem, and Burn Up by level; retain Stone Edge, Skyfire, Acid Rain, and current stats; skip the generic legacy tutors. |
| Cobalion | Add Rock Polish and Swirling Blade as tutors; retain Upper Hand, Iron Resolve, and current stats; skip Rock Climb and Low Sweep. |
| Cofagrigus | Use upstream's +10/-10/+0/+10/+0/+0 stats; add Ominous Wind by level; retain Shadow Claw, Crafty Shield, Dark Pulse, Petrify, Infernal Parade, Eclipse, and Stage Fright; skip Sturdy and Shadow Sneak. |
| Combee | Use Honey Gather / Hustle / Sweet Veil; retain Hive Command, the custom terrain tutors, and current stats. |
| Combusken | Add Early Bird in slot 2; retain Blaze, Speed Boost, Flash Firestorm, Thunderous Kick, and current stats. |
| Corsola | Use upstream's +0/-10/+20/+25/+20/+0 stats; retain Hustle, Natural Cure, Regenerator, Salt Cure, and Reef Protection; skip Toxic. |
| Corsola (Galarian) | Use +0/-10/+0/+25/+0/+0, skipping upstream's 20-point Defense and Special Defense increases; add Unburden in slot 2; retain Weak Armor, Cursed Body, Wailing Reef, Acid Rain, and Eclipse; skip the generic legacy tutors. |
| Cottonee | Add Sleep Powder by level; retain Prankster, Tufted Away, and current stats; skip Tailwind. |
| Crabominable | Add Power-Up Punch, Dizzy Punch, and Mach Punch by level plus Crabhammer as a tutor; retain Protect, Brick Break, Slam, Cold Snap, Upper Hand, Frostbite Ritual, and current stats; skip Bubble and Pursuit. |
| Crabrawler | Add Power-Up Punch, Dizzy Punch, and Mach Punch by level; retain Protect, Brick Break, Slam, Crabhammer, Upper Hand, Frostbite Ritual, and current stats; skip Bubble and Pursuit. |
| Cradily | Add Power Whip, Pain Split, and Sappy Seed by level plus Power Gem as a tutor; retain Wrap, Leech Seed, Mega Drain, Salt Cure, Acid Rain, Petrifying Roots, and current stats; skip Constrict, Wring Out, and Toxic. |
| Cramorant | Use +0/+0/+0/+15/+0/+20 for normal, Gulping, and Gorging forms, skipping upstream's 10 Defense increase; add Belch and Surf by level for every form; retain Mouthful, Gulp Missile, and the existing learnset; skip Superpower, Scald, and the generic legacy tutors. The shared code definitions should propagate these automatically. |
| Cranidos | Keep Divergence as-is; skip Rock Head and Rock Climb because recoil-free Head Smash would be too reliable alongside Accelerock and Thick Skull; retain Mold Breaker, Sheer Force, and current stats. |
| Crobat | Use upstream's +0/+0/+0/+20/+0/+0 stats; add Sniper in slot 2; retain Inner Focus, Infiltrator, Vampiric, Eclipse, and Jetstream. |
| Croconaw | Add Strong Jaw in slot 2 and Psychic Fangs by level; retain Jaw Lock, Death Roll, and current stats; skip Rage, Night Slash, and Chip Away. |
| Crustle | Use upstream's +0/+0/+0/+0/+0/+10 stats; add Crabhammer by level; retain Fury Cutter, Shell Work, Acid Rain, and Infested Terrain; skip Feint Attack. |
| Cryogonal | Use upstream's +0/+0/+0/+15/+0/+0 stats; retain Freezy Frost at Divergence's existing level 60 placement, Flash Freeze, and the existing learnset; skip Sharpen. |
| Cubchoo | Add Ice Fang by level; retain Frost Breath, Snowscape, Ice Hammer, Mountain Gale, Cold Snap, Ice Floe, and current stats; skip Bide, Rock Smash, and Hail. |
| Cubone | Add Head Smash as a tutor; retain Bone Keeper, the bone moves, and current stats; skip Rock Climb. |
| Cursola | Use +0/-20/+0/+0/+0/+25, skipping upstream's 20 HP and 5 Defense increases; add Unburden in slot 2; retain Wailing Reef, Acid Rain, and Eclipse; skip Mystical Fire and the generic legacy tutors. |
| Cyndaquil | Use Blaze / Flash Fire / Adaptability; add Mystical Fire as a tutor; retain Blinding Smoke and current stats. Reassess the package on its evolutions individually. |
| Decidueye | Use upstream's +0/+0/+0/+0/+0/+5 stats; add Keen Eye in slot 2 throughout Rowlet / Dartrix / Decidueye; retain Overgrow, hidden Long Reach, Upper Hand, Cold Snap, Eclipse, and Night Hunter. |
| Decidueye (Hisuian) | Use upstream's +0/+0/+5/+0/+0/+0 stats; add Keen Eye in slot 2, matching Rowlet and Dartrix; retain Overgrow, hidden Scrappy, Upper Hand, and Wandering Hunter. |
| Deerling (all forms) | Add Camouflage, Nature Power, Jump Kick, and High Horsepower by level; retain Bullet Seed, Zen Headbutt, Plain Terrain, Seasons Greeting, and current stats; skip Feint Attack, Aromatherapy, Lunge, and Captivate. |
| Delphox | Use Blaze / Magician / Magic Guard; retain Mega Delphox's existing Ability, Spellbook, Torch Song, Psychic Noise, and current stats, skipping upstream's +0/+0/+0/+1/+0/+0 stats and Toxic. |
| Deoxys | Keep Divergence as-is; skip Moody; retain Pressure, Psychic Noise, Eclipse, Unknown Biology, and current stats. |
| Deoxys (Attack Forme) | Keep Divergence as-is; skip Sheer Force; retain Pressure, Psychic Noise, Unknown Biology, and current stats. |
| Deoxys (Defense Forme) | Keep Divergence as-is; skip Regenerator; retain Pressure, Psychic Noise, Unknown Biology, and current stats. |
| Deoxys (Speed Forme) | Keep Divergence as-is so no single forme receives a regular Ability change; skip Inner Focus; retain Pressure, Psychic Noise, Unknown Biology, and current stats. |
| Dewott | Use upstream's +0/+8/+0/-8/+0/+0 stats; change to Water/Fighting; use Torrent / Shell Armor / Super Luck; add Low Sweep and Revenge by level; retain Soak, Aerial Ace, Tidal Switch, and the existing learnset; skip Water Sport. |
| Dhelmise | Add Spirit Shackle as a late-level move; retain Anchor Shot, Steelworker, Wreckage, Acid Rain, Eclipse, and current stats. |
| Diggersby | Use upstream's +16/+5/+3/+0/+3/+0 stats; retain Pickup, Cheek Pouch, Huge Power, Plain Terrain, and Work Crew. |
| Dodrio | Add Jump Kick and High Jump Kick by level plus U-turn as a tutor; retain Ground/Flying typing, Jetstream, Plain Terrain, Split Instinct, and current stats; skip Rage and Pursuit. |
| Donphan | Make Magnitude a level-up move rather than compatibility-only; retain Stomping Tantrum, Spin Out, Rumble Roll, and current stats; skip the other legacy level additions. |
| Doublade | Add Sharpness as its hidden Ability and Swirling Blade once by level; retain every existing move upstream omits, Bitter Blade, Eclipse, Authority, and current stats; do not duplicate Swirling Blade as a tutor or at two levels. |
| Dragapult | Add Shadow Claw by level; retain U-turn, Eject, the existing learnset, and current stats; skip Sucker Punch, Shadow Sneak, and the generic tutors. |
| Dragonair | Add Fly as a tutor; retain Dragon Majesty and current stats. |
| Drakloak | Add Shadow Claw by level; retain U-turn, Eject, the existing learnset, and current stats; skip Shadow Sneak and the generic tutors. |
| Drampa | Use upstream's +25/-15/+0/+0/+0/+0 stats; apply the same delta to Mega Drampa, producing 103/70/110/160/116/36 (BST 595), so Mega Evolution preserves HP progression; retain Berserk, Sap Sipper, Cloud Nine, Dragon Energy, Earth Power, and Elementalist. |
| Drednaw | Add Aqua Jet and Shell Smash by level; retain Head Smash, Jaw Lock, Ripjaw, the existing learnset, and current stats; skip Scale Shot and the remaining tutors. |
| Drifblim | Replace Aftermath with Wind Rider; add Ominous Wind, Air Slash, and Hurricane by level plus Poltergeist as a tutor; retain Strength Sap, Self-Destruct, Destiny Bond, Tailwind, Floaty Fall, Eclipse, Stage Fright, Balloon Burst, and current stats; skip Constrict, Amnesia, Extrasensory, and Flamethrower. |
| Drifloon | Replace Aftermath with Wind Rider; add Ominous Wind, Air Slash, and Hurricane by level; retain Self-Destruct, Destiny Bond, Tailwind, Floaty Fall, Eclipse, Stage Fright, Balloon Burst, and current stats; skip Constrict, Amnesia, and Extrasensory. |
| Drizzile | Add Shed Skin in slot 2 and Air Cutter as a tutor, preserving Sobble's access; retain Torrent, Sniper, Take Aim, the existing learnset, and current stats; skip the generic tutors. |
| Druddigon | Use upstream's +13/+0/+0/+0/+0/-7 stats; add Dragon Hammer by level plus Spikes and Roost as tutors; retain Metal Claw, Iron Head, Dragon Breath, Rough Skin, Sheer Force, Mold Breaker, and Treasure Hoard; skip the remaining additions. |
| Ducklett | Replace Keen Eye with Friend Guard; add Hydro Pump by level; retain Big Pecks, Hydration, Migration, the existing learnset, and current stats; skip Water Sport. |
| Dugtrio | Add Rototiller by level and make Magnitude a level-up move rather than compatibility-only; retain Sandstorm, Thousand Waves, Sinkhole, Swords Dance as a tutor, the existing learnset, and current stats; skip Mud Bomb and Final Gambit. |
| Dunsparce | Use +0/+20/+0/+0/+0/+0 stats, skipping upstream's additional +25 Defense and -10 Special Attack; retain Burrow, Plain Terrain, Trump Card, and the existing learnset. |
| Durant | Replace Swarm with Heatproof; retain Hustle, Truant, Colony Guardian, Infested Terrain, the existing learnset, and current stats; skip Work Up, Bug Buzz, Rock Climb, and Low Sweep. |
| Dusknoir | Replace Pressure with Iron Fist; retain Frisk, Shadow Grasp, Shadow Bone, Power Shift, Eclipse, Stage Fright, the existing learnset, and current stats. |
| Dwebble | Add Crabhammer by level; retain Fury Cutter, Shell Work, Acid Rain, Infested Terrain, the existing learnset, and current stats; skip Feint Attack. |
| Eelektrik | Add Nuzzle, Breaking Swipe, Psychic Fangs, and Leech Life by level; retain Thunder Wave, Bioluminescence, Static Burst, Thundercrush, the existing learnset, and current stats; skip Draining Kiss. |
| Eelektross | Add Nuzzle and Breaking Swipe by level plus Psychic Fangs and Leech Life as tutors; retain Wild Charge, Bioluminescence, the existing learnset, current stats, and Mega Eelektross's current stats and Eelevate; leave Close Combat, Focus Punch, Outrage, and Ion Deluge as tutors rather than duplicating them by level; skip Draining Kiss. |
| Eevee | Keep Divergence as-is; retain Potential, Veevee Volley, Trump Card, Alluring Voice, Plain Terrain, and current stats; skip Grass Knot. |
| Ekans | Replace level-up Sludge Bomb with Poison Fang; retain Glare, Strike Fear, Acid Rain, Jaw Lock, the existing learnset, and current stats; skip Mud Bomb. |
| Electabuzz | Use Volt Absorb / Static / Vital Spirit; add Dual Chop by level; retain Charge, Giga Impact, Dynamo Fists, the existing learnset, and current stats; leave Cross Chop, Electro Ball, and Wild Charge as tutors; skip Close Combat and Rock Climb. |
| Electivire | Add Iron Fist in slot 2; add Mach Punch, Close Combat, and Dual Chop by level plus Drain Punch as a tutor; retain Electric/Fighting typing, Charge, Thunder, Plasma Fists, Dynamo Fists, the existing learnset, and current stats; skip Rock Climb and unnecessary tutor-to-level duplicates. |
| Electrode | Add Buzzy Buzz by level plus Bug Buzz as a tutor; retain Thunder Shock, Volt Break, Static Burst, Thundercrush, Zippy Zap, the existing learnset, and current stats; skip Energy Ball, Aura Sphere, and Flash Cannon. |
| Elekid | Use Volt Absorb / Static / Vital Spirit; add Dual Chop by level; retain Charge, Plasma Fists, Thundercrush, Dynamo Fists, the existing learnset, and current stats; leave Cross Chop, Electro Ball, and Wild Charge as tutors. |
| Elgyem | Keep Divergence; retain Teleport, Recover, Lumina Crash, Brainstorm, Eclipse, Odd Signal, the existing learnset, and current stats; skip Power Gem, Mystical Fire, the wholesale level additions, and remaining tutors. |
| Emboar | Port upstream's Fire/Ground redesign for regular Emboar while deliberately retaining Fire/Fighting on Mega Emboar. Use the full upstream +5/+2/+15/-30/+15/+0 stat delta, producing 115/125/80/70/80/65 (BST 535), and apply the same delta to Mega Emboar, producing 115/150/90/80/125/75 (BST 635). Retain the approved Blaze / Thick Fat / Mold Breaker on regular Emboar rather than reverting slot 2 to upstream's Reckless; retain Mega's Mold Breaker. Add High Horsepower as the evolution move, Mud Shot and Hammer Arm at level 1, Odor Sleuth at level 9, Cross Chop at level 68, and V-create at level 75; preserve existing moves and Scorching Relay. Mega shares the learnset, including V-create and the Ground moves, but gains Fighting STAB in exchange for Ground STAB. Skip duplicate early-level placements and the unrelated Iron Tail / Rock Climb tutors under the standing selective-port rule. V-create reverses the earlier rejection and should be assessed on both forms during balance testing. |
| Empoleon | Use +0/+0/+0/+5/+0/+0 stats; add Vital Spirit in slot 2; add Brave Bird by level plus Sing and Calm Mind as tutors; retain Water Gun, Wave Crash, Regal Decree, the existing sound moves, and the rest of the learnset; skip Bubble, Toxic, Rock Climb, Scald, and Baby-Doll Eyes. |
| Entei | Replace hidden Inner Focus with Flash Fire; retain Scorched Reign, the existing learnset, and current stats; skip High Horsepower and Earthquake. |
| Escavalier | Replace Swarm with No Guard; add Smart Strike, Shelter, and Twineedle by level plus Gyro Ball as a tutor; retain Knightly, Meteor Assault, Infested Terrain, the existing learnset, and current stats; skip Fury Attack, Slash, Acid Spray, Toxic, and Night Slash. |
| Espeon | Use Synchronize / Forewarn / Magic Bounce, replacing the duplicate slot 2; add Power Gem and Glitzy Glow by level; retain Precognition, Lumina Crash, Psychic Noise, Veevee Volley, Brainstorm, Eclipse, all moves upstream removes, and current stats; skip Extrasensory, Aura Sphere, Quash, Mystical Fire, Zap Cannon, and Growth. |
| Exeggcute | Make Barrage a level-up move rather than compatibility-only; add Soft-Boiled, Natural Gift, and Bestow by level; remove Divergence's Grav Apple; retain Absorb, Mega Drain, Giga Drain, Synthesis, Multitask, Psychic Noise, Brainstorm, and current stats; skip Sleep Powder, Stun Spore, and Poison Powder. |
| Exeggutor | Add Soft-Boiled as a tutor and Natural Gift by level, preserving Exeggcute's access; retain Multitask, Barrage, Egg Bomb, Grav Apple, Psychic Noise, Brainstorm, the existing learnset, and current stats; skip Earth Power and Weather Ball to avoid generic coverage and weather-tool homogenization. |
| Exeggutor (Alolan) | Add Soft-Boiled and Dragon Dance as tutors plus Natural Gift by level, preserving Exeggcute's access; retain Tall Tale, Barrage, Egg Bomb, Psychic Noise, the existing learnset, and current stats; skip upstream's +0/+20/+0/-20/+0/+0 redistribution, Superpower, Explosion, Earth Power, and the generic legacy tutors. |
| Exploud | Use Soundproof / Scrappy / Punk Rock; retain Overdrive, Boomburst, Uproar, Plain Terrain, Distortion, the existing learnset, and current stats; skip upstream's +0/+10/+0/+10/+0/+10 stats, Synchronoise, and Bug Buzz. |
| Farfetch'd | Change Normal/Flying to Fighting/Flying; use Super Luck / Sharpness / Defiant; add Sacred Sword by level plus Swirling Blade as a tutor; retain Cut, Leaf Blade, Duelist, Trump Card, the existing learnset, and current stats; skip upstream's +3/+20/+10/-3/-2/+50 stats and the remaining additions. |
| Farfetch'd (Galarian) | Add Sharpness in slot 2; add Sacred Sword and Swirling Blade as tutors; retain Sword and Board, the existing learnset, and current stats; skip upstream's +18/+20/+20/+5/+0/+10 stats, Drill Run, Roost, and the generic tutors. |
| Fearow | Add Intimidate in slot 2 while retaining Keen Eye and hidden Sniper; retain Territorial, Beak Blast, Plain Terrain, the existing learnset, and current stats; skip upstream's +0/+20/+0/+0/+0/+0 stats, Brave Bird, Dual Wingbeat, and Snarl. |
| Fennekin | Use Blaze / Magician / Magic Guard; retain Spellbook, the existing learnset, and current stats. |
| Feraligatr | Use +0/+5/+0/-10/+0/+10 stats and apply the same redistribution to Mega Feraligatr, producing 85/165/125/79/93/88 (BST 635); keep regular Feraligatr pure Water while retaining Mega Feraligatr's Water/Dragon typing; use Torrent / Strong Jaw / Sheer Force; add Psychic Fangs by level; retain Jaw Lock, Death Roll, and all existing moves; skip Dark typing and the remaining additions. |
| Finneon | Replace Water Veil with Dazzling; add Tail Glow by level; retain Tailwind, Submerge, Sparkling Aria, the existing learnset, and current stats; skip upstream's +1/-20/+4/+11/+0/+4 stats and the remaining move additions. |
| Flaaffy | Use Static / Overcoat / Cotton Down; retain Beacon, Buzzy Buzz, Static Burst, the existing learnset, and current stats; skip upstream's +5/+0/+5/+5/+0/+0 stats. |
| Flabebe | Add Natural Cure in slot 2; add Draining Kiss and Aromatherapy by level; retain Safeguard, Synthesis at its current placement, Flower Ritual, Fleur Cannon, Alluring Voice, the existing learnset, and current stats for every flower form; skip Lucky Chant. |
| Flapple | Use +0/+0/+0/-10/+0/+25 stats for regular and Gigantamax Flapple; add Scale Shot as a tutor; retain Orchard, Wild Growth, and the existing learnset; skip Roost and the generic tutors. |
| Flareon | Use Flash Fire / Flare Boost / Guts, replacing the duplicate slot 2; Smolder activates Flare Boost's special damage increase as an alternative to physical Guts; retain Baton Pass, the existing Fire attacks, Smolder, Veevee Volley, and current stats; skip upstream's +45/+0/+0/-30/-45/+30 stats, Close Combat, Play Rough, Sizzly Slide, and the new tutors. |
| Floatzel | Keep Swift Swim / Water Veil and do not add Technician; add Tail Slap as a tutor; retain Liquidation, Soak, Wave Crash, Undertow, Aqua Step, Surging Strikes, the existing learnset, and current stats; skip Fake Out, Pursuit, Razor Wind, Sonic Boom, and the remaining tutors. |
| Floette | Add Natural Cure in slot 2 for every flower form, including Eternal Flower; add Draining Kiss and Aromatherapy by level; retain Safeguard, Synthesis, Flower Ritual, Fleur Cannon, Eternal Flower's distinct moves, the existing learnset, and current stats; skip Lucky Chant. |
| Florges | Add Natural Cure in slot 2 for every flower form; retain Fairy/Grass typing, Flower Ritual, Psychic Noise, Alluring Voice, the existing support moves, and current stats; skip Toxic. |
| Flygon | Keep Divergence as-is; retain Ground/Dragon typing, Levitate, Desert Shroud, Sandstorm, Jetstream, the existing mixed movepool, and current stats; skip upstream's +10/+10/+5/+50/+5/+0 stats, Bug/Dragon typing, Compound Eyes, Dragon Fly, and the special Bug/Quiver Dance package. |
| Frillish | Keep Divergence as-is; retain Water Absorb / Cursed Body / Damp, Recover, Strength Sap, Maelstrom, the existing learnset, and current stats; skip Water Bubble. |
| Froakie | Add Infiltrator in slot 2; retain Torrent, Protean, Ninja Tools, Eclipse, the existing learnset, and current stats. |
| Frogadier | Add Infiltrator in slot 2; retain Torrent, Protean, Ninja Tools, Eclipse, the existing learnset, and current stats. |
| Froslass | Use +0/-10/+0/+30/+0/+0 stats and apply the same redistribution to Mega Froslass, producing 70/70/70/170/100/120 (BST 600); add Levitate in slot 2 to regular Froslass; retain Whiteout, Freezing Glare, Freezy Frost, Nasty Plot, Eclipse, Nightfall, and the existing learnset; skip Mystical Fire and Icicle Crash. | (this one is a little suspect, 170 in mega form might be a bit too crazy)
| Furfrou | Use +0/+10/+0/+0/+10/+0 stats for every trim; retain every trim's Divergence typing, Fur Coat, Haute Couture, Slam, Mega Kick, Plain Terrain, and the existing learnset. |
| Furret | Use upstream's +10/+9/+1/+0/+0/+15 stats; replace Run Away with Scrappy while retaining Keen Eye and Frisk; add Tail Slap and Extreme Speed by level; retain Scamper and Plain Terrain; skip Fur Coat, Flame Wheel, and the remaining additions. |
| Gabite | Add Dual Chop by level; retain Bite, Bulldoze, Dragon Breath, Sand Onslaught, and current stats; leave Earthquake as tutor-only and skip the rest of upstream's level replacement. |
| Gallade | Keep Divergence as-is for both forms; retain Steadfast because Divergence has buffed it, along with Psychic Parry, Quick Draw, Bitter Blade, Psyblade, and current stats; skip upstream's Ability, stat, and move changes. |
| Galvantula | Keep Divergence as-is; skip upstream's +0/+15/+0/+6/+0/+0 stats and Rock Climb; retain Livewire Nest and the existing web, terrain, and Electric tools. |
| Garbodor | Replace Weak Armor with Toxic Debris for regular and Gigantamax Garbodor; retain current stats, Pain Split's existing level placement, Trash Heap, Baneful Bunker, and Acid Rain; skip upstream's +20/+0/+0/+0/+0/+0 stats, Cross Poison, Double Slap, and six tutors. |
| Garchomp | Add Dual Chop by level and Fly as a tutor for all forms; retain Bite, Bulldoze, Dragon Breath, Sand Onslaught, and current stats; skip Extreme Speed, the Fire Fang duplication, and the remainder of upstream's level replacement. |
| Gardevoir | Keep Divergence as-is for both forms; skip base Gardevoir's +0/-5/+0/+0/+0/+20 stats, Mega Gardevoir's +0/-5/+14/+0/+0/+15 stats, Recover, Magical Leaf, Stored Power, and Ice Beam; retain Mystical Fire, Life Dew, all moves upstream removes, Moonveil, and Glitzy Glow. |
| Gastly line | Use Cursed Body / empty slot / Levitate on Gastly, Haunter, regular Gengar, and Gigantamax Gengar; retain Frightmare, current stats, and the existing learnsets. Haunter receives this arrangement for line consistency despite lacking its own upstream Revised profile; Mega Gengar retains Shadow Tag. |
| Gengar | Add Nightmare by level; skip Aura Sphere; retain Perish Song, Reflect Type, Eerie Spell, Frightmare, Eclipse, Stage Fright, and current stats; keep Mega Gengar's Shadow Tag unchanged. |
| Geodude | Add Rapid Spin and Accelerock by level and make Magnitude level-up rather than compatibility-only; retain Aftershock, the existing moves, and current stats; skip Head Smash, Body Press, Mud Sport, and Rock Climb. |
| Geodude (Alolan) | Add Rapid Spin and Accelerock by level; leave Wild Charge as tutor-only; retain Railgun Charge, the existing moves, and current stats; skip Body Press and the legacy tutors. |
| Gible | Keep Divergence as-is; retain Bite, Bulldoze, Dragon Breath, Sand Onslaught, and current stats; leave Earthquake as tutor-only and skip Dragon Rage and upstream's wholesale level replacement. |
| Gigalith | Keep Divergence as-is; retain Sturdy / Stamina / Sand Force, Diamond Storm, Geode Heart, the physical Rock moves, and current stats; skip upstream's +0/-75/+0/+75/+0/+0 redistribution, Weak Armor, Solar Power, and special coverage package. |
| Girafarig | Keep Divergence as-is; upstream does not revise Farigiraf, and improving the Eviolite-capable pre-evolution is unnecessary alongside Headspace. Skip upstream's +0/+10/+0/+5/+0/+5 stats, Rattled, and level replacement; retain Inner Focus / buffed Early Bird / Sap Sipper, Twin Beam, Psychic Noise, and Plain Terrain. |
| Glaceon | Use upstream's +0/+0/+0/+0/-30/+30 stats; use Snow Cloak / Serene Grace / Ice Body; add Freezy Frost by level; retain Ice Shard, all moves upstream removes, Biting Cold, Veevee Volley, and Alluring Voice; skip Earth Power, Extrasensory, and the generic coverage tutors. |
| Glalie | Change regular and Mega Glalie to Ice/Rock; replace Ice Body with Levitate on regular Glalie; add Rock Slide and Icicle Crash by level; retain Cold Snap, all existing moves, and current stats; skip base Glalie's +0/+30/+30/-20/-20/+0 stats, Mega Glalie's +0/+10/+50/-40/+0/+0 stats, Head Smash, and the Rock tutors. |
| Glameow | Use upstream's +1/+10/+3/+3/+3/+5 stats and add Crunch as a tutor; retain Fatal Grace and Plain Terrain. |
| Gloom | Add Leaf Guard in slot 2; retain Moonblast, Toxic Bloom, Acid Rain, the existing learnset, and current stats; skip Apple Acid, Natural Gift, Lucky Chant, Sludge, and Petal Blizzard. |
| Gogoat | Use upstream's +0/+8/+10/-30/+6/+6 stats and keep pure Grass, skipping the Normal addition; retain Horn Leech, Milk Drink, Wild Growth, and Ruminate; skip Superpower, Return, and Rock Climb. |
| Golbat | Use upstream's +0/+0/+0/+15/+0/+0 stats; add Sniper in slot 2 and Hurricane as a tutor; retain Inner Focus, Infiltrator, Vampiric, Eclipse, and Jetstream. |
| Goldeen | Use upstream's +0/+10/+0/-10/+0/+10 stats; retain Piercing Verdict and the existing learnset. |
| Golduck | Use upstream's +0/-10/+0/+5/+0/+15 stats and retain Divergence's Water/Psychic typing; add Psychic by level; leave Power Gem as tutor-only; retain Headache, Aqua Step, Brainstorm, and Psychic Noise; skip Extrasensory, Me First, Water Sport, and Aura Sphere. |
| Golem | Add Rapid Spin, Accelerock, and Steamroller by level and make Magnitude level-up rather than compatibility-only; retain Aftershock, all existing moves, and current stats; skip Head Smash, Heat Crash, Mud Sport, and Rock Climb. |
| Golem (Alolan) | Add Rapid Spin, Accelerock, and Steamroller by level; leave Wild Charge as tutor-only; retain Railgun Charge, all existing moves, and current stats; skip Heat Crash and the legacy tutors. |
| Golett | Add Focus Punch by level and make Magnitude level-up rather than compatibility-only; retain Heavy Slam, Phantom Force, Shadow Ball, Rage Fist, Rocket Fist, Eclipse, and current stats; skip Rollout, Shadow Sneak, and Stone Edge. |
| Golurk | Add Focus Punch by level, make Magnitude level-up rather than compatibility-only, and add Power-Up Punch as a tutor for regular and Mega Golurk; retain current stats, Shadow Ball, Rocket Fist, Rage Fist, Headlong Rush, and Eclipse; skip upstream's +26/+1/+5/+0/+5/+0 stats, Heat Crash, Rollout, Mud-Slap, and Shadow Sneak. Applying the upstream delta to Mega Golurk would produce 620 BST. |
| Goodra | Use pure Dragon, reverting Divergence's Poison addition; keep current stats; add Acid Armor, Recover, and Sludge Wave by level; retain Water Gun, all existing moves, and Sludge Shift; skip upstream's Dragon/Water typing, Bubble, Bide, Hydro Pump, Outrage, and Waterfall. Hisuian Goodra remains Dragon/Steel. |
| Goodra (Hisuian) | Add Metal Burst and Steel Roller by level; retain Iron Head, all moves upstream removes, Shelter, and current stats; leave Flash Cannon as tutor-only; skip Recover, Iron Defense, Mirror Shot, Bubble, and Bide. |
| Goomy | Retain pure Dragon typing and current stats; add Tearful Look by level; retain Water Gun, all existing moves, and Sludge Shift; skip Recover so it cannot carry into the Hisuian branch, plus upstream's Dragon/Water typing, Bubble, Bide, Hydro Pump, and Water tutors. Recover remains approved on regular Sliggoo and Goodra. |
| Gorebyss | Add Dazzling in slot 2, using Swift Swim / Dazzling / Hydration; retain Water/Fairy typing, Shell Game, Take Heart, Alluring Voice, Misty Terrain, the existing moves, and current stats; skip Water/Psychic typing, Regenerator, Dark Pulse, Liquidation, and Psyshock. Review priority protection alongside Shell Game and Shell Smash in playtesting. |
| Gothita | Keep pure Psychic and add Dark Pulse by level; retain Psych Up, Twisted Hour, Psychic Noise, Eclipse, and current stats; skip Dark typing, Double Slap, Embargo, Feint Attack, Heal Block, and Telekinesis. |
| Gothorita | Keep pure Psychic and add Dark Pulse by level; retain Psych Up, Twisted Hour, Psychic Noise, Eclipse, and current stats; skip Dark typing and the remainder of upstream's level replacement. |
| Gothitelle | Keep pure Psychic and add Dark Pulse and Night Daze by level; retain Psych Up, Twisted Hour, Psychic Noise, Eclipse, and current stats; skip Dark typing, Will-O-Wisp, and the remainder of upstream's additions. |
| Gourgeist | Replace Insomnia with Flash Fire for every size; add Shadow Punch by level plus Synthesis and Fire Punch as tutors; retain Moonblast, Haunted Harvest, Eclipse, Stage Fright, and current stats. |
| Granbull | Use upstream's +10/+0/+0/+0/+0/+0 stats and keep pure Fairy, skipping the Fighting addition; add Double Kick by level; leave Close Combat and Super Fang as tutor-only; retain Growl, Last Resort, Brutal Charge, and Plain Terrain; skip Dynamic Punch, Rage, Covet, and Toxic. |
| Graveler | Add Rapid Spin and Accelerock by level and make Magnitude level-up rather than compatibility-only; retain Aftershock, all existing moves, and current stats; skip Head Smash and Mud Sport. |
| Graveler (Alolan) | Add Rapid Spin and Accelerock by level; leave Wild Charge as tutor-only; retain Railgun Charge, all existing moves, and current stats; skip the legacy tutors. |
| Greedent | Add Recycle by level; retain Stuff Cheeks, Cheek Pouch, Gluttony, Messy Eater, Plain Terrain, the existing learnset, and current stats; skip Toxic, Return, and Frustration. |
| Greninja | Use upstream's +0/+5/+0/+0/+0/+0 stats and apply the same delta to Mega Greninja, producing 72/130/77/133/81/142 (BST 635); add Infiltrator in slot 2 to regular Greninja; retain Torrent, Protean, Ninja Tools, Upper Hand, and Eclipse. |
| Greninja (Battle Bond) | Use upstream's +0/+5/+0/+0/+0/+0 stats and apply the same delta to Ash-Greninja, producing 72/150/67/153/71/132; retain Battle Bond, Ninja Tools, and the existing learnset. |
| Grimer | Keep Divergence as-is; retain Stench / Sticky Hold / Poison Touch, Toxic Deluge, Acid Rain, Foul Mixture, and current stats; skip Regenerator. |
| Grookey | Add Soundproof in slot 2; retain Overgrow, Grassy Surge, Mischief, the existing learnset, and current stats; skip the generic tutors. |
| Grotle | Add Rock Head in slot 2; retain Shell Armor, Grav Apple, Uproot, the existing learnset, and current stats; skip Bulldoze, Leaf Blade, Shell Smash, and Sleep Powder. |
| Grovyle | Use upstream's +0/+20/+0/-20/+0/+0 stats; add Hyper Cutter in slot 2 plus Fury Cutter and Slash by level; leave X-Scissor as tutor-only; retain Assurance, Leafage, Slam, Double Team, Upper Hand, Jetstream, and Canopy Stalker; skip the remainder of upstream's level reshuffling. |
| Growlithe | Add Hone Claws as a tutor; retain Intimidate, Flash Fire, Justified, Immolate, the existing learnset, and current stats. |
| Guzzlord | Add Dragon Hammer, Steamroller, and Wring Out by level; retain Body Slam, Dragon Rush, Giga Impact, Knock Off, Ultra Devour, Eclipse, and current stats; skip Brutal Swing, Dragon Rage, Iron Tail, and the tutors. |
| Haxorus | Keep Divergence as-is; retain Brutal, the existing learnset, and current stats; skip Psycho Cut, Aqua Tail, Work Up, and Rock Climb. |
| Herdier | Add Odor Sleuth by level; retain Baby-Doll Eyes, Field Runner, Plain Terrain, and current stats; skip High Horsepower. |
| Honedge | Add Sharpness as its hidden Ability and Swirling Blade by level; retain No Guard, every existing move, Bitter Blade, Authority, Eclipse, and current stats. |
| Hoppip | Use upstream's +0/+0/+0/+10/+0/+0 stats and Wind Rider / Infiltrator / Aerilate; add Air Cutter, Air Slash, Hyper Voice, and Rage Powder by level; retain all existing moves, Pollen Puff, and Floaty Fall; skip Worry Seed duplication, Toxic, and Round. |
| Huntail | Add Psychic Fangs by level and Intimidate in slot 2, using Swift Swim / Intimidate / Water Veil; retain Water/Dragon typing, Dragon Tail, Abyssal Maw, the existing moves, and current stats; skip Water/Dark typing and Liquidation. Review Intimidate's safer Shell Smash setup alongside Abyssal Maw in playtesting. |
| Hydreigon | Keep Divergence as-is; retain Apocalypse, Fiery Wrath, Eclipse, Nightfall, the existing learnset, and current stats; skip Dragon Rage and Steel Wing. |
| Igglybuff | Add Draining Kiss by level; retain Cute Charm / Competitive / Friend Guard, Disable, Soft Body, Alluring Voice, and current stats; skip Huge Power. |
| Illumise | Use `+0/-14/+10/+14/+10/+15`, producing 65/33/85/87/95/100 (BST 465); skip upstream's additional 5 HP to match Volbeat's approved BST, preserving their equal vanilla BST of 430. Use Oblivious / Tinted Lens / Swarm, replacing Prankster because Response provides conditional status priority; retain Bug/Fairy typing; add Disarming Voice, Quiver Dance, Draining Kiss, and Moonblast by level; retain Flatter, all existing moves, Shimmer, and Response; skip Covet, Dual Wingbeat, and Lunge. |
| Incineroar | Add Rivalry in slot 2 throughout Litten / Torracat / Incineroar; retain Blaze, hidden Intimidate, Showdown, Eclipse, existing moves, and current stats; skip upstream's `+5/+0/+0/+0/+0/+0`. |
| Infernape | Use Blaze / Flame Body / Iron Fist; retain Spellfist, Torment, Raging Fury, the existing learnset, and current stats; skip upstream's `+0/+0/+0/+0/+0/+1`, Punishment, Pyro Ball, and Rock Climb. |
| Inteleon | Add Shed Skin in slot 2, matching Drizzile and preserving the starter-wide second-Ability pattern; add Frost Breath as a tutor; retain Torrent, Sniper, Take Aim, the existing learnset, and current stats; skip Nasty Plot and the generic tutors. |
| Ivysaur | Add Sweet Veil in slot 2; retain Overgrow, Chlorophyll, Vine Lash, Acid Rain, the existing learnset, and current stats. |
| Jellicent | Keep Divergence as-is; retain Water Absorb / Cursed Body / Damp, Recover, Strength Sap, Bouncy Bubble, Maelstrom, and current stats; skip Water Bubble and Toxic. |
| Jigglypuff | Use upstream's `+0/-10/+0/+10/+0/+0`; use Friend Guard / Competitive / Frisk; add Draining Kiss, Moonblast, and Boomburst by level; retain all existing moves, Soft Body, and Alluring Voice; skip Huge Power, Double Slap, Rollout, and Wake-Up Slap. |
| Jirachi | Keep Divergence as-is; retain Wishmaker, the existing learnset, and current stats; skip Draco Meteor. |
| Jolteon | Use Volt Absorb / Static / Quick Feet, replacing the duplicate slot 2; add Buzzy Buzz by level; retain all existing moves, Ionize, Veevee Volley, and current stats; skip Shock Wave, Extrasensory, Thunderbolt duplication, and Grass Knot. |
| Jumpluff | Use upstream's `+0/+0/+0/+30/+0/+0` and Wind Rider / Infiltrator / Aerilate; add Air Cutter, Air Slash, Hyper Voice, and Rage Powder by level; retain all existing moves, Pollen Puff, and Floaty Fall; skip Hurricane, Worry Seed duplication, Fly, and Round. |
| Jynx | Use upstream's `+10/-10/+0/+10/+0/+0`; add Draining Kiss by level; retain Sweet Kiss, Copycat, Confusion, Sing, Psychic, all existing moves, Sweet Nothings, and Freezing Glare; skip the physical additions, Psycho Boost, and Power Gem. |
| Karrablast | Use No Guard / Shed Skin / Swarm, matching Escavalier's No Guard in slot 1 while retaining the stage-specific remaining Abilities; retain Knightly, Meteor Assault, Infested Terrain, the existing learnset, and current stats. |
| Keldeo | Keep Divergence as-is for both forms; retain Flowing Lesson / Resolute Blade, the existing learnset, and current stats; skip Megahorn, Swirling Blade, Low Sweep, and Rock Climb. |
| Kingdra | Keep Divergence as-is; retain Snipe Shot, Whirlpool, Dragon Breath, Water Pulse, Rain Dance, Wave Crash, Marksman, and current stats; skip Bubble and Brine. |
| Kingler | Add Aqua Jet and Ice Hammer by level to regular and Gigantamax Kingler; retain Water Gun, Razor Shell, Swords Dance, Crabhammer, Aquatic Armor, and current stats; skip Bubble, Mud Sport, Vise Grip, and the tutors. |
| Kirlia | Keep Divergence as-is; retain Psybeam, Life Dew, all existing moves, Moonveil, and current stats; skip upstream's `+0/+0/+0/+20/+20/+0`, Recover, Stored Power, Magical Leaf, and Ice Beam. |
| Klang | Use Plus / Minus / Levitate and add Flame Wheel by level; retain Electric moves, Double Iron Bash, Factory Line, and current stats; skip Mirror Shot and High Horsepower. Apply this core package consistently to Klink and Klinklang when reviewed. |
| Klink | Use Plus / Minus / Levitate and add Flame Wheel by level, matching Klang; retain Factory Line, all existing moves, and current stats; skip Mirror Shot, High Horsepower, and Superpower. |
| Klinklang | Use Plus / Minus / Levitate and add Flame Wheel by level, completing the approved line package; retain Electric Terrain, Factory Line, all existing moves, and current stats; skip Mirror Shot, High Horsepower, Superpower, Tri Attack, and Toxic. |
| Koffing | Keep Divergence as-is; retain Toxisphere, Acid Rain, Toxic, the existing learnset, and current stats; skip Lava Plume, Gunk Shot, and Gyro Ball. Apply the same no-change approach to Weezing when reviewed. |
| Kricketot | Use upstream's `+0/+10/+0/+10/+0/+0` and Swarm / Shed Skin / Run Away; retain Call Allies and the existing learnset. |
| Kyogre | Keep Divergence as-is for regular and Primal Kyogre; retain Calm Mind, Ice Beam, Sheer Cold, Abyss, the existing learnset, and current stats; skip Frost Breath and Work Up. |
| Kyurem | Add Icicle Crash as a tutor; retain Ice Fang and Icy Wind as tutors, Freeze-Dry, Sheer Cold, Absolute Zero, the existing learnset, and current stats; skip Dragon Rage, Frost Breath, Work Up, and Rock Climb. |
| Black Kyurem | Add Icicle Crash as a tutor; retain Ice Fang and Icy Wind as tutors, Freeze-Dry, Sheer Cold, Absolute Zero, the existing learnset, and current stats; skip Dragon Rage, Frost Breath, Work Up, and Rock Climb. |
| White Kyurem | Add Icicle Crash as a tutor for consistency with the other Kyurem forms; retain Ice Fang and Icy Wind as tutors, Freeze-Dry, Sheer Cold, Absolute Zero, the existing learnset, and current stats; skip Dragon Rage, Frost Breath, Work Up, and Rock Climb. |
| Lairon | Replace Heavy Metal with Earth Eater, completing the approved Aron/Aggron line change; retain Sturdy, Rock Head, Headlong Rush, Iron Shards, the existing learnset, and current stats. |
| Lanturn | Use upstream's `+0/+0/+0/+15/+10/+0`; add Signal Beam, Parabolic Charge, and Tail Glow by level; retain Take Down, Ion Deluge as tutor-only, Abyssal Light, the existing Abilities, and all existing moves; skip Bubble and Spotlight. |
| Larvitar | Keep Divergence as-is and skip Earth Eater; upstream gives Earth Eater to Larvitar and Tyranitar but not Pupitar, and completing that package would make Tyranitar unnecessarily strong. Retain Guts, Sand Veil, Dominion, the existing learnset, and current stats. |
| Leafeon | Use Leaf Guard / Super Luck / Chlorophyll; add Night Slash and Psycho Cut by level; retain all existing moves, Sylvan Surge, Veevee Volley, and current stats; skip Grass Whistle, Power Whip, Sappy Seed, and the tutors. |
| Ledyba | Skip upstream's `+0/+30/+0/-20/+0/+0`; use Swarm / Technician / Iron Fist; add Meteor Mash by level plus Mega Punch, Dual Wingbeat, and Lunge as tutors; retain all existing moves, Battle Formation, and current stats, matching the approved Ledian package. |
| Lickilicky | Keep current stats; use Unaware / Oblivious / Cloud Nine, replacing Own Tempo while preserving Oblivious's Taunt immunity for the tank role; add Curse as a tutor; retain Taste Test and existing moves; skip Rock Climb. |
| Lickitung | Keep current stats; use Unaware / Oblivious / Cloud Nine consistently with Lickilicky; retain its existing Curse tutor, Taste Test, and existing moves; skip Rock Climb. |
| Lileep | Add Power Gem as a tutor, matching Cradily; retain Petrifying Roots, Salt Cure, the existing learnset, and current stats. |
| Lilligant | Keep Divergence; retain Pretty Petals, all existing moves, and current stats; skip upstream's `+0/+0/+0/+5/+0/+5`, Fiery Dance, Extrasensory, Recover, and the powder/support tutors. |
| Linoone | Add Tail Slap by level; retain Pin Missile, Hone Claws, Baby-Doll Eyes, Take Down, Flail, Switchstep, Plain Terrain, and current stats; skip upstream's `+7/+15/-1/+0/-1/+10`, Play Rough, Rototiller, Odor Sleuth, Mud Sport, Bestow, and the tutors. |
| Galarian Linoone | Keep Divergence as-is; retain Heckler, the existing learnset, and current stats; skip upstream's `+7/+15/-1/+0/-1/+10` and the generic tutors. |
| Litleo | Use upstream's `+0/+20/+0/+0/+0/+0` and add Flare Blitz by level; retain every existing move and Battlecry; skip Iron Tail and Incinerate duplication. Continue the physical direction when Pyroar is reviewed. |
| Litwick | Add Mystical Fire and Dazzling Gleam as tutors, matching Chandelure; retain Pyromancy, Eerie Spell, Eclipse, the existing learnset, and current stats. Give Lampent the same tutors for line consistency. |
| Lombre | Use upstream's `+0/+0/+0/+0/+0/+10`; retain Carnival, Aqua Step, and the existing learnset. |
| Lopunny | Keep current stats for base and Mega Lopunny; change base Lopunny to Normal/Fighting, matching Mega; add Mega Kick, Jump Kick, Blaze Kick, and Dizzy Punch by level; retain Double Kick, Headbutt, Flatter, Bunny Ears, and all existing moves; skip base `+0/+30/+0/-10/+0/+0`, Mega `+0/+0/+0/+20/+0/+0`, Mach Punch, High Horsepower, and the remaining additions. |
| Lotad | Use upstream's `+0/+0/+0/+0/+0/+5`; retain Carnival, Aqua Step, and the existing learnset. |
| Loudred | Use Soundproof / Scrappy / Rattled; retain Distortion, Overdrive, Uproar, the existing learnset, and current stats. |
| Lucario | Use Steadfast / Inner Focus / Mega Launcher on regular Lucario, replacing Justified while preserving buffed Steadfast; keep Mega Lucario's Adaptability; add Power-Up Punch by level; retain Aura, every existing move, and current stats for both forms; skip Laser Focus, Foresight, and Me First. |
| Ludicolo | Use upstream's `+0/+0/+0/+0/+0/+20`, completing the approved Lotad/Lombre Speed progression; retain Carnival, Aqua Step, and the existing learnset. |
| Lugia | Keep Divergence as-is; retain Psychic/Flying typing, Sea Guardian, the existing learnset, and current stats; skip Water/Flying typing, Psycho Boost, Scald, and the other tutors. |
| Lunatone | Use upstream's `+0/+0/+0/+20/+0/+0`; add Night Daze and Lunar Dance by level and move Power Gem from tutor-only to level-up; retain Moonlight, all existing moves, Moon Totem, Eclipse, and Moongeist Beam; skip Psywave, Embargo, Heal Block, and Toxic. |
| Luxio | Add Baby-Doll Eyes and Nuzzle by level; retain Volt Switch, Thunder Wave, Thunder Fang, Volt Tackle, Play Rough as tutor-only, X-Ray Jaws, all existing moves, and current stats. |
| Luxray | Keep pure Electric typing; add Psychic Fangs, Baby-Doll Eyes, and Nuzzle by level; retain Volt Switch, Thunder Wave, Thunder Fang, Volt Tackle, Play Rough as tutor-only, X-Ray Jaws, all existing moves, and current stats; skip Electric/Dark typing, Sucker Punch, Dark Pulse, and Nasty Plot. |
| Lycanroc (Midday) | Keep pure Rock and replace Keen Eye with Super Luck; add Drill Run by level; retain Sucker Punch, Double Team, Instinct, all existing moves, and current stats; skip Ground typing and the other Ground moves and tutors. |
| Lycanroc (Dusk) | Use Tough Claws / Own Tempo / Stakeout and add Thrash by level; retain Crush Claw, every existing move, Instinct, and current stats; skip the elemental fangs, Rock Climb, and Iron Tail. |
| Lycanroc (Midnight) | Keep Rock/Dark typing; use Reckless / Vital Spirit / No Guard; add Accelerock and Head Smash by level; retain Double Team, Endure, Instinct, all existing moves, and current stats; skip Rock/Fighting typing, Submission, Close Combat, Rock Climb, and Earthquake. |
| Machamp | Add Mach Punch and Drain Punch as tutors to regular and Gigantamax Machamp; retain Champion, the existing learnset, and current stats; skip Rock Climb. |
| Machoke | Add Mach Punch and Drain Punch as tutors, matching Machamp; retain Champion, the existing learnset, and current stats; skip Rock Climb. |
| Machop | Add Mach Punch and Drain Punch as tutors, completing the line package; retain Champion, the existing learnset, and current stats; skip Rock Climb. |
| Magby | Add Scorching Sands as a tutor, matching Magmar and Magmortar's existing access; retain Flame Body / empty slot / Vital Spirit, Flameheart, all existing moves, and current stats; skip Magma Armor and the added level moves. |
| Magmar | Keep Divergence as-is; retain Flame Body / empty slot / Vital Spirit, Flame Wheel, Scary Face, Low Kick, Hyper Beam, Flameheart, Scorching Sands, all existing moves, and current stats; skip Magma Armor, Moonblast, Scald, and the other additions. |
| Magmortar | Replace Vital Spirit with Quick Draw, using Flame Body / empty slot / Quick Draw; retain Flameheart, Armor Cannon, Scorching Sands, Fire Blast, every existing move, and current stats; skip Flash Fire, Moonblast, Scald, Magma Storm, and the other additions. |
| Magnemite | Keep Divergence; skip Bug Buzz; retain Thunder Cage, Sighting System, the existing learnset, and current stats. |
| Magneton | Keep Divergence; skip Bug Buzz; retain Thunder Cage, Sighting System, the existing learnset, and current stats. |
| Magnezone | Keep Divergence; skip Bug Buzz; retain Signal Beam, Thunder Cage, Sighting System, the existing learnset, and current stats. |
| Manaphy | Keep Divergence's current stats and learnset, including Water Gun and Take Heart; retain Tideborn and Alluring Voice; leave Hydro Pump as tutor-only; skip Bubble, Water Sport, Signal Beam, and upstream's level reshuffle. |
| Mandibuzz | Add Quash as a tutor; retain Carrion Hour, Eclipse, the existing learnset, and current stats; skip Superpower and Zen Headbutt. |
| Manectric | Keep Divergence as-is for both forms; retain Signal Beam, Static Burst, Charged Cry, the existing learnset, and current stats; skip Energy Ball. |
| Mankey | Use Defiant / Anger Point / Vital Spirit, matching Annihilape's approved arrangement; retain Tantrum, Eclipse, the existing learnset, and current stats; skip Cut. |
| Mareep | Use Static / Overcoat / Cotton Down, matching Flaaffy and Ampharos; retain Beacon, Buzzy Buzz, Static Burst, the existing learnset, and current stats; skip upstream's +0/+0/+5/+0/+0/+0 stats. |
| Marill | Use upstream's +0/+0/+0/+20/+0/+0 stats, matching Azurill and Azumarill; retain Thick Fat, Huge Power, Sap Sipper, Buoyancy, Alluring Voice, and the existing learnset. |
| Marshtomp | Use Torrent / Damp / Water Veil; retain Floodplain, the existing learnset, and current stats. Apply the same regular-Ability arrangement to Mudkip and regular Swampert when reviewed. |
| Masquerain | Add Levitate in slot 2; retain Divergence's Bug/Water typing, Intimidate, Unnerve, Water Glide, the existing learnset, and current stats; skip upstream's +0/-10/-7/+10/+18/+20 stats, Twister, Dive, and Fly. |
| Mawile | Keep Divergence as-is for both forms; retain Vicejaw and current stats; skip upstream's +0/+10/+5/+0/+5/+0 base-form increase rather than applying it inconsistently to Mega Mawile. |
| Medicham | Add Mind Reader by level and Psycho Cut as a tutor for both forms; leave Close Combat as tutor-only; retain Empty Hand, Axe Kick, every existing move, and current stats; skip upstream's +0/+0/+5/+20/+5/+10 stats for both forms, Acrobatics, and the wholesale replacement. |
| Meditite | Add Meditate and Mind Reader by level; retain Work Up, Psybeam, Upper Hand, every existing move, and current stats; leave Close Combat and Psycho Cut as tutor-only; skip upstream's +0/+0/+0/+20/+0/+0 stats and wholesale replacement. |
| Meganium | Keep regular Meganium pure Grass, reserving Grass/Fairy for Mega; use Overgrow / Filter / Triage; add Draining Kiss and Moonblast by level; retain Aromatherapy, Jungle Healing, Flower Field, Earth Power, Dazzling Gleam, every existing move, and current stats for both forms; skip upstream's +0/+0/+0/+10/+0/+0 rather than pushing Mega Meganium to 153 Special Attack, plus Natural Gift, Play Rough, Rock Climb, and the wholesale replacement. |
| Meloetta | Add Wake-Up Slap as a tutor; retain Aria Forme's current stats, Pirouette Forme's physical stats, Close Combat, Relic Song's current placement, every existing move, Opening Verse, Final Step, and Alluring Voice; skip Pirouette's +0/-51/+0/+51/+0/+0 redistribution, Aura Sphere, and the wholesale replacement. |
| Meowth | Use upstream's +5/+15/+0/-10/+0/+0 stats for regular and Gigantamax Meowth; retain Passive Income, Cheap Trick, Plain Terrain, and the existing learnset; skip Dazzling Gleam and Surf. |
| Meowth (Galarian) | Add Bullet Punch as a tutor; retain Double Iron Bash, Tough Claws, Raid Leader, the existing learnset, and current stats; skip Surf and the generic legacy tutors. |
| Mesprit | Change to Psychic/Fairy and add Moonblast by level; leave Play Rough as tutor-only; retain Psychic, Psybeam, Mystical Power, Healing Wish, Burning Heart, Psychic Noise, all existing moves, and current stats; skip Natural Gift, Lucky Chant, and the wholesale replacement. |
| Metagross | Add Iron Fist in slot 2 to regular Metagross and Fly plus Headbutt as tutors for both forms; retain Mega Metagross's Tough Claws, Psyshield Bash, Rock Polish, Hive Mind, every existing move, and current stats. |
| Metang | Add Iron Fist in slot 2 and Fly plus Headbutt as tutors; retain Psyshield Bash, Psychic Noise, Hive Mind, the existing learnset, and current stats. Beldum retains its separately approved Rock Head package until evolving arms. |
| Mienfoo | Add Meditate by level; retain Drain Punch, Aura Sphere, High Jump Kick, every existing move, Like Water, Thunderous Kick, Upper Hand, Jetstream, and current stats; skip Double Slap, Jump Kick, Rock Climb, and the wholesale replacement. |
| Mienshao | Add Meditate by level and Blaze Kick as a tutor; retain Drain Punch, Aura Sphere, High Jump Kick, every existing move, Like Water, Thunderous Kick, Upper Hand, Jetstream, and current stats; skip Double Slap, Jump Kick, Rock Climb, and the wholesale replacement. |
| Milotic | Keep pure Water and current stats; add Moonblast by level; retain Disarming Voice, Life Dew, Sparkling Aria, Tragic Beauty, and every existing move; skip upstream's Water/Fairy typing, Water Sport, Toxic, Dazzling Gleam, and the wholesale replacement. |
| Miltank | Keep Divergence as-is; retain Milk Drink, Pasturized, Plain Terrain, and current stats; skip upstream's +0/+5/+0/+0/+25/+0 stats. |
| Mime Jr. | Use upstream's +0/-10/+0/+10/+0/+5 stats; retain Soundproof, Filter, Technician, Improv, and the existing learnset. Apply the same delta to Mr. Mime when reviewed. |
| Mimikyu | Keep Divergence as-is for every form; retain Disguise, Unspeakable, Eclipse, the existing level progression, and current stats; skip Covet, Feint Attack, and upstream's reshuffle. |
| Minior | Use upstream's +20/+0/+0/+0/+0/+0 stats for every Meteor and Core form and add Air Slash as a tutor; retain Shields Down, Blast Shield, Flak Shield, and the existing learnset. |
| Minun | Use upstream's +5/+50/+45/-35/-35/+15 stats; add Nuzzle, Fake Out, Thunder Punch, Wild Charge, and Swords Dance by level plus Knock Off and U-turn as tutors; retain all moves upstream removes, Minus, Volt Absorb, Negative Charge, Gear Up, Static Burst, and Alluring Voice; skip Signal Beam and Flash Cannon. |
| Misdreavus | Add Fairy Wind, Draining Kiss, Grudge, and Moonblast by level plus Hypnosis as a tutor; retain Confusion, every existing move, Hexcraft, Eerie Spell, Psychic Noise, Eclipse, Nightfall, and Stage Fright; skip Psywave, the wholesale replacement, and any typing change. |
| Mismagius | Change to Ghost/Fairy; add Fairy Wind and Grudge by level plus Moonblast and Hypnosis as tutors, preserving Misdreavus's access; retain Draining Kiss, every existing move, Hexcraft, Eerie Spell, Psychic Noise, Destiny Bond, Eclipse, Nightfall, Stage Fright, and current stats; skip Lucky Chant and Psywave. |
| Moltres | Add Extrasensory as a tutor; retain Solar Call, the existing learnset, and current stats. |
| Moltres (Galarian) | Keep Divergence as-is; retain Spirit Feast, Eclipse, Nightfall, the existing learnset, and current stats; skip Extrasensory and the generic legacy tutors. |
| Monferno | Use Blaze / Flame Body / Iron Fist, matching Chimchar and Infernape; retain Spellfist, Upper Hand, the existing learnset, and current stats; skip Rock Climb. |
| Mr. Mime | Use upstream's +0/-10/+0/+10/+0/+5 stats, matching Mime Jr.; add Barrier and Glitzy Glow by level; retain every existing move, Soundproof, Filter, Technician, Improv, Power Shift, and Brainstorm; skip Double Slap, Psywave, Quash, Power Gem, Tri Attack, Aura Sphere, and the wholesale replacement. |
| Mr. Mime (Galarian) | Use upstream's +0/-10/+0/+10/+0/+5 stats and add Freezy Frost by level; retain Variety Act and every existing move; skip the generic legacy tutors. |
| Mr. Rime | Use upstream's +0/-20/+0/+0/+0/+30 redistribution and add Freezy Frost by level; retain Slack Off, every existing move, Variety Act, and Brainstorm; skip Magical Leaf, Mystical Fire, Charge Beam, and the generic tutors. |
| Mudkip | Use Torrent / Damp / Water Veil, matching Marshtomp; retain Floodplain, the existing learnset, and current stats. |
| Muk | Keep Stench / Sticky Hold / Poison Touch and current stats; add Venom Drench by level; retain Toxic, Mud Shot, every existing move, Toxic Deluge, Acid Rain, and Foul Mixture; skip Regenerator, Moonblast, Mud Bomb, Fling, Sucker Punch, and the wholesale replacement. |
| Muk (Alolan) | Add Venom Drench and Parting Shot by level; retain Toxic, every existing move, Trash Alchemy, Acid Rain, Eclipse, and current stats; skip Sucker Punch, Fling, and the legacy tutors. |
| Munna | Replace Telepathy with Comatose; retain Forewarn, Synchronize, Dream Mist, the existing learnset, and current stats. |
| Musharna | Replace Telepathy with Comatose, matching Munna; retain Forewarn, Synchronize, Dream Mist, Brainstorm, Eclipse, the existing learnset, and current stats. |
| Natu | Add Lucky Chant, Miracle Eye, and Ominous Wind by level; retain every existing move, Omnisense, Esper Wing, Lumina Crash, Eclipse, and current stats; skip Quick Attack, Confusion, Synchronoise, Me First, and the wholesale replacement. |
| Nickit | Add Mystical Fire by level, matching Thievul; retain Bag of Tricks, Cheap Trick, Eclipse, the existing learnset, and current stats; skip the generic legacy tutors. |
| Nidoking | Add Thrash by level; retain every existing move, Regal Pressure, Acid Rain, and current stats; leave Drill Run and Megahorn as tutors rather than duplicating them by level; skip Chip Away and the wholesale replacement. |
| Nidoqueen | Make Body Slam a level-up move; retain every existing move, Royal Charm, Acid Rain, and current stats; leave Drill Run and Superpower as tutors; skip Chip Away and the wholesale replacement. |
| Ninetales | Change to Fire/Fairy; add Dazzling Gleam as its evolution move and Moonblast as a tutor; retain Sun Revelry, every existing move, and current stats matching Alolan Ninetales; skip upstream's +0/-9/+0/+0/+0/+9 redistribution, Psychic, and the wholesale replacement. |
| Ninjask | Add First Impression by level; retain every existing move, Skitterstep, Acid Rain, Infested Terrain, Jetstream, and current stats; skip Drill Run, Extreme Speed, and the wholesale replacement. |
| Noctowl | Use upstream's +10/-10/-10/+20/+20/-10 stats and retain Divergence's Psychic/Flying typing, Gale Wings, Nocturnal, Hypnosis, Esper Wing, Moonblast, Eclipse, and existing learnset; skip Flash and Mystical Fire. |
| Nosepass | Use upstream's +0/-10/+0/+10/+0/+0 redistribution; retain Gravity Well, Tachyon Cutter, Magnitude, Power Shift, and every existing move; leave Head Smash as tutor-only rather than duplicating it by level. |
| Numel | Add Slack Off by level, matching Camerupt; retain Bulldoze, Incinerate, every existing move, Volcanic Rage, Searing Shot, and current stats; skip Flame Burst, Rock Climb, and the wholesale replacement. |
| Nuzleaf | Use upstream's +0/+10/+0/+0/+0/+0 stats; retain Forest Ambush, Cheap Trick, False Surrender, Eclipse, and the existing learnset. |
| Obstagoon | Use upstream's +5/+10/+0/-5/+0/+5 redistribution; retain Heckler and every existing move; skip the generic legacy tutors. |
| Octillery | Use upstream's +0/+10/+0/+10/+0/+0 stats; add Signal Beam, Icicle Spear, and Zap Cannon by level; retain every existing move, Octolock, Snipe Shot, and Adaptive Armory; skip Constrict, Wring Out, Aura Sphere, and the wholesale replacement. |
| Oddish | Add Leaf Guard in slot 2, matching Gloom; retain every existing move, Toxic Bloom, Acid Rain, and current stats; skip Lucky Chant, Sludge, Natural Gift, Apple Acid, and the wholesale replacement. |
| Omanyte | Add Power Gem by level; retain every existing move, Shell Side Arm, Primordial Wake, Acid Rain, and current stats; skip Constrict, Tickle, and the wholesale replacement. |
| Omastar | Add Power Gem and Shelter by level; retain every existing move, Shell Side Arm, Primordial Wake, Acid Rain, and current stats; skip Constrict, Tickle, Rock Climb, and the wholesale replacement. |
| Onix | Use upstream's +0/+30/+0/+0/+0/+10 stats and add Glare and Coil by level; retain every existing move, Subterranean, and Power Shift; skip Rage, Mud Sport, Gyro Ball, Rock Climb, and the wholesale replacement. |
| Oshawott | Use upstream's +0/+8/+0/-8/+0/+0 redistribution and Torrent / Shell Armor / Super Luck, matching Dewott; retain Tidal Switch and every existing move; skip Sucker Punch, Drill Run, Covet, and Scald. |
| Pachirisu | Use upstream's +0/+0/+10/+35/+0/+0 stats, replace Run Away with Prankster, and add Reflect as a tutor; retain Pickup, Volt Absorb, Winter Stash, and every existing move. |
| Palafin (Hero Form) | Keep Divergence as-is; retain vanilla Hero Form stats, Zero to Hero, Hero Landing, and every existing move; skip upstream's +0/-15/-10/-20/-5/+0 nerf. |
| Pancham | Add Sucker Punch as a tutor; retain Second Wind, Darkest Lariat, Eclipse, every existing move, and current stats. |
| Pangoro | Add Sucker Punch as a tutor, preserving Pancham's approved access; otherwise retain existing Abilities, moves, typing, and stats. |
| Paras | Add Lunge and Crabhammer by level; retain every existing move, Fungal Infection, Acid Rain, Infested Terrain, Wild Growth, and current stats. |
| Pawniard | Keep Divergence: retain every existing move, Power Play, Beat Up, Eclipse, and current stats; skip Psycho Cut, Feint Attack, Embargo, and the wholesale replacement. This keeps the full line unchanged without letting the new coverage carry into Kingambit after evolution. |
| Pelipper | Keep Divergence as-is; retain Keen Eye / Wind Rider / Rain Dish, Monsoon, and current stats; skip upstream's +0/+0/+10/+0/+10/+0 stats. |
| Perrserker | Use upstream's +0/+0/+5/+0/+20/+0 stats and add Bullet Punch as a tutor; retain Double Iron Bash, Raid Leader, existing Abilities, and every existing move; skip Surf and the generic legacy tutors. |
| Persian | Use upstream's +0/+20/+0/+15/+0/+0 stats and add Quash as a tutor; retain Passive Income, Cheap Trick, Plain Terrain, and every existing move; skip Dazzling Gleam and Surf. |
| Persian (Alolan) | Use upstream's +0/+0/+0/+20/+0/+5 stats; retain Royal Treatment, Cheap Trick, Eclipse, and every existing move; skip Dazzling Gleam, Surf, and the legacy tutors. |
| Petilil | Keep Divergence as-is; retain Pretty Petals, every existing move, and current stats; skip the powder package, Extrasensory, Aromatherapy, Recover, and the wholesale replacement. |
| Phanpy | Make Magnitude a level-up move rather than compatibility-only, matching Donphan; retain every existing move, Spin Out, Rumble Roll, and current stats; skip Odor Sleuth, Natural Gift, and the wholesale replacement. |
| Phione | Keep Divergence as-is; retain Take Heart, Drift Song, Alluring Voice, every existing move, and current stats; skip Bubble and Water Sport and leave Hydro Pump as tutor-only. |
| Pidgey | Use upstream's +0/-10/-5/+15/+0/+0 redistribution and add Echoed Voice, Air Cutter, and Swift by level; retain every existing move and Strong Winds; skip Ominous Wind, Uproar, Hyper Voice, and the wholesale replacement. |
| Pidgeotto | Use upstream's +0/-10/-5/+15/+0/+0 redistribution, matching Pidgey, and add Echoed Voice, Air Cutter, and Swift by level; retain every existing move and Strong Winds; skip Ominous Wind, Uproar, Hyper Voice, and the wholesale replacement. |
| Pidgeot | Use a moderated +0/-20/-5/+25/+0/+0 redistribution for both Pidgeot and Mega Pidgeot, producing 95 and 160 Special Attack without raising either form's BST; add Echoed Voice, Air Cutter, Swift, and Hyper Voice by level; retain every existing move, Strong Winds, and Mega Pidgeot's No Guard; skip upstream's larger and inconsistent form increases, Ominous Wind, Uproar, Dual Wingbeat, and the wholesale replacement. |
| Pidove | Use upstream's +0/-19/+0/+19/+0/+0 redistribution, replace Rivalry with Unaware, and add Swift by level plus Hurricane as a tutor; retain every existing move, Soaring Gale, and Plain Terrain; skip Razor Wind, Facade, and the wholesale replacement. |
| Pignite | Change to Fire/Ground, beginning the approved Emboar Ground redesign; keep current stats and Blaze / Thick Fat / Sheer Force. Add Mud Shot as the evolution move, retain Arm Thrust at level 1, add Odor Sleuth at level 9, and add Earthquake as a tutor. Preserve Endure, every other existing move, and Scorching Relay; skip duplicate early-level placements from upstream's replacement. Tepig remains pure Fire; Mega Emboar deliberately returns to Fire/Fighting. |
| Pikachu | Add Splishy Splash, Floaty Fall, and Zippy Zap by level; retain every existing move, Pika Papow, Static Charge, Alluring Voice, Upper Hand, and current stats; skip the wholesale replacement. Apply the move additions to regular and Gigantamax Pikachu consistently. |
| Pinsir | Add Megahorn by level for both forms; retain every existing move, Burrowing Horns, Mega Pinsir's Aerilate, and current stats; leave Revenge, Brick Break, and Thrash as tutors; skip Rock Climb and the wholesale replacement. |
| Piplup | Add Vital Spirit in slot 2 plus Sing and Calm Mind as tutors, matching Empoleon; retain Regal Decree, every existing move, and current stats; skip Baby-Doll Eyes, Dual Wingbeat, and Scald. |
| Plusle | Use upstream's +0/+0/+10/+10/+10/+15 stats, matching the approved 450 BST Minun redesign; retain Plus, Lightning Rod, Positive Charge, Gear Up, Static Burst, and Alluring Voice; skip Psychic, Signal Beam, and Flash Cannon. |
| Poliwag | Keep Divergence as-is; retain Hypnosis, Spiral Gaze, every existing move, and current stats; skip Lovely Kiss. |
| Poliwrath | Use upstream's +0/+10/+0/-10/+0/+0 redistribution; add Mind Reader and Storm Throw by level; retain Dynamic Punch, Jet Punch, every existing move, Spiral Gaze, and Upper Hand; leave Close Combat as a tutor; skip Lovely Kiss, Curse, Rock Climb, and the wholesale replacement. |
| Ponyta | Add Jump Kick by level and add a new 70 BP physical Fire pivoting move shared with regular Rapidash; retain every existing move, Hot Pursuit, and current stats; leave Bounce as a tutor. The new move's name and remaining details will be designed during implementation. |
| Ponyta (Galarian) | Add Jump Kick by level; retain Sparkly Swirl, Fairy Tale, every existing move, and current stats; skip the generic legacy tutors. Do not give it the regular Ponyta line's planned Fire pivot move. |
| Poochyena | Use a restrained +0/+0/+0/+0/+0/+15 stat increase, echoing half of Divergence Mightyena's +30 Speed rather than upstream's +0/+20/+0/+0/+0/+30; retain Full Moon, Eclipse, Jaw Lock, and every existing move. |
| Primarina | Keep current stats; add Healer in slot 2 across the entire Popplio line; retain Torrent, Liquid Voice, Solo Serenade, and every existing move; skip upstream's +0/+0/+5/+0/+0/+0 Defense increase. |
| Primeape | Use Defiant / Anger Point / Vital Spirit, matching Mankey and Annihilape; retain Tantrum, Eclipse, every existing move, and current stats; skip Gorilla Tactics, Rock Climb, and Cut. |
| Prinplup | Add Vital Spirit in slot 2 and Calm Mind plus Sing as tutors, matching Piplup and Empoleon; retain Regal Decree, every existing move, and current stats. |
| Probopass | Use upstream's +10/-5/-10/+15/-10/+0 redistribution but retain Rock/Steel typing; add Tri Attack as its evolution move; retain every existing move, Gravity Well, Tachyon Cutter, and Power Shift; skip Rock/Electric typing, Head Smash, and the wholesale replacement. |
| Pumpkaboo | Replace Insomnia with Flash Fire for every size and add Synthesis and Flame Charge as tutors; retain Haunted Harvest, Eclipse, Stage Fright, every existing move, and current stats. |
| Purugly | Use upstream's +19/+13/+6/+1/+1/+3 stats; add Slack Off by level and Crunch as a tutor, preserving Glameow's access; retain Fatal Grace, Plain Terrain, and every existing move; skip Toxic, Play Rough, and the wholesale replacement. |
| Pyroar | Use a moderated +0/+20/+0/+0/+0/+0 increase for both Pyroar and Mega Pyroar, matching Litleo, and add Flare Blitz by level; retain every existing move, Battlecry, and Mega Pyroar's Fire Mane; skip upstream's remaining 15 Attack, Iron Tail, and Return. |
| Pyukumuku | Use a moderated, unchanged-BST +30/-30/+0/+0/+0/+0 redistribution, producing 85 HP and 30 Attack; retain Innards Out, Unaware, Washed Ashore, and every existing move; skip upstream's additional 20 HP. |
| Quagsire | Use upstream's +0/+10/+10/+0/+0/+0 stats; retain Clueless, Unaware, Recover, and every existing move. |
| Quilava | Use Blaze / Flash Fire / Adaptability and add Mystical Fire as a tutor, matching Cyndaquil and Typhlosion; retain Blinding Smoke, every existing move, and current stats. |
| Quilladin | Add Iron Barbs in slot 2; add Pin Missile as its evolution move and Needle Arm and Bulk Up by level; retain Bramble Guard, every existing move, and current stats; skip the remainder of upstream's wholesale replacement. |
| Qwilfish | Use +0/+0/+10/-10/+0/+0, skipping upstream's 10 Speed increase to retain BST 440, matching unchanged Hisuian Qwilfish; add Fell Stinger once as a late-level move; retain Toxic Monsoon, Acid Rain, and every existing move; skip Bubble, Minimize, Revenge, and the wholesale replacement. |
| Qwilfish (Hisuian) | Keep vanilla stats and BST 440, matching regular Qwilfish's approved unchanged-BST redistribution; retain Barbed Monsoon, Acid Rain, Eclipse, and every existing move. Keep Overqwil unchanged. |
| Raboot | Add Receiver in slot 2, matching Cinderace; retain Blaze, Libero, Playmaker, every existing move, and current stats. |
| Raichu | Use upstream's +0/+5/+0/+5/+0/+0 stats for Raichu and apply the same delta to Mega Raichu X and Mega Raichu Y; add Splishy Splash, Floaty Fall, and Zippy Zap as tutors; retain Pika Papow, Static Charge, and every existing move; skip Psychic and Fly. Balance-watch the three premium partner moves on both Mega forms during testing. |
| Raichu (Alolan) | Keep current stats, skipping upstream's +0/+0/+0/+5/+5/+0; add Splishy Splash, Floaty Fall, and Zippy Zap as tutors; retain Pika Papow, Brain Surf, and every existing move. Balance-watch the premium partner moves alongside regular Raichu and both Mega forms. |
| Raikou | Replace hidden Inner Focus with Volt Absorb; retain Lightning Field, every existing move, and current stats; skip Toxic. |
| Ralts | Keep Divergence as-is; retain Moonveil, Alluring Voice, every existing move, and current stats; skip upstream's +0/+0/+0/+20/+20/+0 stats, Recover, Ice Beam, and wholesale learnset. |
| Rampardos | Keep Divergence as-is; retain Accelerock, Head Smash, Thick Skull, every existing move, and current stats; skip Rock Head, Rock Climb, and upstream's wholesale learnset. |
| Rapidash | Keep pure Fire typing and current stats; add Jump Kick by level and the planned 70 BP physical Fire pivoting move shared with regular Ponyta; retain Hot Pursuit and every existing move; skip Fairy typing, Play Rough, and upstream's wholesale learnset. |
| Rapidash (Galarian) | Add Jump Kick by level, matching Galarian Ponyta; retain Sparkly Swirl, Fairy Tale, every existing move, and current stats; skip the duplicate Psycho Cut and generic tutors. Do not give it the regular Rapidash line's planned Fire pivot move. |
| Raticate | Use upstream's +0/+16/+10/-5/+0/+16 stats, reaching 450 BST; retain Gnaw Down, Plain Terrain, and every existing move. |
| Raticate (Alolan) | Use upstream's +0/+9/+0/-5/+20/+13 stats and apply the same delta to its Totem form; retain Nest Boss and every existing move; skip the legacy tutors. |
| Regice | Add Filter in slot 2 and add Freeze-Dry and Freezy Frost as late-level moves; retain Frost Seal, Blizzard, every existing move, and current stats; skip Rock Climb and upstream's wholesale learnset. |
| Regigigas | Keep Divergence as-is; retain Slow Start, Colossal, every existing move, and current stats; skip Unaware because it would bypass the restriction Colossal modifies. |
| Regirock | Add Solid Rock in slot 2 and Rock Wrecker as a late-level move; retain Stone Seal, Rock Slide, every existing move, and current stats; skip Earthquake, Rock Climb, and upstream's wholesale learnset. |
| Registeel | Add Heatproof in slot 2 and Double Iron Bash as a late-level move; retain Iron Seal, Heavy Slam, every existing move, and current stats; skip Earthquake, Toxic, Rock Climb, and upstream's wholesale learnset. |
| Remoraid | Add Signal Beam, Icicle Spear, and Zap Cannon by level, matching Octillery's selected ammunition; retain Snipe Shot, Adaptive Armory, Helping Hand, every existing move, and current stats; skip Rock Blast, Gunk Shot, Aura Sphere, and upstream's wholesale learnset. |
| Rillaboom | Add Soundproof in slot 2, matching Grookey; retain Overgrow, Grassy Surge, Mischief, every existing move, and current stats for regular and Gigantamax Rillaboom; skip upstream's +0/+0/+0/+0/+0/+5 Speed increase. |
| Rolycoly | Add Lava Plume and Power Gem by level, establishing the special Rock/Fire progression approved for Carkol and Coalossal; retain Skyfire, every existing move, and current stats. |
| Rotom | Use upstream's +0/+0/+0/+0/+0/+10 stats and add Ominous Wind and Shadow Ball by level; retain Short Circuit, Eclipse, every existing move, and Divergence's existing Ability setup. |
| Rotom (Fan) | Keep Divergence's existing Ability rework and current stats; add Ominous Wind and Hurricane by level without duplicating Hurricane as a tutor; retain Short Circuit and every existing move. |
| Rotom (Frost) | Keep Divergence's existing Ability rework and current stats; add Ominous Wind and Blizzard by level; retain Short Circuit and every existing move. |
| Rotom (Heat) | Keep Divergence's existing Ability rework and current stats; add Ominous Wind and Overheat by level; retain Short Circuit and every existing move. |
| Rotom (Mow) | Keep Divergence's existing Ability rework and current stats; add Ominous Wind and Leaf Storm by level; retain Short Circuit and every existing move. |
| Rotom (Wash) | Keep Divergence's Static / Water Absorb Ability rework and current stats; add Ominous Wind and Hydro Pump by level; retain Short Circuit and every existing move. |
| Rufflet | Add Steel Wing as a tutor, matching Braviary selectively; retain Keen Eye, Sheer Force, Hustle, Warpath, every existing move, and current stats. Do not give it Quash. |
| Runerigus | Use upstream's +10/+10/+0/-10/+0/+0 redistribution, add Solid Rock as its hidden Ability, and add Shadow Sneak by level; retain Cursed Tablet, Eclipse, Infernal Parade, and every existing move; skip Bulk Up, the legacy tutors, and upstream's wholesale learnset. |
| Sableye | Keep current stats for regular and Mega Sableye, skipping upstream's mismatched stat increases; replace Keen Eye with Magic Guard on regular Sableye; retain Stall, Prankster, Mega Sableye's Magic Bounce, Gemstash, Make It Rain, Cheap Trick, Eclipse, and every existing move. |
| Samurott | Use upstream's +0/+10/+0/-3/+0/+0 stats; change to Water/Fighting; use Torrent / Shell Armor / Super Luck; add Sacred Sword, Low Sweep, Revenge, and Close Combat by level plus Swirling Blade as a tutor; retain Tidal Switch, Upper Hand, Soak, Aerial Ace, and every existing move; skip Shell Smash and the remaining additions. |
| Samurott (Hisuian) | Use upstream's +0/+2/+0/+0/+0/+5 stats; add Shell Armor in slot 2, using Torrent / Shell Armor / Sharpness to preserve Oshawott and Dewott's second-Ability progression; add Swirling Blade as a tutor plus Low Sweep and Revenge by level, preserving Dewott's access; retain Momentum and every existing move. |
| Sandshrew | Add Rough Skin in slot 2 and Sand Tomb by level; retain Sharp Quills, Spin Out, Bulldoze, Agility, every existing move, current stats, and Magnitude as compatibility-only; skip Accelerock and promoting Magnitude to level-up. |
| Sandshrew (Alolan) | Add Ice Body in slot 2 and add Ice Shard and Ice Ball by level; remove Jetstream from Divergence's tutors; retain Snowplow, Spin Out, Cold Snap, Mist, Rollout, Snowscape, every other existing move, and current stats; skip Bide, Hail, and the legacy tutors. |
| Sandslash | Use upstream's +0/+10/+10/-20/+10/+0 redistribution, add Rough Skin in slot 2, and add Spikes and Spiky Shield by level; retain Sharp Quills, Spin Out, Bulldoze, every existing move, and Magnitude as compatibility-only; skip Accelerock, promoting Magnitude to level-up, and the generic tutors. |
| Sandslash (Alolan) | Add Iron Barbs in slot 2 and add Spikes, Spiky Shield, Metal Burst, Defense Curl, and Ice Ball by level; retain Snowplow, Spin Out, Cold Snap, Swords Dance, every existing move, and current stats; skip the legacy tutors. |
| Sawsbuck | Use upstream's +5/+10/+0/+0/+0/+0 stats for all four seasonal forms; add Camouflage, Nature Power, Jump Kick, and High Horsepower by level, matching Deerling; retain Seasons Greeting, Bullet Seed, Zen Headbutt, and every existing move; skip Aromatherapy, Feint Attack, Lunge, Captivate, and the tutors. |
| Sceptile | Use a consistent +0/+20/+0/-20/+0/+0 redistribution for Sceptile and Mega Sceptile, matching Grovyle rather than upstream's mismatched form changes; keep regular Sceptile pure Grass, reserving Grass/Dragon for Mega; add Hyper Cutter in slot 2 plus Dual Chop, Night Slash, Fury Cutter, and Slash by level; retain Canopy Stalker, Mega Sceptile's Lightning Rod, and every existing move; skip Dragon Dance and the remaining additions. |
| Scorbunny | Add Receiver in slot 2, matching Raboot and Cinderace; retain Blaze, Libero, Playmaker, every existing move, and current stats. |
| Scovillain | Keep current stats for Scovillain and Mega Scovillain, skipping upstream's +0/+0/+0/-33/+0/+33 redistribution; add Temper Flare and Raging Fury by level; retain Capsaicin Craze, Mega Scovillain's existing design, Solar Beam, Overheat, and every existing move; skip Solar Blade and Dragon Dance. |
| Seaking | Use upstream's full +0/+30/+15/-20/+0/+10 stats, producing 80/122/80/45/80/78 (BST 485); retain Piercing Verdict, Horn Drill, and every existing move. This preserves Goldeen's approved +10 Speed without an evolution regression; revisit Piercing Verdict later if the stronger spread proves excessive. |
| Sealeo | Add Ice Ball and Slack Off by level; retain Blubber, Cold Snap, Surf, Snowscape, every existing move, and current stats; skip Hail and upstream's wholesale learnset. |
| Seedot | Use upstream's +0/+5/+0/+0/+0/+0 stats; retain Forest Ambush, False Surrender, and every existing move. |
| Seel | Replace hidden Ice Body with Ice Scales, matching Dewgong; retain Thick Fat, Hydration, Glacial Mass, every existing move, and current stats. |
| Sentret | Use upstream's +10/-1/+1/+0/+0/+20 stats; use Scrappy / Keen Eye / Frisk, matching Furret rather than upstream's Hustle arrangement; add Tail Slap by level; retain Scamper, Plain Terrain, and every existing move; skip Flame Wheel, Play Rough, Zen Headbutt, Me First, Foresight, and the tutors. |
| Serperior | Keep pure Grass typing and current stats, skipping upstream's Dragon typing and +5/+1/+0/+1/+0/+0 stats; add Multiscale in slot 2 plus Dragon Breath, Dragon Pulse, and Dragon Rush by level; retain Root Network, Magical Leaf, and every existing move; skip Draco Meteor, Aqua Tail, and the remaining additions. |
| Servine | Keep pure Grass typing and current stats; add Multiscale in slot 2 and Dragon Breath by level; retain Root Network, Magical Leaf, and every existing move; skip Dragon typing and the remaining additions. |
| Seviper | Use upstream's +2/+15/+0/-20/+0/+10 redistribution and change to Poison/Dark; add Venom Drench, Night Slash, Gunk Shot, and Power Whip by level; retain Sidewinder, Barb Barrage, Acid Rain, Foul Mixture, Sludge Bomb, and every existing move; skip Swords Dance, Wring Out, and Final Gambit. |
| Shellos | Keep Divergence as-is for both forms; retain Adaptive Slime, Trump Card, every existing move, and current stats; skip Spikes. |
| Shieldon | Keep current stats, skipping upstream's +0/+20/+0/-20/+0/+0 redistribution; add Solid Rock in slot 2 and Head Smash by level; redesign Impenetrable across the line so super-effective special moves are completely negated; retain Sturdy, Soundproof, and every existing move. |
| Shiftry | Use upstream's +0/+20/+0/+0/+0/+0 stats and add Parting Shot by level; retain Forest Ambush, Cheap Trick, False Surrender, Eclipse, Wild Growth, Leaf Blade, Sunny Day, Air Cutter, Payback, and every existing move; skip the remaining additions and tutors. |
| Shinx | Add Baby-Doll Eyes, Nuzzle, and Thunder Fang by level; retain X-Ray Jaws, Volt Switch, Thunder Wave, Volt Tackle, every existing move, and current stats; skip Play Rough. |
| Shuckle | Make Encore a level-up move; retain Silver Lining, every existing move, and current stats; skip Constrict, Bide, and Recover because reliable recovery would be excessive alongside its unique Ability. |
| Shuppet | Keep Divergence as-is; retain pure Ghost typing, Jumpscare, every existing move, and current stats; skip upstream's Normal typing, Work Up, and Return so Normal remains Mega Banette's payoff. |
| Silvally | Use upstream's +5/+5/+5/+5/+5/+5 stats for every type; retain RKS System, RKS Relay, and every existing move; skip Rock Climb. |
| Sirfetch'd | Keep current stats, skipping upstream's +23/+5/+0/+0/+0/+5 increase; add Sharpness in slot 2, Sacred Sword by level, and Swirling Blade as a tutor, matching Galarian Farfetch'd; retain Sword and Board and every existing move; skip Drill Run, Roost, and the generic tutors. |
| Sizzlipede | Use upstream's +0/+0/+0/-5/+0/+5 redistribution, beginning Centiskorch's approved Speed shift; retain Molten Burrow, Sizzly Slide, Acid Rain, Infested Terrain, and every existing move; skip Toxic. |
| Skiddo | Keep pure Grass, matching Gogoat; retain Ruminate, Wild Growth, every existing move, and current stats; skip Normal typing, Return, and Rock Climb. |
| Skiploom | Use upstream's +0/+0/+0/+20/+0/+0 stats and Wind Rider / Infiltrator / Aerilate; add Air Cutter, Air Slash, Hyper Voice, and Rage Powder by level; retain Pollen Puff, Floaty Fall, and every existing move. |
| Skitty | Use upstream's +0/+5/+5/+5/+5/+10 stats and add Hone Claws as a tutor; retain Heartbreak, Plain Terrain, and every existing move; skip Cut. |
| Skuntank | Keep current stats, skipping upstream's +12/+2/+3/+24/-1/+1 increase; add Acid Spray and Venom Drench by level; retain Septic Fumes, Acid Rain, Foul Mixture, Eclipse, Venoshock, and every existing move; skip Fire Spin, Slash, Poison Jab, and Gunk Shot. |
| Skwovet | Add Recycle by level, matching Greedent; retain Messy Eater, Plain Terrain, every existing move, and current stats; skip the generic tutors. |
| Slaking | Add Slow Start as its hidden Ability while retaining Truant; redesign King's Domain so, when the party shares a type, it nullifies the effects of Slaking's active regular Ability whether it is Truant or Slow Start; retain Plain Terrain, every existing move, and current stats; skip the older attack additions. |
| Slakoth | Add Slow Start as its hidden Ability while retaining Truant, matching Slaking; apply the redesigned King's Domain so its shared-type condition nullifies either regular Ability; retain Plain Terrain, every existing move, and current stats; skip the older attack additions. |
| Sliggoo | Keep pure Dragon typing and current stats; add Recover and Tearful Look by level; retain Sludge Shift, Water Gun, and every existing move; skip Water typing, Bubble, Bide, Hydro Pump, and the Water tutors. |
| Sliggoo (Hisuian) | Add Steel Roller and Tearful Look by level, preserving Goomy's approved utility; retain Shelter, Acid Armor, Rain Dance, Iron Head, every existing move, and current stats; skip Recover, Iron Defense, Mirror Shot, Bubble, and Bide. |
| Slowbro (Galarian) | Use upstream's +0/-25/+10/+15/+0/+0 redistribution; retain Quick Draw, Regenerator, Galarica Rounds, Shell Side Arm, Acid Rain, and every existing move. |
| Slowking | Keep Divergence as-is; retain Royal Rebuke, Psychic Noise, Trump Card, every existing move, and current stats; skip Aura Sphere. |
| Slugma | Use upstream's +10/+10/+10/+20/+10/+0 stats and add Power Gem by level; retain Basalt Shell, Steam Eruption, Acid Rain, and every existing move; skip Flame Burst. |
| Slurpuff | Use upstream's +0/+10/+0/+0/+0/+0 stats and add Covet by level; retain Sugar Rush, String Shot, Sticky Web, and every existing move; skip Light Screen and Safeguard. |
| Smoochum | Use upstream's +0/+0/+0/+10/+0/+0 stats and add Draining Kiss by level, matching Jynx; retain Sweet Nothings, Freezing Glare, and every existing move. |
| Sneasel | Make Ice Shard a level-up move rather than tutor-only; retain Inner Focus / Keen Eye / Pickpocket, Cold Read, Cheap Trick, Eclipse, every existing move, and current stats; skip Technician, Power-Up Punch, and the older Dark moves. |
| Snivy | Add Multiscale in slot 2, matching Servine and Serperior; retain pure Grass typing, Root Network, every existing move, and current stats; skip Dragon Rage and Aqua Tail. |
| Snorlax | Add Rollout by level; retain Deep Sleep, Stuff Cheeks, Plain Terrain, every existing move, and current stats; skip Slack Off, Chip Away, and Rock Climb. |
| Snubbull | Use upstream's +5/+0/+0/+0/+0/+0 stats and keep pure Fairy, matching Granbull; add Double Kick by level; retain Brutal Charge, Plain Terrain, Jaw Lock, every existing move, and Super Fang as a tutor; skip Fighting typing, Covet, Rage, Dynamic Punch, and Close Combat. |
| Sobble | Add Shed Skin in slot 2, completing the approved Sobble-line Ability package, and add Air Cutter as a tutor; retain Torrent, Sniper, Take Aim, every existing move, and current stats; skip the generic tutors. |
| Solrock | Use upstream's +0/+20/+0/+0/+0/+0 stats; retain Sun Totem, Flare Blitz, Sunsteel Strike, and every existing move; skip Mystical Fire, Rock Wrecker, and the wholesale learnset reshuffle. |
| Spearow | Add Intimidate in slot 2, matching Fearow; retain Keen Eye, hidden Sniper, Territorial, Beak Blast, every existing move, and current stats; skip Brave Bird and Dual Wingbeat. |
| Spheal | Add Ice Ball and Slack Off by level, matching Sealeo; retain Blubber, Cold Snap, Snowscape, every existing move, and current stats; skip Hail, Scald, and the wholesale learnset replacement. |
| Spinarak | Keep current stats, skipping upstream's +0/+10/+0/+0/+0/+15 increase; add Poison Fang by level; retain Web Trap, Mortal Spin, Sticky Web, Toxic Thread, Acid Rain, Eclipse, Infested Terrain, and every existing move; skip Swords Dance and the other tutors. |
| Spoink | Use upstream's +5/+0/+0/+0/+0/+0 stats and add Magic Coat and Slack Off by level, matching Grumpig; retain Scramble, Brainstorm, Confusion, Power Gem, and every existing move; skip Psywave, Odor Sleuth, and Extrasensory. |
| Squirtle | Use Torrent / Rain Dish / Shell Armor and apply the same arrangement to Wartortle, matching the approved Blastoise package; retain Shell Formation, every existing move, and current stats. |
| Steelix | Use a consistent +0/+20/+0/-10/+0/+0 redistribution for Steelix and Mega Steelix; add Coil and Glare by level; retain Subterranean, Gyro Ball, and every existing move; skip Rock Climb and the wholesale learnset replacement. |
| Stoutland | Use upstream's +10/+5/+0/+0/+0/+0 stats and add Odor Sleuth by level; retain Field Runner, Plain Terrain, and every existing move; skip High Horsepower. |
| Stunfisk | Use upstream's +0/+0/+0/+9/+0/+0 stats and replace Limber with Dry Skin; retain Static, Sand Veil, Crossed Wires, Static Burst, and every existing move; skip Toxic and Infestation. |
| Stunfisk (Galarian) | Use upstream's +0/+9/+0/+0/+0/+0 stats; add Dry Skin in slot 2, Iron Head by level, and Spikes as a tutor; retain Mimicry, False Ground, Snap Trap, and every existing move; skip Arena Trap, Aqua Tail, and the generic tutors. |
| Stunky | Keep current stats, skipping upstream's +2/+2/+3/+24/-1/+1 increase; add Venom Drench by level, with Acid Spray already present; retain Septic Fumes, Foul Mixture, Acid Rain, Eclipse, and every existing move. |
| Sudowoodo | Use upstream's +20/+15/+10/+0/+10/+0 stats; retain Divergence's Grass typing, Fauxliage, and every existing move. |
| Suicune | Replace hidden Inner Focus with Water Absorb; retain Pressure, Misty Mirage, every existing move, and current stats; skip Frost Breath and Hurricane. |
| Sunkern | Use upstream's +20/+0/+20/+0/+20/+0 stats and replace Early Bird with Seed Sower; retain Chlorophyll, Solar Power, Solarboost, Chloroblast, and every existing move. |
| Surskit | Use upstream's +0/+0/-2/+5/+3/+0 redistribution; retain Bug/Water typing, Swift Swim, Rain Dish, Water Glide, and every existing move. |
| Swablu | Keep Divergence as-is; retain Cotton Guarding, Sparkling Aria, Disarming Voice, Moonblast, Brave Bird access, every existing move, and current stats; skip upstream's +0/+20/+0/+20/+0/+0 increase. |
| Swalot | Use upstream's +0/+0/+5/+20/+5/+0 stats; retain Acid Reflux, Stockpile, Acid Rain, Stuff Cheeks, and every existing move; skip Earth Power and Thunderbolt. |
| Swampert | Use Torrent / Damp / Water Veil on regular Swampert, completing the approved Mudkip-line arrangement; retain Mega Swampert's Swift Swim, Floodplain, every existing move, and current stats; skip Toxic and Scald. |
| Swanna | Use upstream's +7/-24/+2/+22/+20/+0 redistribution and replace Keen Eye with Friend Guard, matching Ducklett; add Hydro Pump by level; retain Big Pecks, Hydration, Migration, and every existing move; skip Water Sport. |
| Swellow | Use upstream's +0/+0/+0/+10/+0/+0 stats; retain Brave Bird, Air Slash, Boomburst, Guts, Scrappy, Bravery, and every existing move; skip Extreme Speed and Dual Wingbeat. |
| Swirlix | Add Covet by level, matching Slurpuff; retain Sugar Rush, Aromatherapy, Draining Kiss, String Shot, every existing move, and current stats; skip Light Screen and Safeguard. |
| Swoobat | Use upstream's +3/+0/+8/+9/+20/+0 stats; add Heart Swap by level; retain Simple, Stored Power, Mood Swing, every existing move, and Nasty Plot as a tutor; skip Hurricane, Mystical Fire, and the wholesale learnset replacement. |
| Sylveon | Use Cute Charm / Healer / Pixilate, replacing the duplicate slot 2; add Sparkly Swirl as a late-level move; retain Serene Voice, Veevee Volley, Alluring Voice, every existing move, and current stats; skip Grass Knot and the wholesale learnset replacement. |
| Taillow | Keep Divergence as-is; retain Guts, Scrappy, Bravery, Jetstream, every existing move, and current stats; skip Dual Wingbeat, matching Swellow. |
| Talonflame | Use upstream's +0/+10/+0/+0/+0/+0 stats; retain Flame Body, Gale Wings, Windsurge, Jetstream, Plain Terrain, and every existing move. |
| Tapu Bulu | Add Play Rough and Rototiller by level; retain Grassy Surge, Rooted Shrine, and every existing move; skip the tutor duplicates, Toxic, Rock Climb, and upstream's wholesale learnset replacement. |
| Tapu Fini | Add Refresh by level; retain Misty Surge, Mistbound Shrine, every existing move, and current stats. |
| Tapu Koko | Add Play Rough by level; retain Electric Surge, Crackling Shrine, every existing move, and current stats; skip Mirror Move, Electro Ball, and upstream's wholesale learnset replacement. |
| Tarountula | Use Analytic / Stakeout / Disguise, matching Spidops; add Stealth Rock, Pounce, and Rage Powder by level; retain Tripwire, Acid Rain, Infested Terrain, every existing move, and current stats; skip Mach Punch, False Surrender, and Poison Jab. |
| Tauros | Keep Divergence as-is; retain Raging Bull, Open Field, Plain Terrain, every existing move, and current stats; skip upstream's +0/+0/+0/+30/+0/+0 Special Attack increase, Head Charge, and Strength. |
| Teddiursa | Use Quick Feet / Supersweet Syrup / Honey Gather and add Mega Kick as a tutor; retain Honey Rage, every existing move, and current stats; skip the Ground moves and upstream's wholesale learnset replacement, deferring the Ground identity decision until Ursaring. |
| Tepig | Use Blaze / Thick Fat / Sheer Force, matching Pignite; retain Scorching Relay, every existing move, and current stats. |
| Terrakion | Add Swirling Blade as a tutor; retain Vengeful Force, every existing move, and current stats; skip Low Sweep and Rock Climb. |
| Thwackey | Add Soundproof in slot 2, completing the approved Grookey-line package; retain Mischief, every existing move, and current stats; skip the generic legacy tutors. |
| Togekiss | Add Air Cutter by level, preserving Togetic's access; retain Divine Favor, every existing move, and current stats; skip Soft-Boiled, Calm Mind, Moonblast, and Hurricane. |
| Togepi | Make Draining Kiss a level-up move rather than tutor-only; retain Divine Favor, every existing move, and current stats; skip Moonblast, Soft-Boiled, Calm Mind, and upstream's wholesale learnset replacement. |
| Togetic | Add Air Cutter and Draining Kiss by level; retain Divine Favor, every existing move, and current stats; skip Moonblast, Calm Mind, Hurricane, Brave Bird, Extrasensory, and upstream's wholesale learnset replacement. |
| Torchic | Add Early Bird in slot 2, matching Combusken and Blaziken; retain Blaze, Speed Boost, Flash Firestorm, every existing move, and current stats. |
| Torterra | Keep current stats, skipping upstream's +10/+0/+5/-10/+0/+5 redistribution; add Rock Head in slot 2; retain Uproot, Grav Apple, and every existing move; skip Head Smash, Shell Smash, Sleep Powder, Leaf Blade, and Rock Climb. |
| Totodile | Add Strong Jaw in slot 2 and Psychic Fangs by level, matching Croconaw and Feraligatr; retain Death Roll, Jaw Lock, every existing move, and current stats; skip Dragon Rush, Avalanche, and upstream's wholesale learnset replacement. |
| Tranquill | Use upstream's +3/-27/+3/+27/+0/+1 redistribution and replace Rivalry with Unaware, continuing Pidove's approved special redesign; make Swift a level-up move rather than tutor-only and add Hurricane as a tutor, preserving Pidove's access; retain Soaring Gale and every existing move; skip Brave Bird and upstream's wholesale learnset replacement. |
| Trapinch | Keep pure Ground typing and current stats, skipping upstream's Bug/Ground typing and +10/+0/+0/+0/+0/+0 increase; make Bug Bite a level-up move rather than tutor-only; retain Desert Shroud, First Impression as a tutor, and every existing move; skip Lunge, Uproar, and upstream's wholesale learnset replacement. |
| Treecko | Use upstream's +0/+20/+0/-20/+0/+0 redistribution, add Hyper Cutter in slot 2, and add Slash by level; retain Sunstalker and every existing move; skip Dynamic Punch, Work Up, Slam, Assurance, and Iron Tail. |
| Trubbish | Keep Divergence as-is; retain Trash Heap, Pain Split, Baneful Bunker, Acid Rain, every existing move, and current stats; skip upstream's +15/+0/+0/+5/+0/+0 increase, Cross Poison, and wholesale learnset replacement. |
| Turtwig | Add Rock Head in slot 2, completing the approved line; retain Uproot, Grav Apple by level, Shell Smash as an existing tutor, every existing move, and current stats; skip Sleep Powder, Leaf Blade, Rock Climb, and upstream's wholesale learnset replacement. |
| Tynamo | Add Nuzzle at level 1, matching Eelektrik and Eelektross; retain Bioluminescence, Thunder Wave, every existing move, and current stats. |
| Typhlosion (Hisuian) | Keep current stats, skipping upstream's +0/+0/-5/+10/-5/+1 redistribution; add Flash Fire in slot 2 and Mystical Fire as a tutor, preserving Cyndaquil/Quilava's access; retain Blaze, hidden Frisk, Guiding Flames, and every existing move. |
| Typhlosion | Keep pure Fire typing and current stats, skipping upstream's Fire/Ground typing and +0/-5/-5/+15/-5/+1 redistribution; use Blaze / Flash Fire / Adaptability and add Mystical Fire as a tutor; retain Blinding Smoke and every existing move; skip Earth Power, Scorching Sands, Rock Climb, and upstream's wholesale learnset replacement. |
| Tyranitar | Keep Divergence as-is; retain Battle Armor / empty slot / Unnerve, Dominion, every existing move, and current stats; skip Earth Eater. Upstream gives Earth Eater to Larvitar and Tyranitar but not Pupitar, and completing the package would make Tyranitar too strong. |
| Tyrantrum | Keep Divergence as-is; retain Strong Jaw, Rock Head, Head Smash, Horn Drill, Tyrant Storm, every existing move, and current stats; skip Accelerock and upstream's wholesale learnset replacement. |
| Tyrunt | Keep Divergence as-is; retain Strong Jaw, Sturdy, Horn Drill, Tyrant Storm, every existing move, and current stats; skip Accelerock and upstream's wholesale learnset replacement, matching Tyrantrum. |
| Umbreon | Use Synchronize / Inner Focus / Poison Touch, moving its former hidden Inner Focus into the duplicate second slot; add Baddy Bad as a late-level move; retain Shadow Carapace, Eclipse, Veevee Volley, every existing move, and current stats; skip Play Rough, Gunk Shot, Mystical Fire, Power Gem, and the other upstream additions. |
| Unfezant | Use upstream's +0/-50/+0/+45/+0/+2 redistribution and replace Rivalry with Unaware, completing Pidove and Tranquill's approved special progression; make Swift a level-up move rather than tutor-only; retain Soaring Gale, Hurricane as an existing tutor, and every existing move; skip Boomburst, Brave Bird, and upstream's wholesale learnset replacement. |
| Ursaring | Keep pure Normal, reserving Ground/Normal for Ursaluna; use Guts / Supersweet Syrup / Unnerve and add Mega Kick as a tutor; retain Honey Rage, High Horsepower, Earthquake access, every existing move, and current stats. Ursaluna and Bloodmoon Ursaluna may naturally retain Mega Kick after evolution; in Divergence it is Fighting-type coverage rather than Normal STAB. |
| Uxie | Change Psychic to Psychic/Fairy and add Moonblast by level, completing the approved lake-trio direction; retain Still Mind, every existing move, and current stats; skip Play Rough, Natural Gift, and upstream's wholesale learnset replacement. |
| Vanillish | Keep Divergence as-is and mono-Ice; retain Meltdown, every existing move, and current stats; skip upstream's +0/-6/+0/+0/+0/+6 redistribution, Disarming Voice, Mirror Shot, and wholesale learnset replacement. |
| Vanillite | Keep Divergence as-is and mono-Ice; retain Meltdown, every existing move, and current stats; skip upstream's +0/-6/+0/+0/+0/+6 redistribution, Disarming Voice, Mirror Shot, and wholesale learnset replacement. |
| Vanilluxe | Keep Divergence as-is and mono-Ice; retain Meltdown, Slush Rush, every existing move, and current stats; skip upstream's Ice/Fairy typing, +0/-16/+0/+0/+0/+16 redistribution, Moonblast, Power Gem, Dazzling Gleam, Extrasensory, and wholesale learnset replacement. |
| Vaporeon | Use Water Absorb / Water Veil / Hydration, replacing the duplicate slot 2; add Bouncy Bubble as a late-level move; retain Body of Water, Veevee Volley, every existing move, and current stats; skip Scald, Extrasensory, Grass Knot, Toxic, and upstream's wholesale learnset replacement. |
| Venomoth | Use upstream's +0/-10/+0/+10/+0/+0 redistribution; skip Hurricane; retain Neurotoxin, Morning Sun as an existing tutor, and every existing move. |
| Venonat | Use upstream's +10/+0/+5/+10/+5/+0 stats; retain Neurotoxin and every existing move. |
| Venusaur | Keep current stats for Venusaur, Mega Venusaur, and Gigantamax Venusaur, skipping upstream's +0/+0/+0/+10/+0/+0 increase; add Sweet Veil in slot 2 to regular and Gigantamax Venusaur, matching Bulbasaur and Ivysaur; retain Mega Venusaur's Thick Fat, Vine Lash, and every existing move. |
| Vespiquen | Use Pressure / Unnerve / Queenly Majesty and add Fly as a tutor; retain Hive Command, every existing move, and current stats. |
| Vibrava | Keep Divergence as-is; retain Ground/Dragon typing, Desert Shroud, Bug Buzz, every existing move, and current stats; skip upstream's Bug/Dragon typing, +10/+10/+0/+40/+5/+0 stats, Compound Eyes, Dragon Fly, Quiver Dance, and wholesale learnset replacement. |
| Victini | Keep Divergence as-is; retain Victory, V-create, Searing Shot, Stored Power, every existing move, and current stats; skip Flame Burst and upstream's wholesale learnset replacement. |
| Vigoroth | Add Sheer Force as its hidden Ability; retain Vital Spirit, Fever Pitch, every existing move, and current stats; skip upstream's wholesale learnset replacement. |
| Vikavolt | Keep Divergence as-is; retain Levitate, Dynamo, every existing move, and current stats; skip upstream's +30/+10/+5/+0/+5/+50 increase. |
| Vileplume | Add Leaf Guard in slot 2, matching Oddish and Gloom; retain Chlorophyll, hidden Effect Spore, Toxic Bloom, Moonblast, every existing move, and current stats; skip upstream's Thick Fat, Earth Power, Apple Acid, and wholesale learnset replacement. |
| Virizion | Add Swirling Blade as a tutor; retain Verdant Vow, every existing move, and current stats; skip Poison Jab, Low Sweep, and Rock Climb. |
| Vivillon | Keep Divergence as-is for every pattern; retain Compound Eyes, Duststorm, Quiver Dance, Hurricane, every existing move, and current stats; skip upstream's +10/-2/+5/+25/+5/+21 increase, Dual Wingbeat, and Fly. |
| Volbeat | Use upstream's +0/-40/+0/+60/+0/+15; retain Bug/Electric typing; use Illuminate / Tinted Lens / Swarm, replacing redundant Prankster; add Flash, Charge Beam, Signal Beam, Thunderbolt, and Thunder by level plus Volt Switch as a tutor; skip Dual Wingbeat; retain Call, Shimmer, Infested Terrain, and every existing move. |
| Volcanion | Add Thermal Exchange as its hidden Ability; retain Water Absorb, Pressure Valve, every existing move, and current stats. |
| Volcarona | Add Inferno as a tutor; retain Radiant, Quiver Dance, Fiery Dance, every existing move, and current stats. |
| Voltorb | Add Bug Buzz as a tutor, matching Electrode; skip Energy Ball and Flash Cannon; retain Volt Break, Static Burst, Thundercrush, Zippy Zap, every existing move, and current stats. |
| Vullaby | Add Hurricane as a tutor; retain Carrion Hour, Eclipse, every existing move, and current stats; skip Superpower and Zen Headbutt, matching the Mandibuzz decision. |
| Vulpix | Add Moonblast as a tutor; retain Sun Revelry, every existing move, and current stats; skip Dazzling Gleam, which remains Ninetales's evolution move. |
| Wailord | Use upstream's +30/+0/+0/+0/+0/+0; add Bouncy Bubble as a late-level move; retain Body Slam, Tidal Flood, and every existing move; skip Focus Blast and the wholesale learnset reshuffle. |
| Walrein | Add Ice Ball and Slack Off by level, completing the approved Spheal and Sealeo package; retain Surf, Snowscape, Blubber, Cold Snap, every existing move, and current stats; skip Hail, Toxic, and Scald. |
| Weavile | Keep Divergence as-is; retain Night Slash, Cold Read, Cheap Trick, Eclipse, every existing move, and current stats; skip Crunch. |
| Galarian Weezing | Add Aromatherapy by level; retain the existing Gunk Shot, Smog Refinery, every existing move, and current stats; skip Lava Plume and the generic legacy tutors. |
| Whimsicott | Use upstream's +0/+0/+0/+10/+0/+0; add Sleep Powder by level, matching Cottonee; retain Tufted Away, the existing Hurricane, Moonblast, Tailwind, and every existing move; skip Grass Whistle and the wholesale learnset reshuffle. |
| Whiscash | Use upstream's +0/+15/+15/+0/+2/+0; replace Oblivious with Simple and add Breaking Swipe as a tutor, matching Barboach; retain Electrocytes and every existing move. |
| Whismur | Add Scrappy in slot 2, matching Loudred; retain Soundproof, Rattled, Distortion, every existing move, and current stats; skip Bug Buzz, matching the Exploud decision. |
| Woobat | Use upstream's +0/+0/+0/+10/+0/+0; retain Simple, Stored Power, Mood Swing, and every existing move; skip the wholesale level-up replacement. |
| Wooloo | Use upstream's +10/+10/+0/+0/+0/+0, matching Dubwool; retain Grazing Field and every existing move; skip Toxic, Return, Frustration, and Hidden Power. |
| Wooper | Use upstream's +0/+10/+10/+0/+0/+0, matching Quagsire; retain Clueless and every existing move. |
| Paldean Wooper | Use upstream's +0/+10/+10/+0/+0/+0; retain Bog Body and every existing move; leave Clodsire unchanged because it has no upstream Revised changes. |
| Xatu | Use upstream's +0/-10/+0/+10/+10/+0; add Lucky Chant, Miracle Eye, and Ominous Wind by level, matching Natu; retain Wish, Omnisense, Esper Wing, Lumina Crash, Eclipse, and every existing move; skip Me First, Mystical Fire, Aura Sphere, and the wholesale learnset replacement. |
| Yamask | Add Ominous Wind by level, matching Cofagrigus; retain Mummy, Petrify, Grudge, Infernal Parade, Eclipse, every existing move, and current stats; skip Cursed Body and Shadow Sneak because those changes would not continue into Cofagrigus. |
| Galarian Yamask | Add Shadow Sneak by level, matching Runerigus; retain Wandering Spirit, Cursed Tablet, Infernal Parade, Eclipse, every existing move, and current stats; skip Cursed Body and the generic legacy tutors because Runerigus receives Solid Rock instead. |
| Yanmega | Add Hypnosis, Fly, and Hurricane as tutors; retain Aerodynamic, Speed Boost, Tinted Lens, every existing move, and current stats. |
| Zacian | Add Swirling Blade as a tutor to both Zacian forms; retain Intrepid Sword, Royal Advance, every existing move, and current stats; skip Toxic, Return, and Frustration. |
| Zangoose | Use upstream's +2/+0/+0/+0/+0/+5; retain X-Scissor as its existing level-up move and tutor, Vendetta, and every existing move; skip Lunge because it is not a slicing move, plus Acrobatics, Rock Climb, Cut, and the wholesale learnset reshuffle. |
| Zapdos | Add Air Slash and Extrasensory as tutors; retain Storm Call, every existing move, and current stats. |
| Galarian Zapdos | Keep Divergence as-is; retain Defiant, Dead Heat, every existing move, and current stats; skip Air Slash, Extrasensory, and the generic legacy tutors. |
| Zebstrika | Add Nuzzle, Flame Wheel, and Flare Blitz by level, completing Blitzle's approved physical package; retain the existing Smart Strike, High Horsepower, Zippy Zap, Redline, every existing move, and current stats; skip Pursuit, Lunge, Low Sweep, and Flamethrower. |
| Zigzagoon | Keep Divergence as-is; retain the existing level-up and tutor access to Tail Slap, Switchstep, every existing move, and current stats; skip upstream's +0/+10/+0/+0/+0/+0 increase, Rock Climb, and wholesale learnset reshuffle, matching the Linoone decision. |
| Galarian Zigzagoon | Keep Divergence as-is; retain Heckler, every existing move, and current stats; skip upstream's +0/+10/+0/+0/+0/+0 increase, matching the Galarian Linoone decision. |
| Zubat | Use upstream's +0/+0/+0/+15/+0/+0; add Sniper in slot 2 and Hurricane as a tutor, beginning the approved Golbat and Crobat special progression; retain Inner Focus, Infiltrator, Vampiric, and every existing move. |
| Zweilous | Keep Divergence as-is; retain Nasty Plot, Assurance, Apocalypse, Fiery Wrath, Eclipse, every existing move, and current stats; skip Dragon Rage and the wholesale level-up reshuffle, matching the Hydreigon decision. |

## Initial Divergence-design review

| Pokemon | Decision |
|---|---|
| Arbok | Replace level-up Sludge Bomb with Poison Fang. |
| Ariados | Add Poison Fang by level, preserving Spinarak's approved access; otherwise keep Divergence as-is. |
| Aromatisse | Copy the upstream Revised Mode learnset. |
| Banette | Copy upstream's base stats for Banette and Mega Banette; add Normal as Mega Banette's secondary type. |
| Beautifly | Use 60/70/50/120/50/85; add Air Slash plus upstream's Fly, Iron Defense, Ominous Wind, and Struggle Bug tutors; skip Hurricane. |
| Beheeyem | Add Psycho Boost as a late-level capstone; keep Divergence's existing learnset additions; skip Power Gem and the rest of upstream's wholesale replacement, including Mystical Fire. |
| Boltund | Add Ice Fang as a tutor; skip upstream's other tutor additions and keep Divergence's existing stats. |
| Cacturne | Add Rough Skin in regular Ability slot 2; make Needle Arm a level-up move; add Rock Climb and Scorching Sands as tutors; keep Power Trip and Divergence's existing changes. |
| Carnivine | Add Leech Life as a tutor; skip Dark Pulse and keep Divergence's +20 HP. |
| Castform | Keep Divergence as-is; do not copy upstream's faster, frailer stat spread. |
| Chimecho | Keep Divergence as-is; skip upstream's Power Gem, Weather Ball, and stat changes. |
| Dedenne | Replace Plus with Electric Surge; add Charge Beam and Draining Kiss; retain Super Fang and skip Thunder Wave. Defer possible 67/58/67/90/67/101 stat spread. |
| Delcatty | Add Hone Claws as a tutor; skip Power Gem and Cut. Defer upstream's much larger stat increase. |
| Delibird | Replace Hustle with Technician and Insomnia with Sheer Force; add Icy Wind, Swift, Air Cutter, Brave Bird, Air Slash, Blizzard, and Hurricane by level plus Nasty Plot and U-turn as tutors; skip Work Up, Hone Claws, Cut, Strength, and upstream's larger stat increase. |
| Dewgong | Replace Ice Body with Ice Scales; use 90/70/80/90/95/70; retain Glacial Mass and Alluring Voice. |
| Dubwool | Add Rock Climb as a tutor; use 82/90/100/60/90/88; skip upstream's legacy tutor additions. |
| Dustox | Add Sludge Bomb by level and Struggle Bug as a tutor; skip Hurricane, Fly, Ominous Wind, and upstream's stat redistribution; retain Repellant and Divergence's stats. |
| Eldegoss | Keep Divergence as-is; skip upstream's legacy tutor and stat additions. |
| Emolga | Add Wild Charge and Electro Ball by level; skip Pursuit, Shock Wave, Last Resort, the wholesale level reshuffle, and upstream's physical stat spread. |
| Falinks | Keep Divergence as-is; do not port upstream's Bug typing, moves, tutors, or stat spread. |
| Frosmoth | Keep Divergence as-is; skip upstream's legacy tutor additions. |
| Grapploct | Change to Fighting/Water; add Sticky Hold in slot 2; add Aqua Jet, Mach Punch, Liquidation, Arm Thrust, and Bubble by level plus Flip Turn as a tutor, preserving Clobbopus's access; skip Brutal Swing, legacy tutors, and upstream's stats; retain Octolock. |
| Grumpig | Add Slack Off and Magic Coat by level; retain Confusion, Scramble, and Divergence's stats; skip Psywave, Odor Sleuth, and Extrasensory. |
| Gumshoos | Use 88/125/70/52/70/45; otherwise retain Divergence's Abilities and learnset. |
| Heatmor | Add Power Whip as a tutor; skip upstream's other Ability, move, and stat changes. |
| Hypno | Keep Divergence as-is. |
| Kecleon | Keep Divergence as-is; its existing stats account for Stealth providing consistently safe entry. |
| Klawf | Replace Shell Armor with Sharpness and Regenerator with Technician; add Accelerock and Mighty Cleave by level; skip Close Combat, Shell Smash, Psychic Fangs, Spikes, and upstream's stats; retain Cliffside. |
| Komala | Use 85/115/65/75/95/75; otherwise retain Divergence as-is. |
| Kricketune | Add Soundproof in slot 2; add Pin Missile by level plus Bullet Seed, Rock Tomb, and Bulldoze as tutors; skip Bide and upstream's stats. |
| Ledian | Add Technician in slot 2 while retaining Swarm and hidden Iron Fist; add Mega Punch and Meteor Mash by level plus Dual Wingbeat and Lunge as tutors; skip Smart Strike and upstream's stats. |
| Lumineon | Replace Water Veil with Dazzling; add Tail Glow by level; retain Tailwind, Submerge, and Divergence's stats; skip the rest of upstream's learnset and stats. |
| Luvdisc | Change to Water/Fairy; add Hydration in slot 2 and replace hidden Hydration with Soul-Heart; add Lovely Kiss and Moonblast by level; retain Wish, Baby-Doll Eyes, Heartthrob, and Divergence's stats; skip Heart Stamp, Captivate, Scald, and upstream's stats. |
| Magcargo | Replace Magma Armor with Weak Armor and hidden Weak Armor with Solid Rock; add Shelter and Power Gem by level; retain Basalt Shell, Steam Eruption, Acid Rain, and Divergence's stats; skip Flame Burst, Toxic, and upstream's stats. |
| Maractus | Use Water Absorb / Rough Skin / Seed Sower; use 75/106/72/86/81/60; add Horn Leech by level and make Needle Arm level-up rather than compatibility-only; retain Prickly and Divergence's Spiky Shield placement; skip the wholesale upstream learnset and Scald. |
| Marowak | Add Head Smash as a tutor to regular and Alolan Marowak, preserving Cubone's approved access; assess the move on both forms during balance testing; skip Rock Climb and Alolan Marowak's legacy additions; otherwise retain both forms as-is. |
| Meowstic | Skip Mystical Fire for both genders and their Megas; retain Divergence's existing moves, gender-differentiated stats, and Screen Test. |
| Mightyena | Keep Divergence as-is; skip upstream's stat redistribution. |
| Mothim | Add Compound Eyes in slot 2; use 70/70/50/110/50/100; add Hurricane as a tutor; retain Swarm Surge, Shimmer, and the custom terrain tutors. |
| Oinkologne | Keep Divergence as-is for both genders. |
| Orbeetle | Keep Divergence as-is for both forms; skip Signal Beam and upstream's legacy tutors. |
| Parasect | Add Crabhammer, Sappy Seed, and Lunge by level plus Grassy Glide as a tutor, preserving Paras's access despite the typing change; skip Bug Buzz and Skitter Smack; retain Divergence's Ghost/Grass typing, stats, and Fungal Infection. |
| Shiinotic | Use 90/45/80/100/105/30; otherwise retain Divergence as-is. |
| Spidops | Use Analytic / Stakeout / Disguise; use 85/94/92/52/86/41; add Stealth Rock, Pounce, and Rage Powder by level; retain moves removed by upstream; skip Mach Punch, False Surrender, and Poison Jab. |
| Squawkabilly | Keep Divergence as-is for all four forms; skip upstream's stat increases. |
| Stonjourner | Keep Divergence as-is; skip upstream's legacy tutor additions. |
| Sunflora | Retain pure Grass typing and Divergence's stats; replace Early Bird with Seed Sower; add Heat Wave and Fiery Dance by level, keeping Heat Wave as its doubles option; retain Solarboost, Chloroblast, and Sappy Seed; skip Flamethrower, Mystical Fire, upstream's other Fire tutors, and wholesale learnset. |
| Thievul | Use 70/38/58/97/112/90; add Mystical Fire by level; retain Bag of Tricks, Cheap Trick, and Eclipse; skip upstream's legacy tutors. |
| Togedemaru | Add Rollout and Zippy Zap by level; otherwise retain Divergence as-is. |
| Trevenant | Keep Divergence as-is; retain Grave Grove, the +10 HP increase, Psychic Noise, Eclipse, Sappy Seed, and Wild Growth. |
| Tropius | Add Natural Gift and Bestow by level plus Weather Ball as a tutor; retain Wide Guard, Outrage, Dragon Hammer, Tropical Canopy, and Divergence's stats; skip upstream's 525 BST spread. |
| Turtonator | Use Shell Armor / Anger Shell / Magma Armor; use 80/58/135/101/85/36; add Lava Plume and Dragon Tail as tutors; retain Mortar Shell and skip Charge Beam and Shock Wave. |
| Unown | Add Nature Power by level; retain Divergence's six existing move additions, Mystic Power, and 48/72/48/72/48/72 stats; skip upstream's larger stat spread. |
| Watchog | Keep Divergence as-is; retain Sentry Post, Plain Terrain, and Divergence's stats. |
| Wigglytuff | Replace Cute Charm with Friend Guard; add Moonblast and Boomburst by level; retain Competitive, Frisk, Soft Body, and Divergence's stats; skip Huge Power, Lovely Kiss, Power Gem, and Curse. |
| Wormadam (Plant Cloak) | Add Shield Dust in slot 2; use 60/50/90/110/110/30 (BST 450); retain Environmental and the custom terrain tutors. |
| Wormadam (Sandy Cloak) | Add Rough Skin in slot 2; use 80/85/105/59/85/36 (BST 450), adding 6 Attack to Divergence's spread so all cloaks share the same BST; retain Environmental and the custom terrain tutors. |
| Wormadam (Trash Cloak) | Add Wonder Skin in slot 2; use 80/72/95/72/95/36 (BST 450), adding 3 Attack and 3 Special Attack to Divergence's spread so all cloaks share the same BST; retain Environmental and Infested Terrain. |
| Wugtrio | Keep Divergence as-is; retain Dirty Tricks and the +15 Attack increase. |

Stats and typing remain a separate review track from Abilities and learnsets.

## Shared design rules audit — approved decisions

All 27 standard starter families receive a distinct second regular Ability.
The following table records slot 2, not the hidden or unique Ability. Each entry
applies to all evolutionary stages.
Regional Decidueye, Typhlosion, and Samurott use the same second slot as their
shared pre-evolutions. Gigantamax forms follow their base forms; Megas retain
their separately approved Mega Abilities. Battle Bond Greninja retains Battle Bond.

| Generation | Grass family — slot 2 | Fire family — slot 2 | Water family — slot 2 |
|---|---|---|---|
| 1 | Bulbasaur — Sweet Veil | Charmander — Defiant | Squirtle — Rain Dish |
| 2 | Chikorita — Filter | Cyndaquil — Flash Fire | Totodile — Strong Jaw |
| 3 | Treecko — Hyper Cutter | Torchic — Early Bird | Mudkip — Damp |
| 4 | Turtwig — Rock Head | Chimchar — Flame Body | Piplup — Vital Spirit |
| 5 | Snivy — Multiscale | Tepig — Thick Fat | Oshawott — Shell Armor |
| 6 | Chespin — Iron Barbs | Fennekin — Magician | Froakie — Infiltrator |
| 7 | Rowlet — Keen Eye | Litten — Rivalry | Popplio — Healer |
| 8 | Grookey — Soundproof | Scorbunny — Receiver | Sobble — Shed Skin |
| 9 | Sprigatito — Magician | Fuecoco — Soundproof | Quaxly — Dancer |

The newly approved Paldea packages are Overgrow / Magician / Protean for
Sprigatito, Floragato, and Meowscarada; Blaze / Soundproof / Unaware for Fuecoco,
Crocalor, and Skeledirge; and Torrent / Dancer / Moxie for Quaxly, Quaxwell, and
Quaquaval. Preserve their current stats, moves, and unique Abilities.

Every Eeveelution has three distinct regular Abilities in the approved design:

| Pokemon | Slot 1 | Slot 2 | Hidden Ability |
|---|---|---|---|
| Vaporeon | Water Absorb | Water Veil | Hydration |
| Jolteon | Volt Absorb | Static | Quick Feet |
| Flareon | Flash Fire | Flare Boost | Guts |
| Espeon | Synchronize | Forewarn | Magic Bounce |
| Umbreon | Synchronize | Inner Focus | Poison Touch |
| Leafeon | Leaf Guard | Super Luck | Chlorophyll |
| Glaceon | Snow Cloak | Serene Grace | Ice Body |
| Sylveon | Cute Charm | Healer | Pixilate |

Eevee itself already has Run Away / Adaptability / Anticipation and needs no
change. Smolder activates Flare Boost in the current battle code, giving Flareon
a special attacking alternative to Guts without changing its stats.

Volbeat and Illumise retain equal BSTs: both are 430 in the canonical baseline
and will be 465 after the approved changes. Illumise drops the earlier +5 HP
increase and replaces hidden Prankster with Swarm, matching Volbeat's final two
slots while retaining its distinct first Ability. Both Qwilfish forms remain at
440 BST. Huntail gains Intimidate and Gorebyss gains Dazzling in slot 2, giving
both three distinct regular Abilities; their stats stay unchanged.
