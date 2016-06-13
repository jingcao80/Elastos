
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/IccIoResult.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                             IccIoResult
//=====================================================================
CAR_INTERFACE_IMPL(IccIoResult, Object, IIccIoResult);

IccIoResult::IccIoResult()
{
}

ECode IccIoResult::constructor(
    /* [in] */ Int32 sw1,
    /* [in] */ Int32 sw2,
    /* [in] */ ArrayOf<Byte>* payload)
{
    // ==================before translated======================
    // this.sw1 = sw1;
    // this.sw2 = sw2;
    // this.payload = payload;
    return NOERROR;
}

ECode IccIoResult::constructor(
    /* [in] */ Int32 sw1,
    /* [in] */ Int32 sw2,
    /* [in] */ const String& hexString)
{
    // ==================before translated======================
    // this(sw1, sw2, IccUtils.hexStringToBytes(hexString));
    return NOERROR;
}

ECode IccIoResult::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return "IccIoResponse sw1:0x" + Integer.toHexString(sw1) + " sw2:0x"
    //         + Integer.toHexString(sw2);
    assert(0);
    return NOERROR;
}

ECode IccIoResult::Success(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return sw1 == 0x90 || sw1 == 0x91 || sw1 == 0x9e || sw1 == 0x9f;
    assert(0);
    return NOERROR;
}

ECode IccIoResult::GetException(
    /* [out] */ IIccException** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (success()) return null;
    //
    // switch (sw1) {
    //     case 0x94:
    //         if (sw2 == 0x08) {
    //             return new IccFileTypeMismatch();
    //         } else {
    //             return new IccFileNotFound();
    //         }
    //     default:
    //         return new IccException("sw1:" + sw1 + " sw2:" + sw2);
    // }
    assert(0);
    return NOERROR;
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
