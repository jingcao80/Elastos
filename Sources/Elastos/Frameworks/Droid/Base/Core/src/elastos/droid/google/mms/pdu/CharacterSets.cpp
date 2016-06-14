#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/google/mms/pdu/CharacterSets.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                            CharacterSets
//=====================================================================
const Int32 CharacterSets::ANY_CHARSET;
const Int32 CharacterSets::US_ASCII;
const Int32 CharacterSets::ISO_8859_1;
const Int32 CharacterSets::ISO_8859_2;
const Int32 CharacterSets::ISO_8859_3;
const Int32 CharacterSets::ISO_8859_4;
const Int32 CharacterSets::ISO_8859_5;
const Int32 CharacterSets::ISO_8859_6;
const Int32 CharacterSets::ISO_8859_7;
const Int32 CharacterSets::ISO_8859_8;
const Int32 CharacterSets::ISO_8859_9;
const Int32 CharacterSets::SHIFT_JIS;
const Int32 CharacterSets::UTF_8;
const Int32 CharacterSets::BIG5;
const Int32 CharacterSets::UCS2;
const Int32 CharacterSets::UTF_16;
const Int32 CharacterSets::DEFAULT_CHARSET;
const String CharacterSets::MIMENAME_ANY_CHARSET("*");
const String CharacterSets::MIMENAME_US_ASCII("us-ascii");
const String CharacterSets::MIMENAME_ISO_8859_1("iso-8859-1");
const String CharacterSets::MIMENAME_ISO_8859_2("iso-8859-2");
const String CharacterSets::MIMENAME_ISO_8859_3("iso-8859-3");
const String CharacterSets::MIMENAME_ISO_8859_4("iso-8859-4");
const String CharacterSets::MIMENAME_ISO_8859_5("iso-8859-5");
const String CharacterSets::MIMENAME_ISO_8859_6("iso-8859-6");
const String CharacterSets::MIMENAME_ISO_8859_7("iso-8859-7");
const String CharacterSets::MIMENAME_ISO_8859_8("iso-8859-8");
const String CharacterSets::MIMENAME_ISO_8859_9("iso-8859-9");
const String CharacterSets::MIMENAME_SHIFT_JIS("shift_JIS");
const String CharacterSets::MIMENAME_UTF_8("utf-8");
const String CharacterSets::MIMENAME_BIG5("big5");
const String CharacterSets::MIMENAME_UCS2("iso-10646-ucs-2");
const String CharacterSets::MIMENAME_UTF_16("utf-16");
const String CharacterSets::DEFAULT_CHARSET_NAME(MIMENAME_UTF_8);
AutoPtr<ArrayOf<Int32> > CharacterSets::MIBENUM_NUMBERS = CharacterSets::MiddleInitMibenumNumbers();
AutoPtr<ArrayOf<String> > CharacterSets::MIME_NAMES = CharacterSets::MiddleInitMimeNames();
AutoPtr<IHashMap> CharacterSets::MIBENUM_TO_NAME_MAP;
AutoPtr<IHashMap> CharacterSets::NAME_TO_MIBENUM_MAP;

String CharacterSets::GetMimeName(
    /* [in] */ Int32 mibEnumValue)
{
    // ==================before translated======================
    // String name = MIBENUM_TO_NAME_MAP.get(mibEnumValue);
    // if (name == null) {
    //     throw new UnsupportedEncodingException();
    // }
    // return name;
    assert(0);
    return String("");
}

Int32 CharacterSets::GetMibEnumValue(
    /* [in] */ const String& mimeName)
{
    // ==================before translated======================
    // if(null == mimeName) {
    //     return -1;
    // }
    //
    // Integer mibEnumValue = NAME_TO_MIBENUM_MAP.get(mimeName);
    // if (mibEnumValue == null) {
    //     throw new UnsupportedEncodingException();
    // }
    // return mibEnumValue;
    assert(0);
    return 0;
}

CharacterSets::CharacterSets()
{
}

AutoPtr< ArrayOf<Int32> > CharacterSets::MiddleInitMibenumNumbers()
{
    // ==================before translated======================
    // ->WWZ_SIGN: ARRAY_INIT_START {
    // ANY_CHARSET,
    //          US_ASCII,
    //          ISO_8859_1,
    //          ISO_8859_2,
    //          ISO_8859_3,
    //          ISO_8859_4,
    //          ISO_8859_5,
    //          ISO_8859_6,
    //          ISO_8859_7,
    //          ISO_8859_8,
    //          ISO_8859_9,
    //          SHIFT_JIS,
    //          UTF_8,
    //          BIG5,
    //          UCS2,
    //          UTF_16,
    // ->WWZ_SIGN: ARRAY_INIT_END }
    assert(0);
    AutoPtr< ArrayOf<Int32> > empty;
    return empty;
}

AutoPtr< ArrayOf<String> > CharacterSets::MiddleInitMimeNames()
{
    // ==================before translated======================
    // ->WWZ_SIGN: ARRAY_INIT_START {
    // MIMENAME_ANY_CHARSET,
    //          MIMENAME_US_ASCII,
    //          MIMENAME_ISO_8859_1,
    //          MIMENAME_ISO_8859_2,
    //          MIMENAME_ISO_8859_3,
    //          MIMENAME_ISO_8859_4,
    //          MIMENAME_ISO_8859_5,
    //          MIMENAME_ISO_8859_6,
    //          MIMENAME_ISO_8859_7,
    //          MIMENAME_ISO_8859_8,
    //          MIMENAME_ISO_8859_9,
    //          MIMENAME_SHIFT_JIS,
    //          MIMENAME_UTF_8,
    //          MIMENAME_BIG5,
    //          MIMENAME_UCS2,
    //          MIMENAME_UTF_16,
    // ->WWZ_SIGN: ARRAY_INIT_END }
    assert(0);
    AutoPtr< ArrayOf<String> > empty;
    return empty;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
