
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/ResultException.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

//=====================================================================
//                           ResultException
//=====================================================================
CAR_INTERFACE_IMPL(ResultException, Object, IResultException);

ResultException::ResultException()
{
}

ECode ResultException::constructor(
    /* [in] */ ResultCode result)
{
    // ==================before translated======================
    // super();
    //
    // // ETSI TS 102 223, 8.12 -- For the general results '20', '21', '26',
    // // '38', '39', '3A', '3C', and '3D', it is mandatory for the terminal
    // // to provide a specific cause value as additional information.
    // switch (result) {
    //     case TERMINAL_CRNTLY_UNABLE_TO_PROCESS:    // 0x20
    //     case NETWORK_CRNTLY_UNABLE_TO_PROCESS:     // 0x21
    //     case LAUNCH_BROWSER_ERROR:                 // 0x26
    //     case MULTI_CARDS_CMD_ERROR:                // 0x38
    //     case USIM_CALL_CONTROL_PERMANENT:          // 0x39
    //     case BIP_ERROR:                            // 0x3a
    //     case FRAMES_ERROR:                         // 0x3c
    //     case MMS_ERROR:                            // 0x3d
    //         throw new AssertionError(
    //                 "For result code, " + result +
    //                 ", additional information must be given!");
    //     default:
    //         break;
    // }
    //
    // mResult = result;
    // mAdditionalInfo = -1;
    // mExplanation = "";
    return NOERROR;
}

ECode ResultException::constructor(
    /* [in] */ ResultCode result,
    /* [in] */ const String& explanation)
{
    // ==================before translated======================
    // this(result);
    // mExplanation = explanation;
    return NOERROR;
}

ECode ResultException::constructor(
    /* [in] */ ResultCode result,
    /* [in] */ Int32 additionalInfo)
{
    // ==================before translated======================
    // this(result);
    //
    // if (additionalInfo < 0) {
    //     throw new AssertionError(
    //             "Additional info must be greater than zero!");
    // }
    //
    // mAdditionalInfo = additionalInfo;
    return NOERROR;
}

ECode ResultException::constructor(
    /* [in] */ ResultCode result,
    /* [in] */ Int32 additionalInfo,
    /* [in] */ const String& explanation)
{
    // ==================before translated======================
    // this(result, additionalInfo);
    // mExplanation = explanation;
    return NOERROR;
}

ECode ResultException::Result(
    /* [out] */ ResultCode* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mResult;
    assert(0);
    return NOERROR;
}

ECode ResultException::HasAdditionalInfo(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mAdditionalInfo >= 0;
    assert(0);
    return NOERROR;
}

ECode ResultException::AdditionalInfo(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mAdditionalInfo;
    assert(0);
    return NOERROR;
}

ECode ResultException::Explanation(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mExplanation;
    assert(0);
    return NOERROR;
}

ECode ResultException::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return "result=" + mResult + " additionalInfo=" + mAdditionalInfo +
    //         " explantion=" + mExplanation;
    assert(0);
    return NOERROR;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
