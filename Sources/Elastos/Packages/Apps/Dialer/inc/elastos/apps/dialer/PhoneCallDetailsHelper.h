#ifndef __ELASTOS_APPS_DIALER_PHONECALLDETAILSHELPER_H__
#define __ELASTOS_APPS_DIALER_PHONECALLDETAILSHELPER_H__

#include "Elastos.Droid.App.h"

using Elastos::Apps::Dialer::CallLog::ICallTypeHelper;
using Elastos::Apps::Dialer::CallLog::IPhoneNumberUtilsWrapper;
using Elastos::Apps::Dialer::CallLog::IPhoneNumberDisplayHelper;

namespace Elastos {
namespace Apps {
namespace Dialer {

/**
 * Helper class to fill in the views in {@link PhoneCallDetailsViews}.
 */
class PhoneCallDetailsHelper
    : public Object
    , public IPhoneCallDetailsHelper
{
public:
    CAR_INTERFACE_DECL()

    PhoneCallDetailsHelper();

    /**
     * Creates a new instance of the helper.
     * <p>
     * Generally you should have a single instance of this helper in any context.
     *
     * @param resources used to look up strings
     */
    CARAPI construtor(
        /* [in] */ IResources* resources,
        /* [in] */ ICallTypeHelper* callTypeHelper,
        /* [in] */ IPhoneNumberUtilsWrapper* phoneUtils);

    /** Fills the call details views with content. */
    CARAPI SetPhoneCallDetails(
        /* [in] */ IPhoneCallDetailsViews* views,
        /* [in] */ IPhoneCallDetails* details);

    /**
     * For a call, if there is an associated contact for the caller, return the known call type
     * (e.g. mobile, home, work).  If there is no associated contact, attempt to use the caller's
     * location if known.
     * @param details Call details to use.
     * @return Type of call (mobile/home) if known, or the location of the caller (if known).
     */
    CARAPI GetCallTypeOrLocation(
        /* [in] */ IPhoneCallDetails* details,
        /* [out] */ ICharSequence* result);

    /**
     * Get the call date/time of the call, relative to the current time.
     * e.g. 3 minutes ago
     * @param details Call details to use.
     * @return String representing when the call occurred.
     */
    CARAPI GetCallDate(
        /* [in] */ IPhoneCallDetails* details,
        /* [out] */ ICharSequence** result);

    /** Sets the text of the header view for the details page of a phone call. */
    // @NeededForTesting
    CARAPI SetCallDetailsHeader(
        /* [in] */ ITextView* nameView,
        /* [in] */ IPhoneCallDetails* details);

    // @NeededForTesting
    CARAPI SetCurrentTimeForTest(
        /* [in] */ Int64 currentTimeMillis);

private:
    /**
     * Builds a string containing the call location and date.
     *
     * @param details The call details.
     * @return The call location and date string.
     */
    CARAPI_(AutoPtr<ICharSequence>) GetCallLocationAndDate(
        /* [in] */ IPhoneCallDetails* details);

    /**
     * Returns the current time in milliseconds since the epoch.
     * <p>
     * It can be injected in tests using {@link #setCurrentTimeForTest(long)}.
     */
    CARAPI_(Int64) GetCurrentTimeMillis();

    /** Sets the call count and date. */
    CARAPI_(void) SetCallCountAndDate(
        /* [in] */ IPhoneCallDetailsViews* views,
        /* [in] */ IInteger32* callCount,
        /* [in] */ ICharSequence* dateText);

private:
    /** The maximum number of icons will be shown to represent the call types in a group. */
    static const Int32 MAX_CALL_TYPE_ICONS; // = 3;

    AutoPtr<IResources> mResources;
    /** The injected current time in milliseconds since the epoch. Used only by tests. */
    AutoPtr<IInteger64> mCurrentTimeMillisForTest;
    // Helper classes.
    AutoPtr<IPhoneNumberDisplayHelper> mPhoneNumberHelper;
    AutoPtr<IPhoneNumberUtilsWrapper> mPhoneNumberUtilsWrapper;

    /**
     * List of items to be concatenated together for accessibility descriptions
     */
    AutoPtr<IArrayList> mDescriptionItems; // = Lists.newArrayList();
};

} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_PHONECALLDETAILSHELPER_H__
