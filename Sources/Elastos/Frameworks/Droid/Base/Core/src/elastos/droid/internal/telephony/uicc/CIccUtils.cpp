#include "elastos/droid/internal/telephony/uicc/CIccUtils.h"
#include "elastos/droid/internal/telephony/uicc/UiccIccUtils.h"
#include "Elastos.Droid.Graphics.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

CAR_INTERFACE_IMPL(CIccUtils, Singleton, IIccUtils)
CAR_SINGLETON_IMPL(CIccUtils)

ECode CIccUtils::BcdToString(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = UiccIccUtils::BcdToString(data, offset, length);
    return NOERROR;
}

ECode CIccUtils::CdmaBcdToString(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = UiccIccUtils::CdmaBcdToString(data, offset, length);
    return NOERROR;
}

ECode CIccUtils::GsmBcdByteToInt(
    /* [in] */ Byte b,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = UiccIccUtils::GsmBcdByteToInt(b);
    return NOERROR;
}

ECode CIccUtils::CdmaBcdByteToInt(
    /* [in] */ Byte b,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = UiccIccUtils::CdmaBcdByteToInt(b);
    return NOERROR;
}

ECode CIccUtils::AdnStringFieldToString(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = UiccIccUtils::AdnStringFieldToString(data, offset, length);
    return NOERROR;
}

ECode CIccUtils::HexCharToInt(
    /* [in] */ Char16 c,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = UiccIccUtils::HexCharToInt32(c);
    return NOERROR;
}

ECode CIccUtils::HexStringToBytes(
    /* [in] */ const String& s,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<Byte> > bytes = UiccIccUtils::HexStringToBytes(s);
    *result = bytes;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CIccUtils::BytesToHexString(
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = UiccIccUtils::BytesToHexString(bytes);
    return NOERROR;
}

ECode CIccUtils::NetworkNameToString(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = UiccIccUtils::NetworkNameToString(data, offset, length);
    return NOERROR;
}

ECode CIccUtils::ParseToBnW(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 length,
    /* [out] */ IBitmap** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IBitmap> bm = UiccIccUtils::ParseToBnW(data, length);
    *result = bm;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CIccUtils::ParseToRGB(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 length,
    /* [in] */ Boolean transparency,
    /* [out] */ IBitmap** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IBitmap> bm = UiccIccUtils::ParseToRGB(data, length, transparency);
    *result = bm;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CIccUtils::StringToAdnStringField(
    /* [in] */ const String& alphaTag,
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<Byte> > array = UiccIccUtils::StringToAdnStringField(alphaTag);
    *result = array;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CIccUtils::StringToAdnStringField(
    /* [in] */ const String& alphaTag,
    /* [in] */ Boolean isUcs2,
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<Byte> > array = UiccIccUtils::StringToAdnStringField(alphaTag, isUcs2);
    *result = array;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
