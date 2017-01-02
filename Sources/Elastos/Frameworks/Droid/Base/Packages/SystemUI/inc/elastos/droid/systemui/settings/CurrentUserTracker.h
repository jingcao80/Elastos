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

#ifndef __ELASTOS_DROID_SYSTEMUI_SETTINGS_CURRENTUSERTRACKER_H__
#define __ELASTOS_DROID_SYSTEMUI_SETTINGS_CURRENTUSERTRACKER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Settings {

class CurrentUserTracker
    : public BroadcastReceiver
{
public:

    TO_STRING_IMPL("CurrentUserTracker")

    CurrentUserTracker(
        /* [in] */ IContext* context);

    CARAPI_(Int32) GetCurrentUserId();

    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    CARAPI_(void) StartTracking();

    CARAPI_(void) StopTracking();

    virtual CARAPI_(void) OnUserSwitched(
        /* [in] */ Int32 newUserId) = 0;

    CARAPI_(Boolean) IsCurrentUserOwner();

private:
    AutoPtr<IContext> mContext;
    Int32 mCurrentUserId;
};

} // namespace Settings
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_SETTINGS_CURRENTUSERTRACKER_H__