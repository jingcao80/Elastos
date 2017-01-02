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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CGSMALPHABETHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CGSMALPHABETHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_CGsmAlphabetHelper.h"

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CGsmAlphabetHelper)
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI CharToGsm(
        /* [in] */ Char32 c,
        /* [out] */ Int32* val);

    CARAPI CharToGsm(
        /* [in] */ Char32 c,
        /* [in] */ Boolean throwException,
        /* [out] */ Int32* val);

    CARAPI CharToGsmExtended(
        /* [in] */ Char32 c,
        /* [out] */ Int32* val);

    CARAPI GsmToChar(
        /* [in] */ Int32 gsmChar,
        /* [out] */ Char32* res);

    CARAPI GsmExtendedToChar(
        /* [in] */ Int32 gsmChar,
        /* [out] */ Char32* res);

    CARAPI StringToGsm7BitPackedWithHeader(
        /* [in] */ const String& data,
        /* [in] */ ArrayOf<Byte>* header,
        /* [out, callee] */ ArrayOf<Byte>** res);

    CARAPI StringToGsm7BitPackedWithHeader(
        /* [in] */ const String& data,
        /* [in] */ ArrayOf<Byte>* header,
        /* [in] */ Int32 languageTable,
        /* [in] */ Int32 languageShiftTable,
        /* [out, callee] */ ArrayOf<Byte>** res);

    CARAPI StringToGsm7BitPacked(
        /* [in] */ const String& data,
        /* [out, callee] */ ArrayOf<Byte>** res);

    CARAPI StringToGsm7BitPacked(
        /* [in] */ const String& data,
        /* [in] */ Int32 languageTable,
        /* [in] */ Int32 languageShiftTable,
        /* [out, callee] */ ArrayOf<Byte>** res);

    CARAPI StringToGsm7BitPacked(
        /* [in] */ const String& data,
        /* [in] */ Int32 startingSeptetOffset,
        /* [in] */ Boolean throwException,
        /* [in] */ Int32 languageTable,
        /* [in] */ Int32 languageShiftTable,
        /* [out, callee] */ ArrayOf<Byte>** res);

    CARAPI Gsm7BitPackedToString(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 lengthSeptets,
        /* [out] */ String* res);

    CARAPI Gsm7BitPackedToString(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 lengthSeptets,
        /* [in] */ Int32 numPaddingBits,
        /* [in] */ Int32 languageTable,
        /* [in] */ Int32 shiftTable,
        /* [out] */ String* res);

    CARAPI Gsm8BitUnpackedToString(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ String* res);

    CARAPI Gsm8BitUnpackedToString(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [in] */ const String& characterset,
        /* [out] */ String* res);

    CARAPI StringToGsm8BitPacked(
        /* [in] */ const String& s,
        /* [out, callee] */ ArrayOf<Byte>** res);

    CARAPI StringToGsm8BitUnpackedField(
        /* [in] */ const String& s,
        /* [in] */ ArrayOf<Byte>* dest,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    CARAPI CountGsmSeptets(
        /* [in] */ Char32 c,
        /* [out] */ Int32* val);

    CARAPI CountGsmSeptets(
        /* [in] */ Char32 c,
        /* [in] */ Boolean throwsException,
        /* [out] */ Int32* val);

    CARAPI CountGsmSeptetsUsingTables(
        /* [in] */ ICharSequence* s,
        /* [in] */ Boolean use7bitOnly,
        /* [in] */ Int32 languageTable,
        /* [in] */ Int32 languageShiftTable,
        /* [out] */ Int32* val);

    CARAPI CountGsmSeptets(
        /* [in] */ ICharSequence* s,
        /* [in] */ Boolean use7bitOnly,
        /* [out] */ IGsmAlphabetTextEncodingDetails** res);

    CARAPI FindGsmSeptetLimitIndex(
        /* [in] */ const String& s,
        /* [in] */ Int32 start,
        /* [in] */ Int32 limit,
        /* [in] */ Int32 langTable,
        /* [in] */ Int32 langShiftTable,
        /* [out] */ Int32* val);

    CARAPI SetEnabledSingleShiftTables(
        /* [in] */ ArrayOf<Int32>* tables);

    CARAPI SetEnabledLockingShiftTables(
        /* [in] */ ArrayOf<Int32>* tables);

    CARAPI GetEnabledSingleShiftTables(
        /* [out, callee] */ ArrayOf<Int32>** res);

    CARAPI GetEnabledLockingShiftTables(
        /* [out, callee] */ ArrayOf<Int32>** res);

private:
};

}
}
}
}

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CGSMALPHABETHELPER_H__
