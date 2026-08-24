# Tower of Ashes Item Icons

The Treasure concept icons under `graphics/items/icons/treasures/` and their
matching palettes under `graphics/items/icon_palettes/treasures/` were adapted
from the public [Tower of Ashes Resource Pack](https://eeveeexpo.com/resources/1557/).

The resource page credits the Tower of Ashes team, including Brawligator and
PikachuMazzinga. It also notes that some item graphics were edited from free RTP
icons by The Infamous Bon Bon. The imported files retain their source working
names so individual provenance can be clarified without ambiguity.

The source archive does not include a conventional SPDX or text license. Keep
these credits with redistributed builds and confirm the applicable reuse terms
with the resource authors before a public Divergence release.

## Adaptation

- Selected source files were stored as 48x48 PNGs.
- Every selected file consisted entirely of uniform 2x2 pixel blocks, so it was
  reduced to its authored 24x24 logical resolution without resampling.
- Fully transparent source colors were merged into palette index 0.
- Visible colors and pixels were otherwise retained in the imported sprites.
- Eclipse Totem and Acid Rain Totem are Divergence palette variants of Sun
  Totem and Rain Totem. Their colors are derived from the existing Eclipse Orb
  and Acid Rock palettes; their pixel indices are unchanged from the bases.
- Each output uses no more than 15 total palette entries, including
  transparency, and has a matching 16-entry CRLF JASC palette for `gbagfx`.
