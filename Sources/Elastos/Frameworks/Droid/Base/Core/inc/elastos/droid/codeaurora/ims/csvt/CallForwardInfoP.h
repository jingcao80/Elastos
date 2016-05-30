#ifndef __ELASTOS_DROID_CODEAURORA_IMS_CSVT_CALLFORWARDINFOP_H
#define __ELASTOS_DROID_CODEAURORA_IMS_CSVT_CALLFORWARDINFOP_H

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.CodeAurora.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace CodeAurora {
namespace Ims {
namespace Csvt {

class CallForwardInfoP
    : public ICallForwardInfoP
    , public IParcelable
    , public Object
{
public:
    CAR_INTERFACE_DECL();

    TO_STRING_IMPL("CallForwardInfoP");

    CallForwardInfoP();

    CARAPI constructor();

    CARAPI GetStatus(
        /* [out] */ Int32* status);

    CARAPI SetStatus(
        /* [in] */ Int32 status);

    CARAPI GetReason(
        /* [out] */ Int32* reason);

    CARAPI SetReason(
        /* [in] */ Int32 reason);

    CARAPI GetServiceClass(
        /* [out] */ Int32* serviceClass);

    CARAPI SetServiceClass(
        /* [in] */ Int32 serviceClass);

    CARAPI GetToa(
        /* [out] */ Int32* toa);

    CARAPI SetToa(
        /* [in] */ Int32 toa);

    CARAPI GetNumber(
        /* [out] */ String* number);

    CARAPI SetNumber(
        /* [in] */ const String& number);

    CARAPI GetTimeSeconds(
        /* [out] */ Int32* timeSeconds);

    CARAPI SetTimeSeconds(
        /* [in] */ Int32 timeSeconds);

    /**
      * Implement the Parcelable interface.
      * @hide
      */
    CARAPI ReadFromParcel(
      /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
      /* [in] */ IParcel* dest);

private:

    Int32 status;      /*1 = active, 0 = not active */
    Int32 reason;      /* from TS 27.007 7.11 "reason" */
    Int32 serviceClass; /*Sum of CommandsInterface.SERVICE_CLASS */
    Int32 toa;         /* "type" from TS 27.007 7.11 */
    String number;      /* "number" from TS 27.007 7.11 */
    Int32 timeSeconds; /* for CF no reply only */

    //@Override
    //public Int32 DescribeContents() {
    //    return 0;
    //}

    //public static const Parcelable.Creator<CallForwardInfoP> CREATOR = new
    //        Parcelable.Creator<CallForwardInfoP>() {

    //            //@Override
    //            public CallForwardInfoP CreateFromParcel(Parcel in) {
    //                return new CallForwardInfoP(in);
    //            }

    //            //@Override
    //            public CallForwardInfoP[] NewArray(Int32 size) {
    //                return new CallForwardInfoP[size];
    //            }
    //        };
};

} // namespace Csvt
} // namespace Ims
} // namespace Codeaurora
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CODEAURORA_IMS_CSVT_CALLFORWARDINFOP_H
