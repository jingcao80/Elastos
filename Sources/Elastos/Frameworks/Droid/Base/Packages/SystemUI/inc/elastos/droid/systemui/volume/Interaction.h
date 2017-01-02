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

#ifndef __ELASTOS_DROID_SYSTEMUI_VOLUME_INTERACTION_H__
#define __ELASTOS_DROID_SYSTEMUI_VOLUME_INTERACTION_H__

#include "_Elastos.Droid.SystemUI.h"
#include <elastos/core/Object.h>
#include "Elastos.Droid.View.h"

using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnGenericMotionListener;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Volume {

class Interaction
    : public Object
{
private:
    class MyOnTouchListener
        : public Object
        , public IViewOnTouchListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyOnTouchListener(
            /* [in] */ IInteractionCallback* callback);

        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

    private:
        AutoPtr<IInteractionCallback> mCallback;
    };

    class MyOnGenericMotionListener
        : public Object
        , public IViewOnGenericMotionListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyOnGenericMotionListener(
            /* [in] */ IInteractionCallback* callback);

        CARAPI OnGenericMotion(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

    private:
        AutoPtr<IInteractionCallback> mCallback;
    };

public:
    static CARAPI Register(
        /* [in] */ IView* v,
        /* [in] */ IInteractionCallback* callback);
};

} // namespace Volume
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_VOLUME_INTERACTION_H__