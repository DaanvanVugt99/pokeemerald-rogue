# Underground hub hallway transitions

The Adventure Gate, Labs junction, Main Hall and both Safari variants use one convention:
the first shaded tile starts the fade; the second shaded tile carries a
three-wide directional warp strip. Decorative hall/facade tiles beyond it are
ordinary dark floor. The warp no longer depends on how far the map buffer extends.

All three source lanes route to the same destination hallway. On arrival,
`RogueHub_GetWarpArrivalPosition` uses the midpoint of that hallway's three warp
coordinates. The existing directional arrival animation walks inward from there.
Script-selected and continue-warp arrivals use the same rule. Interior doors,
the adventure portal return, cave return, teleport points and tutorial gathering
point keep their original arrival positions and indices.

Coordinates below are the centered warp tile, before the inward arrival step:

| District | West | East | North | South |
|---|---|---|---|---|
| Adventure Gate | (0,11) | (18,11) | — | (9,15) |
| Labs | (2,9) | (26,9) | (14,1) | (14,20) |
| Safari, normal and tutorial | (4,15) | (32,15) | — | (18,27) |
| Main Hall | (1,17) | (35,17) | (18,1) | (18,28) |

The original warp IDs and district connections remain stable. All lane tiles
retain their directional arrow behavior; the aisle-side neighboring tile is
walkable ordinary floor, allowing the arrival animation to leave the strip.

Birch's Labs guidance ends at these thresholds. The Adventure Gate's pre-catch
guards and the Safari tutorial's catch requirement cover all three lanes at the
new positions. Tutorial Birch stands just inside the corresponding Safari entry.

On continue, `RogueHub_RecoverHallwayPosition` detects saves in the old decorative
stretch beyond a moved threshold and brings them back to that hallway's center.
This uses the existing continue-warp mechanism, with no save-format change.

The older outdoor districts retain their two-wide exits and existing arrivals.
Widen them and adopt this convention when reskinning each district.

Validation: `Hub hallways:` focused Rogue tests check every incoming lane,
directional metatile behavior, safe inward steps, special/outdoor exclusions and
old hallway positions. The three asset generators verify their authored layouts,
closed connections, corridor seams and tutorial routes. Map binaries remain
Porymap-authored and are not overwritten by normal generation.

Both focused tests passed. Asset verification passed for all three rooms (8
Adventure Gate, 128 Labs and 64 Safari connection/service states). Interactive
emulator checks of the transition animation and tutorial scenes remain unverified.
