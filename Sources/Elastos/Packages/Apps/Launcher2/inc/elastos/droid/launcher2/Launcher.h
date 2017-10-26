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

#ifndef  __ELASTOS_DROID_LAUNCHER2_LAUNCHER_H__
#define  __ELASTOS_DROID_LAUNCHER2_LAUNCHER_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/ItemInfo.h"
#include "elastos/droid/launcher2/HideFromAccessibilityHelper.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/os/AsyncTask.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/os/Handler.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.AppWidget.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>
#include <elastos/core/Thread.h>

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::App::Activity;
using Elastos::Droid::AppWidget::IAppWidgetHostView;
using Elastos::Droid::AppWidget::IAppWidgetManager;
using Elastos::Droid::AppWidget::IAppWidgetProviderInfo;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Graphics::Drawable::IDrawableConstantState;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IMessage;
using Elastos::Core::Runnable;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Text::ISpannableStringBuilder;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IOnDrawListener;
using Elastos::Droid::View::IOnGlobalLayoutListener;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IButton;
using Elastos::Core::Thread;
using Elastos::Core::Object;
using Elastos::Core::IComparator;
using Elastos::Core::IRunnable;
using Elastos::IO::IPrintWriter;
using Elastos::IO::IFileDescriptor;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * Default launcher application.
 */
class Launcher
    : public Activity
    , public ILauncher
    , public ILauncherModelCallbacks
{
public:
    class InnerListener
        : public Object
        , public IViewOnClickListener
        , public IViewOnLongClickListener
        , public IViewOnTouchListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ Launcher* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

        CARAPI OnLongClick(
            /* [in] */ IView* v,
            /* [out] */ Boolean* result);

    private:
        Launcher* mHost;
    };

    class PendingAddArguments
        : public Object
    {
    public:
        Int32 mRequestCode;
        AutoPtr<IIntent> mIntent;
        Int64 mContainer;
        Int32 mScreen;
        Int32 mCellX;
        Int32 mCellY;
    };

    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("Launcher::MyBroadcastReceiver")

        MyBroadcastReceiver();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ ILauncher* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        Launcher* mHost;
    };

    /**
     * Receives notifications when system dialogs are to be closed.
     */
    class CloseSystemDialogsIntentReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("Launcher::CloseSystemDialogsIntentReceiver")

        CloseSystemDialogsIntentReceiver();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ ILauncher* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        Launcher* mHost;
    };

    /**
     * Receives notifications whenever the appwidgets are reset.
     */
    class AppWidgetResetObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("Launcher::AppWidgetResetObserver")

        AppWidgetResetObserver();

        CARAPI constructor(
            /* [in] */ ILauncher* host);

        //@Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        Launcher* mHost;
    };

private:
    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("Launcher::MyRunnable")

        MyRunnable(
            /* [in] */ Launcher* host);

        CARAPI Run();

    private:
        Launcher* mHost;
    };

    class MyAsyncTask
        : public AsyncTask
    {
    public:
        TO_STRING_IMPL("Launcher::MyAsyncTask")

        MyAsyncTask(
            /* [in] */ Launcher* host);

    protected:
        //@Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

        //@Override
        CARAPI OnPostExecute(
            /* [in] */ IInterface* result);

    private:
        Launcher* mHost;
    };

    class LocaleConfiguration
        : public Object
    {
    public:
        LocaleConfiguration();

    public:
        String mLocale;
        Int32 mMcc;
        Int32 mMnc;
    };

    class MyThread
        : public Thread
    {
    public:
        TO_STRING_IMPL("Launcher::MyThread")

        MyThread(
            /* [in] */ Launcher* host,
            /* [in] */ const String& name,
            /* [in] */ LocaleConfiguration* localeConfiguration);

        CARAPI Run();

    private:
        Launcher* mHost;
        AutoPtr<LocaleConfiguration> mLocaleConfiguration;
    };

    class MyRunnable2
        : public Runnable
    {
    public:
        TO_STRING_IMPL("Launcher::MyRunnable2")

        MyRunnable2(
            /* [in] */ Launcher* host,
            /* [in] */ Int32 appWidgetId,
            /* [in] */ IAppWidgetHostView* layout,
            /* [in] */ Int32 resultCode);

        CARAPI Run();

    private:
        Launcher* mHost;
        Int32 mAppWidgetId;
        AutoPtr<IAppWidgetHostView> mLayout;
        Int32 mResultCode;
    };

    class MyRunnable3
        : public Runnable
    {
    public:
        TO_STRING_IMPL("Launcher::MyRunnable3")

        MyRunnable3(
            /* [in] */ Launcher* host,
            /* [in] */ Int32 resultCode);

        CARAPI Run();

    private:
        Launcher* mHost;
        Int32 mResultCode;
    };

    class MyThread2
        : public Thread
    {
    public:
        TO_STRING_IMPL("Launcher::MyThread2")

        MyThread2(
            /* [in] */ Launcher* host,
            /* [in] */ const String& name,
            /* [in] */ Int32 appWidgetId);

        CARAPI Run();

    private:
        Launcher* mHost;
        Int32 mAppWidgetId;
    };

    class MyOnDrawListener
        : public Object
        , public IOnDrawListener
    {
    private:
        class MyRunnable4
            : public Runnable
        {
        public:
            TO_STRING_IMPL("Launcher::MyRunnable4")

            MyRunnable4(
                /* [in] */ Launcher* host,
                /* [in] */ IOnDrawListener* listener);

            CARAPI Run();

        private:
            Launcher* mHost;
            AutoPtr<IOnDrawListener> mListener;
        };

    public:
        TO_STRING_IMPL("Launcher::MyOnDrawListener")

        CAR_INTERFACE_DECL();

        MyOnDrawListener(
            /* [in] */ Launcher* host);

        CARAPI OnDraw();

    private:
        Launcher* mHost;
        Boolean mStarted;
    };

    class MyHandler
        : public Handler
    {
    private:
        class MyRunnable5
            : public Runnable
        {
        public:
            TO_STRING_IMPL("Launcher::MyRunnable5")

            MyRunnable5(
                /* [in] */ IView* v);

            CARAPI Run();

        private:
            AutoPtr<IView> mV;
        };

    public:
        TO_STRING_IMPL("Launcher::MyHandler")

        MyHandler(
            /* [in] */ Launcher* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        Launcher* mHost;
    };

    class MyRunnable6
        : public Runnable
    {
    public:
        TO_STRING_IMPL("Launcher::MyRunnable6")

        MyRunnable6(
            /* [in] */ Launcher* host,
            /* [in] */ Boolean alreadyOnHome);

        CARAPI Run();

    private:
        Launcher* mHost;
        Boolean mAlreadyOnHome;
    };

    class MyAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        TO_STRING_IMPL("Launcher::MyAnimatorListenerAdapter")

        MyAnimatorListenerAdapter(
            /* [in] */ Launcher* host,
            /* [in] */ ICellLayout* cl,
            /* [in] */ IFolderIcon* fi);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        Launcher* mHost;
        AutoPtr<ICellLayout> mCl;
        AutoPtr<IFolderIcon> mFi;
    };

    class ShowAlphaAnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        TO_STRING_IMPL("Launcher::ShowAlphaAnimatorUpdateListener")

        CAR_INTERFACE_DECL();

        ShowAlphaAnimatorUpdateListener(
            /* [in] */ Launcher* host,
            /* [in] */ IView* fromView,
            /* [in] */ IAppsCustomizeTabHost* toView);

        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        Launcher* mHost;
        AutoPtr<IView> mFromView;
        AutoPtr<IAppsCustomizeTabHost> mToView;
    };

    class ShowAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        TO_STRING_IMPL("Launcher::ShowAnimatorListenerAdapter")

        ShowAnimatorListenerAdapter(
            /* [in] */ Launcher* host,
            /* [in] */ IView* fromView,
            /* [in] */ IAppsCustomizeTabHost* toView,
            /* [in] */ Boolean animated,
            /* [in] */ Boolean springLoaded);

        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

    private:
        Launcher* mHost;
        AutoPtr<IView> mFromView;
        AutoPtr<IAppsCustomizeTabHost> mToView;
        Boolean mAnimated;
        Boolean mSpringLoaded;
        Boolean mAnimationCancelled;
    };

    class MyRunnable7
        : public Runnable
    {
    public:
        TO_STRING_IMPL("Launcher::MyRunnable7")

        MyRunnable7(
            /* [in] */ Launcher* host,
            /* [in] */ IAnimatorSet* stateAnimation,
            /* [in] */ IView* fromView,
            /* [in] */ IAppsCustomizeTabHost* toView,
            /* [in] */ Boolean animated,
            /* [in] */ Float scale);

        CARAPI Run();

    private:
        Launcher* mHost;
        AutoPtr<IAnimatorSet> mStateAnimation;
        AutoPtr<IView> mFromView;
        AutoPtr<IAppsCustomizeTabHost> mToView;
        Boolean mAnimated;
        Float mScale;
    };

    class MyOnGlobalLayoutListener
        : public Object
        , public IOnGlobalLayoutListener
    {
    public:
        TO_STRING_IMPL("Launcher::MyOnGlobalLayoutListener")

        CAR_INTERFACE_DECL();

        MyOnGlobalLayoutListener(
            /* [in] */ IRunnable* startAnimRunnable,
            /* [in] */ IAppsCustomizeTabHost* toView);

        CARAPI OnGlobalLayout();

    private:
        AutoPtr<IRunnable> mStartAnimRunnable;
        AutoPtr<IAppsCustomizeTabHost> mToView;
    };

     class MyAnimatorUpdateListener2
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        TO_STRING_IMPL("Launcher::MyAnimatorUpdateListener2")

        CAR_INTERFACE_DECL();

        MyAnimatorUpdateListener2(
            /* [in] */ Launcher* host,
            /* [in] */ IView* fromView,
            /* [in] */ IView* toView);

        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        Launcher* mHost;
        AutoPtr<IView> mFromView;
        AutoPtr<IView> mToView;
    };

    class MyAnimatorListenerAdapter3
        : public AnimatorListenerAdapter
    {
    public:
        TO_STRING_IMPL("Launcher::MyAnimatorListenerAdapter3")

        MyAnimatorListenerAdapter3(
            /* [in] */ Launcher* host,
            /* [in] */ IView* fromView,
            /* [in] */ IView* toView,
            /* [in] */ Boolean animated,
            /* [in] */ IRunnable* onCompleteRunnable);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        Launcher* mHost;
        AutoPtr<IView> mFromView;
        AutoPtr<IView> mToView;
        Boolean mAnimated;
        AutoPtr<IRunnable> mOnCompleteRunnable;
    };

    class MyRunnable8
        : public Runnable
    {
    public:
        TO_STRING_IMPL("Launcher::MyRunnable8")

        MyRunnable8(
            /* [in] */ Launcher* host);

        CARAPI Run();

    private:
        Launcher* mHost;
    };

    class MyRunnable9
        : public Runnable
    {
    public:
        TO_STRING_IMPL("Launcher::MyRunnable9")

        MyRunnable9(
            /* [in] */ Launcher* host,
            /* [in] */ Boolean successfulDrop,
            /* [in] */ IRunnable* onCompleteRunnable);

        CARAPI Run();

    private:
        Launcher* mHost;
        Boolean mSuccessfulDrop;
        AutoPtr<IRunnable> mOnCompleteRunnable;
    };

    class MyRunnable10
        : public Runnable
    {
    public:
        TO_STRING_IMPL("Launcher::MyRunnable10")

        MyRunnable10(
            /* [in] */ Launcher* host,
            /* [in] */ IArrayList* shortcuts,
            /* [in] */ Int32 start,
            /* [in] */ Int32 end);

        CARAPI Run();

    private:
        Launcher* mHost;
        AutoPtr<IArrayList> mShortcuts;
        Int32 mStart;
        Int32 mEnd;
    };

    class MyRunnable11
        : public Runnable
    {
    public:
        TO_STRING_IMPL("Launcher::MyRunnable11")

        MyRunnable11(
            /* [in] */ Launcher* host,
            /* [in] */ IHashMap* folders);

        CARAPI Run();

    private:
        Launcher* mHost;
        AutoPtr<IHashMap> mFolders;
    };

    class MyRunnable12
        : public Runnable
    {
    public:
        TO_STRING_IMPL("Launcher::MyRunnable12")

        MyRunnable12(
            /* [in] */ Launcher* host,
            /* [in] */ ILauncherAppWidgetInfo* item);

        CARAPI Run();

    private:
        Launcher* mHost;
        AutoPtr<ILauncherAppWidgetInfo> mItem;
    };

    class MyRunnable13
        : public Runnable
    {
    public:
        TO_STRING_IMPL("Launcher::MyRunnable13")

        MyRunnable13(
            /* [in] */ Launcher* host);

        CARAPI Run();

    private:
        Launcher* mHost;
    };

    class MyRunnable14
        : public Runnable
    {
    public:
        TO_STRING_IMPL("Launcher::MyRunnable14")

        MyRunnable14(
            /* [in] */ Launcher* host);

        CARAPI Run();

    private:
        Launcher* mHost;
    };

    class MyComparator
        : public Object
        , public IComparator
    {
    public:
        TO_STRING_IMPL("Launcher::MyComparator")

        CAR_INTERFACE_DECL();

        CARAPI Compare(
            /* [in] */ IInterface* lhs,
            /* [in] */ IInterface* rhs,
            /* [out] */ Int32* result);
    };

    class MyAnimatorListenerAdapter4
        : public AnimatorListenerAdapter
    {
    public:
        TO_STRING_IMPL("Launcher::MyAnimatorListenerAdapter4")

        MyAnimatorListenerAdapter4(
            /* [in] */ Launcher* host);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        Launcher* mHost;
    };

    class MyThread3
        : public Thread
    {
    public:
        TO_STRING_IMPL("Launcher::MyThread3")

        MyThread3(
            /* [in] */ Launcher* host,
            /* [in] */ const String& name);

        CARAPI Run();

    private:
        Launcher* mHost;
    };

    class MyRunnable15
        : public Runnable
    {
    public:
        TO_STRING_IMPL("Launcher::MyRunnable15")

        MyRunnable15(
            /* [in] */ Launcher* host,
            /* [in] */ IArrayList* apps);

        CARAPI Run();

    private:
        Launcher* mHost;
        AutoPtr<IArrayList> mApps;
    };

    class MyRunnable16
        : public Runnable
    {
    public:
        TO_STRING_IMPL("Launcher::MyRunnable16")

        MyRunnable16(
            /* [in] */ Launcher* host,
            /* [in] */ IArrayList* apps);

        CARAPI Run();

    private:
        Launcher* mHost;
        AutoPtr<IArrayList> mApps;
    };

    class MyRunnable17
        : public Runnable
    {
    public:
        TO_STRING_IMPL("Launcher::MyRunnable17")

        MyRunnable17(
            /* [in] */ Launcher* host,
            /* [in] */ IArrayList* apps);

        CARAPI Run();

    private:
        Launcher* mHost;
        AutoPtr<IArrayList> mApps;
    };

    class MyRunnable18
        : public Runnable
    {
    public:
        TO_STRING_IMPL("Launcher::MyRunnable18")

        MyRunnable18(
            /* [in] */ Launcher* host,
            /* [in] */ IArrayList* packageNames,
            /* [in] */ IArrayList* appInfos,
            /* [in] */ Boolean matchPackageNamesOnly,
            /* [in] */ IUserHandle* user);

        CARAPI Run();

    private:
        Launcher* mHost;
        AutoPtr<IArrayList> mPackageNames;
        AutoPtr<IArrayList> mAppInfos;
        Boolean mMatchPackageNamesOnly;
        AutoPtr<IUserHandle> mUser;
    };

    class MyRunnable19
        : public Runnable
    {
    public:
        TO_STRING_IMPL("Launcher::MyRunnable19")

        MyRunnable19(
            /* [in] */ Launcher* host);

        CARAPI Run();

    private:
        Launcher* mHost;
    };

    class MyRunnable20
        : public Runnable
    {
    public:
        TO_STRING_IMPL("Launcher::MyRunnable20")

        MyRunnable20(
            /* [in] */ Launcher* host);

        CARAPI Run();

    private:
        Launcher* mHost;
    };

    class MyRunnable21
        : public Runnable
    {
    public:
        TO_STRING_IMPL("Launcher::MyRunnable21")

        MyRunnable21(
            /* [in] */ Launcher* host,
            /* [in] */ ICling* cling);

        CARAPI Run();

    private:
        Launcher* mHost;
        AutoPtr<ICling> mCling;
    };

    class MyThread4
        : public Thread
    {
    public:
        TO_STRING_IMPL("Launcher::MyThread4")

        MyThread4(
            /* [in] */ Launcher* host,
            /* [in] */ const String& name,
            /* [in] */ const String& flag);

        CARAPI Run();

    private:
        Launcher* mHost;
        String mFlag;
    };

    class MyAnimatorListenerAdapter5
        : public AnimatorListenerAdapter
    {
    public:
        TO_STRING_IMPL("Launcher::MyAnimatorListenerAdapter5")

        MyAnimatorListenerAdapter5(
            /* [in] */ Launcher* host,
            /* [in] */ ICling* cling,
            /* [in] */ const String& flag);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        Launcher* mHost;
        AutoPtr<ICling> mCling;
        String mFlag;
    };

    class MyRunnable22
        : public Runnable
    {
    public:
        TO_STRING_IMPL("Launcher::MyRunnable22")

        MyRunnable22(
            /* [in] */ IViewGroup* parent,
            /* [in] */ IView* cling);

        CARAPI Run();

    private:
        AutoPtr<IViewGroup> mParent;
        AutoPtr<IView> mCling;
    };

public:
    CAR_INTERFACE_DECL();

    Launcher();

    CARAPI constructor();

    CARAPI GetDragLayer(
        /* [out] */ IDragLayer** layer);

    CARAPI IsDraggingEnabled(
        /* [out] */ Boolean* result);

    static CARAPI GetScreen(
        /* [out] */ Int32* screen);

    static CARAPI SetScreen(
        /* [in] */ Int32 screen);

    //@Override
    CARAPI OnRetainNonConfigurationInstance(
        /* [out] */ IInterface** obj);

    // We can't hide the IME if it was forced open.  So don't bother
    /*
    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);

        if (hasFocus) {
            final InputMethodManager inputManager = (InputMethodManager)
                    getSystemService(Context.INPUT_METHOD_SERVICE);
            WindowManager.LayoutParams lp = getWindow().getAttributes();
            inputManager.hideSoftInputFromWindow(lp.token, 0, new android.os.ResultReceiver(new
                        android.os.Handler()) {
                        protected void onReceiveResult(int resultCode, Bundle resultData) {
                            Log.d(TAG, "ResultReceiver got resultCode=" + resultCode);
                        }
                    });
            Log.d(TAG, "called hideSoftInputFromWindow from onWindowFocusChanged");
        }
    }
    */

    //@Override
    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    /**
     * Creates a view representing a shortcut.
     *
     * @param info The data structure describing the shortcut.
     *
     * @return A View inflated from R.layout.application.
     */
    CARAPI CreateShortcut(
        /* [in] */ IShortcutInfo* info,
        /* [out] */ IView** view);

    /**
     * Creates a view representing a shortcut inflated from the specified resource.
     *
     * @param layoutResId The id of the XML layout used to create the shortcut.
     * @param parent The group the shortcut belongs to.
     * @param info The data structure describing the shortcut.
     *
     * @return A View inflated from layoutResId.
     */
    CARAPI CreateShortcut(
        /* [in] */ Int32 layoutResId,
        /* [in] */ IViewGroup* parent,
        /* [in] */ IShortcutInfo* info,
        /* [out] */ IView** view);

    /**
     * Add an application shortcut to the workspace.
     *
     * @param data The intent describing the application.
     * @param cellInfo The position on screen where to create the shortcut.
     */
    CARAPI CompleteAddApplication(
        /* [in] */ IIntent* data,
        /* [in] */ Int64 container,
        /* [in] */ Int32 screen,
        /* [in] */ Int32 cellX,
        /* [in] */ Int32 cellY);

    static CARAPI GetSpanForWidget(
        /* [in] */ IContext* context,
        /* [in] */ IComponentName* component,
        /* [in] */ Int32 minWidth,
        /* [in] */ Int32 minHeight,
        /* [out, callee] */ ArrayOf<Int32>** array);

    static CARAPI GetSpanForWidget(
        /* [in] */ IContext* context,
        /* [in] */ IAppWidgetProviderInfo* info,
        /* [out, callee] */ ArrayOf<Int32>** array);

    static CARAPI GetMinSpanForWidget(
        /* [in] */ IContext* context,
        /* [in] */ IAppWidgetProviderInfo* info,
        /* [out, callee] */ ArrayOf<Int32>** array);

    static CARAPI GetSpanForWidget(
        /* [in] */ IContext* context,
        /* [in] */ IPendingAddWidgetInfo* info,
        /* [out, callee] */ ArrayOf<Int32>** array);

    static CARAPI GetMinSpanForWidget(
        /* [in] */ IContext* context,
        /* [in] */ IPendingAddWidgetInfo* info,
        /* [out, callee] */ ArrayOf<Int32>** array);

    //@Override
    CARAPI OnAttachedToWindow();

    //@Override
    CARAPI OnDetachedFromWindow();

    CARAPI OnWindowVisibilityChanged(
        /* [in] */ Int32 visibility);

    CARAPI AddWidgetToAutoAdvanceIfNeeded(
        /* [in] */ IView* hostView,
        /* [in] */ IAppWidgetProviderInfo* appWidgetInfo);

    CARAPI RemoveWidgetToAutoAdvance(
        /* [in] */ IView* hostView);

    CARAPI RemoveAppWidget(
        /* [in] */ ILauncherAppWidgetInfo* launcherInfo);

    CARAPI ShowOutOfSpaceMessage(
        /* [in] */ Boolean isHotseatLayout);

    CARAPI GetAppWidgetHost(
        /* [out] */ ILauncherAppWidgetHost** host);

    CARAPI GetModel(
        /* [out] */ ILauncherModel** mode);

    CARAPI CloseSystemDialogs();

    //@Override
    CARAPI OnRestoreInstanceState(
        /* [in] */ IBundle* state);

    //@Override
    CARAPI OnDestroy();

    CARAPI GetDragController(
        /* [out] */ IDragController** controller);

    //@Override
    CARAPI StartActivityForResult(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 requestCode);

    /**
     * Indicates that we want global search for this activity by setting the globalSearch
     * argument for {@link #startSearch} to true.
     */
    //@Override
    CARAPI StartSearch(
        /* [in] */ const String& initialQuery,
        /* [in] */ Boolean selectInitialQuery,
        /* [in] */ IBundle* appSearchData,
        /* [in] */ Boolean globalSearch);

    /**
     * Starts the global search activity. This code is a copied from SearchManager
     */
    CARAPI StartGlobalSearch(
        /* [in] */ const String& initialQuery,
        /* [in] */ Boolean selectInitialQuery,
        /* [in] */ IBundle* appSearchData,
        /* [in] */ IRect* sourceBounds);

    //@Override
    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnPrepareOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnSearchRequested(
        /* [out] */ Boolean* result);

    CARAPI IsWorkspaceLocked(
        /* [out] */ Boolean* result);

    CARAPI AddAppWidgetImpl(
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IItemInfo* info,
        /* [in] */ IAppWidgetHostView* boundWidget,
        /* [in] */ IAppWidgetProviderInfo* appWidgetInfo);

    /**
     * Process a shortcut drop.
     *
     * @param componentName The name of the component
     * @param screen The screen where it should be added
     * @param cell The cell it should be added to, optional
     * @param position The location on the screen where it was dropped, optional
     */
    CARAPI ProcessShortcutFromDrop(
        /* [in] */ IComponentName* componentName,
        /* [in] */ Int64 container,
        /* [in] */ Int32 screen,
        /* [in] */ ArrayOf<Int32>* cell,
        /* [in] */ ArrayOf<Int32>* loc);

    /**
     * Process a widget drop.
     *
     * @param info The PendingAppWidgetInfo of the widget being added.
     * @param screen The screen where it should be added
     * @param cell The cell it should be added to, optional
     * @param position The location on the screen where it was dropped, optional
     */
    CARAPI AddAppWidgetFromDrop(
        /* [in] */ IPendingAddWidgetInfo* info,
        /* [in] */ Int64 container,
        /* [in] */ Int32 screen,
        /* [in] */ ArrayOf<Int32>* cell,
        /* [in] */ ArrayOf<Int32>* span,
        /* [in] */ ArrayOf<Int32>* loc);

    CARAPI ProcessShortcut(
        /* [in] */ IIntent* intent);

    CARAPI ProcessWallpaper(
        /* [in] */ IIntent* intent);

    CARAPI AddFolder(
        /* [in] */ ICellLayout* layout,
        /* [in] */ Int64 container,
        /* [in] */ Int32 screen,
        /* [in] */ Int32 cellX,
        /* [in] */ Int32 cellY,
        /* [out] */ IFolderIcon** icon);

    CARAPI RemoveFolder(
        /* [in] */ IFolderInfo* folder);

    //@Override
    CARAPI DispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnBackPressed();

    /**
     * Launches the intent referred by the clicked shortcut.
     *
     * @param v The view representing the clicked shortcut.
     */
    virtual CARAPI OnClick(
        /* [in] */ IView* v);

    virtual CARAPI OnTouch(
        /* [in] */ IView* v,
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    /**
     * Event handler for the search button
     *
     * @param v The view that was clicked.
     */
    CARAPI OnClickSearchButton(
        /* [in] */ IView* v);

    /**
     * Event handler for the voice button
     *
     * @param v The view that was clicked.
     */
    CARAPI OnClickVoiceButton(
        /* [in] */ IView* v);

    /**
     * Event handler for the "grid" button that appears on the home screen, which
     * enters all apps mode.
     *
     * @param v The view that was clicked.
     */
    CARAPI OnClickAllAppsButton(
        /* [in] */ IView* v);

    CARAPI OnTouchDownAllAppsButton(
        /* [in] */ IView* v);

    CARAPI OnClickAppMarketButton(
        /* [in] */ IView* v);

    CARAPI StartApplicationDetailsActivity(
        /* [in] */ IComponentName* componentName,
        /* [in] */ IUserHandle* user);

    CARAPI StartApplicationUninstallActivity(
        /* [in] */ IApplicationInfo* appInfo,
        /* [in] */ IUserHandle* user);

    using Activity::StartActivity;

    CARAPI StartActivity(
        /* [in] */ IView* v,
        /* [in] */ IIntent* intent,
        /* [in] */ IInterface* tag,
        /* [out] */ Boolean* result);

    CARAPI StartActivitySafely(
        /* [in] */ IView* v,
        /* [in] */ IIntent* intent,
        /* [in] */ IInterface* tag,
        /* [out] */ Boolean* result);

    CARAPI StartAppWidgetConfigureActivitySafely(
        /* [in] */ Int32 appWidgetId);

    CARAPI StartActivityForResultSafely(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 requestCode);

    /**
     * Opens the user folder described by the specified tag. The opening of the folder
     * is animated relative to the specified View. If the View is null, no animation
     * is played.
     *
     * @param folderInfo The FolderInfo describing the folder to open.
     */
    CARAPI OpenFolder(
        /* [in] */ IFolderIcon* folderIcon);

    CARAPI CloseFolder();

    CARAPI CloseFolder(
        /* [in] */ IFolder* folder);

    virtual CARAPI OnLongClick(
        /* [in] */ IView* v,
        /* [out] */ Boolean* result);

    CARAPI IsHotseatLayout(
        /* [in] */ IView* layout,
        /* [out] */ Boolean* result);

    CARAPI GetHotseat(
        /* [out] */ IHotseat** seat);

    CARAPI GetSearchBar(
        /* [out] */ ISearchDropTargetBar** bar);

    /**
     * Returns the CellLayout of the specified container at the specified screen.
     */
    CARAPI GetCellLayout(
        /* [in] */ Int64 container,
        /* [in] */ Int32 screen,
        /* [out] */ ICellLayout** layout);

    CARAPI GetWorkspace(
        /* [out] */ IWorkspace** workspace);

    // Now a part of LauncherModel.Callbacks. Used to reorder loading steps.
    //@Override
    CARAPI IsAllAppsVisible(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI IsAllAppsButtonRank(
        /* [in] */ Int32 rank,
        /* [out] */ Boolean* result);

    CARAPI DisableWallpaperIfInAllApps();

    CARAPI UpdateWallpaperVisibility(
        /* [in] */ Boolean visible);

    //@Override
    CARAPI OnTrimMemory(
        /* [in] */ Int32 level);

    //@Override
    CARAPI OnWindowFocusChanged(
        /* [in] */ Boolean hasFocus);

    CARAPI ShowWorkspace(
        /* [in] */ Boolean animated);

    CARAPI ShowWorkspace(
        /* [in] */ Boolean animated,
        /* [in] */ IRunnable* onCompleteRunnable);

    CARAPI ShowAllApps(
        /* [in] */ Boolean animated);

    CARAPI EnterSpringLoadedDragMode();

    CARAPI ExitSpringLoadedDragModeDelayed(
        /* [in] */ Boolean successfulDrop,
        /* [in] */ Boolean extendedDelay,
        /* [in] */ IRunnable* onCompleteRunnable);

    CARAPI ExitSpringLoadedDragMode();

    CARAPI HideDockDivider();

    CARAPI ShowDockDivider(
        /* [in] */ Boolean animated);

    CARAPI LockAllApps();

    CARAPI UnlockAllApps();

    /**
     * Shows the hotseat area.
     */
    CARAPI ShowHotseat(
        /* [in] */ Boolean animated);

    /**
     * Hides the hotseat area.
     */
    CARAPI HideHotseat(
        /* [in] */ Boolean animated);

    /**
     * Add an item from all apps or customize onto the given workspace screen.
     * If layout is null, add to the current screen.
     */
    CARAPI AddExternalItemToScreen(
        /* [in] */ IItemInfo* itemInfo,
        /* [in] */ ICellLayout* layout);

    //@Override
    CARAPI DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* result);

    /**
     * If the activity is currently paused, signal that we need to re-run the loader
     * in onResume.
     *
     * This needs to be called from incoming places where resources might have been loaded
     * while we are paused.  That is becaues the Configuration might be wrong
     * when we're not running, and if it comes back to what it was when we
     * were paused, we are not restarted.
     *
     * Implementation of the method from LauncherModel.Callbacks.
     *
     * @return true if we are currently paused.  The caller might be able to
     * skip some work in that case since we will come back again.
     */
    CARAPI SetLoadOnResume(
        /* [out] */ Boolean* result);

    /**
     * Implementation of the method from LauncherModel.Callbacks.
     */
    CARAPI GetCurrentWorkspaceScreen(
        /* [out] */ Int32* screen);

    /**
     * Refreshes the shortcuts shown on the workspace.
     *
     * Implementation of the method from LauncherModel.Callbacks.
     */
    CARAPI StartBinding();

    /**
     * Bind the items start-end from the list.
     *
     * Implementation of the method from LauncherModel.Callbacks.
     */
    CARAPI BindItems(
        /* [in] */ IArrayList* shortcuts,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    /**
     * Implementation of the method from LauncherModel.Callbacks.
     */
    CARAPI BindFolders(
        /* [in] */ IHashMap* folders);

    /**
     * Add the views for a widget to the workspace.
     *
     * Implementation of the method from LauncherModel.Callbacks.
     */
    CARAPI BindAppWidget(
        /* [in] */ ILauncherAppWidgetInfo* item);

    CARAPI OnPageBoundSynchronously(
        /* [in] */ Int32 page);

    /**
     * Callback saying that there aren't any more items to bind.
     *
     * Implementation of the method from LauncherModel.Callbacks.
     */
    CARAPI FinishBindingItems();

    //@Override
    CARAPI BindSearchablesChanged();

    /**
     * Add the icons for all apps.
     *
     * Implementation of the method from LauncherModel.Callbacks.
     */
    CARAPI BindAllApplications(
        /* [in] */ IArrayList* apps);

    /**
     * A package was installed.
     *
     * Implementation of the method from LauncherModel.Callbacks.
     */
    CARAPI BindAppsAdded(
        /* [in] */ IArrayList* apps);

    /**
     * A package was updated.
     *
     * Implementation of the method from LauncherModel.Callbacks.
     */
    CARAPI BindAppsUpdated(
        /* [in] */ IArrayList* apps);

    /**
     * A package was uninstalled.  We take both the super set of packageNames
     * in addition to specific applications to remove, the reason being that
     * this can be called when a package is updated as well.  In that scenario,
     * we only remove specific components from the workspace, where as
     * package-removal should clear all items by package name.
     *
     * Implementation of the method from LauncherModel.Callbacks.
     */
    CARAPI BindComponentsRemoved(
        /* [in] */ IArrayList* packageNames,
        /* [in] */ IArrayList* appInfos,
        /* [in] */ Boolean matchPackageNamesOnly,
        /* [in] */ IUserHandle* user);

    CARAPI BindPackagesUpdated(
        /* [in] */ IArrayList* widgetsAndShortcuts);

    CARAPI IsRotationEnabled(
        /* [out] */ Boolean* result);

    CARAPI LockScreenOrientation();

    CARAPI UnlockScreenOrientation(
        /* [in] */ Boolean immediate);

    CARAPI ShowFirstRunWorkspaceCling();

    CARAPI ShowFirstRunAllAppsCling(
        /* [in] */ ArrayOf<Int32>* position);

    CARAPI ShowFirstRunFoldersCling(
        /* [out] */ ICling** cling);

    CARAPI IsFolderClingVisible(
        /* [out] */ Boolean* result);

    CARAPI DismissWorkspaceCling(
        /* [in] */ IView* v);

    CARAPI DismissAllAppsCling(
        /* [in] */ IView* v);

    CARAPI DismissFolderCling(
        /* [in] */ IView* v);

    /**
     * Prints out out state for debugging.
     */
    CARAPI DumpState();

    //@Override
    CARAPI Dump(
        /* [in] */ const String& prefix,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* writer,
        /* [in] */ ArrayOf<String>* args);

    static CARAPI DumpDebugLogsToConsole();

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnUserLeaveHint();

    //@Override
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

    //@Override
    CARAPI OnStop();

    //@Override
    CARAPI OnStart();

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnNewIntent(
        /* [in] */ IIntent* intent);

    //@Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

private:
    static CARAPI_(Boolean) InitStaticBlock();

    static CARAPI_(Boolean) IsPropertyEnabled(
        /* [in] */ const String& propertyName);

    CARAPI_(void) UpdateGlobalIcons();

    CARAPI_(void) CheckForLocaleChange();

    static CARAPI_(void) ReadConfiguration(
        /* [in] */ IContext* context,
        /* [in] */ LocaleConfiguration* configuration);

    static CARAPI_(void) WriteConfiguration(
        /* [in] */ IContext* context,
        /* [in] */ LocaleConfiguration* configuration);

    /**
     * Returns whether we should delay spring loaded mode -- for shortcuts and widgets that have
     * a configuration step, this allows the proper animations to run after other transitions.
     */
    CARAPI_(Boolean) CompleteAdd(
        /* [in] */ PendingAddArguments* args);

    CARAPI_(void) CompleteTwoStageWidgetDrop(
        /* [in] */ Int32 resultCode,
        /* [in] */ Int32 appWidgetId);

    CARAPI_(Boolean) AcceptFilter();

    CARAPI_(String) GetTypedText();

    CARAPI_(void) ClearTypedText();

    /**
     * Given the integer (ordinal) value of a State enum instance, convert it to a variable of type
     * State
     */
    static CARAPI_(LauncherState) OrdinalToState(
        /* [in] */ Int32 stateOrdinal);

    static CARAPI_(Int32) OrdinalOfState(
        /* [in] */ LauncherState state);

    /**
     * Restores the previous state, if it exists.
     *
     * @param savedState The previous state.
     */
    CARAPI_(void) RestoreState(
        /* [in] */ IBundle* savedState);

    /**
     * Finds all the views we need and configure them properly.
     */
    CARAPI_(void) SetupViews();

    /**
     * Add a shortcut to the workspace.
     *
     * @param data The intent describing the shortcut.
     * @param cellInfo The position on screen where to create the shortcut.
     */
    CARAPI_(void) CompleteAddShortcut(
        /* [in] */ IIntent* data,
        /* [in] */ Int64 container,
        /* [in] */ Int32 screen,
        /* [in] */ Int32 cellX,
        /* [in] */ Int32 cellY);

    /**
     * Add a widget to the workspace.
     *
     * @param appWidgetId The app widget id
     * @param cellInfo The position on screen where to create the widget.
     */
    CARAPI_(void) CompleteAddAppWidget(
        /* [in] */ Int32 appWidgetId,
        /* [in] */ Int64 container,
        /* [in] */ Int32 screen,
        /* [in] */ IAppWidgetHostView* hostView,
        /* [in] */ IAppWidgetProviderInfo* appWidgetInfo);

    CARAPI_(void) SendAdvanceMessage(
        /* [in] */ Int64 delay);

    CARAPI_(void) UpdateRunning();

    CARAPI_(void) ResetAddInfo();

    CARAPI_(void) StartWallpaper();

    /**
     * Registers various content observers. The current implementation registers
     * only a favorites observer to keep track of the favorites applications.
     */
    CARAPI_(void) RegisterContentObservers();

    /**
     * Re-listen when widgets are reset.
     */
    CARAPI_(void) OnAppWidgetReset();

    CARAPI_(void) HandleFolderClick(
        /* [in] */ IFolderIcon* folderIcon);

    /**
     * This method draws the FolderIcon to an ImageView and then adds and positions that ImageView
     * in the DragLayer in the exact absolute location of the original FolderIcon.
     */
    CARAPI_(void) CopyFolderIconToImage(
        /* [in] */ IFolderIcon* fi);

    CARAPI_(void) GrowAndFadeOutFolderIcon(
        /* [in] */ IFolderIcon* fi);

    CARAPI_(void) ShrinkAndFadeInFolderIcon(
        /* [in] */ IFolderIcon* fi);

        /**
     * Helper method for the cameraZoomIn/cameraZoomOut animations
     * @param view The view being animated
     * @param scaleFactor The scale factor used for the zoom
     */
    CARAPI_(void) SetPivotsForZoom(
        /* [in] */ IView* view,
        /* [in] */ Float scaleFactor);

    CARAPI_(void) SetWorkspaceBackground(
        /* [in] */ Boolean workspace);

    CARAPI_(void) DispatchOnLauncherTransitionPrepare(
        /* [in] */ IView* v,
        /* [in] */ Boolean animated,
        /* [in] */ Boolean toWorkspace);

    CARAPI_(void) DispatchOnLauncherTransitionStart(
        /* [in] */ IView* v,
        /* [in] */ Boolean animated,
        /* [in] */ Boolean toWorkspace);

    CARAPI_(void) DispatchOnLauncherTransitionStep(
        /* [in] */ IView* v,
        /* [in] */ Float t);

    CARAPI_(void) DispatchOnLauncherTransitionEnd(
        /* [in] */ IView* v,
        /* [in] */ Boolean animated,
        /* [in] */ Boolean toWorkspace);

    /**
     * Things to test when changing the following seven functions.
     *   - Home from workspace
     *          - from center screen
     *          - from other screens
     *   - Home from all apps
     *          - from center screen
     *          - from other screens
     *   - Back from all apps
     *          - from center screen
     *          - from other screens
     *   - Launch app from workspace and quit
     *          - with back
     *          - with home
     *   - Launch app from all apps and quit
     *          - with back
     *          - with home
     *   - Go to a screen that's not the default, then all
     *     apps, and launch and app, and go back
     *          - with back
     *          -with home
     *   - On workspace, long press power and go back
     *          - with back
     *          - with home
     *   - On all apps, long press power and go back
     *          - with back
     *          - with home
     *   - On workspace, power off
     *   - On all apps, power off
     *   - Launch an app and turn off the screen while in that app
     *          - Go back with home key
     *          - Go back with back key  TODO: make this not go to workspace
     *          - From all apps
     *          - From workspace
     *   - Enter and exit car mode (becuase it causes an extra configuration changed)
     *          - From all apps
     *          - From the center workspace
     *          - From another workspace
     */

    /**
     * Zoom the camera out from the workspace to reveal 'toView'.
     * Assumes that the view to show is anchored at either the very top or very bottom
     * of the screen.
     */
    CARAPI_(void) ShowAppsCustomizeHelper(
        /* [in] */ Boolean animated,
        /* [in] */ Boolean springLoaded);

    /**
     * Zoom the camera back into the workspace, hiding 'fromView'.
     * This is the opposite of showAppsCustomizeHelper.
     * @param animated If true, the transition will be animated.
     */
    CARAPI_(void) HideAppsCustomizeHelper(
        /* [in] */ LauncherState toState,
        /* [in] */ Boolean animated,
        /* [in] */ Boolean springLoaded,
        /* [in] */ IRunnable* onCompleteRunnable);

    /** Maps the current orientation to an index for referencing orientation correct global icons */
    CARAPI_(Int32) GetCurrentOrientationIndexForGlobalIcons();

    CARAPI_(AutoPtr<IDrawable>) GetExternalPackageToolbarIcon(
        /* [in] */ IComponentName* activityName,
        /* [in] */ const String& resourceName);

    // if successful in getting icon, return it; otherwise, set button to use default drawable
    CARAPI_(AutoPtr<IDrawableConstantState>) UpdateTextButtonWithIconFromExternalActivity(
        /* [in] */ Int32 buttonId,
        /* [in] */ IComponentName* activityName,
        /* [in] */ Int32 fallbackDrawableId,
        /* [in] */ const String& toolbarResourceName);

    // if successful in getting icon, return it; otherwise, set button to use default drawable
    CARAPI_(AutoPtr<IDrawableConstantState>) UpdateButtonWithIconFromExternalActivity(
        /* [in] */ Int32 buttonId,
        /* [in] */ IComponentName* activityName,
        /* [in] */ Int32 fallbackDrawableId,
        /* [in] */ const String& toolbarResourceName);

    CARAPI_(void) UpdateTextButtonWithDrawable(
        /* [in] */ Int32 buttonId,
        /* [in] */ IDrawable* d);

    CARAPI_(void) UpdateButtonWithDrawable(
        /* [in] */ Int32 buttonId,
        /* [in] */ IDrawableConstantState* d);

    CARAPI_(void) InvalidatePressedFocusedStates(
        /* [in] */ IView* container,
        /* [in] */ IView* button);

    CARAPI_(Boolean) UpdateGlobalSearchIcon();

    CARAPI_(void) UpdateGlobalSearchIcon(
        /* [in] */ IDrawableConstantState* d);

    CARAPI_(Boolean) UpdateVoiceSearchIcon(
        /* [in] */ Boolean searchVisible);

    CARAPI_(void) UpdateVoiceSearchIcon(
        /* [in] */ IDrawableConstantState* d);

    /**
     * Sets the app market icon
     */
    CARAPI_(void) UpdateAppMarketIcon();

    CARAPI_(void) UpdateAppMarketIcon(
        /* [in] */ IDrawableConstantState* d);

    /**
     * If the activity is currently paused, signal that we need to run the passed Runnable
     * in onResume.
     *
     * This needs to be called from incoming places where resources might have been loaded
     * while we are paused.  That is becaues the Configuration might be wrong
     * when we're not running, and if it comes back to what it was when we
     * were paused, we are not restarted.
     *
     * Implementation of the method from LauncherModel.Callbacks.
     *
     * @return true if we are currently paused.  The caller might be able to
     * skip some work in that case since we will come back again.
     */
    CARAPI_(Boolean) WaitUntilResume(
        /* [in] */ IRunnable* run,
        /* [in] */ Boolean deletePreviousRunnables);

    CARAPI_(Boolean) WaitUntilResume(
        /* [in] */ IRunnable* run);

    CARAPI_(Boolean) CanRunNewAppsAnimation();

    /**
     * Runs a new animation that scales up icons that were added while Launcher was in the
     * background.
     *
     * @param immediate whether to run the animation or show the results immediately
     */
    CARAPI_(void) RunNewAppsAnimation(
        /* [in] */ Boolean immediate);

    CARAPI_(Int32) MapConfigurationOriActivityInfoOri(
        /* [in] */ Int32 configOri);

    /* Cling related */
    CARAPI_(Boolean) IsClingsEnabled();

    CARAPI_(AutoPtr<ICling>) InitCling(
        /* [in] */ Int32 clingId,
        /* [in] */ ArrayOf<Int32>* positionData,
        /* [in] */ Boolean animate,
        /* [in] */ Int32 delay);

    CARAPI_(void) DismissCling(
        /* [in] */ ICling* cling,
        /* [in] */ const String& flag,
        /* [in] */ Int32 duration);

    CARAPI_(void) RemoveCling(
        /* [in] */ Int32 id);

    CARAPI_(Boolean) SkipCustomClingIfNoAccounts();


public:
    AutoPtr<InnerListener> mInnerListener;

private:
    friend class LauncherModel;

    static const String TAG;
    static const Boolean LOGD;

    static const Int32 MENU_GROUP_WALLPAPER;
    static const Int32 MENU_WALLPAPER_SETTINGS;
    static const Int32 MENU_MANAGE_APPS;
    static const Int32 MENU_SYSTEM_SETTINGS;
    static const Int32 MENU_HELP;

    static const Int32 REQUEST_CREATE_SHORTCUT;
    static const Int32 REQUEST_CREATE_APPWIDGET;
    static const Int32 REQUEST_PICK_APPLICATION;
    static const Int32 REQUEST_PICK_SHORTCUT;
    static const Int32 REQUEST_PICK_APPWIDGET;
    static const Int32 REQUEST_PICK_WALLPAPER;

    static const Int32 REQUEST_BIND_APPWIDGET;

    static const String PREFERENCES;

    // Type: int
    static const String RUNTIME_STATE_CURRENT_SCREEN;
    // Type: int
    static const String RUNTIME_STATE;
    // Type: int
    static const String RUNTIME_STATE_PENDING_ADD_CONTAINER;
    // Type: int
    static const String RUNTIME_STATE_PENDING_ADD_SCREEN;
    // Type: int
    static const String RUNTIME_STATE_PENDING_ADD_CELL_X;
    // Type: int
    static const String RUNTIME_STATE_PENDING_ADD_CELL_Y;
    // Type: boolean
    static const String RUNTIME_STATE_PENDING_FOLDER_RENAME;
    // Type: long
    static const String RUNTIME_STATE_PENDING_FOLDER_RENAME_ID;
    // Type: int
    static const String RUNTIME_STATE_PENDING_ADD_SPAN_X;
    // Type: int
    static const String RUNTIME_STATE_PENDING_ADD_SPAN_Y;
    // Type: parcelable
    static const String RUNTIME_STATE_PENDING_ADD_WIDGET_INFO;
    // Type: parcelable
    static const String RUNTIME_STATE_PENDING_ADD_WIDGET_ID;

    static const String TOOLBAR_ICON_METADATA_NAME ;
    static const String TOOLBAR_SEARCH_ICON_METADATA_NAME;
    static const String TOOLBAR_VOICE_SEARCH_ICON_METADATA_NAME;

    LauncherState mState;
    AutoPtr<IAnimatorSet> mStateAnimation;
    AutoPtr<IAnimatorSet> mDividerAnimator;

    static const Int32 EXIT_SPRINGLOADED_MODE_SHORT_TIMEOUT;
    static const Int32 EXIT_SPRINGLOADED_MODE_LONG_TIMEOUT;
    static const Int32 SHOW_CLING_DURATION;
    static const Int32 DISMISS_CLING_DURATION;

    static Object sLock;
    static Int32 sScreen;

    // How long to wait before the new-shortcut animation automatically pans the workspace
    static Int32 NEW_APPS_ANIMATION_INACTIVE_TIMEOUT_SECONDS;

    AutoPtr<IBroadcastReceiver> mCloseSystemDialogsReceiver;
    AutoPtr<IContentObserver> mWidgetObserver;

    AutoPtr<ILayoutInflater> mInflater;

    AutoPtr<IWorkspace> mWorkspace;
    AutoPtr<IView> mQsbDivider;
    AutoPtr<IView> mDockDivider;
    AutoPtr<IView> mLauncherView;
    AutoPtr<IDragLayer> mDragLayer;
    AutoPtr<IDragController> mDragController;

    AutoPtr<IAppWidgetManager> mAppWidgetManager;
    AutoPtr<ILauncherAppWidgetHost> mAppWidgetHost;

    AutoPtr<ItemInfo> mPendingAddInfo;
    AutoPtr<IAppWidgetProviderInfo> mPendingAddWidgetInfo;
    Int32 mPendingAddWidgetId;

    AutoPtr<ArrayOf<Int32> > mTmpAddItemCellCoordinates;

    AutoPtr<IFolderInfo> mFolderInfo;

    AutoPtr<IHotseat> mHotseat;
    AutoPtr<IView> mAllAppsButton;

    AutoPtr<ISearchDropTargetBar> mSearchDropTargetBar;
    AutoPtr<IAppsCustomizeTabHost> mAppsCustomizeTabHost;
    AutoPtr<IAppsCustomizePagedView> mAppsCustomizeContent;
    Boolean mAutoAdvanceRunning;

    AutoPtr<IBundle> mSavedState;
    // We set the state in both onCreate and then onNewIntent in some cases, which causes both
    // scroll issues (because the workspace may not have been measured yet) and extra work.
    // Instead, just save the state that we need to restore Launcher to, and commit it in onResume.
    LauncherState mOnResumeState;

    AutoPtr<ISpannableStringBuilder> mDefaultKeySsb;

    Boolean mWorkspaceLoading;

    Boolean mPaused;
    Boolean mRestoring;
    Boolean mWaitingForResult;
    Boolean mOnResumeNeedsLoad;

    AutoPtr<IArrayList> mOnResumeCallbacks;

    // Keep track of whether the user has left launcher
    static Boolean sPausedFromUserAction;

    AutoPtr<IBundle> mSavedInstanceState;

    AutoPtr<ILauncherModel> mModel;
    AutoPtr<IIconCache> mIconCache;
    Boolean mUserPresent;
    Boolean mVisible;
    Boolean mAttached;

    static AutoPtr<LocaleConfiguration> sLocaleConfiguration;

    static Boolean initStaticBlock;

    static AutoPtr<IHashMap> sFolders;

    AutoPtr<IIntent> mAppMarketIntent;

    // Related to the auto-advancing of widgets
    const Int32 ADVANCE_MSG;
    const Int32 mAdvanceInterval;
    const Int32 mAdvanceStagger;
    Int64 mAutoAdvanceSentTime;
    Int64 mAutoAdvanceTimeLeft;
    AutoPtr<IHashMap> mWidgetsToAdvance;

    // Determines how long to wait after a rotation before restoring the screen orientation to
    // match the sensor state.
    const Int32 mRestoreScreenOrientationDelay;

    // External icons saved in case of resource changes, orientation, etc.
    static AutoPtr<ArrayOf<IDrawableConstantState*> > sGlobalSearchIcon;
    static AutoPtr<ArrayOf<IDrawableConstantState*> > sVoiceSearchIcon;
    static AutoPtr<ArrayOf<IDrawableConstantState*> > sAppMarketIcon;

    AutoPtr<IDrawable> mWorkspaceBackgroundDrawable;

    AutoPtr<IArrayList> mSynchronouslyBoundPages;

    static AutoPtr<IArrayList> sDumpLogs;

    // We only want to get the SharedPreferences once since it does an FS stat each time we get
    // it from the context.
    AutoPtr<ISharedPreferences> mSharedPrefs;

    // Holds the page that we need to animate to, and the icon views that we need to animate up
    // when we scroll to that page on resume.
    Int32 mNewShortcutAnimatePage;
    AutoPtr<IArrayList> mNewShortcutAnimateViews;
    AutoPtr<IImageView> mFolderIconImageView;
    AutoPtr<IBitmap> mFolderIconBitmap;
    AutoPtr<ICanvas> mFolderIconCanvas;
    AutoPtr<IRect> mRectForFolderAnimation;

    AutoPtr<IBubbleTextView> mWaitingForResume;

    AutoPtr<HideFromAccessibilityHelper> mHideFromAccessibilityHelper;

    AutoPtr<IRunnable> mBuildLayersRunnable;

    static AutoPtr<IArrayList> sPendingAddList;

    static Boolean sForceEnableRotation;

    AutoPtr<IBroadcastReceiver> mReceiver;

    AutoPtr<IHandler> mHandler;

    /**
     * A number of packages were updated.
     */

    AutoPtr<IArrayList> mWidgetsAndShortcuts;

    AutoPtr<IRunnable> mBindPackagesUpdatedRunnable;

    AutoPtr<IButton> btnClingDismisView;

};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_LAUNCHER_H__