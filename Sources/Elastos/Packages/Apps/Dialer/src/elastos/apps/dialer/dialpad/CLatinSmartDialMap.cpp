
#include "dialpad/CLatinSmartDialMap.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Dialpad {

CAR_INTERFACE_IMPL_2(CLatinSmartDialMap, Object, ILatinSmartDialMap, ISmartDialMap);

CAR_OBJECT_IMPL(CLatinSmartDialMap);

const Char32 CLatinSmartDialMap::LATIN_LETTERS_TO_DIGITS[] = {
    '2', '2', '2', // A,B,C -> 2
    '3', '3', '3', // D,E,F -> 3
    '4', '4', '4', // G,H,I -> 4
    '5', '5', '5', // J,K,L -> 5
    '6', '6', '6', // M,N,O -> 6
    '7', '7', '7', '7', // P,Q,R,S -> 7
    '8', '8', '8', // T,U,V -> 8
    '9', '9', '9', '9' // W,X,Y,Z -> 9
};

ECode CLatinSmartDialMap::IsValidDialpadAlphabeticChar(
    /* [in] */ Char32 ch,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);

    *result = (ch >= 'a' && ch <= 'z');
    return NOERROR;
}

ECode CLatinSmartDialMap::IsValidDialpadNumericChar(
    /* [in] */ Char32 ch,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);

    *result = (ch >= '0' && ch <= '9');
    return NOERROR;
}

ECode CLatinSmartDialMap::IsValidDialpadCharacter(
    /* [in] */ Char32 ch,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);

    Boolean isAlpha, isNumberic;
    *result = (IsValidDialpadAlphabeticChar(ch, &isAlpha), isAlpha
            || IsValidDialpadNumericChar(ch, &isNumberic), isNumberic);
    return NOERROR;
}

ECode CLatinSmartDialMap::NormalizeCharacter(
    /* [in] */ Char32 ch,
    /* [out] */ Char32* result)
{
    VALUE_NOT_NULL(result);

    switch (ch) {
        case 'À':
            *result = 'a';
            break;
        case 'Á':
            *result = 'a';
            break;
        case 'Â':
            *result = 'a';
            break;
        case 'Ã':
            *result = 'a';
            break;
        case 'Ä':
            *result = 'a';
            break;
        case 'Å':
            *result = 'a';
            break;
        case 'Ç':
            *result = 'c';
            break;
        case 'È':
            *result = 'e';
            break;
        case 'É':
            *result = 'e';
            break;
        case 'Ê':
            *result = 'e';
            break;
        case 'Ë':
            *result = 'e';
            break;
        case 'Ì':
            *result = 'i';
            break;
        case 'Í':
            *result = 'i';
            break;
        case 'Î':
            *result = 'i';
            break;
        case 'Ï':
            *result = 'i';
            break;
        case 'Ð':
            *result = 'd';
            break;
        case 'Ñ':
            *result = 'n';
            break;
        case 'Ò':
            *result = 'o';
            break;
        case 'Ó':
            *result = 'o';
            break;
        case 'Ô':
            *result = 'o';
            break;
        case 'Õ':
            *result = 'o';
            break;
        case 'Ö':
            *result = 'o';
            break;
        case '×':
            *result = 'x';
            break;
        case 'Ø':
            *result = 'o';
            break;
        case 'Ù':
            *result = 'u';
            break;
        case 'Ú':
            *result = 'u';
            break;
        case 'Û':
            *result = 'u';
            break;
        case 'Ü':
            *result = 'u';
            break;
        case 'Ý':
            *result = 'u';
            break;
        case 'à':
            *result = 'a';
            break;
        case 'á':
            *result = 'a';
            break;
        case 'â':
            *result = 'a';
            break;
        case 'ã':
            *result = 'a';
            break;
        case 'ä':
            *result = 'a';
            break;
        case 'å':
            *result = 'a';
            break;
        case 'ç':
            *result = 'c';
            break;
        case 'è':
            *result = 'e';
            break;
        case 'é':
            *result = 'e';
            break;
        case 'ê':
            *result = 'e';
            break;
        case 'ë':
            *result = 'e';
            break;
        case 'ì':
            *result = 'i';
            break;
        case 'í':
            *result = 'i';
            break;
        case 'î':
            *result = 'i';
            break;
        case 'ï':
            *result = 'i';
            break;
        case 'ð':
            *result = 'd';
            break;
        case 'ñ':
            *result = 'n';
            break;
        case 'ò':
            *result = 'o';
            break;
        case 'ó':
            *result = 'o';
            break;
        case 'ô':
            *result = 'o';
            break;
        case 'õ':
            *result = 'o';
            break;
        case 'ö':
            *result = 'o';
            break;
        case 'ø':
            *result = 'o';
            break;
        case 'ù':
            *result = 'u';
            break;
        case 'ú':
            *result = 'u';
            break;
        case 'û':
            *result = 'u';
            break;
        case 'ü':
            *result = 'u';
            break;
        case 'ý':
            *result = 'y';
            break;
        case 'ÿ':
            *result = 'y';
            break;
        case 'Ā':
            *result = 'a';
            break;
        case 'ā':
            *result = 'a';
            break;
        case 'Ă':
            *result = 'a';
            break;
        case 'ă':
            *result = 'a';
            break;
        case 'Ą':
            *result = 'a';
            break;
        case 'ą':
            *result = 'a';
            break;
        case 'Ć':
            *result = 'c';
            break;
        case 'ć':
            *result = 'c';
            break;
        case 'Ĉ':
            *result = 'c';
            break;
        case 'ĉ':
            *result = 'c';
            break;
        case 'Ċ':
            *result = 'c';
            break;
        case 'ċ':
            *result = 'c';
            break;
        case 'Č':
            *result = 'c';
            break;
        case 'č':
            *result = 'c';
            break;
        case 'Ď':
            *result = 'd';
            break;
        case 'ď':
            *result = 'd';
            break;
        case 'Đ':
            *result = 'd';
            break;
        case 'đ':
            *result = 'd';
            break;
        case 'Ē':
            *result = 'e';
            break;
        case 'ē':
            *result = 'e';
            break;
        case 'Ĕ':
            *result = 'e';
            break;
        case 'ĕ':
            *result = 'e';
            break;
        case 'Ė':
            *result = 'e';
            break;
        case 'ė':
            *result = 'e';
            break;
        case 'Ę':
            *result = 'e';
            break;
        case 'ę':
            *result = 'e';
            break;
        case 'Ě':
            *result = 'e';
            break;
        case 'ě':
            *result = 'e';
            break;
        case 'Ĝ':
            *result = 'g';
            break;
        case 'ĝ':
            *result = 'g';
            break;
        case 'Ğ':
            *result = 'g';
            break;
        case 'ğ':
            *result = 'g';
            break;
        case 'Ġ':
            *result = 'g';
            break;
        case 'ġ':
            *result = 'g';
            break;
        case 'Ģ':
            *result = 'g';
            break;
        case 'ģ':
            *result = 'g';
            break;
        case 'Ĥ':
            *result = 'h';
            break;
        case 'ĥ':
            *result = 'h';
            break;
        case 'Ħ':
            *result = 'h';
            break;
        case 'ħ':
            *result = 'h';
            break;
        case 'Ĩ':
            *result = 'i';
            break;
        case 'ĩ':
            *result = 'i';
            break;
        case 'Ī':
            *result = 'i';
            break;
        case 'ī':
            *result = 'i';
            break;
        case 'Ĭ':
            *result = 'i';
            break;
        case 'ĭ':
            *result = 'i';
            break;
        case 'Į':
            *result = 'i';
            break;
        case 'į':
            *result = 'i';
            break;
        case 'İ':
            *result = 'i';
            break;
        case 'ı':
            *result = 'i';
            break;
        case 'Ĵ':
            *result = 'j';
            break;
        case 'ĵ':
            *result = 'j';
            break;
        case 'Ķ':
            *result = 'k';
            break;
        case 'ķ':
            *result = 'k';
            break;
        case 'ĸ':
            *result = 'k';
            break;
        case 'Ĺ':
            *result = 'l';
            break;
        case 'ĺ':
            *result = 'l';
            break;
        case 'Ļ':
            *result = 'l';
            break;
        case 'ļ':
            *result = 'l';
            break;
        case 'Ľ':
            *result = 'l';
            break;
        case 'ľ':
            *result = 'l';
            break;
        case 'Ŀ':
            *result = 'l';
            break;
        case 'ŀ':
            *result = 'l';
            break;
        case 'Ł':
            *result = 'l';
            break;
        case 'ł':
            *result = 'l';
            break;
        case 'Ń':
            *result = 'n';
            break;
        case 'ń':
            *result = 'n';
            break;
        case 'Ņ':
            *result = 'n';
            break;
        case 'ņ':
            *result = 'n';
            break;
        case 'Ň':
            *result = 'n';
            break;
        case 'ň':
            *result = 'n';
            break;
        case 'Ō':
            *result = 'o';
            break;
        case 'ō':
            *result = 'o';
            break;
        case 'Ŏ':
            *result = 'o';
            break;
        case 'ŏ':
            *result = 'o';
            break;
        case 'Ő':
            *result = 'o';
            break;
        case 'ő':
            *result = 'o';
            break;
        case 'Ŕ':
            *result = 'r';
            break;
        case 'ŕ':
            *result = 'r';
            break;
        case 'Ŗ':
            *result = 'r';
            break;
        case 'ŗ':
            *result = 'r';
            break;
        case 'Ř':
            *result = 'r';
            break;
        case 'ř':
            *result = 'r';
            break;
        case 'Ś':
            *result = 's';
            break;
        case 'ś':
            *result = 's';
            break;
        case 'Ŝ':
            *result = 's';
            break;
        case 'ŝ':
            *result = 's';
            break;
        case 'Ş':
            *result = 's';
            break;
        case 'ş':
            *result = 's';
            break;
        case 'Š':
            *result = 's';
            break;
        case 'š':
            *result = 's';
            break;
        case 'Ţ':
            *result = 't';
            break;
        case 'ţ':
            *result = 't';
            break;
        case 'Ť':
            *result = 't';
            break;
        case 'ť':
            *result = 't';
            break;
        case 'Ŧ':
            *result = 't';
            break;
        case 'ŧ':
            *result = 't';
            break;
        case 'Ũ':
            *result = 'u';
            break;
        case 'ũ':
            *result = 'u';
            break;
        case 'Ū':
            *result = 'u';
            break;
        case 'ū':
            *result = 'u';
            break;
        case 'Ŭ':
            *result = 'u';
            break;
        case 'ŭ':
            *result = 'u';
            break;
        case 'Ů':
            *result = 'u';
            break;
        case 'ů':
            *result = 'u';
            break;
        case 'Ű':
            *result = 'u';
            break;
        case 'ű':
            *result = 'u';
            break;
        case 'Ų':
            *result = 'u';
            break;
        case 'ų':
            *result = 'u';
            break;
        case 'Ŵ':
            *result = 'w';
            break;
        case 'ŵ':
            *result = 'w';
            break;
        case 'Ŷ':
            *result = 'y';
            break;
        case 'ŷ':
            *result = 'y';
            break;
        case 'Ÿ':
            *result = 'y';
            break;
        case 'Ź':
            *result = 'z';
            break;
        case 'ź':
            *result = 'z';
            break;
        case 'Ż':
            *result = 'z';
            break;
        case 'ż':
            *result = 'z';
            break;
        case 'Ž':
            *result = 'z';
            break;
        case 'ž':
            *result = 'z';
            break;
        case 'ſ':
            *result = 's';
            break;
        case 'ƀ':
            *result = 'b';
            break;
        case 'Ɓ':
            *result = 'b';
            break;
        case 'Ƃ':
            *result = 'b';
            break;
        case 'ƃ':
            *result = 'b';
            break;
        case 'Ɔ':
            *result = 'o';
            break;
        case 'Ƈ':
            *result = 'c';
            break;
        case 'ƈ':
            *result = 'c';
            break;
        case 'Ɖ':
            *result = 'd';
            break;
        case 'Ɗ':
            *result = 'd';
            break;
        case 'Ƌ':
            *result = 'd';
            break;
        case 'ƌ':
            *result = 'd';
            break;
        case 'ƍ':
            *result = 'd';
            break;
        case 'Ɛ':
            *result = 'e';
            break;
        case 'Ƒ':
            *result = 'f';
            break;
        case 'ƒ':
            *result = 'f';
            break;
        case 'Ɠ':
            *result = 'g';
            break;
        case 'Ɣ':
            *result = 'g';
            break;
        case 'Ɩ':
            *result = 'i';
            break;
        case 'Ɨ':
            *result = 'i';
            break;
        case 'Ƙ':
            *result = 'k';
            break;
        case 'ƙ':
            *result = 'k';
            break;
        case 'ƚ':
            *result = 'l';
            break;
        case 'ƛ':
            *result = 'l';
            break;
        case 'Ɯ':
            *result = 'w';
            break;
        case 'Ɲ':
            *result = 'n';
            break;
        case 'ƞ':
            *result = 'n';
            break;
        case 'Ɵ':
            *result = 'o';
            break;
        case 'Ơ':
            *result = 'o';
            break;
        case 'ơ':
            *result = 'o';
            break;
        case 'Ƥ':
            *result = 'p';
            break;
        case 'ƥ':
            *result = 'p';
            break;
        case 'ƫ':
            *result = 't';
            break;
        case 'Ƭ':
            *result = 't';
            break;
        case 'ƭ':
            *result = 't';
            break;
        case 'Ʈ':
            *result = 't';
            break;
        case 'Ư':
            *result = 'u';
            break;
        case 'ư':
            *result = 'u';
            break;
        case 'Ʊ':
            *result = 'y';
            break;
        case 'Ʋ':
            *result = 'v';
            break;
        case 'Ƴ':
            *result = 'y';
            break;
        case 'ƴ':
            *result = 'y';
            break;
        case 'Ƶ':
            *result = 'z';
            break;
        case 'ƶ':
            *result = 'z';
            break;
        case 'ƿ':
            *result = 'w';
            break;
        case 'Ǎ':
            *result = 'a';
            break;
        case 'ǎ':
            *result = 'a';
            break;
        case 'Ǐ':
            *result = 'i';
            break;
        case 'ǐ':
            *result = 'i';
            break;
        case 'Ǒ':
            *result = 'o';
            break;
        case 'ǒ':
            *result = 'o';
            break;
        case 'Ǔ':
            *result = 'u';
            break;
        case 'ǔ':
            *result = 'u';
            break;
        case 'Ǖ':
            *result = 'u';
            break;
        case 'ǖ':
            *result = 'u';
            break;
        case 'Ǘ':
            *result = 'u';
            break;
        case 'ǘ':
            *result = 'u';
            break;
        case 'Ǚ':
            *result = 'u';
            break;
        case 'ǚ':
            *result = 'u';
            break;
        case 'Ǜ':
            *result = 'u';
            break;
        case 'ǜ':
            *result = 'u';
            break;
        case 'Ǟ':
            *result = 'a';
            break;
        case 'ǟ':
            *result = 'a';
            break;
        case 'Ǡ':
            *result = 'a';
            break;
        case 'ǡ':
            *result = 'a';
            break;
        case 'Ǥ':
            *result = 'g';
            break;
        case 'ǥ':
            *result = 'g';
            break;
        case 'Ǧ':
            *result = 'g';
            break;
        case 'ǧ':
            *result = 'g';
            break;
        case 'Ǩ':
            *result = 'k';
            break;
        case 'ǩ':
            *result = 'k';
            break;
        case 'Ǫ':
            *result = 'o';
            break;
        case 'ǫ':
            *result = 'o';
            break;
        case 'Ǭ':
            *result = 'o';
            break;
        case 'ǭ':
            *result = 'o';
            break;
        case 'ǰ':
            *result = 'j';
            break;
        case 'ǲ':
            *result = 'd';
            break;
        case 'Ǵ':
            *result = 'g';
            break;
        case 'ǵ':
            *result = 'g';
            break;
        case 'Ƿ':
            *result = 'w';
            break;
        case 'Ǹ':
            *result = 'n';
            break;
        case 'ǹ':
            *result = 'n';
            break;
        case 'Ǻ':
            *result = 'a';
            break;
        case 'ǻ':
            *result = 'a';
            break;
        case 'Ǿ':
            *result = 'o';
            break;
        case 'ǿ':
            *result = 'o';
            break;
        case 'Ȁ':
            *result = 'a';
            break;
        case 'ȁ':
            *result = 'a';
            break;
        case 'Ȃ':
            *result = 'a';
            break;
        case 'ȃ':
            *result = 'a';
            break;
        case 'Ȅ':
            *result = 'e';
            break;
        case 'ȅ':
            *result = 'e';
            break;
        case 'Ȇ':
            *result = 'e';
            break;
        case 'ȇ':
            *result = 'e';
            break;
        case 'Ȉ':
            *result = 'i';
            break;
        case 'ȉ':
            *result = 'i';
            break;
        case 'Ȋ':
            *result = 'i';
            break;
        case 'ȋ':
            *result = 'i';
            break;
        case 'Ȍ':
            *result = 'o';
            break;
        case 'ȍ':
            *result = 'o';
            break;
        case 'Ȏ':
            *result = 'o';
            break;
        case 'ȏ':
            *result = 'o';
            break;
        case 'Ȑ':
            *result = 'r';
            break;
        case 'ȑ':
            *result = 'r';
            break;
        case 'Ȓ':
            *result = 'r';
            break;
        case 'ȓ':
            *result = 'r';
            break;
        case 'Ȕ':
            *result = 'u';
            break;
        case 'ȕ':
            *result = 'u';
            break;
        case 'Ȗ':
            *result = 'u';
            break;
        case 'ȗ':
            *result = 'u';
            break;
        case 'Ș':
            *result = 's';
            break;
        case 'ș':
            *result = 's';
            break;
        case 'Ț':
            *result = 't';
            break;
        case 'ț':
            *result = 't';
            break;
        case 'Ȝ':
            *result = 'y';
            break;
        case 'ȝ':
            *result = 'y';
            break;
        case 'Ȟ':
            *result = 'h';
            break;
        case 'ȟ':
            *result = 'h';
            break;
        case 'Ȥ':
            *result = 'z';
            break;
        case 'ȥ':
            *result = 'z';
            break;
        case 'Ȧ':
            *result = 'a';
            break;
        case 'ȧ':
            *result = 'a';
            break;
        case 'Ȩ':
            *result = 'e';
            break;
        case 'ȩ':
            *result = 'e';
            break;
        case 'Ȫ':
            *result = 'o';
            break;
        case 'ȫ':
            *result = 'o';
            break;
        case 'Ȭ':
            *result = 'o';
            break;
        case 'ȭ':
            *result = 'o';
            break;
        case 'Ȯ':
            *result = 'o';
            break;
        case 'ȯ':
            *result = 'o';
            break;
        case 'Ȱ':
            *result = 'o';
            break;
        case 'ȱ':
            *result = 'o';
            break;
        case 'Ȳ':
            *result = 'y';
            break;
        case 'ȳ':
            *result = 'y';
            break;
        case 'A':
            *result = 'a';
            break;
        case 'B':
            *result = 'b';
            break;
        case 'C':
            *result = 'c';
            break;
        case 'D':
            *result = 'd';
            break;
        case 'E':
            *result = 'e';
            break;
        case 'F':
            *result = 'f';
            break;
        case 'G':
            *result = 'g';
            break;
        case 'H':
            *result = 'h';
            break;
        case 'I':
            *result = 'i';
            break;
        case 'J':
            *result = 'j';
            break;
        case 'K':
            *result = 'k';
            break;
        case 'L':
            *result = 'l';
            break;
        case 'M':
            *result = 'm';
            break;
        case 'N':
            *result = 'n';
            break;
        case 'O':
            *result = 'o';
            break;
        case 'P':
            *result = 'p';
            break;
        case 'Q':
            *result = 'q';
            break;
        case 'R':
            *result = 'r';
            break;
        case 'S':
            *result = 's';
            break;
        case 'T':
            *result = 't';
            break;
        case 'U':
            *result = 'u';
            break;
        case 'V':
            *result = 'v';
            break;
        case 'W':
            *result = 'w';
            break;
        case 'X':
            *result = 'x';
            break;
        case 'Y':
            *result = 'y';
            break;
        case 'Z':
            *result = 'z';
            break;
        default:
            *result = ch;
            break;
    }

    return NOERROR;
}

ECode CLatinSmartDialMap::GetDialpadIndex(
    /* [in] */ Char32 ch,
    /* [out] */ Byte* result)
{
    VALUE_NOT_NULL(result);

    if (ch >= '0' && ch <= '9') {
        *result = (Byte) (ch - '0');
    }
    else if (ch >= 'a' && ch <= 'z') {
        *result = (byte) (LATIN_LETTERS_TO_DIGITS[ch - 'a'] - '0');
    }
    else {
        *result = -1;
    }

    return NOERROR;
}

ECode CLatinSmartDialMap::GetDialpadNumericCharacter(
    /* [in] */ Char32 ch,
    /* [out] */ Char32* result)
{
    VALUE_NOT_NULL(result);

    if (ch >= 'a' && ch <= 'z') {
        *result = LATIN_LETTERS_TO_DIGITS[ch - 'a'];
        return NOERROR;
    }
    *result = ch;
    return NOERROR;
}

} // Dialpad
} // Dialer
} // Apps
} // Elastos
