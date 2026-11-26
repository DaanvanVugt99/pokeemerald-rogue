# Market food hall

The Market uses a 37 × 31 authored layout and the shared underground tiles.
Crafting and the recipe helper are in the northwest, the Bank in the northeast,
the Treat Shop in the southwest, and the kitchen and serving table in the
southeast. Stepped outer walls open onto a slate crossing with a brass bowl
inlay. The kitchen has a continuous brick backsplash, a checkerboard work floor, and
a drain below the prep counter. Brass thresholds mark the change from slate to
kitchen tile and from kitchen tile to the oak dining bay. The range includes
its original hood, burners, and base. Custom Market metatiles occupy
unused slots in the shared hub catalogue. Complete sink, cabinet, range,
prep counter, round table, chair and tableware assemblies come from the
original Restaurant tileset. The importer keeps their outlines and joins,
rebases them onto the Market surfaces and remaps them to a shared palette. The workbench sits west of the
south passage. The Bank, Treat Shop
and Bakery retain their existing unlocks. The shop's stock upgrade uses the same
counter and inventory logic.

The four hallway warp centers are north (18,1), east (34,15), south (18,29)
and west (2,15). Indices 0–7 retain the original directions, 10–13 add the third
lanes, and 9 is the teleport and outdoor-save recovery point at (16,18).
Index 8 is reserved and sealed. The old Restaurant map returns to the Market
and loading a save there redirects to (23,21), beside the kitchen.

Both chefs take orders from the south. Their movement scripts approach opposite
sides of the serving aisle and share the same seat at (28,24), with the previous
step reserved for the follower. The pie object spawns for the serving scene and
is removed before the player returns to the counter.

Run `python3 scripts/generate_market.py` after editing the layout's corridor
edges. It generates only the private facade map binaries and an ignored preview.
Use `python3 scripts/generate_market.py --check` to verify the authored room,
runtime closures, service and chef approaches, facades, and object count over
192 connection and service states. The preview includes the full room and a 240 × 160
kitchen view with both chefs and a sample player position.

## Validation record

The 192-state asset check and incremental debug ROM build passed. In mGBA
0.10.5, disposable save fixtures exercised recovery from both legacy locations,
the Market–Labs round trip, the Bank offer, crafting one Normal Pokéblock for
five Lum Berries, ordinary and Shiny Pie effects with a follower, and rejection
or cancellation before payment. The pie foreground and immediate shiny follower
refresh were adjusted after the serving-scene pass and rebuilt. Remaining
interactive checks include those final visuals, the follower-off serving scene,
Treat Shop purchases in both stock states, an outdoor neighbor transition, and
an ordinary save/reload made inside the current hall.
