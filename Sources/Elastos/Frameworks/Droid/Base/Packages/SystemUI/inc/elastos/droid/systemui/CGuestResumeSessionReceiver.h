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

#ifndef  __ELASTOS_DROID_SYSTEMUI_CGUESTRESUMESESSIONRECEIVER_H__
#define  __ELASTOS_DROID_SYSTEMUI_CGUESTRESUMESESSIONRECEIVER_H__

#include "_Elastos_Droid_SystemUI_CGuestResumeSessionReceiver.h"
#include "Elastos.Droid.App.h"
#include <elastos/droid/content/BroadcastReceiver.h>

using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::BroadcastReceiver;

namespace Elastos {
namespace Droid {
namespace SystemUI {

/**
 * Manages notification when a guest session is resumed.
 */
CarClass(CGuestResumeSessionReceiver)
    , public BroadcastReceiver
    , public IGuestResumeSessionReceiver
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Register(
        /* [in] */ IContext* context);

    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    /**
     * Wipes the guest session.
     *
     * The guest must be the current user and its id must be {@param userId}.
     */
    static CARAPI_(void) WipeGuestSession(
        /* [in] */ IContext* context,
        /* [in] */ Int32 userId);

    CARAPI_(void) CancelDialog();

private:
    friend class CResetSessionDialog;
    static const String TAG;
    static const String SETTING_GUEST_HAS_LOGGED_IN;

    AutoPtr<IDialog> mNewSessionDialog;
};

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_CGUESTRESUMESESSIONRECEIVER_H__
