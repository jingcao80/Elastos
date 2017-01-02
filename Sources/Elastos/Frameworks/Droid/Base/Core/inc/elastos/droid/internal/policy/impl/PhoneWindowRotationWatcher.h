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

#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_PHONEWINDOWROTATIONWATCHER_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_PHONEWINDOWROTATIONWATCHER_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::IRotationWatcher;
using Elastos::Core::IRunnable;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

class PhoneWindowRotationWatcher
    : public Object
    , public IBinder
    , public IPhoneWindowRotationWatcher
    , public IRotationWatcher
{
private:
    class InnerRunnable1
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerRunnable1(
            /* [in] */ PhoneWindowRotationWatcher* owner);

        CARAPI Run();

    private:
        PhoneWindowRotationWatcher* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    PhoneWindowRotationWatcher();

    CARAPI constructor();

    // @Override
    CARAPI OnRotationChanged(
        /* [in] */ Int32 rotation);

    virtual CARAPI AddWindow(
        /* [in] */ IPhoneWindow* phoneWindow);

    virtual CARAPI RemoveWindow(
        /* [in] */ IPhoneWindow* phoneWindow);

    virtual CARAPI DispatchRotationChanged();

    CARAPI ToString(
        /* [out] */ String* info);
private:
    AutoPtr<IHandler> mHandler;
    /*const*/ AutoPtr<IRunnable> mRotationChanged;

    // private final ArrayList<WeakReference<PhoneWindow>> mWindows =
    //         new ArrayList<WeakReference<PhoneWindow>>();
    //store IPhoneWindow
    AutoPtr<IArrayList> mWindows;
    Boolean mIsWatching;
    IPhoneWindow* mOwner;
};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_PHONEWINDOWROTATIONWATCHER_H__
