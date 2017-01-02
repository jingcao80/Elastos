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

#ifndef  __ELASTOS_DROID_PHONE_CGETPIN2SCREEN_H__
#define  __ELASTOS_DROID_PHONE_CGETPIN2SCREEN_H__

#include "_Elastos_Droid_TeleService_Phone_CGetPin2Screen.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/Activity.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::IOnEditorActionListener;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * Pin2 entry screen.
 */
CarClass(CGetPin2Screen)
    , public Activity
{
private:
    class InnerListener
        : public Object
        , public IViewOnClickListener
        , public IOnEditorActionListener
    {
    public:
        TO_STRING_IMPL("CGetPin2Screen::InnerListener")

        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ CGetPin2Screen* host)
            : mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IView* v);

        CARAPI OnEditorAction(
            /* [in] */ ITextView* v,
            /* [in] */ Int32 actionId,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);
    private:
        CGetPin2Screen* mHost;
    };

public:

    CAR_OBJECT_DECL();

    CGetPin2Screen();

    CARAPI constructor();

    //@Override
    CARAPI OnEditorAction(
        /* [in] */ ITextView* v,
        /* [in] */ Int32 actionId,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

private:
    CARAPI_(String) GetPin2();

    CARAPI_(void) ReturnResult();

    CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    static const String TAG;

    AutoPtr<IEditText> mPin2Field;
    AutoPtr<IButton> mOkButton;

    AutoPtr<InnerListener> mInnerListener;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CGETPIN2SCREEN_H__