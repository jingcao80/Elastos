
#ifndef __ELASTOS_DROID_TELEPHONY_CICCOPENLOGICALCHANNELRESPONSE_H__
#define __ELASTOS_DROID_TELEPHONY_CICCOPENLOGICALCHANNELRESPONSE_H__

#include "_Elastos_Droid_Telephony_CIccOpenLogicalChannelResponse.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CIccOpenLogicalChannelResponse)
    , public Object
    , public IIccOpenLogicalChannelResponse
    , public IParcelable
{
public:
    CIccOpenLogicalChannelResponse();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 channel,
        /* [in] */ Int32 status,
        /* [in] */ ArrayOf<Byte>* selectResponse);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetChannel(
        /* [out] */ Int32* result);

    CARAPI GetStatus(
        /* [out] */ Int32* result);

    CARAPI GetSelectResponse(
        /* [out, callee] */ ArrayOf<Byte>** result);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    Int32 mChannel;
    Int32 mStatus;
    AutoPtr<ArrayOf<Byte> > mSelectResponse;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CICCOPENLOGICALCHANNELRESPONSE_H__
