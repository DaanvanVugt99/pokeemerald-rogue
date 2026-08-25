# Additional Treasure Item Icons

The NocTurn and How You Survive entries in the Treasure icon catalog under
`graphics/items/icons/treasures/` and their matching palettes under
`graphics/items/icon_palettes/treasures/` were selected from two public
Pokémon Essentials resource packs.

## NocTurn's Megapack

Source: [NocTurn's Megapack](https://eeveeexpo.com/resources/655/)

The pack was published by NocTurn. Its included Nexus credit file attributes
all item icons to Spring. Preserve both credits when redistributing derived
sprites.

Five selected images used opaque white backgrounds and high-color antialiased
edges. The port removed only the boundary-connected light background, retained
the enclosed artwork, and reduced the remaining colors without dithering.
`Item692.png` was resized from 48x48 to the game's native 24x24 item-icon
canvas; the other selected sprites were already 24x24.

## How You Survive a Pokémon Journey?

Source: [How You Survive a Pokémon Journey?](https://eeveeexpo.com/resources/1001/)

The resource pack was published by Caruban. Its included credit file contains
an item-icon section covering the pack's custom survival resources; preserve
the pack attribution when redistributing them.

`POKEFOODCAN.png` was resized from 48x48 to the native 24x24 canvas. The other
selected sprites were already native-sized or exact 2x enlargements that were
losslessly collapsed during selection.

## Shared adaptation

- Transparent pixels use palette index 0.
- Each output uses no more than 15 visible colors plus transparency.
- Every icon has a matching 16-entry CRLF JASC palette for `gbagfx`.
- Source working names remain represented in the filenames and catalog.

Neither pack includes a conventional SPDX or text license covering every
selected sprite. Confirm the applicable reuse terms with the resource authors
before a public Divergence release.
