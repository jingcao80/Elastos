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

#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CEMERGENCYCARRIERAREA_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CEMERGENCYCARRIERAREA_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CEmergencyCarrierArea.h"
#include "elastos/droid/systemui/keyguard/AlphaOptimizedLinearLayout.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CEmergencyCarrierArea)
    , public AlphaOptimizedLinearLayout
    , public IEmergencyCarrierArea
{
private:
    class MyOnClickListener
        : public Object
        , public IViewOnTouchListener
    {
    public:
        TO_STRING_IMPL("CEmergencyCarrierArea::MyOnClickListener")

        CAR_INTERFACE_DECL()

        MyOnClickListener(
            /* [in] */ CEmergencyCarrierArea* host)
            : mHost(host)
        {}

        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

    private:
        CEmergencyCarrierArea* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetCarrierTextVisible(
        /* [in] */ Boolean visible);

protected:
    //@Override
    CARAPI OnFinishInflate();

private:
    AutoPtr<ICarrierText> mCarrierText;
    AutoPtr<IEmergencyButton> mEmergencyButton;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CEMERGENCYCARRIERAREA_H__
