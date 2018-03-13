
#ifndef ESL_UNICODE_HPP
#define ESL_UNICODE_HPP

namespace esl {

// https://en.wikipedia.org/wiki/Unicode_block

inline constexpr char32_t no_code_point = 0xFFFFFFFF;

enum class unicode_plane {
	basic_multilingual = 0, // BMP, 0000..FFFF
	supplementary_multilingual = 1, // SMP, 10000..1FFFF
	supplementary_ideographic = 2, // SIP, 20000..2FFFF
	unassigned = 3,	// 30000..DFFFF
	supplementary_special_purpose = 14, // SSP, E0000..EFFFF
	supplementary_private_use_area_a = 15, // PUA-A, F0000..FFFFF
	supplementary_private_use_area_b = 16, // PUA-B, 100000..10FFFF
	no_plane,
};

enum class unicode_block {
	// 0 BMP
	basic_latin = 0, // 0000..007F
	latin_1_supplement, // 0080..00FF
	latin_extended_a, // 0100..017F
	latin_extended_b, // 0180..024F
	ipa_extensions, // 0250..02AF
	spacing_modifier_letters, // 02B0..02FF
	combining_diacritical_marks, // 0300..036F
	greek_and_coptic, // 0370..03FF
	cyrillic, // 0400..04FF
	cyrillic_supplement, // 0500..052F
	armenian, // 0530..058F
	hebrew, // 0590..05FF
	arabic, // 0600..06FF
	syriac, // 0700..074F
	arabic_supplement, // 0750..077F
	thaana, // 0780..07BF
	nko, // 07C0..07FF
	samaritan, // 0800..083F
	mandaic, // 0840..085F
	syriac_supplement, // 0860..086F
	arabic_extended_a, // 08A0..08FF
	devanagari, // 0900..097F
	bengali, // 0980..09FF
	gurmukhi, // 0A00..0A7F
	gujarati, // 0A80..0AFF
	oriya, // 0B00..0B7F
	tamil, // 0B80..0BFF
	telugu, // 0C00..0C7F
	kannada, // 0C80..0CFF
	malayalam, // 0D00..0D7F
	sinhala, // 0D80..0DFF
	thai, // 0E00..0E7F
	lao, // 0E80..0EFF
	tibetan, // 0F00..0FFF
	myanmar, // 1000..109F
	georgian, // 10A0..10FF
	hangul_jamo, // 1100..11FF
	ethiopic, // 1200..137F
	ethiopic_supplement, // 1380..139F
	cherokee, // 13A0..13FF
	unified_canadian_aboriginal_syllabics, // 1400..167F
	ogham, // 1680..169F
	runic, // 16A0..16FF
	tagalog, // 1700..171F
	hanunoo, // 1720..173F
	buhid, // 1740..175F
	tagbanwa, // 1760..177F
	khmer, // 1780..17FF
	mongolian, // 1800..18AF
	unified_canadian_aboriginal_syllabics_extended, // 18B0..18FF
	limbu, // 1900..194F
	tai_le, // 1950..197F
	new_tai_lue, // 1980..19DF
	khmer_symbols, // 19E0..19FF
	buginese, // 1A00..1A1F
	tai_tham, // 1A20..1AAF
	combining_diacritical_marks_extended, // 1AB0..1AFF
	balinese, // 1B00..1B7F
	sundanese, // 1B80..1BBF
	batak, // 1BC0..1BFF
	lepcha, // 1C00..1C4F
	ol_chiki, // 1C50..1C7F
	cyrillic_extended_c, // 1C80..1C8F
	sundanese_supplement, // 1CC0..1CCF
	vedic_extensions, // 1CD0..1CFF
	phonetic_extensions, // 1D00..1D7F
	phonetic_extensions_supplement, // 1D80..1DBF
	combining_diacritical_marks_supplement, // 1DC0..1DFF
	latin_extended_additional, // 1E00..1EFF
	greek_extended, // 1F00..1FFF
	general_punctuation, // 2000..206F
	superscripts_and_subscripts, // 2070..209F
	currency_symbols, // 20A0..20CF
	combining_diacritical_marks_for_symbols, // 20D0..20FF
	letterlike_symbols, // 2100..214F
	number_forms, // 2150..218F
	arrows, // 2190..21FF
	mathematical_operators, // 2200..22FF
	miscellaneous_technical, // 2300..23FF
	control_pictures, // 2400..243F
	optical_character_recognition, // 2440..245F
	enclosed_alphanumerics, // 2460..24FF
	box_drawing, // 2500..257F
	block_elements, // 2580..259F
	geometric_shapes, // 25A0..25FF
	miscellaneous_symbols, // 2600..26FF
	dingbats, // 2700..27BF
	miscellaneous_mathematical_symbols_a, // 27C0..27EF
	supplemental_arrows_a, // 27F0..27FF
	braille_patterns, // 2800..28FF
	supplemental_arrows_b, // 2900..297F
	miscellaneous_mathematical_symbols_b, // 2980..29FF
	supplemental_mathematical_operators, // 2A00..2AFF
	miscellaneous_symbols_and_arrows, // 2B00..2BFF
	glagolitic, // 2C00..2C5F
	latin_extended_c, // 2C60..2C7F
	coptic, // 2C80..2CFF
	georgian_supplement, // 2D00..2D2F
	tifinagh, // 2D30..2D7F
	ethiopic_extended, // 2D80..2DDF
	cyrillic_extended_a, // 2DE0..2DFF
	supplemental_punctuation, // 2E00..2E7F
	cjk_radicals_supplement, // 2E80..2EFF
	kangxi_radicals, // 2F00..2FDF
	ideographic_description_characters, // 2FF0..2FFF
	cjk_symbols_and_punctuation, // 3000..303F
	hiragana, // 3040..309F
	katakana, // 30A0..30FF
	bopomofo, // 3100..312F
	hangul_compatibility_jamo, // 3130..318F
	kanbun, // 3190..319F
	bopomofo_extended, // 31A0..31BF
	cjk_strokes, // 31C0..31EF
	katakana_phonetic_extensions, // 31F0..31FF
	enclosed_cjk_letters_and_months, // 3200..32FF
	cjk_compatibility, // 3300..33FF
	cjk_unified_ideographs_extension_a, // 3400..4DBF
	yijing_hexagram_symbols, // 4DC0..4DFF
	cjk_unified_ideographs, // 4E00..9FFF
	yi_syllables, // A000..A48F
	yi_radicals, // A490..A4CF
	lisu, // A4D0..A4FF
	vai, // A500..A63F
	cyrillic_extended_b, // A640..A69F
	bamum, // A6A0..A6FF
	modifier_tone_letters, // A700..A71F
	latin_extended_d, // A720..A7FF
	syloti_nagri, // A800..A82F
	common_indic_number_forms, // A830..A83F
	phags_pa, // A840..A87F
	saurashtra, // A880..A8DF
	devanagari_extended, // A8E0..A8FF
	kayah_li, // A900..A92F
	rejang, // A930..A95F
	hangul_jamo_extended_a, // A960..A97F
	javanese, // A980..A9DF
	myanmar_extended_b, // A9E0..A9FF
	cham, // AA00..AA5F
	myanmar_extended_a, // AA60..AA7F
	tai_viet, // AA80..AADF
	meetei_mayek_extensions, // AAE0..AAFF
	ethiopic_extended_a, // AB00..AB2F
	latin_extended_e, // AB30..AB6F
	cherokee_supplement, // AB70..ABBF
	meetei_mayek, // ABC0..ABFF
	hangul_syllables, // AC00..D7AF
	hangul_jamo_extended_b, // D7B0..D7FF
	high_surrogates, // D800..DB7F
	high_private_use_surrogates, // DB80..DBFF
	low_surrogates, // DC00..DFFF
	private_use_area, // E000..F8FF
	cjk_compatibility_ideographs, // F900..FAFF
	alphabetic_presentation_forms, // FB00..FB4F
	arabic_presentation_forms_a, // FB50..FDFF
	variation_selectors, // FE00..FE0F
	vertical_forms, // FE10..FE1F
	combining_half_marks, // FE20..FE2F
	cjk_compatibility_forms, // FE30..FE4F
	small_form_variants, // FE50..FE6F
	arabic_presentation_forms_b, // FE70..FEFF
	halfwidth_and_fullwidth_forms, // FF00..FFEF
	specials, // FFF0..FFFF
	// 1 SMP
	linear_b_syllabary, // 10000..1007F
	linear_b_ideograms, // 10080..100FF
	aegean_numbers, // 10100..1013F
	ancient_greek_numbers, // 10140..1018F
	ancient_symbols, // 10190..101CF
	phaistos_disc, // 101D0..101FF
	lycian, // 10280..1029F
	carian, // 102A0..102DF
	coptic_epact_numbers, // 102E0..102FF
	old_italic, // 10300..1032F
	gothic, // 10330..1034F
	old_permic, // 10350..1037F
	ugaritic, // 10380..1039F
	old_persian, // 103A0..103DF
	deseret, // 10400..1044F
	shavian, // 10450..1047F
	osmanya, // 10480..104AF
	osage, // 104B0..104FF
	elbasan, // 10500..1052F
	caucasian_albanian, // 10530..1056F
	linear_a, // 10600..1077F
	cypriot_syllabary, // 10800..1083F
	imperial_aramaic, // 10840..1085F
	palmyrene, // 10860..1087F
	nabataean, // 10880..108AF
	hatran, // 108E0..108FF
	phoenician, // 10900..1091F
	lydian, // 10920..1093F
	meroitic_hieroglyphs, // 10980..1099F
	meroitic_cursive, // 109A0..109FF
	kharoshthi, // 10A00..10A5F
	old_south_arabian, // 10A60..10A7F
	old_north_arabian, // 10A80..10A9F
	manichaean, // 10AC0..10AFF
	avestan, // 10B00..10B3F
	inscriptional_parthian, // 10B40..10B5F
	inscriptional_pahlavi, // 10B60..10B7F
	psalter_pahlavi, // 10B80..10BAF
	old_turkic, // 10C00..10C4F
	old_hungarian, // 10C80..10CFF
	rumi_numeral_symbols, // 10E60..10E7F
	brahmi, // 11000..1107F
	kaithi, // 11080..110CF
	sora_sompeng, // 110D0..110FF
	chakma, // 11100..1114F
	mahajani, // 11150..1117F
	sharada, // 11180..111DF
	sinhala_archaic_numbers, // 111E0..111FF
	khojki, // 11200..1124F
	multani, // 11280..112AF
	khudawadi, // 112B0..112FF
	grantha, // 11300..1137F
	newa, // 11400..1147F
	tirhuta, // 11480..114DF
	siddham, // 11580..115FF
	modi, // 11600..1165F
	mongolian_supplement, // 11660..1167F
	takri, // 11680..116CF
	ahom, // 11700..1173F
	warang_citi, // 118A0..118FF
	zanabazar_square, // 11A00..11A4F
	soyombo, // 11A50..11AAF
	pau_cin_hau, // 11AC0..11AFF
	bhaiksuki, // 11C00..11C6F
	marchen, // 11C70..11CBF
	masaram_gondi, // 11D00..11D5F
	cuneiform, // 12000..123FF
	cuneiform_numbers_and_punctuation, // 12400..1247F
	early_dynastic_cuneiform, // 12480..1254F
	egyptian_hieroglyphs, // 13000..1342F
	anatolian_hieroglyphs, // 14400..1467F
	bamum_supplement, // 16800..16A3F
	mro, // 16A40..16A6F
	bassa_vah, // 16AD0..16AFF
	pahawh_hmong, // 16B00..16B8F
	miao, // 16F00..16F9F
	ideographic_symbols_and_punctuation, // 16FE0..16FFF
	tangut, // 17000..187FF
	tangut_components, // 18800..18AFF
	kana_supplement, // 1B000..1B0FF
	kana_extended_a, // 1B100..1B12F
	nushu, // 1B170..1B2FF
	duployan, // 1BC00..1BC9F
	shorthand_format_controls, // 1BCA0..1BCAF
	byzantine_musical_symbols, // 1D000..1D0FF
	musical_symbols, // 1D100..1D1FF
	ancient_greek_musical_notation, // 1D200..1D24F
	tai_xuan_jing_symbols, // 1D300..1D35F
	counting_rod_numerals, // 1D360..1D37F
	mathematical_alphanumeric_symbols, // 1D400..1D7FF
	sutton_signwriting, // 1D800..1DAAF
	glagolitic_supplement, // 1E000..1E02F
	mende_kikakui, // 1E800..1E8DF
	adlam, // 1E900..1E95F
	arabic_mathematical_alphabetic_symbols, // 1EE00..1EEFF
	mahjong_tiles, // 1F000..1F02F
	domino_tiles, // 1F030..1F09F
	playing_cards, // 1F0A0..1F0FF
	enclosed_alphanumeric_supplement, // 1F100..1F1FF
	enclosed_ideographic_supplement, // 1F200..1F2FF
	miscellaneous_symbols_and_pictographs, // 1F300..1F5FF
	emoticons, // 1F600..1F64F
	ornamental_dingbats, // 1F650..1F67F
	transport_and_map_symbols, // 1F680..1F6FF
	alchemical_symbols, // 1F700..1F77F
	geometric_shapes_extended, // 1F780..1F7FF
	supplemental_arrows_c, // 1F800..1F8FF
	supplemental_symbols_and_pictographs, // 1F900..1F9FF
	// 2 SIP
	cjk_unified_ideographs_extension_b, // 20000..2A6DF
	cjk_unified_ideographs_extension_c, // 2A700..2B73F
	cjk_unified_ideographs_extension_d, // 2B740..2B81F
	cjk_unified_ideographs_extension_e, // 2B820..2CEAF
	cjk_unified_ideographs_extension_f, // 2CEB0..2EBEF
	cjk_compatibility_ideographs_supplement, // 2F800..2FA1F
	// 14 SSP
	tags, // E0000..E007F
	variation_selectors_supplement, // E0100..E01EF
	// 15 PUA-A
	supplementary_private_use_area_a, // F0000..FFFFF
	// 16 PUA-B
	supplementary_private_use_area_b, // 100000..10FFFF
	// no block
	no_block,
};

inline constexpr char32_t unicode_block_min_code_points_[] = {
	0x0000, 0x0080, 0x0100, 0x0180, 0x0250, 0x02B0, 0x0300, 0x0370, 0x0400, 0x0500, 0x0530, 0x0590, 0x0600, 0x0700, 0x0750, 0x0780,
	0x07C0, 0x0800, 0x0840, 0x0860, 0x08A0, 0x0900, 0x0980, 0x0A00, 0x0A80, 0x0B00, 0x0B80, 0x0C00, 0x0C80, 0x0D00, 0x0D80, 0x0E00,
	0x0E80, 0x0F00, 0x1000, 0x10A0, 0x1100, 0x1200, 0x1380, 0x13A0, 0x1400, 0x1680, 0x16A0, 0x1700, 0x1720, 0x1740, 0x1760, 0x1780,
	0x1800, 0x18B0, 0x1900, 0x1950, 0x1980, 0x19E0, 0x1A00, 0x1A20, 0x1AB0, 0x1B00, 0x1B80, 0x1BC0, 0x1C00, 0x1C50, 0x1C80, 0x1CC0,
	0x1CD0, 0x1D00, 0x1D80, 0x1DC0, 0x1E00, 0x1F00, 0x2000, 0x2070, 0x20A0, 0x20D0, 0x2100, 0x2150, 0x2190, 0x2200, 0x2300, 0x2400,
	0x2440, 0x2460, 0x2500, 0x2580, 0x25A0, 0x2600, 0x2700, 0x27C0, 0x27F0, 0x2800, 0x2900, 0x2980, 0x2A00, 0x2B00, 0x2C00, 0x2C60,
	0x2C80, 0x2D00, 0x2D30, 0x2D80, 0x2DE0, 0x2E00, 0x2E80, 0x2F00, 0x2FF0, 0x3000, 0x3040, 0x30A0, 0x3100, 0x3130, 0x3190, 0x31A0,
	0x31C0, 0x31F0, 0x3200, 0x3300, 0x3400, 0x4DC0, 0x4E00, 0xA000, 0xA490, 0xA4D0, 0xA500, 0xA640, 0xA6A0, 0xA700, 0xA720, 0xA800,
	0xA830, 0xA840, 0xA880, 0xA8E0, 0xA900, 0xA930, 0xA960, 0xA980, 0xA9E0, 0xAA00, 0xAA60, 0xAA80, 0xAAE0, 0xAB00, 0xAB30, 0xAB70,
	0xABC0, 0xAC00, 0xD7B0, 0xD800, 0xDB80, 0xDC00, 0xE000, 0xF900, 0xFB00, 0xFB50, 0xFE00, 0xFE10, 0xFE20, 0xFE30, 0xFE50, 0xFE70,
	0xFF00, 0xFFF0, 0x10000, 0x10080, 0x10100, 0x10140, 0x10190, 0x101D0, 0x10280, 0x102A0, 0x102E0, 0x10300, 0x10330, 0x10350, 0x10380, 0x103A0,
	0x10400, 0x10450, 0x10480, 0x104B0, 0x10500, 0x10530, 0x10600, 0x10800, 0x10840, 0x10860, 0x10880, 0x108E0, 0x10900, 0x10920, 0x10980, 0x109A0,
	0x10A00, 0x10A60, 0x10A80, 0x10AC0, 0x10B00, 0x10B40, 0x10B60, 0x10B80, 0x10C00, 0x10C80, 0x10E60, 0x11000, 0x11080, 0x110D0, 0x11100, 0x11150,
	0x11180, 0x111E0, 0x11200, 0x11280, 0x112B0, 0x11300, 0x11400, 0x11480, 0x11580, 0x11600, 0x11660, 0x11680, 0x11700, 0x118A0, 0x11A00, 0x11A50,
	0x11AC0, 0x11C00, 0x11C70, 0x11D00, 0x12000, 0x12400, 0x12480, 0x13000, 0x14400, 0x16800, 0x16A40, 0x16AD0, 0x16B00, 0x16F00, 0x16FE0, 0x17000,
	0x18800, 0x1B000, 0x1B100, 0x1B170, 0x1BC00, 0x1BCA0, 0x1D000, 0x1D100, 0x1D200, 0x1D300, 0x1D360, 0x1D400, 0x1D800, 0x1E000, 0x1E800, 0x1E900,
	0x1EE00, 0x1F000, 0x1F030, 0x1F0A0, 0x1F100, 0x1F200, 0x1F300, 0x1F600, 0x1F650, 0x1F680, 0x1F700, 0x1F780, 0x1F800, 0x1F900, 0x20000, 0x2A700,
	0x2B740, 0x2B820, 0x2CEB0, 0x2F800, 0xE0000, 0xE0100, 0xF0000, 0x100000, no_code_point,
};

inline constexpr char32_t unicode_block_max_code_points_[] = {
	0x007F, 0x00FF, 0x017F, 0x024F, 0x02AF, 0x02FF, 0x036F, 0x03FF, 0x04FF, 0x052F, 0x058F, 0x05FF, 0x06FF, 0x074F, 0x077F, 0x07BF,
	0x07FF, 0x083F, 0x085F, 0x086F, 0x08FF, 0x097F, 0x09FF, 0x0A7F, 0x0AFF, 0x0B7F, 0x0BFF, 0x0C7F, 0x0CFF, 0x0D7F, 0x0DFF, 0x0E7F,
	0x0EFF, 0x0FFF, 0x109F, 0x10FF, 0x11FF, 0x137F, 0x139F, 0x13FF, 0x167F, 0x169F, 0x16FF, 0x171F, 0x173F, 0x175F, 0x177F, 0x17FF,
	0x18AF, 0x18FF, 0x194F, 0x197F, 0x19DF, 0x19FF, 0x1A1F, 0x1AAF, 0x1AFF, 0x1B7F, 0x1BBF, 0x1BFF, 0x1C4F, 0x1C7F, 0x1C8F, 0x1CCF,
	0x1CFF, 0x1D7F, 0x1DBF, 0x1DFF, 0x1EFF, 0x1FFF, 0x206F, 0x209F, 0x20CF, 0x20FF, 0x214F, 0x218F, 0x21FF, 0x22FF, 0x23FF, 0x243F,
	0x245F, 0x24FF, 0x257F, 0x259F, 0x25FF, 0x26FF, 0x27BF, 0x27EF, 0x27FF, 0x28FF, 0x297F, 0x29FF, 0x2AFF, 0x2BFF, 0x2C5F, 0x2C7F,
	0x2CFF, 0x2D2F, 0x2D7F, 0x2DDF, 0x2DFF, 0x2E7F, 0x2EFF, 0x2FDF, 0x2FFF, 0x303F, 0x309F, 0x30FF, 0x312F, 0x318F, 0x319F, 0x31BF,
	0x31EF, 0x31FF, 0x32FF, 0x33FF, 0x4DBF, 0x4DFF, 0x9FFF, 0xA48F, 0xA4CF, 0xA4FF, 0xA63F, 0xA69F, 0xA6FF, 0xA71F, 0xA7FF, 0xA82F,
	0xA83F, 0xA87F, 0xA8DF, 0xA8FF, 0xA92F, 0xA95F, 0xA97F, 0xA9DF, 0xA9FF, 0xAA5F, 0xAA7F, 0xAADF, 0xAAFF, 0xAB2F, 0xAB6F, 0xABBF,
	0xABFF, 0xD7AF, 0xD7FF, 0xDB7F, 0xDBFF, 0xDFFF, 0xF8FF, 0xFAFF, 0xFB4F, 0xFDFF, 0xFE0F, 0xFE1F, 0xFE2F, 0xFE4F, 0xFE6F, 0xFEFF,
	0xFFEF, 0xFFFF, 0x1007F, 0x100FF, 0x1013F, 0x1018F, 0x101CF, 0x101FF, 0x1029F, 0x102DF, 0x102FF, 0x1032F, 0x1034F, 0x1037F, 0x1039F, 0x103DF,
	0x1044F, 0x1047F, 0x104AF, 0x104FF, 0x1052F, 0x1056F, 0x1077F, 0x1083F, 0x1085F, 0x1087F, 0x108AF, 0x108FF, 0x1091F, 0x1093F, 0x1099F, 0x109FF,
	0x10A5F, 0x10A7F, 0x10A9F, 0x10AFF, 0x10B3F, 0x10B5F, 0x10B7F, 0x10BAF, 0x10C4F, 0x10CFF, 0x10E7F, 0x1107F, 0x110CF, 0x110FF, 0x1114F, 0x1117F,
	0x111DF, 0x111FF, 0x1124F, 0x112AF, 0x112FF, 0x1137F, 0x1147F, 0x114DF, 0x115FF, 0x1165F, 0x1167F, 0x116CF, 0x1173F, 0x118FF, 0x11A4F, 0x11AAF,
	0x11AFF, 0x11C6F, 0x11CBF, 0x11D5F, 0x123FF, 0x1247F, 0x1254F, 0x1342F, 0x1467F, 0x16A3F, 0x16A6F, 0x16AFF, 0x16B8F, 0x16F9F, 0x16FFF, 0x187FF,
	0x18AFF, 0x1B0FF, 0x1B12F, 0x1B2FF, 0x1BC9F, 0x1BCAF, 0x1D0FF, 0x1D1FF, 0x1D24F, 0x1D35F, 0x1D37F, 0x1D7FF, 0x1DAAF, 0x1E02F, 0x1E8DF, 0x1E95F,
	0x1EEFF, 0x1F02F, 0x1F09F, 0x1F0FF, 0x1F1FF, 0x1F2FF, 0x1F5FF, 0x1F64F, 0x1F67F, 0x1F6FF, 0x1F77F, 0x1F7FF, 0x1F8FF, 0x1F9FF, 0x2A6DF, 0x2B73F,
	0x2B81F, 0x2CEAF, 0x2EBEF, 0x2FA1F, 0xE007F, 0xE01EF, 0xFFFFF, 0x10FFFF, no_code_point,
};

// plane_at
// code point to plane
inline constexpr unicode_plane plane_at(char32_t ch) noexcept {
	if (ch >= 0x30000 && ch <= 0xDFFFF) {
		return unicode_plane::unassigned;
	} else if (ch > 0x10FFFF) {
		return unicode_plane::no_plane;
	}
	return static_cast<unicode_plane>(ch >> 16);
}

// block_at
// code point to block
inline constexpr unicode_block block_at(char32_t ch) noexcept {
	const char32_t* first = unicode_block_max_code_points_;
	std::size_t count = std::size(unicode_block_max_code_points_);
	const char32_t* it = nullptr;
	while (count > 0) {
		std::size_t step = count / 2;
		it = first + step;
		if (*it < ch) {
			first = it + 1;
			count -= step + 1;
		} else {
			count = step;
		}
	}
	auto idx = first - unicode_block_max_code_points_;
	if (ch < unicode_block_min_code_points_[idx]) {
		return unicode_block::no_block;
	}
	return static_cast<unicode_block>(idx);
}

inline constexpr char32_t min_code_point(unicode_plane plane) noexcept {
	return plane == unicode_plane::no_plane ? no_code_point : (static_cast<char32_t>(plane) << 16);
}

inline constexpr char32_t max_code_point(unicode_plane plane) noexcept {
	return plane == unicode_plane::unassigned ? 0xDFFFF : (min_code_point(plane) | 0xFFFF);
}

inline constexpr char32_t min_code_point(unicode_block blk) noexcept {
	return unicode_block_min_code_points_[static_cast<int>(blk)];
}

inline constexpr char32_t max_code_point(unicode_block blk) noexcept {
	return unicode_block_max_code_points_[static_cast<int>(blk)];
}

// plane_at
// block to plane
inline constexpr unicode_plane plane_at(unicode_block blk) noexcept {
	return plane_at(min_code_point(blk));
}

} // namespace esl

#endif //ESL_UNICODE_HPP

