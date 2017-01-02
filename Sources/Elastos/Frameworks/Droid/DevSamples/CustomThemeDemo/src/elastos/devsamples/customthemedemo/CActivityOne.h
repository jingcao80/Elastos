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

#ifndef __Elastos_DevSamples_CustomThemeDemo_CActivityOne_H__
#define __Elastos_DevSamples_CustomThemeDemo_CActivityOne_H__

#include "_Elastos_DevSamples_CustomThemeDemo_CActivityOne.h"
#include <elastos/droid/app/Activity.h>
#include <Elastos.Droid.Content.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::View::IViewOnClickListener;

namespace Elastos {
namespace DevSamples {
namespace CustomThemeDemo {

CarClass(CActivityOne)
    , public Activity
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

protected:

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

private:
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);
};

} // namespace CustomThemeDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_CustomThemeDemo_CActivityOne_H__
