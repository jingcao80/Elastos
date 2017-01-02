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

#ifndef __ELASTOS_DROID_WIDGET_MEDIACONTROLLER_H__
#define __ELASTOS_DROID_WIDGET_MEDIACONTROLLER_H__

#include "elastos/droid/widget/FrameLayout.h"
#include "elastos/droid/os/Handler.h"

#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::IViewOnLayoutChangeListener;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::IImageButton;
using Elastos::Droid::Widget::ISeekBar;
using Elastos::Droid::Widget::IMediaPlayerControl;

using Elastos::Core::StringBuilder;
using Elastos::Utility::IFormatter;

namespace Elastos {
namespace Droid {
namespace Widget {

class ECO_PUBLIC MediaController
    : public FrameLayout
    , public IMediaController
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("MediaController::MyHandler")

        MyHandler(
            /* [in] */ MediaController* host)
            : mHost(host)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        MediaController* mHost;
    };

    class MediaControllerOnTouchListener
        : public Object
        , public IViewOnTouchListener
    {
    public:
        CAR_INTERFACE_DECL()

        MediaControllerOnTouchListener(
            /* [in] */ MediaController* host);

        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* res);

    private:
        MediaController* mHost;
    };

    class MediaControllerOnLayoutChangeListener
        : public Object
        , public IViewOnLayoutChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        MediaControllerOnLayoutChangeListener(
            /* [in] */ MediaController* host);

        CARAPI OnLayoutChange(
            /* [in] */ IView* v,
            /* [in] */ Int32 left,
            /* [in] */ Int32 top,
            /* [in] */ Int32 right,
            /* [in] */ Int32 bottom,
            /* [in] */ Int32 oldLeft,
            /* [in] */ Int32 oldTop,
            /* [in] */ Int32 oldRight,
            /* [in] */ Int32 oldBottom);

    private:
        MediaController* mHost;
    };

    class PauseOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        PauseOnClickListener(
            /* [in] */ MediaController* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        MediaController* mHost;
    };

    class FfwdOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        FfwdOnClickListener(
            /* [in] */ MediaController* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        MediaController* mHost;
    };

    class RewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        RewOnClickListener(
            /* [in] */ MediaController* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        MediaController* mHost;
    };

    class MediaControllerOnSeekBarChangeListener
        : public Object
        , public ISeekBarOnSeekBarChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        MediaControllerOnSeekBarChangeListener(
            /* [in] */ MediaController* host);

        CARAPI OnStartTrackingTouch(
            /* [in] */ ISeekBar* seekBar);

        CARAPI OnProgressChanged(
            /* [in] */ ISeekBar* seekBar,
            /* [in] */ Int32 progress,
            /* [in] */ Boolean fromUser);

        CARAPI OnStopTrackingTouch(
            /* [in] */ ISeekBar* seekBar);

    private:
        MediaController* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    MediaController();

    ~MediaController();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Boolean useFastForWard);

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI OnFinishInflate();

    CARAPI SetMediaPlayer(
        /* [in] */ IMediaPlayerControl* player);

    CARAPI SetAnchorView(
        /* [in] */ IView* view);

    CARAPI Show();

    CARAPI Show(
        /* [in] */ Int32 timeout);

    CARAPI IsShowing(
        /* [out] */ Boolean* isShowing);

    CARAPI Hide();

    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnTrackballEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI DispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    CARAPI SetPrevNextListeners(
        /* [in] */ IViewOnClickListener* next,
        /* [in] */ IViewOnClickListener* prev);

protected:
    CARAPI_(AutoPtr<IView>) MakeControllerView();

private:
    CARAPI_(void) InitFloatingWindow();

    CARAPI_(void) InitFloatingWindowLayout();

    CARAPI_(void) UpdateFloatingWindowLayout();

    CARAPI_(void) InitControllerView(
        /* [in] */ IView* view);

    CARAPI_(void) DisableUnsupportedButtons();

    CARAPI_(String) StringForTime(
        /* [in] */ Int32 timeMs);

    CARAPI_(Int32) SetProgress();

    CARAPI_(void) UpdatePausePlay();

    CARAPI_(void) DoPauseResume();

    CARAPI_(void) InstallPrevNextListeners();

    CARAPI HandleShowProgress();

private:
    AutoPtr<IMediaPlayerControl> mPlayer;
    AutoPtr<IContext> mContext;
    AutoPtr<IView> mAnchor;
    AutoPtr<IView> mRoot;
    AutoPtr<IWindowManager> mWindowManager;
    AutoPtr<IWindow> mWindow;
    AutoPtr<IView> mDecor;
    AutoPtr<IWindowManagerLayoutParams> mDecorLayoutParams;

    AutoPtr<IProgressBar> mProgress;
    AutoPtr<ITextView> mEndTime;
    AutoPtr<ITextView> mCurrentTime;
    Boolean mShowing;
    Boolean mDragging;

    static const Int32 sDefaultTimeout;
    static const Int32 FADE_OUT;
    static const Int32 SHOW_PROGRESS;
    static const String MEDIACONTROLLER_NAME;

    Boolean mUseFastForward;
    Boolean mFromXml;
    Boolean mListenersSet;
    AutoPtr<IViewOnClickListener> mNextListener;
    AutoPtr<IViewOnClickListener> mPrevListener;
    AutoPtr<StringBuilder> mFormatBuilder;
    AutoPtr<IFormatter> mFormatter;

    AutoPtr<IImageButton> mPauseButton;
    AutoPtr<IImageButton> mFfwdButton;
    AutoPtr<IImageButton> mRewButton;
    AutoPtr<IImageButton> mNextButton;
    AutoPtr<IImageButton> mPrevButton;
    AutoPtr<ICharSequence> mPlayDescription;
    AutoPtr<ICharSequence> mPauseDescription;
    AutoPtr<IViewOnLayoutChangeListener> mLayoutChangeListener;
    AutoPtr<IViewOnTouchListener> mTouchListener;
    AutoPtr<IHandler> mHandler;
    AutoPtr<IViewOnClickListener> mPauseListener;
    AutoPtr<ISeekBarOnSeekBarChangeListener> mSeekListener;
    AutoPtr<IViewOnClickListener> mRewListener;
    AutoPtr<IViewOnClickListener> mFfwdListener;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_MEDIACONTROLLER_H__
