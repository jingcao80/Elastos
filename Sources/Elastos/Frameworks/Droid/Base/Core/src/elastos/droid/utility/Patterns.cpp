
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/utility/Patterns.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/Character.h>

using Elastos::Core::Character;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Regex::IMatchResult;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;

namespace Elastos {
namespace Droid {
namespace Utility {

static AutoPtr<IPattern> InitPattern(
    /* [in] */ const String& regex)
{
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);

    AutoPtr<IPattern> pattern;
    helper->Compile(regex, (IPattern**)&pattern);
    return pattern;
}

const String Patterns::TOP_LEVEL_DOMAIN_STR =
    String("((aero|arpa|asia|a[cdefgilmnoqrstuwxz])"
        "|(biz|b[abdefghijmnorstvwyz])"
        "|(cat|com|coop|c[acdfghiklmnoruvxyz])"
        "|d[ejkmoz]"
        "|(edu|e[cegrstu])"
        "|f[ijkmor]"
        "|(gov|g[abdefghilmnpqrstuwy])"
        "|h[kmnrtu]"
        "|(info|int|i[delmnoqrst])"
        "|(jobs|j[emop])"
        "|k[eghimnprwyz]"
        "|l[abcikrstuvy]"
        "|(mil|mobi|museum|m[acdeghklmnopqrstuvwxyz])"
        "|(name|net|n[acefgilopruz])"
        "|(org|om)"
        "|(pro|p[aefghklmnrstwy])"
        "|qa"
        "|r[eosuw]"
        "|s[abcdeghijklmnortuvyz]"
        "|(tel|travel|t[cdfghjklmnoprtvwz])"
        "|u[agksyz]"
        "|v[aceginu]"
        "|w[fs]"
        "|(\u03b4\u03bf\u03ba\u03b9\u03bc\u03ae|\u0438\u0441\u043f\u044b\u0442\u0430\u043d\u0438\u0435|\u0440\u0444|\u0441\u0440\u0431|\u05d8\u05e2\u05e1\u05d8|\u0622\u0632\u0645\u0627\u06cc\u0634\u06cc|\u0625\u062e\u062a\u0628\u0627\u0631|\u0627\u0644\u0627\u0631\u062f\u0646|\u0627\u0644\u062c\u0632\u0627\u0626\u0631|\u0627\u0644\u0633\u0639\u0648\u062f\u064a\u0629|\u0627\u0644\u0645\u063a\u0631\u0628|\u0627\u0645\u0627\u0631\u0627\u062a|\u0628\u06be\u0627\u0631\u062a|\u062a\u0648\u0646\u0633|\u0633\u0648\u0631\u064a\u0629|\u0641\u0644\u0633\u0637\u064a\u0646|\u0642\u0637\u0631|\u0645\u0635\u0631|\u092a\u0930\u0940\u0915\u094d\u0937\u093e|\u092d\u093e\u0930\u0924|\u09ad\u09be\u09b0\u09a4|\u0a2d\u0a3e\u0a30\u0a24|\u0aad\u0abe\u0ab0\u0aa4|\u0b87\u0ba8\u0bcd\u0ba4\u0bbf\u0baf\u0bbe|\u0b87\u0bb2\u0b99\u0bcd\u0b95\u0bc8|\u0b9a\u0bbf\u0b99\u0bcd\u0b95\u0baa\u0bcd\u0baa\u0bc2\u0bb0\u0bcd|\u0baa\u0bb0\u0bbf\u0b9f\u0bcd\u0b9a\u0bc8|\u0c2d\u0c3e\u0c30\u0c24\u0c4d|\u0dbd\u0d82\u0d9a\u0dcf|\u0e44\u0e17\u0e22|\u30c6\u30b9\u30c8|\u4e2d\u56fd|\u4e2d\u570b|\u53f0\u6e7e|\u53f0\u7063|\u65b0\u52a0\u5761|\u6d4b\u8bd5|\u6e2c\u8a66|\u9999\u6e2f|\ud14c\uc2a4\ud2b8|\ud55c\uad6d|xn\\-\\-0zwm56d|xn\\-\\-11b5bs3a9aj6g|xn\\-\\-3e0b707e|xn\\-\\-45brj9c|xn\\-\\-80akhbyknj4f|xn\\-\\-90a3ac|xn\\-\\-9t4b11yi5a|xn\\-\\-clchc0ea0b2g2a9gcd|xn\\-\\-deba0ad|xn\\-\\-fiqs8s|xn\\-\\-fiqz9s|xn\\-\\-fpcrj9c3d|xn\\-\\-fzc2c9e2c|xn\\-\\-g6w251d|xn\\-\\-gecrj9c|xn\\-\\-h2brj9c|xn\\-\\-hgbk6aj7f53bba|xn\\-\\-hlcj6aya9esc7a|xn\\-\\-j6w193g|xn\\-\\-jxalpdlp|xn\\-\\-kgbechtv|xn\\-\\-kprw13d|xn\\-\\-kpry57d|xn\\-\\-lgbbat1ad8j|xn\\-\\-mgbaam7a8h|xn\\-\\-mgbayh7gpa|xn\\-\\-mgbbh1a71e|xn\\-\\-mgbc0a9azcg|xn\\-\\-mgberp4a5d4ar|xn\\-\\-o3cw4h|xn\\-\\-ogbpf8fl|xn\\-\\-p1ai|xn\\-\\-pgbs0dh|xn\\-\\-s9brj9c|xn\\-\\-wgbh1c|xn\\-\\-wgbl6a|xn\\-\\-xkc2al3hye2a|xn\\-\\-xkc2dl3a5ee0h|xn\\-\\-yfro4i67o|xn\\-\\-ygbi2ammx|xn\\-\\-zckzah|xxx)"
        "|y[et]"
        "|z[amw])");

const String Patterns::TOP_LEVEL_DOMAIN_STR_FOR_WEB_URL =
    String("(?:"
        "(?:aero|arpa|asia|a[cdefgilmnoqrstuwxz])"
        "|(?:biz|b[abdefghijmnorstvwyz])"
        "|(?:cat|com|coop|c[acdfghiklmnoruvxyz])"
        "|d[ejkmoz]"
        "|(?:edu|e[cegrstu])"
        "|f[ijkmor]"
        "|(?:gov|g[abdefghilmnpqrstuwy])"
        "|h[kmnrtu]"
        "|(?:info|int|i[delmnoqrst])"
        "|(?:jobs|j[emop])"
        "|k[eghimnprwyz]"
        "|l[abcikrstuvy]"
        "|(?:mil|mobi|museum|m[acdeghklmnopqrstuvwxyz])"
        "|(?:name|net|n[acefgilopruz])"
        "|(?:org|om)"
        "|(?:pro|p[aefghklmnrstwy])"
        "|qa"
        "|r[eosuw]"
        "|s[abcdeghijklmnortuvyz]"
        "|(?:tel|travel|t[cdfghjklmnoprtvwz])"
        "|u[agksyz]"
        "|v[aceginu]"
        "|w[fs]"
        "|(?:\u03b4\u03bf\u03ba\u03b9\u03bc\u03ae|\u0438\u0441\u043f\u044b\u0442\u0430\u043d\u0438\u0435|\u0440\u0444|\u0441\u0440\u0431|\u05d8\u05e2\u05e1\u05d8|\u0622\u0632\u0645\u0627\u06cc\u0634\u06cc|\u0625\u062e\u062a\u0628\u0627\u0631|\u0627\u0644\u0627\u0631\u062f\u0646|\u0627\u0644\u062c\u0632\u0627\u0626\u0631|\u0627\u0644\u0633\u0639\u0648\u062f\u064a\u0629|\u0627\u0644\u0645\u063a\u0631\u0628|\u0627\u0645\u0627\u0631\u0627\u062a|\u0628\u06be\u0627\u0631\u062a|\u062a\u0648\u0646\u0633|\u0633\u0648\u0631\u064a\u0629|\u0641\u0644\u0633\u0637\u064a\u0646|\u0642\u0637\u0631|\u0645\u0635\u0631|\u092a\u0930\u0940\u0915\u094d\u0937\u093e|\u092d\u093e\u0930\u0924|\u09ad\u09be\u09b0\u09a4|\u0a2d\u0a3e\u0a30\u0a24|\u0aad\u0abe\u0ab0\u0aa4|\u0b87\u0ba8\u0bcd\u0ba4\u0bbf\u0baf\u0bbe|\u0b87\u0bb2\u0b99\u0bcd\u0b95\u0bc8|\u0b9a\u0bbf\u0b99\u0bcd\u0b95\u0baa\u0bcd\u0baa\u0bc2\u0bb0\u0bcd|\u0baa\u0bb0\u0bbf\u0b9f\u0bcd\u0b9a\u0bc8|\u0c2d\u0c3e\u0c30\u0c24\u0c4d|\u0dbd\u0d82\u0d9a\u0dcf|\u0e44\u0e17\u0e22|\u30c6\u30b9\u30c8|\u4e2d\u56fd|\u4e2d\u570b|\u53f0\u6e7e|\u53f0\u7063|\u65b0\u52a0\u5761|\u6d4b\u8bd5|\u6e2c\u8a66|\u9999\u6e2f|\ud14c\uc2a4\ud2b8|\ud55c\uad6d|xn\\-\\-0zwm56d|xn\\-\\-11b5bs3a9aj6g|xn\\-\\-3e0b707e|xn\\-\\-45brj9c|xn\\-\\-80akhbyknj4f|xn\\-\\-90a3ac|xn\\-\\-9t4b11yi5a|xn\\-\\-clchc0ea0b2g2a9gcd|xn\\-\\-deba0ad|xn\\-\\-fiqs8s|xn\\-\\-fiqz9s|xn\\-\\-fpcrj9c3d|xn\\-\\-fzc2c9e2c|xn\\-\\-g6w251d|xn\\-\\-gecrj9c|xn\\-\\-h2brj9c|xn\\-\\-hgbk6aj7f53bba|xn\\-\\-hlcj6aya9esc7a|xn\\-\\-j6w193g|xn\\-\\-jxalpdlp|xn\\-\\-kgbechtv|xn\\-\\-kprw13d|xn\\-\\-kpry57d|xn\\-\\-lgbbat1ad8j|xn\\-\\-mgbaam7a8h|xn\\-\\-mgbayh7gpa|xn\\-\\-mgbbh1a71e|xn\\-\\-mgbc0a9azcg|xn\\-\\-mgberp4a5d4ar|xn\\-\\-o3cw4h|xn\\-\\-ogbpf8fl|xn\\-\\-p1ai|xn\\-\\-pgbs0dh|xn\\-\\-s9brj9c|xn\\-\\-wgbh1c|xn\\-\\-wgbl6a|xn\\-\\-xkc2al3hye2a|xn\\-\\-xkc2dl3a5ee0h|xn\\-\\-yfro4i67o|xn\\-\\-ygbi2ammx|xn\\-\\-zckzah|xxx)"
        "|y[et]"
        "|z[amw]))");

const String Patterns::GOOD_IRI_CHAR =
    String("a-zA-Z0-9\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF");


const AutoPtr<IPattern> Patterns::TOP_LEVEL_DOMAIN
     = InitPattern(String(
        "((aero|arpa|asia|a[cdefgilmnoqrstuwxz])"
        "|(biz|b[abdefghijmnorstvwyz])"
        "|(cat|com|coop|c[acdfghiklmnoruvxyz])"
        "|d[ejkmoz]"
        "|(edu|e[cegrstu])"
        "|f[ijkmor]"
        "|(gov|g[abdefghilmnpqrstuwy])"
        "|h[kmnrtu]"
        "|(info|int|i[delmnoqrst])"
        "|(jobs|j[emop])"
        "|k[eghimnprwyz]"
        "|l[abcikrstuvy]"
        "|(mil|mobi|museum|m[acdeghklmnopqrstuvwxyz])"
        "|(name|net|n[acefgilopruz])"
        "|(org|om)"
        "|(pro|p[aefghklmnrstwy])"
        "|qa"
        "|r[eosuw]"
        "|s[abcdeghijklmnortuvyz]"
        "|(tel|travel|t[cdfghjklmnoprtvwz])"
        "|u[agksyz]"
        "|v[aceginu]"
        "|w[fs]"
        "|(\u03b4\u03bf\u03ba\u03b9\u03bc\u03ae|\u0438\u0441\u043f\u044b\u0442\u0430\u043d\u0438\u0435|\u0440\u0444|\u0441\u0440\u0431|\u05d8\u05e2\u05e1\u05d8|\u0622\u0632\u0645\u0627\u06cc\u0634\u06cc|\u0625\u062e\u062a\u0628\u0627\u0631|\u0627\u0644\u0627\u0631\u062f\u0646|\u0627\u0644\u062c\u0632\u0627\u0626\u0631|\u0627\u0644\u0633\u0639\u0648\u062f\u064a\u0629|\u0627\u0644\u0645\u063a\u0631\u0628|\u0627\u0645\u0627\u0631\u0627\u062a|\u0628\u06be\u0627\u0631\u062a|\u062a\u0648\u0646\u0633|\u0633\u0648\u0631\u064a\u0629|\u0641\u0644\u0633\u0637\u064a\u0646|\u0642\u0637\u0631|\u0645\u0635\u0631|\u092a\u0930\u0940\u0915\u094d\u0937\u093e|\u092d\u093e\u0930\u0924|\u09ad\u09be\u09b0\u09a4|\u0a2d\u0a3e\u0a30\u0a24|\u0aad\u0abe\u0ab0\u0aa4|\u0b87\u0ba8\u0bcd\u0ba4\u0bbf\u0baf\u0bbe|\u0b87\u0bb2\u0b99\u0bcd\u0b95\u0bc8|\u0b9a\u0bbf\u0b99\u0bcd\u0b95\u0baa\u0bcd\u0baa\u0bc2\u0bb0\u0bcd|\u0baa\u0bb0\u0bbf\u0b9f\u0bcd\u0b9a\u0bc8|\u0c2d\u0c3e\u0c30\u0c24\u0c4d|\u0dbd\u0d82\u0d9a\u0dcf|\u0e44\u0e17\u0e22|\u30c6\u30b9\u30c8|\u4e2d\u56fd|\u4e2d\u570b|\u53f0\u6e7e|\u53f0\u7063|\u65b0\u52a0\u5761|\u6d4b\u8bd5|\u6e2c\u8a66|\u9999\u6e2f|\ud14c\uc2a4\ud2b8|\ud55c\uad6d|xn\\-\\-0zwm56d|xn\\-\\-11b5bs3a9aj6g|xn\\-\\-3e0b707e|xn\\-\\-45brj9c|xn\\-\\-80akhbyknj4f|xn\\-\\-90a3ac|xn\\-\\-9t4b11yi5a|xn\\-\\-clchc0ea0b2g2a9gcd|xn\\-\\-deba0ad|xn\\-\\-fiqs8s|xn\\-\\-fiqz9s|xn\\-\\-fpcrj9c3d|xn\\-\\-fzc2c9e2c|xn\\-\\-g6w251d|xn\\-\\-gecrj9c|xn\\-\\-h2brj9c|xn\\-\\-hgbk6aj7f53bba|xn\\-\\-hlcj6aya9esc7a|xn\\-\\-j6w193g|xn\\-\\-jxalpdlp|xn\\-\\-kgbechtv|xn\\-\\-kprw13d|xn\\-\\-kpry57d|xn\\-\\-lgbbat1ad8j|xn\\-\\-mgbaam7a8h|xn\\-\\-mgbayh7gpa|xn\\-\\-mgbbh1a71e|xn\\-\\-mgbc0a9azcg|xn\\-\\-mgberp4a5d4ar|xn\\-\\-o3cw4h|xn\\-\\-ogbpf8fl|xn\\-\\-p1ai|xn\\-\\-pgbs0dh|xn\\-\\-s9brj9c|xn\\-\\-wgbh1c|xn\\-\\-wgbl6a|xn\\-\\-xkc2al3hye2a|xn\\-\\-xkc2dl3a5ee0h|xn\\-\\-yfro4i67o|xn\\-\\-ygbi2ammx|xn\\-\\-zckzah|xxx)"
        "|y[et]"
        "|z[amw])"));

static AutoPtr<IPattern> InitWebUrl()
{
    StringBuilder sb(
        "((?:(http|https|Http|Https|rtsp|Rtsp):\\/\\/(?:(?:[a-zA-Z0-9\\$\\-\\_\\.\\+\\!\\*\\'\\(\\)"
        "\\,\\;\\?\\&\\=]|(?:\\%[a-fA-F0-9]{2})){1,64}(?:\\:(?:[a-zA-Z0-9\\$\\-\\_"
        "\\.\\+\\!\\*\\'\\(\\)\\,\\;\\?\\&\\=]|(?:\\%[a-fA-F0-9]{2})){1,25})?\\@)?)?");

    sb += "(?:";

    //DOMAIN_NAME
    sb += "(";
        //HOST_NAME
        sb += "(";
            //IRI
            sb += "a-zA-Z\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF"; //GOOD_GTLD_CHAR;
            sb += "]([";
            sb += "a-zA-Z\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF"; //GOOD_GTLD_CHAR;
            sb += "\\-]{0,61}[";
            sb += "a-zA-Z\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF"; //GOOD_GTLD_CHAR;
            sb += "]){0,1}";

        sb += "\\.)+";

            //GTLD
            sb += "[";
            sb += "a-zA-Z\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF"; //GOOD_GTLD_CHAR;
            sb += "]{2,63}";

        sb += "|";
        //IP_ADDRESS
        sb += "((25[0-5]|2[0-4][0-9]|[0-1][0-9]{2}|[1-9][0-9]|[1-9])\\.(25[0-5]|2[0-4]"
            "[0-9]|[0-1][0-9]{2}|[1-9][0-9]|[1-9]|0)\\.(25[0-5]|2[0-4][0-9]|[0-1]"
            "[0-9]{2}|[1-9][0-9]|[1-9]|0)\\.(25[0-5]|2[0-4][0-9]|[0-1][0-9]{2}"
            "|[1-9][0-9]|[0-9]))";
    sb += ")";

    sb += ")";

    sb +=  "(?:\\:\\d{1,5})?)" // plus option port number
        "(\\/(?:(?:[";
    sb += "a-zA-Z0-9\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF";   // GOOD_IRI_CHAR
    sb += "\\;\\/\\?\\:\\@\\&\\=\\#\\~"  // plus option query params
        "\\-\\.\\+\\!\\*\\'\\(\\)\\,\\_])|(?:\\%[a-fA-F0-9]{2}))*)?"
        "(?:\\b|$)";
    // and finally, a word boundary or end of input.  This is to stop foo.sure from matching as foo.su

    AutoPtr<IPattern> pattern = InitPattern(sb.ToString());
    return pattern;
}

// and finally, a word boundary or end of
// input.  This is to stop foo.sure from
// matching as foo.su
const AutoPtr<IPattern> Patterns::WEB_URL = InitWebUrl();

const AutoPtr<IPattern> Patterns::IP_ADDRESS
     = InitPattern(String(
        "((25[0-5]|2[0-4][0-9]|[0-1][0-9]{2}|[1-9][0-9]|[1-9])\\.(25[0-5]|2[0-4]"
        "[0-9]|[0-1][0-9]{2}|[1-9][0-9]|[1-9]|0)\\.(25[0-5]|2[0-4][0-9]|[0-1]"
        "[0-9]{2}|[1-9][0-9]|[1-9]|0)\\.(25[0-5]|2[0-4][0-9]|[0-1][0-9]{2}"
        "|[1-9][0-9]|[0-9]))"));

static AutoPtr<IPattern> InitDomainName()
{
    StringBuilder sb("(");
    //HOST_NAME
    sb += "(";
        //IRI
        sb += "a-zA-Z\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF"; //GOOD_GTLD_CHAR;
        sb += "]([";
        sb += "a-zA-Z\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF"; //GOOD_GTLD_CHAR;
        sb += "\\-]{0,61}[";
        sb += "a-zA-Z\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF"; //GOOD_GTLD_CHAR;
        sb += "]){0,1}";

        sb += "\\.)+";

        //GTLD
        sb += "[";
        sb += "a-zA-Z\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF"; //GOOD_GTLD_CHAR;
        sb += "]{2,63}";

    sb += "|";

    //IP_ADDRESS
    sb += "((25[0-5]|2[0-4][0-9]|[0-1][0-9]{2}|[1-9][0-9]|[1-9])\\.(25[0-5]|2[0-4]"
        "[0-9]|[0-1][0-9]{2}|[1-9][0-9]|[1-9]|0)\\.(25[0-5]|2[0-4][0-9]|[0-1]"
        "[0-9]{2}|[1-9][0-9]|[1-9]|0)\\.(25[0-5]|2[0-4][0-9]|[0-1][0-9]{2}"
        "|[1-9][0-9]|[0-9]))";
    sb += ")";

    AutoPtr<IPattern> pattern = InitPattern(sb.ToString());
    return pattern;
}

const AutoPtr<IPattern> Patterns::DOMAIN_NAME = InitDomainName();

const AutoPtr<IPattern> Patterns::EMAIL_ADDRESS
     = InitPattern(String(
        "[a-zA-Z0-9\\+\\.\\_\\%\\-\\+]{1,256}"
        "\\@"
        "[a-zA-Z0-9][a-zA-Z0-9\\-]{0,64}"
        "("
        "\\."
        "[a-zA-Z0-9][a-zA-Z0-9\\-]{0,25}"
        ")+"));

const AutoPtr<IPattern> Patterns::PHONE
     = InitPattern(String(                          // sdd = space, dot, or dash
        "(\\+[0-9]+[\\- \\.]*)?"                    // +<digits><sdd>*
        "(\\([0-9]+\\)[\\- \\.]*)?"                 // (<digits>)<sdd>*
        "([0-9][0-9\\- \\.]+[0-9])"));              // <digit><digit|sdd>+<digit>


String Patterns::ConcatGroups(
    /* [in] */ IMatcher* matcher)
{
    if (matcher == NULL) {
        return String(NULL);
    }

    IMatchResult* mr = IMatchResult::Probe(matcher);
    StringBuilder b;
    Int32 numGroups;
    mr->GroupCount(&numGroups);

    String s;
    for (Int32 i = 1; i <= numGroups; i++) {
        mr->Group(i, &s);
        if (s != NULL) {
            b += s;
        }
    }

    return b.ToString();
}

String Patterns::DigitsAndPlusOnly(
    /* [in] */ IMatcher* matcher)
{
    if (matcher == NULL) {
        return String(NULL);
    }

    IMatchResult* mr = IMatchResult::Probe(matcher);
    StringBuilder buffer;
    String matchingRegion;
    mr->Group(&matchingRegion);

    AutoPtr<ArrayOf<Char32> > charArray = matchingRegion.GetChars();
    Char32 character;
    for (UInt32 i = 0, size = charArray->GetLength(); i < size; i++) {
        character = (*charArray)[i];

        if (character == '+' || Character::IsDigit(character)) {
            buffer += character;
        }
    }
    return buffer.ToString();
}

}
}
}
