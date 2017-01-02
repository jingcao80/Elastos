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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CICCUTILS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CICCUTILS_H__

#include "_Elastos_Droid_Internal_Telephony_Uicc_CIccUtils.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Graphics::IBitmap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

CarClass(CIccUtils)
    , public Singleton
    , public IIccUtils
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI BcdToString(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ String* result);

    CARAPI CdmaBcdToString(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ String* result);

    CARAPI GsmBcdByteToInt(
        /* [in] */ Byte b,
        /* [out] */ Int32* result);

    CARAPI CdmaBcdByteToInt(
        /* [in] */ Byte b,
        /* [out] */ Int32* result);

    CARAPI AdnStringFieldToString(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ String* result);

    CARAPI HexCharToInt32(
        /* [in] */ Char32 c,
        /* [out] */ Int32* result);

    CARAPI HexStringToBytes(
        /* [in] */ const String& s,
        /* [out, callee] */ ArrayOf<Byte>** result);

    CARAPI BytesToHexString(
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [out] */ String* result);

    CARAPI NetworkNameToString(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ String* result);

    CARAPI ParseToBnW(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 length,
        /* [out] */ IBitmap** result);

    CARAPI ParseToRGB(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 length,
        /* [in] */ Boolean transparency,
        /* [out] */ IBitmap** result);

    CARAPI StringToAdnStringField(
        /* [in] */ const String& alphaTag,
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI StringToAdnStringField(
        /* [in] */ const String& alphaTag,
        /* [in] */ Boolean isUcs2,
        /* [out] */ ArrayOf<Byte>** result);

};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CICCUTILS_H__
