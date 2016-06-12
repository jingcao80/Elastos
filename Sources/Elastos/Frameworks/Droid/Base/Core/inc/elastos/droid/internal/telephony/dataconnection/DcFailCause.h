
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCFAILCAUSE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCFAILCAUSE_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

/**
 * Returned as the reason for a connection failure as defined
 * by RIL_DataCallFailCause in ril.h and some local errors.
 */
class DcFailCause
    : public Object
    , public IDcFailCause
{
public:
    CAR_INTERFACE_DECL()

    DcFailCause();

    CARAPI constructor(
        /* [in] */ Int32 errorCode);

    CARAPI GetErrorCode(
        /* [out] */ Int32* result);

    /** Radio has failed such that the radio should be restarted */
    CARAPI IsRestartRadioFail(
        /* [out] */ Boolean* result);

    CARAPI IsPermanentFail(
        /* [out] */ Boolean* result);

    CARAPI IsEventLoggable(
        /* [out] */ Boolean* result);

    static CARAPI FromInt32(
        /* [in] */ Int32 errorCode,
        /* [out] */ IDcFailCause** result);

private:
    static CARAPI_(AutoPtr<IHashMap>) InitErrorCodeToFailCauseMap();

private:
    Boolean mRestartRadioOnRegularDeactivation;

    Int32 mErrorCode;

    static AutoPtr<IHashMap> sErrorCodeToFailCauseMap;
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCFAILCAUSE_H__
