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

#ifndef  __ELASTOS_DROID_LAUNCHER2_LAUNCHERANIMATORUPDATELISTENER_H__
#define  __ELASTOS_DROID_LAUNCHER2_LAUNCHERANIMATORUPDATELISTENER_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Animation.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class LauncherAnimatorUpdateListener
    : public Object
    , public IAnimatorUpdateListener
{
public:
    CAR_INTERFACE_DECL();

    CARAPI OnAnimationUpdate(
        /* [in] */ IValueAnimator* animation);

    virtual CARAPI OnAnimationUpdate(
        /* [in] */ Float a,
        /* [in] */ Float b) = 0;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_LAUNCHERANIMATORUPDATELISTENER_H__