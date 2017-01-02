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

#ifndef __ELASTOS_DROID_SERVER_WM_CWINDOWID_H__
#define __ELASTOS_DROID_SERVER_WM_CWINDOWID_H__

#include "_Elastos_Droid_Server_Wm_CWindowId.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::IIWindowId;
using Elastos::Droid::View::IIWindowFocusObserver;
using Elastos::Droid::View::IWindowState;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

CarClass(CWindowId)
    , public Object
    , public IIWindowId
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI RegisterFocusObserver(
        /* [in] */ IIWindowFocusObserver* observer);

    CARAPI UnregisterFocusObserver(
        /* [in] */ IIWindowFocusObserver* observer);

    CARAPI IsFocused(
        /* [out] */ Boolean* result);

    CARAPI constructor(
        /* [in] */ IWindowState* host);

private:
    AutoPtr<IWeakReference> mWeakOwner;
};

} // Wm
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_WM_CWindowId_H__
