# Safari Hub Rework: Pokémon Containment Lab

Status: implemented. The agreed specification is retained below; implementation
details and validation notes follow it. Interactive emulator verification remains
required before treating the visual and tutorial flow as manually approved.

## Agreed direction

Rebuild the Safari hub node as a Pokémon holding and research lab within the
underground base. Combine glass habitats with specimen capsules. Display a small
selection of actual Safari-buffer Pokémon, with the existing full collection
and purchase flow accessed through a dark PC modeled on the Adventure Console.

Reserve the rear/north side for a natural cave wall and the existing legendary
cave entrance. Preserve the cave interior and its mechanics. Reframe the opening
catch tutorial as a containment accident: the glass broke and Pokémon escaped.

## Existing integration points

- The normal and tutorial Safari maps are separate 38 × 32 layouts. Rework both
  as states of the same room, retaining their identities and buffers.
- West, east, and south connect to hub districts. North is intentionally reserved
  in the hub-building logic and must remain unavailable as a district connection.
- Normal-map warps 0–5 serve hub exits, 6 enters the legendary cave, and 7 is the
  hub teleport arrival. Preserve these indices. The tutorial has its own six
  arrival warps and entrance-specific exit restrictions.
- The current sign opens View Pokémon / Dismiss All and the existing Pokéblock
  purchase flow. This remains the functional interface, presented as a PC.
- Normal Safari roaming spawns are currently disabled. Containment displays need
  dedicated bounded objects, not a global re-enabling of wild spawning.
- The tutorial creates three starter choices from the selected Pokédex and ends
  after one successful catch. Preserve those choices, battle rules, and timing.
- The legendary entrance unlock comes from the existing True Legend quest reward.

## Room composition

Use a compact chamber inside the existing buffer, with deliberate black exterior
space rather than filling the whole map with floor. Establish an integer centerline
and symmetrical architecture before placing equipment.

The rear center is exposed grey rock framed by industrial supports. When locked,
the cave opening is sealed rock; the existing unlock reveals a natural tunnel
into `Rogue_Interior_SafariCave`. Keep its approach unobstructed. It should read
as a lab built around an unusual cavern, not an ordinary north hub hallway.

Arrange two glass habitat pens to the left and right of the main approach. Give
each enough visible floor for a Pokémon to pace, turn, and remain recognizable.
Place two smaller specimen capsules against rear wall sections to complement
the larger habitats. This provides four normal display positions: two moving
habitat occupants and two capsule occupants with restrained idle movement.

Place the dark Safari PC in an obvious control alcove facing the public aisle,
away from the cave doorway. Reuse the Adventure Console's dark chassis and black
screen with small text lines, with Safari-specific interaction text. Keep the
two existing builder workbenches available, positioned against top walls in
side recesses. Keep all NPCs and equipment out of the hub crossing.

West, east, and south passages must be three walkable tiles wide throughout,
including their shadow fades and private facade continuations. Every lane gets
a working warp. Unbuilt directions terminate in complete structural barriers.

Use the established grey metal flooring, black-backed horizontal walls, correct
pillar joins, and restrained indicator colors. Habitat glass should use readable
frames and highlights; it must not obscure the Pokémon or rely on true alpha
transparency. Keep the walking space visually distinct from the public aisle.

## Display behavior and PC

Select up to four distinct regular Safari-buffer entries when entering the normal
room. Keep that selection stable while walking around; do not reshuffle it on
camera movement. Match the species/form/shiny/custom appearance represented by
the existing Safari preview and acquisition flow. Legendary entries remain in
the existing cave system.

Empty buffer positions leave empty habitats/capsules. Do not populate decorative
Pokémon that imply unavailable purchases, change buffer capacity, or add Pokémon
to the saved collection to make the room look occupied.

Confine each habitat occupant to explicit legal tiles. Capsule occupants remain
within their visible housing. Check large and tall overworld sprites, foreground
glass priority, follower palettes, and object-slot limits before finalizing the
display count. Reserve up to four display slots without taking the player
follower's slot or enabling ordinary Safari encounters in the public aisle.

The PC is the management and acquisition interaction. Preserve prices, purchase
requirements, confirmation, delivery/full-party handling, cancellation, refunds,
and Dismiss All behavior. Update its copy from sightings/roaming/sign terminology
to the Pokémon currently housed in the lab. Keep its name and purpose distinct
from ordinary Pokémon storage and Adventure Setup.

Refresh display assignments after a purchase, dismissal, buffer compaction,
map reload, or other buffer change. Remove stale occupants and rebind indices
safely; a display must never show or refer to an entry that was removed or moved.
Do not attach catch battles to the normal containment displays.

## Opening containment accident

Use the tutorial map as a damaged version of the same architecture: visibly
broken habitat panels, a few safe glass fragments, warning indicators, and the
three existing starter Pokémon loose in reachable floor areas. Keep exits and
NPC staging recognizable between the two versions.

1. In Birch's Lab, replace the invasion report with a report that the containment
   glass broke and Pokémon got out. Birch asks the player to help secure them.
2. Preserve the existing journey through the Adventure Gate and into this node.
   Update linked guidance where needed so the whole story describes one incident.
3. On arrival from west, east, or south, Birch explains the accident and supplies
   the existing Poké Ball: “Try to catch one. I'll secure the others!”
4. Preserve the three starter choices and the current successful-catch condition.
   Players choose one partner; this does not become a requirement to catch all three.
5. After the catch, clear the remaining loose Pokémon consistently, place Birch
   at a safe nearby dialogue position, and explain that he secured the others.
   Return to the existing conversation in his lab and advance the same intro state.
6. Subsequent normal visits show repaired glass and normal PC/display operation.
   The tutorial does not grant extra Safari-buffer entries or repeat on reload.

Keep PC management unavailable during the accident, with brief emergency text
if interacted with. Preserve the tutorial's exit restriction across all three
lanes of each relevant passage. Fix the existing south-exit pushback movement,
which currently calls `walk_left` despite being named `PlayerUp`.

Replace the post-catch unconditional placement of Birch two tiles above the
player with a verified reachable dialogue position; enclosures could otherwise
put him inside machinery or glass. Preserve retry behavior, Poké Ball recovery,
and progression if the player cancels, flees, or fails an attempt.

## Assets, persistence, and source workflow

- Inspect complete assemblies in the Galactic HQ, Encounter Lab, Lysandre Labs,
  Facility/Rocket, and cave source maps before selecting containment pieces.
  Record actual donor map coordinates. Prototype a habitat and a capsule with
  real Pokémon sprites before arranging the full room.
- Build private tilesets/facades for this district. Copy the dark PC's composition
  into private resources; leave the Adventure Gate, research junction, and
  legendary-cave artwork unchanged.
- Use `MAP_TYPE_INDOOR` and `WEATHER_NONE` on both Safari hub variants. Preserve
  music, movement permissions, buffer mechanics, and progression.
- Replace Safari-specific outdoor updates in `src/rogue_hub.c` with named interior
  metatiles; exclude both variants from outdoor style changes.
- Keep original warp indices and relocate callers/returns together. Append the
  third-lane warps and update their dynamic routing. Preserve tutorial-map
  redirection and the existing north-space reservation.
- Keep both layouts and borders directly editable in Porymap. A generator reads
  them for previews/validation and generates only private assets/facades. Verify
  that normal/tutorial architecture stays aligned without overwriting hand edits.
- Keep display assignments transient. No save-format change or save deletion.
  Recover old outdoor save positions safely according to the current intro state;
  do not replay completed scenes or regenerate the selected partner on continue.
- Edit authoritative map JSON and `.pory` files. Update the True Legend unlock
  popup's outdoor “Mountains shift…” wording in quest JSON while retaining its
  reward and trigger. Regenerate affected artifacts and add a changelog entry.

## Implementation and validation order

1. Prove habitat/capsule assemblies, sprite containment, and layer/palette limits.
2. Build the normal room, PC, cave facade, workbenches, and three-wide passages.
   Render actual event sprites and inspect native 240 × 160 camera views.
3. Add buffer-backed display selection and refresh behavior while preserving the
   existing purchase system.
4. Build the aligned damaged tutorial variant and rewrite the connected opening
   dialogue, movements, completion placement, and exit gates.
5. Finish existing-save recovery, generated checks, documentation, and ROM build.

Check all eight west/east/south connection masks, cave locked/unlocked, workbench
hidden/visible, empty/partial/full buffers, and both map variants. Verify service
reachability, three-wide corridors, every warp lane, sealed exits, cave return,
teleport arrival, and stable selections across camera movement.

Focused runtime coverage should check display bounds and identity after buffer
mutation, absence of public-aisle spawns, tutorial arrival/restrictions from all
three directions, completion/retries, and safe save/reload behavior. Verify
purchase cancellation, insufficient currency, party/box capacity, and Dismiss All
against the existing acquisition rules.

Finish with asset verification, narrow relevant Rogue tests, an incremental ROM
build, and `git diff --check`. Manually inspect glass layering, large Pokémon,
follower movement, emergency-to-repaired continuity, PC use, the first catch,
and cave transitions. Explicitly report any emulator checks not performed.

Deliver both map variants, a rebuilt ROM, normal/damaged previews, and asset notes.
The legendary-cave interior, purchase balance, new services, and other districts
remain outside this rework.

## Implemented layout and assets

- Both 38 × 32 maps retain their identities. The central axis is x18; side walls
  are x6/x30. Side exits occupy y14–16 and the south exit x17–19. The private
  horizontal and vertical facade offsets are 12 and 15 respectively.
- Habitat frames start at (8,9) and (23,9); occupants wander randomly across the
  full 4×3 interiors x9–12/x24–27, y10–12, starting at (10,11)/(26,11).
  Horizontal range 2 includes a solid frame column, which collision excludes;
  vertical range 1 keeps movement within the three interior rows. The map objects
  use `MOVEMENT_TYPE_WANDER_AROUND`; display detection follows graphics slots
  0–3, independently of local object numbers changed by service edits. The shared
  generator checks both movement types and exact walkable 4×3 bounds against
  the authored map, including collision and elevation.
  Capsules start at (9,5)/(25,5), with occupants at (10,7)/(26,7).
  The tutorial instead places its three existing choices on the public floor,
  with broken front panels at y13 and rear-wall warning indicators.
- The Safari PC is at (14,8), accessible from (14,9). Workbenches at (9,20) and
  (27,20) sit below matching rear walls of the lower side bays. The cave opens
  at (18,5), with its upper tile at (18,4), preserving normal warp 6; teleport arrival remains warp 7 at (18,18).
- Original normal warps 0–7 and tutorial warps 0–5 keep their indices. Normal
  warps 8–10 and tutorial warps 6–8 cover the third exit lanes. Tutorial warp 9
  gathers the player at (18,13), facing Birch at (18,12), after the successful
  catch. This frames both habitats above the dialogue box. It does not regenerate
  the starter choices or repeat the Ball gift.
- Architecture is copied into private resources from the finished research
  junction, whose source assemblies are documented in `lab_junction.md`.
  Galactic HQ's complete specimen-machine assembly at (1,1), size 2 × 3,
  informed hardware proportions. Habitat/capsule frames are new indexed tile
  compositions, not untouched donor objects. Their frames and front plinths use
  covered metatiles, so players and followers in the aisle draw in front of them.
  Capsule backgrounds use a dedicated neutral palette.
- The grey rock bank copies the original Safari facade at (15,0), size 7 × 6.
  Its original metatile IDs are retained in `data/tilesets/safari_lab_sources.json`
  so regeneration does not read the converted map as its own donor. Cave-door
  pieces use Fallarbor metatiles 0x259/0x261. Only the private rock palettes are
  desaturated. The existing dark Adventure Console object is reused unchanged.
- Assets use 173 background tiles and all 13 permitted background palettes.
  Displays occupy follower graphics slots 0–3 (object palettes 6–9); the player
  follower, PC and workbenches retain separate palettes. There are seven normal
  map objects before the player/follower, and six on the tutorial map.

## Display and persistence details

Displays select distinct regular-buffer entries, preserving the same egg-species,
shiny and custom-color preview rules as the existing Safari acquisition flow.
For example, an ordinary Pikachu entry is represented by Pichu. Entries with
overworld sprites larger than 32 × 32 are omitted from glass displays; they remain
fully available at the PC. Empty positions remain empty.

Camera culling neither releases display assignments nor triggers wild-spawn
animations. Display objects use guidance scripts and solid collision rather than
catch battles. Closing the PC silently reloads the room to refresh selection after
purchase, cancellation, dismissal or compaction; prices and delivery/refund code
are unchanged. Normal roaming spawns remain disabled.

Existing outdoor object snapshots, or positions newly covered by solid terrain,
recover through the existing continue-warp mechanism. A catch in progress returns
to the appropriate tutorial arrival; post-catch progress returns to its gathering
point; completed introductions return to the normal teleport point. No serialized
fields or save version changed, and no save files were deleted.

## Verification and previews

Completed checks: all four `Safari lab:` runtime tests passed; generated assets
and all 64 layout states passed verification; the incremental debug ROM build
and `git diff --check` passed. The rebuilt `pokeemerald.gba` includes both room
variants and the regenerated quest/script data.

`python3 scripts/generate_safari_lab.py --check` verifies generated bytes, tile and
palette capacity, border fill, facade pixels, service reachability, enclosure
movement bounds and exit coverage across 64 combinations (both maps × eight
connection masks × cave upgrade × workbench visibility). It reads the edited map
binaries without overwriting them. `.pory` and quest outputs regenerate through
the normal incremental build.

The focused `Safari lab:` Rogue tests cover all connection/cave combinations,
added warp routing, empty/partial/full buffer displays, shiny/egg previews,
oversized-sprite filtering, camera removal/respawn, dismissal/compaction, follower
slot preservation, solid display collision and old-save recovery across intro
states and all three entry directions.

Previews in `build/safari_lab/` include `room.png`, `unlocked.png`, `empty.png`,
`west-only.png` and the matching `tutorial-` versions. These use actual event
sprites and illustrative buffer occupants. Each has 240 × 160 `*-native.png`
camera views and enlarged nearest-neighbor copies at the habitats, PC, crossing
and workbenches. These are composed tile previews, not emulator screenshots.

Manual checks still needed: the complete new-game capture sequence (including
flee/retry and Ball recovery), PC purchase/cancel/refund and full-party/box paths,
live sprite layering/follower movement, cave entry/return, and saving/continuing
inside the district. Automated reachability and runtime tests do not replace
these interactive checks.

Hallway threshold and centered-arrival updates are documented in
[hub_hallways.md](hub_hallways.md); its coordinates supersede earlier edge-warp
positions in this design history.
