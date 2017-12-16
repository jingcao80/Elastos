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

#ifndef __ELASTOS_DROID_APP_IINSTRUMENTATIONWATCHER_H__
#define __ELASTOS_DROID_APP_IINSTRUMENTATIONWATCHER_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>
#include <binder/Binder.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Os::IBundle;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace App {

class IInstrumentationWatcherProxy
    : public Object
    , public IInstrumentationWatcher
{
public:
    CAR_INTERFACE_DECL();

    IInstrumentationWatcherProxy(
        /* [in] */ const android::sp<android::IBinder>& bpBinder);

    CARAPI InstrumentationStatus(
        /* [in] */ IComponentName* name,
        /* [in] */ Int32 resultCode,
        /* [in] */ IBundle* results);

    CARAPI InstrumentationFinished(
        /* [in] */ IComponentName* name,
        /* [in] */ Int32 resultCode,
        /* [in] */ IBundle* results);

private:
    const android::sp<android::IBinder> mBpBinder;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_IINSTRUMENTATIONWATCHER_H__
