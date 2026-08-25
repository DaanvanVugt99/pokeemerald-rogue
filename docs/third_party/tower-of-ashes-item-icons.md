# Tower of Ashes Item Icons

The Tower of Ashes entries in the Treasure icon catalog under
`graphics/items/icons/treasures/` and their matching palettes under
`graphics/items/icon_palettes/treasures/` were adapted from the public
[Tower of Ashes Resource Pack](https://eeveeexpo.com/resources/1557/).

The resource page credits the Tower of Ashes team, including Brawligator and
PikachuMazzinga. It also notes that some item graphics were edited from free RTP
icons by The Infamous Bon Bon. The imported files retain their source working
names so individual provenance can be clarified without ambiguity.

The source archive does not include a conventional SPDX or text license. Keep
these credits with redistributed builds and confirm the applicable reuse terms
with the resource authors before a public Divergence release.

## Adaptation

- Selected 48x48 source files that consisted entirely of uniform 2x2 pixel
  blocks were reduced to their authored 24x24 logical resolution without
  resampling.
- Exterior-connected white matte pixels were made transparent while enclosed
  white highlights and details were retained.
- Fully transparent source colors were merged into palette index 0.
- Visible colors and pixels were otherwise retained, except where a sprite
  exceeded the GBA limit and required a nondithered palette reduction.
- Eclipse Totem and Acid Rain Totem are Divergence palette variants of Sun
  Totem and Rain Totem. Their colors are derived from the existing Eclipse Orb
  and Acid Rock palettes; their pixel indices are unchanged from the bases.
- Each output uses no more than 15 visible colors plus transparency at index 0
  and has a matching 16-entry CRLF JASC palette for `gbagfx`.
