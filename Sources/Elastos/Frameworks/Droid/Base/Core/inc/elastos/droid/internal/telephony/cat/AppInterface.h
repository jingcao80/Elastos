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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_APPINTERFACE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_APPINTERFACE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// package com.android.internal.telephony.cat;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

/**
  * Interface for communication between STK App and CAT Telephony
  *
  * {@hide}
  */
class AppInterface
{
public:
    /*
      * Callback function from app to telephony to pass a result code and user's
      * input back to the ICC.
      */
    virtual CARAPI OnCmdResponse(
        /* [in] */ CatResponseMessage* resMsg) = 0;

public:
    /*
      * Intent's actions which are broadcasted by the Telephony once a new CAT
      * proactive command, session end, ALPHA during STK CC arrive.
      */
    static const String CAT_CMD_ACTION;
    static const String CAT_SESSION_END_ACTION;
    static const String CAT_ALPHA_NOTIFY_ACTION;
    //This is used to send ALPHA string from card to STK App.
    static const String ALPHA_STRING;
    // This is used to send refresh-result when MSG_ID_ICC_REFRESH is received.
    static const String REFRESH_RESULT;
    //This is used to send card status from card to STK App.
    static const String CARD_STATUS;
    //Intent's actions are broadcasted by Telephony once IccRefresh occurs.
    static const String CAT_ICC_STATUS_CHANGE;
    // Permission required by STK command receiver
    static const String STK_PERMISSION;
    /*
      * This is broadcast from the ActivityManagerService when the screen
      * switches to idle or busy state
      */
    static const String CAT_IDLE_SCREEN_ACTION;
    /*
      * This is broadcast from the Stk Apps to ActivityManagerService when the screen
      * status is requested.
      */
    static const String CHECK_SCREEN_IDLE_ACTION;
    /*
      * Intent action broadcasted by StkAppService when the ACTIVATE proactive command
      * arrives.
      */
    static const String CAT_ACTIVATE_NOTIFY_ACTION;
    /*
      * Intent action broadcasted by NfcService when the HCI Connectivity event occurs.
      */
    static const String CAT_HCI_CONNECTIVITY_ACTION;
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_APPINTERFACE_H__

