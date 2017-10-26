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

#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDPINBASEDINPUTVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDPINBASEDINPUTVIEW_H__

#include "_Elastos.Droid.SystemUI.h"
#include "elastos/droid/systemui/keyguard/KeyguardAbsKeyInputView.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

/**
 * A Pin based Keyguard input view
 */
class KeyguardPinBasedInputView
    : public KeyguardAbsKeyInputView
{
private:
    class PasswordListener
        : public Object
        , public IViewOnClickListener
        , public IViewOnKeyListener
    {
    public:
        TO_STRING_IMPL("KeyguardPinBasedInputView::PasswordListener")

        CAR_INTERFACE_DECL()

        PasswordListener(
            /* [in] */ KeyguardPinBasedInputView* host)
            : mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IView* v);

        CARAPI OnKey(
            /* [in] */ IView* v,
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

    private:
        KeyguardPinBasedInputView* mHost;
    };

    class MyOnClickListener2
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("KeyguardPinBasedInputView::MyOnClickListener2")

        CAR_INTERFACE_DECL()

        MyOnClickListener2(
            /* [in] */ KeyguardPinBasedInputView* host)
            : mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        KeyguardPinBasedInputView* mHost;
    };

    class MyOnClickListener3
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("KeyguardPinBasedInputView::MyOnClickListener3")

        CAR_INTERFACE_DECL()

        MyOnClickListener3(
            /* [in] */ KeyguardPinBasedInputView* host)
            : mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        KeyguardPinBasedInputView* mHost;
    };

    class MyViewOnLongClickListener
        : public Object
        , public IViewOnLongClickListener
    {
    public:
        TO_STRING_IMPL("KeyguardPinBasedInputView::MyViewOnLongClickListener")

        CAR_INTERFACE_DECL()

        MyViewOnLongClickListener(
            /* [in] */ KeyguardPinBasedInputView* host)
            : mHost(host)
        {}

        CARAPI OnLongClick(
            /* [in] */ IView* v,
            /* [out] */ Boolean* result);

    private:
        KeyguardPinBasedInputView* mHost;
    };

public:
    TO_STRING_IMPL("KeyguardPinBasedInputView")

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    //@Override
    CARAPI Reset();

    //@Override
    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    virtual CARAPI OnKey(
        /* [in] */ IView* v,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

protected:
    //@Override
    CARAPI OnRequestFocusInDescendants(
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect,
        /* [out] */ Boolean* result);

    CARAPI ResetState();

    //@Override
    CARAPI SetPasswordEntryEnabled(
        /* [in] */ Boolean enabled);

    //@Override
    CARAPI ResetPasswordText(
        /* [in] */ Boolean animate);

    //@Override
    CARAPI GetPasswordText(
        /* [out] */ String* text);

    //@Override
    CARAPI OnFinishInflate();

private:
    using KeyguardAbsKeyInputView::PerformClick;

    CARAPI_(void) PerformClick(
        /* [in] */ IView* view);

    CARAPI_(void) PerformNumberClick(
        /* [in] */ Int32 number);

protected:
    AutoPtr<IPasswordTextView> mPasswordEntry;

private:
    AutoPtr<IView> mOkButton;
    AutoPtr<IView> mDeleteButton;
    AutoPtr<IView> mButton0;
    AutoPtr<IView> mButton1;
    AutoPtr<IView> mButton2;
    AutoPtr<IView> mButton3;
    AutoPtr<IView> mButton4;
    AutoPtr<IView> mButton5;
    AutoPtr<IView> mButton6;
    AutoPtr<IView> mButton7;
    AutoPtr<IView> mButton8;
    AutoPtr<IView> mButton9;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDPINBASEDINPUTVIEW_H__