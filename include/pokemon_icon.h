#ifndef GUARD_POKEMON_ICON_H
#define GUARD_POKEMON_ICON_H

extern const struct SpritePalette gMonIconPaletteTable[];

const u32 *GetMonIconCompressedFrame(u16 species, u32 personality, u8 gender, u8 frame);
// The returned scratch buffer must be consumed before another decompression.
const u8 *DecompressMonIconFrame(u16 species, u32 personality, u8 gender, u8 frame);
const u16 *GetColoredMonIconPalette(u16 species, u32 personality, u8 gender, bool8 shiny, u32 otId, u16 *tintedPalette);
void TryLoadAllMonIconPalettesAtOffset(u16 offset);
u8 GetValidMonIconPalIndex(u16 species);
const u16 *GetValidMonIconPalettePtr(u16 speciesId);
u16 GetIconSpecies(u16 species, u32 personality);
u16 GetUnownLetterByPersonality(u32 personality);
u16 GetIconSpeciesNoPersonality(u16 speciesId);
const u16* GetValidMonIconPalettePtr(u16 species);
void LoadMonIconPalettes(void);
void LoadMonIconPalette(u16 species);
void LoadMonIconPaletteCustomOffset(u16 species, u16 paletteOffset);
void FreeMonIconPalettes(void);
u8 CreateMonIconNoPersonality(u16 species, void (*callback)(struct Sprite *), s16 x, s16 y, u8 subpriority);
u8 CreateMonIconCustomPaletteOffset(u16 species, void (*callback)(struct Sprite *), s16 x, s16 y, u8 subpriority, u16 paletteOffset);
u8 CreateMissingMonIcon(void (*callback)(struct Sprite *), s16 x, s16 y, u8 subpriority, u16 paletteOffset);
void FreeMonIconPalette(u16 species);
void FreeMonIconPaletteCustomOffset(u16 species, u16 paletteOffset);
void FreeAndDestroyMonIconSprite(struct Sprite *sprite);
u8 CreateMonIcon(u16 species, void (*callback)(struct Sprite *), s16 x, s16 y, u8 subpriority, u32 personality, u8 gender);
u8 CreatePartyMonIcon(u16 species, s16 x, s16 y, u32 personality, u8 gender);
u8 UpdateMonIconFrame(struct Sprite *sprite);
void LoadMonIconPalette(u16 species);
void LoadMonIconPaletteGender(u16 species, u8 gender);
void SpriteCB_MonIcon(struct Sprite *sprite);
void SetPartyHPBarSprite(struct Sprite *sprite, u8 animNum);
u8 GetMonIconPaletteIndexFromSpecies(u16 species);
u8 GetMonIconPaletteFromSpecies(u16 species);

#endif // GUARD_POKEMON_ICON_H
