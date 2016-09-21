/* C++ code produced by gperf version 3.0.3 */
/* Command-line: gperf --key-positions='*' -P -n -m 50 -D  */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif


// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "CSSPropertyNames.h"
#include "core/css/HashTools.h"
#include <string.h>

#include "wtf/ASCIICType.h"
#include "wtf/text/AtomicString.h"
#include "wtf/text/WTFString.h"

namespace WebCore {
static const char propertyNameStringsPool[] = {
    "display\0"
    "-webkit-animation\0"
    "-webkit-animation-delay\0"
    "-webkit-animation-direction\0"
    "-webkit-animation-duration\0"
    "-webkit-animation-fill-mode\0"
    "-webkit-animation-iteration-count\0"
    "-webkit-animation-name\0"
    "-webkit-animation-play-state\0"
    "-webkit-animation-timing-function\0"
    "animation\0"
    "animation-delay\0"
    "animation-direction\0"
    "animation-duration\0"
    "animation-fill-mode\0"
    "animation-iteration-count\0"
    "animation-name\0"
    "animation-play-state\0"
    "animation-timing-function\0"
    "-webkit-transition\0"
    "-webkit-transition-delay\0"
    "-webkit-transition-duration\0"
    "-webkit-transition-property\0"
    "-webkit-transition-timing-function\0"
    "transition\0"
    "transition-delay\0"
    "transition-duration\0"
    "transition-property\0"
    "transition-timing-function\0"
    "color\0"
    "direction\0"
    "font\0"
    "font-family\0"
    "font-kerning\0"
    "font-size\0"
    "font-style\0"
    "font-variant\0"
    "font-variant-ligatures\0"
    "font-weight\0"
    "text-rendering\0"
    "-webkit-font-feature-settings\0"
    "-webkit-font-smoothing\0"
    "-webkit-locale\0"
    "-webkit-text-orientation\0"
    "-webkit-writing-mode\0"
    "zoom\0"
    "line-height\0"
    "background\0"
    "background-attachment\0"
    "background-blend-mode\0"
    "background-clip\0"
    "background-color\0"
    "background-image\0"
    "background-origin\0"
    "background-position\0"
    "background-position-x\0"
    "background-position-y\0"
    "background-repeat\0"
    "background-repeat-x\0"
    "background-repeat-y\0"
    "background-size\0"
    "border\0"
    "border-bottom\0"
    "border-bottom-color\0"
    "border-bottom-left-radius\0"
    "border-bottom-right-radius\0"
    "border-bottom-style\0"
    "border-bottom-width\0"
    "border-collapse\0"
    "border-color\0"
    "border-image\0"
    "border-image-outset\0"
    "border-image-repeat\0"
    "border-image-slice\0"
    "border-image-source\0"
    "border-image-width\0"
    "border-left\0"
    "border-left-color\0"
    "border-left-style\0"
    "border-left-width\0"
    "border-radius\0"
    "border-right\0"
    "border-right-color\0"
    "border-right-style\0"
    "border-right-width\0"
    "border-spacing\0"
    "border-style\0"
    "border-top\0"
    "border-top-color\0"
    "border-top-left-radius\0"
    "border-top-right-radius\0"
    "border-top-style\0"
    "border-top-width\0"
    "border-width\0"
    "bottom\0"
    "box-shadow\0"
    "box-sizing\0"
    "caption-side\0"
    "clear\0"
    "clip\0"
    "-webkit-clip-path\0"
    "content\0"
    "counter-increment\0"
    "counter-reset\0"
    "cursor\0"
    "empty-cells\0"
    "float\0"
    "font-stretch\0"
    "height\0"
    "image-rendering\0"
    "isolation\0"
    "justify-self\0"
    "left\0"
    "letter-spacing\0"
    "list-style\0"
    "list-style-image\0"
    "list-style-position\0"
    "list-style-type\0"
    "margin\0"
    "margin-bottom\0"
    "margin-left\0"
    "margin-right\0"
    "margin-top\0"
    "mask-source-type\0"
    "max-height\0"
    "max-width\0"
    "min-height\0"
    "min-width\0"
    "mix-blend-mode\0"
    "opacity\0"
    "orphans\0"
    "outline\0"
    "outline-color\0"
    "outline-offset\0"
    "outline-style\0"
    "outline-width\0"
    "object-fit\0"
    "object-position\0"
    "overflow\0"
    "overflow-wrap\0"
    "overflow-x\0"
    "overflow-y\0"
    "padding\0"
    "padding-bottom\0"
    "padding-left\0"
    "padding-right\0"
    "padding-top\0"
    "page\0"
    "page-break-after\0"
    "page-break-before\0"
    "page-break-inside\0"
    "pointer-events\0"
    "position\0"
    "quotes\0"
    "resize\0"
    "right\0"
    "scroll-behavior\0"
    "size\0"
    "src\0"
    "speak\0"
    "table-layout\0"
    "tab-size\0"
    "text-align\0"
    "text-align-last\0"
    "text-decoration\0"
    "text-decoration-line\0"
    "text-decoration-style\0"
    "text-decoration-color\0"
    "text-indent\0"
    "text-justify\0"
    "text-line-through-color\0"
    "text-line-through-mode\0"
    "text-line-through-style\0"
    "text-line-through-width\0"
    "text-overflow\0"
    "text-overline-color\0"
    "text-overline-mode\0"
    "text-overline-style\0"
    "text-overline-width\0"
    "text-shadow\0"
    "text-transform\0"
    "text-underline-color\0"
    "text-underline-mode\0"
    "text-underline-style\0"
    "text-underline-width\0"
    "text-underline-position\0"
    "top\0"
    "touch-action\0"
    "touch-action-delay\0"
    "unicode-bidi\0"
    "unicode-range\0"
    "vertical-align\0"
    "visibility\0"
    "white-space\0"
    "widows\0"
    "width\0"
    "will-change\0"
    "word-break\0"
    "word-spacing\0"
    "word-wrap\0"
    "z-index\0"
    "-webkit-appearance\0"
    "-webkit-aspect-ratio\0"
    "backface-visibility\0"
    "-webkit-backface-visibility\0"
    "-webkit-background-clip\0"
    "-webkit-background-composite\0"
    "-webkit-background-origin\0"
    "-webkit-background-size\0"
    "-webkit-border-after\0"
    "-webkit-border-after-color\0"
    "-webkit-border-after-style\0"
    "-webkit-border-after-width\0"
    "-webkit-border-before\0"
    "-webkit-border-before-color\0"
    "-webkit-border-before-style\0"
    "-webkit-border-before-width\0"
    "-webkit-border-end\0"
    "-webkit-border-end-color\0"
    "-webkit-border-end-style\0"
    "-webkit-border-end-width\0"
    "-webkit-border-fit\0"
    "-webkit-border-horizontal-spacing\0"
    "-webkit-border-image\0"
    "-webkit-border-radius\0"
    "-webkit-border-start\0"
    "-webkit-border-start-color\0"
    "-webkit-border-start-style\0"
    "-webkit-border-start-width\0"
    "-webkit-border-vertical-spacing\0"
    "-webkit-box-align\0"
    "-webkit-box-direction\0"
    "-webkit-box-flex\0"
    "-webkit-box-flex-group\0"
    "-webkit-box-lines\0"
    "-webkit-box-ordinal-group\0"
    "-webkit-box-orient\0"
    "-webkit-box-pack\0"
    "-webkit-box-reflect\0"
    "-webkit-box-shadow\0"
    "-internal-callback\0"
    "-webkit-column-break-after\0"
    "-webkit-column-break-before\0"
    "-webkit-column-break-inside\0"
    "-webkit-column-count\0"
    "column-fill\0"
    "-webkit-column-gap\0"
    "-webkit-column-rule\0"
    "-webkit-column-rule-color\0"
    "-webkit-column-rule-style\0"
    "-webkit-column-rule-width\0"
    "-webkit-column-span\0"
    "-webkit-column-width\0"
    "-webkit-columns\0"
    "-webkit-box-decoration-break\0"
    "-webkit-filter\0"
    "align-content\0"
    "align-items\0"
    "align-self\0"
    "flex\0"
    "flex-basis\0"
    "flex-direction\0"
    "flex-flow\0"
    "flex-grow\0"
    "flex-shrink\0"
    "flex-wrap\0"
    "justify-content\0"
    "-webkit-font-size-delta\0"
    "grid-auto-columns\0"
    "grid-auto-flow\0"
    "grid-auto-rows\0"
    "grid-area\0"
    "grid-column\0"
    "grid-column-end\0"
    "grid-column-start\0"
    "grid\0"
    "grid-template\0"
    "grid-template-columns\0"
    "grid-template-rows\0"
    "grid-row\0"
    "grid-row-end\0"
    "grid-row-start\0"
    "grid-template-areas\0"
    "-webkit-highlight\0"
    "-webkit-hyphenate-character\0"
    "-webkit-line-box-contain\0"
    "-webkit-line-break\0"
    "-webkit-line-clamp\0"
    "-webkit-logical-width\0"
    "-webkit-logical-height\0"
    "-webkit-margin-after-collapse\0"
    "-webkit-margin-before-collapse\0"
    "-webkit-margin-bottom-collapse\0"
    "-webkit-margin-top-collapse\0"
    "-webkit-margin-collapse\0"
    "-webkit-margin-after\0"
    "-webkit-margin-before\0"
    "-webkit-margin-end\0"
    "-webkit-margin-start\0"
    "-webkit-mask\0"
    "-webkit-mask-box-image\0"
    "-webkit-mask-box-image-outset\0"
    "-webkit-mask-box-image-repeat\0"
    "-webkit-mask-box-image-slice\0"
    "-webkit-mask-box-image-source\0"
    "-webkit-mask-box-image-width\0"
    "-webkit-mask-clip\0"
    "-webkit-mask-composite\0"
    "-webkit-mask-image\0"
    "-webkit-mask-origin\0"
    "-webkit-mask-position\0"
    "-webkit-mask-position-x\0"
    "-webkit-mask-position-y\0"
    "-webkit-mask-repeat\0"
    "-webkit-mask-repeat-x\0"
    "-webkit-mask-repeat-y\0"
    "-webkit-mask-size\0"
    "-webkit-max-logical-width\0"
    "-webkit-max-logical-height\0"
    "-webkit-min-logical-width\0"
    "-webkit-min-logical-height\0"
    "order\0"
    "-webkit-padding-after\0"
    "-webkit-padding-before\0"
    "-webkit-padding-end\0"
    "-webkit-padding-start\0"
    "perspective\0"
    "-webkit-perspective\0"
    "perspective-origin\0"
    "-webkit-perspective-origin\0"
    "-webkit-perspective-origin-x\0"
    "-webkit-perspective-origin-y\0"
    "-webkit-print-color-adjust\0"
    "-webkit-rtl-ordering\0"
    "-webkit-ruby-position\0"
    "-webkit-text-combine\0"
    "-webkit-text-decorations-in-effect\0"
    "-webkit-text-emphasis\0"
    "-webkit-text-emphasis-color\0"
    "-webkit-text-emphasis-position\0"
    "-webkit-text-emphasis-style\0"
    "-webkit-text-fill-color\0"
    "-webkit-text-security\0"
    "-webkit-text-stroke\0"
    "-webkit-text-stroke-color\0"
    "-webkit-text-stroke-width\0"
    "transform\0"
    "-webkit-transform\0"
    "transform-origin\0"
    "-webkit-transform-origin\0"
    "-webkit-transform-origin-x\0"
    "-webkit-transform-origin-y\0"
    "-webkit-transform-origin-z\0"
    "transform-style\0"
    "-webkit-transform-style\0"
    "-webkit-user-drag\0"
    "-webkit-user-modify\0"
    "-webkit-user-select\0"
    "shape-outside\0"
    "shape-margin\0"
    "shape-image-threshold\0"
    "-webkit-wrap-flow\0"
    "-webkit-wrap-through\0"
    "max-zoom\0"
    "min-zoom\0"
    "orientation\0"
    "user-zoom\0"
    "-webkit-tap-highlight-color\0"
    "-webkit-app-region\0"
    "all\0"
    "-internal-marquee-direction\0"
    "-internal-marquee-increment\0"
    "-internal-marquee-repetition\0"
    "-internal-marquee-speed\0"
    "-internal-marquee-style\0"
    "buffered-rendering\0"
    "clip-path\0"
    "clip-rule\0"
    "mask\0"
    "enable-background\0"
    "filter\0"
    "flood-color\0"
    "flood-opacity\0"
    "lighting-color\0"
    "stop-color\0"
    "stop-opacity\0"
    "color-interpolation\0"
    "color-interpolation-filters\0"
    "color-rendering\0"
    "fill\0"
    "fill-opacity\0"
    "fill-rule\0"
    "marker\0"
    "marker-end\0"
    "marker-mid\0"
    "marker-start\0"
    "mask-type\0"
    "shape-rendering\0"
    "stroke\0"
    "stroke-dasharray\0"
    "stroke-dashoffset\0"
    "stroke-linecap\0"
    "stroke-linejoin\0"
    "stroke-miterlimit\0"
    "stroke-opacity\0"
    "stroke-width\0"
    "alignment-baseline\0"
    "baseline-shift\0"
    "dominant-baseline\0"
    "glyph-orientation-horizontal\0"
    "glyph-orientation-vertical\0"
    "text-anchor\0"
    "vector-effect\0"
    "writing-mode\0"
    "paint-order\0"
};

static const unsigned short propertyNameStringsOffsets[] = {
    0,
    8,
    26,
    50,
    78,
    105,
    133,
    167,
    190,
    219,
    253,
    263,
    279,
    299,
    318,
    338,
    364,
    379,
    400,
    426,
    445,
    470,
    498,
    526,
    561,
    572,
    589,
    609,
    629,
    656,
    662,
    672,
    677,
    689,
    702,
    712,
    723,
    736,
    759,
    771,
    786,
    816,
    839,
    854,
    879,
    900,
    905,
    917,
    928,
    950,
    972,
    988,
    1005,
    1022,
    1040,
    1060,
    1082,
    1104,
    1122,
    1142,
    1162,
    1178,
    1185,
    1199,
    1219,
    1245,
    1272,
    1292,
    1312,
    1328,
    1341,
    1354,
    1374,
    1394,
    1413,
    1433,
    1452,
    1464,
    1482,
    1500,
    1518,
    1532,
    1545,
    1564,
    1583,
    1602,
    1617,
    1630,
    1641,
    1658,
    1681,
    1705,
    1722,
    1739,
    1752,
    1759,
    1770,
    1781,
    1794,
    1800,
    1805,
    1823,
    1831,
    1849,
    1863,
    1870,
    1882,
    1888,
    1901,
    1908,
    1924,
    1934,
    1947,
    1952,
    1967,
    1978,
    1995,
    2015,
    2031,
    2038,
    2052,
    2064,
    2077,
    2088,
    2105,
    2116,
    2126,
    2137,
    2147,
    2162,
    2170,
    2178,
    2186,
    2200,
    2215,
    2229,
    2243,
    2254,
    2270,
    2279,
    2293,
    2304,
    2315,
    2323,
    2338,
    2351,
    2365,
    2377,
    2382,
    2399,
    2417,
    2435,
    2450,
    2459,
    2466,
    2473,
    2479,
    2495,
    2500,
    2504,
    2510,
    2523,
    2532,
    2543,
    2559,
    2575,
    2596,
    2618,
    2640,
    2652,
    2665,
    2689,
    2712,
    2736,
    2760,
    2774,
    2794,
    2813,
    2833,
    2853,
    2865,
    2880,
    2901,
    2921,
    2942,
    2963,
    2987,
    2991,
    3004,
    3023,
    3036,
    3050,
    3065,
    3076,
    3088,
    3095,
    3101,
    3113,
    3124,
    3137,
    3147,
    3155,
    3174,
    3195,
    3215,
    3243,
    3267,
    3296,
    3322,
    3346,
    3367,
    3394,
    3421,
    3448,
    3470,
    3498,
    3526,
    3554,
    3573,
    3598,
    3623,
    3648,
    3667,
    3701,
    3722,
    3744,
    3765,
    3792,
    3819,
    3846,
    3878,
    3896,
    3918,
    3935,
    3958,
    3976,
    4002,
    4021,
    4038,
    4058,
    4077,
    4096,
    4123,
    4151,
    4179,
    4200,
    4212,
    4231,
    4251,
    4277,
    4303,
    4329,
    4349,
    4370,
    4386,
    4415,
    4430,
    4444,
    4456,
    4467,
    4472,
    4483,
    4498,
    4508,
    4518,
    4530,
    4540,
    4556,
    4580,
    4598,
    4613,
    4628,
    4638,
    4650,
    4666,
    4684,
    4689,
    4703,
    4725,
    4744,
    4753,
    4766,
    4781,
    4801,
    4819,
    4847,
    4872,
    4891,
    4910,
    4932,
    4955,
    4985,
    5016,
    5047,
    5075,
    5099,
    5120,
    5142,
    5161,
    5182,
    5195,
    5218,
    5248,
    5278,
    5307,
    5337,
    5366,
    5384,
    5407,
    5426,
    5446,
    5468,
    5492,
    5516,
    5536,
    5558,
    5580,
    5598,
    5624,
    5651,
    5677,
    5704,
    5710,
    5732,
    5755,
    5775,
    5797,
    5809,
    5829,
    5848,
    5875,
    5904,
    5933,
    5960,
    5981,
    6003,
    6024,
    6059,
    6081,
    6109,
    6140,
    6168,
    6192,
    6214,
    6234,
    6260,
    6286,
    6296,
    6314,
    6331,
    6356,
    6383,
    6410,
    6437,
    6453,
    6477,
    6495,
    6515,
    6535,
    6549,
    6562,
    6584,
    6602,
    6623,
    6632,
    6641,
    6653,
    6663,
    6691,
    6710,
    6714,
    6742,
    6770,
    6799,
    6823,
    6847,
    6866,
    6876,
    6886,
    6891,
    6909,
    6916,
    6928,
    6942,
    6957,
    6968,
    6981,
    7001,
    7029,
    7045,
    7050,
    7063,
    7073,
    7080,
    7091,
    7102,
    7115,
    7125,
    7141,
    7148,
    7165,
    7183,
    7198,
    7214,
    7232,
    7247,
    7260,
    7279,
    7294,
    7312,
    7341,
    7368,
    7380,
    7394,
    7407,
};

enum
  {
    TOTAL_KEYWORDS = 445,
    MIN_WORD_LENGTH = 3,
    MAX_WORD_LENGTH = 34,
    MIN_HASH_VALUE = 20,
    MAX_HASH_VALUE = 3137
  };

/* maximum key range = 3118, duplicates = 0 */

class CSSPropertyNamesHash
{
private:
  static inline unsigned int property_hash_function (const char *str, unsigned int len);
public:
  static const struct Property *findPropertyImpl (const char *str, unsigned int len);
};

inline unsigned int
CSSPropertyNamesHash::property_hash_function (register const char *str, register unsigned int len)
{
  static const unsigned short asso_values[] =
    {
      3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138,
      3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138,
      3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138,
      3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138,
      3138, 3138, 3138, 3138, 3138,    3, 3138, 3138, 3138, 3138,
      3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138,
      3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138,
      3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138,
      3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138,
      3138, 3138, 3138, 3138, 3138, 3138, 3138,    3,    3,  158,
         3,    3,  444,   32,  628,    3,    5,    3,  102,    7,
         3,    4,   77,    9,    5,   39,    3,  135,  318,    3,
       477,  796,  252, 3138, 3138, 3138, 3138, 3138, 3138, 3138,
      3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138,
      3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138,
      3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138,
      3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138,
      3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138,
      3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138,
      3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138,
      3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138,
      3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138,
      3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138,
      3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138,
      3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138, 3138,
      3138, 3138, 3138, 3138, 3138, 3138
    };
  register int hval = 0;

  switch (len)
    {
      default:
        hval += asso_values[(unsigned char)str[33]];
      /*FALLTHROUGH*/
      case 33:
        hval += asso_values[(unsigned char)str[32]];
      /*FALLTHROUGH*/
      case 32:
        hval += asso_values[(unsigned char)str[31]];
      /*FALLTHROUGH*/
      case 31:
        hval += asso_values[(unsigned char)str[30]];
      /*FALLTHROUGH*/
      case 30:
        hval += asso_values[(unsigned char)str[29]];
      /*FALLTHROUGH*/
      case 29:
        hval += asso_values[(unsigned char)str[28]];
      /*FALLTHROUGH*/
      case 28:
        hval += asso_values[(unsigned char)str[27]];
      /*FALLTHROUGH*/
      case 27:
        hval += asso_values[(unsigned char)str[26]];
      /*FALLTHROUGH*/
      case 26:
        hval += asso_values[(unsigned char)str[25]];
      /*FALLTHROUGH*/
      case 25:
        hval += asso_values[(unsigned char)str[24]];
      /*FALLTHROUGH*/
      case 24:
        hval += asso_values[(unsigned char)str[23]];
      /*FALLTHROUGH*/
      case 23:
        hval += asso_values[(unsigned char)str[22]];
      /*FALLTHROUGH*/
      case 22:
        hval += asso_values[(unsigned char)str[21]];
      /*FALLTHROUGH*/
      case 21:
        hval += asso_values[(unsigned char)str[20]];
      /*FALLTHROUGH*/
      case 20:
        hval += asso_values[(unsigned char)str[19]];
      /*FALLTHROUGH*/
      case 19:
        hval += asso_values[(unsigned char)str[18]];
      /*FALLTHROUGH*/
      case 18:
        hval += asso_values[(unsigned char)str[17]];
      /*FALLTHROUGH*/
      case 17:
        hval += asso_values[(unsigned char)str[16]];
      /*FALLTHROUGH*/
      case 16:
        hval += asso_values[(unsigned char)str[15]];
      /*FALLTHROUGH*/
      case 15:
        hval += asso_values[(unsigned char)str[14]];
      /*FALLTHROUGH*/
      case 14:
        hval += asso_values[(unsigned char)str[13]];
      /*FALLTHROUGH*/
      case 13:
        hval += asso_values[(unsigned char)str[12]];
      /*FALLTHROUGH*/
      case 12:
        hval += asso_values[(unsigned char)str[11]];
      /*FALLTHROUGH*/
      case 11:
        hval += asso_values[(unsigned char)str[10]];
      /*FALLTHROUGH*/
      case 10:
        hval += asso_values[(unsigned char)str[9]];
      /*FALLTHROUGH*/
      case 9:
        hval += asso_values[(unsigned char)str[8]];
      /*FALLTHROUGH*/
      case 8:
        hval += asso_values[(unsigned char)str[7]];
      /*FALLTHROUGH*/
      case 7:
        hval += asso_values[(unsigned char)str[6]];
      /*FALLTHROUGH*/
      case 6:
        hval += asso_values[(unsigned char)str[5]];
      /*FALLTHROUGH*/
      case 5:
        hval += asso_values[(unsigned char)str[4]];
      /*FALLTHROUGH*/
      case 4:
        hval += asso_values[(unsigned char)str[3]];
      /*FALLTHROUGH*/
      case 3:
        hval += asso_values[(unsigned char)str[2]];
      /*FALLTHROUGH*/
      case 2:
        hval += asso_values[(unsigned char)str[1]];
      /*FALLTHROUGH*/
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval;
}

struct stringpool_t
  {
    char stringpool_str0[sizeof("order")];
    char stringpool_str1[sizeof("border")];
    char stringpool_str2[sizeof("bottom")];
    char stringpool_str3[sizeof("marker")];
    char stringpool_str4[sizeof("animation")];
    char stringpool_str5[sizeof("word-break")];
    char stringpool_str6[sizeof("orientation")];
    char stringpool_str7[sizeof("marker-end")];
    char stringpool_str8[sizeof("marker-mid")];
    char stringpool_str9[sizeof("grid")];
    char stringpool_str10[sizeof("-webkit-order")];
    char stringpool_str11[sizeof("border-bottom")];
    char stringpool_str12[sizeof("animation-name")];
    char stringpool_str13[sizeof("mask")];
    char stringpool_str14[sizeof("margin")];
    char stringpool_str15[sizeof("widows")];
    char stringpool_str16[sizeof("-webkit-animation")];
    char stringpool_str17[sizeof("stroke")];
    char stringpool_str18[sizeof("grid-row")];
    char stringpool_str19[sizeof("-webkit-border-end")];
    char stringpool_str20[sizeof("grid-area")];
    char stringpool_str21[sizeof("transition")];
    char stringpool_str22[sizeof("grid-row-end")];
    char stringpool_str23[sizeof("writing-mode")];
    char stringpool_str24[sizeof("border-image")];
    char stringpool_str25[sizeof("-webkit-animation-name")];
    char stringpool_str26[sizeof("-webkit-mask")];
    char stringpool_str27[sizeof("margin-bottom")];
    char stringpool_str28[sizeof("marker-start")];
    char stringpool_str29[sizeof("top")];
    char stringpool_str30[sizeof("-webkit-margin-end")];
    char stringpool_str31[sizeof("-webkit-transition")];
    char stringpool_str32[sizeof("-webkit-writing-mode")];
    char stringpool_str33[sizeof("-webkit-border-image")];
    char stringpool_str34[sizeof("-webkit-border-start")];
    char stringpool_str35[sizeof("word-wrap")];
    char stringpool_str36[sizeof("border-top")];
    char stringpool_str37[sizeof("image-rendering")];
    char stringpool_str38[sizeof("paint-order")];
    char stringpool_str39[sizeof("grid-row-start")];
    char stringpool_str40[sizeof("page")];
    char stringpool_str41[sizeof("padding")];
    char stringpool_str42[sizeof("speak")];
    char stringpool_str43[sizeof("-webkit-mask-image")];
    char stringpool_str44[sizeof("-webkit-mask-origin")];
    char stringpool_str45[sizeof("-webkit-margin-start")];
    char stringpool_str46[sizeof("position")];
    char stringpool_str47[sizeof("margin-top")];
    char stringpool_str48[sizeof("padding-bottom")];
    char stringpool_str49[sizeof("-webkit-line-break")];
    char stringpool_str50[sizeof("-webkit-padding-end")];
    char stringpool_str51[sizeof("isolation")];
    char stringpool_str52[sizeof("border-image-repeat")];
    char stringpool_str53[sizeof("-webkit-mask-repeat")];
    char stringpool_str54[sizeof("content")];
    char stringpool_str55[sizeof("direction")];
    char stringpool_str56[sizeof("stroke-linejoin")];
    char stringpool_str57[sizeof("dominant-baseline")];
    char stringpool_str58[sizeof("page-break-inside")];
    char stringpool_str59[sizeof("quotes")];
    char stringpool_str60[sizeof("animation-duration")];
    char stringpool_str61[sizeof("-webkit-rtl-ordering")];
    char stringpool_str62[sizeof("stroke-miterlimit")];
    char stringpool_str63[sizeof("align-items")];
    char stringpool_str64[sizeof("src")];
    char stringpool_str65[sizeof("-webkit-padding-start")];
    char stringpool_str66[sizeof("all")];
    char stringpool_str67[sizeof("padding-top")];
    char stringpool_str68[sizeof("border-radius")];
    char stringpool_str69[sizeof("-webkit-mask-position")];
    char stringpool_str70[sizeof("-webkit-animation-duration")];
    char stringpool_str71[sizeof("animation-direction")];
    char stringpool_str72[sizeof("-webkit-align-items")];
    char stringpool_str73[sizeof("transition-duration")];
    char stringpool_str74[sizeof("-webkit-app-region")];
    char stringpool_str75[sizeof("-webkit-border-radius")];
    char stringpool_str76[sizeof("-webkit-animation-direction")];
    char stringpool_str77[sizeof("grid-auto-rows")];
    char stringpool_str78[sizeof("grid-template")];
    char stringpool_str79[sizeof("-webkit-user-drag")];
    char stringpool_str80[sizeof("outline")];
    char stringpool_str81[sizeof("-webkit-transition-duration")];
    char stringpool_str82[sizeof("-epub-word-break")];
    char stringpool_str83[sizeof("border-image-outset")];
    char stringpool_str84[sizeof("zoom")];
    char stringpool_str85[sizeof("clear")];
    char stringpool_str86[sizeof("color")];
    char stringpool_str87[sizeof("min-zoom")];
    char stringpool_str88[sizeof("-epub-writing-mode")];
    char stringpool_str89[sizeof("size")];
    char stringpool_str90[sizeof("border-color")];
    char stringpool_str91[sizeof("grid-template-rows")];
    char stringpool_str92[sizeof("caption-side")];
    char stringpool_str93[sizeof("grid-template-areas")];
    char stringpool_str94[sizeof("resize")];
    char stringpool_str95[sizeof("tab-size")];
    char stringpool_str96[sizeof("object-position")];
    char stringpool_str97[sizeof("alignment-baseline")];
    char stringpool_str98[sizeof("align-content")];
    char stringpool_str99[sizeof("unicode-bidi")];
    char stringpool_str100[sizeof("border-bottom-color")];
    char stringpool_str101[sizeof("-webkit-aspect-ratio")];
    char stringpool_str102[sizeof("word-spacing")];
    char stringpool_str103[sizeof("-webkit-border-end-color")];
    char stringpool_str104[sizeof("color-rendering")];
    char stringpool_str105[sizeof("clip")];
    char stringpool_str106[sizeof("border-spacing")];
    char stringpool_str107[sizeof("cursor")];
    char stringpool_str108[sizeof("-webkit-align-content")];
    char stringpool_str109[sizeof("background")];
    char stringpool_str110[sizeof("unicode-range")];
    char stringpool_str111[sizeof("-webkit-appearance")];
    char stringpool_str112[sizeof("counter-reset")];
    char stringpool_str113[sizeof("animation-iteration-count")];
    char stringpool_str114[sizeof("-webkit-mask-size")];
    char stringpool_str115[sizeof("-webkit-mask-composite")];
    char stringpool_str116[sizeof("-webkit-border-start-color")];
    char stringpool_str117[sizeof("border-image-slice")];
    char stringpool_str118[sizeof("border-top-color")];
    char stringpool_str119[sizeof("-webkit-animation-iteration-count")];
    char stringpool_str120[sizeof("-webkit-locale")];
    char stringpool_str121[sizeof("stop-color")];
    char stringpool_str122[sizeof("background-image")];
    char stringpool_str123[sizeof("background-origin")];
    char stringpool_str124[sizeof("-internal-marquee-repetition")];
    char stringpool_str125[sizeof("stroke-linecap")];
    char stringpool_str126[sizeof("-webkit-mask-clip")];
    char stringpool_str127[sizeof("border-image-source")];
    char stringpool_str128[sizeof("-internal-marquee-speed")];
    char stringpool_str129[sizeof("-webkit-background-origin")];
    char stringpool_str130[sizeof("letter-spacing")];
    char stringpool_str131[sizeof("background-repeat")];
    char stringpool_str132[sizeof("user-zoom")];
    char stringpool_str133[sizeof("font")];
    char stringpool_str134[sizeof("grid-column")];
    char stringpool_str135[sizeof("grid-column-end")];
    char stringpool_str136[sizeof("enable-background")];
    char stringpool_str137[sizeof("pointer-events")];
    char stringpool_str138[sizeof("-webkit-columns")];
    char stringpool_str139[sizeof("-internal-marquee-direction")];
    char stringpool_str140[sizeof("-webkit-line-clamp")];
    char stringpool_str141[sizeof("background-blend-mode")];
    char stringpool_str142[sizeof("-internal-marquee-increment")];
    char stringpool_str143[sizeof("background-position")];
    char stringpool_str144[sizeof("color-interpolation")];
    char stringpool_str145[sizeof("-webkit-border-fit")];
    char stringpool_str146[sizeof("counter-increment")];
    char stringpool_str147[sizeof("text-indent")];
    char stringpool_str148[sizeof("-webkit-border-after")];
    char stringpool_str149[sizeof("font-kerning")];
    char stringpool_str150[sizeof("-webkit-column-break-inside")];
    char stringpool_str151[sizeof("grid-column-start")];
    char stringpool_str152[sizeof("-webkit-border-before")];
    char stringpool_str153[sizeof("transform")];
    char stringpool_str154[sizeof("border-collapse")];
    char stringpool_str155[sizeof("-webkit-user-select")];
    char stringpool_str156[sizeof("-epub-caption-side")];
    char stringpool_str157[sizeof("outline-color")];
    char stringpool_str158[sizeof("-webkit-box-orient")];
    char stringpool_str159[sizeof("-webkit-transform")];
    char stringpool_str160[sizeof("-webkit-margin-after")];
    char stringpool_str161[sizeof("-webkit-margin-before")];
    char stringpool_str162[sizeof("-webkit-column-gap")];
    char stringpool_str163[sizeof("text-rendering")];
    char stringpool_str164[sizeof("-webkit-text-orientation")];
    char stringpool_str165[sizeof("left")];
    char stringpool_str166[sizeof("float")];
    char stringpool_str167[sizeof("-webkit-column-span")];
    char stringpool_str168[sizeof("filter")];
    char stringpool_str169[sizeof("transform-origin")];
    char stringpool_str170[sizeof("-webkit-margin-collapse")];
    char stringpool_str171[sizeof("-webkit-text-stroke")];
    char stringpool_str172[sizeof("border-left")];
    char stringpool_str173[sizeof("-webkit-print-color-adjust")];
    char stringpool_str174[sizeof("-webkit-filter")];
    char stringpool_str175[sizeof("clip-rule")];
    char stringpool_str176[sizeof("-webkit-transform-origin")];
    char stringpool_str177[sizeof("-webkit-margin-bottom-collapse")];
    char stringpool_str178[sizeof("page-break-after")];
    char stringpool_str179[sizeof("page-break-before")];
    char stringpool_str180[sizeof("margin-left")];
    char stringpool_str181[sizeof("-webkit-padding-after")];
    char stringpool_str182[sizeof("-webkit-padding-before")];
    char stringpool_str183[sizeof("-webkit-mask-box-image")];
    char stringpool_str184[sizeof("mix-blend-mode")];
    char stringpool_str185[sizeof("background-color")];
    char stringpool_str186[sizeof("object-fit")];
    char stringpool_str187[sizeof("text-align")];
    char stringpool_str188[sizeof("width")];
    char stringpool_str189[sizeof("grid-auto-columns")];
    char stringpool_str190[sizeof("background-size")];
    char stringpool_str191[sizeof("fill")];
    char stringpool_str192[sizeof("-webkit-mask-repeat-x")];
    char stringpool_str193[sizeof("-webkit-box-align")];
    char stringpool_str194[sizeof("-webkit-margin-top-collapse")];
    char stringpool_str195[sizeof("min-width")];
    char stringpool_str196[sizeof("-webkit-box-lines")];
    char stringpool_str197[sizeof("-internal-callback")];
    char stringpool_str198[sizeof("border-width")];
    char stringpool_str199[sizeof("-webkit-wrap-flow")];
    char stringpool_str200[sizeof("right")];
    char stringpool_str201[sizeof("-webkit-background-size")];
    char stringpool_str202[sizeof("-webkit-background-composite")];
    char stringpool_str203[sizeof("text-decoration")];
    char stringpool_str204[sizeof("padding-left")];
    char stringpool_str205[sizeof("-webkit-column-rule")];
    char stringpool_str206[sizeof("perspective")];
    char stringpool_str207[sizeof("background-clip")];
    char stringpool_str208[sizeof("border-bottom-width")];
    char stringpool_str209[sizeof("-webkit-text-combine")];
    char stringpool_str210[sizeof("-webkit-mask-position-x")];
    char stringpool_str211[sizeof("-webkit-box-direction")];
    char stringpool_str212[sizeof("border-right")];
    char stringpool_str213[sizeof("grid-template-columns")];
    char stringpool_str214[sizeof("stroke-width")];
    char stringpool_str215[sizeof("-webkit-border-end-width")];
    char stringpool_str216[sizeof("animation-fill-mode")];
    char stringpool_str217[sizeof("-webkit-mask-box-image-repeat")];
    char stringpool_str218[sizeof("-webkit-perspective")];
    char stringpool_str219[sizeof("-webkit-background-clip")];
    char stringpool_str220[sizeof("border-image-width")];
    char stringpool_str221[sizeof("-webkit-box-decoration-break")];
    char stringpool_str222[sizeof("margin-right")];
    char stringpool_str223[sizeof("-webkit-animation-fill-mode")];
    char stringpool_str224[sizeof("align-self")];
    char stringpool_str225[sizeof("-webkit-column-count")];
    char stringpool_str226[sizeof("vertical-align")];
    char stringpool_str227[sizeof("perspective-origin")];
    char stringpool_str228[sizeof("z-index")];
    char stringpool_str229[sizeof("-webkit-border-start-width")];
    char stringpool_str230[sizeof("grid-auto-flow")];
    char stringpool_str231[sizeof("-epub-text-orientation")];
    char stringpool_str232[sizeof("-webkit-box-pack")];
    char stringpool_str233[sizeof("border-top-width")];
    char stringpool_str234[sizeof("font-size")];
    char stringpool_str235[sizeof("max-zoom")];
    char stringpool_str236[sizeof("-webkit-align-self")];
    char stringpool_str237[sizeof("orphans")];
    char stringpool_str238[sizeof("-webkit-perspective-origin")];
    char stringpool_str239[sizeof("text-underline-mode")];
    char stringpool_str240[sizeof("text-align-last")];
    char stringpool_str241[sizeof("-webkit-border-after-color")];
    char stringpool_str242[sizeof("-webkit-border-before-color")];
    char stringpool_str243[sizeof("text-decoration-line")];
    char stringpool_str244[sizeof("font-variant")];
    char stringpool_str245[sizeof("border-bottom-left-radius")];
    char stringpool_str246[sizeof("padding-right")];
    char stringpool_str247[sizeof("-webkit-line-box-contain")];
    char stringpool_str248[sizeof("-webkit-mask-box-image-outset")];
    char stringpool_str249[sizeof("shape-margin")];
    char stringpool_str250[sizeof("shape-rendering")];
    char stringpool_str251[sizeof("box-sizing")];
    char stringpool_str252[sizeof("-webkit-border-bottom-left-radius")];
    char stringpool_str253[sizeof("-webkit-shape-margin")];
    char stringpool_str254[sizeof("flood-color")];
    char stringpool_str255[sizeof("animation-timing-function")];
    char stringpool_str256[sizeof("-webkit-box-sizing")];
    char stringpool_str257[sizeof("-webkit-transform-origin-z")];
    char stringpool_str258[sizeof("-webkit-text-stroke-color")];
    char stringpool_str259[sizeof("border-left-color")];
    char stringpool_str260[sizeof("border-top-left-radius")];
    char stringpool_str261[sizeof("-webkit-animation-timing-function")];
    char stringpool_str262[sizeof("transition-timing-function")];
    char stringpool_str263[sizeof("-webkit-border-top-left-radius")];
    char stringpool_str264[sizeof("overflow")];
    char stringpool_str265[sizeof("text-underline-position")];
    char stringpool_str266[sizeof("-webkit-box-ordinal-group")];
    char stringpool_str267[sizeof("-epub-text-combine")];
    char stringpool_str268[sizeof("-webkit-font-size-delta")];
    char stringpool_str269[sizeof("outline-width")];
    char stringpool_str270[sizeof("fill-rule")];
    char stringpool_str271[sizeof("-webkit-transition-timing-function")];
    char stringpool_str272[sizeof("-webkit-column-break-after")];
    char stringpool_str273[sizeof("border-bottom-right-radius")];
    char stringpool_str274[sizeof("-webkit-column-break-before")];
    char stringpool_str275[sizeof("-webkit-mask-box-image-slice")];
    char stringpool_str276[sizeof("white-space")];
    char stringpool_str277[sizeof("background-repeat-x")];
    char stringpool_str278[sizeof("mask-type")];
    char stringpool_str279[sizeof("-webkit-border-bottom-right-radius")];
    char stringpool_str280[sizeof("animation-delay")];
    char stringpool_str281[sizeof("shape-outside")];
    char stringpool_str282[sizeof("text-overline-mode")];
    char stringpool_str283[sizeof("text-decoration-color")];
    char stringpool_str284[sizeof("-webkit-column-rule-color")];
    char stringpool_str285[sizeof("-webkit-mask-box-image-source")];
    char stringpool_str286[sizeof("-webkit-border-vertical-spacing")];
    char stringpool_str287[sizeof("-webkit-animation-delay")];
    char stringpool_str288[sizeof("-webkit-shape-outside")];
    char stringpool_str289[sizeof("background-position-x")];
    char stringpool_str290[sizeof("border-style")];
    char stringpool_str291[sizeof("-webkit-mask-repeat-y")];
    char stringpool_str292[sizeof("border-right-color")];
    char stringpool_str293[sizeof("overflow-wrap")];
    char stringpool_str294[sizeof("border-top-right-radius")];
    char stringpool_str295[sizeof("transition-delay")];
    char stringpool_str296[sizeof("border-bottom-style")];
    char stringpool_str297[sizeof("-webkit-border-top-right-radius")];
    char stringpool_str298[sizeof("text-transform")];
    char stringpool_str299[sizeof("-webkit-transition-delay")];
    char stringpool_str300[sizeof("-webkit-border-end-style")];
    char stringpool_str301[sizeof("-webkit-mask-position-y")];
    char stringpool_str302[sizeof("display")];
    char stringpool_str303[sizeof("text-underline-color")];
    char stringpool_str304[sizeof("flex")];
    char stringpool_str305[sizeof("-webkit-margin-after-collapse")];
    char stringpool_str306[sizeof("-webkit-margin-before-collapse")];
    char stringpool_str307[sizeof("will-change")];
    char stringpool_str308[sizeof("transition-property")];
    char stringpool_str309[sizeof("opacity")];
    char stringpool_str310[sizeof("-webkit-border-start-style")];
    char stringpool_str311[sizeof("-webkit-flex")];
    char stringpool_str312[sizeof("clip-path")];
    char stringpool_str313[sizeof("border-top-style")];
    char stringpool_str314[sizeof("column-fill")];
    char stringpool_str315[sizeof("-webkit-transition-property")];
    char stringpool_str316[sizeof("animation-play-state")];
    char stringpool_str317[sizeof("-webkit-opacity")];
    char stringpool_str318[sizeof("-webkit-transform-origin-x")];
    char stringpool_str319[sizeof("-webkit-logical-width")];
    char stringpool_str320[sizeof("flex-grow")];
    char stringpool_str321[sizeof("-webkit-column-width")];
    char stringpool_str322[sizeof("-webkit-clip-path")];
    char stringpool_str323[sizeof("lighting-color")];
    char stringpool_str324[sizeof("-webkit-min-logical-width")];
    char stringpool_str325[sizeof("-webkit-animation-play-state")];
    char stringpool_str326[sizeof("list-style")];
    char stringpool_str327[sizeof("color-interpolation-filters")];
    char stringpool_str328[sizeof("-webkit-flex-grow")];
    char stringpool_str329[sizeof("-webkit-ruby-position")];
    char stringpool_str330[sizeof("buffered-rendering")];
    char stringpool_str331[sizeof("stroke-opacity")];
    char stringpool_str332[sizeof("touch-action")];
    char stringpool_str333[sizeof("flex-basis")];
    char stringpool_str334[sizeof("flex-wrap")];
    char stringpool_str335[sizeof("font-variant-ligatures")];
    char stringpool_str336[sizeof("font-weight")];
    char stringpool_str337[sizeof("max-width")];
    char stringpool_str338[sizeof("-webkit-flex-basis")];
    char stringpool_str339[sizeof("-webkit-flex-wrap")];
    char stringpool_str340[sizeof("list-style-image")];
    char stringpool_str341[sizeof("-webkit-border-after-width")];
    char stringpool_str342[sizeof("-webkit-border-before-width")];
    char stringpool_str343[sizeof("table-layout")];
    char stringpool_str344[sizeof("background-attachment")];
    char stringpool_str345[sizeof("box-shadow")];
    char stringpool_str346[sizeof("text-shadow")];
    char stringpool_str347[sizeof("stop-opacity")];
    char stringpool_str348[sizeof("-webkit-box-shadow")];
    char stringpool_str349[sizeof("outline-offset")];
    char stringpool_str350[sizeof("outline-style")];
    char stringpool_str351[sizeof("-webkit-font-smoothing")];
    char stringpool_str352[sizeof("-webkit-font-feature-settings")];
    char stringpool_str353[sizeof("text-overline-color")];
    char stringpool_str354[sizeof("-webkit-text-stroke-width")];
    char stringpool_str355[sizeof("flex-direction")];
    char stringpool_str356[sizeof("border-left-width")];
    char stringpool_str357[sizeof("-epub-text-transform")];
    char stringpool_str358[sizeof("-webkit-box-reflect")];
    char stringpool_str359[sizeof("list-style-position")];
    char stringpool_str360[sizeof("-webkit-flex-direction")];
    char stringpool_str361[sizeof("-internal-marquee-style")];
    char stringpool_str362[sizeof("background-repeat-y")];
    char stringpool_str363[sizeof("-webkit-perspective-origin-x")];
    char stringpool_str364[sizeof("-webkit-mask-box-image-width")];
    char stringpool_str365[sizeof("visibility")];
    char stringpool_str366[sizeof("baseline-shift")];
    char stringpool_str367[sizeof("mask-source-type")];
    char stringpool_str368[sizeof("background-position-y")];
    char stringpool_str369[sizeof("text-anchor")];
    char stringpool_str370[sizeof("empty-cells")];
    char stringpool_str371[sizeof("font-stretch")];
    char stringpool_str372[sizeof("height")];
    char stringpool_str373[sizeof("-webkit-text-emphasis")];
    char stringpool_str374[sizeof("min-height")];
    char stringpool_str375[sizeof("-webkit-column-rule-width")];
    char stringpool_str376[sizeof("border-right-width")];
    char stringpool_str377[sizeof("overflow-x")];
    char stringpool_str378[sizeof("text-overflow")];
    char stringpool_str379[sizeof("-webkit-border-horizontal-spacing")];
    char stringpool_str380[sizeof("scroll-behavior")];
    char stringpool_str381[sizeof("-webkit-transform-origin-y")];
    char stringpool_str382[sizeof("text-underline-width")];
    char stringpool_str383[sizeof("font-style")];
    char stringpool_str384[sizeof("line-height")];
    char stringpool_str385[sizeof("-webkit-text-fill-color")];
    char stringpool_str386[sizeof("-webkit-text-emphasis-position")];
    char stringpool_str387[sizeof("-webkit-border-after-style")];
    char stringpool_str388[sizeof("-webkit-border-before-style")];
    char stringpool_str389[sizeof("transform-style")];
    char stringpool_str390[sizeof("-webkit-user-modify")];
    char stringpool_str391[sizeof("-webkit-transform-style")];
    char stringpool_str392[sizeof("-epub-text-emphasis")];
    char stringpool_str393[sizeof("border-left-style")];
    char stringpool_str394[sizeof("-webkit-box-flex")];
    char stringpool_str395[sizeof("stroke-dasharray")];
    char stringpool_str396[sizeof("vector-effect")];
    char stringpool_str397[sizeof("-webkit-wrap-through")];
    char stringpool_str398[sizeof("-webkit-max-logical-width")];
    char stringpool_str399[sizeof("-webkit-perspective-origin-y")];
    char stringpool_str400[sizeof("text-overline-width")];
    char stringpool_str401[sizeof("flex-flow")];
    char stringpool_str402[sizeof("-webkit-text-emphasis-color")];
    char stringpool_str403[sizeof("flood-opacity")];
    char stringpool_str404[sizeof("justify-content")];
    char stringpool_str405[sizeof("-webkit-flex-flow")];
    char stringpool_str406[sizeof("text-decoration-style")];
    char stringpool_str407[sizeof("-webkit-column-rule-style")];
    char stringpool_str408[sizeof("-webkit-justify-content")];
    char stringpool_str409[sizeof("border-right-style")];
    char stringpool_str410[sizeof("-webkit-text-security")];
    char stringpool_str411[sizeof("stroke-dashoffset")];
    char stringpool_str412[sizeof("overflow-y")];
    char stringpool_str413[sizeof("text-underline-style")];
    char stringpool_str414[sizeof("fill-opacity")];
    char stringpool_str415[sizeof("flex-shrink")];
    char stringpool_str416[sizeof("-webkit-logical-height")];
    char stringpool_str417[sizeof("-webkit-flex-shrink")];
    char stringpool_str418[sizeof("-webkit-min-logical-height")];
    char stringpool_str419[sizeof("max-height")];
    char stringpool_str420[sizeof("-epub-text-emphasis-color")];
    char stringpool_str421[sizeof("-webkit-box-flex-group")];
    char stringpool_str422[sizeof("-webkit-text-decorations-in-effect")];
    char stringpool_str423[sizeof("font-family")];
    char stringpool_str424[sizeof("text-overline-style")];
    char stringpool_str425[sizeof("text-justify")];
    char stringpool_str426[sizeof("list-style-type")];
    char stringpool_str427[sizeof("touch-action-delay")];
    char stringpool_str428[sizeof("justify-self")];
    char stringpool_str429[sizeof("backface-visibility")];
    char stringpool_str430[sizeof("text-line-through-mode")];
    char stringpool_str431[sizeof("-webkit-backface-visibility")];
    char stringpool_str432[sizeof("-webkit-highlight")];
    char stringpool_str433[sizeof("-webkit-max-logical-height")];
    char stringpool_str434[sizeof("shape-image-threshold")];
    char stringpool_str435[sizeof("-webkit-shape-image-threshold")];
    char stringpool_str436[sizeof("-webkit-text-emphasis-style")];
    char stringpool_str437[sizeof("glyph-orientation-vertical")];
    char stringpool_str438[sizeof("text-line-through-color")];
    char stringpool_str439[sizeof("-webkit-tap-highlight-color")];
    char stringpool_str440[sizeof("-epub-text-emphasis-style")];
    char stringpool_str441[sizeof("text-line-through-width")];
    char stringpool_str442[sizeof("glyph-orientation-horizontal")];
    char stringpool_str443[sizeof("text-line-through-style")];
    char stringpool_str444[sizeof("-webkit-hyphenate-character")];
  };
static const struct stringpool_t stringpool_contents =
  {
    "order",
    "border",
    "bottom",
    "marker",
    "animation",
    "word-break",
    "orientation",
    "marker-end",
    "marker-mid",
    "grid",
    "-webkit-order",
    "border-bottom",
    "animation-name",
    "mask",
    "margin",
    "widows",
    "-webkit-animation",
    "stroke",
    "grid-row",
    "-webkit-border-end",
    "grid-area",
    "transition",
    "grid-row-end",
    "writing-mode",
    "border-image",
    "-webkit-animation-name",
    "-webkit-mask",
    "margin-bottom",
    "marker-start",
    "top",
    "-webkit-margin-end",
    "-webkit-transition",
    "-webkit-writing-mode",
    "-webkit-border-image",
    "-webkit-border-start",
    "word-wrap",
    "border-top",
    "image-rendering",
    "paint-order",
    "grid-row-start",
    "page",
    "padding",
    "speak",
    "-webkit-mask-image",
    "-webkit-mask-origin",
    "-webkit-margin-start",
    "position",
    "margin-top",
    "padding-bottom",
    "-webkit-line-break",
    "-webkit-padding-end",
    "isolation",
    "border-image-repeat",
    "-webkit-mask-repeat",
    "content",
    "direction",
    "stroke-linejoin",
    "dominant-baseline",
    "page-break-inside",
    "quotes",
    "animation-duration",
    "-webkit-rtl-ordering",
    "stroke-miterlimit",
    "align-items",
    "src",
    "-webkit-padding-start",
    "all",
    "padding-top",
    "border-radius",
    "-webkit-mask-position",
    "-webkit-animation-duration",
    "animation-direction",
    "-webkit-align-items",
    "transition-duration",
    "-webkit-app-region",
    "-webkit-border-radius",
    "-webkit-animation-direction",
    "grid-auto-rows",
    "grid-template",
    "-webkit-user-drag",
    "outline",
    "-webkit-transition-duration",
    "-epub-word-break",
    "border-image-outset",
    "zoom",
    "clear",
    "color",
    "min-zoom",
    "-epub-writing-mode",
    "size",
    "border-color",
    "grid-template-rows",
    "caption-side",
    "grid-template-areas",
    "resize",
    "tab-size",
    "object-position",
    "alignment-baseline",
    "align-content",
    "unicode-bidi",
    "border-bottom-color",
    "-webkit-aspect-ratio",
    "word-spacing",
    "-webkit-border-end-color",
    "color-rendering",
    "clip",
    "border-spacing",
    "cursor",
    "-webkit-align-content",
    "background",
    "unicode-range",
    "-webkit-appearance",
    "counter-reset",
    "animation-iteration-count",
    "-webkit-mask-size",
    "-webkit-mask-composite",
    "-webkit-border-start-color",
    "border-image-slice",
    "border-top-color",
    "-webkit-animation-iteration-count",
    "-webkit-locale",
    "stop-color",
    "background-image",
    "background-origin",
    "-internal-marquee-repetition",
    "stroke-linecap",
    "-webkit-mask-clip",
    "border-image-source",
    "-internal-marquee-speed",
    "-webkit-background-origin",
    "letter-spacing",
    "background-repeat",
    "user-zoom",
    "font",
    "grid-column",
    "grid-column-end",
    "enable-background",
    "pointer-events",
    "-webkit-columns",
    "-internal-marquee-direction",
    "-webkit-line-clamp",
    "background-blend-mode",
    "-internal-marquee-increment",
    "background-position",
    "color-interpolation",
    "-webkit-border-fit",
    "counter-increment",
    "text-indent",
    "-webkit-border-after",
    "font-kerning",
    "-webkit-column-break-inside",
    "grid-column-start",
    "-webkit-border-before",
    "transform",
    "border-collapse",
    "-webkit-user-select",
    "-epub-caption-side",
    "outline-color",
    "-webkit-box-orient",
    "-webkit-transform",
    "-webkit-margin-after",
    "-webkit-margin-before",
    "-webkit-column-gap",
    "text-rendering",
    "-webkit-text-orientation",
    "left",
    "float",
    "-webkit-column-span",
    "filter",
    "transform-origin",
    "-webkit-margin-collapse",
    "-webkit-text-stroke",
    "border-left",
    "-webkit-print-color-adjust",
    "-webkit-filter",
    "clip-rule",
    "-webkit-transform-origin",
    "-webkit-margin-bottom-collapse",
    "page-break-after",
    "page-break-before",
    "margin-left",
    "-webkit-padding-after",
    "-webkit-padding-before",
    "-webkit-mask-box-image",
    "mix-blend-mode",
    "background-color",
    "object-fit",
    "text-align",
    "width",
    "grid-auto-columns",
    "background-size",
    "fill",
    "-webkit-mask-repeat-x",
    "-webkit-box-align",
    "-webkit-margin-top-collapse",
    "min-width",
    "-webkit-box-lines",
    "-internal-callback",
    "border-width",
    "-webkit-wrap-flow",
    "right",
    "-webkit-background-size",
    "-webkit-background-composite",
    "text-decoration",
    "padding-left",
    "-webkit-column-rule",
    "perspective",
    "background-clip",
    "border-bottom-width",
    "-webkit-text-combine",
    "-webkit-mask-position-x",
    "-webkit-box-direction",
    "border-right",
    "grid-template-columns",
    "stroke-width",
    "-webkit-border-end-width",
    "animation-fill-mode",
    "-webkit-mask-box-image-repeat",
    "-webkit-perspective",
    "-webkit-background-clip",
    "border-image-width",
    "-webkit-box-decoration-break",
    "margin-right",
    "-webkit-animation-fill-mode",
    "align-self",
    "-webkit-column-count",
    "vertical-align",
    "perspective-origin",
    "z-index",
    "-webkit-border-start-width",
    "grid-auto-flow",
    "-epub-text-orientation",
    "-webkit-box-pack",
    "border-top-width",
    "font-size",
    "max-zoom",
    "-webkit-align-self",
    "orphans",
    "-webkit-perspective-origin",
    "text-underline-mode",
    "text-align-last",
    "-webkit-border-after-color",
    "-webkit-border-before-color",
    "text-decoration-line",
    "font-variant",
    "border-bottom-left-radius",
    "padding-right",
    "-webkit-line-box-contain",
    "-webkit-mask-box-image-outset",
    "shape-margin",
    "shape-rendering",
    "box-sizing",
    "-webkit-border-bottom-left-radius",
    "-webkit-shape-margin",
    "flood-color",
    "animation-timing-function",
    "-webkit-box-sizing",
    "-webkit-transform-origin-z",
    "-webkit-text-stroke-color",
    "border-left-color",
    "border-top-left-radius",
    "-webkit-animation-timing-function",
    "transition-timing-function",
    "-webkit-border-top-left-radius",
    "overflow",
    "text-underline-position",
    "-webkit-box-ordinal-group",
    "-epub-text-combine",
    "-webkit-font-size-delta",
    "outline-width",
    "fill-rule",
    "-webkit-transition-timing-function",
    "-webkit-column-break-after",
    "border-bottom-right-radius",
    "-webkit-column-break-before",
    "-webkit-mask-box-image-slice",
    "white-space",
    "background-repeat-x",
    "mask-type",
    "-webkit-border-bottom-right-radius",
    "animation-delay",
    "shape-outside",
    "text-overline-mode",
    "text-decoration-color",
    "-webkit-column-rule-color",
    "-webkit-mask-box-image-source",
    "-webkit-border-vertical-spacing",
    "-webkit-animation-delay",
    "-webkit-shape-outside",
    "background-position-x",
    "border-style",
    "-webkit-mask-repeat-y",
    "border-right-color",
    "overflow-wrap",
    "border-top-right-radius",
    "transition-delay",
    "border-bottom-style",
    "-webkit-border-top-right-radius",
    "text-transform",
    "-webkit-transition-delay",
    "-webkit-border-end-style",
    "-webkit-mask-position-y",
    "display",
    "text-underline-color",
    "flex",
    "-webkit-margin-after-collapse",
    "-webkit-margin-before-collapse",
    "will-change",
    "transition-property",
    "opacity",
    "-webkit-border-start-style",
    "-webkit-flex",
    "clip-path",
    "border-top-style",
    "column-fill",
    "-webkit-transition-property",
    "animation-play-state",
    "-webkit-opacity",
    "-webkit-transform-origin-x",
    "-webkit-logical-width",
    "flex-grow",
    "-webkit-column-width",
    "-webkit-clip-path",
    "lighting-color",
    "-webkit-min-logical-width",
    "-webkit-animation-play-state",
    "list-style",
    "color-interpolation-filters",
    "-webkit-flex-grow",
    "-webkit-ruby-position",
    "buffered-rendering",
    "stroke-opacity",
    "touch-action",
    "flex-basis",
    "flex-wrap",
    "font-variant-ligatures",
    "font-weight",
    "max-width",
    "-webkit-flex-basis",
    "-webkit-flex-wrap",
    "list-style-image",
    "-webkit-border-after-width",
    "-webkit-border-before-width",
    "table-layout",
    "background-attachment",
    "box-shadow",
    "text-shadow",
    "stop-opacity",
    "-webkit-box-shadow",
    "outline-offset",
    "outline-style",
    "-webkit-font-smoothing",
    "-webkit-font-feature-settings",
    "text-overline-color",
    "-webkit-text-stroke-width",
    "flex-direction",
    "border-left-width",
    "-epub-text-transform",
    "-webkit-box-reflect",
    "list-style-position",
    "-webkit-flex-direction",
    "-internal-marquee-style",
    "background-repeat-y",
    "-webkit-perspective-origin-x",
    "-webkit-mask-box-image-width",
    "visibility",
    "baseline-shift",
    "mask-source-type",
    "background-position-y",
    "text-anchor",
    "empty-cells",
    "font-stretch",
    "height",
    "-webkit-text-emphasis",
    "min-height",
    "-webkit-column-rule-width",
    "border-right-width",
    "overflow-x",
    "text-overflow",
    "-webkit-border-horizontal-spacing",
    "scroll-behavior",
    "-webkit-transform-origin-y",
    "text-underline-width",
    "font-style",
    "line-height",
    "-webkit-text-fill-color",
    "-webkit-text-emphasis-position",
    "-webkit-border-after-style",
    "-webkit-border-before-style",
    "transform-style",
    "-webkit-user-modify",
    "-webkit-transform-style",
    "-epub-text-emphasis",
    "border-left-style",
    "-webkit-box-flex",
    "stroke-dasharray",
    "vector-effect",
    "-webkit-wrap-through",
    "-webkit-max-logical-width",
    "-webkit-perspective-origin-y",
    "text-overline-width",
    "flex-flow",
    "-webkit-text-emphasis-color",
    "flood-opacity",
    "justify-content",
    "-webkit-flex-flow",
    "text-decoration-style",
    "-webkit-column-rule-style",
    "-webkit-justify-content",
    "border-right-style",
    "-webkit-text-security",
    "stroke-dashoffset",
    "overflow-y",
    "text-underline-style",
    "fill-opacity",
    "flex-shrink",
    "-webkit-logical-height",
    "-webkit-flex-shrink",
    "-webkit-min-logical-height",
    "max-height",
    "-epub-text-emphasis-color",
    "-webkit-box-flex-group",
    "-webkit-text-decorations-in-effect",
    "font-family",
    "text-overline-style",
    "text-justify",
    "list-style-type",
    "touch-action-delay",
    "justify-self",
    "backface-visibility",
    "text-line-through-mode",
    "-webkit-backface-visibility",
    "-webkit-highlight",
    "-webkit-max-logical-height",
    "shape-image-threshold",
    "-webkit-shape-image-threshold",
    "-webkit-text-emphasis-style",
    "glyph-orientation-vertical",
    "text-line-through-color",
    "-webkit-tap-highlight-color",
    "-epub-text-emphasis-style",
    "text-line-through-width",
    "glyph-orientation-horizontal",
    "text-line-through-style",
    "-webkit-hyphenate-character"
  };
#define stringpool ((const char *) &stringpool_contents)

static const struct Property property_word_list[] =
  {
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str0, CSSPropertyOrder},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str1, CSSPropertyBorder},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str2, CSSPropertyBottom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str3, CSSPropertyMarker},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str4, CSSPropertyAnimation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str5, CSSPropertyWordBreak},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str6, CSSPropertyOrientation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str7, CSSPropertyMarkerEnd},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str8, CSSPropertyMarkerMid},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str9, CSSPropertyGrid},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str10, CSSPropertyOrder},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str11, CSSPropertyBorderBottom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str12, CSSPropertyAnimationName},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str13, CSSPropertyMask},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str14, CSSPropertyMargin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str15, CSSPropertyWidows},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str16, CSSPropertyWebkitAnimation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str17, CSSPropertyStroke},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str18, CSSPropertyGridRow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str19, CSSPropertyWebkitBorderEnd},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str20, CSSPropertyGridArea},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str21, CSSPropertyTransition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str22, CSSPropertyGridRowEnd},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str23, CSSPropertyWritingMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str24, CSSPropertyBorderImage},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str25, CSSPropertyWebkitAnimationName},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str26, CSSPropertyWebkitMask},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str27, CSSPropertyMarginBottom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str28, CSSPropertyMarkerStart},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str29, CSSPropertyTop},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str30, CSSPropertyWebkitMarginEnd},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str31, CSSPropertyWebkitTransition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str32, CSSPropertyWebkitWritingMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str33, CSSPropertyWebkitBorderImage},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str34, CSSPropertyWebkitBorderStart},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str35, CSSPropertyWordWrap},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str36, CSSPropertyBorderTop},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str37, CSSPropertyImageRendering},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str38, CSSPropertyPaintOrder},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str39, CSSPropertyGridRowStart},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str40, CSSPropertyPage},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str41, CSSPropertyPadding},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str42, CSSPropertySpeak},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str43, CSSPropertyWebkitMaskImage},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str44, CSSPropertyWebkitMaskOrigin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str45, CSSPropertyWebkitMarginStart},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str46, CSSPropertyPosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str47, CSSPropertyMarginTop},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str48, CSSPropertyPaddingBottom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str49, CSSPropertyWebkitLineBreak},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str50, CSSPropertyWebkitPaddingEnd},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str51, CSSPropertyIsolation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str52, CSSPropertyBorderImageRepeat},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str53, CSSPropertyWebkitMaskRepeat},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str54, CSSPropertyContent},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str55, CSSPropertyDirection},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str56, CSSPropertyStrokeLinejoin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str57, CSSPropertyDominantBaseline},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str58, CSSPropertyPageBreakInside},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str59, CSSPropertyQuotes},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str60, CSSPropertyAnimationDuration},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str61, CSSPropertyWebkitRtlOrdering},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str62, CSSPropertyStrokeMiterlimit},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str63, CSSPropertyAlignItems},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str64, CSSPropertySrc},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str65, CSSPropertyWebkitPaddingStart},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str66, CSSPropertyAll},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str67, CSSPropertyPaddingTop},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str68, CSSPropertyBorderRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str69, CSSPropertyWebkitMaskPosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str70, CSSPropertyWebkitAnimationDuration},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str71, CSSPropertyAnimationDirection},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str72, CSSPropertyAlignItems},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str73, CSSPropertyTransitionDuration},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str74, CSSPropertyWebkitAppRegion},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str75, CSSPropertyWebkitBorderRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str76, CSSPropertyWebkitAnimationDirection},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str77, CSSPropertyGridAutoRows},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str78, CSSPropertyGridTemplate},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str79, CSSPropertyWebkitUserDrag},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str80, CSSPropertyOutline},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str81, CSSPropertyWebkitTransitionDuration},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str82, CSSPropertyWordBreak},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str83, CSSPropertyBorderImageOutset},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str84, CSSPropertyZoom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str85, CSSPropertyClear},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str86, CSSPropertyColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str87, CSSPropertyMinZoom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str88, CSSPropertyWebkitWritingMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str89, CSSPropertySize},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str90, CSSPropertyBorderColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str91, CSSPropertyGridTemplateRows},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str92, CSSPropertyCaptionSide},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str93, CSSPropertyGridTemplateAreas},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str94, CSSPropertyResize},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str95, CSSPropertyTabSize},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str96, CSSPropertyObjectPosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str97, CSSPropertyAlignmentBaseline},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str98, CSSPropertyAlignContent},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str99, CSSPropertyUnicodeBidi},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str100, CSSPropertyBorderBottomColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str101, CSSPropertyWebkitAspectRatio},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str102, CSSPropertyWordSpacing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str103, CSSPropertyWebkitBorderEndColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str104, CSSPropertyColorRendering},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str105, CSSPropertyClip},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str106, CSSPropertyBorderSpacing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str107, CSSPropertyCursor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str108, CSSPropertyAlignContent},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str109, CSSPropertyBackground},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str110, CSSPropertyUnicodeRange},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str111, CSSPropertyWebkitAppearance},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str112, CSSPropertyCounterReset},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str113, CSSPropertyAnimationIterationCount},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str114, CSSPropertyWebkitMaskSize},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str115, CSSPropertyWebkitMaskComposite},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str116, CSSPropertyWebkitBorderStartColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str117, CSSPropertyBorderImageSlice},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str118, CSSPropertyBorderTopColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str119, CSSPropertyWebkitAnimationIterationCount},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str120, CSSPropertyWebkitLocale},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str121, CSSPropertyStopColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str122, CSSPropertyBackgroundImage},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str123, CSSPropertyBackgroundOrigin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str124, CSSPropertyInternalMarqueeRepetition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str125, CSSPropertyStrokeLinecap},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str126, CSSPropertyWebkitMaskClip},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str127, CSSPropertyBorderImageSource},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str128, CSSPropertyInternalMarqueeSpeed},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str129, CSSPropertyWebkitBackgroundOrigin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str130, CSSPropertyLetterSpacing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str131, CSSPropertyBackgroundRepeat},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str132, CSSPropertyUserZoom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str133, CSSPropertyFont},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str134, CSSPropertyGridColumn},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str135, CSSPropertyGridColumnEnd},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str136, CSSPropertyEnableBackground},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str137, CSSPropertyPointerEvents},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str138, CSSPropertyWebkitColumns},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str139, CSSPropertyInternalMarqueeDirection},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str140, CSSPropertyWebkitLineClamp},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str141, CSSPropertyBackgroundBlendMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str142, CSSPropertyInternalMarqueeIncrement},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str143, CSSPropertyBackgroundPosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str144, CSSPropertyColorInterpolation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str145, CSSPropertyWebkitBorderFit},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str146, CSSPropertyCounterIncrement},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str147, CSSPropertyTextIndent},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str148, CSSPropertyWebkitBorderAfter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str149, CSSPropertyFontKerning},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str150, CSSPropertyWebkitColumnBreakInside},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str151, CSSPropertyGridColumnStart},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str152, CSSPropertyWebkitBorderBefore},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str153, CSSPropertyTransform},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str154, CSSPropertyBorderCollapse},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str155, CSSPropertyWebkitUserSelect},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str156, CSSPropertyCaptionSide},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str157, CSSPropertyOutlineColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str158, CSSPropertyWebkitBoxOrient},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str159, CSSPropertyWebkitTransform},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str160, CSSPropertyWebkitMarginAfter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str161, CSSPropertyWebkitMarginBefore},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str162, CSSPropertyWebkitColumnGap},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str163, CSSPropertyTextRendering},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str164, CSSPropertyWebkitTextOrientation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str165, CSSPropertyLeft},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str166, CSSPropertyFloat},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str167, CSSPropertyWebkitColumnSpan},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str168, CSSPropertyFilter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str169, CSSPropertyTransformOrigin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str170, CSSPropertyWebkitMarginCollapse},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str171, CSSPropertyWebkitTextStroke},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str172, CSSPropertyBorderLeft},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str173, CSSPropertyWebkitPrintColorAdjust},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str174, CSSPropertyWebkitFilter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str175, CSSPropertyClipRule},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str176, CSSPropertyWebkitTransformOrigin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str177, CSSPropertyWebkitMarginBottomCollapse},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str178, CSSPropertyPageBreakAfter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str179, CSSPropertyPageBreakBefore},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str180, CSSPropertyMarginLeft},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str181, CSSPropertyWebkitPaddingAfter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str182, CSSPropertyWebkitPaddingBefore},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str183, CSSPropertyWebkitMaskBoxImage},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str184, CSSPropertyMixBlendMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str185, CSSPropertyBackgroundColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str186, CSSPropertyObjectFit},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str187, CSSPropertyTextAlign},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str188, CSSPropertyWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str189, CSSPropertyGridAutoColumns},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str190, CSSPropertyBackgroundSize},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str191, CSSPropertyFill},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str192, CSSPropertyWebkitMaskRepeatX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str193, CSSPropertyWebkitBoxAlign},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str194, CSSPropertyWebkitMarginTopCollapse},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str195, CSSPropertyMinWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str196, CSSPropertyWebkitBoxLines},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str197, CSSPropertyInternalCallback},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str198, CSSPropertyBorderWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str199, CSSPropertyWebkitWrapFlow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str200, CSSPropertyRight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str201, CSSPropertyWebkitBackgroundSize},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str202, CSSPropertyWebkitBackgroundComposite},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str203, CSSPropertyTextDecoration},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str204, CSSPropertyPaddingLeft},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str205, CSSPropertyWebkitColumnRule},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str206, CSSPropertyPerspective},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str207, CSSPropertyBackgroundClip},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str208, CSSPropertyBorderBottomWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str209, CSSPropertyWebkitTextCombine},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str210, CSSPropertyWebkitMaskPositionX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str211, CSSPropertyWebkitBoxDirection},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str212, CSSPropertyBorderRight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str213, CSSPropertyGridTemplateColumns},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str214, CSSPropertyStrokeWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str215, CSSPropertyWebkitBorderEndWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str216, CSSPropertyAnimationFillMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str217, CSSPropertyWebkitMaskBoxImageRepeat},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str218, CSSPropertyWebkitPerspective},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str219, CSSPropertyWebkitBackgroundClip},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str220, CSSPropertyBorderImageWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str221, CSSPropertyWebkitBoxDecorationBreak},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str222, CSSPropertyMarginRight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str223, CSSPropertyWebkitAnimationFillMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str224, CSSPropertyAlignSelf},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str225, CSSPropertyWebkitColumnCount},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str226, CSSPropertyVerticalAlign},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str227, CSSPropertyPerspectiveOrigin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str228, CSSPropertyZIndex},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str229, CSSPropertyWebkitBorderStartWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str230, CSSPropertyGridAutoFlow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str231, CSSPropertyWebkitTextOrientation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str232, CSSPropertyWebkitBoxPack},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str233, CSSPropertyBorderTopWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str234, CSSPropertyFontSize},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str235, CSSPropertyMaxZoom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str236, CSSPropertyAlignSelf},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str237, CSSPropertyOrphans},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str238, CSSPropertyWebkitPerspectiveOrigin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str239, CSSPropertyTextUnderlineMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str240, CSSPropertyTextAlignLast},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str241, CSSPropertyWebkitBorderAfterColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str242, CSSPropertyWebkitBorderBeforeColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str243, CSSPropertyTextDecorationLine},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str244, CSSPropertyFontVariant},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str245, CSSPropertyBorderBottomLeftRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str246, CSSPropertyPaddingRight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str247, CSSPropertyWebkitLineBoxContain},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str248, CSSPropertyWebkitMaskBoxImageOutset},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str249, CSSPropertyShapeMargin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str250, CSSPropertyShapeRendering},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str251, CSSPropertyBoxSizing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str252, CSSPropertyBorderBottomLeftRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str253, CSSPropertyShapeMargin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str254, CSSPropertyFloodColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str255, CSSPropertyAnimationTimingFunction},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str256, CSSPropertyBoxSizing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str257, CSSPropertyWebkitTransformOriginZ},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str258, CSSPropertyWebkitTextStrokeColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str259, CSSPropertyBorderLeftColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str260, CSSPropertyBorderTopLeftRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str261, CSSPropertyWebkitAnimationTimingFunction},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str262, CSSPropertyTransitionTimingFunction},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str263, CSSPropertyBorderTopLeftRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str264, CSSPropertyOverflow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str265, CSSPropertyTextUnderlinePosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str266, CSSPropertyWebkitBoxOrdinalGroup},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str267, CSSPropertyWebkitTextCombine},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str268, CSSPropertyWebkitFontSizeDelta},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str269, CSSPropertyOutlineWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str270, CSSPropertyFillRule},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str271, CSSPropertyWebkitTransitionTimingFunction},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str272, CSSPropertyWebkitColumnBreakAfter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str273, CSSPropertyBorderBottomRightRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str274, CSSPropertyWebkitColumnBreakBefore},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str275, CSSPropertyWebkitMaskBoxImageSlice},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str276, CSSPropertyWhiteSpace},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str277, CSSPropertyBackgroundRepeatX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str278, CSSPropertyMaskType},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str279, CSSPropertyBorderBottomRightRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str280, CSSPropertyAnimationDelay},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str281, CSSPropertyShapeOutside},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str282, CSSPropertyTextOverlineMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str283, CSSPropertyTextDecorationColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str284, CSSPropertyWebkitColumnRuleColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str285, CSSPropertyWebkitMaskBoxImageSource},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str286, CSSPropertyWebkitBorderVerticalSpacing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str287, CSSPropertyWebkitAnimationDelay},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str288, CSSPropertyShapeOutside},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str289, CSSPropertyBackgroundPositionX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str290, CSSPropertyBorderStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str291, CSSPropertyWebkitMaskRepeatY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str292, CSSPropertyBorderRightColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str293, CSSPropertyOverflowWrap},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str294, CSSPropertyBorderTopRightRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str295, CSSPropertyTransitionDelay},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str296, CSSPropertyBorderBottomStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str297, CSSPropertyBorderTopRightRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str298, CSSPropertyTextTransform},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str299, CSSPropertyWebkitTransitionDelay},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str300, CSSPropertyWebkitBorderEndStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str301, CSSPropertyWebkitMaskPositionY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str302, CSSPropertyDisplay},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str303, CSSPropertyTextUnderlineColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str304, CSSPropertyFlex},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str305, CSSPropertyWebkitMarginAfterCollapse},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str306, CSSPropertyWebkitMarginBeforeCollapse},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str307, CSSPropertyWillChange},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str308, CSSPropertyTransitionProperty},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str309, CSSPropertyOpacity},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str310, CSSPropertyWebkitBorderStartStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str311, CSSPropertyFlex},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str312, CSSPropertyClipPath},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str313, CSSPropertyBorderTopStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str314, CSSPropertyColumnFill},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str315, CSSPropertyWebkitTransitionProperty},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str316, CSSPropertyAnimationPlayState},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str317, CSSPropertyOpacity},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str318, CSSPropertyWebkitTransformOriginX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str319, CSSPropertyWebkitLogicalWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str320, CSSPropertyFlexGrow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str321, CSSPropertyWebkitColumnWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str322, CSSPropertyWebkitClipPath},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str323, CSSPropertyLightingColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str324, CSSPropertyWebkitMinLogicalWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str325, CSSPropertyWebkitAnimationPlayState},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str326, CSSPropertyListStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str327, CSSPropertyColorInterpolationFilters},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str328, CSSPropertyFlexGrow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str329, CSSPropertyWebkitRubyPosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str330, CSSPropertyBufferedRendering},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str331, CSSPropertyStrokeOpacity},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str332, CSSPropertyTouchAction},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str333, CSSPropertyFlexBasis},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str334, CSSPropertyFlexWrap},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str335, CSSPropertyFontVariantLigatures},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str336, CSSPropertyFontWeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str337, CSSPropertyMaxWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str338, CSSPropertyFlexBasis},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str339, CSSPropertyFlexWrap},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str340, CSSPropertyListStyleImage},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str341, CSSPropertyWebkitBorderAfterWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str342, CSSPropertyWebkitBorderBeforeWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str343, CSSPropertyTableLayout},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str344, CSSPropertyBackgroundAttachment},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str345, CSSPropertyBoxShadow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str346, CSSPropertyTextShadow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str347, CSSPropertyStopOpacity},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str348, CSSPropertyWebkitBoxShadow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str349, CSSPropertyOutlineOffset},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str350, CSSPropertyOutlineStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str351, CSSPropertyWebkitFontSmoothing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str352, CSSPropertyWebkitFontFeatureSettings},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str353, CSSPropertyTextOverlineColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str354, CSSPropertyWebkitTextStrokeWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str355, CSSPropertyFlexDirection},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str356, CSSPropertyBorderLeftWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str357, CSSPropertyTextTransform},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str358, CSSPropertyWebkitBoxReflect},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str359, CSSPropertyListStylePosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str360, CSSPropertyFlexDirection},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str361, CSSPropertyInternalMarqueeStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str362, CSSPropertyBackgroundRepeatY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str363, CSSPropertyWebkitPerspectiveOriginX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str364, CSSPropertyWebkitMaskBoxImageWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str365, CSSPropertyVisibility},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str366, CSSPropertyBaselineShift},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str367, CSSPropertyMaskSourceType},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str368, CSSPropertyBackgroundPositionY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str369, CSSPropertyTextAnchor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str370, CSSPropertyEmptyCells},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str371, CSSPropertyFontStretch},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str372, CSSPropertyHeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str373, CSSPropertyWebkitTextEmphasis},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str374, CSSPropertyMinHeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str375, CSSPropertyWebkitColumnRuleWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str376, CSSPropertyBorderRightWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str377, CSSPropertyOverflowX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str378, CSSPropertyTextOverflow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str379, CSSPropertyWebkitBorderHorizontalSpacing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str380, CSSPropertyScrollBehavior},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str381, CSSPropertyWebkitTransformOriginY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str382, CSSPropertyTextUnderlineWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str383, CSSPropertyFontStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str384, CSSPropertyLineHeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str385, CSSPropertyWebkitTextFillColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str386, CSSPropertyWebkitTextEmphasisPosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str387, CSSPropertyWebkitBorderAfterStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str388, CSSPropertyWebkitBorderBeforeStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str389, CSSPropertyTransformStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str390, CSSPropertyWebkitUserModify},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str391, CSSPropertyWebkitTransformStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str392, CSSPropertyWebkitTextEmphasis},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str393, CSSPropertyBorderLeftStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str394, CSSPropertyWebkitBoxFlex},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str395, CSSPropertyStrokeDasharray},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str396, CSSPropertyVectorEffect},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str397, CSSPropertyWebkitWrapThrough},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str398, CSSPropertyWebkitMaxLogicalWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str399, CSSPropertyWebkitPerspectiveOriginY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str400, CSSPropertyTextOverlineWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str401, CSSPropertyFlexFlow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str402, CSSPropertyWebkitTextEmphasisColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str403, CSSPropertyFloodOpacity},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str404, CSSPropertyJustifyContent},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str405, CSSPropertyFlexFlow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str406, CSSPropertyTextDecorationStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str407, CSSPropertyWebkitColumnRuleStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str408, CSSPropertyJustifyContent},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str409, CSSPropertyBorderRightStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str410, CSSPropertyWebkitTextSecurity},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str411, CSSPropertyStrokeDashoffset},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str412, CSSPropertyOverflowY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str413, CSSPropertyTextUnderlineStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str414, CSSPropertyFillOpacity},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str415, CSSPropertyFlexShrink},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str416, CSSPropertyWebkitLogicalHeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str417, CSSPropertyFlexShrink},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str418, CSSPropertyWebkitMinLogicalHeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str419, CSSPropertyMaxHeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str420, CSSPropertyWebkitTextEmphasisColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str421, CSSPropertyWebkitBoxFlexGroup},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str422, CSSPropertyWebkitTextDecorationsInEffect},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str423, CSSPropertyFontFamily},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str424, CSSPropertyTextOverlineStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str425, CSSPropertyTextJustify},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str426, CSSPropertyListStyleType},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str427, CSSPropertyTouchActionDelay},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str428, CSSPropertyJustifySelf},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str429, CSSPropertyBackfaceVisibility},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str430, CSSPropertyTextLineThroughMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str431, CSSPropertyWebkitBackfaceVisibility},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str432, CSSPropertyWebkitHighlight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str433, CSSPropertyWebkitMaxLogicalHeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str434, CSSPropertyShapeImageThreshold},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str435, CSSPropertyShapeImageThreshold},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str436, CSSPropertyWebkitTextEmphasisStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str437, CSSPropertyGlyphOrientationVertical},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str438, CSSPropertyTextLineThroughColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str439, CSSPropertyWebkitTapHighlightColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str440, CSSPropertyWebkitTextEmphasisStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str441, CSSPropertyTextLineThroughWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str442, CSSPropertyGlyphOrientationHorizontal},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str443, CSSPropertyTextLineThroughStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str444, CSSPropertyWebkitHyphenateCharacter}
  };

static const short lookup[] =
  {
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      0,  -1,  -1,   1,   2,  -1,   3,  -1,  -1,  -1,
     -1,  -1,   4,  -1,  -1,   5,  -1,   6,   7,  -1,
     -1,  -1,   8,   9,  10,  -1,  -1,  -1,  -1,  -1,
     11,  12,  13,  14,  -1,  15,  16,  17,  18,  19,
     20,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  21,
     22,  -1,  23,  -1,  24,  25,  26,  -1,  -1,  -1,
     27,  -1,  28,  -1,  29,  -1,  -1,  -1,  -1,  30,
     -1,  -1,  -1,  31,  -1,  -1,  32,  -1,  33,  -1,
     -1,  -1,  -1,  34,  -1,  -1,  35,  -1,  -1,  -1,
     36,  37,  38,  -1,  39,  40,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  41,  42,  -1,  43,  -1,  44,
     -1,  -1,  -1,  45,  -1,  -1,  46,  -1,  -1,  -1,
     47,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  48,  -1,  -1,  -1,  49,  -1,  -1,  -1,  -1,
     50,  -1,  -1,  -1,  51,  -1,  -1,  -1,  -1,  -1,
     -1,  52,  -1,  53,  -1,  -1,  -1,  54,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  55,  56,  -1,  -1,  -1,
     -1,  57,  58,  59,  60,  61,  -1,  -1,  -1,  62,
     -1,  63,  64,  -1,  65,  -1,  -1,  66,  -1,  -1,
     -1,  67,  -1,  -1,  68,  69,  -1,  -1,  70,  -1,
     71,  -1,  -1,  -1,  -1,  72,  -1,  -1,  -1,  -1,
     -1,  73,  -1,  -1,  74,  -1,  -1,  -1,  75,  -1,
     -1,  -1,  -1,  -1,  76,  77,  -1,  78,  -1,  -1,
     -1,  -1,  79,  80,  -1,  81,  -1,  -1,  -1,  82,
     -1,  -1,  -1,  -1,  83,  -1,  -1,  84,  -1,  -1,
     -1,  85,  -1,  86,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  87,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  88,  89,  -1,  90,
     -1,  91,  92,  93,  -1,  94,  -1,  -1,  -1,  95,
     -1,  -1,  -1,  -1,  -1,  96,  -1,  -1,  -1,  -1,
     -1,  97,  -1,  98,  99,  -1, 100,  -1, 101,  -1,
     -1,  -1,  -1, 102,  -1, 103, 104,  -1,  -1,  -1,
    105, 106,  -1,  -1,  -1,  -1, 107, 108,  -1, 109,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 110, 111,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 112,  -1,  -1,
     -1, 113,  -1,  -1,  -1,  -1, 114, 115,  -1, 116,
     -1,  -1, 117,  -1,  -1,  -1, 118,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 119, 120,  -1,  -1, 121,
    122,  -1, 123,  -1,  -1,  -1, 124,  -1,  -1, 125,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 126,
     -1, 127,  -1,  -1, 128,  -1, 129,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 130,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 131,  -1,  -1,  -1,
     -1,  -1, 132,  -1, 133, 134,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 135,  -1, 136,
    137,  -1, 138,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 139, 140, 141, 142, 143,  -1,
     -1,  -1, 144,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    145,  -1, 146,  -1,  -1,  -1,  -1, 147, 148, 149,
    150, 151, 152, 153, 154,  -1,  -1, 155,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 156,  -1,  -1, 157,
     -1,  -1, 158,  -1,  -1,  -1,  -1, 159, 160,  -1,
     -1,  -1, 161,  -1,  -1,  -1,  -1,  -1, 162, 163,
    164,  -1, 165,  -1,  -1,  -1, 166,  -1, 167,  -1,
    168,  -1,  -1,  -1,  -1,  -1, 169,  -1, 170,  -1,
    171,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 172,  -1,
     -1,  -1, 173,  -1, 174,  -1,  -1,  -1, 175,  -1,
    176,  -1,  -1,  -1,  -1, 177, 178,  -1,  -1,  -1,
    179,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 180, 181,
     -1,  -1,  -1, 182, 183,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 184, 185,  -1,  -1,  -1, 186,
     -1,  -1, 187,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    188,  -1, 189,  -1,  -1,  -1,  -1,  -1,  -1, 190,
     -1, 191,  -1, 192, 193, 194, 195,  -1,  -1,  -1,
     -1, 196,  -1, 197,  -1,  -1, 198,  -1, 199,  -1,
     -1, 200,  -1, 201, 202,  -1,  -1,  -1, 203, 204,
     -1, 205,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 206,
     -1,  -1, 207, 208, 209, 210, 211, 212, 213,  -1,
    214,  -1, 215,  -1,  -1,  -1, 216,  -1,  -1,  -1,
     -1, 217,  -1, 218,  -1,  -1, 219, 220,  -1,  -1,
    221,  -1,  -1,  -1,  -1,  -1,  -1, 222,  -1,  -1,
    223,  -1,  -1,  -1, 224,  -1,  -1,  -1,  -1, 225,
     -1, 226, 227,  -1, 228,  -1, 229, 230,  -1,  -1,
    231,  -1, 232, 233, 234,  -1,  -1, 235, 236, 237,
     -1,  -1,  -1,  -1,  -1,  -1, 238,  -1,  -1, 239,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 240,  -1, 241,  -1,  -1,  -1, 242,  -1,
     -1,  -1, 243,  -1,  -1, 244, 245,  -1, 246,  -1,
     -1,  -1, 247,  -1, 248,  -1, 249,  -1,  -1,  -1,
     -1,  -1,  -1, 250,  -1,  -1,  -1,  -1,  -1, 251,
    252,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    253,  -1,  -1, 254,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 255, 256,  -1, 257, 258,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 259,  -1, 260,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 261,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 262,
    263,  -1,  -1, 264,  -1,  -1,  -1,  -1, 265,  -1,
    266,  -1,  -1,  -1, 267, 268, 269,  -1,  -1, 270,
     -1,  -1,  -1, 271,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 272, 273,  -1,  -1, 274,  -1,
     -1,  -1, 275, 276,  -1,  -1, 277,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 278,  -1,  -1,  -1,  -1, 279,
     -1,  -1, 280, 281,  -1,  -1,  -1,  -1,  -1,  -1,
    282,  -1,  -1,  -1, 283,  -1,  -1, 284,  -1,  -1,
     -1, 285,  -1, 286,  -1,  -1, 287, 288, 289, 290,
     -1,  -1, 291, 292, 293, 294,  -1,  -1,  -1, 295,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 296,  -1,  -1, 297,
     -1,  -1, 298, 299,  -1, 300,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 301,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 302,  -1, 303, 304,  -1,  -1, 305,
     -1,  -1,  -1, 306,  -1,  -1,  -1,  -1,  -1,  -1,
    307,  -1, 308,  -1, 309,  -1,  -1,  -1,  -1, 310,
    311,  -1,  -1,  -1, 312,  -1, 313,  -1,  -1,  -1,
     -1,  -1,  -1, 314,  -1,  -1, 315, 316, 317,  -1,
    318, 319,  -1, 320,  -1,  -1, 321,  -1, 322,  -1,
     -1,  -1, 323,  -1,  -1,  -1,  -1, 324,  -1,  -1,
     -1, 325,  -1, 326, 327,  -1,  -1, 328,  -1,  -1,
     -1,  -1, 329, 330, 331, 332,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 333, 334,  -1,  -1,
     -1,  -1,  -1, 335,  -1,  -1,  -1,  -1,  -1, 336,
    337,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    338, 339,  -1,  -1, 340,  -1,  -1,  -1,  -1,  -1,
     -1, 341,  -1,  -1,  -1, 342,  -1,  -1,  -1,  -1,
    343,  -1,  -1,  -1,  -1,  -1, 344, 345,  -1, 346,
    347,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1, 348,  -1, 349,  -1,  -1,  -1,  -1,  -1, 350,
     -1,  -1,  -1,  -1, 351, 352, 353,  -1,  -1,  -1,
     -1,  -1,  -1, 354, 355,  -1,  -1,  -1,  -1,  -1,
     -1, 356,  -1,  -1,  -1,  -1, 357,  -1,  -1, 358,
     -1,  -1, 359,  -1,  -1,  -1,  -1,  -1, 360,  -1,
     -1,  -1, 361,  -1,  -1, 362, 363,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 364,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 365,  -1,  -1,  -1,  -1,  -1, 366,
     -1, 367,  -1,  -1,  -1,  -1,  -1, 368,  -1,  -1,
    369,  -1,  -1, 370,  -1,  -1, 371, 372,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 373, 374,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 375,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    376,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 377,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 378,  -1,  -1, 379,  -1,  -1,  -1,  -1,
    380,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 381,
     -1,  -1, 382,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    383,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1, 384,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    385,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1, 386,  -1,  -1, 387,  -1,  -1,  -1, 388, 389,
     -1,  -1,  -1,  -1,  -1,  -1, 390,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 391,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 392,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 393,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 394,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 395,  -1,  -1,  -1, 396,
    397,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1, 398,  -1,  -1,  -1, 399,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 400,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 401,  -1,  -1,  -1,  -1,  -1, 402,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 403, 404, 405,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 406,  -1,  -1, 407,  -1, 408,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 409,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 410,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    411,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 412,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 413,  -1,  -1, 414,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    415,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 416,  -1,
     -1,  -1,  -1,  -1, 417,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 418,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 419, 420,  -1,
     -1,  -1,  -1, 421,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 422,  -1,
     -1,  -1, 423,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 424,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 425,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 426,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 427, 428,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1, 429,  -1,  -1,  -1,  -1,  -1,  -1, 430,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 431,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 432,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 433, 434,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 435,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 436,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 437,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 438,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 439,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 440,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1, 441,  -1,  -1,  -1, 442,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 443,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 444
  };

const struct Property *
CSSPropertyNamesHash::findPropertyImpl (register const char *str, register unsigned int len)
{
  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = property_hash_function (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int index = lookup[key];

          if (index >= 0)
            {
              register const char *s = property_word_list[index].nameOffset + stringpool;

              if (*str == *s && !strncmp (str + 1, s + 1, len - 1) && s[len] == '\0')
                return &property_word_list[index];
            }
        }
    }
  return 0;
}

const Property* findProperty(register const char* str, register unsigned int len)
{
    return CSSPropertyNamesHash::findPropertyImpl(str, len);
}

const char* getPropertyName(CSSPropertyID id)
{
    if (id < firstCSSProperty)
        return 0;
    int index = id - firstCSSProperty;
    if (index >= numCSSProperties)
        return 0;
    return propertyNameStringsPool + propertyNameStringsOffsets[index];
}

const AtomicString& getPropertyNameAtomicString(CSSPropertyID id)
{
    if (id < firstCSSProperty)
        return nullAtom;
    int index = id - firstCSSProperty;
    if (index >= numCSSProperties)
        return nullAtom;

    static AtomicString* propertyStrings = new AtomicString[numCSSProperties]; // Intentionally never destroyed.
    AtomicString& propertyString = propertyStrings[index];
    if (propertyString.isNull()) {
        const char* propertyName = propertyNameStringsPool + propertyNameStringsOffsets[index];
        propertyString = AtomicString(propertyName, strlen(propertyName), AtomicString::ConstructFromLiteral);
    }
    return propertyString;
}

String getPropertyNameString(CSSPropertyID id)
{
    // We share the StringImpl with the AtomicStrings.
    return getPropertyNameAtomicString(id).string();
}

String getJSPropertyName(CSSPropertyID id)
{
    char result[maxCSSPropertyNameLength + 1];
    const char* cssPropertyName = getPropertyName(id);
    const char* propertyNamePointer = cssPropertyName;
    if (!propertyNamePointer)
        return emptyString();

    char* resultPointer = result;
    while (char character = *propertyNamePointer++) {
        if (character == '-') {
            char nextCharacter = *propertyNamePointer++;
            if (!nextCharacter)
                break;
            character = (propertyNamePointer - 2 != cssPropertyName) ? toASCIIUpper(nextCharacter) : nextCharacter;
        }
        *resultPointer++ = character;
    }
    *resultPointer = '\0';
    return String(result);
}

bool isInternalProperty(CSSPropertyID id)
{
    switch (id) {
        case CSSPropertyInternalCallback:
case CSSPropertyInternalMarqueeDirection:
case CSSPropertyInternalMarqueeIncrement:
case CSSPropertyInternalMarqueeRepetition:
case CSSPropertyInternalMarqueeSpeed:
case CSSPropertyInternalMarqueeStyle:
            return true;
        default:
            return false;
    }
}

} // namespace WebCore
