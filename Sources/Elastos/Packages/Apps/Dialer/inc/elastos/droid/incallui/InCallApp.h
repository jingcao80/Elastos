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

#ifndef __ELASTOS_DROID_INCALLUI_INCALLAPP_H__
#define __ELASTOS_DROID_INCALLUI_INCALLAPP_H__

#include "_Elastos.Droid.Dialer.h"
#include <elastos/droid/app/Application.h>
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::App::Application;
using Elastos::Droid::Content::BroadcastReceiver;

namespace Elastos {
namespace Droid {
namespace InCallUI {

class InCallApp
    : public Application
{
public:
    /**
     * Accepts broadcatst Intents which will be prepared by {@link StatusBarNotifier} and thus
     * sent from framework's notification mechanism (which is outside Phone context).
     * This should be visible from outside, but shouldn't be in "exported" state.
     */
    class NotificationBroadcastReceiver : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("InCallApp::NotificationBroadcastReceiver")

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);
    };

public:
    //@Override
    CARAPI OnCreate();

    //@Override
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

public:
    /**
     * Intent Action used for hanging up the current call from Notification bar. This will
     * choose first ringing call, first active call, or first background call (typically in
     * STATE_HOLDING state).
     */
    static const String ACTION_DECLINE_INCOMING_CALL;
    static const String ACTION_HANG_UP_ONGOING_CALL;
    static const String ACTION_ANSWER_VIDEO_INCOMING_CALL;
    static const String ACTION_ANSWER_VOICE_INCOMING_CALL;
    static const String ACTION_ACCEPT_VIDEO_UPGRADE_REQUEST;
    static const String ACTION_DECLINE_VIDEO_UPGRADE_REQUEST;
};

} // InCallUI
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_INCALLUI_INCALLAPP_H__
