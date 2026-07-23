# Random Pokemon Nickname Data

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

## Source snapshots

### Princeton WordNet 3.0

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

### ESDB/SCOWL

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

### SSA baby names through 2025

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

### Wikidata

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
