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

#ifndef __ELASTOS_DROID_SERVER_WM_KEYGURADDISABLEHANDLER_H__
#define __ELASTOS_DROID_SERVER_WM_KEYGURADDISABLEHANDLER_H__

#include "_Elastos.Droid.Server.h"
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.View.h>
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/TokenWatcher.h"

using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::TokenWatcher;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IWindowManagerPolicy;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

class KeyguardDisableHandler : public Handler
{
public:
    class KeyguardTokenWatcher : public TokenWatcher
    {
    public:
        TO_STRING_IMPL("KeyguardDisableHandler::KeyguardTokenWatcher")

        KeyguardTokenWatcher(
            /* [in] */ KeyguardDisableHandler* handler);

        CARAPI Acquired();

        CARAPI Released();

    private:
        KeyguardDisableHandler* mHost;
    };

public:
    TO_STRING_IMPL("KeyguardDisableHandler")

    KeyguardDisableHandler(
        /* [in] */ IContext* context,
        /* [in] */ IWindowManagerPolicy* policy);

    virtual CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

public:
    // Message.what constants
    static const Int32 KEYGUARD_DISABLE = 1;
    static const Int32 KEYGUARD_REENABLE = 2;
    static const Int32 KEYGUARD_POLICY_CHANGED = 3;

    AutoPtr<IContext> mContext;
    AutoPtr<IWindowManagerPolicy> mPolicy;
    AutoPtr<KeyguardTokenWatcher> mKeyguardTokenWatcher;

private:
    static const String TAG;

    static const Int32 ALLOW_DISABLE_YES = 1;
    static const Int32 ALLOW_DISABLE_NO = 0;
    static const Int32 ALLOW_DISABLE_UNKNOWN = -1; // check with DevicePolicyManager
    Int32 mAllowDisableKeyguard; // sync'd by mKeyguardTokenWatcher
};

} // Wm
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_WM_KEYGURADDISABLEHANDLER_H__
