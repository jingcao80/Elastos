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
#include "CSSValueKeywords.h"
#include "core/css/HashTools.h"
#include <string.h>

namespace WebCore {
static const char valueListStringPool[] = {
"\0"
    "inherit\0"
    "initial\0"
    "none\0"
    "hidden\0"
    "inset\0"
    "groove\0"
    "outset\0"
    "ridge\0"
    "dotted\0"
    "dashed\0"
    "solid\0"
    "double\0"
    "caption\0"
    "icon\0"
    "menu\0"
    "message-box\0"
    "small-caption\0"
    "-webkit-mini-control\0"
    "-webkit-small-control\0"
    "-webkit-control\0"
    "status-bar\0"
    "italic\0"
    "oblique\0"
    "all\0"
    "small-caps\0"
    "common-ligatures\0"
    "no-common-ligatures\0"
    "discretionary-ligatures\0"
    "no-discretionary-ligatures\0"
    "historical-ligatures\0"
    "no-historical-ligatures\0"
    "contextual\0"
    "no-contextual\0"
    "normal\0"
    "bold\0"
    "bolder\0"
    "lighter\0"
    "100\0"
    "200\0"
    "300\0"
    "400\0"
    "500\0"
    "600\0"
    "700\0"
    "800\0"
    "900\0"
    "xx-small\0"
    "x-small\0"
    "small\0"
    "medium\0"
    "large\0"
    "x-large\0"
    "xx-large\0"
    "-webkit-xxx-large\0"
    "smaller\0"
    "larger\0"
    "serif\0"
    "sans-serif\0"
    "cursive\0"
    "fantasy\0"
    "monospace\0"
    "-webkit-body\0"
    "-webkit-pictograph\0"
    "aqua\0"
    "black\0"
    "blue\0"
    "fuchsia\0"
    "gray\0"
    "green\0"
    "lime\0"
    "maroon\0"
    "navy\0"
    "olive\0"
    "orange\0"
    "purple\0"
    "red\0"
    "silver\0"
    "teal\0"
    "white\0"
    "yellow\0"
    "transparent\0"
    "-webkit-link\0"
    "-webkit-activelink\0"
    "activeborder\0"
    "activecaption\0"
    "appworkspace\0"
    "background\0"
    "buttonface\0"
    "buttonhighlight\0"
    "buttonshadow\0"
    "buttontext\0"
    "captiontext\0"
    "graytext\0"
    "highlight\0"
    "highlighttext\0"
    "inactiveborder\0"
    "inactivecaption\0"
    "inactivecaptiontext\0"
    "infobackground\0"
    "infotext\0"
    "menutext\0"
    "scrollbar\0"
    "threeddarkshadow\0"
    "threedface\0"
    "threedhighlight\0"
    "threedlightshadow\0"
    "threedshadow\0"
    "window\0"
    "windowframe\0"
    "windowtext\0"
    "-internal-active-list-box-selection\0"
    "-internal-active-list-box-selection-text\0"
    "-internal-inactive-list-box-selection\0"
    "-internal-inactive-list-box-selection-text\0"
    "-webkit-focus-ring-color\0"
    "currentcolor\0"
    "grey\0"
    "-webkit-text\0"
    "repeat\0"
    "repeat-x\0"
    "repeat-y\0"
    "no-repeat\0"
    "clear\0"
    "copy\0"
    "source-over\0"
    "source-in\0"
    "source-out\0"
    "source-atop\0"
    "destination-over\0"
    "destination-in\0"
    "destination-out\0"
    "destination-atop\0"
    "xor\0"
    "plus-darker\0"
    "plus-lighter\0"
    "baseline\0"
    "middle\0"
    "sub\0"
    "super\0"
    "text-top\0"
    "text-bottom\0"
    "top\0"
    "bottom\0"
    "-webkit-baseline-middle\0"
    "-webkit-auto\0"
    "left\0"
    "right\0"
    "center\0"
    "justify\0"
    "-webkit-left\0"
    "-webkit-right\0"
    "-webkit-center\0"
    "-webkit-match-parent\0"
    "inter-word\0"
    "distribute\0"
    "outside\0"
    "inside\0"
    "disc\0"
    "circle\0"
    "square\0"
    "decimal\0"
    "decimal-leading-zero\0"
    "arabic-indic\0"
    "binary\0"
    "bengali\0"
    "cambodian\0"
    "khmer\0"
    "devanagari\0"
    "gujarati\0"
    "gurmukhi\0"
    "kannada\0"
    "lower-hexadecimal\0"
    "lao\0"
    "malayalam\0"
    "mongolian\0"
    "myanmar\0"
    "octal\0"
    "oriya\0"
    "persian\0"
    "urdu\0"
    "telugu\0"
    "tibetan\0"
    "thai\0"
    "upper-hexadecimal\0"
    "lower-roman\0"
    "upper-roman\0"
    "lower-greek\0"
    "lower-alpha\0"
    "lower-latin\0"
    "upper-alpha\0"
    "upper-latin\0"
    "afar\0"
    "ethiopic-halehame-aa-et\0"
    "ethiopic-halehame-aa-er\0"
    "amharic\0"
    "ethiopic-halehame-am-et\0"
    "amharic-abegede\0"
    "ethiopic-abegede-am-et\0"
    "cjk-earthly-branch\0"
    "cjk-heavenly-stem\0"
    "ethiopic\0"
    "ethiopic-halehame-gez\0"
    "ethiopic-abegede\0"
    "ethiopic-abegede-gez\0"
    "hangul-consonant\0"
    "hangul\0"
    "lower-norwegian\0"
    "oromo\0"
    "ethiopic-halehame-om-et\0"
    "sidama\0"
    "ethiopic-halehame-sid-et\0"
    "somali\0"
    "ethiopic-halehame-so-et\0"
    "tigre\0"
    "ethiopic-halehame-tig\0"
    "tigrinya-er\0"
    "ethiopic-halehame-ti-er\0"
    "tigrinya-er-abegede\0"
    "ethiopic-abegede-ti-er\0"
    "tigrinya-et\0"
    "ethiopic-halehame-ti-et\0"
    "tigrinya-et-abegede\0"
    "ethiopic-abegede-ti-et\0"
    "upper-greek\0"
    "upper-norwegian\0"
    "asterisks\0"
    "footnotes\0"
    "hebrew\0"
    "armenian\0"
    "lower-armenian\0"
    "upper-armenian\0"
    "georgian\0"
    "cjk-ideographic\0"
    "hiragana\0"
    "katakana\0"
    "hiragana-iroha\0"
    "katakana-iroha\0"
    "inline\0"
    "block\0"
    "list-item\0"
    "inline-block\0"
    "table\0"
    "inline-table\0"
    "table-row-group\0"
    "table-header-group\0"
    "table-footer-group\0"
    "table-row\0"
    "table-column-group\0"
    "table-column\0"
    "table-cell\0"
    "table-caption\0"
    "-webkit-box\0"
    "-webkit-inline-box\0"
    "flex\0"
    "inline-flex\0"
    "grid\0"
    "inline-grid\0"
    "-webkit-flex\0"
    "-webkit-inline-flex\0"
    "auto\0"
    "crosshair\0"
    "default\0"
    "pointer\0"
    "move\0"
    "vertical-text\0"
    "cell\0"
    "context-menu\0"
    "alias\0"
    "progress\0"
    "no-drop\0"
    "not-allowed\0"
    "zoom-in\0"
    "zoom-out\0"
    "e-resize\0"
    "ne-resize\0"
    "nw-resize\0"
    "n-resize\0"
    "se-resize\0"
    "sw-resize\0"
    "s-resize\0"
    "w-resize\0"
    "ew-resize\0"
    "ns-resize\0"
    "nesw-resize\0"
    "nwse-resize\0"
    "col-resize\0"
    "row-resize\0"
    "text\0"
    "wait\0"
    "help\0"
    "all-scroll\0"
    "-webkit-grab\0"
    "-webkit-grabbing\0"
    "-webkit-zoom-in\0"
    "-webkit-zoom-out\0"
    "ltr\0"
    "rtl\0"
    "capitalize\0"
    "uppercase\0"
    "lowercase\0"
    "visible\0"
    "collapse\0"
    "a3\0"
    "a4\0"
    "a5\0"
    "above\0"
    "absolute\0"
    "always\0"
    "avoid\0"
    "b4\0"
    "b5\0"
    "below\0"
    "bidi-override\0"
    "blink\0"
    "both\0"
    "close-quote\0"
    "embed\0"
    "fixed\0"
    "hand\0"
    "hide\0"
    "invert\0"
    "-webkit-isolate\0"
    "-webkit-isolate-override\0"
    "-webkit-plaintext\0"
    "landscape\0"
    "ledger\0"
    "legal\0"
    "letter\0"
    "line-through\0"
    "local\0"
    "no-close-quote\0"
    "no-open-quote\0"
    "nowrap\0"
    "open-quote\0"
    "overlay\0"
    "overline\0"
    "portrait\0"
    "pre\0"
    "pre-line\0"
    "pre-wrap\0"
    "-internal-presence\0"
    "relative\0"
    "scroll\0"
    "separate\0"
    "show\0"
    "static\0"
    "thick\0"
    "thin\0"
    "underline\0"
    "wavy\0"
    "-webkit-nowrap\0"
    "stretch\0"
    "start\0"
    "end\0"
    "clone\0"
    "slice\0"
    "reverse\0"
    "horizontal\0"
    "vertical\0"
    "inline-axis\0"
    "block-axis\0"
    "single\0"
    "multiple\0"
    "flex-start\0"
    "flex-end\0"
    "space-between\0"
    "space-around\0"
    "row\0"
    "row-reverse\0"
    "column\0"
    "column-reverse\0"
    "wrap-reverse\0"
    "forwards\0"
    "backwards\0"
    "ahead\0"
    "up\0"
    "down\0"
    "slow\0"
    "fast\0"
    "infinite\0"
    "slide\0"
    "alternate\0"
    "read-only\0"
    "read-write\0"
    "read-write-plaintext-only\0"
    "element\0"
    "intrinsic\0"
    "min-intrinsic\0"
    "-webkit-min-content\0"
    "-webkit-max-content\0"
    "-webkit-fill-available\0"
    "-webkit-fit-content\0"
    "min-content\0"
    "max-content\0"
    "clip\0"
    "ellipsis\0"
    "discard\0"
    "dot-dash\0"
    "dot-dot-dash\0"
    "wave\0"
    "continuous\0"
    "skip-white-space\0"
    "break-all\0"
    "break-word\0"
    "space\0"
    "loose\0"
    "strict\0"
    "after-white-space\0"
    "checkbox\0"
    "radio\0"
    "push-button\0"
    "square-button\0"
    "button\0"
    "button-bevel\0"
    "inner-spin-button\0"
    "listbox\0"
    "listitem\0"
    "media-enter-fullscreen-button\0"
    "media-exit-fullscreen-button\0"
    "media-fullscreen-volume-slider\0"
    "media-fullscreen-volume-slider-thumb\0"
    "media-mute-button\0"
    "media-play-button\0"
    "media-overlay-play-button\0"
    "media-toggle-closed-captions-button\0"
    "media-slider\0"
    "media-sliderthumb\0"
    "media-volume-slider-container\0"
    "media-volume-slider\0"
    "media-volume-sliderthumb\0"
    "media-controls-background\0"
    "media-controls-fullscreen-background\0"
    "media-current-time-display\0"
    "media-time-remaining-display\0"
    "menulist\0"
    "menulist-button\0"
    "menulist-text\0"
    "menulist-textfield\0"
    "meter\0"
    "progress-bar\0"
    "progress-bar-value\0"
    "slider-horizontal\0"
    "slider-vertical\0"
    "sliderthumb-horizontal\0"
    "sliderthumb-vertical\0"
    "caret\0"
    "searchfield\0"
    "searchfield-decoration\0"
    "searchfield-results-decoration\0"
    "searchfield-cancel-button\0"
    "textfield\0"
    "relevancy-level-indicator\0"
    "continuous-capacity-level-indicator\0"
    "discrete-capacity-level-indicator\0"
    "rating-level-indicator\0"
    "textarea\0"
    "caps-lock-indicator\0"
    "round\0"
    "border\0"
    "border-box\0"
    "content\0"
    "content-box\0"
    "padding\0"
    "padding-box\0"
    "margin-box\0"
    "contain\0"
    "cover\0"
    "logical\0"
    "visual\0"
    "lines\0"
    "alternate-reverse\0"
    "running\0"
    "paused\0"
    "flat\0"
    "preserve-3d\0"
    "ease\0"
    "linear\0"
    "ease-in\0"
    "ease-out\0"
    "ease-in-out\0"
    "step-start\0"
    "step-middle\0"
    "step-end\0"
    "document\0"
    "reset\0"
    "zoom\0"
    "visiblepainted\0"
    "visiblefill\0"
    "visiblestroke\0"
    "painted\0"
    "fill\0"
    "stroke\0"
    "bounding-box\0"
    "spell-out\0"
    "digits\0"
    "literal-punctuation\0"
    "no-punctuation\0"
    "antialiased\0"
    "subpixel-antialiased\0"
    "optimizespeed\0"
    "optimizelegibility\0"
    "geometricprecision\0"
    "economy\0"
    "exact\0"
    "lr\0"
    "rl\0"
    "tb\0"
    "lr-tb\0"
    "rl-tb\0"
    "tb-rl\0"
    "horizontal-tb\0"
    "vertical-rl\0"
    "vertical-lr\0"
    "horizontal-bt\0"
    "after\0"
    "before\0"
    "over\0"
    "under\0"
    "filled\0"
    "open\0"
    "dot\0"
    "double-circle\0"
    "triangle\0"
    "sesame\0"
    "ellipse\0"
    "closest-side\0"
    "closest-corner\0"
    "farthest-side\0"
    "farthest-corner\0"
    "sideways\0"
    "sideways-right\0"
    "upright\0"
    "vertical-right\0"
    "font\0"
    "glyphs\0"
    "inline-box\0"
    "replaced\0"
    "on\0"
    "off\0"
    "optimizequality\0"
    "-webkit-optimize-contrast\0"
    "nonzero\0"
    "evenodd\0"
    "at\0"
    "maximum\0"
    "wrap\0"
    "alphabetic\0"
    "sticky\0"
    "coarse\0"
    "fine\0"
    "multiply\0"
    "screen\0"
    "darken\0"
    "lighten\0"
    "color-dodge\0"
    "color-burn\0"
    "hard-light\0"
    "soft-light\0"
    "difference\0"
    "exclusion\0"
    "hue\0"
    "saturation\0"
    "color\0"
    "luminosity\0"
    "scale-down\0"
    "balance\0"
    "-webkit-paged-x\0"
    "-webkit-paged-y\0"
    "drag\0"
    "no-drag\0"
    "span\0"
    "each-line\0"
    "progressive\0"
    "interlace\0"
    "markers\0"
    "-internal-extend-to-zoom\0"
    "isolate\0"
    "script\0"
    "pan-x\0"
    "pan-y\0"
    "manipulation\0"
    "self-start\0"
    "self-end\0"
    "true\0"
    "safe\0"
    "instant\0"
    "smooth\0"
    "contents\0"
    "scroll-position\0"
    "unset\0"
    "aliceblue\0"
    "alpha\0"
    "antiquewhite\0"
    "aquamarine\0"
    "azure\0"
    "beige\0"
    "bisque\0"
    "blanchedalmond\0"
    "blueviolet\0"
    "brown\0"
    "burlywood\0"
    "cadetblue\0"
    "chartreuse\0"
    "chocolate\0"
    "coral\0"
    "cornflowerblue\0"
    "cornsilk\0"
    "crimson\0"
    "cyan\0"
    "darkblue\0"
    "darkcyan\0"
    "darkgoldenrod\0"
    "darkgray\0"
    "darkgreen\0"
    "darkgrey\0"
    "darkkhaki\0"
    "darkmagenta\0"
    "darkolivegreen\0"
    "darkorange\0"
    "darkorchid\0"
    "darkred\0"
    "darksalmon\0"
    "darkseagreen\0"
    "darkslateblue\0"
    "darkslategray\0"
    "darkslategrey\0"
    "darkturquoise\0"
    "darkviolet\0"
    "deeppink\0"
    "deepskyblue\0"
    "dimgray\0"
    "dimgrey\0"
    "dodgerblue\0"
    "firebrick\0"
    "floralwhite\0"
    "forestgreen\0"
    "gainsboro\0"
    "ghostwhite\0"
    "gold\0"
    "goldenrod\0"
    "greenyellow\0"
    "honeydew\0"
    "hotpink\0"
    "indianred\0"
    "indigo\0"
    "ivory\0"
    "khaki\0"
    "lavender\0"
    "lavenderblush\0"
    "lawngreen\0"
    "lemonchiffon\0"
    "lightblue\0"
    "lightcoral\0"
    "lightcyan\0"
    "lightgoldenrodyellow\0"
    "lightgray\0"
    "lightgreen\0"
    "lightgrey\0"
    "lightpink\0"
    "lightsalmon\0"
    "lightseagreen\0"
    "lightskyblue\0"
    "lightslategray\0"
    "lightslategrey\0"
    "lightsteelblue\0"
    "lightyellow\0"
    "limegreen\0"
    "linen\0"
    "luminance\0"
    "magenta\0"
    "mediumaquamarine\0"
    "mediumblue\0"
    "mediumorchid\0"
    "mediumpurple\0"
    "mediumseagreen\0"
    "mediumslateblue\0"
    "mediumspringgreen\0"
    "mediumturquoise\0"
    "mediumvioletred\0"
    "midnightblue\0"
    "mintcream\0"
    "mistyrose\0"
    "moccasin\0"
    "navajowhite\0"
    "oldlace\0"
    "olivedrab\0"
    "orangered\0"
    "orchid\0"
    "palegoldenrod\0"
    "palegreen\0"
    "paleturquoise\0"
    "palevioletred\0"
    "papayawhip\0"
    "peachpuff\0"
    "peru\0"
    "pink\0"
    "plum\0"
    "powderblue\0"
    "rosybrown\0"
    "royalblue\0"
    "saddlebrown\0"
    "salmon\0"
    "sandybrown\0"
    "seagreen\0"
    "seashell\0"
    "sienna\0"
    "skyblue\0"
    "slateblue\0"
    "slategray\0"
    "slategrey\0"
    "snow\0"
    "springgreen\0"
    "steelblue\0"
    "tan\0"
    "thistle\0"
    "tomato\0"
    "turquoise\0"
    "violet\0"
    "wheat\0"
    "whitesmoke\0"
    "yellowgreen\0"
    "accumulate\0"
    "new\0"
    "srgb\0"
    "linearrgb\0"
    "crispedges\0"
    "butt\0"
    "miter\0"
    "bevel\0"
    "before-edge\0"
    "after-edge\0"
    "central\0"
    "text-before-edge\0"
    "text-after-edge\0"
    "ideographic\0"
    "hanging\0"
    "mathematical\0"
    "use-script\0"
    "no-change\0"
    "reset-size\0"
    "dynamic\0"
    "non-scaling-stroke\0"
};

static const unsigned short valueListStringOffsets[] = {
  0,
  1,
  9,
  17,
  22,
  29,
  35,
  42,
  49,
  55,
  62,
  69,
  75,
  82,
  90,
  95,
  100,
  112,
  126,
  147,
  169,
  185,
  196,
  203,
  211,
  215,
  226,
  243,
  263,
  287,
  314,
  335,
  359,
  370,
  384,
  391,
  396,
  403,
  411,
  415,
  419,
  423,
  427,
  431,
  435,
  439,
  443,
  447,
  456,
  464,
  470,
  477,
  483,
  491,
  500,
  518,
  526,
  533,
  539,
  550,
  558,
  566,
  576,
  589,
  608,
  613,
  619,
  624,
  632,
  637,
  643,
  648,
  655,
  660,
  666,
  673,
  680,
  684,
  691,
  696,
  702,
  709,
  721,
  734,
  753,
  766,
  780,
  793,
  804,
  815,
  831,
  844,
  855,
  867,
  876,
  886,
  900,
  915,
  931,
  951,
  966,
  975,
  984,
  994,
  1011,
  1022,
  1038,
  1056,
  1069,
  1076,
  1088,
  1099,
  1135,
  1176,
  1214,
  1257,
  1282,
  1295,
  1300,
  1313,
  1320,
  1329,
  1338,
  1348,
  1354,
  1359,
  1371,
  1381,
  1392,
  1404,
  1421,
  1436,
  1452,
  1469,
  1473,
  1485,
  1498,
  1507,
  1514,
  1518,
  1524,
  1533,
  1545,
  1549,
  1556,
  1580,
  1593,
  1598,
  1604,
  1611,
  1619,
  1632,
  1646,
  1661,
  1682,
  1693,
  1704,
  1712,
  1719,
  1724,
  1731,
  1738,
  1746,
  1767,
  1780,
  1787,
  1795,
  1805,
  1811,
  1822,
  1831,
  1840,
  1848,
  1866,
  1870,
  1880,
  1890,
  1898,
  1904,
  1910,
  1918,
  1923,
  1930,
  1938,
  1943,
  1961,
  1973,
  1985,
  1997,
  2009,
  2021,
  2033,
  2045,
  2050,
  2074,
  2098,
  2106,
  2130,
  2146,
  2169,
  2188,
  2206,
  2215,
  2237,
  2254,
  2275,
  2292,
  2299,
  2315,
  2321,
  2345,
  2352,
  2377,
  2384,
  2408,
  2414,
  2436,
  2448,
  2472,
  2492,
  2515,
  2527,
  2551,
  2571,
  2594,
  2606,
  2622,
  2632,
  2642,
  2649,
  2658,
  2673,
  2688,
  2697,
  2713,
  2722,
  2731,
  2746,
  2761,
  2768,
  2774,
  2784,
  2797,
  2803,
  2816,
  2832,
  2851,
  2870,
  2880,
  2899,
  2912,
  2923,
  2937,
  2949,
  2968,
  2973,
  2985,
  2990,
  3002,
  3015,
  3035,
  3040,
  3050,
  3058,
  3066,
  3071,
  3085,
  3090,
  3103,
  3109,
  3118,
  3126,
  3138,
  3146,
  3155,
  3164,
  3174,
  3184,
  3193,
  3203,
  3213,
  3222,
  3231,
  3241,
  3251,
  3263,
  3275,
  3286,
  3297,
  3302,
  3307,
  3312,
  3323,
  3336,
  3353,
  3369,
  3386,
  3390,
  3394,
  3405,
  3415,
  3425,
  3433,
  3442,
  3445,
  3448,
  3451,
  3457,
  3466,
  3473,
  3479,
  3482,
  3485,
  3491,
  3505,
  3511,
  3516,
  3528,
  3534,
  3540,
  3545,
  3550,
  3557,
  3573,
  3598,
  3616,
  3626,
  3633,
  3639,
  3646,
  3659,
  3665,
  3680,
  3694,
  3701,
  3712,
  3720,
  3729,
  3738,
  3742,
  3751,
  3760,
  3779,
  3788,
  3795,
  3804,
  3809,
  3816,
  3822,
  3827,
  3837,
  3842,
  3857,
  3865,
  3871,
  3875,
  3881,
  3887,
  3895,
  3906,
  3915,
  3927,
  3938,
  3945,
  3954,
  3965,
  3974,
  3988,
  4001,
  4005,
  4017,
  4024,
  4039,
  4052,
  4061,
  4071,
  4077,
  4080,
  4085,
  4090,
  4095,
  4104,
  4110,
  4120,
  4130,
  4141,
  4167,
  4175,
  4185,
  4199,
  4219,
  4239,
  4262,
  4282,
  4294,
  4306,
  4311,
  4320,
  4328,
  4337,
  4350,
  4355,
  4366,
  4383,
  4393,
  4404,
  4410,
  4416,
  4423,
  4441,
  4450,
  4456,
  4468,
  4482,
  4489,
  4502,
  4520,
  4528,
  4537,
  4567,
  4596,
  4627,
  4664,
  4682,
  4700,
  4726,
  4762,
  4775,
  4793,
  4823,
  4843,
  4868,
  4894,
  4931,
  4958,
  4987,
  4996,
  5012,
  5026,
  5045,
  5051,
  5064,
  5083,
  5101,
  5117,
  5140,
  5161,
  5167,
  5179,
  5202,
  5233,
  5259,
  5269,
  5295,
  5331,
  5365,
  5388,
  5397,
  5417,
  5423,
  5430,
  5441,
  5449,
  5461,
  5469,
  5481,
  5492,
  5500,
  5506,
  5514,
  5521,
  5527,
  5545,
  5553,
  5560,
  5565,
  5577,
  5582,
  5589,
  5597,
  5606,
  5618,
  5629,
  5641,
  5650,
  5659,
  5665,
  5670,
  5685,
  5697,
  5711,
  5719,
  5724,
  5731,
  5744,
  5754,
  5761,
  5781,
  5796,
  5808,
  5829,
  5843,
  5862,
  5881,
  5889,
  5895,
  5898,
  5901,
  5904,
  5910,
  5916,
  5922,
  5936,
  5948,
  5960,
  5974,
  5980,
  5987,
  5992,
  5998,
  6005,
  6010,
  6014,
  6028,
  6037,
  6044,
  6052,
  6065,
  6080,
  6094,
  6110,
  6119,
  6134,
  6142,
  6157,
  6162,
  6169,
  6180,
  6189,
  6192,
  6196,
  6212,
  6238,
  6246,
  6254,
  6257,
  6265,
  6270,
  6281,
  6288,
  6295,
  6300,
  6309,
  6316,
  6323,
  6331,
  6343,
  6354,
  6365,
  6376,
  6387,
  6397,
  6401,
  6412,
  6418,
  6429,
  6440,
  6448,
  6464,
  6480,
  6485,
  6493,
  6498,
  6508,
  6520,
  6530,
  6538,
  6563,
  6571,
  6578,
  6584,
  6590,
  6603,
  6614,
  6623,
  6628,
  6633,
  6641,
  6648,
  6657,
  6673,
  6679,
  6689,
  6695,
  6708,
  6719,
  6725,
  6731,
  6738,
  6753,
  6764,
  6770,
  6780,
  6790,
  6801,
  6811,
  6817,
  6832,
  6841,
  6849,
  6854,
  6863,
  6872,
  6886,
  6895,
  6905,
  6914,
  6924,
  6936,
  6951,
  6962,
  6973,
  6981,
  6992,
  7005,
  7019,
  7033,
  7047,
  7061,
  7072,
  7081,
  7093,
  7101,
  7109,
  7120,
  7130,
  7142,
  7154,
  7164,
  7175,
  7180,
  7190,
  7202,
  7211,
  7219,
  7229,
  7236,
  7242,
  7248,
  7257,
  7271,
  7281,
  7294,
  7304,
  7315,
  7325,
  7346,
  7356,
  7367,
  7377,
  7387,
  7399,
  7413,
  7426,
  7441,
  7456,
  7471,
  7483,
  7493,
  7499,
  7509,
  7517,
  7534,
  7545,
  7558,
  7571,
  7586,
  7602,
  7620,
  7636,
  7652,
  7665,
  7675,
  7685,
  7694,
  7706,
  7714,
  7724,
  7734,
  7741,
  7755,
  7765,
  7779,
  7793,
  7804,
  7814,
  7819,
  7824,
  7829,
  7840,
  7850,
  7860,
  7872,
  7879,
  7890,
  7899,
  7908,
  7915,
  7923,
  7933,
  7943,
  7953,
  7958,
  7970,
  7980,
  7984,
  7992,
  7999,
  8009,
  8016,
  8022,
  8033,
  8045,
  8056,
  8060,
  8065,
  8075,
  8086,
  8091,
  8097,
  8103,
  8115,
  8126,
  8134,
  8151,
  8167,
  8179,
  8187,
  8200,
  8211,
  8221,
  8232,
  8240,
};

/* maximum key range = 5381, duplicates = 0 */

class CSSValueKeywordsHash
{
private:
  static inline unsigned int value_hash_function (const char *str, unsigned int len);
public:
  static const struct Value *findValueImpl (const char *str, unsigned int len);
};

inline unsigned int
CSSValueKeywordsHash::value_hash_function (register const char *str, register unsigned int len)
{
  static const unsigned short asso_values[] =
    {
      5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399,
      5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399,
      5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399,
      5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399,
      5399, 5399, 5399, 5399, 5399,   85,  861, 5399, 5399,    9,
        20,   19,   11,   12,   10,   18,   17,   14, 5399, 5399,
      5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399,
      5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399,
      5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399,
      5399, 5399, 5399, 5399, 5399, 5399, 5399,   13,   65,   25,
       277,   11,  173,  625,   53,   13,  560,  430,   13,   10,
        10,    9,   26,  977,  137,    9,    9,   93,  828, 1269,
       917,  906,  834,    9, 5399, 5399, 5399, 5399, 5399, 5399,
      5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399,
      5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399,
      5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399,
      5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399,
      5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399,
      5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399,
      5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399,
      5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399,
      5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399,
      5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399,
      5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399,
      5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399, 5399,
      5399, 5399, 5399, 5399, 5399, 5399, 5399
    };
  register int hval = 0;

  switch (len)
    {
      default:
        hval += asso_values[(unsigned char)str[41]];
      /*FALLTHROUGH*/
      case 41:
        hval += asso_values[(unsigned char)str[40]];
      /*FALLTHROUGH*/
      case 40:
        hval += asso_values[(unsigned char)str[39]];
      /*FALLTHROUGH*/
      case 39:
        hval += asso_values[(unsigned char)str[38]];
      /*FALLTHROUGH*/
      case 38:
        hval += asso_values[(unsigned char)str[37]];
      /*FALLTHROUGH*/
      case 37:
        hval += asso_values[(unsigned char)str[36]];
      /*FALLTHROUGH*/
      case 36:
        hval += asso_values[(unsigned char)str[35]];
      /*FALLTHROUGH*/
      case 35:
        hval += asso_values[(unsigned char)str[34]];
      /*FALLTHROUGH*/
      case 34:
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
        hval += asso_values[(unsigned char)str[12]+1];
      /*FALLTHROUGH*/
      case 12:
        hval += asso_values[(unsigned char)str[11]];
      /*FALLTHROUGH*/
      case 11:
        hval += asso_values[(unsigned char)str[10]+1];
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
        hval += asso_values[(unsigned char)str[2]+1];
      /*FALLTHROUGH*/
      case 2:
        hval += asso_values[(unsigned char)str[1]+1];
      /*FALLTHROUGH*/
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval;
}

struct stringpool_t
  {
    char stringpool_str0[sizeof("on")];
    char stringpool_str1[sizeof("lr")];
    char stringpool_str2[sizeof("a5")];
    char stringpool_str3[sizeof("a3")];
    char stringpool_str4[sizeof("a4")];
    char stringpool_str5[sizeof("100")];
    char stringpool_str6[sizeof("600")];
    char stringpool_str7[sizeof("400")];
    char stringpool_str8[sizeof("500")];
    char stringpool_str9[sizeof("end")];
    char stringpool_str10[sizeof("900")];
    char stringpool_str11[sizeof("all")];
    char stringpool_str12[sizeof("tb")];
    char stringpool_str13[sizeof("800")];
    char stringpool_str14[sizeof("700")];
    char stringpool_str15[sizeof("300")];
    char stringpool_str16[sizeof("200")];
    char stringpool_str17[sizeof("inset")];
    char stringpool_str18[sizeof("none")];
    char stringpool_str19[sizeof("oromo")];
    char stringpool_str20[sizeof("inline")];
    char stringpool_str21[sizeof("instant")];
    char stringpool_str22[sizeof("b5")];
    char stringpool_str23[sizeof("tomato")];
    char stringpool_str24[sizeof("b4")];
    char stringpool_str25[sizeof("normal")];
    char stringpool_str26[sizeof("clone")];
    char stringpool_str27[sizeof("tan")];
    char stringpool_str28[sizeof("somali")];
    char stringpool_str29[sizeof("loose")];
    char stringpool_str30[sizeof("coral")];
    char stringpool_str31[sizeof("armenian")];
    char stringpool_str32[sizeof("ellipse")];
    char stringpool_str33[sizeof("oldlace")];
    char stringpool_str34[sizeof("isolate")];
    char stringpool_str35[sizeof("ease")];
    char stringpool_str36[sizeof("content")];
    char stringpool_str37[sizeof("thai")];
    char stringpool_str38[sizeof("ellipsis")];
    char stringpool_str39[sizeof("lao")];
    char stringpool_str40[sizeof("contain")];
    char stringpool_str41[sizeof("at")];
    char stringpool_str42[sizeof("contents")];
    char stringpool_str43[sizeof("small")];
    char stringpool_str44[sizeof("maroon")];
    char stringpool_str45[sizeof("salmon")];
    char stringpool_str46[sizeof("ltr")];
    char stringpool_str47[sizeof("smooth")];
    char stringpool_str48[sizeof("caret")];
    char stringpool_str49[sizeof("table")];
    char stringpool_str50[sizeof("unset")];
    char stringpool_str51[sizeof("collapse")];
    char stringpool_str52[sizeof("srgb")];
    char stringpool_str53[sizeof("monospace")];
    char stringpool_str54[sizeof("chocolate")];
    char stringpool_str55[sizeof("rl")];
    char stringpool_str56[sizeof("logical")];
    char stringpool_str57[sizeof("strict")];
    char stringpool_str58[sizeof("magenta")];
    char stringpool_str59[sizeof("column")];
    char stringpool_str60[sizeof("absolute")];
    char stringpool_str61[sizeof("baseline")];
    char stringpool_str62[sizeof("balance")];
    char stringpool_str63[sizeof("ethiopic")];
    char stringpool_str64[sizeof("ease-in")];
    char stringpool_str65[sizeof("inherit")];
    char stringpool_str66[sizeof("urdu")];
    char stringpool_str67[sizeof("color")];
    char stringpool_str68[sizeof("pre")];
    char stringpool_str69[sizeof("stretch")];
    char stringpool_str70[sizeof("antialiased")];
    char stringpool_str71[sizeof("bottom")];
    char stringpool_str72[sizeof("static")];
    char stringpool_str73[sizeof("font")];
    char stringpool_str74[sizeof("cell")];
    char stringpool_str75[sizeof("italic")];
    char stringpool_str76[sizeof("amharic")];
    char stringpool_str77[sizeof("sesame")];
    char stringpool_str78[sizeof("closest-side")];
    char stringpool_str79[sizeof("element")];
    char stringpool_str80[sizeof("radio")];
    char stringpool_str81[sizeof("both")];
    char stringpool_str82[sizeof("rtl")];
    char stringpool_str83[sizeof("portrait")];
    char stringpool_str84[sizeof("persian")];
    char stringpool_str85[sizeof("fast")];
    char stringpool_str86[sizeof("flat")];
    char stringpool_str87[sizeof("smaller")];
    char stringpool_str88[sizeof("inline-table")];
    char stringpool_str89[sizeof("teal")];
    char stringpool_str90[sizeof("under")];
    char stringpool_str91[sizeof("help")];
    char stringpool_str92[sizeof("legal")];
    char stringpool_str93[sizeof("small-caps")];
    char stringpool_str94[sizeof("table-cell")];
    char stringpool_str95[sizeof("coarse")];
    char stringpool_str96[sizeof("footnotes")];
    char stringpool_str97[sizeof("menu")];
    char stringpool_str98[sizeof("slateblue")];
    char stringpool_str99[sizeof("ease-out")];
    char stringpool_str100[sizeof("crosshair")];
    char stringpool_str101[sizeof("continuous")];
    char stringpool_str102[sizeof("manipulation")];
    char stringpool_str103[sizeof("peru")];
    char stringpool_str104[sizeof("cadetblue")];
    char stringpool_str105[sizeof("alternate")];
    char stringpool_str106[sizeof("underline")];
    char stringpool_str107[sizeof("medium")];
    char stringpool_str108[sizeof("start")];
    char stringpool_str109[sizeof("red")];
    char stringpool_str110[sizeof("intrinsic")];
    char stringpool_str111[sizeof("all-scroll")];
    char stringpool_str112[sizeof("interlace")];
    char stringpool_str113[sizeof("icon")];
    char stringpool_str114[sizeof("screen")];
    char stringpool_str115[sizeof("menulist")];
    char stringpool_str116[sizeof("scroll")];
    char stringpool_str117[sizeof("inside")];
    char stringpool_str118[sizeof("embed")];
    char stringpool_str119[sizeof("solid")];
    char stringpool_str120[sizeof("reset")];
    char stringpool_str121[sizeof("pre-line")];
    char stringpool_str122[sizeof("local")];
    char stringpool_str123[sizeof("script")];
    char stringpool_str124[sizeof("seashell")];
    char stringpool_str125[sizeof("clear")];
    char stringpool_str126[sizeof("center")];
    char stringpool_str127[sizeof("table-column")];
    char stringpool_str128[sizeof("serif")];
    char stringpool_str129[sizeof("bold")];
    char stringpool_str130[sizeof("central")];
    char stringpool_str131[sizeof("mathematical")];
    char stringpool_str132[sizeof("moccasin")];
    char stringpool_str133[sizeof("transparent")];
    char stringpool_str134[sizeof("dot")];
    char stringpool_str135[sizeof("hand")];
    char stringpool_str136[sizeof("octal")];
    char stringpool_str137[sizeof("meter")];
    char stringpool_str138[sizeof("letter")];
    char stringpool_str139[sizeof("landscape")];
    char stringpool_str140[sizeof("saturation")];
    char stringpool_str141[sizeof("steelblue")];
    char stringpool_str142[sizeof("ease-in-out")];
    char stringpool_str143[sizeof("cambodian")];
    char stringpool_str144[sizeof("ahead")];
    char stringpool_str145[sizeof("mediumblue")];
    char stringpool_str146[sizeof("indianred")];
    char stringpool_str147[sizeof("chartreuse")];
    char stringpool_str148[sizeof("sans-serif")];
    char stringpool_str149[sizeof("border")];
    char stringpool_str150[sizeof("bolder")];
    char stringpool_str151[sizeof("cornsilk")];
    char stringpool_str152[sizeof("each-line")];
    char stringpool_str153[sizeof("scrollbar")];
    char stringpool_str154[sizeof("closest-corner")];
    char stringpool_str155[sizeof("threedface")];
    char stringpool_str156[sizeof("block")];
    char stringpool_str157[sizeof("stroke")];
    char stringpool_str158[sizeof("step-start")];
    char stringpool_str159[sizeof("status-bar")];
    char stringpool_str160[sizeof("use-script")];
    char stringpool_str161[sizeof("thin")];
    char stringpool_str162[sizeof("lime")];
    char stringpool_str163[sizeof("black")];
    char stringpool_str164[sizeof("color-burn")];
    char stringpool_str165[sizeof("khmer")];
    char stringpool_str166[sizeof("lines")];
    char stringpool_str167[sizeof("linen")];
    char stringpool_str168[sizeof("alias")];
    char stringpool_str169[sizeof("mediumpurple")];
    char stringpool_str170[sizeof("slice")];
    char stringpool_str171[sizeof("sidama")];
    char stringpool_str172[sizeof("clip")];
    char stringpool_str173[sizeof("thistle")];
    char stringpool_str174[sizeof("self-start")];
    char stringpool_str175[sizeof("initial")];
    char stringpool_str176[sizeof("crimson")];
    char stringpool_str177[sizeof("listitem")];
    char stringpool_str178[sizeof("hide")];
    char stringpool_str179[sizeof("tibetan")];
    char stringpool_str180[sizeof("orchid")];
    char stringpool_str181[sizeof("circle")];
    char stringpool_str182[sizeof("hotpink")];
    char stringpool_str183[sizeof("markers")];
    char stringpool_str184[sizeof("indigo")];
    char stringpool_str185[sizeof("searchfield")];
    char stringpool_str186[sizeof("step-end")];
    char stringpool_str187[sizeof("lemonchiffon")];
    char stringpool_str188[sizeof("ethiopic-abegede")];
    char stringpool_str189[sizeof("dashed")];
    char stringpool_str190[sizeof("dotted")];
    char stringpool_str191[sizeof("infinite")];
    char stringpool_str192[sizeof("mediumslateblue")];
    char stringpool_str193[sizeof("lighten")];
    char stringpool_str194[sizeof("safe")];
    char stringpool_str195[sizeof("document")];
    char stringpool_str196[sizeof("list-item")];
    char stringpool_str197[sizeof("large")];
    char stringpool_str198[sizeof("asterisks")];
    char stringpool_str199[sizeof("mongolian")];
    char stringpool_str200[sizeof("orange")];
    char stringpool_str201[sizeof("destination-atop")];
    char stringpool_str202[sizeof("linear")];
    char stringpool_str203[sizeof("self-end")];
    char stringpool_str204[sizeof("lightsalmon")];
    char stringpool_str205[sizeof("fine")];
    char stringpool_str206[sizeof("fill")];
    char stringpool_str207[sizeof("destination-out")];
    char stringpool_str208[sizeof("farthest-corner")];
    char stringpool_str209[sizeof("tigre")];
    char stringpool_str210[sizeof("sienna")];
    char stringpool_str211[sizeof("decimal")];
    char stringpool_str212[sizeof("pointer")];
    char stringpool_str213[sizeof("mintcream")];
    char stringpool_str214[sizeof("aliceblue")];
    char stringpool_str215[sizeof("darken")];
    char stringpool_str216[sizeof("peachpuff")];
    char stringpool_str217[sizeof("accumulate")];
    char stringpool_str218[sizeof("miter")];
    char stringpool_str219[sizeof("right")];
    char stringpool_str220[sizeof("break-all")];
    char stringpool_str221[sizeof("kannada")];
    char stringpool_str222[sizeof("left")];
    char stringpool_str223[sizeof("green")];
    char stringpool_str224[sizeof("min-content")];
    char stringpool_str225[sizeof("dot-dash")];
    char stringpool_str226[sizeof("lighter")];
    char stringpool_str227[sizeof("afar")];
    char stringpool_str228[sizeof("darksalmon")];
    char stringpool_str229[sizeof("small-caption")];
    char stringpool_str230[sizeof("blanchedalmond")];
    char stringpool_str231[sizeof("progress")];
    char stringpool_str232[sizeof("true")];
    char stringpool_str233[sizeof("hangul")];
    char stringpool_str234[sizeof("table-caption")];
    char stringpool_str235[sizeof("larger")];
    char stringpool_str236[sizeof("sub")];
    char stringpool_str237[sizeof("slide")];
    char stringpool_str238[sizeof("lightblue")];
    char stringpool_str239[sizeof("disc")];
    char stringpool_str240[sizeof("plum")];
    char stringpool_str241[sizeof("after")];
    char stringpool_str242[sizeof("middle")];
    char stringpool_str243[sizeof("lightcoral")];
    char stringpool_str244[sizeof("ethiopic-halehame-so-et")];
    char stringpool_str245[sizeof("ethiopic-halehame-om-et")];
    char stringpool_str246[sizeof("ethiopic-halehame-ti-et")];
    char stringpool_str247[sizeof("ethiopic-halehame-am-et")];
    char stringpool_str248[sizeof("ethiopic-halehame-aa-et")];
    char stringpool_str249[sizeof("zoom")];
    char stringpool_str250[sizeof("ethiopic-abegede-ti-et")];
    char stringpool_str251[sizeof("ethiopic-abegede-am-et")];
    char stringpool_str252[sizeof("above")];
    char stringpool_str253[sizeof("palegreen")];
    char stringpool_str254[sizeof("bengali")];
    char stringpool_str255[sizeof("purple")];
    char stringpool_str256[sizeof("blue")];
    char stringpool_str257[sizeof("multiple")];
    char stringpool_str258[sizeof("digits")];
    char stringpool_str259[sizeof("hidden")];
    char stringpool_str260[sizeof("media-slider")];
    char stringpool_str261[sizeof("luminance")];
    char stringpool_str262[sizeof("cyan")];
    char stringpool_str263[sizeof("inline-grid")];
    char stringpool_str264[sizeof("gold")];
    char stringpool_str265[sizeof("auto")];
    char stringpool_str266[sizeof("inline-block")];
    char stringpool_str267[sizeof("khaki")];
    char stringpool_str268[sizeof("xor")];
    char stringpool_str269[sizeof("non-scaling-stroke")];
    char stringpool_str270[sizeof("lr-tb")];
    char stringpool_str271[sizeof("painted")];
    char stringpool_str272[sizeof("outset")];
    char stringpool_str273[sizeof("darkblue")];
    char stringpool_str274[sizeof("ledger")];
    char stringpool_str275[sizeof("drag")];
    char stringpool_str276[sizeof("horizontal")];
    char stringpool_str277[sizeof("aqua")];
    char stringpool_str278[sizeof("butt")];
    char stringpool_str279[sizeof("azure")];
    char stringpool_str280[sizeof("telugu")];
    char stringpool_str281[sizeof("zoom-in")];
    char stringpool_str282[sizeof("top")];
    char stringpool_str283[sizeof("button")];
    char stringpool_str284[sizeof("searchfield-decoration")];
    char stringpool_str285[sizeof("exact")];
    char stringpool_str286[sizeof("ethiopic-halehame-ti-er")];
    char stringpool_str287[sizeof("before")];
    char stringpool_str288[sizeof("ethiopic-halehame-aa-er")];
    char stringpool_str289[sizeof("pink")];
    char stringpool_str290[sizeof("palevioletred")];
    char stringpool_str291[sizeof("padding")];
    char stringpool_str292[sizeof("ethiopic-abegede-ti-er")];
    char stringpool_str293[sizeof("forestgreen")];
    char stringpool_str294[sizeof("nonzero")];
    char stringpool_str295[sizeof("thick")];
    char stringpool_str296[sizeof("skyblue")];
    char stringpool_str297[sizeof("seagreen")];
    char stringpool_str298[sizeof("filled")];
    char stringpool_str299[sizeof("tb-rl")];
    char stringpool_str300[sizeof("hangul-consonant")];
    char stringpool_str301[sizeof("malayalam")];
    char stringpool_str302[sizeof("hue")];
    char stringpool_str303[sizeof("span")];
    char stringpool_str304[sizeof("alpha")];
    char stringpool_str305[sizeof("katakana")];
    char stringpool_str306[sizeof("up")];
    char stringpool_str307[sizeof("blink")];
    char stringpool_str308[sizeof("myanmar")];
    char stringpool_str309[sizeof("row")];
    char stringpool_str310[sizeof("rl-tb")];
    char stringpool_str311[sizeof("vertical")];
    char stringpool_str312[sizeof("line-through")];
    char stringpool_str313[sizeof("palegoldenrod")];
    char stringpool_str314[sizeof("space")];
    char stringpool_str315[sizeof("pan-y")];
    char stringpool_str316[sizeof("zoom-out")];
    char stringpool_str317[sizeof("text")];
    char stringpool_str318[sizeof("new")];
    char stringpool_str319[sizeof("pan-x")];
    char stringpool_str320[sizeof("inline-axis")];
    char stringpool_str321[sizeof("no-repeat")];
    char stringpool_str322[sizeof("maximum")];
    char stringpool_str323[sizeof("caption")];
    char stringpool_str324[sizeof("contextual")];
    char stringpool_str325[sizeof("deeppink")];
    char stringpool_str326[sizeof("nowrap")];
    char stringpool_str327[sizeof("square")];
    char stringpool_str328[sizeof("oblique")];
    char stringpool_str329[sizeof("inline-box")];
    char stringpool_str330[sizeof("source-in")];
    char stringpool_str331[sizeof("step-middle")];
    char stringpool_str332[sizeof("orangered")];
    char stringpool_str333[sizeof("min-intrinsic")];
    char stringpool_str334[sizeof("arabic-indic")];
    char stringpool_str335[sizeof("col-resize")];
    char stringpool_str336[sizeof("lowercase")];
    char stringpool_str337[sizeof("content-box")];
    char stringpool_str338[sizeof("lightpink")];
    char stringpool_str339[sizeof("ethiopic-halehame-sid-et")];
    char stringpool_str340[sizeof("open")];
    char stringpool_str341[sizeof("mediumseagreen")];
    char stringpool_str342[sizeof("darkslateblue")];
    char stringpool_str343[sizeof("dynamic")];
    char stringpool_str344[sizeof("distribute")];
    char stringpool_str345[sizeof("fantasy")];
    char stringpool_str346[sizeof("aquamarine")];
    char stringpool_str347[sizeof("alphabetic")];
    char stringpool_str348[sizeof("vertical-lr")];
    char stringpool_str349[sizeof("relative")];
    char stringpool_str350[sizeof("push-button")];
    char stringpool_str351[sizeof("destination-in")];
    char stringpool_str352[sizeof("hard-light")];
    char stringpool_str353[sizeof("default")];
    char stringpool_str354[sizeof("progress-bar")];
    char stringpool_str355[sizeof("royalblue")];
    char stringpool_str356[sizeof("darkred")];
    char stringpool_str357[sizeof("katakana-iroha")];
    char stringpool_str358[sizeof("threedhighlight")];
    char stringpool_str359[sizeof("dot-dot-dash")];
    char stringpool_str360[sizeof("context-menu")];
    char stringpool_str361[sizeof("paused")];
    char stringpool_str362[sizeof("double")];
    char stringpool_str363[sizeof("text-top")];
    char stringpool_str364[sizeof("single")];
    char stringpool_str365[sizeof("down")];
    char stringpool_str366[sizeof("menutext")];
    char stringpool_str367[sizeof("source-out")];
    char stringpool_str368[sizeof("max-content")];
    char stringpool_str369[sizeof("buttonface")];
    char stringpool_str370[sizeof("currentcolor")];
    char stringpool_str371[sizeof("outside")];
    char stringpool_str372[sizeof("media-sliderthumb")];
    char stringpool_str373[sizeof("lower-latin")];
    char stringpool_str374[sizeof("economy")];
    char stringpool_str375[sizeof("triangle")];
    char stringpool_str376[sizeof("inline-flex")];
    char stringpool_str377[sizeof("off")];
    char stringpool_str378[sizeof("dodgerblue")];
    char stringpool_str379[sizeof("textarea")];
    char stringpool_str380[sizeof("flex")];
    char stringpool_str381[sizeof("round")];
    char stringpool_str382[sizeof("reset-size")];
    char stringpool_str383[sizeof("text-bottom")];
    char stringpool_str384[sizeof("darkorchid")];
    char stringpool_str385[sizeof("hiragana")];
    char stringpool_str386[sizeof("discard")];
    char stringpool_str387[sizeof("no-punctuation")];
    char stringpool_str388[sizeof("snow")];
    char stringpool_str389[sizeof("slow")];
    char stringpool_str390[sizeof("vertical-rl")];
    char stringpool_str391[sizeof("move")];
    char stringpool_str392[sizeof("show")];
    char stringpool_str393[sizeof("literal-punctuation")];
    char stringpool_str394[sizeof("preserve-3d")];
    char stringpool_str395[sizeof("repeat")];
    char stringpool_str396[sizeof("exclusion")];
    char stringpool_str397[sizeof("separate")];
    char stringpool_str398[sizeof("ridge")];
    char stringpool_str399[sizeof("no-drop")];
    char stringpool_str400[sizeof("ideographic")];
    char stringpool_str401[sizeof("mediumspringgreen")];
    char stringpool_str402[sizeof("lower-alpha")];
    char stringpool_str403[sizeof("fuchsia")];
    char stringpool_str404[sizeof("lower-roman")];
    char stringpool_str405[sizeof("wrap")];
    char stringpool_str406[sizeof("mediumorchid")];
    char stringpool_str407[sizeof("brown")];
    char stringpool_str408[sizeof("spell-out")];
    char stringpool_str409[sizeof("goldenrod")];
    char stringpool_str410[sizeof("limegreen")];
    char stringpool_str411[sizeof("searchfield-cancel-button")];
    char stringpool_str412[sizeof("hanging")];
    char stringpool_str413[sizeof("no-common-ligatures")];
    char stringpool_str414[sizeof("geometricprecision")];
    char stringpool_str415[sizeof("ethiopic-halehame-tig")];
    char stringpool_str416[sizeof("olive")];
    char stringpool_str417[sizeof("firebrick")];
    char stringpool_str418[sizeof("close-quote")];
    char stringpool_str419[sizeof("menulist-button")];
    char stringpool_str420[sizeof("highlight")];
    char stringpool_str421[sizeof("beige")];
    char stringpool_str422[sizeof("hiragana-iroha")];
    char stringpool_str423[sizeof("farthest-side")];
    char stringpool_str424[sizeof("violet")];
    char stringpool_str425[sizeof("invert")];
    char stringpool_str426[sizeof("lower-armenian")];
    char stringpool_str427[sizeof("soft-light")];
    char stringpool_str428[sizeof("cover")];
    char stringpool_str429[sizeof("grid")];
    char stringpool_str430[sizeof("mediumaquamarine")];
    char stringpool_str431[sizeof("wheat")];
    char stringpool_str432[sizeof("lightgreen")];
    char stringpool_str433[sizeof("gainsboro")];
    char stringpool_str434[sizeof("oriya")];
    char stringpool_str435[sizeof("square-button")];
    char stringpool_str436[sizeof("visible")];
    char stringpool_str437[sizeof("groove")];
    char stringpool_str438[sizeof("visual")];
    char stringpool_str439[sizeof("searchfield-results-decoration")];
    char stringpool_str440[sizeof("darkmagenta")];
    char stringpool_str441[sizeof("below")];
    char stringpool_str442[sizeof("bevel")];
    char stringpool_str443[sizeof("flex-start")];
    char stringpool_str444[sizeof("not-allowed")];
    char stringpool_str445[sizeof("silver")];
    char stringpool_str446[sizeof("glyphs")];
    char stringpool_str447[sizeof("crispedges")];
    char stringpool_str448[sizeof("column-reverse")];
    char stringpool_str449[sizeof("linearrgb")];
    char stringpool_str450[sizeof("darkgreen")];
    char stringpool_str451[sizeof("powderblue")];
    char stringpool_str452[sizeof("listbox")];
    char stringpool_str453[sizeof("textfield")];
    char stringpool_str454[sizeof("darkorange")];
    char stringpool_str455[sizeof("over")];
    char stringpool_str456[sizeof("block-axis")];
    char stringpool_str457[sizeof("avoid")];
    char stringpool_str458[sizeof("border-box")];
    char stringpool_str459[sizeof("infotext")];
    char stringpool_str460[sizeof("gray")];
    char stringpool_str461[sizeof("menulist-text")];
    char stringpool_str462[sizeof("scroll-position")];
    char stringpool_str463[sizeof("lightsteelblue")];
    char stringpool_str464[sizeof("georgian")];
    char stringpool_str465[sizeof("table-row")];
    char stringpool_str466[sizeof("replaced")];
    char stringpool_str467[sizeof("destination-over")];
    char stringpool_str468[sizeof("running")];
    char stringpool_str469[sizeof("no-change")];
    char stringpool_str470[sizeof("overline")];
    char stringpool_str471[sizeof("lightcyan")];
    char stringpool_str472[sizeof("inactiveborder")];
    char stringpool_str473[sizeof("amharic-abegede")];
    char stringpool_str474[sizeof("mediumvioletred")];
    char stringpool_str475[sizeof("color-dodge")];
    char stringpool_str476[sizeof("flex-end")];
    char stringpool_str477[sizeof("checkbox")];
    char stringpool_str478[sizeof("mistyrose")];
    char stringpool_str479[sizeof("darkviolet")];
    char stringpool_str480[sizeof("media-enter-fullscreen-button")];
    char stringpool_str481[sizeof("hebrew")];
    char stringpool_str482[sizeof("read-only")];
    char stringpool_str483[sizeof("lightseagreen")];
    char stringpool_str484[sizeof("binary")];
    char stringpool_str485[sizeof("visiblefill")];
    char stringpool_str486[sizeof("space-around")];
    char stringpool_str487[sizeof("grey")];
    char stringpool_str488[sizeof("bisque")];
    char stringpool_str489[sizeof("darkkhaki")];
    char stringpool_str490[sizeof("cursive")];
    char stringpool_str491[sizeof("darkcyan")];
    char stringpool_str492[sizeof("pre-wrap")];
    char stringpool_str493[sizeof("reverse")];
    char stringpool_str494[sizeof("darkgoldenrod")];
    char stringpool_str495[sizeof("s-resize")];
    char stringpool_str496[sizeof("n-resize")];
    char stringpool_str497[sizeof("e-resize")];
    char stringpool_str498[sizeof("cjk-ideographic")];
    char stringpool_str499[sizeof("darkseagreen")];
    char stringpool_str500[sizeof("upright")];
    char stringpool_str501[sizeof("slategrey")];
    char stringpool_str502[sizeof("slategray")];
    char stringpool_str503[sizeof("lavender")];
    char stringpool_str504[sizeof("blueviolet")];
    char stringpool_str505[sizeof("lawngreen")];
    char stringpool_str506[sizeof("tigrinya-er")];
    char stringpool_str507[sizeof("margin-box")];
    char stringpool_str508[sizeof("multiply")];
    char stringpool_str509[sizeof("luminosity")];
    char stringpool_str510[sizeof("plus-darker")];
    char stringpool_str511[sizeof("x-small")];
    char stringpool_str512[sizeof("difference")];
    char stringpool_str513[sizeof("alternate-reverse")];
    char stringpool_str514[sizeof("-webkit-control")];
    char stringpool_str515[sizeof("white")];
    char stringpool_str516[sizeof("progressive")];
    char stringpool_str517[sizeof("always")];
    char stringpool_str518[sizeof("saddlebrown")];
    char stringpool_str519[sizeof("horizontal-tb")];
    char stringpool_str520[sizeof("common-ligatures")];
    char stringpool_str521[sizeof("activecaption")];
    char stringpool_str522[sizeof("after-edge")];
    char stringpool_str523[sizeof("floralwhite")];
    char stringpool_str524[sizeof("tigrinya-et")];
    char stringpool_str525[sizeof("ns-resize")];
    char stringpool_str526[sizeof("buttonhighlight")];
    char stringpool_str527[sizeof("wait")];
    char stringpool_str528[sizeof("lightslategray")];
    char stringpool_str529[sizeof("olivedrab")];
    char stringpool_str530[sizeof("forwards")];
    char stringpool_str531[sizeof("message-box")];
    char stringpool_str532[sizeof("-webkit-isolate")];
    char stringpool_str533[sizeof("fixed")];
    char stringpool_str534[sizeof("copy")];
    char stringpool_str535[sizeof("double-circle")];
    char stringpool_str536[sizeof("no-drag")];
    char stringpool_str537[sizeof("turquoise")];
    char stringpool_str538[sizeof("buttontext")];
    char stringpool_str539[sizeof("super")];
    char stringpool_str540[sizeof("cjk-earthly-branch")];
    char stringpool_str541[sizeof("capitalize")];
    char stringpool_str542[sizeof("threedshadow")];
    char stringpool_str543[sizeof("source-over")];
    char stringpool_str544[sizeof("plus-lighter")];
    char stringpool_str545[sizeof("-webkit-auto")];
    char stringpool_str546[sizeof("lower-greek")];
    char stringpool_str547[sizeof("background")];
    char stringpool_str548[sizeof("horizontal-bt")];
    char stringpool_str549[sizeof("lightslategrey")];
    char stringpool_str550[sizeof("visiblepainted")];
    char stringpool_str551[sizeof("sticky")];
    char stringpool_str552[sizeof("scale-down")];
    char stringpool_str553[sizeof("evenodd")];
    char stringpool_str554[sizeof("activeborder")];
    char stringpool_str555[sizeof("ghostwhite")];
    char stringpool_str556[sizeof("inter-word")];
    char stringpool_str557[sizeof("table-row-group")];
    char stringpool_str558[sizeof("se-resize")];
    char stringpool_str559[sizeof("ne-resize")];
    char stringpool_str560[sizeof("gurmukhi")];
    char stringpool_str561[sizeof("gujarati")];
    char stringpool_str562[sizeof("padding-box")];
    char stringpool_str563[sizeof("menulist-textfield")];
    char stringpool_str564[sizeof("inner-spin-button")];
    char stringpool_str565[sizeof("table-column-group")];
    char stringpool_str566[sizeof("-webkit-right")];
    char stringpool_str567[sizeof("source-atop")];
    char stringpool_str568[sizeof("captiontext")];
    char stringpool_str569[sizeof("-webkit-center")];
    char stringpool_str570[sizeof("media-controls-background")];
    char stringpool_str571[sizeof("paleturquoise")];
    char stringpool_str572[sizeof("read-write")];
    char stringpool_str573[sizeof("row-resize")];
    char stringpool_str574[sizeof("-webkit-small-control")];
    char stringpool_str575[sizeof("button-bevel")];
    char stringpool_str576[sizeof("optimizespeed")];
    char stringpool_str577[sizeof("before-edge")];
    char stringpool_str578[sizeof("midnightblue")];
    char stringpool_str579[sizeof("nwse-resize")];
    char stringpool_str580[sizeof("media-mute-button")];
    char stringpool_str581[sizeof("-webkit-match-parent")];
    char stringpool_str582[sizeof("-webkit-min-content")];
    char stringpool_str583[sizeof("ethiopic-halehame-gez")];
    char stringpool_str584[sizeof("-webkit-link")];
    char stringpool_str585[sizeof("ethiopic-abegede-gez")];
    char stringpool_str586[sizeof("navy")];
    char stringpool_str587[sizeof("uppercase")];
    char stringpool_str588[sizeof("media-fullscreen-volume-slider")];
    char stringpool_str589[sizeof("visiblestroke")];
    char stringpool_str590[sizeof("-internal-presence")];
    char stringpool_str591[sizeof("repeat-y")];
    char stringpool_str592[sizeof("darkslategrey")];
    char stringpool_str593[sizeof("no-open-quote")];
    char stringpool_str594[sizeof("darkslategray")];
    char stringpool_str595[sizeof("repeat-x")];
    char stringpool_str596[sizeof("whitesmoke")];
    char stringpool_str597[sizeof("upper-latin")];
    char stringpool_str598[sizeof("deepskyblue")];
    char stringpool_str599[sizeof("sliderthumb-horizontal")];
    char stringpool_str600[sizeof("ivory")];
    char stringpool_str601[sizeof("open-quote")];
    char stringpool_str602[sizeof("infobackground")];
    char stringpool_str603[sizeof("vertical-text")];
    char stringpool_str604[sizeof("lightgrey")];
    char stringpool_str605[sizeof("lightgray")];
    char stringpool_str606[sizeof("yellow")];
    char stringpool_str607[sizeof("-webkit-left")];
    char stringpool_str608[sizeof("springgreen")];
    char stringpool_str609[sizeof("media-current-time-display")];
    char stringpool_str610[sizeof("no-historical-ligatures")];
    char stringpool_str611[sizeof("upper-alpha")];
    char stringpool_str612[sizeof("upper-roman")];
    char stringpool_str613[sizeof("darkgrey")];
    char stringpool_str614[sizeof("darkgray")];
    char stringpool_str615[sizeof("break-word")];
    char stringpool_str616[sizeof("table-footer-group")];
    char stringpool_str617[sizeof("row-reverse")];
    char stringpool_str618[sizeof("slider-vertical")];
    char stringpool_str619[sizeof("-webkit-fit-content")];
    char stringpool_str620[sizeof("darkolivegreen")];
    char stringpool_str621[sizeof("justify")];
    char stringpool_str622[sizeof("caps-lock-indicator")];
    char stringpool_str623[sizeof("vertical-right")];
    char stringpool_str624[sizeof("overlay")];
    char stringpool_str625[sizeof("dimgrey")];
    char stringpool_str626[sizeof("dimgray")];
    char stringpool_str627[sizeof("lower-hexadecimal")];
    char stringpool_str628[sizeof("backwards")];
    char stringpool_str629[sizeof("devanagari")];
    char stringpool_str630[sizeof("upper-armenian")];
    char stringpool_str631[sizeof("graytext")];
    char stringpool_str632[sizeof("lightyellow")];
    char stringpool_str633[sizeof("cornflowerblue")];
    char stringpool_str634[sizeof("honeydew")];
    char stringpool_str635[sizeof("rosybrown")];
    char stringpool_str636[sizeof("x-large")];
    char stringpool_str637[sizeof("inactivecaption")];
    char stringpool_str638[sizeof("media-fullscreen-volume-slider-thumb")];
    char stringpool_str639[sizeof("media-volume-slider")];
    char stringpool_str640[sizeof("wrap-reverse")];
    char stringpool_str641[sizeof("-webkit-baseline-middle")];
    char stringpool_str642[sizeof("wave")];
    char stringpool_str643[sizeof("table-header-group")];
    char stringpool_str644[sizeof("highlighttext")];
    char stringpool_str645[sizeof("antiquewhite")];
    char stringpool_str646[sizeof("after-white-space")];
    char stringpool_str647[sizeof("buttonshadow")];
    char stringpool_str648[sizeof("-webkit-grab")];
    char stringpool_str649[sizeof("greenyellow")];
    char stringpool_str650[sizeof("-webkit-text")];
    char stringpool_str651[sizeof("threedlightshadow")];
    char stringpool_str652[sizeof("space-between")];
    char stringpool_str653[sizeof("lavenderblush")];
    char stringpool_str654[sizeof("nesw-resize")];
    char stringpool_str655[sizeof("no-contextual")];
    char stringpool_str656[sizeof("xx-small")];
    char stringpool_str657[sizeof("media-controls-fullscreen-background")];
    char stringpool_str658[sizeof("slider-horizontal")];
    char stringpool_str659[sizeof("sandybrown")];
    char stringpool_str660[sizeof("-webkit-box")];
    char stringpool_str661[sizeof("-webkit-body")];
    char stringpool_str662[sizeof("sliderthumb-vertical")];
    char stringpool_str663[sizeof("sideways")];
    char stringpool_str664[sizeof("sw-resize")];
    char stringpool_str665[sizeof("nw-resize")];
    char stringpool_str666[sizeof("ew-resize")];
    char stringpool_str667[sizeof("media-volume-sliderthumb")];
    char stringpool_str668[sizeof("darkturquoise")];
    char stringpool_str669[sizeof("-webkit-plaintext")];
    char stringpool_str670[sizeof("bidi-override")];
    char stringpool_str671[sizeof("subpixel-antialiased")];
    char stringpool_str672[sizeof("threeddarkshadow")];
    char stringpool_str673[sizeof("-webkit-paged-y")];
    char stringpool_str674[sizeof("-webkit-paged-x")];
    char stringpool_str675[sizeof("-webkit-mini-control")];
    char stringpool_str676[sizeof("media-volume-slider-container")];
    char stringpool_str677[sizeof("-webkit-inline-box")];
    char stringpool_str678[sizeof("text-before-edge")];
    char stringpool_str679[sizeof("-webkit-nowrap")];
    char stringpool_str680[sizeof("mediumturquoise")];
    char stringpool_str681[sizeof("bounding-box")];
    char stringpool_str682[sizeof("lightskyblue")];
    char stringpool_str683[sizeof("tigrinya-er-abegede")];
    char stringpool_str684[sizeof("no-close-quote")];
    char stringpool_str685[sizeof("-webkit-pictograph")];
    char stringpool_str686[sizeof("w-resize")];
    char stringpool_str687[sizeof("progress-bar-value")];
    char stringpool_str688[sizeof("optimizelegibility")];
    char stringpool_str689[sizeof("tigrinya-et-abegede")];
    char stringpool_str690[sizeof("-webkit-inline-flex")];
    char stringpool_str691[sizeof("text-after-edge")];
    char stringpool_str692[sizeof("-webkit-flex")];
    char stringpool_str693[sizeof("media-play-button")];
    char stringpool_str694[sizeof("upper-greek")];
    char stringpool_str695[sizeof("-webkit-optimize-contrast")];
    char stringpool_str696[sizeof("discretionary-ligatures")];
    char stringpool_str697[sizeof("-webkit-max-content")];
    char stringpool_str698[sizeof("historical-ligatures")];
    char stringpool_str699[sizeof("yellowgreen")];
    char stringpool_str700[sizeof("media-toggle-closed-captions-button")];
    char stringpool_str701[sizeof("cjk-heavenly-stem")];
    char stringpool_str702[sizeof("-webkit-grabbing")];
    char stringpool_str703[sizeof("papayawhip")];
    char stringpool_str704[sizeof("-internal-active-list-box-selection")];
    char stringpool_str705[sizeof("burlywood")];
    char stringpool_str706[sizeof("window")];
    char stringpool_str707[sizeof("-webkit-isolate-override")];
    char stringpool_str708[sizeof("rating-level-indicator")];
    char stringpool_str709[sizeof("navajowhite")];
    char stringpool_str710[sizeof("-internal-extend-to-zoom")];
    char stringpool_str711[sizeof("media-overlay-play-button")];
    char stringpool_str712[sizeof("xx-large")];
    char stringpool_str713[sizeof("-webkit-focus-ring-color")];
    char stringpool_str714[sizeof("decimal-leading-zero")];
    char stringpool_str715[sizeof("wavy")];
    char stringpool_str716[sizeof("inactivecaptiontext")];
    char stringpool_str717[sizeof("-webkit-zoom-in")];
    char stringpool_str718[sizeof("media-time-remaining-display")];
    char stringpool_str719[sizeof("upper-hexadecimal")];
    char stringpool_str720[sizeof("media-exit-fullscreen-button")];
    char stringpool_str721[sizeof("-webkit-zoom-out")];
    char stringpool_str722[sizeof("-webkit-activelink")];
    char stringpool_str723[sizeof("continuous-capacity-level-indicator")];
    char stringpool_str724[sizeof("discrete-capacity-level-indicator")];
    char stringpool_str725[sizeof("-webkit-fill-available")];
    char stringpool_str726[sizeof("-internal-inactive-list-box-selection")];
    char stringpool_str727[sizeof("windowframe")];
    char stringpool_str728[sizeof("skip-white-space")];
    char stringpool_str729[sizeof("lower-norwegian")];
    char stringpool_str730[sizeof("no-discretionary-ligatures")];
    char stringpool_str731[sizeof("relevancy-level-indicator")];
    char stringpool_str732[sizeof("appworkspace")];
    char stringpool_str733[sizeof("sideways-right")];
    char stringpool_str734[sizeof("lightgoldenrodyellow")];
    char stringpool_str735[sizeof("windowtext")];
    char stringpool_str736[sizeof("-internal-active-list-box-selection-text")];
    char stringpool_str737[sizeof("optimizequality")];
    char stringpool_str738[sizeof("-internal-inactive-list-box-selection-text")];
    char stringpool_str739[sizeof("read-write-plaintext-only")];
    char stringpool_str740[sizeof("upper-norwegian")];
    char stringpool_str741[sizeof("-webkit-xxx-large")];
  };
static const struct stringpool_t stringpool_contents =
  {
    "on",
    "lr",
    "a5",
    "a3",
    "a4",
    "100",
    "600",
    "400",
    "500",
    "end",
    "900",
    "all",
    "tb",
    "800",
    "700",
    "300",
    "200",
    "inset",
    "none",
    "oromo",
    "inline",
    "instant",
    "b5",
    "tomato",
    "b4",
    "normal",
    "clone",
    "tan",
    "somali",
    "loose",
    "coral",
    "armenian",
    "ellipse",
    "oldlace",
    "isolate",
    "ease",
    "content",
    "thai",
    "ellipsis",
    "lao",
    "contain",
    "at",
    "contents",
    "small",
    "maroon",
    "salmon",
    "ltr",
    "smooth",
    "caret",
    "table",
    "unset",
    "collapse",
    "srgb",
    "monospace",
    "chocolate",
    "rl",
    "logical",
    "strict",
    "magenta",
    "column",
    "absolute",
    "baseline",
    "balance",
    "ethiopic",
    "ease-in",
    "inherit",
    "urdu",
    "color",
    "pre",
    "stretch",
    "antialiased",
    "bottom",
    "static",
    "font",
    "cell",
    "italic",
    "amharic",
    "sesame",
    "closest-side",
    "element",
    "radio",
    "both",
    "rtl",
    "portrait",
    "persian",
    "fast",
    "flat",
    "smaller",
    "inline-table",
    "teal",
    "under",
    "help",
    "legal",
    "small-caps",
    "table-cell",
    "coarse",
    "footnotes",
    "menu",
    "slateblue",
    "ease-out",
    "crosshair",
    "continuous",
    "manipulation",
    "peru",
    "cadetblue",
    "alternate",
    "underline",
    "medium",
    "start",
    "red",
    "intrinsic",
    "all-scroll",
    "interlace",
    "icon",
    "screen",
    "menulist",
    "scroll",
    "inside",
    "embed",
    "solid",
    "reset",
    "pre-line",
    "local",
    "script",
    "seashell",
    "clear",
    "center",
    "table-column",
    "serif",
    "bold",
    "central",
    "mathematical",
    "moccasin",
    "transparent",
    "dot",
    "hand",
    "octal",
    "meter",
    "letter",
    "landscape",
    "saturation",
    "steelblue",
    "ease-in-out",
    "cambodian",
    "ahead",
    "mediumblue",
    "indianred",
    "chartreuse",
    "sans-serif",
    "border",
    "bolder",
    "cornsilk",
    "each-line",
    "scrollbar",
    "closest-corner",
    "threedface",
    "block",
    "stroke",
    "step-start",
    "status-bar",
    "use-script",
    "thin",
    "lime",
    "black",
    "color-burn",
    "khmer",
    "lines",
    "linen",
    "alias",
    "mediumpurple",
    "slice",
    "sidama",
    "clip",
    "thistle",
    "self-start",
    "initial",
    "crimson",
    "listitem",
    "hide",
    "tibetan",
    "orchid",
    "circle",
    "hotpink",
    "markers",
    "indigo",
    "searchfield",
    "step-end",
    "lemonchiffon",
    "ethiopic-abegede",
    "dashed",
    "dotted",
    "infinite",
    "mediumslateblue",
    "lighten",
    "safe",
    "document",
    "list-item",
    "large",
    "asterisks",
    "mongolian",
    "orange",
    "destination-atop",
    "linear",
    "self-end",
    "lightsalmon",
    "fine",
    "fill",
    "destination-out",
    "farthest-corner",
    "tigre",
    "sienna",
    "decimal",
    "pointer",
    "mintcream",
    "aliceblue",
    "darken",
    "peachpuff",
    "accumulate",
    "miter",
    "right",
    "break-all",
    "kannada",
    "left",
    "green",
    "min-content",
    "dot-dash",
    "lighter",
    "afar",
    "darksalmon",
    "small-caption",
    "blanchedalmond",
    "progress",
    "true",
    "hangul",
    "table-caption",
    "larger",
    "sub",
    "slide",
    "lightblue",
    "disc",
    "plum",
    "after",
    "middle",
    "lightcoral",
    "ethiopic-halehame-so-et",
    "ethiopic-halehame-om-et",
    "ethiopic-halehame-ti-et",
    "ethiopic-halehame-am-et",
    "ethiopic-halehame-aa-et",
    "zoom",
    "ethiopic-abegede-ti-et",
    "ethiopic-abegede-am-et",
    "above",
    "palegreen",
    "bengali",
    "purple",
    "blue",
    "multiple",
    "digits",
    "hidden",
    "media-slider",
    "luminance",
    "cyan",
    "inline-grid",
    "gold",
    "auto",
    "inline-block",
    "khaki",
    "xor",
    "non-scaling-stroke",
    "lr-tb",
    "painted",
    "outset",
    "darkblue",
    "ledger",
    "drag",
    "horizontal",
    "aqua",
    "butt",
    "azure",
    "telugu",
    "zoom-in",
    "top",
    "button",
    "searchfield-decoration",
    "exact",
    "ethiopic-halehame-ti-er",
    "before",
    "ethiopic-halehame-aa-er",
    "pink",
    "palevioletred",
    "padding",
    "ethiopic-abegede-ti-er",
    "forestgreen",
    "nonzero",
    "thick",
    "skyblue",
    "seagreen",
    "filled",
    "tb-rl",
    "hangul-consonant",
    "malayalam",
    "hue",
    "span",
    "alpha",
    "katakana",
    "up",
    "blink",
    "myanmar",
    "row",
    "rl-tb",
    "vertical",
    "line-through",
    "palegoldenrod",
    "space",
    "pan-y",
    "zoom-out",
    "text",
    "new",
    "pan-x",
    "inline-axis",
    "no-repeat",
    "maximum",
    "caption",
    "contextual",
    "deeppink",
    "nowrap",
    "square",
    "oblique",
    "inline-box",
    "source-in",
    "step-middle",
    "orangered",
    "min-intrinsic",
    "arabic-indic",
    "col-resize",
    "lowercase",
    "content-box",
    "lightpink",
    "ethiopic-halehame-sid-et",
    "open",
    "mediumseagreen",
    "darkslateblue",
    "dynamic",
    "distribute",
    "fantasy",
    "aquamarine",
    "alphabetic",
    "vertical-lr",
    "relative",
    "push-button",
    "destination-in",
    "hard-light",
    "default",
    "progress-bar",
    "royalblue",
    "darkred",
    "katakana-iroha",
    "threedhighlight",
    "dot-dot-dash",
    "context-menu",
    "paused",
    "double",
    "text-top",
    "single",
    "down",
    "menutext",
    "source-out",
    "max-content",
    "buttonface",
    "currentcolor",
    "outside",
    "media-sliderthumb",
    "lower-latin",
    "economy",
    "triangle",
    "inline-flex",
    "off",
    "dodgerblue",
    "textarea",
    "flex",
    "round",
    "reset-size",
    "text-bottom",
    "darkorchid",
    "hiragana",
    "discard",
    "no-punctuation",
    "snow",
    "slow",
    "vertical-rl",
    "move",
    "show",
    "literal-punctuation",
    "preserve-3d",
    "repeat",
    "exclusion",
    "separate",
    "ridge",
    "no-drop",
    "ideographic",
    "mediumspringgreen",
    "lower-alpha",
    "fuchsia",
    "lower-roman",
    "wrap",
    "mediumorchid",
    "brown",
    "spell-out",
    "goldenrod",
    "limegreen",
    "searchfield-cancel-button",
    "hanging",
    "no-common-ligatures",
    "geometricprecision",
    "ethiopic-halehame-tig",
    "olive",
    "firebrick",
    "close-quote",
    "menulist-button",
    "highlight",
    "beige",
    "hiragana-iroha",
    "farthest-side",
    "violet",
    "invert",
    "lower-armenian",
    "soft-light",
    "cover",
    "grid",
    "mediumaquamarine",
    "wheat",
    "lightgreen",
    "gainsboro",
    "oriya",
    "square-button",
    "visible",
    "groove",
    "visual",
    "searchfield-results-decoration",
    "darkmagenta",
    "below",
    "bevel",
    "flex-start",
    "not-allowed",
    "silver",
    "glyphs",
    "crispedges",
    "column-reverse",
    "linearrgb",
    "darkgreen",
    "powderblue",
    "listbox",
    "textfield",
    "darkorange",
    "over",
    "block-axis",
    "avoid",
    "border-box",
    "infotext",
    "gray",
    "menulist-text",
    "scroll-position",
    "lightsteelblue",
    "georgian",
    "table-row",
    "replaced",
    "destination-over",
    "running",
    "no-change",
    "overline",
    "lightcyan",
    "inactiveborder",
    "amharic-abegede",
    "mediumvioletred",
    "color-dodge",
    "flex-end",
    "checkbox",
    "mistyrose",
    "darkviolet",
    "media-enter-fullscreen-button",
    "hebrew",
    "read-only",
    "lightseagreen",
    "binary",
    "visiblefill",
    "space-around",
    "grey",
    "bisque",
    "darkkhaki",
    "cursive",
    "darkcyan",
    "pre-wrap",
    "reverse",
    "darkgoldenrod",
    "s-resize",
    "n-resize",
    "e-resize",
    "cjk-ideographic",
    "darkseagreen",
    "upright",
    "slategrey",
    "slategray",
    "lavender",
    "blueviolet",
    "lawngreen",
    "tigrinya-er",
    "margin-box",
    "multiply",
    "luminosity",
    "plus-darker",
    "x-small",
    "difference",
    "alternate-reverse",
    "-webkit-control",
    "white",
    "progressive",
    "always",
    "saddlebrown",
    "horizontal-tb",
    "common-ligatures",
    "activecaption",
    "after-edge",
    "floralwhite",
    "tigrinya-et",
    "ns-resize",
    "buttonhighlight",
    "wait",
    "lightslategray",
    "olivedrab",
    "forwards",
    "message-box",
    "-webkit-isolate",
    "fixed",
    "copy",
    "double-circle",
    "no-drag",
    "turquoise",
    "buttontext",
    "super",
    "cjk-earthly-branch",
    "capitalize",
    "threedshadow",
    "source-over",
    "plus-lighter",
    "-webkit-auto",
    "lower-greek",
    "background",
    "horizontal-bt",
    "lightslategrey",
    "visiblepainted",
    "sticky",
    "scale-down",
    "evenodd",
    "activeborder",
    "ghostwhite",
    "inter-word",
    "table-row-group",
    "se-resize",
    "ne-resize",
    "gurmukhi",
    "gujarati",
    "padding-box",
    "menulist-textfield",
    "inner-spin-button",
    "table-column-group",
    "-webkit-right",
    "source-atop",
    "captiontext",
    "-webkit-center",
    "media-controls-background",
    "paleturquoise",
    "read-write",
    "row-resize",
    "-webkit-small-control",
    "button-bevel",
    "optimizespeed",
    "before-edge",
    "midnightblue",
    "nwse-resize",
    "media-mute-button",
    "-webkit-match-parent",
    "-webkit-min-content",
    "ethiopic-halehame-gez",
    "-webkit-link",
    "ethiopic-abegede-gez",
    "navy",
    "uppercase",
    "media-fullscreen-volume-slider",
    "visiblestroke",
    "-internal-presence",
    "repeat-y",
    "darkslategrey",
    "no-open-quote",
    "darkslategray",
    "repeat-x",
    "whitesmoke",
    "upper-latin",
    "deepskyblue",
    "sliderthumb-horizontal",
    "ivory",
    "open-quote",
    "infobackground",
    "vertical-text",
    "lightgrey",
    "lightgray",
    "yellow",
    "-webkit-left",
    "springgreen",
    "media-current-time-display",
    "no-historical-ligatures",
    "upper-alpha",
    "upper-roman",
    "darkgrey",
    "darkgray",
    "break-word",
    "table-footer-group",
    "row-reverse",
    "slider-vertical",
    "-webkit-fit-content",
    "darkolivegreen",
    "justify",
    "caps-lock-indicator",
    "vertical-right",
    "overlay",
    "dimgrey",
    "dimgray",
    "lower-hexadecimal",
    "backwards",
    "devanagari",
    "upper-armenian",
    "graytext",
    "lightyellow",
    "cornflowerblue",
    "honeydew",
    "rosybrown",
    "x-large",
    "inactivecaption",
    "media-fullscreen-volume-slider-thumb",
    "media-volume-slider",
    "wrap-reverse",
    "-webkit-baseline-middle",
    "wave",
    "table-header-group",
    "highlighttext",
    "antiquewhite",
    "after-white-space",
    "buttonshadow",
    "-webkit-grab",
    "greenyellow",
    "-webkit-text",
    "threedlightshadow",
    "space-between",
    "lavenderblush",
    "nesw-resize",
    "no-contextual",
    "xx-small",
    "media-controls-fullscreen-background",
    "slider-horizontal",
    "sandybrown",
    "-webkit-box",
    "-webkit-body",
    "sliderthumb-vertical",
    "sideways",
    "sw-resize",
    "nw-resize",
    "ew-resize",
    "media-volume-sliderthumb",
    "darkturquoise",
    "-webkit-plaintext",
    "bidi-override",
    "subpixel-antialiased",
    "threeddarkshadow",
    "-webkit-paged-y",
    "-webkit-paged-x",
    "-webkit-mini-control",
    "media-volume-slider-container",
    "-webkit-inline-box",
    "text-before-edge",
    "-webkit-nowrap",
    "mediumturquoise",
    "bounding-box",
    "lightskyblue",
    "tigrinya-er-abegede",
    "no-close-quote",
    "-webkit-pictograph",
    "w-resize",
    "progress-bar-value",
    "optimizelegibility",
    "tigrinya-et-abegede",
    "-webkit-inline-flex",
    "text-after-edge",
    "-webkit-flex",
    "media-play-button",
    "upper-greek",
    "-webkit-optimize-contrast",
    "discretionary-ligatures",
    "-webkit-max-content",
    "historical-ligatures",
    "yellowgreen",
    "media-toggle-closed-captions-button",
    "cjk-heavenly-stem",
    "-webkit-grabbing",
    "papayawhip",
    "-internal-active-list-box-selection",
    "burlywood",
    "window",
    "-webkit-isolate-override",
    "rating-level-indicator",
    "navajowhite",
    "-internal-extend-to-zoom",
    "media-overlay-play-button",
    "xx-large",
    "-webkit-focus-ring-color",
    "decimal-leading-zero",
    "wavy",
    "inactivecaptiontext",
    "-webkit-zoom-in",
    "media-time-remaining-display",
    "upper-hexadecimal",
    "media-exit-fullscreen-button",
    "-webkit-zoom-out",
    "-webkit-activelink",
    "continuous-capacity-level-indicator",
    "discrete-capacity-level-indicator",
    "-webkit-fill-available",
    "-internal-inactive-list-box-selection",
    "windowframe",
    "skip-white-space",
    "lower-norwegian",
    "no-discretionary-ligatures",
    "relevancy-level-indicator",
    "appworkspace",
    "sideways-right",
    "lightgoldenrodyellow",
    "windowtext",
    "-internal-active-list-box-selection-text",
    "optimizequality",
    "-internal-inactive-list-box-selection-text",
    "read-write-plaintext-only",
    "upper-norwegian",
    "-webkit-xxx-large"
  };
#define stringpool ((const char *) &stringpool_contents)
const struct Value *
CSSValueKeywordsHash::findValueImpl (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 742,
      MIN_WORD_LENGTH = 2,
      MAX_WORD_LENGTH = 42,
      MIN_HASH_VALUE = 18,
      MAX_HASH_VALUE = 5398
    };

  static const struct Value value_word_list[] =
    {
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str0, CSSValueOn},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1, CSSValueLr},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str2, CSSValueA5},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str3, CSSValueA3},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str4, CSSValueA4},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str5, CSSValue100},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str6, CSSValue600},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str7, CSSValue400},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str8, CSSValue500},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str9, CSSValueEnd},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str10, CSSValue900},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str11, CSSValueAll},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str12, CSSValueTb},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str13, CSSValue800},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str14, CSSValue700},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str15, CSSValue300},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str16, CSSValue200},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str17, CSSValueInset},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str18, CSSValueNone},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str19, CSSValueOromo},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str20, CSSValueInline},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str21, CSSValueInstant},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str22, CSSValueB5},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str23, CSSValueTomato},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str24, CSSValueB4},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str25, CSSValueNormal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str26, CSSValueClone},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str27, CSSValueTan},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str28, CSSValueSomali},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str29, CSSValueLoose},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str30, CSSValueCoral},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str31, CSSValueArmenian},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str32, CSSValueEllipse},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str33, CSSValueOldlace},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str34, CSSValueIsolate},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str35, CSSValueEase},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str36, CSSValueContent},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str37, CSSValueThai},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str38, CSSValueEllipsis},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str39, CSSValueLao},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str40, CSSValueContain},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str41, CSSValueAt},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str42, CSSValueContents},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str43, CSSValueSmall},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str44, CSSValueMaroon},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str45, CSSValueSalmon},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str46, CSSValueLtr},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str47, CSSValueSmooth},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str48, CSSValueCaret},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str49, CSSValueTable},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str50, CSSValueUnset},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str51, CSSValueCollapse},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str52, CSSValueSrgb},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str53, CSSValueMonospace},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str54, CSSValueChocolate},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str55, CSSValueRl},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str56, CSSValueLogical},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str57, CSSValueStrict},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str58, CSSValueMagenta},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str59, CSSValueColumn},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str60, CSSValueAbsolute},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str61, CSSValueBaseline},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str62, CSSValueBalance},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str63, CSSValueEthiopic},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str64, CSSValueEaseIn},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str65, CSSValueInherit},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str66, CSSValueUrdu},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str67, CSSValueColor},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str68, CSSValuePre},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str69, CSSValueStretch},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str70, CSSValueAntialiased},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str71, CSSValueBottom},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str72, CSSValueStatic},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str73, CSSValueFont},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str74, CSSValueCell},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str75, CSSValueItalic},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str76, CSSValueAmharic},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str77, CSSValueSesame},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str78, CSSValueClosestSide},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str79, CSSValueElement},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str80, CSSValueRadio},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str81, CSSValueBoth},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str82, CSSValueRtl},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str83, CSSValuePortrait},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str84, CSSValuePersian},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str85, CSSValueFast},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str86, CSSValueFlat},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str87, CSSValueSmaller},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str88, CSSValueInlineTable},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str89, CSSValueTeal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str90, CSSValueUnder},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str91, CSSValueHelp},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str92, CSSValueLegal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str93, CSSValueSmallCaps},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str94, CSSValueTableCell},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str95, CSSValueCoarse},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str96, CSSValueFootnotes},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str97, CSSValueMenu},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str98, CSSValueSlateblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str99, CSSValueEaseOut},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str100, CSSValueCrosshair},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str101, CSSValueContinuous},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str102, CSSValueManipulation},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str103, CSSValuePeru},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str104, CSSValueCadetblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str105, CSSValueAlternate},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str106, CSSValueUnderline},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str107, CSSValueMedium},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str108, CSSValueStart},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str109, CSSValueRed},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str110, CSSValueIntrinsic},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str111, CSSValueAllScroll},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str112, CSSValueInterlace},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str113, CSSValueIcon},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str114, CSSValueScreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str115, CSSValueMenulist},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str116, CSSValueScroll},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str117, CSSValueInside},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str118, CSSValueEmbed},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str119, CSSValueSolid},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str120, CSSValueReset},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str121, CSSValuePreLine},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str122, CSSValueLocal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str123, CSSValueScript},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str124, CSSValueSeashell},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str125, CSSValueClear},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str126, CSSValueCenter},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str127, CSSValueTableColumn},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str128, CSSValueSerif},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str129, CSSValueBold},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str130, CSSValueCentral},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str131, CSSValueMathematical},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str132, CSSValueMoccasin},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str133, CSSValueTransparent},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str134, CSSValueDot},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str135, CSSValueHand},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str136, CSSValueOctal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str137, CSSValueMeter},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str138, CSSValueLetter},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str139, CSSValueLandscape},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str140, CSSValueSaturation},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str141, CSSValueSteelblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str142, CSSValueEaseInOut},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str143, CSSValueCambodian},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str144, CSSValueAhead},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str145, CSSValueMediumblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str146, CSSValueIndianred},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str147, CSSValueChartreuse},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str148, CSSValueSansSerif},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str149, CSSValueBorder},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str150, CSSValueBolder},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str151, CSSValueCornsilk},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str152, CSSValueEachLine},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str153, CSSValueScrollbar},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str154, CSSValueClosestCorner},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str155, CSSValueThreedface},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str156, CSSValueBlock},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str157, CSSValueStroke},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str158, CSSValueStepStart},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str159, CSSValueStatusBar},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str160, CSSValueUseScript},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str161, CSSValueThin},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str162, CSSValueLime},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str163, CSSValueBlack},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str164, CSSValueColorBurn},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str165, CSSValueKhmer},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str166, CSSValueLines},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str167, CSSValueLinen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str168, CSSValueAlias},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str169, CSSValueMediumpurple},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str170, CSSValueSlice},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str171, CSSValueSidama},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str172, CSSValueClip},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str173, CSSValueThistle},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str174, CSSValueSelfStart},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str175, CSSValueInitial},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str176, CSSValueCrimson},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str177, CSSValueListitem},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str178, CSSValueHide},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str179, CSSValueTibetan},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str180, CSSValueOrchid},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str181, CSSValueCircle},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str182, CSSValueHotpink},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str183, CSSValueMarkers},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str184, CSSValueIndigo},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str185, CSSValueSearchfield},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str186, CSSValueStepEnd},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str187, CSSValueLemonchiffon},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str188, CSSValueEthiopicAbegede},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str189, CSSValueDashed},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str190, CSSValueDotted},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str191, CSSValueInfinite},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str192, CSSValueMediumslateblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str193, CSSValueLighten},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str194, CSSValueSafe},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str195, CSSValueDocument},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str196, CSSValueListItem},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str197, CSSValueLarge},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str198, CSSValueAsterisks},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str199, CSSValueMongolian},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str200, CSSValueOrange},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str201, CSSValueDestinationAtop},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str202, CSSValueLinear},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str203, CSSValueSelfEnd},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str204, CSSValueLightsalmon},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str205, CSSValueFine},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str206, CSSValueFill},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str207, CSSValueDestinationOut},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str208, CSSValueFarthestCorner},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str209, CSSValueTigre},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str210, CSSValueSienna},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str211, CSSValueDecimal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str212, CSSValuePointer},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str213, CSSValueMintcream},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str214, CSSValueAliceblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str215, CSSValueDarken},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str216, CSSValuePeachpuff},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str217, CSSValueAccumulate},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str218, CSSValueMiter},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str219, CSSValueRight},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str220, CSSValueBreakAll},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str221, CSSValueKannada},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str222, CSSValueLeft},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str223, CSSValueGreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str224, CSSValueMinContent},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str225, CSSValueDotDash},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str226, CSSValueLighter},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str227, CSSValueAfar},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str228, CSSValueDarksalmon},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str229, CSSValueSmallCaption},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str230, CSSValueBlanchedalmond},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str231, CSSValueProgress},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str232, CSSValueTrue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str233, CSSValueHangul},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str234, CSSValueTableCaption},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str235, CSSValueLarger},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str236, CSSValueSub},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str237, CSSValueSlide},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str238, CSSValueLightblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str239, CSSValueDisc},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str240, CSSValuePlum},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str241, CSSValueAfter},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str242, CSSValueMiddle},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str243, CSSValueLightcoral},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str244, CSSValueEthiopicHalehameSoEt},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str245, CSSValueEthiopicHalehameOmEt},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str246, CSSValueEthiopicHalehameTiEt},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str247, CSSValueEthiopicHalehameAmEt},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str248, CSSValueEthiopicHalehameAaEt},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str249, CSSValueZoom},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str250, CSSValueEthiopicAbegedeTiEt},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str251, CSSValueEthiopicAbegedeAmEt},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str252, CSSValueAbove},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str253, CSSValuePalegreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str254, CSSValueBengali},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str255, CSSValuePurple},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str256, CSSValueBlue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str257, CSSValueMultiple},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str258, CSSValueDigits},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str259, CSSValueHidden},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str260, CSSValueMediaSlider},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str261, CSSValueLuminance},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str262, CSSValueCyan},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str263, CSSValueInlineGrid},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str264, CSSValueGold},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str265, CSSValueAuto},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str266, CSSValueInlineBlock},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str267, CSSValueKhaki},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str268, CSSValueXor},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str269, CSSValueNonScalingStroke},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str270, CSSValueLrTb},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str271, CSSValuePainted},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str272, CSSValueOutset},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str273, CSSValueDarkblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str274, CSSValueLedger},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str275, CSSValueDrag},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str276, CSSValueHorizontal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str277, CSSValueAqua},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str278, CSSValueButt},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str279, CSSValueAzure},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str280, CSSValueTelugu},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str281, CSSValueZoomIn},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str282, CSSValueTop},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str283, CSSValueButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str284, CSSValueSearchfieldDecoration},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str285, CSSValueExact},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str286, CSSValueEthiopicHalehameTiEr},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str287, CSSValueBefore},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str288, CSSValueEthiopicHalehameAaEr},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str289, CSSValuePink},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str290, CSSValuePalevioletred},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str291, CSSValuePadding},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str292, CSSValueEthiopicAbegedeTiEr},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str293, CSSValueForestgreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str294, CSSValueNonzero},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str295, CSSValueThick},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str296, CSSValueSkyblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str297, CSSValueSeagreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str298, CSSValueFilled},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str299, CSSValueTbRl},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str300, CSSValueHangulConsonant},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str301, CSSValueMalayalam},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str302, CSSValueHue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str303, CSSValueSpan},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str304, CSSValueAlpha},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str305, CSSValueKatakana},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str306, CSSValueUp},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str307, CSSValueBlink},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str308, CSSValueMyanmar},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str309, CSSValueRow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str310, CSSValueRlTb},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str311, CSSValueVertical},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str312, CSSValueLineThrough},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str313, CSSValuePalegoldenrod},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str314, CSSValueSpace},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str315, CSSValuePanY},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str316, CSSValueZoomOut},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str317, CSSValueText},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str318, CSSValueNew},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str319, CSSValuePanX},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str320, CSSValueInlineAxis},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str321, CSSValueNoRepeat},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str322, CSSValueMaximum},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str323, CSSValueCaption},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str324, CSSValueContextual},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str325, CSSValueDeeppink},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str326, CSSValueNowrap},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str327, CSSValueSquare},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str328, CSSValueOblique},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str329, CSSValueInlineBox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str330, CSSValueSourceIn},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str331, CSSValueStepMiddle},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str332, CSSValueOrangered},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str333, CSSValueMinIntrinsic},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str334, CSSValueArabicIndic},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str335, CSSValueColResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str336, CSSValueLowercase},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str337, CSSValueContentBox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str338, CSSValueLightpink},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str339, CSSValueEthiopicHalehameSidEt},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str340, CSSValueOpen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str341, CSSValueMediumseagreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str342, CSSValueDarkslateblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str343, CSSValueDynamic},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str344, CSSValueDistribute},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str345, CSSValueFantasy},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str346, CSSValueAquamarine},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str347, CSSValueAlphabetic},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str348, CSSValueVerticalLr},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str349, CSSValueRelative},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str350, CSSValuePushButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str351, CSSValueDestinationIn},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str352, CSSValueHardLight},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str353, CSSValueDefault},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str354, CSSValueProgressBar},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str355, CSSValueRoyalblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str356, CSSValueDarkred},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str357, CSSValueKatakanaIroha},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str358, CSSValueThreedhighlight},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str359, CSSValueDotDotDash},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str360, CSSValueContextMenu},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str361, CSSValuePaused},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str362, CSSValueDouble},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str363, CSSValueTextTop},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str364, CSSValueSingle},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str365, CSSValueDown},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str366, CSSValueMenutext},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str367, CSSValueSourceOut},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str368, CSSValueMaxContent},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str369, CSSValueButtonface},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str370, CSSValueCurrentcolor},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str371, CSSValueOutside},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str372, CSSValueMediaSliderthumb},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str373, CSSValueLowerLatin},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str374, CSSValueEconomy},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str375, CSSValueTriangle},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str376, CSSValueInlineFlex},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str377, CSSValueOff},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str378, CSSValueDodgerblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str379, CSSValueTextarea},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str380, CSSValueFlex},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str381, CSSValueRound},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str382, CSSValueResetSize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str383, CSSValueTextBottom},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str384, CSSValueDarkorchid},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str385, CSSValueHiragana},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str386, CSSValueDiscard},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str387, CSSValueNoPunctuation},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str388, CSSValueSnow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str389, CSSValueSlow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str390, CSSValueVerticalRl},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str391, CSSValueMove},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str392, CSSValueShow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str393, CSSValueLiteralPunctuation},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str394, CSSValuePreserve3d},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str395, CSSValueRepeat},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str396, CSSValueExclusion},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str397, CSSValueSeparate},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str398, CSSValueRidge},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str399, CSSValueNoDrop},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str400, CSSValueIdeographic},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str401, CSSValueMediumspringgreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str402, CSSValueLowerAlpha},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str403, CSSValueFuchsia},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str404, CSSValueLowerRoman},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str405, CSSValueWrap},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str406, CSSValueMediumorchid},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str407, CSSValueBrown},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str408, CSSValueSpellOut},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str409, CSSValueGoldenrod},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str410, CSSValueLimegreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str411, CSSValueSearchfieldCancelButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str412, CSSValueHanging},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str413, CSSValueNoCommonLigatures},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str414, CSSValueGeometricprecision},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str415, CSSValueEthiopicHalehameTig},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str416, CSSValueOlive},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str417, CSSValueFirebrick},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str418, CSSValueCloseQuote},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str419, CSSValueMenulistButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str420, CSSValueHighlight},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str421, CSSValueBeige},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str422, CSSValueHiraganaIroha},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str423, CSSValueFarthestSide},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str424, CSSValueViolet},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str425, CSSValueInvert},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str426, CSSValueLowerArmenian},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str427, CSSValueSoftLight},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str428, CSSValueCover},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str429, CSSValueGrid},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str430, CSSValueMediumaquamarine},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str431, CSSValueWheat},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str432, CSSValueLightgreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str433, CSSValueGainsboro},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str434, CSSValueOriya},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str435, CSSValueSquareButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str436, CSSValueVisible},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str437, CSSValueGroove},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str438, CSSValueVisual},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str439, CSSValueSearchfieldResultsDecoration},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str440, CSSValueDarkmagenta},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str441, CSSValueBelow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str442, CSSValueBevel},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str443, CSSValueFlexStart},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str444, CSSValueNotAllowed},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str445, CSSValueSilver},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str446, CSSValueGlyphs},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str447, CSSValueCrispedges},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str448, CSSValueColumnReverse},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str449, CSSValueLinearrgb},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str450, CSSValueDarkgreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str451, CSSValuePowderblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str452, CSSValueListbox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str453, CSSValueTextfield},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str454, CSSValueDarkorange},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str455, CSSValueOver},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str456, CSSValueBlockAxis},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str457, CSSValueAvoid},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str458, CSSValueBorderBox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str459, CSSValueInfotext},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str460, CSSValueGray},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str461, CSSValueMenulistText},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str462, CSSValueScrollPosition},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str463, CSSValueLightsteelblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str464, CSSValueGeorgian},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str465, CSSValueTableRow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str466, CSSValueReplaced},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str467, CSSValueDestinationOver},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str468, CSSValueRunning},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str469, CSSValueNoChange},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str470, CSSValueOverline},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str471, CSSValueLightcyan},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str472, CSSValueInactiveborder},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str473, CSSValueAmharicAbegede},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str474, CSSValueMediumvioletred},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str475, CSSValueColorDodge},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str476, CSSValueFlexEnd},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str477, CSSValueCheckbox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str478, CSSValueMistyrose},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str479, CSSValueDarkviolet},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str480, CSSValueMediaEnterFullscreenButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str481, CSSValueHebrew},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str482, CSSValueReadOnly},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str483, CSSValueLightseagreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str484, CSSValueBinary},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str485, CSSValueVisiblefill},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str486, CSSValueSpaceAround},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str487, CSSValueGrey},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str488, CSSValueBisque},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str489, CSSValueDarkkhaki},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str490, CSSValueCursive},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str491, CSSValueDarkcyan},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str492, CSSValuePreWrap},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str493, CSSValueReverse},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str494, CSSValueDarkgoldenrod},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str495, CSSValueSResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str496, CSSValueNResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str497, CSSValueEResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str498, CSSValueCjkIdeographic},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str499, CSSValueDarkseagreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str500, CSSValueUpright},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str501, CSSValueSlategrey},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str502, CSSValueSlategray},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str503, CSSValueLavender},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str504, CSSValueBlueviolet},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str505, CSSValueLawngreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str506, CSSValueTigrinyaEr},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str507, CSSValueMarginBox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str508, CSSValueMultiply},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str509, CSSValueLuminosity},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str510, CSSValuePlusDarker},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str511, CSSValueXSmall},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str512, CSSValueDifference},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str513, CSSValueAlternateReverse},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str514, CSSValueWebkitControl},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str515, CSSValueWhite},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str516, CSSValueProgressive},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str517, CSSValueAlways},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str518, CSSValueSaddlebrown},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str519, CSSValueHorizontalTb},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str520, CSSValueCommonLigatures},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str521, CSSValueActivecaption},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str522, CSSValueAfterEdge},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str523, CSSValueFloralwhite},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str524, CSSValueTigrinyaEt},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str525, CSSValueNsResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str526, CSSValueButtonhighlight},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str527, CSSValueWait},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str528, CSSValueLightslategray},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str529, CSSValueOlivedrab},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str530, CSSValueForwards},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str531, CSSValueMessageBox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str532, CSSValueWebkitIsolate},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str533, CSSValueFixed},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str534, CSSValueCopy},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str535, CSSValueDoubleCircle},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str536, CSSValueNoDrag},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str537, CSSValueTurquoise},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str538, CSSValueButtontext},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str539, CSSValueSuper},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str540, CSSValueCjkEarthlyBranch},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str541, CSSValueCapitalize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str542, CSSValueThreedshadow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str543, CSSValueSourceOver},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str544, CSSValuePlusLighter},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str545, CSSValueWebkitAuto},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str546, CSSValueLowerGreek},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str547, CSSValueBackground},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str548, CSSValueHorizontalBt},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str549, CSSValueLightslategrey},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str550, CSSValueVisiblepainted},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str551, CSSValueSticky},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str552, CSSValueScaleDown},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str553, CSSValueEvenodd},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str554, CSSValueActiveborder},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str555, CSSValueGhostwhite},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str556, CSSValueInterWord},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str557, CSSValueTableRowGroup},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str558, CSSValueSeResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str559, CSSValueNeResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str560, CSSValueGurmukhi},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str561, CSSValueGujarati},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str562, CSSValuePaddingBox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str563, CSSValueMenulistTextfield},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str564, CSSValueInnerSpinButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str565, CSSValueTableColumnGroup},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str566, CSSValueWebkitRight},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str567, CSSValueSourceAtop},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str568, CSSValueCaptiontext},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str569, CSSValueWebkitCenter},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str570, CSSValueMediaControlsBackground},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str571, CSSValuePaleturquoise},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str572, CSSValueReadWrite},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str573, CSSValueRowResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str574, CSSValueWebkitSmallControl},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str575, CSSValueButtonBevel},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str576, CSSValueOptimizespeed},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str577, CSSValueBeforeEdge},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str578, CSSValueMidnightblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str579, CSSValueNwseResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str580, CSSValueMediaMuteButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str581, CSSValueWebkitMatchParent},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str582, CSSValueWebkitMinContent},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str583, CSSValueEthiopicHalehameGez},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str584, CSSValueWebkitLink},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str585, CSSValueEthiopicAbegedeGez},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str586, CSSValueNavy},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str587, CSSValueUppercase},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str588, CSSValueMediaFullscreenVolumeSlider},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str589, CSSValueVisiblestroke},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str590, CSSValueInternalPresence},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str591, CSSValueRepeatY},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str592, CSSValueDarkslategrey},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str593, CSSValueNoOpenQuote},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str594, CSSValueDarkslategray},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str595, CSSValueRepeatX},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str596, CSSValueWhitesmoke},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str597, CSSValueUpperLatin},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str598, CSSValueDeepskyblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str599, CSSValueSliderthumbHorizontal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str600, CSSValueIvory},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str601, CSSValueOpenQuote},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str602, CSSValueInfobackground},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str603, CSSValueVerticalText},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str604, CSSValueLightgrey},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str605, CSSValueLightgray},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str606, CSSValueYellow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str607, CSSValueWebkitLeft},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str608, CSSValueSpringgreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str609, CSSValueMediaCurrentTimeDisplay},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str610, CSSValueNoHistoricalLigatures},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str611, CSSValueUpperAlpha},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str612, CSSValueUpperRoman},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str613, CSSValueDarkgrey},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str614, CSSValueDarkgray},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str615, CSSValueBreakWord},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str616, CSSValueTableFooterGroup},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str617, CSSValueRowReverse},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str618, CSSValueSliderVertical},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str619, CSSValueWebkitFitContent},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str620, CSSValueDarkolivegreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str621, CSSValueJustify},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str622, CSSValueCapsLockIndicator},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str623, CSSValueVerticalRight},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str624, CSSValueOverlay},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str625, CSSValueDimgrey},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str626, CSSValueDimgray},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str627, CSSValueLowerHexadecimal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str628, CSSValueBackwards},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str629, CSSValueDevanagari},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str630, CSSValueUpperArmenian},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str631, CSSValueGraytext},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str632, CSSValueLightyellow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str633, CSSValueCornflowerblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str634, CSSValueHoneydew},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str635, CSSValueRosybrown},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str636, CSSValueXLarge},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str637, CSSValueInactivecaption},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str638, CSSValueMediaFullscreenVolumeSliderThumb},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str639, CSSValueMediaVolumeSlider},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str640, CSSValueWrapReverse},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str641, CSSValueWebkitBaselineMiddle},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str642, CSSValueWave},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str643, CSSValueTableHeaderGroup},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str644, CSSValueHighlighttext},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str645, CSSValueAntiquewhite},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str646, CSSValueAfterWhiteSpace},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str647, CSSValueButtonshadow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str648, CSSValueWebkitGrab},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str649, CSSValueGreenyellow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str650, CSSValueWebkitText},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str651, CSSValueThreedlightshadow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str652, CSSValueSpaceBetween},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str653, CSSValueLavenderblush},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str654, CSSValueNeswResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str655, CSSValueNoContextual},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str656, CSSValueXxSmall},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str657, CSSValueMediaControlsFullscreenBackground},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str658, CSSValueSliderHorizontal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str659, CSSValueSandybrown},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str660, CSSValueWebkitBox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str661, CSSValueWebkitBody},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str662, CSSValueSliderthumbVertical},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str663, CSSValueSideways},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str664, CSSValueSwResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str665, CSSValueNwResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str666, CSSValueEwResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str667, CSSValueMediaVolumeSliderthumb},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str668, CSSValueDarkturquoise},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str669, CSSValueWebkitPlaintext},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str670, CSSValueBidiOverride},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str671, CSSValueSubpixelAntialiased},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str672, CSSValueThreeddarkshadow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str673, CSSValueWebkitPagedY},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str674, CSSValueWebkitPagedX},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str675, CSSValueWebkitMiniControl},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str676, CSSValueMediaVolumeSliderContainer},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str677, CSSValueWebkitInlineBox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str678, CSSValueTextBeforeEdge},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str679, CSSValueWebkitNowrap},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str680, CSSValueMediumturquoise},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str681, CSSValueBoundingBox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str682, CSSValueLightskyblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str683, CSSValueTigrinyaErAbegede},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str684, CSSValueNoCloseQuote},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str685, CSSValueWebkitPictograph},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str686, CSSValueWResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str687, CSSValueProgressBarValue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str688, CSSValueOptimizelegibility},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str689, CSSValueTigrinyaEtAbegede},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str690, CSSValueWebkitInlineFlex},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str691, CSSValueTextAfterEdge},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str692, CSSValueWebkitFlex},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str693, CSSValueMediaPlayButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str694, CSSValueUpperGreek},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str695, CSSValueWebkitOptimizeContrast},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str696, CSSValueDiscretionaryLigatures},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str697, CSSValueWebkitMaxContent},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str698, CSSValueHistoricalLigatures},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str699, CSSValueYellowgreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str700, CSSValueMediaToggleClosedCaptionsButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str701, CSSValueCjkHeavenlyStem},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str702, CSSValueWebkitGrabbing},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str703, CSSValuePapayawhip},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str704, CSSValueInternalActiveListBoxSelection},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str705, CSSValueBurlywood},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str706, CSSValueWindow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str707, CSSValueWebkitIsolateOverride},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str708, CSSValueRatingLevelIndicator},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str709, CSSValueNavajowhite},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str710, CSSValueInternalExtendToZoom},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str711, CSSValueMediaOverlayPlayButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str712, CSSValueXxLarge},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str713, CSSValueWebkitFocusRingColor},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str714, CSSValueDecimalLeadingZero},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str715, CSSValueWavy},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str716, CSSValueInactivecaptiontext},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str717, CSSValueWebkitZoomIn},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str718, CSSValueMediaTimeRemainingDisplay},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str719, CSSValueUpperHexadecimal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str720, CSSValueMediaExitFullscreenButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str721, CSSValueWebkitZoomOut},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str722, CSSValueWebkitActivelink},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str723, CSSValueContinuousCapacityLevelIndicator},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str724, CSSValueDiscreteCapacityLevelIndicator},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str725, CSSValueWebkitFillAvailable},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str726, CSSValueInternalInactiveListBoxSelection},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str727, CSSValueWindowframe},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str728, CSSValueSkipWhiteSpace},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str729, CSSValueLowerNorwegian},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str730, CSSValueNoDiscretionaryLigatures},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str731, CSSValueRelevancyLevelIndicator},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str732, CSSValueAppworkspace},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str733, CSSValueSidewaysRight},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str734, CSSValueLightgoldenrodyellow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str735, CSSValueWindowtext},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str736, CSSValueInternalActiveListBoxSelectionText},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str737, CSSValueOptimizequality},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str738, CSSValueInternalInactiveListBoxSelectionText},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str739, CSSValueReadWritePlaintextOnly},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str740, CSSValueUpperNorwegian},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str741, CSSValueWebkitXxxLarge}
    };

  static const short lookup[] =
    {
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   0,  -1,
       -1,  -1,   1,   2,   3,   4,  -1,   5,   6,   7,
        8,   9,  10,  11,  12,  13,  14,  15,  16,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  17,  -1,  -1,  -1,  -1,  18,  -1,  -1,  -1,
       -1,  -1,  -1,  19,  -1,  -1,  20,  -1,  -1,  -1,
       -1,  -1,  21,  -1,  -1,  22,  23,  24,  -1,  -1,
       -1,  25,  26,  27,  28,  29,  30,  -1,  -1,  31,
       32,  -1,  33,  -1,  34,  -1,  35,  -1,  -1,  36,
       37,  38,  -1,  -1,  39,  40,  41,  -1,  42,  -1,
       43,  -1,  44,  45,  -1,  46,  47,  -1,  -1,  48,
       -1,  -1,  -1,  49,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  50,  -1,  51,  -1,  -1,  52,  -1,  53,  -1,
       -1,  -1,  -1,  -1,  54,  -1,  -1,  55,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  56,  -1,  57,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  58,  -1,  -1,  59,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  60,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  61,  -1,  62,
       -1,  -1,  -1,  63,  64,  65,  66,  67,  68,  69,
       -1,  70,  71,  -1,  72,  -1,  -1,  73,  -1,  -1,
       -1,  74,  75,  -1,  76,  77,  -1,  -1,  78,  -1,
       -1,  -1,  -1,  -1,  79,  80,  -1,  81,  -1,  -1,
       82,  -1,  83,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  84,  -1,  -1,  85,  86,  87,  88,
       89,  90,  91,  -1,  -1,  92,  -1,  -1,  93,  -1,
       94,  -1,  -1,  95,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  96,  -1,  -1,  97,  98,  -1,  -1,  -1,
       -1,  -1,  99,  -1, 100,  -1, 101,  -1,  -1,  -1,
      102, 103,  -1, 104,  -1,  -1,  -1, 105, 106,  -1,
      107,  -1,  -1, 108,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 109, 110,  -1, 111, 112, 113, 114,  -1, 115,
      116,  -1, 117,  -1, 118, 119,  -1,  -1,  -1, 120,
      121,  -1, 122, 123,  -1,  -1, 124,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 125,  -1,
       -1,  -1,  -1,  -1, 126,  -1,  -1,  -1, 127,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 128, 129, 130,
      131,  -1,  -1, 132,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 133,  -1,  -1,  -1, 134,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 135, 136,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 137,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 138,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 139,  -1,
       -1, 140,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 141,  -1,  -1, 142,  -1,  -1, 143,  -1, 144,
       -1,  -1, 145,  -1, 146,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      147,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      148,  -1,  -1,  -1,  -1, 149, 150,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 151,  -1,  -1, 152,  -1,
       -1,  -1,  -1,  -1,  -1, 153,  -1, 154,  -1,  -1,
       -1,  -1, 155,  -1,  -1,  -1, 156,  -1,  -1,  -1,
       -1, 157,  -1, 158,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 159, 160,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 161,  -1, 162, 163,  -1, 164,  -1,  -1,
       -1, 165, 166, 167,  -1, 168,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 169,  -1, 170, 171,  -1,  -1,  -1,
       -1, 172,  -1,  -1, 173,  -1,  -1, 174,  -1,  -1,
      175,  -1, 176,  -1, 177, 178,  -1, 179, 180,  -1,
       -1,  -1,  -1, 181,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 182,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 183,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      184,  -1,  -1, 185, 186,  -1,  -1,  -1, 187, 188,
       -1,  -1, 189, 190,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 191,  -1,  -1, 192,  -1,  -1, 193,
      194,  -1,  -1, 195,  -1,  -1,  -1,  -1,  -1, 196,
       -1,  -1,  -1, 197, 198,  -1,  -1,  -1, 199, 200,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 201,  -1,
       -1,  -1,  -1, 202,  -1,  -1,  -1,  -1, 203,  -1,
       -1, 204,  -1, 205,  -1,  -1, 206, 207,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 208,  -1,
      209,  -1,  -1,  -1,  -1, 210, 211,  -1,  -1, 212,
       -1,  -1,  -1,  -1, 213,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 214, 215,  -1,  -1,  -1,  -1, 216,  -1, 217,
       -1, 218, 219,  -1, 220,  -1,  -1, 221,  -1,  -1,
      222,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 223,  -1,
       -1, 224,  -1, 225,  -1,  -1, 226,  -1,  -1,  -1,
      227,  -1,  -1,  -1,  -1, 228, 229, 230,  -1,  -1,
       -1,  -1, 231,  -1,  -1,  -1,  -1, 232, 233, 234,
      235,  -1, 236,  -1,  -1,  -1,  -1, 237,  -1,  -1,
      238, 239,  -1,  -1, 240,  -1,  -1,  -1,  -1, 241,
       -1,  -1, 242,  -1,  -1, 243,  -1, 244, 245,  -1,
       -1, 246, 247,  -1,  -1, 248, 249,  -1,  -1,  -1,
       -1, 250, 251, 252,  -1,  -1, 253,  -1,  -1,  -1,
       -1, 254,  -1, 255, 256,  -1,  -1,  -1,  -1,  -1,
      257, 258, 259,  -1,  -1,  -1,  -1, 260,  -1,  -1,
       -1,  -1,  -1, 261, 262,  -1,  -1, 263, 264,  -1,
       -1,  -1,  -1, 265,  -1, 266,  -1,  -1,  -1,  -1,
       -1, 267, 268,  -1,  -1,  -1, 269, 270, 271, 272,
       -1,  -1,  -1, 273,  -1,  -1,  -1,  -1,  -1,  -1,
      274,  -1,  -1,  -1,  -1,  -1, 275,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 276,
       -1, 277,  -1,  -1,  -1, 278,  -1,  -1, 279,  -1,
       -1,  -1,  -1, 280, 281,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 282,  -1, 283, 284, 285,  -1,  -1, 286,
      287,  -1,  -1, 288,  -1, 289, 290, 291,  -1, 292,
      293,  -1,  -1,  -1,  -1,  -1, 294, 295, 296,  -1,
       -1, 297,  -1,  -1, 298, 299, 300,  -1,  -1,  -1,
       -1,  -1,  -1, 301, 302,  -1,  -1,  -1,  -1,  -1,
       -1, 303,  -1,  -1,  -1,  -1, 304, 305,  -1,  -1,
      306,  -1,  -1,  -1,  -1, 307,  -1,  -1,  -1, 308,
      309,  -1, 310, 311,  -1, 312, 313, 314,  -1,  -1,
       -1, 315, 316,  -1,  -1,  -1,  -1, 317,  -1,  -1,
      318,  -1, 319, 320, 321,  -1,  -1, 322, 323,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 324,  -1,  -1, 325, 326,
       -1,  -1,  -1,  -1,  -1, 327,  -1,  -1, 328,  -1,
       -1,  -1, 329,  -1, 330,  -1,  -1,  -1,  -1, 331,
       -1,  -1,  -1,  -1, 332, 333,  -1,  -1,  -1,  -1,
      334, 335, 336,  -1, 337,  -1,  -1, 338, 339, 340,
       -1, 341,  -1,  -1,  -1,  -1,  -1, 342,  -1,  -1,
       -1, 343,  -1, 344, 345, 346,  -1,  -1,  -1, 347,
      348,  -1,  -1,  -1, 349, 350,  -1,  -1,  -1, 351,
       -1,  -1, 352, 353, 354, 355, 356,  -1,  -1, 357,
       -1,  -1, 358, 359, 360,  -1, 361,  -1,  -1,  -1,
      362,  -1,  -1,  -1,  -1,  -1, 363, 364,  -1,  -1,
      365, 366, 367, 368,  -1,  -1, 369,  -1,  -1, 370,
      371, 372,  -1,  -1,  -1,  -1, 373,  -1,  -1, 374,
      375,  -1,  -1,  -1, 376,  -1,  -1,  -1,  -1, 377,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 378,
       -1, 379,  -1, 380,  -1,  -1,  -1,  -1, 381,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 382,  -1, 383,  -1, 384, 385,  -1, 386,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 387,  -1,
       -1,  -1,  -1, 388, 389, 390, 391, 392, 393, 394,
      395,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 396, 397,  -1, 398,  -1, 399,  -1,  -1,  -1,
      400, 401,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 402,
       -1,  -1,  -1,  -1,  -1,  -1, 403, 404,  -1, 405,
       -1, 406,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 407,
       -1, 408, 409,  -1,  -1,  -1,  -1,  -1, 410,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 411,  -1,  -1,
      412,  -1,  -1, 413,  -1,  -1, 414,  -1,  -1,  -1,
       -1, 415,  -1,  -1,  -1,  -1,  -1,  -1, 416,  -1,
       -1,  -1,  -1, 417,  -1,  -1,  -1, 418, 419,  -1,
       -1,  -1, 420,  -1, 421,  -1,  -1,  -1, 422,  -1,
       -1,  -1, 423,  -1,  -1,  -1,  -1, 424, 425,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 426,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 427, 428,  -1,
       -1, 429, 430,  -1,  -1,  -1,  -1, 431,  -1,  -1,
       -1,  -1, 432,  -1,  -1,  -1,  -1,  -1,  -1, 433,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 434, 435, 436,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 437,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 438,  -1,  -1, 439,
       -1,  -1,  -1,  -1, 440,  -1, 441,  -1,  -1,  -1,
       -1, 442,  -1,  -1,  -1, 443,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 444,  -1, 445,  -1, 446,  -1,  -1,
       -1,  -1, 447,  -1,  -1, 448,  -1,  -1,  -1,  -1,
      449,  -1,  -1,  -1,  -1, 450, 451,  -1,  -1,  -1,
       -1,  -1, 452,  -1, 453,  -1, 454,  -1, 455,  -1,
       -1,  -1,  -1, 456,  -1,  -1,  -1,  -1, 457,  -1,
       -1, 458, 459,  -1,  -1, 460, 461, 462,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 463,  -1,
       -1,  -1, 464, 465,  -1,  -1, 466,  -1,  -1,  -1,
       -1, 467, 468,  -1, 469, 470,  -1,  -1,  -1,  -1,
       -1,  -1, 471, 472,  -1,  -1, 473,  -1,  -1,  -1,
       -1,  -1, 474, 475,  -1,  -1, 476, 477,  -1,  -1,
      478,  -1,  -1,  -1, 479,  -1,  -1, 480, 481,  -1,
       -1,  -1,  -1,  -1,  -1, 482, 483,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      484,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 485,  -1,
      486,  -1,  -1, 487,  -1, 488,  -1,  -1,  -1,  -1,
      489,  -1,  -1, 490,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 491,  -1,  -1, 492,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 493,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 494,  -1, 495, 496, 497,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 498,  -1,  -1,
      499,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 500,
       -1,  -1,  -1, 501,  -1, 502,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 503,  -1,  -1,  -1, 504,  -1, 505,
       -1,  -1,  -1,  -1,  -1,  -1, 506,  -1, 507,  -1,
       -1,  -1,  -1,  -1,  -1, 508,  -1,  -1,  -1,  -1,
      509,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 510, 511,  -1,  -1,  -1,
      512,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 513,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      514,  -1, 515,  -1,  -1,  -1, 516,  -1, 517,  -1,
       -1,  -1,  -1,  -1,  -1, 518,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 519, 520, 521,  -1, 522, 523,
      524,  -1,  -1,  -1,  -1, 525, 526,  -1,  -1,  -1,
       -1,  -1,  -1, 527, 528,  -1,  -1,  -1,  -1,  -1,
      529,  -1,  -1, 530,  -1, 531,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 532,  -1,  -1,  -1, 533,  -1,  -1,
       -1,  -1,  -1,  -1, 534,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 535,  -1,  -1,  -1,  -1,  -1, 536,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 537,  -1,
      538,  -1, 539,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 540,  -1, 541,  -1,  -1,  -1, 542, 543,  -1,
       -1, 544,  -1,  -1,  -1, 545, 546,  -1, 547,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 548,  -1,
       -1,  -1, 549,  -1,  -1,  -1,  -1,  -1,  -1, 550,
       -1,  -1,  -1, 551,  -1, 552, 553,  -1,  -1,  -1,
       -1,  -1,  -1, 554,  -1,  -1,  -1, 555,  -1,  -1,
      556,  -1,  -1, 557,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 558, 559,
       -1, 560,  -1,  -1,  -1,  -1,  -1,  -1, 561,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 562, 563,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 564,  -1,  -1,  -1,  -1,  -1, 565,
       -1,  -1,  -1,  -1,  -1,  -1, 566,  -1,  -1, 567,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 568,  -1,
       -1, 569,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 570,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 571,  -1,  -1,  -1, 572,  -1,  -1,  -1,
      573, 574,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 575,
      576, 577,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 578, 579,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 580,
       -1,  -1, 581,  -1,  -1,  -1,  -1,  -1,  -1, 582,
       -1,  -1,  -1,  -1, 583,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 584,  -1, 585,  -1,  -1,  -1,  -1,  -1,
      586,  -1,  -1, 587,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 588,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 589,  -1, 590,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 591,  -1,  -1,  -1, 592, 593, 594,  -1,  -1,
       -1,  -1, 595,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 596,  -1,  -1,  -1,  -1,  -1, 597, 598, 599,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 600,  -1, 601,  -1,  -1, 602,  -1,  -1,  -1,
      603,  -1,  -1,  -1,  -1,  -1,  -1, 604,  -1, 605,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      606,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 607, 608,  -1,  -1,  -1,
       -1,  -1,  -1, 609,  -1,  -1,  -1,  -1, 610,  -1,
      611,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 612,  -1,
      613,  -1, 614,  -1,  -1,  -1,  -1, 615,  -1, 616,
       -1, 617,  -1,  -1, 618,  -1, 619,  -1,  -1,  -1,
       -1,  -1,  -1, 620,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 621,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      622,  -1, 623,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      624,  -1,  -1,  -1,  -1,  -1, 625,  -1, 626,  -1,
       -1, 627,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 628, 629,  -1,  -1,  -1,  -1, 630,  -1,
       -1, 631,  -1,  -1,  -1,  -1,  -1, 632,  -1, 633,
       -1,  -1, 634,  -1,  -1,  -1,  -1,  -1, 635,  -1,
       -1,  -1,  -1,  -1, 636,  -1, 637,  -1,  -1,  -1,
       -1, 638,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      639,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 640,  -1,
       -1,  -1,  -1,  -1,  -1, 641,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 642,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 643, 644,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 645,  -1,
      646,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 647,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 648,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 649,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 650,  -1,  -1,  -1,  -1, 651,  -1, 652,
       -1, 653,  -1, 654,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 655,  -1,
       -1,  -1, 656,  -1, 657,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 658,  -1,  -1, 659, 660,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 661, 662,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 663,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 664, 665, 666,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      667,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 668,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 669,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 670, 671,  -1, 672,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 673,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 674,  -1, 675,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 676,  -1,  -1,  -1,  -1,  -1,  -1, 677,
       -1, 678,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 679,  -1, 680,  -1,  -1,  -1, 681,  -1,  -1,
      682,  -1,  -1,  -1,  -1,  -1, 683,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 684,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 685,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 686,  -1,  -1,
       -1,  -1,  -1, 687,  -1,  -1,  -1,  -1,  -1, 688,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      689,  -1, 690,  -1, 691,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 692, 693,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 694,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 695,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 696,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 697,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 698, 699,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 700,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 701,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 702,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 703,  -1,  -1,  -1,  -1,  -1, 704,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 705,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 706,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 707,  -1, 708,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 709,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 710,  -1,  -1,  -1,  -1,  -1,  -1,
      711,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 712,  -1,  -1, 713,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 714,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 715,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 716,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      717,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 718,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 719,  -1,  -1,  -1, 720,  -1, 721,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 722,  -1,  -1,
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
       -1, 723,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      724,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 725,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 726,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 727,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 728,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 729,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      730,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 731,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 732,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 733,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 734,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 735,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 736,  -1,
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
       -1,  -1,  -1, 737,  -1,  -1,  -1,  -1,  -1,  -1,
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
       -1,  -1,  -1,  -1, 738,  -1,  -1,  -1,  -1,  -1,
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
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 739,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 740,  -1,  -1,  -1,
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
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 741
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = value_hash_function (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int index = lookup[key];

          if (index >= 0)
            {
              register const char *s = value_word_list[index].nameOffset + stringpool;

              if (*str == *s && !strncmp (str + 1, s + 1, len - 1) && s[len] == '\0')
                return &value_word_list[index];
            }
        }
    }
  return 0;
}

const Value* findValue(register const char* str, register unsigned int len)
{
    return CSSValueKeywordsHash::findValueImpl(str, len);
}

const char* getValueName(unsigned short id)
{
    if (id >= numCSSValueKeywords || id <= 0)
        return 0;
    return valueListStringPool + valueListStringOffsets[id];
}

bool isValueAllowedInMode(unsigned short id, CSSParserMode mode)
{
    switch (id) {
        case CSSValueInternalActiveListBoxSelection:
        case CSSValueInternalActiveListBoxSelectionText:
        case CSSValueInternalInactiveListBoxSelection:
        case CSSValueInternalInactiveListBoxSelectionText:
        case CSSValueInternalPresence:
        case CSSValueInternalExtendToZoom:
            return isUASheetBehavior(mode);
        case CSSValueWebkitFocusRingColor:
    case CSSValueWebkitText:
            return isUASheetBehavior(mode) || isQuirksModeBehavior(mode);
        default:
            return true;
    }
}

} // namespace WebCore
