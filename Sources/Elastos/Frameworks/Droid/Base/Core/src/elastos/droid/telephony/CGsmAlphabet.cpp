
#include "CGsmAlphabet.h"
#include "elastos/droid/content/res/CResourcesHelper.h"
#include "elastos/droid/R.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::Core::CString;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Utility::Logging::Slogger;
//using Elastos::IO::CharSet::CharSet;
//using Elastos::IO::CharSet::ICharSet;
//using Elastos::IO::IByteBuffer;
//using Elastos::IO::ICharBuffer;
//using Elastos::IO::ByteBuffer;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

#define LANGUAGEtABLE_LEN 14

const String CGsmAlphabet::TAG("CGsmAlphabet");
Boolean CGsmAlphabet::sDisableCountryEncodingCheck = FALSE;

AutoPtr < ArrayOf< AutoPtr<SparseInt32Array> > > CGsmAlphabet::sCharsToGsmTables;
AutoPtr < ArrayOf< AutoPtr<SparseInt32Array> > > CGsmAlphabet::sCharsToShiftTables;
AutoPtr < ArrayOf<Int32> > CGsmAlphabet::sEnabledSingleShiftTables;
AutoPtr < ArrayOf<Int32> > CGsmAlphabet::sEnabledLockingShiftTables;
Int32 CGsmAlphabet::sHighestEnabledSingleShiftCode;

static AutoPtr< ArrayOf<String> > InitsLanguageTables()
{
    AutoPtr< ArrayOf<String> > languageTables = ArrayOf<String>::Alloc(LANGUAGEtABLE_LEN);
    (*languageTables)[0] = /* 6.2.1.1 GSM 7 bit Default Alphabet Extension Table
         0123456789A.....BCDEF0123456789ABCDEF0123456789ABCDEF.0123456789ABCDEF0123456789ABCDEF */
        String("          \u000c         ^                   {}     \\            [~] |               ")
            // 0123456789ABCDEF012345.....6789ABCDEF0123456789ABCDEF
            + String("                     \u20ac                          ");

    /* A.2.1 Turkish National Language Single Shift Table
     0123456789A.....BCDEF0123456789ABCDEF0123456789ABCDEF.0123456789ABCDEF01234567.....8 */
    (*languageTables)[1] = String("          \u000c         ^                   {}     \\            [~] |      \u011e ")
        // 9.....ABCDEF0123.....456789ABCDEF0123.....45.....67.....89.....ABCDEF0123.....
        + String("\u0130         \u015e               \u00e7 \u20ac \u011f \u0131         \u015f")
        // 456789ABCDEF
        + String("            ");

    /* A.2.2 Spanish National Language Single Shift Table
     0123456789.....A.....BCDEF0123456789ABCDEF0123456789ABCDEF.0123456789ABCDEF01.....23 */
    (*languageTables)[2] = String("         \u00e7\u000c         ^                   {}     \\            [~] |\u00c1  ")
        // 456789.....ABCDEF.....012345.....6789ABCDEF01.....2345.....6789.....ABCDEF.....012
        + String("     \u00cd     \u00d3     \u00da           \u00e1   \u20ac   \u00ed     \u00f3   ")
        // 345.....6789ABCDEF
        + String("  \u00fa          ");

    /* A.2.3 Portuguese National Language Single Shift Table
     012345.....6789.....A.....B.....C.....DE.....F.....012.....3.....45.....6.....7.....8....*/
    (*languageTables)[3] = String("     \u00ea   \u00e7\u000c\u00d4\u00f4 \u00c1\u00e1  \u03a6\u0393^\u03a9\u03a0\u03a8\u03a3")
        // 9.....ABCDEF.....0123456789ABCDEF.0123456789ABCDEF01.....23456789.....ABCDE
        + String("\u0398     \u00ca        {}     \\            [~] |\u00c0       \u00cd     ")
        // F.....012345.....6789AB.....C.....DEF01.....2345.....6789.....ABCDEF.....01234
        + String("\u00d3     \u00da     \u00c3\u00d5    \u00c2   \u20ac   \u00ed     \u00f3     ")
        // 5.....6789AB.....C.....DEF.....
        + String("\u00fa     \u00e3\u00f5  \u00e2");

    /* A.2.4 Bengali National Language Single Shift Table
     01.....23.....4.....5.6.....789A.....BCDEF0123.....45.....6789.....A.....BC.....D..... */
    (*languageTables)[4] = String("@\u00a3$\u00a5\u00bf\"\u00a4%&'\u000c*+ -/<=>\u00a1^\u00a1_#*\u09e6\u09e7 \u09e8\u09e9")
        // E.....F.....0.....1.....2.....3.....4.....5.....6.....7.....89A.....B.....C.....
        + String("\u09ea\u09eb\u09ec\u09ed\u09ee\u09ef\u09df\u09e0\u09e1\u09e2{}\u09e3\u09f2\u09f3")
        // D.....E.....F.0.....1.....2.....3.....4.....56789ABCDEF0123456789ABCDEF
        + String("\u09f4\u09f5\\\u09f6\u09f7\u09f8\u09f9\u09fa       [~] |ABCDEFGHIJKLMNO")
        // 0123456789ABCDEF012345.....6789ABCDEF0123456789ABCDEF
        + String("PQRSTUVWXYZ          \u20ac                          ");

    /* A.2.5 Gujarati National Language Single Shift Table
     01.....23.....4.....5.6.....789A.....BCDEF0123.....45.....6789.....A.....BC.....D..... */
    (*languageTables)[5] = String("@\u00a3$\u00a5\u00bf\"\u00a4%&'\u000c*+ -/<=>\u00a1^\u00a1_#*\u0964\u0965 \u0ae6\u0ae7")
        // E.....F.....0.....1.....2.....3.....4.....5.....6789ABCDEF.0123456789ABCDEF
        + String("\u0ae8\u0ae9\u0aea\u0aeb\u0aec\u0aed\u0aee\u0aef  {}     \\            [~] ")
        // 0123456789ABCDEF0123456789ABCDEF012345.....6789ABCDEF0123456789ABCDEF
        + String("|ABCDEFGHIJKLMNOPQRSTUVWXYZ          \u20ac                          ");

    /* A.2.6 Hindi National Language Single Shift Table
     01.....23.....4.....5.6.....789A.....BCDEF0123.....45.....6789.....A.....BC.....D..... */
    (*languageTables)[6] = String("@\u00a3$\u00a5\u00bf\"\u00a4%&'\u000c*+ -/<=>\u00a1^\u00a1_#*\u0964\u0965 \u0966\u0967")
        // E.....F.....0.....1.....2.....3.....4.....5.....6.....7.....89A.....B.....C.....
        + String("\u0968\u0969\u096a\u096b\u096c\u096d\u096e\u096f\u0951\u0952{}\u0953\u0954\u0958")
        // D.....E.....F.0.....1.....2.....3.....4.....5.....6.....7.....8.....9.....A.....
        + String("\u0959\u095a\\\u095b\u095c\u095d\u095e\u095f\u0960\u0961\u0962\u0963\u0970\u0971")
        // BCDEF0123456789ABCDEF0123456789ABCDEF012345.....6789ABCDEF0123456789ABCDEF
        + String(" [~] |ABCDEFGHIJKLMNOPQRSTUVWXYZ          \u20ac                          ");

    /* A.2.7 Kannada National Language Single Shift Table
     01.....23.....4.....5.6.....789A.....BCDEF0123.....45.....6789.....A.....BC.....D..... */
    (*languageTables)[7] = String("@\u00a3$\u00a5\u00bf\"\u00a4%&'\u000c*+ -/<=>\u00a1^\u00a1_#*\u0964\u0965 \u0ce6\u0ce7")
        // E.....F.....0.....1.....2.....3.....4.....5.....6.....7.....89A.....BCDEF.01234567
        + String("\u0ce8\u0ce9\u0cea\u0ceb\u0cec\u0ced\u0cee\u0cef\u0cde\u0cf1{}\u0cf2    \\        ")
        // 89ABCDEF0123456789ABCDEF0123456789ABCDEF012345.....6789ABCDEF0123456789ABCDEF
        + String("    [~] |ABCDEFGHIJKLMNOPQRSTUVWXYZ          \u20ac                          ");

    /* A.2.8 Malayalam National Language Single Shift Table
     01.....23.....4.....5.6.....789A.....BCDEF0123.....45.....6789.....A.....BC.....D..... */
    (*languageTables)[8] = String("@\u00a3$\u00a5\u00bf\"\u00a4%&'\u000c*+ -/<=>\u00a1^\u00a1_#*\u0964\u0965 \u0d66\u0d67")
        // E.....F.....0.....1.....2.....3.....4.....5.....6.....7.....89A.....B.....C.....
        + String("\u0d68\u0d69\u0d6a\u0d6b\u0d6c\u0d6d\u0d6e\u0d6f\u0d70\u0d71{}\u0d72\u0d73\u0d74")
        // D.....E.....F.0.....1.....2.....3.....4.....56789ABCDEF0123456789ABCDEF0123456789A
        + String("\u0d75\u0d7a\\\u0d7b\u0d7c\u0d7d\u0d7e\u0d7f       [~] |ABCDEFGHIJKLMNOPQRSTUVWXYZ")
        // BCDEF012345.....6789ABCDEF0123456789ABCDEF
        + String("          \u20ac                          ");

    /* A.2.9 Oriya National Language Single Shift Table
     01.....23.....4.....5.6.....789A.....BCDEF0123.....45.....6789.....A.....BC.....D..... */
    (*languageTables)[9] = String("@\u00a3$\u00a5\u00bf\"\u00a4%&'\u000c*+ -/<=>\u00a1^\u00a1_#*\u0964\u0965 \u0b66\u0b67")
        // E.....F.....0.....1.....2.....3.....4.....5.....6.....7.....89A.....B.....C.....DE
        + String("\u0b68\u0b69\u0b6a\u0b6b\u0b6c\u0b6d\u0b6e\u0b6f\u0b5c\u0b5d{}\u0b5f\u0b70\u0b71  ")
        // F.0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF012345.....6789ABCDEF0123456789A
        + String("\\            [~] |ABCDEFGHIJKLMNOPQRSTUVWXYZ          \u20ac                     ")
        // BCDEF
        + String("     ");

    /* A.2.10 Punjabi National Language Single Shift Table
     01.....23.....4.....5.6.....789A.....BCDEF0123.....45.....6789.....A.....BC.....D..... */
    (*languageTables)[10] = String("@\u00a3$\u00a5\u00bf\"\u00a4%&'\u000c*+ -/<=>\u00a1^\u00a1_#*\u0964\u0965 \u0a66\u0a67")
        // E.....F.....0.....1.....2.....3.....4.....5.....6.....7.....89A.....B.....C.....
        + String("\u0a68\u0a69\u0a6a\u0a6b\u0a6c\u0a6d\u0a6e\u0a6f\u0a59\u0a5a{}\u0a5b\u0a5c\u0a5e")
        // D.....EF.0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF012345.....6789ABCDEF01
        + String("\u0a75 \\            [~] |ABCDEFGHIJKLMNOPQRSTUVWXYZ          \u20ac            ")
        // 23456789ABCDEF
        + String("              ");

    /* A.2.11 Tamil National Language Single Shift Table
       NOTE: TS 23.038 V9.1.1 shows code 0x24 as \u0bef, corrected to \u0bee (typo)
     01.....23.....4.....5.6.....789A.....BCDEF0123.....45.....6789.....A.....BC.....D..... */
    (*languageTables)[11] = String("@\u00a3$\u00a5\u00bf\"\u00a4%&'\u000c*+ -/<=>\u00a1^\u00a1_#*\u0964\u0965 \u0be6\u0be7")
        // E.....F.....0.....1.....2.....3.....4.....5.....6.....7.....89A.....B.....C.....
        + String("\u0be8\u0be9\u0bea\u0beb\u0bec\u0bed\u0bee\u0bef\u0bf3\u0bf4{}\u0bf5\u0bf6\u0bf7")
        // D.....E.....F.0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF012345.....6789ABC
        + String("\u0bf8\u0bfa\\            [~] |ABCDEFGHIJKLMNOPQRSTUVWXYZ          \u20ac       ")
        // DEF0123456789ABCDEF
        + String("                   ");

    /* A.2.12 Telugu National Language Single Shift Table
       NOTE: TS 23.038 V9.1.1 shows code 0x22-0x23 as \u06cc\u06cd, corrected to \u0c6c\u0c6d
     01.....23.....4.....5.6.....789A.....BCDEF0123.....45.....6789ABC.....D.....E.....F..... */
    (*languageTables)[12] = String("@\u00a3$\u00a5\u00bf\"\u00a4%&'\u000c*+ -/<=>\u00a1^\u00a1_#*   \u0c66\u0c67\u0c68\u0c69")
        // 0.....1.....2.....3.....4.....5.....6.....7.....89A.....B.....C.....D.....E.....F.
        + String("\u0c6a\u0c6b\u0c6c\u0c6d\u0c6e\u0c6f\u0c58\u0c59{}\u0c78\u0c79\u0c7a\u0c7b\u0c7c\\")
        // 0.....1.....2.....3456789ABCDEF0123456789ABCDEF0123456789ABCDEF012345.....6789ABCD
        + String("\u0c7d\u0c7e\u0c7f         [~] |ABCDEFGHIJKLMNOPQRSTUVWXYZ          \u20ac        ")
        // EF0123456789ABCDEF
        + String("                  ");

    /* A.2.13 Urdu National Language Single Shift Table
     01.....23.....4.....5.6.....789A.....BCDEF0123.....45.....6789.....A.....BC.....D..... */
    (*languageTables)[13] = String("@\u00a3$\u00a5\u00bf\"\u00a4%&'\u000c*+ -/<=>\u00a1^\u00a1_#*\u0600\u0601 \u06f0\u06f1")
        // E.....F.....0.....1.....2.....3.....4.....5.....6.....7.....89A.....B.....C.....
        + String("\u06f2\u06f3\u06f4\u06f5\u06f6\u06f7\u06f8\u06f9\u060c\u060d{}\u060e\u060f\u0610")
        // D.....E.....F.0.....1.....2.....3.....4.....5.....6.....7.....8.....9.....A.....
        + String("\u0611\u0612\\\u0613\u0614\u061b\u061f\u0640\u0652\u0658\u066b\u066c\u0672\u0673")
        // B.....CDEF.....0123456789ABCDEF0123456789ABCDEF012345.....6789ABCDEF0123456789ABCDEF
        + String("\u06cd[~]\u06d4|ABCDEFGHIJKLMNOPQRSTUVWXYZ          \u20ac                          ");


    return languageTables;

}

AutoPtr< ArrayOf<String> > CGsmAlphabet::sLanguageTables = InitsLanguageTables();

static AutoPtr< ArrayOf<String> > InitsLanguageShiftTables()
{
    AutoPtr< ArrayOf<String> > languageShiftTables = ArrayOf<String>::Alloc(LANGUAGEtABLE_LEN);

    /* 6.2.1.1 GSM 7 bit Default Alphabet Extension Table
     0123456789A.....BCDEF0123456789ABCDEF0123456789ABCDEF.0123456789ABCDEF0123456789ABCDEF */
    (*languageShiftTables)[0] = String("          \u000c         ^                   {}     \\            [~] |               ")
        // 0123456789ABCDEF012345.....6789ABCDEF0123456789ABCDEF
        + String("                     \u20ac                          ");

    /* A.2.1 Turkish National Language Single Shift Table
     0123456789A.....BCDEF0123456789ABCDEF0123456789ABCDEF.0123456789ABCDEF01234567.....8 */
    (*languageShiftTables)[1] = String("          \u000c         ^                   {}     \\            [~] |      \u011e ")
        // 9.....ABCDEF0123.....456789ABCDEF0123.....45.....67.....89.....ABCDEF0123.....
        + String("\u0130         \u015e               \u00e7 \u20ac \u011f \u0131         \u015f")
        // 456789ABCDEF
        + String("            ");

    /* A.2.2 Spanish National Language Single Shift Table
     0123456789.....A.....BCDEF0123456789ABCDEF0123456789ABCDEF.0123456789ABCDEF01.....23 */
    (*languageShiftTables)[2] = String("         \u00e7\u000c         ^                   {}     \\            [~] |\u00c1  ")
        // 456789.....ABCDEF.....012345.....6789ABCDEF01.....2345.....6789.....ABCDEF.....012
        + String("     \u00cd     \u00d3     \u00da           \u00e1   \u20ac   \u00ed     \u00f3   ")
        // 345.....6789ABCDEF
        + String("  \u00fa          ");

    /* A.2.3 Portuguese National Language Single Shift Table
     012345.....6789.....A.....B.....C.....DE.....F.....012.....3.....45.....6.....7.....8....*/
    (*languageShiftTables)[3] = String("     \u00ea   \u00e7\u000c\u00d4\u00f4 \u00c1\u00e1  \u03a6\u0393^\u03a9\u03a0\u03a8\u03a3")
        // 9.....ABCDEF.....0123456789ABCDEF.0123456789ABCDEF01.....23456789.....ABCDE
        + String("\u0398     \u00ca        {}     \\            [~] |\u00c0       \u00cd     ")
        // F.....012345.....6789AB.....C.....DEF01.....2345.....6789.....ABCDEF.....01234
        + String("\u00d3     \u00da     \u00c3\u00d5    \u00c2   \u20ac   \u00ed     \u00f3     ")
        // 5.....6789AB.....C.....DEF.....
        + String("\u00fa     \u00e3\u00f5  \u00e2");

    /* A.2.4 Bengali National Language Single Shift Table
     01.....23.....4.....5.6.....789A.....BCDEF0123.....45.....6789.....A.....BC.....D..... */
    (*languageShiftTables)[4] = String("@\u00a3$\u00a5\u00bf\"\u00a4%&'\u000c*+ -/<=>\u00a1^\u00a1_#*\u09e6\u09e7 \u09e8\u09e9")
        // E.....F.....0.....1.....2.....3.....4.....5.....6.....7.....89A.....B.....C.....
        + String("\u09ea\u09eb\u09ec\u09ed\u09ee\u09ef\u09df\u09e0\u09e1\u09e2{}\u09e3\u09f2\u09f3")
        // D.....E.....F.0.....1.....2.....3.....4.....56789ABCDEF0123456789ABCDEF
        + String("\u09f4\u09f5\\\u09f6\u09f7\u09f8\u09f9\u09fa       [~] |ABCDEFGHIJKLMNO")
        // 0123456789ABCDEF012345.....6789ABCDEF0123456789ABCDEF
        + String("PQRSTUVWXYZ          \u20ac                          ");

    /* A.2.5 Gujarati National Language Single Shift Table
     01.....23.....4.....5.6.....789A.....BCDEF0123.....45.....6789.....A.....BC.....D..... */
    (*languageShiftTables)[5] = String("@\u00a3$\u00a5\u00bf\"\u00a4%&'\u000c*+ -/<=>\u00a1^\u00a1_#*\u0964\u0965 \u0ae6\u0ae7")
        // E.....F.....0.....1.....2.....3.....4.....5.....6789ABCDEF.0123456789ABCDEF
        + String("\u0ae8\u0ae9\u0aea\u0aeb\u0aec\u0aed\u0aee\u0aef  {}     \\            [~] ")
        // 0123456789ABCDEF0123456789ABCDEF012345.....6789ABCDEF0123456789ABCDEF
        + String("|ABCDEFGHIJKLMNOPQRSTUVWXYZ          \u20ac                          ");

    /* A.2.6 Hindi National Language Single Shift Table
     01.....23.....4.....5.6.....789A.....BCDEF0123.....45.....6789.....A.....BC.....D..... */
    (*languageShiftTables)[6] = String("@\u00a3$\u00a5\u00bf\"\u00a4%&'\u000c*+ -/<=>\u00a1^\u00a1_#*\u0964\u0965 \u0966\u0967")
        // E.....F.....0.....1.....2.....3.....4.....5.....6.....7.....89A.....B.....C.....
        + String("\u0968\u0969\u096a\u096b\u096c\u096d\u096e\u096f\u0951\u0952{}\u0953\u0954\u0958")
        // D.....E.....F.0.....1.....2.....3.....4.....5.....6.....7.....8.....9.....A.....
        + String("\u0959\u095a\\\u095b\u095c\u095d\u095e\u095f\u0960\u0961\u0962\u0963\u0970\u0971")
        // BCDEF0123456789ABCDEF0123456789ABCDEF012345.....6789ABCDEF0123456789ABCDEF
        + String(" [~] |ABCDEFGHIJKLMNOPQRSTUVWXYZ          \u20ac                          ");

    /* A.2.7 Kannada National Language Single Shift Table
     01.....23.....4.....5.6.....789A.....BCDEF0123.....45.....6789.....A.....BC.....D..... */
    (*languageShiftTables)[7] = String("@\u00a3$\u00a5\u00bf\"\u00a4%&'\u000c*+ -/<=>\u00a1^\u00a1_#*\u0964\u0965 \u0ce6\u0ce7")
        // E.....F.....0.....1.....2.....3.....4.....5.....6.....7.....89A.....BCDEF.01234567
        + String("\u0ce8\u0ce9\u0cea\u0ceb\u0cec\u0ced\u0cee\u0cef\u0cde\u0cf1{}\u0cf2    \\        ")
        // 89ABCDEF0123456789ABCDEF0123456789ABCDEF012345.....6789ABCDEF0123456789ABCDEF
        + String("    [~] |ABCDEFGHIJKLMNOPQRSTUVWXYZ          \u20ac                          ");

    /* A.2.8 Malayalam National Language Single Shift Table
     01.....23.....4.....5.6.....789A.....BCDEF0123.....45.....6789.....A.....BC.....D..... */
    (*languageShiftTables)[8] = String("@\u00a3$\u00a5\u00bf\"\u00a4%&'\u000c*+ -/<=>\u00a1^\u00a1_#*\u0964\u0965 \u0d66\u0d67")
        // E.....F.....0.....1.....2.....3.....4.....5.....6.....7.....89A.....B.....C.....
        + String("\u0d68\u0d69\u0d6a\u0d6b\u0d6c\u0d6d\u0d6e\u0d6f\u0d70\u0d71{}\u0d72\u0d73\u0d74")
        // D.....E.....F.0.....1.....2.....3.....4.....56789ABCDEF0123456789ABCDEF0123456789A
        + String("\u0d75\u0d7a\\\u0d7b\u0d7c\u0d7d\u0d7e\u0d7f       [~] |ABCDEFGHIJKLMNOPQRSTUVWXYZ")
        // BCDEF012345.....6789ABCDEF0123456789ABCDEF
        + String("          \u20ac                          ");

    /* A.2.9 Oriya National Language Single Shift Table
     01.....23.....4.....5.6.....789A.....BCDEF0123.....45.....6789.....A.....BC.....D..... */
    (*languageShiftTables)[9] = String("@\u00a3$\u00a5\u00bf\"\u00a4%&'\u000c*+ -/<=>\u00a1^\u00a1_#*\u0964\u0965 \u0b66\u0b67")
        // E.....F.....0.....1.....2.....3.....4.....5.....6.....7.....89A.....B.....C.....DE
        + String("\u0b68\u0b69\u0b6a\u0b6b\u0b6c\u0b6d\u0b6e\u0b6f\u0b5c\u0b5d{}\u0b5f\u0b70\u0b71  ")
        // F.0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF012345.....6789ABCDEF0123456789A
        + String("\\            [~] |ABCDEFGHIJKLMNOPQRSTUVWXYZ          \u20ac                     ")
        // BCDEF
        + String("     ");

    /* A.2.10 Punjabi National Language Single Shift Table
     01.....23.....4.....5.6.....789A.....BCDEF0123.....45.....6789.....A.....BC.....D..... */
    (*languageShiftTables)[10] = String("@\u00a3$\u00a5\u00bf\"\u00a4%&'\u000c*+ -/<=>\u00a1^\u00a1_#*\u0964\u0965 \u0a66\u0a67")
        // E.....F.....0.....1.....2.....3.....4.....5.....6.....7.....89A.....B.....C.....
        + String("\u0a68\u0a69\u0a6a\u0a6b\u0a6c\u0a6d\u0a6e\u0a6f\u0a59\u0a5a{}\u0a5b\u0a5c\u0a5e")
        // D.....EF.0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF012345.....6789ABCDEF01
        + String("\u0a75 \\            [~] |ABCDEFGHIJKLMNOPQRSTUVWXYZ          \u20ac            ")
        // 23456789ABCDEF
        + String("              ");

    /* A.2.11 Tamil National Language Single Shift Table
       NOTE: TS 23.038 V9.1.1 shows code 0x24 as \u0bef, corrected to \u0bee (typo)
     01.....23.....4.....5.6.....789A.....BCDEF0123.....45.....6789.....A.....BC.....D..... */
    (*languageShiftTables)[11] = String("@\u00a3$\u00a5\u00bf\"\u00a4%&'\u000c*+ -/<=>\u00a1^\u00a1_#*\u0964\u0965 \u0be6\u0be7")
        // E.....F.....0.....1.....2.....3.....4.....5.....6.....7.....89A.....B.....C.....
        + String("\u0be8\u0be9\u0bea\u0beb\u0bec\u0bed\u0bee\u0bef\u0bf3\u0bf4{}\u0bf5\u0bf6\u0bf7")
        // D.....E.....F.0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF012345.....6789ABC
        + String("\u0bf8\u0bfa\\            [~] |ABCDEFGHIJKLMNOPQRSTUVWXYZ          \u20ac       ")
        // DEF0123456789ABCDEF
        + String("                   ");

    /* A.2.12 Telugu National Language Single Shift Table
       NOTE: TS 23.038 V9.1.1 shows code 0x22-0x23 as \u06cc\u06cd, corrected to \u0c6c\u0c6d
     01.....23.....4.....5.6.....789A.....BCDEF0123.....45.....6789ABC.....D.....E.....F..... */
    (*languageShiftTables)[12] = String("@\u00a3$\u00a5\u00bf\"\u00a4%&'\u000c*+ -/<=>\u00a1^\u00a1_#*   \u0c66\u0c67\u0c68\u0c69")
        // 0.....1.....2.....3.....4.....5.....6.....7.....89A.....B.....C.....D.....E.....F.
        + String("\u0c6a\u0c6b\u0c6c\u0c6d\u0c6e\u0c6f\u0c58\u0c59{}\u0c78\u0c79\u0c7a\u0c7b\u0c7c\\")
        // 0.....1.....2.....3456789ABCDEF0123456789ABCDEF0123456789ABCDEF012345.....6789ABCD
        + String("\u0c7d\u0c7e\u0c7f         [~] |ABCDEFGHIJKLMNOPQRSTUVWXYZ          \u20ac        ")
        // EF0123456789ABCDEF
        + String("                  ");

    /* A.2.13 Urdu National Language Single Shift Table
     01.....23.....4.....5.6.....789A.....BCDEF0123.....45.....6789.....A.....BC.....D..... */
    (*languageShiftTables)[13] = String("@\u00a3$\u00a5\u00bf\"\u00a4%&'\u000c*+ -/<=>\u00a1^\u00a1_#*\u0600\u0601 \u06f0\u06f1")
        // E.....F.....0.....1.....2.....3.....4.....5.....6.....7.....89A.....B.....C.....
        + String("\u06f2\u06f3\u06f4\u06f5\u06f6\u06f7\u06f8\u06f9\u060c\u060d{}\u060e\u060f\u0610")
        // D.....E.....F.0.....1.....2.....3.....4.....5.....6.....7.....8.....9.....A.....
        + String("\u0611\u0612\\\u0613\u0614\u061b\u061f\u0640\u0652\u0658\u066b\u066c\u0672\u0673")
        // B.....CDEF.....0123456789ABCDEF0123456789ABCDEF012345.....6789ABCDEF0123456789ABCDEF
        + String("\u06cd[~]\u06d4|ABCDEFGHIJKLMNOPQRSTUVWXYZ          \u20ac                          ");

    return languageShiftTables;
}

AutoPtr< ArrayOf<String> > CGsmAlphabet::sLanguageShiftTables = InitsLanguageShiftTables();

Boolean CGsmAlphabet::sIsInitedStaticVarOnce = FALSE;

CAR_INTERFACE_IMPL(CGsmAlphabet::TextEncodingDetails, IGsmAlphabetTextEncodingDetails);

ECode CGsmAlphabet::TextEncodingDetails::SetMsgCount(
    /* [in] */ Int32 msgCount)
{
    mMsgCount = msgCount;
    return NOERROR;
}

ECode CGsmAlphabet::TextEncodingDetails::GetMsgCount(
    /* [out] */ Int32* msgCount)
{
    VALIDATE_NOT_NULL(msgCount);
    *msgCount = mMsgCount;
    return NOERROR;
}

ECode CGsmAlphabet::TextEncodingDetails::SetCodeUnitCount(
    /* [in] */ Int32 codeUnitCount)
{
    mCodeUnitCount = codeUnitCount;
    return NOERROR;
}

ECode CGsmAlphabet::TextEncodingDetails::GetCodeUnitCount(
    /* [out] */ Int32* codeUnitCount)
{
    VALIDATE_NOT_NULL(codeUnitCount);
    *codeUnitCount = mCodeUnitCount;
    return NOERROR;
}

ECode CGsmAlphabet::TextEncodingDetails::SetCodeUnitsRemaining(
    /* [in] */ Int32 codeUnitsRemaining)
{
    mCodeUnitsRemaining = codeUnitsRemaining;
    return NOERROR;
}

ECode CGsmAlphabet::TextEncodingDetails::GetCodeUnitsRemaining(
    /* [out] */ Int32* codeUnitsRemaining)
{
    VALIDATE_NOT_NULL(codeUnitsRemaining);
    *codeUnitsRemaining = mCodeUnitsRemaining;
    return NOERROR;
}

ECode CGsmAlphabet::TextEncodingDetails::SetCodeUnitSize(
    /* [in] */ Int32 codeUnitSize)
{
    mCodeUnitSize = codeUnitSize;
    return NOERROR;
}

ECode CGsmAlphabet::TextEncodingDetails::GetCodeUnitSize(
    /* [out] */ Int32* codeUnitSize)
{
    VALIDATE_NOT_NULL(codeUnitSize);
    *codeUnitSize = mCodeUnitSize;
    return NOERROR;
}


ECode CGsmAlphabet::TextEncodingDetails::SetLanguageTable(
    /* [in] */ Int32 languageTable)
{
    mLanguageTable = languageTable;
    return NOERROR;
}

ECode CGsmAlphabet::TextEncodingDetails::GetLanguageTable(
    /* [out] */ Int32* languageTable)
{
    VALIDATE_NOT_NULL(languageTable);
    *languageTable = mLanguageTable;
    return NOERROR;
}

ECode CGsmAlphabet::TextEncodingDetails::SetLanguageShiftTable(
    /* [in] */ Int32 languageShiftTable)
{
    mLanguageShiftTable = languageShiftTable;
    return NOERROR;
}

ECode CGsmAlphabet::TextEncodingDetails::GetLanguageShiftTable(
    /* [out] */ Int32* languageShiftTable)
{
    VALIDATE_NOT_NULL(languageShiftTable);
    *languageShiftTable = mLanguageShiftTable;
    return NOERROR;
}

ECode CGsmAlphabet::TextEncodingDetails::ToString(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    StringBuilder sb;

    sb.Append("TextEncodingDetails");
    sb.Append("{ msgCount="); sb.Append(mMsgCount);
    sb.Append(", codeUnitCount="); sb.Append(mCodeUnitCount);
    sb.Append(", codeUnitsRemaining="); sb.Append(mCodeUnitsRemaining);
    sb.Append(", codeUnitSize="); sb.Append(mCodeUnitSize);
    sb.Append(", languageTable="); sb.Append(mLanguageTable);
    sb.Append(", languageShiftTable="); sb.Append(mLanguageShiftTable);
    sb.Append(" }");
    *res = sb.ToString();
    return NOERROR;
}

Boolean CGsmAlphabet::InitStaticVar()
{
    if(!sIsInitedStaticVarOnce) {
        EnableCountrySpecificEncodings();
        Int32 numTables = LANGUAGEtABLE_LEN;
        Int32 numShiftTables = LANGUAGEtABLE_LEN;
        if (numTables != numShiftTables) {
            Slogger::E(TAG, String("Error: language tables array length ") + StringUtils::Int32ToString(numTables) +
                    String(" != shift tables array length ") + StringUtils::Int32ToString(numShiftTables));
        }

        sCharsToGsmTables = ArrayOf< AutoPtr<SparseInt32Array> >::Alloc(numTables)/*new SparseInt32Array[numTables]*/;
        for (Int32 i = 0; i < numTables; i++) {
            String table = (*sLanguageTables)[i];

            Int32 tableLen = table.GetLength();
            if (tableLen != 0 && tableLen != 128) {
                Slogger::E(TAG, String("Error: language tables index ") + StringUtils::Int32ToString(i) +
                        String(" length ") + StringUtils::Int32ToString(tableLen) + String(" (expected 128 or 0)"));
            }

            AutoPtr<SparseInt32Array> charToGsmTable = new SparseInt32Array(tableLen);
        sCharsToGsmTables->Set(i, charToGsmTable);
            for (Int32 j = 0; j < tableLen; j++) {
                Char32 c = table.GetChar(j);
                charToGsmTable->Put(c, j);
            }
        }

        sCharsToShiftTables = ArrayOf< AutoPtr<SparseInt32Array> >::Alloc(numTables)/*new SparseInt32Array[numTables]*/;
        for (Int32 i = 0; i < numShiftTables; i++) {
            String shiftTable = (*sLanguageShiftTables)[i];

            Int32 shiftTableLen = shiftTable.GetLength();
            if (shiftTableLen != 0 && shiftTableLen != 128) {
                Slogger::E(TAG, String("Error: language shift tables index ") + StringUtils::Int32ToString(i) +
                        String(" length ") + StringUtils::Int32ToString(shiftTableLen) + String(" (expected 128 or 0)"));
            }

            AutoPtr<SparseInt32Array> charToShiftTable = new SparseInt32Array(shiftTableLen);
            sCharsToShiftTables->Set(i, charToShiftTable);
            for (Int32 j = 0; j < shiftTableLen; j++) {
                Char32 c = shiftTable.GetChar(j);
                if (c != ' ') {
                    charToShiftTable->Put(c, j);
                }
            }
        }
    }
    return TRUE;
}

ECode CGsmAlphabet::CharToGsm(
    /* [in] */ Char32 c,
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val);

    // try {
    //     return charToGsm(c, false);
    // } catch (EncodeException ex) {
    //     // this should never happen
    //     return sCharsToGsmTables[0].get(' ', ' ');
    // }
    if (FAILED(CharToGsm(c, FALSE, val))) {
        *val = (*sCharsToGsmTables)[0]->Get(' ', ' ');
    }

    return NOERROR;
}

ECode CGsmAlphabet::CharToGsm(
    /* [in] */ Char32 c,
    /* [in] */ Boolean throwException,
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val);
    Int32 ret;

    ret = (*sCharsToGsmTables)[0]->Get(c, -1);

    if (ret == -1) {
        ret = (*sCharsToShiftTables)[0]->Get(c, -1);

        if (ret == -1) {
            if (throwException) {
                //throw new EncodeException(c);
                return E_FAIL;
            } else {
                *val = (*sCharsToGsmTables)[0]->Get(' ', ' ');
                return NOERROR;
            }
        } else {
            *val = GSM_EXTENDED_ESCAPE;
            return NOERROR;
        }
    }

    *val = ret;
    return NOERROR;
}

ECode CGsmAlphabet::CharToGsmExtended(
    /* [in] */ Char32 c,
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val);
    Int32 ret;

    ret = (*sCharsToShiftTables)[0]->Get(c, -1);

    if (ret == -1) {
        *val = (*sCharsToGsmTables)[0]->Get(' ', ' ');
        return NOERROR;
    }

    *val = ret;
    return NOERROR;
}

ECode CGsmAlphabet::GsmToChar(
    /* [in] */ Int32 gsmChar,
    /* [out] */ Char32* res)
{
    VALIDATE_NOT_NULL(res);
    if (gsmChar >= 0 && gsmChar < 128) {
        *res = (*sLanguageTables)[0].GetChar(gsmChar);
        return NOERROR;
    } else {
        *res = ' ';
        return NOERROR;
    }
}

ECode CGsmAlphabet::GsmExtendedToChar(
    /* [in] */ Int32 gsmChar,
    /* [out] */ Char32* res)
{
    VALIDATE_NOT_NULL(res);
    if (gsmChar == GSM_EXTENDED_ESCAPE) {
        *res = ' ';
        return NOERROR;
    } else if (gsmChar >= 0 && gsmChar < 128) {
        Char32 c = (*sLanguageShiftTables)[0].GetChar(gsmChar);
        if (c == ' ') {
            *res = (*sLanguageTables)[0].GetChar(gsmChar);
            return NOERROR;
        } else {
            *res = c;
            return NOERROR;
        }
    } else {
        *res = ' ';     // out of range
        return NOERROR;
    }
}

ECode CGsmAlphabet::StringToGsm7BitPackedWithHeader(
    /* [in] */ const String& data,
    /* [in] */ ArrayOf<Byte>* header,
    /* [out, callee] */ ArrayOf<Byte>** res)
{
    VALIDATE_NOT_NULL(res);
    return StringToGsm7BitPackedWithHeader(data, header, 0, 0, res);
}

ECode CGsmAlphabet::StringToGsm7BitPackedWithHeader(
    /* [in] */ const String& data,
    /* [in] */ ArrayOf<Byte>* header,
    /* [in] */ Int32 languageTable,
    /* [in] */ Int32 languageShiftTable,
    /* [out, callee] */ ArrayOf<Byte>** res)
{
    VALIDATE_NOT_NULL(res);
    if (header == NULL || header->GetLength() == 0) {
        return StringToGsm7BitPacked(data, languageTable, languageShiftTable, res);
    }

    Int32 headerBits = (header->GetLength() + 1) * 8;
    Int32 headerSeptets = (headerBits + 6) / 7;

    AutoPtr< ArrayOf<Byte> > ret;
    StringToGsm7BitPacked(data, headerSeptets, TRUE, languageTable,
            languageShiftTable, (ArrayOf<Byte>**)&ret);

    // Paste in the header
    (*ret)[1] = (Byte)header->GetLength();
    //System.arraycopy(header, 0, ret, 2, header.length);
    ret->Copy(2, header, 0, header->GetLength());
    *res = ret;
    REFCOUNT_ADD(*res);

    return NOERROR;
}

ECode CGsmAlphabet::StringToGsm7BitPacked(
    /* [in] */ const String& data,
    /* [out, callee] */ ArrayOf<Byte>** res)
{
    VALIDATE_NOT_NULL(res);
    return StringToGsm7BitPacked(data, 0, TRUE, 0, 0, res);
}

ECode CGsmAlphabet::StringToGsm7BitPacked(
    /* [in] */ const String& data,
    /* [in] */ Int32 languageTable,
    /* [in] */ Int32 languageShiftTable,
    /* [out, callee] */ ArrayOf<Byte>** res)
{
    VALIDATE_NOT_NULL(res);
    return StringToGsm7BitPacked(data, 0, TRUE, languageTable, languageShiftTable, res);
}

ECode CGsmAlphabet::StringToGsm7BitPacked(
    /* [in] */ const String& data,
    /* [in] */ Int32 startingSeptetOffset,
    /* [in] */ Boolean throwException,
    /* [in] */ Int32 languageTable,
    /* [in] */ Int32 languageShiftTable,
    /* [out, callee] */ ArrayOf<Byte>** res)
{
    VALIDATE_NOT_NULL(res);
    Int32 dataLen = data.GetLength();
    Int32 septetCount;
    AutoPtr<ICharSequence> cs;
    CString::New(data, (ICharSequence**)&cs);
    CountGsmSeptetsUsingTables(cs, !throwException,
            languageTable, languageShiftTable, &septetCount);
    if (septetCount == -1) {
        //throw new EncodeException("countGsmSeptetsUsingTables(): unencodable char");
        Slogger::E(TAG, "countGsmSeptetsUsingTables(): unencodable char");
        return E_FAIL;
    }
    septetCount += startingSeptetOffset;
    if (septetCount > 255) {
        //throw new EncodeException("Payload cannot exceed 255 septets");
        Slogger::E(TAG, "Payload cannot exceed 255 septets");
        return E_FAIL;
    }
    Int32 byteCount = ((septetCount * 7) + 7) / 8;
    AutoPtr< ArrayOf<Byte> > ret = ArrayOf<Byte>::Alloc(byteCount + 1);  // Include space for one byte length prefix.
    AutoPtr<SparseInt32Array> charToLanguageTable = (*sCharsToGsmTables)[languageTable];
    AutoPtr<SparseInt32Array> charToShiftTable = (*sCharsToShiftTables)[languageShiftTable];
    for (Int32 i = 0, septets = startingSeptetOffset, bitOffset = startingSeptetOffset * 7;
             i < dataLen && septets < septetCount;
             i++, bitOffset += 7) {
        Char32 c = data.GetChar(i);
        Int32 v = charToLanguageTable->Get(c, -1);
        if (v == -1) {
            v = charToShiftTable->Get(c, -1);  // Lookup the extended char.
            if (v == -1) {
                if (throwException) {
                    //throw new EncodeException("stringToGsm7BitPacked(): unencodable char");
                    Slogger::E(TAG, "stringToGsm7BitPacked(): unencodable char");
                    return E_FAIL;
                } else {
                    v = charToLanguageTable->Get(' ', ' ');   // should return ASCII space
                }
            } else {
                PackSmsChar(ret, bitOffset, GSM_EXTENDED_ESCAPE);
                bitOffset += 7;
                septets++;
            }
        }
        PackSmsChar(ret, bitOffset, v);
        septets++;
    }
    (*ret)[0] = (Byte) (septetCount);  // Validated by check above.
    *res = ret;
    REFCOUNT_ADD(*res);

    return NOERROR;
}

ECode CGsmAlphabet::Gsm7BitPackedToString(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 lengthSeptets,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    return Gsm7BitPackedToString(pdu, offset, lengthSeptets, 0, 0, 0, res);
}

ECode CGsmAlphabet::Gsm7BitPackedToString(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 lengthSeptets,
    /* [in] */ Int32 numPaddingBits,
    /* [in] */ Int32 languageTable,
    /* [in] */ Int32 shiftTable,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    StringBuilder ret;

    if (languageTable < 0 || languageTable > sLanguageTables->GetLength()) {
        Slogger::W(TAG, "unknown language table %d, using default", languageTable);
        languageTable = 0;
    }
    if (shiftTable < 0 || shiftTable > sLanguageShiftTables->GetLength()) {
        Slogger::W(TAG, "unknown single shift table %d, using default", shiftTable);
        shiftTable = 0;
    }

    //try {
    Boolean prevCharWasEscape = FALSE;
    String languageTableToChar = (*sLanguageTables)[languageTable];
    String shiftTableToChar = (*sLanguageShiftTables)[shiftTable];

    if (languageTableToChar.IsEmpty()) {
        Slogger::W(TAG, "no language table for code %d, using default", languageTable);
        languageTableToChar = (*sLanguageTables)[0];
    }
    if (shiftTableToChar.IsEmpty()) {
        Slogger::W(TAG, "no single shift table for code %d, using default", shiftTable);
        shiftTableToChar = (*sLanguageShiftTables)[0];
    }

    for (Int32 i = 0 ; i < lengthSeptets ; i++) {
        Int32 bitOffset = (7 * i) + numPaddingBits;

        Int32 byteOffset = bitOffset / 8;
        Int32 shift = bitOffset % 8;
        Int32 gsmVal;

        gsmVal = (0x7f & ((*pdu)[offset + byteOffset] >> shift));

        // if it crosses a byte boundary
        if (shift > 1) {
            // set msb bits to 0
            gsmVal &= 0x7f >> (shift - 1);

            gsmVal |= 0x7f & ((*pdu)[offset + byteOffset + 1] << (8 - shift));
        }

        if (prevCharWasEscape) {
            if (gsmVal == GSM_EXTENDED_ESCAPE) {
                ret.AppendChar(' ');    // display ' ' for reserved double escape sequence
            } else {
                Char32 c = shiftTableToChar.GetChar(gsmVal);
                if (c == ' ') {
                    ret.AppendChar(languageTableToChar.GetChar(gsmVal));
                } else {
                    ret.AppendChar(c);
                }
            }
            prevCharWasEscape = FALSE;
        } else if (gsmVal == GSM_EXTENDED_ESCAPE) {
            prevCharWasEscape = TRUE;
        } else {
            ret.AppendChar(languageTableToChar.GetChar(gsmVal));
        }
    }
    /*} catch (RuntimeException ex) {
        Slogger::E(TAG, "Error GSM 7 bit packed: ex");
        return null;
    }*/

    *res = ret.ToString();

    return NOERROR;
}

ECode CGsmAlphabet::Gsm8BitUnpackedToString(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    return Gsm8BitUnpackedToString(data, offset, length, String(""), res);
}

ECode CGsmAlphabet::Gsm8BitUnpackedToString(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ const String& characterset,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    /*Boolean isMbcs = FALSE;
    AutoPtr<ICharset> charset;
    AutoPtr<IByteBuffer> mbcsBuffer;
    Boolean tmpRes;

    if (!characterset.IsEmpty()
            && !characterset.RqualsIgnoreCase("us-ascii")
            && (Charset::IsSupported(characterset, &tmpRes), tmpRes)) {
        isMbcs = TRUE;
        Charset::ForName(characterset, (ICharset**)&charset);
        ByteBuffer::Allocate(2, (IByteBuffer**)&mbcsBuffer);
    }

    // Always use GSM 7 bit default alphabet table for this method
    String languageTableToChar32 = (*sLanguageTables)[0].
    String shiftTableToChar32 = (*sLanguageShiftTables)[0].

    StringBuilder ret;
    Boolean prevWasEscape = FALSE;
    for (Int32 i = offset ; i < offset + length ; i++) {
        // Never underestimate the pain that can be caused
        // by signed bytes
        Int32 c = (*data)[i] & 0xff;

        if (c == 0xff) {
            break;
        } else if (c == GSM_EXTENDED_ESCAPE) {
            if (prevWasEscape) {
                // Two escape chars in a row
                // We treat this as a space
                // See Note 1 in table 6.2.1.1 of TS 23.038 v7.00
                ret.AppendChar(' ');
                prevWasEscape = FALSE;
            } else {
                prevWasEscape = TRUE;
            }
        } else {
            if (prevWasEscape) {
                Char32 shiftChar32 = shiftTableToChar.GetChar(c);
                if (shiftChar32 == ' ') {
                    // display character from main table if not present in shift table
                    ret.AppendChar(languageTableToChar.GetChar(c));
                } else {
                    ret.AppendChar(shiftChar);
                }
            } else {
                if (!isMbcs || c < 0x80 || i + 1 >= offset + length) {
                    ret.AppendChar(languageTableToChar.GetChar(c));
                } else {
                    // isMbcs must be TRUE. So both mbcsBuffer and charset are initialized.
                    mbcsBuffer.clear();
                    mbcsBuffer.put(data, i++, 2);
                    mbcsBuffer.flip();
                    AutoPtr<ICharBuffer> cb;
                    charset->Decode(mbcsBuffer, (ICharBuffer**)&cb);
                    String str;
                    cb->ToString(&str);
                    ret.AppendString(str);
                }
            }
            prevWasEscape = FALSE;
        }
    }

    *res = ret.ToString();*/
    assert(0);
    return NOERROR;
}

ECode CGsmAlphabet::StringToGsm8BitPacked(
    /* [in] */ const String& s,
    /* [out, callee] */ ArrayOf<Byte>** res)
{
    VALIDATE_NOT_NULL(res);

    AutoPtr< ArrayOf<Byte> > ret;

    Int32 septets;
    AutoPtr<ICharSequence> cs;
    CString::New(s, (ICharSequence**)&cs);
    CountGsmSeptetsUsingTables(cs, TRUE, 0, 0, &septets);

    // Enough for all the septets and the length byte prefix
    ret = ArrayOf<Byte>::Alloc(septets);

    StringToGsm8BitUnpackedField(s, ret, 0, ret->GetLength());

    *res = ret;
    REFCOUNT_ADD(*res);

    return NOERROR;
}

ECode CGsmAlphabet::StringToGsm8BitUnpackedField(
    /* [in] */ const String& s,
    /* [in] */ ArrayOf<Byte>* dest,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    Int32 outByteIndex = offset;
    AutoPtr<SparseInt32Array> charToLanguageTable = (*sCharsToGsmTables)[0];
    AutoPtr<SparseInt32Array> charToShiftTable = (*sCharsToShiftTables)[0];

    // Septets are stored in byte-aligned octets
    for (Int32 i = 0, sz = s.GetLength()
            ; i < sz && (outByteIndex - offset) < length
            ; i++
    ) {
        Char32 c = s.GetChar(i);

        Int32 v = charToLanguageTable->Get(c, -1);

        if (v == -1) {
            v = charToShiftTable->Get(c, -1);
            if (v == -1) {
                v = charToLanguageTable->Get(' ', ' ');  // fall back to ASCII space
            } else {
                // make sure we can fit an escaped char
                if (! (outByteIndex + 1 - offset < length)) {
                    break;
                }

                (*dest)[outByteIndex++] = GSM_EXTENDED_ESCAPE;
            }
        }

        (*dest)[outByteIndex++] = (Byte)v;
    }

    // pad with 0xff's
    while((outByteIndex - offset) < length) {
        (*dest)[outByteIndex++] = (Byte)0xff;
    }

    return NOERROR;
}

ECode CGsmAlphabet::CountGsmSeptets(
    /* [in] */ Char32 c,
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val);
    *val = 0;
    return CountGsmSeptets(c, FALSE, val);
}

ECode CGsmAlphabet::CountGsmSeptets(
    /* [in] */ Char32 c,
    /* [in] */ Boolean throwsException,
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val);

    if ((*sCharsToGsmTables)[0]->Get(c, -1) != -1) {
        *val = 1;
        return NOERROR;
    }

    if ((*sCharsToShiftTables)[0]->Get(c, -1) != -1) {
        *val = 2;
        return NOERROR;
    }

    if (throwsException) {
        //throw new EncodeException(c);
        return E_FAIL;
    } else {
        // count as a space char
        *val = 1;
        return NOERROR;
    }
}

ECode CGsmAlphabet::CountGsmSeptetsUsingTables(
    /* [in] */ ICharSequence* s,
    /* [in] */ Boolean use7bitOnly,
    /* [in] */ Int32 languageTable,
    /* [in] */ Int32 languageShiftTable,
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val);

    Int32 count = 0;
    String str;
    s->ToString(&str);
    Int32 sz = str.GetLength();
    AutoPtr<SparseInt32Array> charToLanguageTable = (*sCharsToGsmTables)[languageTable];
    AutoPtr<SparseInt32Array> charToShiftTable = (*sCharsToShiftTables)[languageShiftTable];
    for (Int32 i = 0; i < sz; i++) {
        Char32 c = str.GetChar(i);
        if (c == GSM_EXTENDED_ESCAPE) {
            Slogger::W(TAG, "countGsmSeptets() string contains Escape character, skipping.");
            continue;
        }
        if (charToLanguageTable->Get(c, -1) != -1) {
            count++;
        } else if (charToShiftTable->Get(c, -1) != -1) {
            count += 2; // escape + shift table index
        } else if (use7bitOnly) {
            count++;    // encode as space
        } else {
            *val = -1;  // caller must check for this case
            return NOERROR;
        }
    }
    *val = count;
    return NOERROR;
}

ECode CGsmAlphabet::CountGsmSeptets(
    /* [in] */ ICharSequence* s,
    /* [in] */ Boolean use7bitOnly,
    /* [out] */ IGsmAlphabetTextEncodingDetails** res)
{
    VALIDATE_NOT_NULL(res);
    // Load enabled language tables from config.xml, including any MCC overlays
    if (!sDisableCountryEncodingCheck) {
        EnableCountrySpecificEncodings();
    }
    // fast path for common case where no national language shift tables are enabled
    if (sEnabledSingleShiftTables->GetLength() + sEnabledLockingShiftTables->GetLength() == 0) {
        AutoPtr<TextEncodingDetails> ted = new TextEncodingDetails();
        Int32 septets;
        CountGsmSeptetsUsingTables(s, use7bitOnly, 0, 0, &septets);
        if (septets == -1) {
            *res = NULL;
            return NOERROR;
        }
        ted->mCodeUnitSize = ISmsConstants::ENCODING_7BIT;
        ted->mCodeUnitCount = septets;
        if (septets > ISmsConstants::MAX_USER_DATA_SEPTETS) {
            ted->mMsgCount = (septets + (ISmsConstants::MAX_USER_DATA_SEPTETS_WITH_HEADER - 1)) /
                    ISmsConstants::MAX_USER_DATA_SEPTETS_WITH_HEADER;
            ted->mCodeUnitsRemaining = (ted->mMsgCount *
                    ISmsConstants::MAX_USER_DATA_SEPTETS_WITH_HEADER) - septets;
        } else {
            ted->mMsgCount = 1;
            ted->mCodeUnitsRemaining = ISmsConstants::MAX_USER_DATA_SEPTETS - septets;
        }
        ted->mCodeUnitSize = ISmsConstants::ENCODING_7BIT;
        *res = ted;
        return NOERROR;
    }

    return NOERROR;
}

ECode CGsmAlphabet::FindGsmSeptetLimitIndex(
    /* [in] */ const String& s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 limit,
    /* [in] */ Int32 langTable,
    /* [in] */ Int32 langShiftTable,
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val);
    Int32 accumulator = 0;
    Int32 size = s.GetLength();

    AutoPtr<SparseInt32Array> charToLangTable = (*sCharsToGsmTables)[langTable];
    AutoPtr<SparseInt32Array> charToLangShiftTable = (*sCharsToShiftTables)[langShiftTable];
    for (Int32 i = start; i < size; i++) {
        Int32 encodedSeptet = charToLangTable->Get(s.GetChar(i), -1);
        if (encodedSeptet == -1) {
            encodedSeptet = charToLangShiftTable->Get(s.GetChar(i), -1);
            if (encodedSeptet == -1) {
                // Char32 not found, assume we're replacing with space
                accumulator++;
            } else {
                accumulator += 2;  // escape character + shift table index
            }
        } else {
            accumulator++;
        }
        if (accumulator > limit) {
            *val = i;
            return NOERROR;
        }
    }
    *val = size;
    return NOERROR;
}

ECode CGsmAlphabet::SetEnabledSingleShiftTables(
    /* [in] */ ArrayOf<Int32>* tables)
{
    sEnabledSingleShiftTables = tables;
    return NOERROR;
}

ECode CGsmAlphabet::SetEnabledLockingShiftTables(
    /* [in] */ ArrayOf<Int32>* tables)
{
    sEnabledLockingShiftTables = tables;
    return NOERROR;
}

ECode CGsmAlphabet::GetEnabledSingleShiftTables(
    /* [out, callee] */ ArrayOf<Int32>** res)
{
    VALIDATE_NOT_NULL(res);
    *res = sEnabledSingleShiftTables;
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CGsmAlphabet::GetEnabledLockingShiftTables(
    /* [out, callee] */ ArrayOf<Int32>** res)
{
    VALIDATE_NOT_NULL(res);
    *res = sEnabledLockingShiftTables;
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CGsmAlphabet::constructor()
{
    InitStaticVar();
    return NOERROR;
}

ECode CGsmAlphabet::PackSmsChar(
    /* [in] */ ArrayOf<Byte>* packedChars,
    /* [in] */ Int32 bitOffset,
    /* [in] */ Int32 value)
{
    Int32 byteOffset = bitOffset / 8;
    Int32 shift = bitOffset % 8;

    (*packedChars)[++byteOffset] |= value << shift;

    if (shift > 1) {
        (*packedChars)[++byteOffset] = (byte)(value >> (8 - shift));
    }

    return NOERROR;
}

ECode CGsmAlphabet::EnableCountrySpecificEncodings()
{
    AutoPtr<IResourcesHelper> rh;
    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&rh);
    AutoPtr<IResources> r;
    rh->GetSystem((IResources**)&r);
    // See comments in frameworks/base/core/res/res/values/config.xml for allowed values
    sEnabledSingleShiftTables = NULL;
    sEnabledLockingShiftTables = NULL;
    r->GetInt32Array(R::array::config_sms_enabled_single_shift_tables, (ArrayOf<Int32>**)&sEnabledSingleShiftTables);
    r->GetInt32Array(R::array::config_sms_enabled_locking_shift_tables, (ArrayOf<Int32>**)&sEnabledLockingShiftTables);

    if (sEnabledSingleShiftTables->GetLength() > 0) {
        sHighestEnabledSingleShiftCode =
                (*sEnabledSingleShiftTables)[sEnabledSingleShiftTables->GetLength()-1];
    } else {
        sHighestEnabledSingleShiftCode = 0;
    }

    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos