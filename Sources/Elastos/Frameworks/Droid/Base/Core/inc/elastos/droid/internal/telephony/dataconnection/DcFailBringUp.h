
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCFAILBRINGUP_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCFAILBRINGUP_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

/**
 * A package visible class for supporting testing failing bringUp commands. This
 * saves the parameters from a action_fail_bringup intent. See
 * {@link DataConnection#doOnConnect} and {@see DcTesterFailBringUpAll} for more info.
 */
class DcFailBringUp
    : public Object
{
public:
    // Get the Extra Intent parameters
    CARAPI SaveParameters(
        /* [in] */ IIntent* intent,
        /* [in] */ const String& s);

    CARAPI SaveParameters(
        /* [in] */ Int32 counter,
        /* [in] */ Int32 failCause,
        /* [in] */ Int32 suggestedRetryTime);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

private:
    static CARAPI Log(
        /* [in] */ const String& s);

public:
    static const String INTENT_BASE;

    static const String ACTION_FAIL_BRINGUP;

    // counter with its --ei option name and default value
    static const String COUNTER;

    static const Int32 DEFAULT_COUNTER;

    Int32 mCounter;

    // failCause with its --ei option name and default value
    static const String FAIL_CAUSE;

    static const EnumDcFailCause DEFAULT_FAIL_CAUSE;

    AutoPtr<IDcFailCause> mFailCause;

    // suggestedRetryTime with its --ei option name and default value
    static const String SUGGESTED_RETRY_TIME;

    static const Int32 DEFAULT_SUGGESTED_RETRY_TIME;

    Int32 mSuggestedRetryTime;

private:
    static const String LOG__TAG;

    static const Boolean DBG;
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCFAILBRINGUP_H__
