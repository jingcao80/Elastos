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

#ifndef __ELASTOS_DROID_VIEW_CWINDOWMANAGERGLOBAL_SESSION_CALLBACK_H__
#define __ELASTOS_DROID_VIEW_CWINDOWMANAGERGLOBAL_SESSION_CALLBACK_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_View_CWindowManagerGlobalSessionCallback.h"
#include <elastos/core/Object.h>
#include <binder/Binder.h>

using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CWindowManagerGlobalSessionCallback)
    , public Object
    , public IIWindowSessionCallback
    , public IBinder
{
public:

    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CWindowManagerGlobalSessionCallback();

    CARAPI constructor();

    CARAPI OnAnimatorScaleChanged(
        /* [in] */ Float scale);

    CARAPI ToString(
        /* [out] */ String* info);

public:
    android::IBinder* mBBinder;
};

} // namespace View
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CWINDOWMANAGERGLOBAL_SESSION_CALLBACK_H__
