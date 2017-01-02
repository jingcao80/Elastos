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

#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/google/mms/pdu/CharacterSets.h"

#include <elastos/core/CoreUtils.h>

using Elastos::Core::CoreUtils;

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
    AutoPtr<IInterface> p;
    MIBENUM_TO_NAME_MAP->Get(CoreUtils::Convert(mibEnumValue), (IInterface**)&p);
    String name;
    ICharSequence::Probe(p)->ToString(&name);
    if (name.IsNull()) {
        // throw new UnsupportedEncodingException();
        return String(NULL);
    }
    return name;
}

Int32 CharacterSets::GetMibEnumValue(
    /* [in] */ const String& mimeName)
{
    if (mimeName.IsNull()) {
        return -1;
    }

    AutoPtr<IInterface> p;
    NAME_TO_MIBENUM_MAP->Get(CoreUtils::Convert(mimeName), (IInterface**)&p);
    AutoPtr<IInteger32> mibEnumValue = IInteger32::Probe(p);
    if (mibEnumValue == NULL) {
        // throw new UnsupportedEncodingException();
        return -1;
    }
    Int32 res = 0;
    mibEnumValue->GetValue(&res);
    return res;
}

CharacterSets::CharacterSets()
{
}

AutoPtr<ArrayOf<Int32> > CharacterSets::MiddleInitMibenumNumbers()
{
    AutoPtr<ArrayOf<Int32> > result = ArrayOf<Int32>::Alloc(16);
    (*result)[0] = ANY_CHARSET;
    (*result)[1] = US_ASCII;
    (*result)[2] = ISO_8859_1;
    (*result)[3] = ISO_8859_2;
    (*result)[4] = ISO_8859_3;
    (*result)[5] = ISO_8859_4;
    (*result)[6] = ISO_8859_5;
    (*result)[7] = ISO_8859_6;
    (*result)[8] = ISO_8859_7;
    (*result)[9] = ISO_8859_8;
    (*result)[10] = ISO_8859_9;
    (*result)[11] = SHIFT_JIS;
    (*result)[12] = UTF_8;
    (*result)[13] = BIG5;
    (*result)[14] = UCS2;
    (*result)[15] = UTF_16;
    return result;
}

AutoPtr<ArrayOf<String> > CharacterSets::MiddleInitMimeNames()
{
    AutoPtr<ArrayOf<String> > result = ArrayOf<String>::Alloc(16);
    (*result)[0] = MIMENAME_ANY_CHARSET;
    (*result)[1] = MIMENAME_US_ASCII;
    (*result)[2] = MIMENAME_ISO_8859_1;
    (*result)[3] = MIMENAME_ISO_8859_2;
    (*result)[4] = MIMENAME_ISO_8859_3;
    (*result)[5] = MIMENAME_ISO_8859_4;
    (*result)[6] = MIMENAME_ISO_8859_5;
    (*result)[7] = MIMENAME_ISO_8859_6;
    (*result)[8] = MIMENAME_ISO_8859_7;
    (*result)[9] = MIMENAME_ISO_8859_8;
    (*result)[10] = MIMENAME_ISO_8859_9;
    (*result)[11] = MIMENAME_SHIFT_JIS;
    (*result)[12] = MIMENAME_UTF_8;
    (*result)[13] = MIMENAME_BIG5;
    (*result)[14] = MIMENAME_UCS2;
    (*result)[15] = MIMENAME_UTF_16;
    return result;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
