#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_SIP_SIPCALLBASE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_SIP_SIPCALLBASE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/internal/telephony/Call.h"

// import com.android.internal.telephony.Connection;
// import java.util.Iterator;
// import java.util.List;

using Elastos::Droid::Internal::Telephony::Call;
using Elastos::Droid::Internal::Telephony::ICallState;
using Elastos::Utility::IList;


namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Sip {

class SipCallBase
    : public Call
    //, public ISipCallBase
{
public:
    // @Override
    CARAPI GetConnections(
        /* [out] */ IList/*<Connection>*/** result);

    // @Override
    CARAPI IsMultiparty(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    virtual CARAPI ClearDisconnected();

protected:
    virtual CARAPI_(void) SetState(
        /* [in] */ ICallState* newState) = 0;
};

} // namespace Sip
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_SIP_SIPCALLBASE_H__

