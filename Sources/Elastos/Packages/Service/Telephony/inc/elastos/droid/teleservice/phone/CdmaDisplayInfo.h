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

#ifndef  __ELASTOS_DROID_PHONE_CDMADISPLAYINFO_H__
#define  __ELASTOS_DROID_PHONE_CDMADISPLAYINFO_H__

#include "_Elastos.Droid.TeleService.h"
#include "Elastos.Droid.App.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * Helper class for displaying the DisplayInfo sent by CDMA network.
 */
class CdmaDisplayInfo
    : public Object
{
public:
    TO_STRING_IMPL("CdmaDisplayInfo")

    /**
     * Handle the DisplayInfo record and display the alert dialog with
     * the network message.
     *
     * @param context context to get strings.
     * @param infoMsg Text message from Network.
     */
    static void DisplayInfoRecord(
        /* [in] */ IContext* context,
        /* [in] */ const String& infoMsg);

    /**
     * Dismiss the DisplayInfo record
     */
    static CARAPI_(void) DismissDisplayInfoRecord();

private:
    static CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    static const String TAG;
    static Boolean DBG;

    /** CDMA DisplayInfo dialog */
    static AutoPtr<IAlertDialog> sDisplayInfoDialog;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CDMADISPLAYINFO_H__