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
