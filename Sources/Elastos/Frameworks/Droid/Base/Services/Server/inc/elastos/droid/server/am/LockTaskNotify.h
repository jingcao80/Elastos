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

#ifndef __ELASTOS_DROID_SERVER_AM_LOCKTASKNOTIFY_H__
#define __ELASTOS_DROID_SERVER_AM_LOCKTASKNOTIFY_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/droid/os/Handler.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::View::IWindowManagerPolicy;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::Widget::IToast;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class ActivityRecord;

class LockTaskNotify : public Object
{
private:
    class H : public Handler
    {
    public:
        TO_STRING_IMPL("LockTaskNotify::H")

        H(
            /* [in] */ LockTaskNotify* host)
            : mHost(host)
        {}

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        static const Int32 SHOW_TOAST = 3;

    private:
        LockTaskNotify* mHost;
    };

public:
    LockTaskNotify(
        /* [in] */ IContext* context);

    CARAPI_(void) ShowToast(
        /* [in] */ Boolean isLocked);

    CARAPI_(void) HandleShowToast(
        /* [in] */ Boolean isLocked);

    CARAPI_(void) Show(
        /* [in] */ Boolean starting);

private:
    static const String TAG;

    AutoPtr<IContext> mContext;
    AutoPtr<H> mHandler;
    AutoPtr<IWindowManagerPolicy> mPolicy;
    AutoPtr<IAccessibilityManager> mAccessibilityManager;
    AutoPtr<IToast> mLastToast;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_LOCKTASKNOTIFY_H__
