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

#ifndef __ELASTOS_DROID_INCALLUI_CALLERINFOUTILS_H__
#define __ELASTOS_DROID_INCALLUI_CALLERINFOUTILS_H__

#include "elastos/droid/incallui/CallerInfo.h"
#include "elastos/droid/incallui/Call.h"

namespace Elastos {
namespace Droid {
namespace InCallUI {

/**
 * Utility methods for contact and caller info related functionality
 */
class CallerInfoUtils
{
private:
    // class MyOnLoadCompleteListener
    //     : public Object
    //     , public IOnLoadCompleteListener
    // {
    // public:
    //     CAR_INTERFACE_DECL()
    // };

public:
    /**
     * This is called to get caller info for a call. This will return a CallerInfo
     * object immediately based off information in the call, but
     * more information is returned to the OnQueryCompleteListener (which contains
     * information about the phone number label, user's name, etc).
     */
    static CARAPI_(AutoPtr<CallerInfo>) GetCallerInfoForCall(
        /* [in] */ IContext* context,
        /* [in] */ Call* call,
        /* [in] */ IOnQueryCompleteListener* listener);

    static CARAPI_(AutoPtr<CallerInfo>) BuildCallerInfo(
        /* [in] */ IContext* context,
        /* [in] */ Call* call);

    /**
     * Handles certain "corner cases" for CNAP. When we receive weird phone numbers
     * from the network to indicate different number presentations, convert them to
     * expected number and presentation values within the CallerInfo object.
     * @param number number we use to verify if we are in a corner case
     * @param presentation presentation value used to verify if we are in a corner case
     * @return the new String that should be used for the phone number
     */
    /* package */
    static CARAPI_(String) ModifyForSpecialCnapCases(
        /* [in] */ IContext* context,
        /* [in] */ CallerInfo* ci,
        /* [in] */ const String& number,
        /* [in] */ Int32 presentation);

    /**
     * Send a notification using a {@link ContactLoader} to inform the sync adapter that we are
     * viewing a particular contact, so that it can download the high-res photo.
     */
    static CARAPI_(void) SendViewNotification(
        /* [in] */ IContext* context,
        /* [in] */ IUri* contactUri);

private:
    static CARAPI_(Boolean) IsCnapSpecialCaseRestricted(
        /* [in] */ const String& n);

    static CARAPI_(Boolean) IsCnapSpecialCaseUnknown(
        /* [in] */ const String& n);

private:
    static const String TAG;

    /** Define for not a special CNAP string */
    static const Int32 CNAP_SPECIAL_CASE_NO = -1;

    static const Int32 QUERY_TOKEN = -1;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_CALLERINFOUTILS_H__
