//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Character.h"
#include "Math.h"
#include "StringBuilder.h"
#include "elastos/utility/logging/Slogger.h"

DEFINE_CONVERSION_FOR(Elastos::Core::Character::UnicodeBlock, IInterface)


namespace Elastos {
namespace Core {

const UInt32 Character::MIN_HIGH_SURROGATE = 0xD800;
const UInt32 Character::MAX_HIGH_SURROGATE = 0xDBFF;
const UInt32 Character::MIN_LOW_SURROGATE = 0xDC00;
const UInt32 Character::MAX_LOW_SURROGATE = 0xDFFF;
const UInt32 Character::MIN_SURROGATE = 0xD800;
const UInt32 Character::MAX_SURROGATE = 0xDFFF;
const UInt32 Character::MIN_SUPPLEMENTARY_CODE_POINT = 0x10000;
const Int32 Character::MIN_RADIX = 2;
const Int32 Character::MAX_RADIX = 36;
const UInt32 Character::MIN_CODE_POINT = 0x000000;
const UInt32 Character::MAX_CODE_POINT = 0x10FFFF;
const Int32 Character::SIZE = 32;

const Byte Character::UNASSIGNED = 0;
const Byte Character::UPPERCASE_LETTER = 1;
const Byte Character::LOWERCASE_LETTER = 2;
const Byte Character::TITLECASE_LETTER = 3;
const Byte Character::MODIFIER_LETTER = 4;
const Byte Character::OTHER_LETTER = 5;
const Byte Character::NON_SPACING_MARK = 6;
const Byte Character::ENCLOSING_MARK = 7;
const Byte Character::COMBINING_SPACING_MARK = 8;
const Byte Character::DECIMAL_DIGIT_NUMBER = 9;
const Byte Character::LETTER_NUMBER = 10;
const Byte Character::OTHER_NUMBER = 11;
const Byte Character::SPACE_SEPARATOR;
const Byte Character::LINE_SEPARATOR;
const Byte Character::PARAGRAPH_SEPARATOR;
const Byte Character::CONTROL;
const Byte Character::FORMAT;
const Byte Character::PRIVATE_USE = 18;
const Byte Character::SURROGATE = 19;
const Byte Character::DASH_PUNCTUATION = 20;
const Byte Character::START_PUNCTUATION = 21;
const Byte Character::END_PUNCTUATION = 22;
const Byte Character::CONNECTOR_PUNCTUATION = 23;
const Byte Character::OTHER_PUNCTUATION = 24;
const Byte Character::MATH_SYMBOL = 25;
const Byte Character::CURRENCY_SYMBOL = 26;
const Byte Character::MODIFIER_SYMBOL = 27;
const Byte Character::OTHER_SYMBOL = 28;
const Byte Character::INITIAL_QUOTE_PUNCTUATION = 29;
const Byte Character::FINAL_QUOTE_PUNCTUATION = 30;
const Byte Character::DIRECTIONALITY_UNDEFINED = -1;
const Byte Character::DIRECTIONALITY_LEFT_TO_RIGHT;
const Byte Character::DIRECTIONALITY_RIGHT_TO_LEFT;
const Byte Character::DIRECTIONALITY_RIGHT_TO_LEFT_ARABIC;
const Byte Character::DIRECTIONALITY_EUROPEAN_NUMBER = 3;
const Byte Character::DIRECTIONALITY_EUROPEAN_NUMBER_SEPARATOR = 4;
const Byte Character::DIRECTIONALITY_EUROPEAN_NUMBER_TERMINATOR = 5;
const Byte Character::DIRECTIONALITY_ARABIC_NUMBER = 6;
const Byte Character::DIRECTIONALITY_COMMON_NUMBER_SEPARATOR = 7;
const Byte Character::DIRECTIONALITY_NONSPACING_MARK = 8;
const Byte Character::DIRECTIONALITY_BOUNDARY_NEUTRAL;
const Byte Character::DIRECTIONALITY_PARAGRAPH_SEPARATOR = 10;
const Byte Character::DIRECTIONALITY_SEGMENT_SEPARATOR = 11;
const Byte Character::DIRECTIONALITY_WHITESPACE = 12;
const Byte Character::DIRECTIONALITY_OTHER_NEUTRALS = 13;
const Byte Character::DIRECTIONALITY_LEFT_TO_RIGHT_EMBEDDING;
const Byte Character::DIRECTIONALITY_LEFT_TO_RIGHT_OVERRIDE;
const Byte Character::DIRECTIONALITY_RIGHT_TO_LEFT_EMBEDDING;
const Byte Character::DIRECTIONALITY_RIGHT_TO_LEFT_OVERRIDE;
const Byte Character::DIRECTIONALITY_POP_DIRECTIONAL_FORMAT;


const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SURROGATES_AREA = new Character::UnicodeBlock(String("SURROGATES_AREA"), 0X0, 0X0);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::BASIC_LATIN = new Character::UnicodeBlock(String("BASIC_LATIN"), 0x0, 0x7f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::LATIN_1_SUPPLEMENT = new Character::UnicodeBlock(String("LATIN_1_SUPPLEMENT"), 0x80, 0xff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::LATIN_EXTENDED_A = new Character::UnicodeBlock(String("LATIN_EXTENDED_A"), 0x100, 0x17f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::LATIN_EXTENDED_B = new Character::UnicodeBlock(String("LATIN_EXTENDED_B"), 0x180, 0x24f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::IPA_EXTENSIONS = new Character::UnicodeBlock(String("IPA_EXTENSIONS"), 0x250, 0x2af);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SPACING_MODIFIER_LETTERS = new Character::UnicodeBlock(String("SPACING_MODIFIER_LETTERS"), 0x2b0, 0x2ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::COMBINING_DIACRITICAL_MARKS = new Character::UnicodeBlock(String("COMBINING_DIACRITICAL_MARKS"), 0x300, 0x36f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::GREEK = new Character::UnicodeBlock(String("GREEK"), 0x370, 0x3ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CYRILLIC = new Character::UnicodeBlock(String("CYRILLIC"), 0x400, 0x4ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CYRILLIC_SUPPLEMENTARY = new Character::UnicodeBlock(String("CYRILLIC_SUPPLEMENTARY"), 0x500, 0x52f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ARMENIAN = new Character::UnicodeBlock(String("ARMENIAN"), 0x530, 0x58f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::HEBREW = new Character::UnicodeBlock(String("HEBREW"), 0x590, 0x5ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ARABIC = new Character::UnicodeBlock(String("ARABIC"), 0x600, 0x6ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SYRIAC = new Character::UnicodeBlock(String("SYRIAC"), 0x700, 0x74f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::THAANA = new Character::UnicodeBlock(String("THAANA"), 0x780, 0x7bf);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::DEVANAGARI = new Character::UnicodeBlock(String("DEVANAGARI"), 0x900, 0x97f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::BENGALI = new Character::UnicodeBlock(String("BENGALI"), 0x980, 0x9ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::GURMUKHI = new Character::UnicodeBlock(String("GURMUKHI"), 0xa00, 0xa7f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::GUJARATI = new Character::UnicodeBlock(String("GUJARATI"), 0xa80, 0xaff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ORIYA = new Character::UnicodeBlock(String("ORIYA"), 0xb00, 0xb7f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::TAMIL = new Character::UnicodeBlock(String("TAMIL"), 0xb80, 0xbff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::TELUGU = new Character::UnicodeBlock(String("TELUGU"), 0xc00, 0xc7f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::KANNADA = new Character::UnicodeBlock(String("KANNADA"), 0xc80, 0xcff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MALAYALAM = new Character::UnicodeBlock(String("MALAYALAM"), 0xd00, 0xd7f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SINHALA = new Character::UnicodeBlock(String("SINHALA"), 0xd80, 0xdff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::THAI = new Character::UnicodeBlock(String("THAI"), 0xe00, 0xe7f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::LAO = new Character::UnicodeBlock(String("LAO"), 0xe80, 0xeff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::TIBETAN = new Character::UnicodeBlock(String("TIBETAN"), 0xf00, 0xfff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MYANMAR = new Character::UnicodeBlock(String("MYANMAR"), 0x1000, 0x109f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::GEORGIAN = new Character::UnicodeBlock(String("GEORGIAN"), 0x10a0, 0x10ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::HANGUL_JAMO = new Character::UnicodeBlock(String("HANGUL_JAMO"), 0x1100, 0x11ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ETHIOPIC = new Character::UnicodeBlock(String("ETHIOPIC"), 0x1200, 0x137f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CHEROKEE = new Character::UnicodeBlock(String("CHEROKEE"), 0x13a0, 0x13ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS = new Character::UnicodeBlock(String("UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS"), 0x1400, 0x167f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::OGHAM = new Character::UnicodeBlock(String("OGHAM"), 0x1680, 0x169f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::RUNIC = new Character::UnicodeBlock(String("RUNIC"), 0x16a0, 0x16ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::TAGALOG = new Character::UnicodeBlock(String("TAGALOG"), 0x1700, 0x171f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::HANUNOO = new Character::UnicodeBlock(String("HANUNOO"), 0x1720, 0x173f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::BUHID = new Character::UnicodeBlock(String("BUHID"), 0x1740, 0x175f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::TAGBANWA = new Character::UnicodeBlock(String("TAGBANWA"), 0x1760, 0x177f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::KHMER = new Character::UnicodeBlock(String("KHMER"), 0x1780, 0x17ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MONGOLIAN = new Character::UnicodeBlock(String("MONGOLIAN"), 0x1800, 0x18af);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::LIMBU = new Character::UnicodeBlock(String("LIMBU"), 0x1900, 0x194f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::TAI_LE = new Character::UnicodeBlock(String("TAI_LE"), 0x1950, 0x197f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::KHMER_SYMBOLS = new Character::UnicodeBlock(String("KHMER_SYMBOLS"), 0x19e0, 0x19ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::PHONETIC_EXTENSIONS = new Character::UnicodeBlock(String("PHONETIC_EXTENSIONS"), 0x1d00, 0x1d7f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::LATIN_EXTENDED_ADDITIONAL = new Character::UnicodeBlock(String("LATIN_EXTENDED_ADDITIONAL"), 0x1e00, 0x1eff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::GREEK_EXTENDED = new Character::UnicodeBlock(String("GREEK_EXTENDED"), 0x1f00, 0x1fff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::GENERAL_PUNCTUATION = new Character::UnicodeBlock(String("GENERAL_PUNCTUATION"), 0x2000, 0x206f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SUPERSCRIPTS_AND_SUBSCRIPTS = new Character::UnicodeBlock(String("SUPERSCRIPTS_AND_SUBSCRIPTS"), 0x2070, 0x209f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CURRENCY_SYMBOLS = new Character::UnicodeBlock(String("CURRENCY_SYMBOLS"), 0x20a0, 0x20cf);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::COMBINING_MARKS_FOR_SYMBOLS = new Character::UnicodeBlock(String("COMBINING_MARKS_FOR_SYMBOLS"), 0x20d0, 0x20ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::LETTERLIKE_SYMBOLS = new Character::UnicodeBlock(String("LETTERLIKE_SYMBOLS"), 0x2100, 0x214f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::NUMBER_FORMS = new Character::UnicodeBlock(String("NUMBER_FORMS"), 0x2150, 0x218f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ARROWS = new Character::UnicodeBlock(String("ARROWS"), 0x2190, 0x21ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MATHEMATICAL_OPERATORS = new Character::UnicodeBlock(String("MATHEMATICAL_OPERATORS"), 0x2200, 0x22ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MISCELLANEOUS_TECHNICAL = new Character::UnicodeBlock(String("MISCELLANEOUS_TECHNICAL"), 0x2300, 0x23ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CONTROL_PICTURES = new Character::UnicodeBlock(String("CONTROL_PICTURES"), 0x2400, 0x243f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::OPTICAL_CHARACTER_RECOGNITION = new Character::UnicodeBlock(String("OPTICAL_CHARACTER_RECOGNITION"), 0x2440, 0x245f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ENCLOSED_ALPHANUMERICS = new Character::UnicodeBlock(String("ENCLOSED_ALPHANUMERICS"), 0x2460, 0x24ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::BOX_DRAWING = new Character::UnicodeBlock(String("BOX_DRAWING"), 0x2500, 0x257f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::BLOCK_ELEMENTS = new Character::UnicodeBlock(String("BLOCK_ELEMENTS"), 0x2580, 0x259f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::GEOMETRIC_SHAPES = new Character::UnicodeBlock(String("GEOMETRIC_SHAPES"), 0x25a0, 0x25ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MISCELLANEOUS_SYMBOLS = new Character::UnicodeBlock(String("MISCELLANEOUS_SYMBOLS"), 0x2600, 0x26ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::DINGBATS = new Character::UnicodeBlock(String("DINGBATS"), 0x2700, 0x27bf);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A = new Character::UnicodeBlock(String("MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A"), 0x27c0, 0x27ef);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SUPPLEMENTAL_ARROWS_A = new Character::UnicodeBlock(String("SUPPLEMENTAL_ARROWS_A"), 0x27f0, 0x27ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::BRAILLE_PATTERNS = new Character::UnicodeBlock(String("BRAILLE_PATTERNS"), 0x2800, 0x28ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SUPPLEMENTAL_ARROWS_B = new Character::UnicodeBlock(String("SUPPLEMENTAL_ARROWS_B"), 0x2900, 0x297f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B = new Character::UnicodeBlock(String("MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B"), 0x2980, 0x29ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SUPPLEMENTAL_MATHEMATICAL_OPERATORS = new Character::UnicodeBlock(String("SUPPLEMENTAL_MATHEMATICAL_OPERATORS"), 0x2a00, 0x2aff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MISCELLANEOUS_SYMBOLS_AND_ARROWS = new Character::UnicodeBlock(String("MISCELLANEOUS_SYMBOLS_AND_ARROWS"), 0x2b00, 0x2bff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CJK_RADICALS_SUPPLEMENT = new Character::UnicodeBlock(String("CJK_RADICALS_SUPPLEMENT"), 0x2e80, 0x2eff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::KANGXI_RADICALS = new Character::UnicodeBlock(String("KANGXI_RADICALS"), 0x2f00, 0x2fdf);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::IDEOGRAPHIC_DESCRIPTION_CHARACTERS = new Character::UnicodeBlock(String("IDEOGRAPHIC_DESCRIPTION_CHARACTERS"), 0x2ff0, 0x2fff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CJK_SYMBOLS_AND_PUNCTUATION = new Character::UnicodeBlock(String("CJK_SYMBOLS_AND_PUNCTUATION"), 0x3000, 0x303f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::HIRAGANA = new Character::UnicodeBlock(String("HIRAGANA"), 0x3040, 0x309f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::KATAKANA = new Character::UnicodeBlock(String("KATAKANA"), 0x30a0, 0x30ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::BOPOMOFO = new Character::UnicodeBlock(String("BOPOMOFO"), 0x3100, 0x312f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::HANGUL_COMPATIBILITY_JAMO = new Character::UnicodeBlock(String("HANGUL_COMPATIBILITY_JAMO"), 0x3130, 0x318f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::KANBUN = new Character::UnicodeBlock(String("KANBUN"), 0x3190, 0x319f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::BOPOMOFO_EXTENDED = new Character::UnicodeBlock(String("BOPOMOFO_EXTENDED"), 0x31a0, 0x31bf);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::KATAKANA_PHONETIC_EXTENSIONS = new Character::UnicodeBlock(String("KATAKANA_PHONETIC_EXTENSIONS"), 0x31f0, 0x31ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ENCLOSED_CJK_LETTERS_AND_MONTHS = new Character::UnicodeBlock(String("ENCLOSED_CJK_LETTERS_AND_MONTHS"), 0x3200, 0x32ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CJK_COMPATIBILITY = new Character::UnicodeBlock(String("CJK_COMPATIBILITY"), 0x3300, 0x33ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A = new Character::UnicodeBlock(String("CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A"), 0x3400, 0x4dbf);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::YIJING_HEXAGRAM_SYMBOLS = new Character::UnicodeBlock(String("YIJING_HEXAGRAM_SYMBOLS"), 0x4dc0, 0x4dff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CJK_UNIFIED_IDEOGRAPHS = new Character::UnicodeBlock(String("CJK_UNIFIED_IDEOGRAPHS"), 0x4e00, 0x9fff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::YI_SYLLABLES = new Character::UnicodeBlock(String("YI_SYLLABLES"), 0xa000, 0xa48f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::YI_RADICALS = new Character::UnicodeBlock(String("YI_RADICALS"), 0xa490, 0xa4cf);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::HANGUL_SYLLABLES = new Character::UnicodeBlock(String("HANGUL_SYLLABLES"), 0xac00, 0xd7af);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::HIGH_SURROGATES = new Character::UnicodeBlock(String("HIGH_SURROGATES"), 0xd800, 0xdb7f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::HIGH_PRIVATE_USE_SURROGATES = new Character::UnicodeBlock(String("HIGH_PRIVATE_USE_SURROGATES"), 0xdb80, 0xdbff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::LOW_SURROGATES = new Character::UnicodeBlock(String("LOW_SURROGATES"), 0xdc00, 0xdfff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::PRIVATE_USE_AREA = new Character::UnicodeBlock(String("PRIVATE_USE_AREA"), 0xe000, 0xf8ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CJK_COMPATIBILITY_IDEOGRAPHS = new Character::UnicodeBlock(String("CJK_COMPATIBILITY_IDEOGRAPHS"), 0xf900, 0xfaff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ALPHABETIC_PRESENTATION_FORMS = new Character::UnicodeBlock(String("ALPHABETIC_PRESENTATION_FORMS"), 0xfb00, 0xfb4f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ARABIC_PRESENTATION_FORMS_A = new Character::UnicodeBlock(String("ARABIC_PRESENTATION_FORMS_A"), 0xfb50, 0xfdff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::VARIATION_SELECTORS = new Character::UnicodeBlock(String("VARIATION_SELECTORS"), 0xfe00, 0xfe0f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::COMBINING_HALF_MARKS = new Character::UnicodeBlock(String("COMBINING_HALF_MARKS"), 0xfe20, 0xfe2f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CJK_COMPATIBILITY_FORMS = new Character::UnicodeBlock(String("CJK_COMPATIBILITY_FORMS"), 0xfe30, 0xfe4f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SMALL_FORM_VARIANTS = new Character::UnicodeBlock(String("SMALL_FORM_VARIANTS"), 0xfe50, 0xfe6f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ARABIC_PRESENTATION_FORMS_B = new Character::UnicodeBlock(String("ARABIC_PRESENTATION_FORMS_B"), 0xfe70, 0xfeff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::HALFWIDTH_AND_FULLWIDTH_FORMS = new Character::UnicodeBlock(String("HALFWIDTH_AND_FULLWIDTH_FORMS"), 0xff00, 0xffef);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SPECIALS = new Character::UnicodeBlock(String("SPECIALS"), 0xfff0, 0xffff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::LINEAR_B_SYLLABARY = new Character::UnicodeBlock(String("LINEAR_B_SYLLABARY"), 0x10000, 0x1007f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::LINEAR_B_IDEOGRAMS = new Character::UnicodeBlock(String("LINEAR_B_IDEOGRAMS"), 0x10080, 0x100ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::AEGEAN_NUMBERS = new Character::UnicodeBlock(String("AEGEAN_NUMBERS"), 0x10100, 0x1013f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::OLD_ITALIC = new Character::UnicodeBlock(String("OLD_ITALIC"), 0x10300, 0x1032f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::GOTHIC = new Character::UnicodeBlock(String("GOTHIC"), 0x10330, 0x1034f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::UGARITIC = new Character::UnicodeBlock(String("UGARITIC"), 0x10380, 0x1039f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::DESERET = new Character::UnicodeBlock(String("DESERET"), 0x10400, 0x1044f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SHAVIAN = new Character::UnicodeBlock(String("SHAVIAN"), 0x10450, 0x1047f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::OSMANYA = new Character::UnicodeBlock(String("OSMANYA"), 0x10480, 0x104af);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CYPRIOT_SYLLABARY = new Character::UnicodeBlock(String("CYPRIOT_SYLLABARY"), 0x10800, 0x1083f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::BYZANTINE_MUSICAL_SYMBOLS = new Character::UnicodeBlock(String("BYZANTINE_MUSICAL_SYMBOLS"), 0x1d000, 0x1d0ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MUSICAL_SYMBOLS = new Character::UnicodeBlock(String("MUSICAL_SYMBOLS"), 0x1d100, 0x1d1ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::TAI_XUAN_JING_SYMBOLS = new Character::UnicodeBlock(String("TAI_XUAN_JING_SYMBOLS"), 0x1d300, 0x1d35f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MATHEMATICAL_ALPHANUMERIC_SYMBOLS = new Character::UnicodeBlock(String("MATHEMATICAL_ALPHANUMERIC_SYMBOLS"), 0x1d400, 0x1d7ff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B = new Character::UnicodeBlock(String("CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B"), 0x20000, 0x2a6df);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT = new Character::UnicodeBlock(String("CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT"), 0x2f800, 0x2fa1f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::TAGS = new Character::UnicodeBlock(String("TAGS"), 0xe0000, 0xe007f);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::VARIATION_SELECTORS_SUPPLEMENT = new Character::UnicodeBlock(String("VARIATION_SELECTORS_SUPPLEMENT"), 0xe0100, 0xe01ef);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SUPPLEMENTARY_PRIVATE_USE_AREA_A = new Character::UnicodeBlock(String("SUPPLEMENTARY_PRIVATE_USE_AREA_A"), 0xf0000, 0xfffff);
const AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SUPPLEMENTARY_PRIVATE_USE_AREA_B = new Character::UnicodeBlock(String("SUPPLEMENTARY_PRIVATE_USE_AREA_B"), 0x100000, 0x10ffff);

AutoPtr<ArrayOf<Character::UnicodeBlock*> > Character::UnicodeBlock::BLOCKS;

Character::Subset::Subset(
    /* [in]*/ const String& string)
    : mName(string)
{}

Character::Subset::~Subset()
{}

ECode Character::Subset::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = mName;
    return NOERROR;
}

void Character::UnicodeBlock::InitBlocks()
{
    if (BLOCKS == NULL) {
        BLOCKS = ArrayOf<UnicodeBlock*>::Alloc(126);

        BLOCKS->Set(0, NULL);
        BLOCKS->Set(1, BASIC_LATIN);
        BLOCKS->Set(2, LATIN_1_SUPPLEMENT);
        BLOCKS->Set(3, LATIN_EXTENDED_A);
        BLOCKS->Set(4, LATIN_EXTENDED_B);
        BLOCKS->Set(5, IPA_EXTENSIONS);
        BLOCKS->Set(6, SPACING_MODIFIER_LETTERS);
        BLOCKS->Set(7, COMBINING_DIACRITICAL_MARKS);
        BLOCKS->Set(8, GREEK);
        BLOCKS->Set(9, CYRILLIC);
        BLOCKS->Set(10, ARMENIAN);
        BLOCKS->Set(11, HEBREW);
        BLOCKS->Set(12, ARABIC);
        BLOCKS->Set(13, SYRIAC);
        BLOCKS->Set(14, THAANA);
        BLOCKS->Set(15, DEVANAGARI);
        BLOCKS->Set(16, BENGALI);
        BLOCKS->Set(17, GURMUKHI);
        BLOCKS->Set(18, GUJARATI);
        BLOCKS->Set(19, ORIYA);
        BLOCKS->Set(20, TAMIL);
        BLOCKS->Set(21, TELUGU);
        BLOCKS->Set(22, KANNADA);
        BLOCKS->Set(23, MALAYALAM);
        BLOCKS->Set(24, SINHALA);
        BLOCKS->Set(25, THAI);
        BLOCKS->Set(26, LAO);
        BLOCKS->Set(27, TIBETAN);
        BLOCKS->Set(28, MYANMAR);
        BLOCKS->Set(29, GEORGIAN);
        BLOCKS->Set(30, HANGUL_JAMO);
        BLOCKS->Set(31, ETHIOPIC);
        BLOCKS->Set(32, CHEROKEE);
        BLOCKS->Set(33, UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS);
        BLOCKS->Set(34, OGHAM);
        BLOCKS->Set(35, RUNIC);
        BLOCKS->Set(36, KHMER);
        BLOCKS->Set(37, MONGOLIAN);
        BLOCKS->Set(38, LATIN_EXTENDED_ADDITIONAL);
        BLOCKS->Set(39, GREEK_EXTENDED);
        BLOCKS->Set(40, GENERAL_PUNCTUATION);
        BLOCKS->Set(41, SUPERSCRIPTS_AND_SUBSCRIPTS);
        BLOCKS->Set(42, CURRENCY_SYMBOLS);
        BLOCKS->Set(43, COMBINING_MARKS_FOR_SYMBOLS);
        BLOCKS->Set(44, LETTERLIKE_SYMBOLS);
        BLOCKS->Set(45, NUMBER_FORMS);
        BLOCKS->Set(46, ARROWS);
        BLOCKS->Set(47, MATHEMATICAL_OPERATORS);
        BLOCKS->Set(47, MISCELLANEOUS_TECHNICAL);
        BLOCKS->Set(49, CONTROL_PICTURES);
        BLOCKS->Set(50, OPTICAL_CHARACTER_RECOGNITION);
        BLOCKS->Set(51, ENCLOSED_ALPHANUMERICS);
        BLOCKS->Set(52, BOX_DRAWING);
        BLOCKS->Set(53, BLOCK_ELEMENTS);
        BLOCKS->Set(54, GEOMETRIC_SHAPES);
        BLOCKS->Set(55, MISCELLANEOUS_SYMBOLS);
        BLOCKS->Set(56, DINGBATS);
        BLOCKS->Set(57, BRAILLE_PATTERNS);
        BLOCKS->Set(58, CJK_RADICALS_SUPPLEMENT);
        BLOCKS->Set(59, KANGXI_RADICALS);
        BLOCKS->Set(60, IDEOGRAPHIC_DESCRIPTION_CHARACTERS);
        BLOCKS->Set(61, CJK_SYMBOLS_AND_PUNCTUATION);
        BLOCKS->Set(62, HIRAGANA);
        BLOCKS->Set(63, KATAKANA);
        BLOCKS->Set(64, BOPOMOFO);
        BLOCKS->Set(65, HANGUL_COMPATIBILITY_JAMO);
        BLOCKS->Set(66, KANBUN);
        BLOCKS->Set(67, BOPOMOFO_EXTENDED);
        BLOCKS->Set(68, ENCLOSED_CJK_LETTERS_AND_MONTHS);
        BLOCKS->Set(69, CJK_COMPATIBILITY);
        BLOCKS->Set(70, CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A);
        BLOCKS->Set(71, CJK_UNIFIED_IDEOGRAPHS);
        BLOCKS->Set(72, YI_SYLLABLES);
        BLOCKS->Set(73, YI_RADICALS);
        BLOCKS->Set(74, HANGUL_SYLLABLES);
        BLOCKS->Set(75, HIGH_SURROGATES);
        BLOCKS->Set(76, HIGH_PRIVATE_USE_SURROGATES);
        BLOCKS->Set(77, LOW_SURROGATES);
        BLOCKS->Set(78, PRIVATE_USE_AREA);
        BLOCKS->Set(79, CJK_COMPATIBILITY_IDEOGRAPHS);
        BLOCKS->Set(80, ALPHABETIC_PRESENTATION_FORMS);
        BLOCKS->Set(81, ARABIC_PRESENTATION_FORMS_A);
        BLOCKS->Set(82, COMBINING_HALF_MARKS);
        BLOCKS->Set(83, CJK_COMPATIBILITY_FORMS);
        BLOCKS->Set(84, SMALL_FORM_VARIANTS);
        BLOCKS->Set(85, ARABIC_PRESENTATION_FORMS_B);
        BLOCKS->Set(86, SPECIALS);
        BLOCKS->Set(87, HALFWIDTH_AND_FULLWIDTH_FORMS);
        BLOCKS->Set(88, OLD_ITALIC);
        BLOCKS->Set(89, GOTHIC);
        BLOCKS->Set(90, DESERET);
        BLOCKS->Set(91, BYZANTINE_MUSICAL_SYMBOLS);
        BLOCKS->Set(92, MUSICAL_SYMBOLS);
        BLOCKS->Set(93, MATHEMATICAL_ALPHANUMERIC_SYMBOLS);
        BLOCKS->Set(94, CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B);
        BLOCKS->Set(95, CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT);
        BLOCKS->Set(96, TAGS);
        BLOCKS->Set(97, CYRILLIC_SUPPLEMENTARY);
        BLOCKS->Set(98, TAGALOG);
        BLOCKS->Set(99, HANUNOO);
        BLOCKS->Set(100, BUHID);
        BLOCKS->Set(101, TAGBANWA);
        BLOCKS->Set(102, MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A);
        BLOCKS->Set(103, SUPPLEMENTAL_ARROWS_A);
        BLOCKS->Set(104, SUPPLEMENTAL_ARROWS_B);
        BLOCKS->Set(105, MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B);
        BLOCKS->Set(106, SUPPLEMENTAL_MATHEMATICAL_OPERATORS);
        BLOCKS->Set(107, KATAKANA_PHONETIC_EXTENSIONS);
        BLOCKS->Set(108, VARIATION_SELECTORS);
        BLOCKS->Set(109, SUPPLEMENTARY_PRIVATE_USE_AREA_A);
        BLOCKS->Set(110, SUPPLEMENTARY_PRIVATE_USE_AREA_B);
        BLOCKS->Set(111, LIMBU);
        BLOCKS->Set(112, TAI_LE);
        BLOCKS->Set(113, KHMER_SYMBOLS);
        BLOCKS->Set(114, PHONETIC_EXTENSIONS);
        BLOCKS->Set(115, MISCELLANEOUS_SYMBOLS_AND_ARROWS);
        BLOCKS->Set(116, YIJING_HEXAGRAM_SYMBOLS);
        BLOCKS->Set(117, LINEAR_B_SYLLABARY);
        BLOCKS->Set(118, LINEAR_B_IDEOGRAMS);
        BLOCKS->Set(119, AEGEAN_NUMBERS);
        BLOCKS->Set(120, UGARITIC);
        BLOCKS->Set(121, SHAVIAN);
        BLOCKS->Set(122, OSMANYA);
        BLOCKS->Set(123, CYPRIOT_SYLLABARY);
        BLOCKS->Set(124, TAI_XUAN_JING_SYMBOLS);
        BLOCKS->Set(125, VARIATION_SELECTORS_SUPPLEMENT);
    }
}

const Byte Character::DIRECTIONALITY[] = {
    DIRECTIONALITY_LEFT_TO_RIGHT, DIRECTIONALITY_RIGHT_TO_LEFT,
    DIRECTIONALITY_EUROPEAN_NUMBER,
    DIRECTIONALITY_EUROPEAN_NUMBER_SEPARATOR,
    DIRECTIONALITY_EUROPEAN_NUMBER_TERMINATOR,
    DIRECTIONALITY_ARABIC_NUMBER,
    DIRECTIONALITY_COMMON_NUMBER_SEPARATOR,
    DIRECTIONALITY_PARAGRAPH_SEPARATOR,
    DIRECTIONALITY_SEGMENT_SEPARATOR, DIRECTIONALITY_WHITESPACE,
    DIRECTIONALITY_OTHER_NEUTRALS,
    DIRECTIONALITY_LEFT_TO_RIGHT_EMBEDDING,
    DIRECTIONALITY_LEFT_TO_RIGHT_OVERRIDE,
    DIRECTIONALITY_RIGHT_TO_LEFT_ARABIC,
    DIRECTIONALITY_RIGHT_TO_LEFT_EMBEDDING,
    DIRECTIONALITY_RIGHT_TO_LEFT_OVERRIDE,
    DIRECTIONALITY_POP_DIRECTIONAL_FORMAT,
    DIRECTIONALITY_NONSPACING_MARK, DIRECTIONALITY_BOUNDARY_NEUTRAL
};

static const Char32 kByteMask = 0x000000BF;
static const Char32 kByteMark = 0x00000080;

// Mask used to set appropriate bits in first byte of UTF-8 sequence,
// indexed by number of bytes in the sequence.
// 0xxxxxxx
// -> (00-7f) 7bit. Bit mask for the first byte is 0x00000000
// 110yyyyx 10xxxxxx
// -> (c0-df)(80-bf) 11bit. Bit mask is 0x000000C0
// 1110yyyy 10yxxxxx 10xxxxxx
// -> (e0-ef)(80-bf)(80-bf) 16bit. Bit mask is 0x000000E0
// 11110yyy 10yyxxxx 10xxxxxx 10xxxxxx
// -> (f0-f7)(80-bf)(80-bf)(80-bf) 21bit. Bit mask is 0x000000F0
static const Char32 kFirstByteMark[] = {
    0x00000000, 0x00000000, 0x000000C0, 0x000000E0, 0x000000F0
};


AutoPtr<Character::UnicodeBlock>
Character::UnicodeBlock::ForName(
    /* [in] */ const String& blockName)
{
    assert(!blockName.IsNull());
    Int32 block = Character::ForNameImpl(blockName);
    if (block == -1) {
        if(blockName.Equals("SURROGATES_AREA")) {
            return SURROGATES_AREA;
        }
        else if(blockName.EqualsIgnoreCase("greek")) {
            return GREEK;
        }
        else if(blockName.Equals("COMBINING_MARKS_FOR_SYMBOLS") ||
                blockName.Equals("Combining Marks for Symbols") ||
                blockName.Equals("CombiningMarksforSymbols")) {
            return COMBINING_MARKS_FOR_SYMBOLS;
        }
        return NULL;
    }

    InitBlocks();
    return (*BLOCKS)[block];
}

AutoPtr<Character::UnicodeBlock>
Character::UnicodeBlock::Of(
    /* [in] */ Char32 c)
{
    if (!IsValidCodePoint(c)) {
        return NULL;
    }

    InitBlocks();

    Int32 block = Character::OfImpl((Int32)c);
    Int32 len = BLOCKS->GetLength();
    if (block == -1 || block >= len) {
        return NULL;
    }

    return (*BLOCKS)[block];
}

Character::UnicodeBlock::UnicodeBlock(
    /* [in] */ const String& blockName,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
    :Subset(blockName)
{
}

Character::UnicodeBlock::~UnicodeBlock()
{
}

Boolean Character::IsValidCodePoint(
    /* [in] */ Char32 c)
{
    if (c > MAX_CODE_POINT) {
        return FALSE;
    }
    else if (MIN_HIGH_SURROGATE <= c && c <= MAX_LOW_SURROGATE) {
        return FALSE;
    }

    return TRUE;
}

Int32 Character::GetByteCount(
    /* [in] */ Char32 c)
{
    if (!IsValidCodePoint(c)) {
        return 0;
    }

    Int32 bytesToWrite;

    // Figure out how many bytes the result will require.
    if (c < 0x00000080) {
        bytesToWrite = 1;
    }
    else if (c < 0x00000800) {
        bytesToWrite = 2;
    }
    else if (c < 0x00010000) {
          bytesToWrite = 3;
    }
    else {
        bytesToWrite = 4;
    }

    return bytesToWrite;
}

ECode Character::GetCharAt(
    /* [in] */ const ArrayOf<Byte>& seq,
    /* [in] */ Int32 index,
    /* [out] */ Char32* c,
    /* [out] */ Int32* nextIndex)
{
    VALIDATE_NOT_NULL(c);

    if (index < 0 || index >= seq.GetLength()) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    *c = (Char32)GetCharAt((const char*)seq.GetPayload(), seq.GetLength(), index, nextIndex);
    return NOERROR;
}

ECode Character::GetCharAt(
    /* [in] */ const String& seq,
    /* [in] */ Int32 index,
    /* [out] */ Char32* c,
    /* [out] */ Int32* nextIndex)
{
    VALIDATE_NOT_NULL(c);

    if (seq.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    if (index < 0 || index >= seq.GetLength()) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    *c = (Char32)GetCharAt(seq, seq.GetLength(), index, nextIndex);
    return NOERROR;
}

ECode Character::GetCharBefore(
    /*[in]*/ const ArrayOf<Byte>& seq,
    /*[in]*/ Int32 index,
    /* [out] */ Char32* c)
{
    VALIDATE_NOT_NULL(c);

    if (index < 1 || index > seq.GetLength()) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    const char* src = (const char*)seq.GetPayload();
    char b = src[--index];
    if ((b & 0x80) == 0) { // ASCII
        *c = b;
        return NOERROR;
    }
    while ((b & 0xc0) == 0x80) {
        if (index < 1) {
            break;
        }
        b = src[--index];
    }
    Int32 numRead;
    *c = (Char32)GetCharAtInternal(src + index, &numRead);
    return NOERROR;
}

ECode Character::GetCharBefore(
    /*[in]*/ const String& seq,
    /*[in]*/ Int32 index,
    /* [out] */ Char32* c)
{
    VALIDATE_NOT_NULL(c);
    *c = '\0';

    if (seq.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    if (index < 1 || index > seq.GetLength()) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    const char* src = seq.string();
    char b = src[--index];
    if ((b & 0x80) == 0) { // ASCII
        *c = b;
        return NOERROR;
    }
    while ((b & 0xc0) == 0x80) {
        if (index < 1) {
            break;
        }
        b = src[--index];
    }
    Int32 numRead;
    *c = (Char32)GetCharAtInternal(src + index, &numRead);
    return NOERROR;
}

ECode Character::ToChars(
    /*[in]*/ Char32 c,
    /*[in]*/ ArrayOf<Byte>& dst,
    /*[in]*/ Int32 dstIndex,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = 0;

    if (!IsValidCodePoint(c)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 bytes = GetByteCount(c);
    if (dstIndex < 0 || dstIndex + bytes >= dst.GetLength()) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    WriteUTFBytesToBuffer((Byte*)dst.GetPayload() + dstIndex, c, bytes);
    *number = bytes;
    return NOERROR;
}

ECode Character::ToChars(
    /* [in] */ Char32 c,
    /*[out, callee] */ ArrayOf<Byte>** seq)
{
    VALIDATE_NOT_NULL(seq);
    *seq = NULL;

    if (!IsValidCodePoint(c)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 bytes = GetByteCount(c);
    *seq = ArrayOf<Byte>::Alloc(bytes);
    REFCOUNT_ADD(*seq);
    WriteUTFBytesToBuffer((Byte*)(*seq)->GetPayload(), c, bytes);
    return NOERROR;
}

ECode Character::ToChars(
    /* [in] */ const ArrayOf<Char32>& src,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [out, callee] */ ArrayOf<Byte>** dst,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = 0;
    VALIDATE_NOT_NULL(dst);

    if (offset < 0 || count < 0 || offset > src.GetLength() - count)
        return E_ILLEGAL_ARGUMENT_EXCEPTION;

    *dst = ArrayOf<Byte>::Alloc(src.GetLength() * 4);
    REFCOUNT_ADD(*dst);
    Int32 dstOffset = 0;
    for (Int32 i = 0; i < count; ++i) {
        Int32 bytes;
        Character::ToChars(src[i + offset], **dst, dstOffset, &bytes);
        dstOffset += bytes;
    }
    *number = dstOffset;

    return NOERROR;
}

ECode Character::ToChar32s(
    /* [in] */ const String& src,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Char32>* dst,
    /* [in] */ Int32 dstOffset)
{
    VALIDATE_NOT_NULL(dst);

    if (offset < 0 || count < 0 || offset > src.GetLength() - count)
        return E_ILLEGAL_ARGUMENT_EXCEPTION;

    if (count > dst->GetLength() - dstOffset)
        return E_ILLEGAL_ARGUMENT_EXCEPTION;

    Int32 charIndex = 0;
    GetOffsetByChars(src, 0, offset, &charIndex);
    Int32 srcByteLength = src.GetByteLength();
    Int32 nextIndex = 0;
    for(Int32 i = 0; i < count; i++) {
        (*dst)[i + dstOffset] = (Char32)GetCharAt(src, srcByteLength, charIndex, &nextIndex);
        charIndex = nextIndex;
    }

    return NOERROR;
}

ECode Character::GetCharCount(
    /*[in]*/ const ArrayOf<Byte>& seq,
    /*[in]*/ Int32 offset,
    /*[in]*/ Int32 count,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = 0;

    Int32 len = seq.GetLength();
    Int32 endIndex = offset + count;
    if (offset < 0 || count < 0 || endIndex > len) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Int32 result = 0;
    const char* src = (const char*)seq.GetPayload();
    while (offset < endIndex) {
        if (GetCharAt(src, len, offset, &offset) <= 0) break;
        result++;
    }
    *number = result;
    return NOERROR;
}

ECode Character::GetCharCount(
    /*[in]*/ const String& seq,
    /*[in]*/ Int32 offset,
    /*[in]*/ Int32 count,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = 0;

    if (seq.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    Int32 len = seq.GetByteLength();
    Int32 endIndex = offset + count;
    if (offset < 0 || count < 0 || endIndex > len) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Int32 result = 0;
    const char* src = seq.string();
    while (offset < endIndex) {
        if (GetCharAt(src, len, offset, &offset) <= 0) break;
        result++;
    }
    *number = result;
    return NOERROR;
}

// ECode Character::CodePointAt(
//     /*[in]*/ ArrayOf<Char32>* seq,
//     /*[in]*/ Int32 index,
//     /*[out]*/ Int32* code)
// {
//     VALIDATE_NOT_NULL(code)

//     if (seq == NULL) {
//         SLOGGERD("Character", "seq == null")
//         return E_NULL_POINTER_EXCEPTION;
//     }

//     Int32 len = seq->GetLength();
//     if (index < 0 || index >= len) {
//         return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
//     }

//     Char32 high = (*seq)[index++];
//     if (index >= len) {
//         *code = high;
//         return NOERROR;
//     }

//     Char32 low = (*seq)[index];
//     if (IsSurrogatePair(high, low)) {
//         return ToCodePoint(high, low, code);
//     }

//     *code = high;
//     return NOERROR;
// }

// ECode Character::CodePointAt(
//     /*[in]*/ ICharSequence* seq,
//     /*[in]*/ Int32 index,
//     /* [out] */ Int32* code)
// {
//     if (seq == NULL) {
//         SLOGGERD("Character", "seq == null")
//         return E_NULL_POINTER_EXCEPTION;
//     }
//     Int32 len;
//     seq->GetLength(&len);
//     if (index < 0 || index >= len) {
//         return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
//     }

//     Char32 high;
//     seq->GetCharAt(index++, &high);
//     if (index >= len) {
//         *code = high;
//         return NOERROR;
//     }
//     Char32 low;
//     seq->GetCharAt(index, &low);
//     if (IsSurrogatePair(high, low)) {
//         return ToCodePoint(high, low, code);
//     }
//     *code = high;
//     return NOERROR;
// }

// ECode Character::CodePointAt(
//     /*[in]*/ ArrayOf<Char32>* seq,
//     /*[in]*/ Int32 index,
//     [in] Int32 limit,
//     /* [out] */ Int32* code)
// {
//     if (index < 0 || index >= limit || limit < 0 || limit > seq->GetLength()) {
//         return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
//     }

//     Char32 high = (*seq)[index++];
//     if (index >= limit) {
//         *code = high;
//     }
//     Char32 low = (*seq)[index];
//     if (IsSurrogatePair(high, low)) {
//         return ToCodePoint(high, low, code);
//     }
//     *code = high;
//     return NOERROR;
// }

// ECode Character::ToCodePoint(
//     /*[in]*/ Char32 high,
//     /*[in]*/ Char32 low,
//     /*[out]*/ Int32* value)
// {
//     VALIDATE_NOT_NULL(value)

//     // See RFC 2781, Section 2.2
//     // http://www.ietf.org/rfc/rfc2781.txt
//     Int32 h = (high & 0x3FF) << 10;
//     Int32 l = low & 0x3FF;
//     *value = (h | l) + 0x10000;
//     return NOERROR;
// }

ECode Character::GetOffsetByChars(
    /*[in]*/ const ArrayOf<Byte>& seq,
    /*[in]*/ Int32 start,
    /*[in]*/ Int32 count,
    /*[in]*/ Int32 index,
    /*[in]*/ Int32 charOffset,
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset);
    *offset = 0;

    Int32 end = start + count;
    if (start < 0 || count < 0 || end > seq.GetLength() || index < start
            || index > end) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (charOffset == 0) {
        *offset = index;
        return NOERROR;
    }

    if (charOffset > 0) {
        Int32 chars = charOffset;
        Int32 i = index;
        Byte* bytes = seq.GetPayload();
        while (chars > 0) {
            chars--;
            if (i >= end) {
                *offset = --i;
                return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
            }
            if ((bytes[i++] & 0xc0) == 0xc0) {
                if (i >= end) {
                    *offset = --i;
                    return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
                }
                while ((bytes[i++] & 0xc0) == 0x80) {
                    if (i >= end) {
                        *offset = --i;
                        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
                    }
                }
                i--;
            }
        }
        *offset = i;
        return NOERROR;
    }

    assert(charOffset < 0);
    Int32 chars = -charOffset;
    Int32 i = index;
    Byte* bytes = seq.GetPayload();
    while (chars > 0) {
        chars--;
        if (i < start) {
            *offset = ++i;
            return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
        }
        if ((bytes[i--] & 0xc0) == 0xc0) {
            if (i < start) {
                *offset = ++i;
                return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
            }
            while ((bytes[i--] & 0xc0) == 0x80) {
                if (i < start) {
                    *offset = ++i;
                    return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
                }
            }
            i++;
        }
    }
    *offset = i;
    return NOERROR;
}

ECode Character::GetOffsetByChars(
    /*[in]*/ const String& seq,
    /*[in]*/ Int32 index,
    /*[in]*/ Int32 charOffset,
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset);

    Int32 len = seq.GetLength();
    if (index < 0 || index > len) {
        *offset = 0;
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (charOffset == 0) {
        *offset = index;
        return NOERROR;
    }

    if (charOffset > 0) {
        Int32 chars = charOffset;
        Int32 i = index;
        while (chars > 0) {
            chars--;
            if (i >= len) {
                *offset = --i;
                return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
            }
            if ((seq[i++] & 0xc0) == 0xc0) {
                if (i >= len) {
                    *offset = --i;
                    return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
                }
                while ((seq[i++] & 0xc0) == 0x80) {
                    if (i >= len) {
                        *offset = --i;
                        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
                    }
                }
                i--;
            }
        }
        *offset = i;
        return NOERROR;
    }

    assert(charOffset < 0);
    Int32 chars = -charOffset;
    Int32 i = index;
    while (chars > 0) {
        chars--;
        if (i < 0) {
            *offset = ++i;
            return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
        }
        if ((seq[i--] & 0xc0) == 0xc0) {
            if (i < 0) {
                *offset = ++i;
                return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
            }
            while ((seq[i--] & 0xc0) == 0x80) {
                if (i < 0) {
                    *offset = ++i;
                    return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
                }
            }
            i++;
        }
    }
    *offset = i;
    return NOERROR;
}

Int32 Character::ToDigit(
    /* [in] */ Char32 c,
    /* [in] */ Int32 radix)
{
    if (radix < MIN_RADIX || radix > MAX_RADIX) {
        return -1;
    }
    if (c < 128) {
        // Optimized for ASCII
        Int32 result = -1;
        if ('0' <= c && c <= '9') {
            result = c - '0';
        }
        else if ('a' <= c && c <= 'z') {
            result = 10 + (c - 'a');
        }
        else if ('A' <= c && c <= 'Z') {
            result = 10 + (c - 'A');
        }
        return result < radix ? result : -1;
    }
    return DigitImpl((Int32)c, radix);
}

Char32 Character::ForDigit(
    /* [in] */ Int32 digit,
    /* [in] */ Int32 radix)
{
    if (MIN_RADIX <= radix && radix <= MAX_RADIX) {
        if (0 <= digit && digit < radix) {
            return (Char32)(digit < 10 ? digit + '0' : digit + 'a' - 10);
        }
    }
    return 0;
}

Int32 Character::GetNumericValue(
    /* [in] */ Char32 c)
{
    if (c < 128) {
        // Optimized for ASCII
        if (c >= '0' && c <= '9') {
            return c - '0';
        }
        if (c >= 'a' && c <= 'z') {
            return c - ('a' - 10);
        }
        if (c >= 'A' && c <= 'Z') {
            return c - ('A' - 10);
        }
        return -1;
    }
    return GetNumericValueImpl((Int32)c);
}

Int32 Character::GetType(
    /* [in] */ Char32 c)
{
    Int32 type = GetTypeImpl((Int32)c);
    // The type values returned by ICU are not RI-compatible. The RI skips the value 17.
    if (type <= FORMAT) {
        return type;
    }
    return (type + 1);
}

Byte Character::GetDirectionality(
    /* [in] */ Char32 c)
{
    if (GetType(c) == UNASSIGNED) {
        return DIRECTIONALITY_UNDEFINED;
    }

    Byte directionality = GetDirectionalityImpl((Int32)c);
    if (directionality == -1) {
        return -1;
    }
    return DIRECTIONALITY[directionality];
}

Boolean Character::IsMirrored(
    /* [in] */ Char32 c)
{
    return IsMirroredImpl((Int32)c);
}

Boolean Character::IsDefined(
    /* [in] */ Char32 c)
{
    return IsDefinedImpl((Int32)c);
}

Boolean Character::IsDigit(
    /* [in] */ Char32 c)
{
    // Optimized case for ASCII
    if ('0' <= c && c <= '9') {
        return TRUE;
    }
    if (c < 1632) {
        return FALSE;
    }
    return IsDigitImpl((Int32)c);
}

Boolean Character::IsIdentifierIgnorable(
    /* [in] */ Char32 c)
{
    if (c < 0x600) {
        return (c >= 0 && c <= 8) || (c >= 0xe && c <= 0x1b) ||
                (c >= 0x7f && c <= 0x9f) || (c == 0xad);
    }
    return IsIdentifierIgnorableImpl((Int32)c);
}

Boolean Character::IsISOControl(
    /* [in] */ Char32 c)
{
    return (c >= 0 && c <= 0x1f) || (c >= 0x7f && c <= 0x9f);
}

Boolean Character::IsCarIdentifierPart(
    /* [in] */ Char32 c)
{
    // BEGIN android-changed: use precomputed bitmasks for the ASCII range.
    // Optimized case for ASCII
    if (c < 64) {
        return (0x3ff00100fffc1ffLL & (1LL << c)) != 0;
    }
    else if (c < 128) {
        return (0x87fffffe87fffffeLL & (1LL << (c - 64))) != 0;
    }
    Int32 type = GetType(c);
    return (type >= UPPERCASE_LETTER && type <= OTHER_LETTER)
            || type == CURRENCY_SYMBOL || type == CONNECTOR_PUNCTUATION
            || (type >= DECIMAL_DIGIT_NUMBER && type <= LETTER_NUMBER)
            || type == COMBINING_SPACING_MARK || type == NON_SPACING_MARK
            || (c >= 0 && c <= 8) || (c >= 0xe && c <= 0x1b)
            || (c >= 0x7f && c <= 0x9f) || type == FORMAT;
    // END android-changed
}

Boolean Character::IsCarIdentifierStart(
    /* [in] */ Char32 c)
{
    // BEGIN android-changed: use precomputed bitmasks for the ASCII range.
    // Optimized case for ASCII
    if (c < 64) {
        return (c == '$'); // There's only one character in this range.
    }
    else if (c < 128) {
        return (0x7fffffe87fffffeLL & (1LL << (c - 64))) != 0;
    }
    Int32 type = GetType(c);
     return (type >= UPPERCASE_LETTER && type <= OTHER_LETTER) || type == CURRENCY_SYMBOL
            || type == CONNECTOR_PUNCTUATION || type == LETTER_NUMBER;
    // END android-changed
}

Boolean Character::IsCarLetter(
    /* [in] */ Char32 c)
{
    return IsCarIdentifierStart(c);
}

Boolean Character::IsCarLetterOrDigit(
    /* [in] */ Char32 c)
{
    return IsCarIdentifierPart(c);
}

Boolean Character::IsLetter(
    /* [in] */ Char32 c)
{
    if (('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z')) {
        return TRUE;
    }
    if (c < 128) {
        return FALSE;
    }
    return IsLetterImpl((Int32)c);
}

Boolean Character::IsLetterOrDigit(
    /* [in] */ Char32 c)
{
    // Optimized case for ASCII
    if (('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z')) {
        return TRUE;
    }
    if ('0' <= c && c <= '9') {
        return TRUE;
    }
    if (c < 128) {
        return FALSE;
    }
    return IsLetterOrDigitImpl((Int32)c);
}

Boolean Character::IsLowerCase(
    /* [in] */ Char32 c)
{
    // Optimized case for ASCII
    if ('a' <= c && c <= 'z') {
        return TRUE;
     }
    if (c < 128) {
        return FALSE;
    }
    return IsLowerCaseImpl((Int32)c);
}

Boolean Character::IsSpace(
    /* [in] */ Char32 c)
{
    return c == '\n' || c == '\t' || c == '\f' || c == '\r' || c == ' ';
}

Boolean Character::IsSpaceChar(
    /* [in] */ Char32 c)
{
    if (c == 0x20 || c == 0xa0 || c == 0x1680) {
        return TRUE;
    }
    if (c < 0x2000) {
        return FALSE;
    }
    if (c <= 0xffff) {
        return c <= 0x200b || c == 0x2028 || c == 0x2029 ||
                c == 0x202f || c == 0x3000;
    }
    return IsSpaceCharImpl((Int32)c);
}

Boolean Character::IsTitleCase(
    /* [in] */ Char32 c)
{
    return IsTitleCaseImpl((Int32)c);
}

Boolean Character::IsUnicodeIdentifierStart(
    /* [in] */ Char32 c)
{
    return IsUnicodeIdentifierStartImpl((Int32)c);
}

Boolean Character::IsUpperCase(
    /* [in] */ Char32 c)
{
    // Optimized case for ASCII
    if ('A' <= c && c <= 'Z') {
        return TRUE;
    }
    if (c < 128) {
        return FALSE;
    }
    return IsUpperCaseImpl((Int32)c);
}

Boolean Character::IsWhitespace(
    /* [in] */ Char32 c)
{
    // Optimized case for ASCII
    if ((c >= 0x1c && c <= 0x20) || (c >= 0x9 && c <= 0xd)) {
        return TRUE;
    }
    if (c == 0x1680) {
        return TRUE;
    }
    if (c < 0x2000 || c == 0x2007) {
        return FALSE;
    }
    if (c <= 0xffff) {
        return c <= 0x200b || c == 0x2028 || c == 0x2029 ||
               c == 0x3000;
    }
    return IsWhitespaceImpl((Int32)c);
}

Boolean Character::IsHighSurrogate(
    /* [in] */ Char32 ch)
{
    return (MIN_HIGH_SURROGATE <= ch && MAX_HIGH_SURROGATE >= ch);
}

Boolean Character::IsLowSurrogate(
    /* [in] */ Char32 ch)
{
    return (MIN_LOW_SURROGATE <= ch && MAX_LOW_SURROGATE >= ch);
}

Boolean Character::IsSurrogate(
    /* [in] */ Char32 ch)
{
    return ch >= MIN_SURROGATE && ch <= MAX_SURROGATE;
}

Boolean Character::IsSurrogatePair(
    /* [in] */ Char32 high,
    /* [in] */ Char32 low)
{
    return (IsHighSurrogate(high) && IsLowSurrogate(low));
}

Char32 Character::ToLowerCase(
    /* [in] */ Char32 c)
{
    // Optimized case for ASCII
    if ('A' <= c && c <= 'Z') {
        return (c + ('a' - 'A'));
    }
    if (c < 192) {
        return c;
    }
    return (Char32)ToLowerCaseImpl((Int32)c);
}

Char32 Character::ToTitleCase(
    /*[in]*/ Char32 c)
{
    return (Char32)ToTitleCaseImpl((Int32)c);
}

Char32 Character::ToUpperCase(
    /* [in] */ Char32 c)
{
    // Optimized case for ASCII
    if ('a' <= c && c <= 'z') {
        return (c - ('a' - 'A'));
    }
    if (c < 181) {
        return c;
    }
    return (Char32)ToUpperCaseImpl((Int32)c);
}

Int32 Character::OfImpl(
    /* [in] */ Int32 codePoint)
{
    return ublock_getCode(codePoint);
}

Int32 Character::DigitImpl(
    /* [in] */ Int32 codePoint,
    /* [in] */ Int32 radix)
{
    return u_digit(codePoint, radix);
}

Int32 Character::GetNumericValueImpl(
    /* [in] */ Int32 codePoint)
{
    // The letters A-Z in their uppercase ('\u0041' through '\u005A'),
    //                          lowercase ('\u0061' through '\u007A'),
    //             and full width variant ('\uFF21' through '\uFF3A'
    //                                 and '\uFF41' through '\uFF5A') forms
    // have numeric values from 10 through 35. This is independent of the
    // Unicode specification, which does not assign numeric values to these
    // char values.
    if (codePoint >= 0x41 && codePoint <= 0x5A) {
        return codePoint - 0x37;
    }
    if (codePoint >= 0x61 && codePoint <= 0x7A) {
        return codePoint - 0x57;
    }
    if (codePoint >= 0xFF21 && codePoint <= 0xFF3A) {
        return codePoint - 0xFF17;
    }
    if (codePoint >= 0xFF41 && codePoint <= 0xFF5A) {
        return codePoint - 0xFF37;
    }

    Double result = u_getNumericValue(codePoint);

    if (result == U_NO_NUMERIC_VALUE) {
        return -1;
    } else if (result < 0 || Math::Floor(result + 0.5) != result) {
        return -2;
    }

    return (Int32)result;
}

Int32 Character::GetTypeImpl(
    /* [in] */ Int32 codePoint)
{
    return u_charType(codePoint);
}

Byte Character::GetDirectionalityImpl(
    /* [in] */ Int32 codePoint)
{
    return u_charDirection(codePoint);
}

Boolean Character::IsMirroredImpl(
    /* [in] */ Int32 codePoint)
{
    return u_isMirrored(codePoint);
}

Int32 Character::ForNameImpl(
    /* [in] */ const String& blockName)
{
    if (blockName.IsNull()) {
        return 0;
    }
    return u_getPropertyValueEnum(UCHAR_BLOCK, (const char*)blockName.string());
}

Boolean Character::IsDefinedImpl(
    /* [in] */ Int32 codePoint)
{
    return u_isdefined(codePoint);
}

Boolean Character::IsDigitImpl(
    /* [in] */ Int32 codePoint)
{
    return u_isdigit(codePoint);
}

Boolean Character::IsIdentifierIgnorableImpl(
    /* [in] */ Int32 codePoint)
{
    // Java also returns true for U+0085 Next Line (it omits U+0085 from whitespace ISO controls).
    if(codePoint == 0x0085) {
        return TRUE;
    }
    return u_isIDIgnorable(codePoint);
}

Boolean Character::IsLetterImpl(
    /* [in] */ Int32 codePoint)
{
    return u_isalpha(codePoint);
}

Boolean Character::IsLetterOrDigitImpl(
    /* [in] */ Int32 codePoint)
{
    return u_isalnum(codePoint);
}

Boolean Character::IsLowerCaseImpl(
    /* [in] */ Int32 codePoint)
{
    return u_islower(codePoint);
}

Boolean Character::IsSpaceCharImpl(
    /* [in] */ Int32 codePoint)
{
    return u_isJavaSpaceChar(codePoint);
}

Boolean Character::IsTitleCaseImpl(
    /* [in] */ Int32 codePoint)
{
    return u_istitle(codePoint);
}

Boolean Character::IsUnicodeIdentifierPartImpl(
    /* [in] */ Int32 codePoint)
{
    return u_isIDPart(codePoint);
}

Boolean Character::IsUnicodeIdentifierStartImpl(
    /* [in] */ Int32 codePoint)
{
    return u_isIDStart(codePoint);
}

Boolean Character::IsUpperCaseImpl(
    /* [in] */ Int32 codePoint)
{
    return u_isupper(codePoint);
}

Boolean Character::IsWhitespaceImpl(
    /* [in] */ Int32 codePoint)
{
    // Java omits U+0085
    if(codePoint == 0x0085) {
        return FALSE;
    }
    return u_isWhitespace(codePoint);
}

Int32 Character::ToLowerCaseImpl(
    /* [in] */ Int32 codePoint)
{
    return u_tolower(codePoint);
}

Int32 Character::ToTitleCaseImpl(
    /* [in] */ Int32 codePoint)
{
    return u_totitle(codePoint);
}

Int32 Character::ToUpperCaseImpl(
    /* [in] */ Int32 codePoint)
{
    return u_toupper(codePoint);
}

void Character::WriteUTFBytesToBuffer(
    /* [in] */ Byte* dstP,
    /* [in] */ Char32 srcChar,
    /* [in] */ Int32 bytes)
{
    dstP += bytes;
    switch (bytes)
    {   /* note: everything falls through. */
        case 4: *--dstP = (Byte)((srcChar | kByteMark) & kByteMask); srcChar >>= 6;
        case 3: *--dstP = (Byte)((srcChar | kByteMark) & kByteMask); srcChar >>= 6;
        case 2: *--dstP = (Byte)((srcChar | kByteMark) & kByteMask); srcChar >>= 6;
        case 1: *--dstP = (Byte)(srcChar | kFirstByteMark[bytes]);
    }
}

Int32 Character::GetCharAt(
    /* [in] */ const char* src,
    /* [in] */ Int32 srcLen,
    /* [in] */ Int32 index,
    /* [out] */ Int32* nextIndex)
{
    if (index >= srcLen) {
        return -1;
    }
    Int32 dummyIndex;
    if (nextIndex == NULL) {
        nextIndex = &dummyIndex;
    }
    Int32 numRead;
    Int32 ret = GetCharAtInternal(src + index, &numRead);
    if (ret >= 0) {
        *nextIndex = index + numRead;
    }
    return ret;
}

Int32 Character::GetCharAtInternal(
    /* [in] */ const char* cur,
    /* [out] */ Int32* numRead)
{
    const Byte firstChar = *cur;
    if ((firstChar & 0x80) == 0) { // ASCII
        *numRead = 1;
        return *cur;
    }
    cur++;
    Char32 mask, toIgnoreMask;
    Int32 numToRead = 0;
    Char32 utf32 = firstChar;
    for (numToRead = 1, mask = 0x40, toIgnoreMask = 0xFFFFFF80;
         (firstChar & mask);
         numToRead++, toIgnoreMask |= mask, mask >>= 1) {
        // 0x3F == 00111111
        utf32 = (utf32 << 6) + (*cur++ & 0x3F);
    }
    toIgnoreMask |= mask;
    utf32 &= ~(toIgnoreMask << (6 * (numToRead - 1)));

    *numRead = numToRead;
    return (Int32)utf32;
}


} // namespace Core
} // namespace Elastos
