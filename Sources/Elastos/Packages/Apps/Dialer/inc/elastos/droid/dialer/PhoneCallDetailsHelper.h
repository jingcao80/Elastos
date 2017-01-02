//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_DIALER_PHONECALLDETAILSHELPER_H__
#define __ELASTOS_DROID_DIALER_PHONECALLDETAILSHELPER_H__

#include "elastos/droid/dialer/PhoneCallDetails.h"
#include "elastos/droid/dialer/PhoneCallDetailsViews.h"
#include "elastos/droid/dialer/calllog/PhoneNumberUtilsWrapper.h"
#include "elastos/droid/dialer/calllog/PhoneNumberDisplayHelper.h"
#include "elastos/droid/dialer/calllog/CallTypeHelper.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Dialer::CallLog::CallTypeHelper;
using Elastos::Droid::Dialer::CallLog::PhoneNumberUtilsWrapper;
using Elastos::Droid::Dialer::CallLog::PhoneNumberDisplayHelper;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Dialer {

/**
 * Helper class to fill in the views in {@link PhoneCallDetailsViews}.
 */
class PhoneCallDetailsHelper : public Object
{
public:

    /**
     * Creates a new instance of the helper.
     * <p>
     * Generally you should have a single instance of this helper in any context.
     *
     * @param resources used to look up strings
     */
    PhoneCallDetailsHelper(
        /* [in] */ IResources* resources,
        /* [in] */ CallTypeHelper* callTypeHelper,
        /* [in] */ PhoneNumberUtilsWrapper* phoneUtils);

    /** Fills the call details views with content. */
    CARAPI_(void) SetPhoneCallDetails(
        /* [in] */ PhoneCallDetailsViews* views,
        /* [in] */ PhoneCallDetails* details);

    /**
     * For a call, if there is an associated contact for the caller, return the known call type
     * (e.g. mobile, home, work).  If there is no associated contact, attempt to use the caller's
     * location if known.
     * @param details Call details to use.
     * @return Type of call (mobile/home) if known, or the location of the caller (if known).
     */
    CARAPI_(AutoPtr<ICharSequence>) GetCallTypeOrLocation(
        /* [in] */ PhoneCallDetails* details);

    /**
     * Get the call date/time of the call, relative to the current time.
     * e.g. 3 minutes ago
     * @param details Call details to use.
     * @return String representing when the call occurred.
     */
    CARAPI_(AutoPtr<ICharSequence>) GetCallDate(
        /* [in] */ PhoneCallDetails* details);

    /** Sets the text of the header view for the details page of a phone call. */
    // @NeededForTesting
    CARAPI_(void) SetCallDetailsHeader(
        /* [in] */ ITextView* nameView,
        /* [in] */ PhoneCallDetails* details);

    // @NeededForTesting
    CARAPI_(void) SetCurrentTimeForTest(
        /* [in] */ Int64 currentTimeMillis);

private:
    /**
     * Builds a string containing the call location and date.
     *
     * @param details The call details.
     * @return The call location and date string.
     */
    CARAPI_(AutoPtr<ICharSequence>) GetCallLocationAndDate(
        /* [in] */ PhoneCallDetails* details);

    /**
     * Returns the current time in milliseconds since the epoch.
     * <p>
     * It can be injected in tests using {@link #setCurrentTimeForTest(long)}.
     */
    CARAPI_(Int64) GetCurrentTimeMillis();

    /** Sets the call count and date. */
    CARAPI_(void) SetCallCountAndDate(
        /* [in] */ PhoneCallDetailsViews* views,
        /* [in] */ IInteger32* callCount,
        /* [in] */ ICharSequence* dateText);

private:
    /** The maximum number of icons will be shown to represent the call types in a group. */
    static const Int32 MAX_CALL_TYPE_ICONS = 3;

    AutoPtr<IResources> mResources;
    /** The injected current time in milliseconds since the epoch. Used only by tests. */
    Int64 mCurrentTimeMillisForTest;
    // Helper classes.
    AutoPtr<PhoneNumberDisplayHelper> mPhoneNumberHelper;
    AutoPtr<PhoneNumberUtilsWrapper> mPhoneNumberUtilsWrapper;

    /**
     * List of items to be concatenated together for accessibility descriptions
     */
    AutoPtr<IArrayList> mDescriptionItems;
};

} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_PHONECALLDETAILSHELPER_H__
