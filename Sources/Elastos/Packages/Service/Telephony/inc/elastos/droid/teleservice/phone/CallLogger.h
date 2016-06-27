#ifndef  __ELASTOS_DROID_PHONE_CALLLOGGER_H__
#define  __ELASTOS_DROID_PHONE_CALLLOGGER_H__

#include "_Elastos.Droid.TeleService.h"
#include <elastos/core/Object.h>
#include "Elastos.Droid.Internal.h"

using Elastos::Droid::Internal::Telephony::ICallerInfo;
using Elastos::Droid::Internal::Telephony::IConnection;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * Helper class for interacting with the call log.
 */
class CallLogger
    : public Object
{
public:
    TO_STRING_IMPL("CallLogger")

    CallLogger(
        /* [in] */ IPhoneGlobals* application,
        /* [in] */ Object* callLogAsync = NULL);

    /**
     * Logs a call to the call log based on the connection object passed in.
     *
     * @param c The connection object for the call being logged.
     * @param callLogType The type of call log entry.
     */
    CARAPI LogCall(
        /* [in] */ IConnection* c,
        /* [in] */ Int32 callLogType);

    /**
     * Came as logCall(Connection,int) but calculates the call type from the connection object.
     */
    CARAPI LogCall(
        /* [in] */ IConnection* c);

    /**
     * Logs a call to the call from the parameters passed in.
     */
    CARAPI LogCall(
        /* [in] */ ICallerInfo* ci,
        /* [in] */ const String& number,
        /* [in] */ Int32 presentation,
        /* [in] */ Int32 callType,
        /* [in] */ Int64 start,
        /* [in] */ Int64 duration);

private:
    /**
     * Get the caller info.
     *
     * @param conn The phone connection.
     * @return The CallerInfo associated with the connection. Maybe null.
     */
    CARAPI_(AutoPtr<ICallerInfo>) GetCallerInfoFromConnection(
        /* [in] */ IConnection* conn);

    /**
     * Retrieve the phone number from the caller info or the connection.
     *
     * For incoming call the number is in the Connection object. For
     * outgoing call we use the CallerInfo phoneNumber field if
     * present. All the processing should have been done already (CDMA vs GSM numbers).
     *
     * If CallerInfo is missing the phone number, get it from the connection.
     * Apply the Call Name Presentation (CNAP) transform in the connection on the number.
     *
     * @param conn The phone connection.
     * @param callerInfo The CallerInfo. Maybe null.
     * @return the phone number.
     */
    CARAPI_(String) GetLogNumber(
        /* [in] */ IConnection* conn,
        /* [in] */ ICallerInfo* callerInfo);

    /**
     * Get the presentation from the callerinfo if not null otherwise,
     * get it from the connection.
     *
     * @param conn The phone connection.
     * @param callerInfo The CallerInfo. Maybe null.
     * @return The presentation to use in the logs.
     */
    CARAPI_(Int32) GetPresentation(
        /* [in] */ IConnection* conn,
        /* [in] */ ICallerInfo* callerInfo);

    CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    static const String TAG;
    static Boolean DBG;
    static Boolean VDBG;

    AutoPtr<IPhoneGlobals> mApplication;
    // AutoPtr<CallLogAsync> mCallLog;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CALLLOGGER_H__