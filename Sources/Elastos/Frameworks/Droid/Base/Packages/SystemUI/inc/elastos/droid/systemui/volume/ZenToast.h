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

#ifndef __ELASTOS_DROID_SYSTEMUI_VOLUME_ZENTOAST_H__
#define __ELASTOS_DROID_SYSTEMUI_VOLUME_ZENTOAST_H__

#include "_Elastos.Droid.SystemUI.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Handler.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnAttachStateChangeListener;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Volume {

class ZenToast
    : public Object
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("ZenToast::MyHandler")

        MyHandler(
            /* [in] */ ZenToast* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        ZenToast* mHost;
    };

    class MyReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("ZenToast::MyReceiver")

        MyReceiver(
            /* [in] */ ZenToast* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        ZenToast* mHost;
    };

    class OnAttachStateChangeListener
        : public Object
        , public IViewOnAttachStateChangeListener
    {
    public:
        TO_STRING_IMPL("ZenToast::OnAttachStateChangeListener")

        CAR_INTERFACE_DECL()

        OnAttachStateChangeListener(
            /* [in] */ ITextView* message,
            /* [in] */ ZenToast* host);

        CARAPI OnViewAttachedToWindow(
            /* [in] */ IView* v);

        CARAPI OnViewDetachedFromWindow(
            /* [in] */ IView* v);

    private:
        AutoPtr<ITextView> mMessage;
        ZenToast* mHost;
    };

public:
    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI Show(
        /* [in] */ Int32 zen);

    CARAPI Hide();

private:
    CARAPI_(void) HandleShow(
        /* [in] */ Int32 zen,
        /* [in] */ String overrideText);

    CARAPI_(void) HandleHide();

private:
    const static String ACTION_SHOW;
    const static String ACTION_HIDE;
    const static String EXTRA_ZEN;
    const static String EXTRA_TEXT;

    const static Int32 MSG_SHOW = 1;
    const static Int32 MSG_HIDE = 2;

    AutoPtr<IContext> mContext;
    AutoPtr<IWindowManager> mWindowManager;

    AutoPtr<IView> mZenToast;
    AutoPtr<MyHandler> mHandler;
    AutoPtr<MyReceiver> mReceiver;
};

} // namespace Volume
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_VOLUME_ZENTOAST_H__