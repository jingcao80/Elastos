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

#ifndef  __ELASTOS_DROID_LAUNCHER2_SPRINGLOADEDDRAGCONTROLLER_H__
#define  __ELASTOS_DROID_LAUNCHER2_SPRINGLOADEDDRAGCONTROLLER_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class SpringLoadedDragController
    : public Object
{
private:
    class InnerListener
        : public Object
        , public IAlarmOnAlarmListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ SpringLoadedDragController* host);

        CARAPI OnAlarm(
            /* [in] */ IAlarm* alarm);

    private:
        SpringLoadedDragController* mHost;
    };

public:

    SpringLoadedDragController(
        /* [in] */ ILauncher* launcher);

    CARAPI Cancel();

    // Set a new alarm to expire for the screen that we are hovering over now
    CARAPI SetAlarm(
        /* [in] */ ICellLayout* cl);

    // this is called when our timer runs out
    virtual CARAPI OnAlarm(
        /* [in] */ IAlarm* alarm);

public:
    // how long the user must hover over a mini-screen before it unshrinks
    const Int64 ENTER_SPRING_LOAD_HOVER_TIME;
    const Int64 ENTER_SPRING_LOAD_CANCEL_HOVER_TIME;
    const Int64 EXIT_SPRING_LOAD_HOVER_TIME;

    AutoPtr<IAlarm> mAlarm;

private:
    // the screen the user is currently hovering over, if any
    AutoPtr<ICellLayout> mScreen;
    ILauncher* mLauncher;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_SPRINGLOADEDDRAGCONTROLLER_H__