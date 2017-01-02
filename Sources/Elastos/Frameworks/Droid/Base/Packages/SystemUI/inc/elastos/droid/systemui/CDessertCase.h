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

#ifndef  __ELASTOS_DROID_SYSTEMUI_CDESSERTCASE_H__
#define  __ELASTOS_DROID_SYSTEMUI_CDESSERTCASE_H__

#include "_Elastos_Droid_SystemUI_CDessertCase.h"
#include <elastos/droid/app/Activity.h>
#include <elastos/core/Runnable.h>

using Elastos::Droid::App::Activity;
using Elastos::Core::Runnable;
using Elastos::Droid::SystemUI::IDessertCaseView;

namespace Elastos {
namespace Droid {
namespace SystemUI {

CarClass(CDessertCase)
    , public Activity
    , public IDessertCase
{
private:
    class Runnable_1
        : public Runnable
    {
    public:
        Runnable_1(
            /* [in] */ CDessertCase* owner);

        CARAPI Run();

    public:
        CDessertCase* mOwner;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

public:
    AutoPtr<IDessertCaseView> mView;
};

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_CDESSERTCASE_H__
