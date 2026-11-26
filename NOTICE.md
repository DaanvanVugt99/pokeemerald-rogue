# Third-party notices

Divergence builds on pret's pokeemerald, RHH's pokeemerald-expansion,
and Pokabbie's Pokemon Emerald Rogue.

## Additional Treasure Item Icons

The NocTurn and How You Survive entries in the Treasure icon catalog under
`graphics/items/icons/treasures/` and their matching palettes under
`graphics/items/icon_palettes/treasures/` were selected from two public
Pokémon Essentials resource packs.

### NocTurn's Megapack

Source: [NocTurn's Megapack](https://eeveeexpo.com/resources/655/)

The pack was published by NocTurn. Its included Nexus credit file attributes
all item icons to Spring. Preserve both credits when redistributing derived
sprites.

Five selected images used opaque white backgrounds and high-color antialiased
edges. The port removed only the boundary-connected light background, retained
the enclosed artwork, and reduced the remaining colors without dithering.
`Item692.png` was resized from 48x48 to the game's native 24x24 item-icon
canvas; the other selected sprites were already 24x24.

### How You Survive a Pokémon Journey?

Source: [How You Survive a Pokémon Journey?](https://eeveeexpo.com/resources/1001/)

The resource pack was published by Caruban. Its included credit file contains
an item-icon section covering the pack's custom survival resources; preserve
the pack attribution when redistributing them.

`POKEFOODCAN.png` was resized from 48x48 to the native 24x24 canvas. The other
selected sprites were already native-sized or exact 2x enlargements that were
losslessly collapsed during selection.

### Shared adaptation

- Transparent pixels use palette index 0.
- Each output uses no more than 15 visible colors plus transparency.
- Every icon has a matching 16-entry CRLF JASC palette for `gbagfx`.
- Source working names remain represented in the filenames and catalog.

Neither pack includes a conventional SPDX or text license covering every
selected sprite. Confirm the applicable reuse terms with the resource authors
before a public Divergence release.

## Adventure Paths Astral Geology

The restrained cyan and violet mineral seams embedded in the Adventure Paths
bedrock formations are adapted from J-Treecko's public Gen 3 crystal sprites:

https://www.reddit.com/r/PokemonRMXP/comments/ockkrc/free_crystals_you_dont_hear_that_often/

J-Treecko published the sprites for others to use with credit and explicitly
permitted recoloring. The Divergence versions are reduced to a few recolored
pixels inside larger repository-derived cave formations. The full clear-blue
surface clusters are a separate project-local 16x16 redraw made specifically
for the Adventure Paths palette and metatile footprint.

The cardinal island lips, corner cases, and south-facing cobbled underside are
adapted from Buch's CC0 tiles in *The Field of the Floating Islands*:

https://opengameart.org/content/the-field-of-the-floating-islands

The source silhouettes and pixel clusters are recolored into the Adventure
Paths palette and composited over the map's own astral floor texture. The large
2x3 bedrock formations adapt existing `cave` tiles, while the late-game terrace
faces adapt existing `GeneralHub` mountain metatiles. The smaller stone and
mineral outcrops reuse the original map's `GeneralHub` rock upper layer at its
authored rock anchors, recolored and composited over the astral floor rather
than copied with its old grass background. Existing upstream attribution for
those repository assets remains intact.

## Land of Pixels laboratory tiles

Selected decorations in the shared hub tileset are adapted from **Laboratory
Tileset PixelArt 16px** by **marceles**.

- Source: <https://opengameart.org/content/laboratory-tileset-pixelart-16px>
- Author page: <https://marceles.itch.io/>
- License: [Creative Commons Attribution 4.0 International](https://creativecommons.org/licenses/by/4.0/)
- Vendored source: `graphics/tilesets/third_party/land_of_pixels_laboratory/tiles_stuff_16px.png`

The imported status consoles, displays, storage, wall controls, specimen tank,
server cabinets, analysis monitors, equipment benches, consoles, and floor
controls are recolored into the existing shared-hub palette banks and split into
GBA 8x8 4bpp tiles. Their source-cell coordinates and generated metatile ranges
are recorded by `scripts/lab_decorations.py` and
`data/tilesets/hub_sources.json`.

## Random Pokemon Nickname Data

The random nickname pool in
`src/data/rogue/pokemon_nicknames.txt` contains 2,500 manually reviewed
selections derived from the sources below. Only the curated nickname list is
distributed with the project; source archives and curation tooling are not.

The snapshot was curated on 2026-07-23 and contains:

- 1,700 common words selected semantically with Princeton WordNet 3.0 and
  checked for standard American English spelling and commonness with
  ESDB/SCOWL.
- 500 established human names from the United States Social Security
  Administration data: 125 familiar and 125 distinctive names for each sex.
- 300 proper names from Wikidata: 140 mythological names, 80 astronomical
  names, and 80 mineral or gemstone names.

All entries were normalized using the game's importer before counting. Entries
must contain only ASCII letters, digits, or spaces; be 3-10 characters long;
and be unique after normalization. The review excludes Pokemon species names,
abbreviations, function words, awkward inflections, explicit or offensive
terms, diagnoses, brands, and unusably generic words. The source headings are
organizational only: every nickname remains equally likely at runtime.

### Source snapshots

#### Princeton WordNet 3.0

- Source: https://wordnetcode.princeton.edu/3.0/
- Archive: `WNdb-3.0.tar.gz`
- SHA-256:
  `658b1ba191f5f98c2e9bae3e25c186013158f30ef779f191d2a44e5d25046dc8`

Required notice:

> This software and database is being provided to you, the LICENSEE, by
> Princeton University under the following license. By obtaining, using and/or
> copying this software and database, you agree that you have read, understood,
> and will comply with these terms and conditions.:
>
> Permission to use, copy, modify and distribute this software and database and
> its documentation for any purpose and without fee or royalty is hereby
> granted, provided that you agree to comply with the following copyright
> notice and statements, including the disclaimer, and that the same appear on
> ALL copies of the software, database and documentation, including
> modifications that you make for internal use or for distribution.
>
> WordNet 3.0 Copyright 2006 by Princeton University. All rights reserved.
>
> THIS SOFTWARE AND DATABASE IS PROVIDED "AS IS" AND PRINCETON UNIVERSITY MAKES
> NO REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED. BY WAY OF EXAMPLE, BUT
> NOT LIMITATION, PRINCETON UNIVERSITY MAKES NO REPRESENTATIONS OR WARRANTIES
> OF MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR PURPOSE OR THAT THE USE OF
> THE LICENSED SOFTWARE, DATABASE OR DOCUMENTATION WILL NOT INFRINGE ANY THIRD
> PARTY PATENTS, COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS.
>
> The name of Princeton University or Princeton may not be used in advertising
> or publicity pertaining to distribution of the software and/or database.
> Title to copyright in this software, database and any associated
> documentation shall at all times remain with Princeton University and
> LICENSEE agrees to preserve same.

#### ESDB/SCOWL

- Source: https://github.com/en-wl/wordlist
- Commit: `1e5b7d3a72f47a71da5d28686c1dd4b397178485`
- Selected metadata SHA-256:
  `81a4890727ced6b88141f76813342af765f85a2300d85f538ebf2ecd5cdce5bd`

Copyright 2000-2026 by Kevin Atkinson.

Permission to use, copy, modify, distribute, and sell any part of the English
Speller Database (ESDB, previously known as SCOWLv2), or word lists created
from it, is hereby granted without fee, provided that the above copyright
notice appears in all copies and that both the above copyright notice and this
notice appear in supporting documentation. Kevin Atkinson makes no
representations about the suitability of this database for any purpose. It is
provided "as is" without express or implied warranty.

#### SSA baby names through 2025

- Authoritative source:
  https://catalog.data.gov/dataset/baby-names-from-social-security-card-applications-national-data
- Pinned data mirror: https://github.com/mpatnott/name-generator
- Mirror commit: `d2f905c8ed6462d8807af7c3a9196461790114d1`
- `yob2025.txt` SHA-256:
  `711d65b5a0ab84596b62dc47a942ac2da3bedb1cb32513ff41039ca41d304196`
- License: [Creative Commons CC0 1.0](https://creativecommons.org/publicdomain/zero/1.0/)

The mirror contains the SSA national annual files from 1880 through 2025. The
selection uses each name's highest annual count to distinguish familiar from
distinctive established names.

#### Wikidata

- Source and licensing: https://www.wikidata.org/wiki/Wikidata:Licensing
- Query endpoint: https://query.wikidata.org/
- Retrieved: 2026-07-23
- Combined query-snapshot SHA-256:
  `940459a27d1b96c339e2410875cc530f8a0478be0a1b09d060dc4db0980b77f7`
- License: [Creative Commons CC0 1.0](https://creativecommons.org/publicdomain/zero/1.0/)

The snapshots queried English labels for mythological deities (`Q178885`) and
creatures (`Q2239243`), constellations (`Q8928`), natural satellites (`Q2537`),
minerals and mineral species (`Q7946` and `Q12089225`), and gemstones
(`Q83437`). The curated pool uses only labels that passed the same normalization
and review as the other source groups.

## Pokemon Showdown

Some expanded canonical ability descriptions are adapted from Pokemon Showdown's
`data/text/abilities.ts`.

Source: https://github.com/smogon/pokemon-showdown

### License

The MIT License (MIT)

Copyright (c) 2011-2026 Guangcong Luo and other contributors http://pokemonshowdown.com/

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

## Tower of Ashes Item Icons

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

### Adaptation

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
