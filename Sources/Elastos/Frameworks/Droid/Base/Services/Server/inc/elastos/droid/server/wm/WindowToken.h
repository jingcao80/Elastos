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

#ifndef __ELASTOS_DROID_SERVER_WM_WINDOWTOKEN_H__
#define __ELASTOS_DROID_SERVER_WM_WINDOWTOKEN_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;

typedef IArrayList WindowList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

class AppWindowToken;
class CWindowManagerService;

class WindowToken
    : public Object
{
public:
    WindowToken()
        : mService(NULL)
        , mWindowType(0)
        , mExplicit(FALSE)
        , mAppWindowToken(NULL)
        , mPaused(FALSE)
        , mHidden(FALSE)
        , mHasVisible(FALSE)
        , mWaitingToShow(FALSE)
        , mWaitingToHide(FALSE)
        , mSendingToBottom(FALSE)
        , mSendingToTop(FALSE)
    {}

    CARAPI constructor(
        /* [in] */ CWindowManagerService* service,
        /* [in] */ IBinder* token,
        /* [in] */ Int32 type,
        /* [in] */ Boolean isExplicit)
    {
        mService = service;
        mToken = token;
        mWindowType = type;
        mExplicit = isExplicit;
        return CArrayList::New((IArrayList**)&mWindows);
    }

    CARAPI ToString(
        /* [out] */ String* str)
    {
        VALIDATE_NOT_NULL(str)
        *str = mStringName;
        return NOERROR;
    }
public:
    // The window manager!
    CWindowManagerService* mService;

    // The actual token.
    AutoPtr<IBinder> mToken;

    // The type of window this token is for, as per WindowManager.LayoutParams.
    Int32 mWindowType;

    // Set if this token was explicitly added by a client, so should
    // not be removed when all windows are removed.
    Boolean mExplicit;

    // For printing.
    String mStringName;

    // If this is an AppWindowToken, this is non-null.
    AppWindowToken* mAppWindowToken;

    // All of the windows associated with this token.
    AutoPtr<WindowList> mWindows;

    // Is key dispatching paused for this token?
    Boolean mPaused;

    // Should this token's windows be hidden?
    Boolean mHidden;

    // Temporary for finding which tokens no longer have visible windows.
    Boolean mHasVisible;

    // Set to true when this token is in a pending transaction where it
    // will be shown.
    Boolean mWaitingToShow;

    // Set to true when this token is in a pending transaction where it
    // will be hidden.
    Boolean mWaitingToHide;

    // Set to true when this token is in a pending transaction where its
    // windows will be put to the bottom of the list.
    Boolean mSendingToBottom;

    // Set to true when this token is in a pending transaction where its
    // windows will be put to the top of the list.
    Boolean mSendingToTop;
};

} // Wm
} // Server
} // Droid
} // Elastos

#define To_WindowToken(obj) \
    ((WindowToken*)IObject::Probe(obj))

#endif //__ELASTOS_DROID_SERVER_WM_WINDOWTOKEN_H__
