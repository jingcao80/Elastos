
#ifndef __ELASTOS_APPS_DIALER_CALLLOG_CALLLOGLISTITEMHELPER_H__
#define __ELASTOS_APPS_DIALER_CALLLOG_CALLLOGLISTITEMHELPER_H__

#include "_Elastos.Apps.Dialer.h"
#include <elastos/core/Object.h>
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;


namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

class CallLogListItemHelper
    : public Object
{
public:
    /**
    * Creates a new helper instance.
    *
    * @param phoneCallDetailsHelper used to set the details of a phone call
    * @param phoneNumberHelper used to process phone number
    */
     CallLogListItemHelper(
        /* [in] */ IPhoneCallDetailsHelper* phoneCallDetailsHelper,
        /* [in] */ IPhoneNumberDisplayHelper* phoneNumberHelper,
        /* [in] */ IResources* resources);

    /**
     * Sets the name, label, and number for a contact.
     *
     * @param context The application context.
     * @param views the views to populate
     * @param details the details of a phone call needed to fill in the data
     */
    CARAPI_(void) SetPhoneCallDetails(
        /* [in] */ IContext* context,
        /* [in] */ ICallLogListItemViews* views,
        /* [in] */ IPhoneCallDetails* details);

    /**
     * Sets the accessibility descriptions for the action buttons in the action button ViewStub.
     *
     * @param views The views associated with the current call log entry.
     */
    CARAPI_(void) SetActionContentDescriptions(
        /* [in] */ ICallLogListItemViews* views);

    /**
     * Returns the accessibility description of the "return call/call" action for a call log
     * entry.
     * Accessibility text is a combination of:
     * {Voicemail Prefix}. {Number of Calls}. {Caller information}.
     * If most recent call is a voicemail, {Voicemail Prefix} is "New Voicemail.", otherwise "".
     *
     * If more than one call for the caller, {Number of Calls} is:
     * "{number of calls} calls.", otherwise "".
     *
     * The {Caller Information} references the most recent call associated with the caller.
     * For incoming calls:
     * If missed call:  Missed call from {Name/Number} {Call Type} {Call Time}.
     * If answered call: Answered call from {Name/Number} {Call Type} {Call Time}.
     *
     * For outgoing calls:
     * If outgoing:  Call to {Name/Number] {Call Type} {Call Time}.
     *
     * Where:
     * {Name/Number} is the name or number of the caller (as shown in call log).
     * {Call type} is the contact phone number type (eg mobile) or location.
     * {Call Time} is the time since the last call for the contact occurred.
     *
     * Examples:
     * 3 calls.  New Voicemail.  Missed call from Joe Smith mobile 2 hours ago.
     * 2 calls.  Answered call from John Doe mobile.  Last called 1 hour ago.
     *
     * @param context The application context.
     * @param details Details of call.
     * @return Return call action description.
     */
    CARAPI_(AutoPtr<ICharSequence>) GetCallDescription(
        /* [in] */ IContext* context,
        /* [in] */ IPhoneCallDetails* details);

    /**
     * Determine the appropriate string ID to describe a call for accessibility purposes.
     *
     * @param details Call details.
     * @return String resource ID to use.
     */
    CARAPI_(Int32) GetCallDescriptionStringID(
        /* [in] */ IPhoneCallDetails* details);

private:
    /**
     * Returns the accessibility description for the contact badge for a call log entry.
     *
     * @param details Details of call.
     * @return Accessibility description.
     */
    CARAPI_(AutoPtr<ICharSequence>) GetContactBadgeDescription(
        /* [in] */ IPhoneCallDetails* details);

    /**
     * Determine the call type for the most recent call.
     * @param callTypes Call types to check.
     * @return Call type.
     */
    CARAPI_(Int32) GetLastCallType(
        /* [in] */ ArrayOf<Int32>* callTypes);

    /**
     * Return the name or number of the caller specified by the details.
     * @param details Call details
     * @return the name (if known) of the caller, otherwise the formatted number.
     */
    CARAPI_(AutoPtr<ICharSequence>) GetNameOrNumber(
        /* [in] */ IPhoneCallDetails* details);

private:
    /** Helper for populating the details of a phone call. */
    AutoPtr<IPhoneCallDetailsHelper> mPhoneCallDetailsHelper;
    /** Helper for handling phone numbers. */
    AutoPtr<IPhoneNumberDisplayHelper> mPhoneNumberHelper;
    /** Resources to look up strings. */
    AutoPtr<IResources> mResources;
};

} // CallLog
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CALLLOG_CALLLOGLISTITEMHELPER_H__
