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

#ifndef  __ELASTOS_DROID_SYSTEMUI_CDESSERTCASEDREAM_H__
#define  __ELASTOS_DROID_SYSTEMUI_CDESSERTCASEDREAM_H__

#include "_Elastos_Droid_SystemUI_CDessertCaseDream.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/droid/service/dreams/DreamService.h>
#include <elastos/core/Runnable.h>

using Elastos::Core::Runnable;
using Elastos::Droid::Service::Dreams::DreamService;
using Elastos::Droid::Widget::IFrameLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {

CarClass(CDessertCaseDream)
    , public DreamService
    , public IDessertCaseDream
{
private:
    class Runnable_1
        : public Runnable
    {
    public:
        Runnable_1(
            /* [in] */ CDessertCaseDream* owner);

        CARAPI Run();

    public:
        CDessertCaseDream* mOwner;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI OnAttachedToWindow();

    CARAPI OnDreamingStarted();

    CARAPI OnDreamingStopped();

private:
    AutoPtr<IDessertCaseView> mView;
    AutoPtr</*IDessertCaseViewRescalingContainer*/IFrameLayout> mContainer;
};

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_CDESSERTCASEDREAM_H__
