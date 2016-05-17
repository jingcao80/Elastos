
#include "elastos/droid/launcher2/Launcher.h"
#include "elastos/droid/launcher2/LauncherApplication.h"
#include "elastos/droid/launcher2/LauncherAnimUtils.h"
#include "elastos/droid/launcher2/CellLayout.h"
#include "elastos/droid/launcher2/DragController.h"
#include "elastos/droid/launcher2/LauncherAppWidgetHost.h"
#include "elastos/droid/launcher2/FirstFrameAnimatorHelper.h"
#include "elastos/droid/launcher2/InstallShortcutReceiver.h"
#include "elastos/droid/launcher2/ShortcutInfo.h"
#include "elastos/droid/launcher2/PendingAddItemInfo.h"
#include "elastos/droid/launcher2/LauncherAppWidgetInfo.h"
#include "elastos/droid/launcher2/FolderInfo.h"
#include "elastos/droid/launcher2/LauncherProvider.h"
#include "elastos/droid/launcher2/DragLayer.h"
#include "elastos/droid/launcher2/LauncherAnimUtils.h"
#include "elastos/droid/launcher2/LauncherSettings.h"
#include "elastos/droid/launcher2/Folder.h"
#include "elastos/droid/launcher2/LauncherViewPropertyAnimator.h"
#include "elastos/droid/launcher2/Workspace.h"
#include "elastos/droid/launcher2/IconCache.h"
#include "elastos/droid/launcher2/CLauncherBroadcastReceiver.h"
#include "elastos/droid/launcher2/CLauncherCloseSystemDialogsIntentReceiver.h"
#include "elastos/droid/launcher2/CLauncherAppWidgetResetObserver.h"
#include "elastos/droid/appwidget/AppWidgetHostView.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/text/SpannableStringBuilder.h"
#include "Elastos.Droid.Accounts.h"
#include "elastos/droid/R.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Speech.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Character.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include "R.h"

using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Accounts::IAccountManager;
using Elastos::Droid::Accounts::IAccountManagerHelper;
using Elastos::Droid::Accounts::CAccountManagerHelper;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::IPropertyValuesHolderHelper;
using Elastos::Droid::Animation::CPropertyValuesHolderHelper;
using Elastos::Droid::Animation::IAnimatorSetBuilder;
using Elastos::Droid::App::IApplication;
using Elastos::Droid::App::ISearchManager;
using Elastos::Droid::App::IActivityOptions;
using Elastos::Droid::App::CActivityOptionsHelper;
using Elastos::Droid::App::IActivityOptionsHelper;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::AppWidget::IAppWidgetHost;
using Elastos::Droid::AppWidget::CAppWidgetManagerHelper;
using Elastos::Droid::AppWidget::IAppWidgetManagerHelper;
using Elastos::Droid::AppWidget::AppWidgetHostView;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentHelper;
using Elastos::Droid::Content::CIntentHelper;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::PorterDuffMode_CLEAR;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CStrictMode;
using Elastos::Droid::Os::IStrictMode;
using Elastos::Droid::Os::IStrictModeVmPolicy;
using Elastos::Droid::Os::IStrictModeThreadPolicy;
using Elastos::Droid::Os::CStrictModeVmPolicyBuilder;
using Elastos::Droid::Os::IStrictModeVmPolicyBuilder;
using Elastos::Droid::Os::CStrictModeThreadPolicyBuilder;
using Elastos::Droid::Os::IStrictModeThreadPolicyBuilder;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Droid::View::Animation::IDecelerateInterpolator;
using Elastos::Droid::View::Animation::CAccelerateInterpolator;
using Elastos::Droid::View::Animation::IAccelerateInterpolator;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::EIID_IViewOnLongClickListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IOnDrawListener;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::View::EIID_IOnGlobalLayoutListener;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::IHapticFeedbackConstants;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::Animation::CAccelerateDecelerateInterpolator;
using Elastos::Droid::View::Animation::IAccelerateDecelerateInterpolator;
using Elastos::Droid::Speech::IRecognizerIntent;
using Elastos::Droid::Text::SpannableStringBuilder;
using Elastos::Droid::Text::CSpannableStringBuilder;
using Elastos::Droid::Text::ISpannableStringBuilder;
using Elastos::Droid::Text::CSelection;
using Elastos::Droid::Text::ISelection;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::Method::IKeyListener;
using Elastos::Droid::Text::Method::ITextKeyListener;
using Elastos::Droid::Text::Method::ITextKeyListenerHelper;
using Elastos::Droid::Text::Method::CTextKeyListenerHelper;
using Elastos::Droid::Widget::IAdvanceable;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::ITabHost;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::CImageView;
using Elastos::Core::IFloat;
using Elastos::Core::IBoolean;
using Elastos::Core::EIID_IComparator;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::Character;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::CoreUtils;
using Elastos::IO::IInputStream;
using Elastos::IO::CDataInputStream;
using Elastos::IO::IDataInputStream;
using Elastos::IO::IDataInput;
using Elastos::IO::ICloseable;
using Elastos::IO::CDataOutputStream;
using Elastos::IO::IDataOutputStream;
using Elastos::IO::IDataOutput;
using Elastos::IO::IOutputStream;
using Elastos::IO::IFlushable;
using Elastos::Utility::ISet;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IMap;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

Launcher::MyRunnable::MyRunnable(
    /* [in] */ Launcher* host)
    : mHost(host)
{
}

ECode Launcher::MyRunnable::Run()
{
    if (mHost->mWorkspace != NULL) {
        return mHost->mWorkspace->BuildPageHardwareLayers();
    }
    return NOERROR;
}

Launcher::MyAsyncTask::MyAsyncTask(
    /* [in] */ Launcher* owner)
    : mHost(owner)
{
}

ECode Launcher::MyAsyncTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
Logger::E("Launcher::MyAsyncTask", "============================MyAsyncTask::DoInBackground 1");
    AutoPtr<LocaleConfiguration> localeConfiguration = new LocaleConfiguration();
    mHost->ReadConfiguration(mHost, localeConfiguration);
    *result = TO_IINTERFACE(localeConfiguration);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode Launcher::MyAsyncTask::OnPostExecute(
    /* [in] */ IInterface* result)
{
Logger::E("Launcher::MyAsyncTask", "============================MyAsyncTask::OnPostExecute");
    mHost->sLocaleConfiguration = (LocaleConfiguration*)IObject::Probe(result);
    mHost->CheckForLocaleChange();  // recursive, but now with a locale configuration
    return NOERROR;
}

Launcher::LocaleConfiguration::LocaleConfiguration()
    : mLocale(NULL)
    , mMcc(-1)
    , mMnc(-1)
{
}

Launcher::MyThread::MyThread(
    /* [in] */ Launcher* host,
    /* [in] */ const String& name,
    /* [in] */ LocaleConfiguration* localeConfiguration)
    : mHost(host)
    , mLocaleConfiguration(localeConfiguration)
{
    Thread::constructor(name);
}

ECode Launcher::MyThread::Run()
{
    mHost->WriteConfiguration(mHost, mLocaleConfiguration);
    return NOERROR;
}

Launcher::MyRunnable2::MyRunnable2(
    /* [in] */ Launcher* host,
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IAppWidgetHostView* layout,
    /* [in] */ Int32 resultCode)
    : mHost(host)
    , mAppWidgetId(appWidgetId)
    , mLayout(layout)
    , mResultCode(resultCode)
{
}

ECode Launcher::MyRunnable2::Run()
{
    mHost->CompleteAddAppWidget(mAppWidgetId, mHost->mPendingAddInfo->mContainer,
            mHost->mPendingAddInfo->mScreen, mLayout, NULL);
    return mHost->ExitSpringLoadedDragModeDelayed((mResultCode != RESULT_CANCELED), FALSE,
            NULL);
}

Launcher::MyRunnable3::MyRunnable3(
    /* [in] */ Launcher* host,
    /* [in] */ Int32 resultCode)
    : mHost(host)
    , mResultCode(resultCode)
{
}

ECode Launcher::MyRunnable3::Run()
{
    return mHost->ExitSpringLoadedDragModeDelayed((mResultCode != RESULT_CANCELED), FALSE,
            NULL);
}


Launcher::MyThread2::MyThread2(
    /* [in] */ Launcher* host,
    /* [in] */ const String& name,
    /* [in] */ Int32 appWidgetId)
    : mHost(host)
    , mAppWidgetId(appWidgetId)
{
    Thread::constructor(name);
}

ECode Launcher::MyThread2::Run()
{
    return IAppWidgetHost::Probe(mHost->mAppWidgetHost)->DeleteAppWidgetId(mAppWidgetId);
}

Launcher::MyBroadcastReceiver::MyBroadcastReceiver()
{
}

ECode Launcher::MyBroadcastReceiver::constructor()
{
    return BroadcastReceiver::constructor();
}

ECode Launcher::MyBroadcastReceiver::constructor(
    /* [in] */ ILauncher* host)
{
    mHost = (Launcher*)host;
    return BroadcastReceiver::constructor();
}

ECode Launcher::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_SCREEN_OFF.Equals(action)) {
        mHost->mUserPresent = FALSE;
        mHost->mDragLayer->ClearAllResizeFrames();
        mHost->UpdateRunning();

        // Reset AllApps to its initial state only if we are not in the middle of
        // processing a multi-step drop
        if (mHost->mAppsCustomizeTabHost != NULL &&
                mHost->mPendingAddInfo->mContainer == IItemInfo::NO_ID) {
            mHost->mAppsCustomizeTabHost->Reset();
            mHost->ShowWorkspace(FALSE);
        }
    }
    else if (IIntent::ACTION_USER_PRESENT.Equals(action)) {
        mHost->mUserPresent = TRUE;
        mHost->UpdateRunning();
    }
    else if ((IIntent::ACTION_MANAGED_PROFILE_ADDED.Equals(action))
            || (IIntent::ACTION_MANAGED_PROFILE_REMOVED.Equals(action))) {
        AutoPtr<ILauncherModel> mode;
        mHost->GetModel((ILauncherModel**)&mode);
        mode->ForceReload();
    }
    return NOERROR;
}

Launcher::MyOnDrawListener::MyRunnable4::MyRunnable4(
    /* [in] */ Launcher* host,
    /* [in] */ IOnDrawListener* listener)
    : mHost(host)
    , mListener(listener)
{
}

ECode Launcher::MyOnDrawListener::MyRunnable4::Run()
{
    if (mHost->mWorkspace != NULL) {
        AutoPtr<IViewTreeObserver> observer;
        IView::Probe(mHost->mWorkspace)->GetViewTreeObserver((IViewTreeObserver**)&observer);
        if (observer != NULL) {
            return observer->RemoveOnDrawListener(mListener);
        }
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(Launcher::MyOnDrawListener, Object, IOnDrawListener);

Launcher::MyOnDrawListener::MyOnDrawListener(
    /* [in] */ Launcher* host)
    : mHost(host)
    , mStarted(FALSE)
{
}

ECode Launcher::MyOnDrawListener::OnDraw()
{
    if (mStarted) return NOERROR;
    mStarted = TRUE;
    // We delay the layer building a bit in order to give
    // other message processing a time to run.  In particular
    // this avoids a delay in hiding the IME if it was
    // currently shown, because doing that may involve
    // some communication back with the app.
    Boolean tmp;
    IView::Probe(mHost->mWorkspace)->PostDelayed(mHost->mBuildLayersRunnable, 500, &tmp);
    AutoPtr<IOnDrawListener> listener = this;
    AutoPtr<IRunnable> r = new MyRunnable4(mHost, listener);
    return IView::Probe(mHost->mWorkspace)->Post(r, &tmp);
}

Launcher::MyHandler::MyRunnable5::MyRunnable5(
    /* [in] */ IView* v)
    : mV(v)
{
}

ECode Launcher::MyHandler::MyRunnable5::Run()
{
    return IAdvanceable::Probe(mV)->Advance();
}

Launcher::MyHandler::MyHandler(
    /* [in] */ Launcher* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode Launcher::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    if (what == mHost->ADVANCE_MSG) {
        AutoPtr<ISet> keySet;
        mHost->mWidgetsToAdvance->GetKeySet((ISet**)&keySet);
        AutoPtr<ArrayOf<IInterface*> > array;
        keySet->ToArray((ArrayOf<IInterface*>**)&array);
        for (Int32 i = 0; i < array->GetLength(); i++) {
            AutoPtr<IView> key = IView::Probe((*array)[i]);

            AutoPtr<IInterface> obj;
            mHost->mWidgetsToAdvance->Get(key, (IInterface**)&obj);
            AutoPtr<IAppWidgetProviderInfo> info = IAppWidgetProviderInfo::Probe(obj);
            Int32 id;
            info->GetAutoAdvanceViewId(&id);
            AutoPtr<IView> v;
            key->FindViewById(id, (IView**)&v);
            Int32 delay = mHost->mAdvanceStagger * i;
            if (IAdvanceable::Probe(v) != NULL) {
                AutoPtr<IRunnable> r = new MyRunnable5(v);
                Boolean res;
                IView::Probe(mHost)->PostDelayed(r, delay, &res);
            }
            i++;
        }
        mHost->SendAdvanceMessage(mHost->mAdvanceInterval);
    }
    return NOERROR;
}

Launcher::MyRunnable6::MyRunnable6(
    /* [in] */ Launcher* host,
    /* [in] */ Boolean alreadyOnHome)
    : mHost(host)
    , mAlreadyOnHome(alreadyOnHome)
{
}

ECode Launcher::MyRunnable6::Run()
{
    if (mHost->mWorkspace == NULL) {
        // Can be cases where mWorkspace is null, this prevents a NPE
        return NOERROR;
    }
    AutoPtr<IFolder> openFolder;
    mHost->mWorkspace->GetOpenFolder((IFolder**)&openFolder);
    // In all these cases, only animate if we're already on home
    mHost->mWorkspace->ExitWidgetResizeMode();
    Boolean res;
    if (mAlreadyOnHome && mHost->mState == Launcher_WORKSPACE && (mHost->mWorkspace->IsTouchActive(&res), !res) &&
            openFolder == NULL) {
        mHost->mWorkspace->MoveToDefaultScreen(TRUE);
    }

    mHost->CloseFolder();
    mHost->ExitSpringLoadedDragMode();

    // If we are already on home, then just animate back to the workspace,
    // otherwise, just wait until onResume to set the state back to Workspace
    if (mAlreadyOnHome) {
        mHost->ShowWorkspace(TRUE);
    }
    else {
        mHost->mOnResumeState = Launcher_WORKSPACE;
    }

    AutoPtr<IWindow> window;
    mHost->GetWindow((IWindow**)&window);
    AutoPtr<IView> v;
    window->PeekDecorView((IView**)&v);

    if (v != NULL) {
        AutoPtr<IBinder> token;
        v->GetWindowToken((IBinder**)&token);
        if (token != NULL) {
            AutoPtr<IInterface> obj;
            mHost->GetSystemService(INPUT_METHOD_SERVICE, (IInterface**)&obj);
            AutoPtr<IInputMethodManager> imm = IInputMethodManager::Probe(obj);
            Boolean tmp;
            imm->HideSoftInputFromWindow(token, 0, &tmp);
        }
    }

    // Reset AllApps to its initial state
    if (!mAlreadyOnHome && mHost->mAppsCustomizeTabHost != NULL) {
        mHost->mAppsCustomizeTabHost->Reset();
    }
    return NOERROR;
}

Launcher::MyAnimatorListenerAdapter::MyAnimatorListenerAdapter(
    /* [in] */ Launcher* host,
    /* [in] */ ICellLayout* cl,
    /* [in] */ IFolderIcon* fi)
    : mHost(host)
    , mCl(cl)
    , mFi(fi)
{
}

ECode Launcher::MyAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (mCl != NULL) {
        mCl->ClearFolderLeaveBehind();
        // Remove the ImageView copy of the FolderIcon and make the original visible.
        IViewGroup::Probe(mHost->mDragLayer)->RemoveView(IView::Probe(mHost->mFolderIconImageView));
        return IView::Probe(mFi)->SetVisibility(IView::VISIBLE);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(Launcher::MyAnimatorUpdateListener, Object, IAnimatorUpdateListener);

Launcher::MyAnimatorUpdateListener::MyAnimatorUpdateListener(
    /* [in] */ Launcher* host,
    /* [in] */ IView* fromView,
    /* [in] */ IAppsCustomizeTabHost* toView)
    : mHost(host)
    , mFromView(fromView)
    , mToView(toView)
{
}

ECode Launcher::MyAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    if (animation == NULL) {
        //throw new RuntimeException("animation is null");
        Slogger::E("Launcher::MyAnimatorUpdateListener", "animation is null");
        return E_RUNTIME_EXCEPTION;
    }
    AutoPtr<IInterface> value;
Slogger::D("Launcher::MyAnimatorUpdateListener", "======OnAnimationUpdate animation=%p",animation);
    animation->GetAnimatedValue((IInterface**)&value);
Slogger::D("Launcher::MyAnimatorUpdateListener", "======OnAnimationUpdate value=%p",value.Get());
    AutoPtr<IFloat> obj = IFloat::Probe(value);
    Float t;
Slogger::D("Launcher::MyAnimatorUpdateListener", "======OnAnimationUpdate obj=%p",obj.Get());
    obj->GetValue(&t);
    mHost->DispatchOnLauncherTransitionStep(IView::Probe(mFromView), t);
    mHost->DispatchOnLauncherTransitionStep(IView::Probe(mToView), t);
Slogger::D("Launcher::MyAnimatorUpdateListener", "======OnAnimationUpdate return");
    return NOERROR;
}

Launcher::MyAnimatorListenerAdapter2::MyAnimatorListenerAdapter2(
    /* [in] */ Launcher* host,
    /* [in] */ IView* fromView,
    /* [in] */ IAppsCustomizeTabHost* toView,
    /* [in] */ Boolean animated,
    /* [in] */ Boolean springLoaded)
    : mHost(host)
    , mFromView(fromView)
    , mToView(toView)
    , mAnimated(animated)
    , mSpringLoaded(springLoaded)
    , mAnimationCancelled(FALSE)
{
}

ECode Launcher::MyAnimatorListenerAdapter2::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    mHost->UpdateWallpaperVisibility(TRUE);
    // Prepare the position
    IView::Probe(mToView)->SetTranslationX(0.0f);
    IView::Probe(mToView)->SetTranslationY(0.0f);
    IView::Probe(mToView)->SetVisibility(IView::VISIBLE);
    return IView::Probe(mToView)->BringToFront();
}

ECode Launcher::MyAnimatorListenerAdapter2::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->DispatchOnLauncherTransitionEnd(IView::Probe(mFromView), mAnimated, FALSE);
    mHost->DispatchOnLauncherTransitionEnd(IView::Probe(mToView), mAnimated, FALSE);

    Boolean res;
    if (mHost->mWorkspace != NULL && !mSpringLoaded &&
            (LauncherApplication::IsScreenLarge(&res), !res)) {
        // Hide the workspace scrollbar
        IPagedView::Probe(mHost->mWorkspace)->HideScrollingIndicator(TRUE);
        mHost->HideDockDivider();
    }
    if (!mAnimationCancelled) {
        mHost->UpdateWallpaperVisibility(FALSE);
    }

    // Hide the search bar
    if (mHost->mSearchDropTargetBar != NULL) {
        mHost->mSearchDropTargetBar->HideSearchBar(FALSE);
    }
    return NOERROR;
}

ECode Launcher::MyAnimatorListenerAdapter2::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    mAnimationCancelled = TRUE;
    return NOERROR;
}

Launcher::MyRunnable7::MyRunnable7(
    /* [in] */ Launcher* host,
    /* [in] */ IAnimatorSet* stateAnimation,
    /* [in] */ IView* fromView,
    /* [in] */ IAppsCustomizeTabHost* toView,
    /* [in] */ Boolean animated,
    /* [in] */ Float scale)
    : mHost(host)
    , mStateAnimation(stateAnimation)
    , mFromView(fromView)
    , mToView(toView)
    , mAnimated(animated)
    , mScale(scale)
{
}

ECode Launcher::MyRunnable7::Run()
{
    // Check that mStateAnimation hasn't changed while
    // we waited for a layout/draw pass
    if (TO_IINTERFACE(mHost->mStateAnimation) != TO_IINTERFACE(mStateAnimation)) {
        return NOERROR;
    }
    mHost->SetPivotsForZoom(IView::Probe(mToView), mScale);
    mHost->DispatchOnLauncherTransitionStart(mFromView, mAnimated, FALSE);
    mHost->DispatchOnLauncherTransitionStart(IView::Probe(mToView), mAnimated, FALSE);
    LauncherAnimUtils::StartAnimationAfterNextDraw(IAnimator::Probe(mHost->mStateAnimation),
            IView::Probe(mToView));
    return NOERROR;
}

CAR_INTERFACE_IMPL(Launcher::MyOnGlobalLayoutListener, Object, IOnGlobalLayoutListener);

Launcher::MyOnGlobalLayoutListener::MyOnGlobalLayoutListener(
    /* [in] */ IRunnable* startAnimRunnable,
    /* [in] */ IAppsCustomizeTabHost* toView)
    : mStartAnimRunnable(startAnimRunnable)
    , mToView(toView)
{
}

ECode Launcher::MyOnGlobalLayoutListener::OnGlobalLayout()
{
    mStartAnimRunnable->Run();
    AutoPtr<IViewTreeObserver> observe;
    IView::Probe(mToView)->GetViewTreeObserver((IViewTreeObserver**)&observe);
    return observe->RemoveOnGlobalLayoutListener(this);
}

CAR_INTERFACE_IMPL(Launcher::MyAnimatorUpdateListener2, Object, IAnimatorUpdateListener);

Launcher::MyAnimatorUpdateListener2::MyAnimatorUpdateListener2(
    /* [in] */ Launcher* host,
    /* [in] */ IView* fromView,
    /* [in] */ IView* toView)
    : mHost(host)
    , mFromView(fromView)
    , mToView(toView)
{
}

ECode Launcher::MyAnimatorUpdateListener2::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> value;
    animation->GetAnimatedValue((IInterface**)&value);
    AutoPtr<IFloat> obj = IFloat::Probe(value);
    Float t;
    obj->GetValue(&t);
    mHost->DispatchOnLauncherTransitionStep(mFromView, t);
    mHost->DispatchOnLauncherTransitionStep(mToView, t);
    return NOERROR;
}

Launcher::MyAnimatorListenerAdapter3::MyAnimatorListenerAdapter3(
    /* [in] */ Launcher* host,
    /* [in] */ IView* fromView,
    /* [in] */ IView* toView,
    /* [in] */ Boolean animated,
    /* [in] */ IRunnable* onCompleteRunnable)
    : mHost(host)
    , mFromView(fromView)
    , mToView(toView)
    , mAnimated(animated)
    , mOnCompleteRunnable(onCompleteRunnable)
{
}

ECode Launcher::MyAnimatorListenerAdapter3::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->UpdateWallpaperVisibility(TRUE);
    mFromView->SetVisibility(IView::GONE);
    mHost->DispatchOnLauncherTransitionEnd(mFromView, mAnimated, TRUE);
    mHost->DispatchOnLauncherTransitionEnd(mToView, mAnimated, TRUE);
    if (mHost->mWorkspace != NULL) {
        IPagedView::Probe(mHost->mWorkspace)->HideScrollingIndicator(FALSE);
    }
    if (mOnCompleteRunnable != NULL) {
        mOnCompleteRunnable->Run();
    }
    IPagedView::Probe(mHost->mAppsCustomizeContent)->UpdateCurrentPageScroll();
    return IPagedView::Probe(mHost->mAppsCustomizeContent)->ResumeScrolling();
}

Launcher::MyRunnable8::MyRunnable8(
    /* [in] */ Launcher* host)
    : mHost(host)
{
}

ECode Launcher::MyRunnable8::Run()
{
    return mHost->DisableWallpaperIfInAllApps();
}

Launcher::MyRunnable9::MyRunnable9(
    /* [in] */ Launcher* host,
    /* [in] */ Boolean successfulDrop,
    /* [in] */ IRunnable* onCompleteRunnable)
    : mHost(host)
    , mSuccessfulDrop(successfulDrop)
    , mOnCompleteRunnable(onCompleteRunnable)
{
}

ECode Launcher::MyRunnable9::Run()
{
    if (mSuccessfulDrop) {
        // Before we show workspace, hide all apps again because
        // exitSpringLoadedDragMode made it visible. This is a bit hacky; we should
        // clean up our state transition functions
        IView::Probe(mHost->mAppsCustomizeTabHost)->SetVisibility(IView::GONE);
        return mHost->ShowWorkspace(true, mOnCompleteRunnable);
    }
    else {
        return mHost->ExitSpringLoadedDragMode();
    }
    return NOERROR;
}

Launcher::CloseSystemDialogsIntentReceiver::CloseSystemDialogsIntentReceiver()
{
}

ECode Launcher::CloseSystemDialogsIntentReceiver::constructor()
{
    return BroadcastReceiver::constructor();
}

ECode Launcher::CloseSystemDialogsIntentReceiver::constructor(
    /* [in] */ ILauncher* host)
{
    mHost = (Launcher*)host;
    return BroadcastReceiver::constructor();
}

ECode Launcher::CloseSystemDialogsIntentReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    return mHost->CloseSystemDialogs();
}

Launcher::AppWidgetResetObserver::AppWidgetResetObserver()
{
}

ECode Launcher::AppWidgetResetObserver::constructor()
{
    return NOERROR;//ContentObserver::constructor();
}

ECode Launcher::AppWidgetResetObserver::constructor(
    /* [in] */ ILauncher* host)

{
    mHost = (Launcher*)host;
    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    return ContentObserver::constructor(handler);
}

ECode Launcher::AppWidgetResetObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    mHost->OnAppWidgetReset();
    return NOERROR;
}

Launcher::MyRunnable10::MyRunnable10(
    /* [in] */ Launcher* host,
    /* [in] */ IArrayList* shortcuts,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
    : mHost(host)
    , mShortcuts(shortcuts)
    , mStart(start)
    , mEnd(end)
{
}

ECode Launcher::MyRunnable10::Run()
{
    return mHost->BindItems(mShortcuts, mStart, mEnd);
}

Launcher::MyRunnable11::MyRunnable11(
    /* [in] */ Launcher* host,
    /* [in] */ IHashMap* folders)
    : mHost(host)
    , mFolders(folders)
{
}

ECode Launcher::MyRunnable11::Run()
{
    return mHost->BindFolders(mFolders);
}

Launcher::MyRunnable12::MyRunnable12(
    /* [in] */ Launcher* host,
    /* [in] */ ILauncherAppWidgetInfo* item)
    : mHost(host)
    , mItem(item)
{
}

ECode Launcher::MyRunnable12::Run()
{
    return mHost->BindAppWidget(mItem);
}

Launcher::MyRunnable13::MyRunnable13(
    /* [in] */ Launcher* host)
    : mHost(host)
{
}

ECode Launcher::MyRunnable13::Run()
{
    return mHost->FinishBindingItems();
}

Launcher::MyRunnable14::MyRunnable14(
    /* [in] */ Launcher* host)
    : mHost(host)
{
}

ECode Launcher::MyRunnable14::Run()
{
    mHost->RunNewAppsAnimation(FALSE);
    return NOERROR;
}

CAR_INTERFACE_IMPL(Launcher::MyComparator, Object, IComparator);

ECode Launcher::MyComparator::Compare(
    /* [in] */ IInterface* a,
    /* [in] */ IInterface* b,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IViewGroupLayoutParams> aParams;
    IView::Probe(a)->GetLayoutParams((IViewGroupLayoutParams**)&aParams);
    AutoPtr<CellLayout::CellLayoutLayoutParams> alp =
            (CellLayout::CellLayoutLayoutParams*)ICellLayoutLayoutParams::Probe(aParams);

    AutoPtr<IViewGroupLayoutParams> bParams;
    IView::Probe(b)->GetLayoutParams((IViewGroupLayoutParams**)&bParams);
    AutoPtr<CellLayout::CellLayoutLayoutParams> blp =
            (CellLayout::CellLayoutLayoutParams*)ICellLayoutLayoutParams::Probe(bParams);

    Int32 cellCountX;
    LauncherModel::GetCellCountX(&cellCountX);
    *result = (alp->mCellY * cellCountX + alp->mCellX) - (blp->mCellY * cellCountX + blp->mCellX);
    return NOERROR;
}

Launcher::MyAnimatorListenerAdapter4::MyAnimatorListenerAdapter4(
    /* [in] */ Launcher* host)
    : mHost(host)
{
}

ECode Launcher::MyAnimatorListenerAdapter4::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (mHost->mWorkspace != NULL) {
        Boolean tmp;
        return IView::Probe(mHost->mWorkspace)->PostDelayed(
                mHost->mBuildLayersRunnable, 500, &tmp);
    }
    return NOERROR;
}

Launcher::MyThread3::MyThread3(
    /* [in] */ Launcher* host,
    /* [in] */ const String& name)
    : mHost(host)
{
    Thread::constructor(name);
}

ECode Launcher::MyThread3::Run()
{
    AutoPtr<ISharedPreferencesEditor> editor;
    mHost->mSharedPrefs->Edit((ISharedPreferencesEditor**)&editor);
    editor->PutInt32(IInstallShortcutReceiver::NEW_APPS_PAGE_KEY, -1);
    editor->PutStringSet(IInstallShortcutReceiver::NEW_APPS_LIST_KEY, NULL);
    Boolean succeded;
    return editor->Commit(&succeded);
}

Launcher::MyRunnable15::MyRunnable15(
    /* [in] */ Launcher* host,
    /* [in] */ IArrayList* apps)
    : mHost(host)
    , mApps(apps)
{
}

ECode Launcher::MyRunnable15::Run()
{
    if (mHost->mAppsCustomizeContent != NULL) {
        return mHost->mAppsCustomizeContent->SetApps(mApps);
    }
    return NOERROR;
}

Launcher::MyRunnable16::MyRunnable16(
    /* [in] */ Launcher* host,
    /* [in] */ IArrayList* apps)
    : mHost(host)
    , mApps(apps)
{
}

ECode Launcher::MyRunnable16::Run()
{
    return mHost->BindAppsAdded(mApps);
}

Launcher::MyRunnable17::MyRunnable17(
    /* [in] */ Launcher* host,
    /* [in] */ IArrayList* apps)
    : mHost(host)
    , mApps(apps)
{
}

ECode Launcher::MyRunnable17::Run()
{
    return mHost->BindAppsUpdated(mApps);
}

Launcher::MyRunnable18::MyRunnable18(
    /* [in] */ Launcher* host,
    /* [in] */ IArrayList* packageNames,
    /* [in] */ IArrayList* appInfos,
    /* [in] */ Boolean matchPackageNamesOnly,
    /* [in] */ IUserHandle* user)
    : mHost(host)
    , mPackageNames(packageNames)
    , mAppInfos(appInfos)
    , mMatchPackageNamesOnly(matchPackageNamesOnly)
    , mUser(user)
{
}

ECode Launcher::MyRunnable18::Run()
{
    return mHost->BindComponentsRemoved(mPackageNames, mAppInfos,
            mMatchPackageNamesOnly, mUser);
}

Launcher::MyRunnable19::MyRunnable19(
    /* [in] */ Launcher* host)
    : mHost(host)
{
}

ECode Launcher::MyRunnable19::Run()
{
    mHost->BindPackagesUpdated(mHost->mWidgetsAndShortcuts);
    mHost->mWidgetsAndShortcuts = NULL;
    return NOERROR;
}

Launcher::MyRunnable20::MyRunnable20(
    /* [in] */ Launcher* host)
    : mHost(host)
{
}

ECode Launcher::MyRunnable20::Run()
{
    return mHost->SetRequestedOrientation(IActivityInfo::SCREEN_ORIENTATION_UNSPECIFIED);
}

Launcher::MyRunnable21::MyRunnable21(
    /* [in] */ Launcher* host,
    /* [in] */ ICling* cling)
    : mHost(host)
    , mCling(cling)
{
}

ECode Launcher::MyRunnable21::Run()
{
    IView::Probe(mCling)->SetFocusable(TRUE);
    Boolean tmp;
    return IView::Probe(mCling)->RequestFocus(&tmp);
}

Launcher::MyThread4::MyThread4(
    /* [in] */ Launcher* host,
    /* [in] */ const String& name,
    /* [in] */ const String& flag)
    : mHost(host)
    , mFlag(flag)
{
    Thread::constructor(name);
}

ECode Launcher::MyThread4::Run()
{
    AutoPtr<ISharedPreferencesEditor> editor;
    mHost->mSharedPrefs->Edit((ISharedPreferencesEditor**)&editor);
    editor->PutBoolean(mFlag, TRUE);
    Boolean tmp;
    return editor->Commit(&tmp);
}

Launcher::MyAnimatorListenerAdapter5::MyAnimatorListenerAdapter5(
    /* [in] */ Launcher* host,
    /* [in] */ ICling* cling,
    /* [in] */ const String& flag)
    : mHost(host)
    , mCling(cling)
    , mFlag(flag)
{
}

ECode Launcher::MyAnimatorListenerAdapter5::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    IView::Probe(mCling)->SetVisibility(IView::GONE);
    mCling->Cleanup();
    // We should update the shared preferences on a background thread
    AutoPtr<IThread> t = new MyThread4(mHost, String("dismissClingThread"), mFlag);
    return t->Start();
}

Launcher::MyRunnable22::MyRunnable22(
    /* [in] */ IViewGroup* parent,
    /* [in] */ IView* cling)
    : mParent(parent)
    , mCling(cling)
{
}

ECode Launcher::MyRunnable22::Run()
{
    return mParent->RemoveView(mCling);
}

const String Launcher::TAG("Launcher");
const Boolean Launcher::LOGD = FALSE;

const Int32 Launcher::MENU_GROUP_WALLPAPER = 1;
const Int32 Launcher::MENU_WALLPAPER_SETTINGS = IMenu::FIRST + 1;
const Int32 Launcher::MENU_MANAGE_APPS = MENU_WALLPAPER_SETTINGS + 1;
const Int32 Launcher::MENU_SYSTEM_SETTINGS = MENU_MANAGE_APPS + 1;
const Int32 Launcher::MENU_HELP = MENU_SYSTEM_SETTINGS + 1;

const Int32 Launcher::REQUEST_CREATE_SHORTCUT = 1;
const Int32 Launcher::REQUEST_CREATE_APPWIDGET = 5;
const Int32 Launcher::REQUEST_PICK_APPLICATION = 6;
const Int32 Launcher::REQUEST_PICK_SHORTCUT = 7;
const Int32 Launcher::REQUEST_PICK_APPWIDGET = 9;
const Int32 Launcher::REQUEST_PICK_WALLPAPER = 10;

const Int32 Launcher::REQUEST_BIND_APPWIDGET = 11;

const String Launcher::PREFERENCES("launcher.preferences");

const String Launcher::RUNTIME_STATE_CURRENT_SCREEN("launcher.current_screen");
const String Launcher::RUNTIME_STATE("launcher.state");
const String Launcher::RUNTIME_STATE_PENDING_ADD_CONTAINER("launcher.add_container");
const String Launcher::RUNTIME_STATE_PENDING_ADD_SCREEN("launcher.add_screen");
const String Launcher::RUNTIME_STATE_PENDING_ADD_CELL_X("launcher.add_cell_x");
const String Launcher::RUNTIME_STATE_PENDING_ADD_CELL_Y("launcher.add_cell_y");
const String Launcher::RUNTIME_STATE_PENDING_FOLDER_RENAME("launcher.rename_folder");
const String Launcher::RUNTIME_STATE_PENDING_FOLDER_RENAME_ID("launcher.rename_folder_id");
const String Launcher::RUNTIME_STATE_PENDING_ADD_SPAN_X("launcher.add_span_x");
const String Launcher::RUNTIME_STATE_PENDING_ADD_SPAN_Y("launcher.add_span_y");
const String Launcher::RUNTIME_STATE_PENDING_ADD_WIDGET_INFO("launcher.add_widget_info");
const String Launcher::RUNTIME_STATE_PENDING_ADD_WIDGET_ID("launcher.add_widget_id");

const String Launcher::TOOLBAR_ICON_METADATA_NAME("com.android.launcher.toolbar_icon");
const String Launcher::TOOLBAR_SEARCH_ICON_METADATA_NAME(
        "com.android.launcher.toolbar_search_icon");
const String Launcher::TOOLBAR_VOICE_SEARCH_ICON_METADATA_NAME(
        "com.android.launcher.toolbar_voice_search_icon");

const Int32 Launcher::EXIT_SPRINGLOADED_MODE_SHORT_TIMEOUT = 300;
const Int32 Launcher::EXIT_SPRINGLOADED_MODE_LONG_TIMEOUT = 600;
const Int32 Launcher::SHOW_CLING_DURATION = 550;
const Int32 Launcher::DISMISS_CLING_DURATION = 250;

Object Launcher::sLock;
Int32 Launcher::sScreen = DEFAULT_SCREEN;

// How long to wait before the new-shortcut animation automatically pans the workspace
Int32 Launcher::NEW_APPS_ANIMATION_INACTIVE_TIMEOUT_SECONDS = 10;

Boolean Launcher::sPausedFromUserAction = FALSE;

AutoPtr<Launcher::LocaleConfiguration> Launcher::sLocaleConfiguration;

AutoPtr<IHashMap> Launcher::sFolders;

AutoPtr<ArrayOf<IDrawableConstantState*> > Launcher::sGlobalSearchIcon;
AutoPtr<ArrayOf<IDrawableConstantState*> > Launcher::sVoiceSearchIcon;
AutoPtr<ArrayOf<IDrawableConstantState*> > Launcher::sAppMarketIcon;

AutoPtr<IArrayList> Launcher::sDumpLogs;

AutoPtr<IArrayList> Launcher::sPendingAddList;

Boolean Launcher::InitStaticBlock()
{
    CHashMap::New((IHashMap**)&sFolders);

    sGlobalSearchIcon = ArrayOf<IDrawableConstantState*>::Alloc(2);
    sVoiceSearchIcon = ArrayOf<IDrawableConstantState*>::Alloc(2);
    sAppMarketIcon = ArrayOf<IDrawableConstantState*>::Alloc(2);

    CArrayList::New((IArrayList**)&sDumpLogs);
    CArrayList::New((IArrayList**)&sPendingAddList);
    return TRUE;
}

Boolean Launcher::initStaticBlock = InitStaticBlock();

Boolean Launcher::IsPropertyEnabled(
        /* [in] */ const String& propertyName)
{
    return Logger::IsLoggable(propertyName, Logger::VERBOSE);
}

Boolean Launcher::sForceEnableRotation = IsPropertyEnabled(FORCE_ENABLE_ROTATION_PROPERTY);

CAR_INTERFACE_IMPL_5(Launcher, Activity, ILauncher, IViewOnClickListener,
        IViewOnLongClickListener, ILauncherModelCallbacks, IViewOnTouchListener);

Launcher::Launcher()
    : mState(Launcher_WORKSPACE)
    , mPendingAddWidgetId(-1)
    , mAutoAdvanceRunning(FALSE)
    , mOnResumeState(Launcher_NONE)
    , mWorkspaceLoading(TRUE)
    , mPaused(TRUE)
    , mRestoring(FALSE)
    , mWaitingForResult(FALSE)
    , mOnResumeNeedsLoad(FALSE)
    , mUserPresent(TRUE)
    , mVisible(FALSE)
    , mAttached(FALSE)
    , ADVANCE_MSG(1)
    , mAdvanceInterval(20000)
    , mAdvanceStagger(250)
    , mAutoAdvanceSentTime(0)
    , mAutoAdvanceTimeLeft(-1)
    , mRestoreScreenOrientationDelay(500)
    , mNewShortcutAnimatePage(-1)
{
Slogger::E("Launcher", "============================Launcher::Launcher() 1");
    Activity::constructor();

    CLauncherCloseSystemDialogsIntentReceiver::New(this, (IBroadcastReceiver**)&mCloseSystemDialogsReceiver);
    CLauncherAppWidgetResetObserver::New(this, (IContentObserver**)&mWidgetObserver);

    mPendingAddInfo = new ItemInfo();
    mPendingAddInfo->constructor();

    mTmpAddItemCellCoordinates = ArrayOf<Int32>::Alloc(2);

    CArrayList::New((IArrayList**)&mOnResumeCallbacks);

    CHashMap::New((IHashMap**)&mWidgetsToAdvance);

    CArrayList::New((IArrayList**)&mSynchronouslyBoundPages);

    CArrayList::New((IArrayList**)&mNewShortcutAnimateViews);

    CRect::New((IRect**)&mRectForFolderAnimation);

    mHideFromAccessibilityHelper = new HideFromAccessibilityHelper();

    AutoPtr<IRunnable> mBuildLayersRunnable = new MyRunnable(this);

    CLauncherBroadcastReceiver::New(this, (IBroadcastReceiver**)&mReceiver);

    mHandler = new MyHandler(this);

    mBindPackagesUpdatedRunnable = new MyRunnable19(this);
Slogger::E("Launcher", "============================Launcher::Launcher() return");
}

ECode Launcher::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
Slogger::E("Launcher", "============================Launcher::OnCreate 1");
    if (DEBUG_STRICT_MODE) {
        AutoPtr<IStrictModeThreadPolicyBuilder> builder;
        CStrictModeThreadPolicyBuilder::New((IStrictModeThreadPolicyBuilder**)&builder);
        builder->DetectDiskReads();
        builder->DetectDiskWrites();
        builder->DetectNetwork();   // or .detectAll() for all detectable problems
        builder->PenaltyLog();
        AutoPtr<IStrictModeThreadPolicy> policy;
        builder->Build((IStrictModeThreadPolicy**)&policy);
        AutoPtr<IStrictMode> mode;
        CStrictMode::AcquireSingleton((IStrictMode**)&mode);
        mode->SetThreadPolicy(policy);

        AutoPtr<IStrictModeVmPolicyBuilder> builder2;
        CStrictModeVmPolicyBuilder::New((IStrictModeVmPolicyBuilder**)&builder2);
        builder2->DetectLeakedSqlLiteObjects();
        builder2->DetectLeakedClosableObjects();
        builder2->PenaltyLog();
        builder2->PenaltyDeath();
        AutoPtr<IStrictModeVmPolicy> policy2;
        builder2->Build((IStrictModeVmPolicy**)&policy2);
        mode->SetVmPolicy(policy2);
    }

    Activity::OnCreate(savedInstanceState);
    AutoPtr<IApplication> appl;
    GetApplication((IApplication**)&appl);
    AutoPtr<ILauncherApplication> app = ILauncherApplication::Probe(appl);
    String spKey;
    LauncherApplication::GetSharedPreferencesKey(&spKey);
    GetSharedPreferences(spKey, IContext::MODE_PRIVATE, (ISharedPreferences**)&mSharedPrefs);
Slogger::E("Launcher", "============================Launcher call LauncherApplication::SetLauncher");
    app->SetLauncher(this, (ILauncherModel**)&mModel);
    app->GetIconCache((IIconCache**)&mIconCache);

    mDragController = new DragController();
    ((DragController*)mDragController.Get())->constructor(ILauncher::Probe(this));

    GetLayoutInflater((ILayoutInflater**)&mInflater);

    AutoPtr<IAppWidgetManagerHelper> helper;
    CAppWidgetManagerHelper::AcquireSingleton((IAppWidgetManagerHelper**)&helper);
    helper->GetInstance((IContext*)this, (IAppWidgetManager**)&mAppWidgetManager);
    mAppWidgetHost = new LauncherAppWidgetHost(ILauncher::Probe(this), APPWIDGET_HOST_ID);
Slogger::E("Launcher", "============================Launcher call mAppWidgetHost->StartListening");
    IAppWidgetHost::Probe(mAppWidgetHost)->StartListening();
    // If we are getting an onCreate, we can actually preempt onResume and unset mPaused here,
    // this also ensures that any synchronous binding below doesn't re-trigger another
    // LauncherModel load.
    mPaused = FALSE;

    // if (PROFILE_STARTUP) {
    //     android.os.Debug.startMethodTracing(
    //             Environment.getExternalStorageDirectory() + "/launcher");
    // }
Slogger::E("Launcher", "============================Launcher call CheckForLocaleChange");
    CheckForLocaleChange();
    SetContentView(R::layout::launcher);
Slogger::E("Launcher", "============================Launcher call SetupViews");
    SetupViews();
    ShowFirstRunWorkspaceCling();
    RegisterContentObservers();
    LockAllApps();
    mSavedState = savedInstanceState;
    RestoreState(mSavedState);

Slogger::E("Launcher", "============================Launcher add temporary by snow begin");
    AutoPtr<IView> clingDismisView = FindViewById(R::id::cling_dismiss_button);
    btnClingDismisView = IButton::Probe(clingDismisView);
    IView::Probe(btnClingDismisView)->SetOnClickListener(this);
Slogger::E("Launcher", "============================Launcher add temporary by snow end");

    // Update customization drawer _after_ restoring the states
    if (mAppsCustomizeContent != NULL) {
        AutoPtr<IArrayList> list;
        LauncherModel::GetSortedWidgetsAndShortcuts((IContext*)this,
                (IArrayList**)&list);
Int32 size;
list->GetSize(&size);
Slogger::E("Launcher", "============================Launcher call OnPackagesUpdated size=%d",size);
        mAppsCustomizeContent->OnPackagesUpdated(list);
    }
    // if (PROFILE_STARTUP) {
    //     android.os.Debug.stopMethodTracing();
    // }
    if (!mRestoring) {
        if (sPausedFromUserAction) {
            // If the user leaves launcher, then we should just load items asynchronously when
            // they return.
            mModel->StartLoader(TRUE, -1);
        }
        else {
            // We only load the page synchronously if the user rotates (or triggers a
            // configuration change) while launcher is in the foreground
            Int32 page;
            IPagedView::Probe(mWorkspace)->GetCurrentPage(&page);
Slogger::E("Launcher", "============================Launcher::OnCreate StartLoader page=%d", page);
            mModel->StartLoader(TRUE, page);
        }
    }
Slogger::E("Launcher", "============================Launcher::OnCreate 18");
    Boolean res;
    mModel->IsAllAppsLoaded(&res);
    if (!res) {
        AutoPtr<IViewParent> p;
        IView::Probe(mAppsCustomizeContent)->GetParent((IViewParent**)&p);
        AutoPtr<IViewGroup> appsCustomizeContentParent = IViewGroup::Probe(p);
        AutoPtr<IView> tmp;
        mInflater->Inflate(
                Elastos::Droid::Launcher2::R::layout::apps_customize_progressbar,
                appsCustomizeContentParent, (IView**)&tmp);
    }
Slogger::E("Launcher", "============================Launcher::OnCreate 19");
    // For handling default keys
    CSpannableStringBuilder::New((ISpannableStringBuilder**)&mDefaultKeySsb);
    AutoPtr<ISelection> helper2;
    CSelection::AcquireSingleton((ISelection**)&helper2);
    helper2->SetSelection(ISpannable::Probe(mDefaultKeySsb), 0);
Slogger::E("Launcher", "============================Launcher::OnCreate 20");
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IIntent::ACTION_CLOSE_SYSTEM_DIALOGS, (IIntentFilter**)&filter);
    AutoPtr<IIntent> tmp;
Slogger::E("Launcher", "============================Launcher::OnCreate 21");
    RegisterReceiver(mCloseSystemDialogsReceiver, filter, (IIntent**)&tmp);
Slogger::E("Launcher", "============================Launcher::OnCreate 22");
    UpdateGlobalIcons();
Slogger::E("Launcher", "============================Launcher::OnCreate return");
    // On large interfaces, we want the screen to auto-rotate based on the current orientation
    return UnlockScreenOrientation(TRUE);
}

ECode Launcher::OnUserLeaveHint()
{
    Activity::OnUserLeaveHint();
    sPausedFromUserAction = TRUE;
    return NOERROR;
}

void Launcher::UpdateGlobalIcons()
{
Slogger::E("Launcher", "=================UpdateGlobalIcons 1");
    Boolean searchVisible = FALSE;
    Boolean voiceVisible = FALSE;
    // If we have a saved version of these external icons, we load them up immediately
    Int32 coi = GetCurrentOrientationIndexForGlobalIcons();
    if ((*sGlobalSearchIcon)[coi] == NULL || (*sVoiceSearchIcon)[coi] == NULL ||
            (*sAppMarketIcon)[coi] == NULL) {
        UpdateAppMarketIcon();
        searchVisible = UpdateGlobalSearchIcon();
        voiceVisible = UpdateVoiceSearchIcon(searchVisible);
    }
Slogger::E("Launcher", "=================UpdateGlobalIcons 2");
    if ((*sGlobalSearchIcon)[coi] != NULL) {
        UpdateGlobalSearchIcon((*sGlobalSearchIcon)[coi]);
        searchVisible = TRUE;
    }
Slogger::E("Launcher", "=================UpdateGlobalIcons 3");
    if ((*sVoiceSearchIcon)[coi] != NULL) {
        UpdateVoiceSearchIcon((*sVoiceSearchIcon)[coi]);
        voiceVisible = TRUE;
    }
Slogger::E("Launcher", "=================UpdateGlobalIcons 4");
    if ((*sAppMarketIcon)[coi] != NULL) {
        UpdateAppMarketIcon((*sAppMarketIcon)[coi]);
    }
Slogger::E("Launcher", "=================UpdateGlobalIcons 5");
    if (mSearchDropTargetBar != NULL) {
        mSearchDropTargetBar->OnSearchPackagesChanged(searchVisible, voiceVisible);
    }
Slogger::E("Launcher", "=================UpdateGlobalIcons 6");
    return;
}

void Launcher::CheckForLocaleChange()
{
    if (sLocaleConfiguration == NULL) {
        AutoPtr<MyAsyncTask> task = new MyAsyncTask(this);
        AutoPtr<ArrayOf<IInterface*> > null;
        task->Execute(null);
        return;
    }

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<IConfiguration> configuration;
    resources->GetConfiguration((IConfiguration**)&configuration);

    String previousLocale = sLocaleConfiguration->mLocale;
    AutoPtr<ILocale> _locale;
    configuration->GetLocale((ILocale**)&_locale);
    String locale;
    _locale->ToString(&locale);
Logger::E("Launcher", "===========Launcher::CheckForLocaleChange previousLocale=%s, locale=%s", previousLocale.string(), locale.string());
    Int32 previousMcc = sLocaleConfiguration->mMcc;
    Int32 mcc;
    configuration->GetMcc(&mcc);
Logger::E("Launcher", "===========Launcher::CheckForLocaleChange previousMcc=%d, mcc=%d", previousMcc, mcc);
    Int32 previousMnc = sLocaleConfiguration->mMnc;
    Int32 mnc;
    configuration->GetMnc(&mnc);
Logger::E("Launcher", "===========Launcher::CheckForLocaleChange previousMnc=%d, mnc=%d", previousMnc, mnc);
    Boolean localeChanged = !locale.Equals(previousLocale) || mcc != previousMcc
            || mnc != previousMnc;
Logger::E("Launcher", "===========Launcher::CheckForLocaleChange localeChanged=%d", localeChanged);
    if (localeChanged) {
        sLocaleConfiguration->mLocale = locale;
        sLocaleConfiguration->mMcc = mcc;
        sLocaleConfiguration->mMnc = mnc;

        ((IconCache*)mIconCache.Get())->Flush();

        AutoPtr<LocaleConfiguration> localeConfiguration = sLocaleConfiguration;
        AutoPtr<MyThread> t = new MyThread(this, String("WriteLocaleConfiguration"),
                localeConfiguration);
        t->Start();
    }
    return;
}

void Launcher::ReadConfiguration(
    /* [in] */ IContext* context,
    /* [in] */ LocaleConfiguration* configuration)
{
    AutoPtr<IDataInputStream> in;
    // try {
    AutoPtr<IFileInputStream> fileInputStream;
    FAIL_GOTO(context->OpenFileInput(PREFERENCES, (IFileInputStream**)&fileInputStream), FINALLY)
    CDataInputStream::New(IInputStream::Probe(fileInputStream), (IDataInputStream**)&in);
    FAIL_GOTO(IDataInput::Probe(in)->ReadUTF(&(configuration->mLocale)), FINALLY)
    FAIL_GOTO(IDataInput::Probe(in)->ReadInt32(&(configuration->mMcc)), FINALLY)
    FAIL_GOTO(IDataInput::Probe(in)->ReadInt32(&(configuration->mMnc)), FINALLY)
    // } catch (FileNotFoundException e) {
    //     // Ignore
    // } catch (IOException e) {
    //     // Ignore
    // } finally {
FINALLY:
    if (in != NULL) {
        // try {
        ICloseable::Probe(in)->Close();
        // } catch (IOException e) {
        //     // Ignore
        // }
    }
    // }
    return;
}

void Launcher::WriteConfiguration(
    /* [in] */ IContext* context,
    /* [in] */ LocaleConfiguration* configuration)
{
    AutoPtr<IDataOutputStream> out;
    // try {
    AutoPtr<IFileOutputStream> fileOutputStream;
    ECode ec;
    FAIL_GOTO(ec = context->OpenFileOutput(PREFERENCES, MODE_PRIVATE,
            (IFileOutputStream**)&fileOutputStream), ERROR)
    CDataOutputStream::New(IOutputStream::Probe(fileOutputStream), (IDataOutputStream**)&out);
    FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(configuration->mLocale), ERROR)
    FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteInt32(configuration->mMcc), ERROR)
    FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteInt32(configuration->mMnc), ERROR)
    FAIL_GOTO(ec = IFlushable::Probe(out)->Flush(), ERROR)
    // } catch (FileNotFoundException e) {
    //     // Ignore
    // } catch (IOException e) {
    //     //noinspection ResultOfMethodCallIgnored
ERROR:
    if (ec == (ECode)E_IO_EXCEPTION) {
        AutoPtr<IFile> file;
        context->GetFileStreamPath(PREFERENCES, (IFile**)&file);
        file->Delete();
    }
    // } finally {
    if (out != NULL) {
        // try {
        ICloseable::Probe(out)->Close();
        // } catch (IOException e) {
            // Ignore
        // }
    }
    // }
    return;
}

ECode Launcher::GetDragLayer(
    /* [out] */ IDragLayer** layer)
{
    VALIDATE_NOT_NULL(layer);

    *layer = mDragLayer;
    REFCOUNT_ADD(*layer);
    return NOERROR;
}

ECode Launcher::IsDraggingEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // We prevent dragging when we are loading the workspace as it is possible to pick up a view
    // that is subsequently removed from the workspace in startBinding().
    Boolean res;
    mModel->IsLoadingWorkspace(&res);
    *result = !res;
    return NOERROR;
}

ECode Launcher::GetScreen(
    /* [out] */ Int32* screen)
{
    VALIDATE_NOT_NULL(screen);

    synchronized(sLock) {
        *screen = sScreen;
    }
    return NOERROR;
}

ECode Launcher::SetScreen(
    /* [in] */ Int32 screen)
{
    synchronized(sLock) {
        sScreen = screen;
    }
    return NOERROR;
}

Boolean Launcher::CompleteAdd(
    /* [in] */ PendingAddArguments* args)
{
    Boolean result = FALSE;
    switch (args->mRequestCode) {
        case REQUEST_PICK_APPLICATION:
            CompleteAddApplication(args->mIntent, args->mContainer, args->mScreen, args->mCellX,
                    args->mCellY);
            break;
        case REQUEST_PICK_SHORTCUT:
            ProcessShortcut(args->mIntent);
            break;
        case REQUEST_CREATE_SHORTCUT:
            CompleteAddShortcut(args->mIntent, args->mContainer, args->mScreen, args->mCellX,
                    args->mCellY);
            result = TRUE;
            break;
        case REQUEST_CREATE_APPWIDGET:
            Int32 appWidgetId;
            args->mIntent->GetInt32Extra(IAppWidgetManager::EXTRA_APPWIDGET_ID, -1, &appWidgetId);
            CompleteAddAppWidget(appWidgetId, args->mContainer, args->mScreen, NULL, NULL);
            result = TRUE;
            break;
        case REQUEST_PICK_WALLPAPER:
            // We just wanted the activity result here so we can clear mWaitingForResult
            break;
    }
    // Before adding this resetAddInfo(), after a shortcut was added to a workspace screen,
    // if you turned the screen off and then back while in All Apps, Launcher would not
    // return to the workspace. Clearing mAddInfo.container here fixes this issue
    ResetAddInfo();
    return result;
}

ECode Launcher::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    Int32 pendingAddWidgetId = mPendingAddWidgetId;
    mPendingAddWidgetId = -1;

    if (requestCode == REQUEST_BIND_APPWIDGET) {
        Int32 appWidgetId;
        if (data != NULL) {
            data->GetInt32Extra(IAppWidgetManager::EXTRA_APPWIDGET_ID, -1, &appWidgetId);
        }
        else {
            appWidgetId = -1;
        }
        if (resultCode == RESULT_CANCELED) {
            CompleteTwoStageWidgetDrop(RESULT_CANCELED, appWidgetId);
        }
        else if (resultCode == RESULT_OK) {
            AddAppWidgetImpl(appWidgetId, mPendingAddInfo, NULL, mPendingAddWidgetInfo);
        }
        return NOERROR;
    }
    Boolean delayExitSpringLoadedMode = FALSE;
    Boolean isWidgetDrop = (requestCode == REQUEST_PICK_APPWIDGET ||
            requestCode == REQUEST_CREATE_APPWIDGET);
    mWaitingForResult = FALSE;

    // We have special handling for widgets
    if (isWidgetDrop) {
        Int32 appWidgetId;
        Int32 widgetId;
        if (data != NULL) {
            data->GetInt32Extra(IAppWidgetManager::EXTRA_APPWIDGET_ID, -1, &widgetId);
        }
        else {
            widgetId = -1;
        }
        if (widgetId < 0) {
            appWidgetId = pendingAddWidgetId;
        }
        else {
            appWidgetId = widgetId;
        }

        if (appWidgetId < 0) {
            Slogger::E(TAG, "Error: appWidgetId (EXTRA_APPWIDGET_ID) was not returned from the \\"
                    "widget configuration activity.");
            CompleteTwoStageWidgetDrop(RESULT_CANCELED, appWidgetId);
        }
        else {
            CompleteTwoStageWidgetDrop(resultCode, appWidgetId);
        }
        return NOERROR;
    }

    // The pattern used here is that a user PICKs a specific application,
    // which, depending on the target, might need to CREATE the actual target.

    // For example, the user would PICK_SHORTCUT for "Music playlist", and we
    // launch over to the Music app to actually CREATE_SHORTCUT.
    if (resultCode == RESULT_OK && mPendingAddInfo->mContainer != IItemInfo::NO_ID) {
        AutoPtr<PendingAddArguments> args = new PendingAddArguments();
        args->mRequestCode = requestCode;
        args->mIntent = data;
        args->mContainer = mPendingAddInfo->mContainer;
        args->mScreen = mPendingAddInfo->mScreen;
        args->mCellX = mPendingAddInfo->mCellX;
        args->mCellY = mPendingAddInfo->mCellY;
        Boolean res;
        IsWorkspaceLocked(&res);
        if (res) {
            sPendingAddList->Add(TO_IINTERFACE(args));
        }
        else {
            delayExitSpringLoadedMode = CompleteAdd(args);
        }
    }
    mDragLayer->ClearAnimatedView();
    // Exit spring loaded mode if necessary after cancelling the configuration of a widget
    return ExitSpringLoadedDragModeDelayed((resultCode != RESULT_CANCELED), delayExitSpringLoadedMode,
            NULL);
}

void Launcher::CompleteTwoStageWidgetDrop(
    /* [in] */ Int32 resultCode,
    /* [in] */ Int32 appWidgetId)
{
    AutoPtr<IView> view;
    IViewGroup::Probe(mWorkspace)->GetChildAt(mPendingAddInfo->mScreen, (IView**)&view);
    AutoPtr<ICellLayout> cellLayout = ICellLayout::Probe(view);
    AutoPtr<IRunnable> onCompleteRunnable;
    Int32 animationType = 0;

    AutoPtr<IAppWidgetHostView> boundWidget;
    if (resultCode == RESULT_OK) {
        animationType = IWorkspace::COMPLETE_TWO_STAGE_WIDGET_DROP_ANIMATION;
        AutoPtr<IAppWidgetHostView> layout;
        IAppWidgetHost::Probe(mAppWidgetHost)->CreateView((IContext*)this, appWidgetId,
                mPendingAddWidgetInfo, (IAppWidgetHostView**)&layout);
        boundWidget = layout;
        onCompleteRunnable = new MyRunnable2(this, appWidgetId, layout, resultCode);
    }
    else if (resultCode == RESULT_CANCELED) {
        IAppWidgetHost::Probe(mAppWidgetHost)->DeleteAppWidgetId(appWidgetId);
        animationType = IWorkspace::CANCEL_TWO_STAGE_WIDGET_DROP_ANIMATION;
        onCompleteRunnable = new MyRunnable3(this, resultCode);
    }

    AutoPtr<IView> view2;
    mDragLayer->GetAnimatedView((IView**)&view2);
    if (view2 != NULL) {
        mWorkspace->AnimateWidgetDrop(IItemInfo::Probe(mPendingAddInfo), cellLayout,
                IDragView::Probe(view), onCompleteRunnable,
                animationType, IView::Probe(boundWidget), TRUE);
    }
    else {
        // The animated view may be null in the case of a rotation during widget configuration
        onCompleteRunnable->Run();
    }
    return;
}

ECode Launcher::OnStop()
{
    Activity::OnStop();
    return FirstFrameAnimatorHelper::SetIsVisible(FALSE);
}

ECode Launcher::OnStart()
{
    Activity::OnStart();
    return FirstFrameAnimatorHelper::SetIsVisible(TRUE);
}

ECode Launcher::OnResume()
{
    Int64 startTime = 0;
    if (DEBUG_RESUME_TIME) {
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        system->GetCurrentTimeMillis(&startTime);
    }
    Activity::OnResume();

    // Restore the previous launcher state
    if (mOnResumeState == Launcher_WORKSPACE) {
        ShowWorkspace(FALSE);
    }
    else if (mOnResumeState == Launcher_APPS_CUSTOMIZE) {
        ShowAllApps(FALSE);
    }
    mOnResumeState = Launcher_NONE;

    // Background was set to gradient in onPause(), restore to black if in all apps.
    SetWorkspaceBackground(mState == Launcher_WORKSPACE);

    // Process any items that were added while Launcher was away
    InstallShortcutReceiver::FlushInstallQueue(this);

    mPaused = FALSE;
    sPausedFromUserAction = FALSE;
    if (mRestoring || mOnResumeNeedsLoad) {
        mWorkspaceLoading = TRUE;
        mModel->StartLoader(TRUE, -1);
        mRestoring = FALSE;
        mOnResumeNeedsLoad = FALSE;
    }
    Int32 size;
    mOnResumeCallbacks->GetSize(&size);
    if (size > 0) {
        // We might have postponed some bind calls until onResume (see waitUntilResume) --
        // execute them here
        Int64 startTimeCallbacks = 0;
        if (DEBUG_RESUME_TIME) {
            AutoPtr<ISystem> system;
            CSystem::AcquireSingleton((ISystem**)&system);
            system->GetCurrentTimeMillis(&startTimeCallbacks);
        }

        if (mAppsCustomizeContent != NULL) {
            mAppsCustomizeContent->SetBulkBind(TRUE);
        }
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mOnResumeCallbacks->Get(i, (IInterface**)&obj);
            AutoPtr<IRunnable> r = IRunnable::Probe(obj);
            r->Run();
        }
        if (mAppsCustomizeContent != NULL) {
            mAppsCustomizeContent->SetBulkBind(FALSE);
        }
        mOnResumeCallbacks->Clear();
        // if (DEBUG_RESUME_TIME) {
        //     Slogger::D(TAG, "Time spent processing callbacks in onResume: " +
        //         (System.currentTimeMillis() - startTimeCallbacks));
        // }
    }

    // Reset the pressed state of icons that were locked in the press state while activities
    // were launching
    if (mWaitingForResume != NULL) {
        // Resets the previous workspace icon press state
        mWaitingForResume->SetStayPressed(FALSE);
    }
    if (mAppsCustomizeContent != NULL) {
        // Resets the previous all apps icon press state
        mAppsCustomizeContent->ResetDrawableState();
    }
    // It is possible that widgets can receive updates while launcher is not in the foreground.
    // Consequently, the widgets will be inflated in the orientation of the foreground activity
    // (framework issue). On resuming, we ensure that any widgets are inflated for the current
    // orientation.
    AutoPtr<IWorkspace> workspace;
    GetWorkspace((IWorkspace**)&workspace);
    workspace->ReinflateWidgetsIfNecessary();

    // Again, as with the above scenario, it's possible that one or more of the global icons
    // were updated in the wrong orientation.
    UpdateGlobalIcons();
    // if (DEBUG_RESUME_TIME) {
    //     Log.d(TAG, "Time spent in onResume: " + (System.currentTimeMillis() - startTime));
    // }
    return NOERROR;
}

ECode Launcher::OnPause()
{
    // NOTE: We want all transitions from launcher to act as if the wallpaper were enabled
    // to be consistent.  So re-enable the flag here, and we will re-disable it as necessary
    // when Launcher resumes and we are still in AllApps.
    UpdateWallpaperVisibility(TRUE);

    Activity::OnPause();
    mPaused = TRUE;
    mDragController->CancelDrag();
    return mDragController->ResetLastGestureUpTime();
}

ECode Launcher::OnRetainNonConfigurationInstance(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    // Flag the loader to stop early before switching
    mModel->StopLoader();
    if (mAppsCustomizeContent != NULL) {
        mAppsCustomizeContent->Surrender();
    }
    AutoPtr<IBoolean> b = CoreUtils::Convert(TRUE);
    *obj = TO_IINTERFACE(b);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

Boolean Launcher::AcceptFilter()
{
    AutoPtr<IInterface> obj;
    GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&obj);
    AutoPtr<IInputMethodManager> inputManager = IInputMethodManager::Probe(obj);
    Boolean res;
    inputManager->IsFullscreenMode(&res);
    return !res;
}

ECode Launcher::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 uniChar;
    event->GetUnicodeChar(&uniChar);
    Boolean handled;
    Activity::OnKeyDown(keyCode, event, &handled);
    Boolean res;
    Boolean isKeyNotWhitespace = uniChar > 0 && !Character::IsWhitespace(uniChar);

    if (!handled && AcceptFilter() && isKeyNotWhitespace) {
        AutoPtr<ITextKeyListenerHelper> listenerHelper;
        CTextKeyListenerHelper::AcquireSingleton((ITextKeyListenerHelper**)&listenerHelper);
        AutoPtr<ITextKeyListener> listener;
        listenerHelper->GetInstance((ITextKeyListener**)&listener);

        Boolean gotKey;
        IKeyListener::Probe(listener)->OnKeyDown(IView::Probe(mWorkspace),
                IEditable::Probe(mDefaultKeySsb), keyCode, event, &gotKey);
        Int32 length;
        if (gotKey && mDefaultKeySsb != NULL &&
                (((SpannableStringBuilder*)mDefaultKeySsb.Get())->GetLength(&length), length) > 0) {
            // something usable has been typed - start a search
            // the typed text will be retrieved and cleared by
            // showSearchDialog()
            // If there are multiple keystrokes before the search dialog takes focus,
            // onSearchRequested() will be called for every keystroke,
            // but it is idempotent, so it's fine.
            return OnSearchRequested(result);
        }
    }

    // Eat the long press event so the keyboard doesn't come up.

    if (keyCode == IKeyEvent::KEYCODE_MENU && (event->IsLongPress(&res), res)) {
        *result = TRUE;
        return NOERROR;
    }

    *result = handled;
    return NOERROR;
}

String Launcher::GetTypedText()
{
    String str;
    IObject::Probe(mDefaultKeySsb)->ToString(&str);
    return str;
}

void Launcher::ClearTypedText()
{
    ((SpannableStringBuilder*)mDefaultKeySsb.Get())->Clear();
    ((SpannableStringBuilder*)mDefaultKeySsb.Get())->ClearSpans();
    AutoPtr<ISelection> helper;
    CSelection::AcquireSingleton((ISelection**)&helper);
    helper->SetSelection(ISpannable::Probe(mDefaultKeySsb), 0);
}

LauncherState Launcher::OrdinalToState(
    /* [in] */ Int32 stateOrdinal)
{
    switch(stateOrdinal){
        case 0:
            return Launcher_NONE;
        case 1:
            return Launcher_WORKSPACE;
        case 2:
            return Launcher_APPS_CUSTOMIZE;
        case 3:
            return Launcher_APPS_CUSTOMIZE_SPRING_LOADED;
    }
    return Launcher_WORKSPACE;
}

Int32 Launcher::OrdinalOfState(
        /* [in] */ LauncherState state)
{
    switch(state){
        case Launcher_NONE:
            return 0;
        case Launcher_WORKSPACE:
            return 1;
        case Launcher_APPS_CUSTOMIZE:
            return 2;
        case Launcher_APPS_CUSTOMIZE_SPRING_LOADED:
            return 3;
    }
    return 1;
}

void Launcher::RestoreState(
    /* [in] */ IBundle* savedState)
{
    if (savedState == NULL) {
        return;
    }

    Int32 s;
    savedState->GetInt32(RUNTIME_STATE, OrdinalOfState(Launcher_WORKSPACE), &s);
    LauncherState state = OrdinalToState(s);
    if (state == Launcher_APPS_CUSTOMIZE) {
        mOnResumeState = Launcher_APPS_CUSTOMIZE;
    }

    Int32 currentScreen;
    savedState->GetInt32(RUNTIME_STATE_CURRENT_SCREEN, -1, &currentScreen);
    if (currentScreen > -1) {
        IPagedView::Probe(mWorkspace)->SetCurrentPage(currentScreen);
    }

    Int64 pendingAddContainer;
    savedState->GetInt64(RUNTIME_STATE_PENDING_ADD_CONTAINER, -1, &pendingAddContainer);
    Int32 pendingAddScreen;
    savedState->GetInt32(RUNTIME_STATE_PENDING_ADD_SCREEN, -1, &pendingAddScreen);

    if (pendingAddContainer != IItemInfo::NO_ID && pendingAddScreen > -1) {
        mPendingAddInfo->mContainer = pendingAddContainer;
        mPendingAddInfo->mScreen = pendingAddScreen;
        savedState->GetInt32(RUNTIME_STATE_PENDING_ADD_CELL_X, &(mPendingAddInfo->mCellX));
        savedState->GetInt32(RUNTIME_STATE_PENDING_ADD_CELL_Y, &(mPendingAddInfo->mCellY));
        savedState->GetInt32(RUNTIME_STATE_PENDING_ADD_SPAN_X, &(mPendingAddInfo->mSpanX));
        savedState->GetInt32(RUNTIME_STATE_PENDING_ADD_SPAN_Y, &(mPendingAddInfo->mSpanY));
        AutoPtr<IParcelable> p;
        savedState->GetParcelable(RUNTIME_STATE_PENDING_ADD_WIDGET_INFO, (IParcelable**)&p);
        mPendingAddWidgetInfo = IAppWidgetProviderInfo::Probe(p);
        savedState->GetInt32(RUNTIME_STATE_PENDING_ADD_WIDGET_ID, &mPendingAddWidgetId);
        mWaitingForResult = TRUE;
        mRestoring = TRUE;
    }


    Boolean renameFolder;
    savedState->GetBoolean(RUNTIME_STATE_PENDING_FOLDER_RENAME, FALSE, &renameFolder);
    if (renameFolder) {
        Int64 id;
        savedState->GetInt64(RUNTIME_STATE_PENDING_FOLDER_RENAME_ID, &id);
        mModel->GetFolderById(this, sFolders, id, (IFolderInfo**)&mFolderInfo);
        mRestoring = TRUE;
    }


    // Restore the AppsCustomize tab
    if (mAppsCustomizeTabHost != NULL) {
        String curTab;
        savedState->GetString(String("apps_customize_currentTab"), &curTab);
        if (!curTab.IsNull()) {
            AppsCustomizePagedViewContentType type;
            mAppsCustomizeTabHost->GetContentTypeForTabTag(curTab, &type);
            mAppsCustomizeTabHost->SetContentTypeImmediate(type);

            Int32 page;
            IPagedView::Probe(mAppsCustomizeContent)->GetCurrentPage(&page);
            IPagedView::Probe(mAppsCustomizeContent)->LoadAssociatedPages(page);
        }

        Int32 currentIndex;
        savedState->GetInt32(String("apps_customize_currentIndex"), &currentIndex);
        mAppsCustomizeContent->RestorePageForIndex(currentIndex);
    }
}

void Launcher::SetupViews()
{
    AutoPtr<IDragController> dragController = mDragController;
Slogger::E("Launcher", "============================Launcher::SetupViews 1");
    mLauncherView = FindViewById(R::id::launcher);
Slogger::E("Launcher", "============================Launcher::SetupViews 2 mLauncherView=%p",mLauncherView.Get());
    AutoPtr<IView> view = FindViewById(R::id::drag_layer);
Slogger::E("Launcher", "============================Launcher::SetupViews 2 view=%p",view.Get());
    mDragLayer = IDragLayer::Probe(view);
Slogger::E("Launcher", "============================Launcher::SetupViews 2 mDragLayer=%p",mDragLayer.Get());
    AutoPtr<IView> view2;
Slogger::E("Launcher", "============================Launcher::SetupViews 2 IView::Probe(mDragLayer)=%p",IView::Probe(mDragLayer));
    IView::Probe(mDragLayer)->FindViewById(R::id::workspace, (IView**)&view2);
Slogger::E("Launcher", "============================Launcher::SetupViews 3 view2=%p",view2.Get());
    mWorkspace = IWorkspace::Probe(view2);
Slogger::E("Launcher", "============================Launcher::SetupViews 4 mWorkspace=%p", mWorkspace.Get());
    mQsbDivider = FindViewById(R::id::qsb_divider);
Slogger::E("Launcher", "============================Launcher::SetupViews 4 mQsbDivider=%p=========================", mQsbDivider.Get());
    mDockDivider = FindViewById(R::id::dock_divider);
Slogger::E("Launcher", "============================Launcher::SetupViews 4 mDockDivider=%p", mDockDivider.Get());
    mLauncherView->SetSystemUiVisibility(IView::SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN);
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    resources->GetDrawable(Elastos::Droid::Launcher2::R::drawable::workspace_bg,
            (IDrawable**)&mWorkspaceBackgroundDrawable);
Slogger::E("Launcher", "============================Launcher Setup the draglayer");
    // Setup the drag layer
    mDragLayer->Setup(this, dragController);
Slogger::E("Launcher", "============================Launcher Setup the hotseat");
    // Setup the hotseat
    AutoPtr<IView> view3 = FindViewById(Elastos::Droid::Launcher2::R::id::hotseat);
    mHotseat = IHotseat::Probe(view3);
    if (mHotseat != NULL) {
        mHotseat->Setup(this);
    }
Slogger::E("Launcher", "============================Launcher SetupViews SetHapticFeedbackEnabled=false");
    // Setup the workspace
    IView::Probe(mWorkspace)->SetHapticFeedbackEnabled(FALSE);
Slogger::E("Launcher", "============================Launcher::SetupViews OnLongClickListener");
    IView::Probe(mWorkspace)->SetOnLongClickListener(IViewOnLongClickListener::Probe(this));
Slogger::E("Launcher", "============================Launcher::SetupViews 9");
    mWorkspace->Setup(dragController);
Slogger::E("Launcher", "============================Launcher::SetupViews 10");
    dragController->AddDragListener(IDragControllerDragListener::Probe(mWorkspace));
Slogger::E("Launcher", "============================Launcher::SetupViews 11");
    // Get the search/delete bar
    AutoPtr<IView> view4;
Slogger::E("Launcher", "============================Launcher::SetupViews 12");
    IView::Probe(mDragLayer)->FindViewById(Elastos::Droid::Launcher2::R::id::qsb_bar, (IView**)&view4);
    mSearchDropTargetBar = ISearchDropTargetBar::Probe(view4);
Slogger::E("Launcher", "============================Launcher::SetupViews mSearchDropTargetBar=%p",mSearchDropTargetBar.Get());
    // Setup AppsCustomize
    AutoPtr<IView> view5 = FindViewById(Elastos::Droid::Launcher2::R::id::apps_customize_pane);
Slogger::E("Launcher", "============================Launcher::SetupViews view5=%p",view5.Get());
    mAppsCustomizeTabHost = IAppsCustomizeTabHost::Probe(view5);
Slogger::E("Launcher", "============================Launcher::SetupViews mAppsCustomizeTabHost=%p",mAppsCustomizeTabHost.Get());
    AutoPtr<IView> view6;
    IView::Probe(mAppsCustomizeTabHost)->FindViewById(Elastos::Droid::Launcher2::R::id::apps_customize_pane_content,
            (IView**)&view6);
    mAppsCustomizeContent = IAppsCustomizePagedView::Probe(view6);
Slogger::E("Launcher", "============================Launcher::SetupViews mAppsCustomizeContent=%p",mAppsCustomizeContent.Get());
    mAppsCustomizeContent->Setup(ILauncher::Probe(this), dragController);
Slogger::E("Launcher", "============================Launcher::SetupViews 13");
    // Setup the drag controller (drop targets have to be added in reverse order in priority)
    dragController->SetDragScoller(IDragScroller::Probe(mWorkspace));
    dragController->SetScrollView(IView::Probe(mDragLayer));
    dragController->SetMoveTarget(IView::Probe(mWorkspace));
    dragController->AddDropTarget(IDropTarget::Probe(mWorkspace));
Slogger::E("Launcher", "============================Launcher::SetupViews 14");
    if (mSearchDropTargetBar != NULL) {
        mSearchDropTargetBar->Setup(this, dragController);
    }
Slogger::E("Launcher", "============================Launcher::SetupViews 15");
}

ECode Launcher::CreateShortcut(
    /* [in] */ IShortcutInfo* info,
    /* [out] */ IView** outview)
{
    VALIDATE_NOT_NULL(outview);

    Int32 page;
    IPagedView::Probe(mWorkspace)->GetCurrentPage(&page);
    AutoPtr<IView> view;
    IViewGroup::Probe(mWorkspace)->GetChildAt(page, (IView**)&view);
    return CreateShortcut(
            Elastos::Droid::Launcher2::R::layout::application,
            IViewGroup::Probe(view) , info, outview);
}

ECode Launcher::CreateShortcut(
    /* [in] */ Int32 layoutResId,
    /* [in] */ IViewGroup* parent,
    /* [in] */ IShortcutInfo* info,
    /* [out] */ IView** outview)
{
    VALIDATE_NOT_NULL(outview);

    AutoPtr<IView> view;
    mInflater->Inflate(layoutResId, parent, FALSE, (IView**)&view);
    AutoPtr<IBubbleTextView> favorite = IBubbleTextView::Probe(view);
    favorite->ApplyFromShortcutInfo(info, mIconCache);
    IView::Probe(favorite)->SetOnClickListener(this);
    *outview = IView::Probe(favorite);
    REFCOUNT_ADD(*outview);
    return NOERROR;
}

ECode Launcher::CompleteAddApplication(
    /* [in] */ IIntent* data,
    /* [in] */ Int64 container,
    /* [in] */ Int32 screen,
    /* [in] */ Int32 cellX,
    /* [in] */ Int32 cellY)
{
    AutoPtr<ArrayOf<Int32> > cellXY = mTmpAddItemCellCoordinates;
    AutoPtr<ICellLayout> layout;
    GetCellLayout(container, screen, (ICellLayout**)&layout);

    // First we check if we already know the exact location where we want to add this item.
    Boolean res;
    if (cellX >= 0 && cellY >= 0) {
        (*cellXY)[0] = cellX;
        (*cellXY)[1] = cellY;
    }
    else if (layout->FindCellForSpan(cellXY, 1, 1, &res), !res) {
        Boolean tmp;
        IsHotseatLayout(IView::Probe(layout), &tmp);
        ShowOutOfSpaceMessage(tmp);
        return NOERROR;
    }

    AutoPtr<IPackageManager> packageManager;
    GetPackageManager((IPackageManager**)&packageManager);
    AutoPtr<IUserHandle> user;
    Process::MyUserHandle((IUserHandle**)&user);
    AutoPtr<IShortcutInfo> info;
    mModel->GetShortcutInfo(packageManager, data, user, this, (IShortcutInfo**)&info);

    if (info != NULL) {
        // Necessary flags are added when the activity is launched via
        // LauncherApps
        ((ShortcutInfo*)info.Get())->SetActivity(data);
        ((ShortcutInfo*)info.Get())->mContainer = IItemInfo::NO_ID;
        Boolean res;
        IsWorkspaceLocked(&res);
        mWorkspace->AddApplicationShortcut(info, layout, container, screen, (*cellXY)[0], (*cellXY)[1],
                res, cellX, cellY);
    }
    else {
        Slogger::E(TAG, String("Couldn't find ActivityInfo for selected application: ") + TO_STR(data));
    }
    return NOERROR;
}

void Launcher::CompleteAddShortcut(
    /* [in] */ IIntent* data,
    /* [in] */ Int64 container,
    /* [in] */ Int32 screen,
    /* [in] */ Int32 cellX,
    /* [in] */ Int32 cellY)
{
    AutoPtr<ArrayOf<Int32> > cellXY = mTmpAddItemCellCoordinates;
    AutoPtr<ArrayOf<Int32> > touchXY = mPendingAddInfo->mDropPos;
    AutoPtr<ICellLayout> layout;
    GetCellLayout(container, screen, (ICellLayout**)&layout);

    Boolean foundCellSpan = FALSE;

    AutoPtr<IShortcutInfo> info;
    mModel->InfoFromShortcutIntent(this, data, NULL, (IShortcutInfo**)&info);
    if (info == NULL) {
        return;
    }
    AutoPtr<IView> view;
    CreateShortcut(info, (IView**)&view);

    // First we check if we already know the exact location where we want to add this item.
    if (cellX >= 0 && cellY >= 0) {
        (*cellXY)[0] = cellX;
        (*cellXY)[1] = cellY;
        foundCellSpan = TRUE;

        // If appropriate, either create a folder or add to an existing folder
        Boolean res;
        mWorkspace->CreateUserFolderIfNecessary(view, container, layout, cellXY, 0,
                TRUE, NULL, NULL, &res);
        if (res) {
            return;
        }
        AutoPtr<DragObject> dragObject = new DragObject();
        dragObject->mDragInfo = info;
        mWorkspace->AddToExistingFolderIfNecessary(view, layout, cellXY, 0, dragObject,
                TRUE, &res);
        if (res) {
            return;
        }
    }
    else if (touchXY != NULL) {
        // when dragging and dropping, just find the closest free spot
        AutoPtr<ArrayOf<Int32> > result;
        layout->FindNearestVacantArea((*touchXY)[0], (*touchXY)[1], 1, 1, cellXY, (ArrayOf<Int32>**)&result);
        foundCellSpan = (result != NULL);
    }
    else {
        layout->FindCellForSpan(cellXY, 1, 1, &foundCellSpan);
    }

    if (!foundCellSpan) {
        Boolean res;
        IsHotseatLayout(IView::Probe(layout), &res);
        ShowOutOfSpaceMessage(res);
        return;
    }

    LauncherModel::AddItemToDatabase((IContext*)this, (ItemInfo*)IItemInfo::Probe(info),
            container, screen, (*cellXY)[0], (*cellXY)[1], FALSE);

    if (!mRestoring) {
        Boolean res;
        IsWorkspaceLocked(&res);
        mWorkspace->AddInScreen(view, container, screen, (*cellXY)[0], (*cellXY)[1], 1, 1, res);
    }
    return;
}

ECode Launcher::GetSpanForWidget(
    /* [in] */ IContext* context,
    /* [in] */ IComponentName* component,
    /* [in] */ Int32 minWidth,
    /* [in] */ Int32 minHeight,
    /* [out, callee] */ ArrayOf<Int32>** array)
{
    VALIDATE_NOT_NULL(array);

    AutoPtr<IRect> padding = AppWidgetHostView::GetDefaultPaddingForWidget(context,
            component, NULL);
    // We want to account for the extra amount of padding that we are adding to the widget
    // to ensure that it gets the full amount of space that it has requested
    Int32 left, top, right, bottom;
    padding->Get(&left, &top, &right, &bottom);
    Int32 requiredWidth = minWidth + left + right;
    Int32 requiredHeight = minHeight + top + bottom;
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    return CellLayout::RectToCell(resources, requiredWidth, requiredHeight, NULL, array);
}

ECode Launcher::GetSpanForWidget(
    /* [in] */ IContext* context,
    /* [in] */ IAppWidgetProviderInfo* info,
    /* [out, callee] */ ArrayOf<Int32>** array)
{
    VALIDATE_NOT_NULL(array);

    AutoPtr<IComponentName> name;
    info->GetProvider((IComponentName**)&name);
    Int32 width, height;
    info->GetMinWidth(&width);
    info->GetMinHeight(&height);
    return GetSpanForWidget(context, name, width, height, array);
}

ECode Launcher::GetMinSpanForWidget(
    /* [in] */ IContext* context,
    /* [in] */ IAppWidgetProviderInfo* info,
    /* [out, callee] */ ArrayOf<Int32>** array)
{
    VALIDATE_NOT_NULL(array);

    AutoPtr<IComponentName> name;
    info->GetProvider((IComponentName**)&name);
    Int32 width, height;
    info->GetMinResizeWidth(&width);
    info->GetMinResizeHeight(&height);
    return GetSpanForWidget(context, name, width, height, array);
}

ECode Launcher::GetSpanForWidget(
    /* [in] */ IContext* context,
    /* [in] */ IPendingAddWidgetInfo* info,
    /* [out, callee] */ ArrayOf<Int32>** array)
{
    VALIDATE_NOT_NULL(array);

    PendingAddWidgetInfo* _info = (PendingAddWidgetInfo*)info;
    Int32 width, height;
    _info->mInfo->GetMinWidth(&width);
    _info->mInfo->GetMinHeight(&height);
    return GetSpanForWidget(context, _info->mComponentName, width, height, array);
}

ECode Launcher::GetMinSpanForWidget(
    /* [in] */ IContext* context,
    /* [in] */ IPendingAddWidgetInfo* info,
    /* [out, callee] */ ArrayOf<Int32>** array)
{
    VALIDATE_NOT_NULL(array);

    PendingAddWidgetInfo* _info = (PendingAddWidgetInfo*)info;
    Int32 width;
    _info->mInfo->GetMinResizeWidth(&width);
    Int32 height;
    _info->mInfo->GetMinResizeHeight(&height);
    return GetSpanForWidget(context, _info->mComponentName, width, height, array);
}

void Launcher::CompleteAddAppWidget(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ Int64 container,
    /* [in] */ Int32 screen,
    /* [in] */ IAppWidgetHostView* hostView,
    /* [in] */ IAppWidgetProviderInfo* appWidgetInfo)
{
    if (appWidgetInfo == NULL) {
        mAppWidgetManager->GetAppWidgetInfo(appWidgetId, (IAppWidgetProviderInfo**)&appWidgetInfo);
    }

    // Calculate the grid spans needed to fit this widget
    AutoPtr<ICellLayout> layout;
    GetCellLayout(container, screen, (ICellLayout**)&layout);

    AutoPtr<ArrayOf<Int32> > minSpanXY;
    GetMinSpanForWidget(this, appWidgetInfo, (ArrayOf<Int32>**)&minSpanXY);
    AutoPtr<ArrayOf<Int32> > spanXY;
    GetSpanForWidget(this, appWidgetInfo, (ArrayOf<Int32>**)&spanXY);

    // Try finding open space on Launcher screen
    // We have saved the position to which the widget was dragged-- this really only matters
    // if we are placing widgets on a "spring-loaded" screen
    AutoPtr<ArrayOf<Int32> > cellXY = mTmpAddItemCellCoordinates;
    AutoPtr<ArrayOf<Int32> > touchXY = mPendingAddInfo->mDropPos;
    AutoPtr<ArrayOf<Int32> > finalSpan = ArrayOf<Int32>::Alloc(2);
    Boolean foundCellSpan = FALSE;
    if (mPendingAddInfo->mCellX >= 0 && mPendingAddInfo->mCellY >= 0) {
        (*cellXY)[0] = mPendingAddInfo->mCellX;
        (*cellXY)[1] = mPendingAddInfo->mCellY;
        (*spanXY)[0] = mPendingAddInfo->mSpanX;
        (*spanXY)[1] = mPendingAddInfo->mSpanY;
        foundCellSpan = TRUE;
    }
    else if (touchXY != NULL) {
        // when dragging and dropping, just find the closest free spot
        AutoPtr<ArrayOf<Int32> > result;
        layout->FindNearestVacantArea(
            (*touchXY)[0], (*touchXY)[1], (*minSpanXY)[0], (*minSpanXY)[1], (*spanXY)[0],
            (*spanXY)[1], cellXY, finalSpan, (ArrayOf<Int32>**)&result);
        (*spanXY)[0] = (*finalSpan)[0];
        (*spanXY)[1] = (*finalSpan)[1];
        foundCellSpan = (result != NULL);
    }
    else {
        layout->FindCellForSpan(cellXY, (*minSpanXY)[0], (*minSpanXY)[1], &foundCellSpan);
    }

    if (!foundCellSpan) {
        if (appWidgetId != -1) {
            // Deleting an app widget ID is a void call but writes to disk before returning
            // to the caller...
            AutoPtr<IThread> t = new MyThread2(this, String("deleteAppWidgetId"), appWidgetId);
            t->Start();
        }
        Boolean res;
        IsHotseatLayout(IView::Probe(layout), &res);
        ShowOutOfSpaceMessage(res);
        return;
    }

    // Build Launcher-specific widget info and save to database
    AutoPtr<IComponentName> name;
    appWidgetInfo->GetProvider((IComponentName**)&name);
    AutoPtr<LauncherAppWidgetInfo> launcherInfo = new LauncherAppWidgetInfo();
    launcherInfo->constructor(appWidgetId, name);
    launcherInfo->mSpanX = (*spanXY)[0];
    launcherInfo->mSpanY = (*spanXY)[1];
    launcherInfo->mMinSpanX = mPendingAddInfo->mMinSpanX;
    launcherInfo->mMinSpanY = mPendingAddInfo->mMinSpanY;
    appWidgetInfo->GetProfile((IUserHandle**)&(launcherInfo->mUser));

    LauncherModel::AddItemToDatabase(this, launcherInfo,
            container, screen, (*cellXY)[0], (*cellXY)[1], FALSE);

    if (!mRestoring) {
        if (hostView == NULL) {
            // Perform actual inflation because we're live
            IAppWidgetHost::Probe(mAppWidgetHost)->CreateView((IContext*)this,
                appWidgetId, appWidgetInfo, (IAppWidgetHostView**)&(launcherInfo->mHostView));
            launcherInfo->mHostView->SetAppWidget(appWidgetId, appWidgetInfo);
        }
        else {
            // The AppWidgetHostView has already been inflated and instantiated
            launcherInfo->mHostView = hostView;
        }

        IView::Probe(launcherInfo->mHostView)->SetTag(TO_IINTERFACE(launcherInfo));
        IView::Probe(launcherInfo->mHostView)->SetVisibility(IView::VISIBLE);
        launcherInfo->NotifyWidgetSizeChanged(this);

        Boolean res;
        IsWorkspaceLocked(&res);
        mWorkspace->AddInScreen(IView::Probe(launcherInfo->mHostView), container, screen,
                (*cellXY)[0], (*cellXY)[1], launcherInfo->mSpanX, launcherInfo->mSpanY, res);

        AddWidgetToAutoAdvanceIfNeeded(IView::Probe(launcherInfo->mHostView), appWidgetInfo);
    }
    ResetAddInfo();
    return;
}

ECode Launcher::OnAttachedToWindow()
{
    Activity::OnAttachedToWindow();

    // Listen for broadcasts related to user-presence
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_SCREEN_OFF);
    filter->AddAction(IIntent::ACTION_USER_PRESENT);
    filter->AddAction(IIntent::ACTION_MANAGED_PROFILE_ADDED);
    filter->AddAction(IIntent::ACTION_MANAGED_PROFILE_REMOVED);

    AutoPtr<IIntent> tmp;
    RegisterReceiver(mReceiver, filter, (IIntent**)&tmp);
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    AutoPtr<IView> view;
    window->GetDecorView((IView**)&view);
    FirstFrameAnimatorHelper::InitializeDrawListener(view);
    mAttached = TRUE;
    mVisible = TRUE;
    return NOERROR;
}

ECode Launcher::OnDetachedFromWindow()
{
    Activity::OnDetachedFromWindow();
    mVisible = FALSE;

    if (mAttached) {
        UnregisterReceiver(mReceiver);
        mAttached = FALSE;
    }
    UpdateRunning();
    return NOERROR;
}

ECode Launcher::OnWindowVisibilityChanged(
    /* [in] */ Int32 visibility)
{
    mVisible = visibility == IView::VISIBLE;
    UpdateRunning();
    // The following code used to be in onResume, but it turns out onResume is called when
    // you're in All Apps and click home to go to the workspace. onWindowVisibilityChanged
    // is a more appropriate event to handle
    if (mVisible) {
        mAppsCustomizeTabHost->OnWindowVisible();
        if (!mWorkspaceLoading) {
            AutoPtr<IViewTreeObserver> observer;
            IView::Probe(mWorkspace)->GetViewTreeObserver((IViewTreeObserver**)&observer);
            // We want to let Launcher draw itself at least once before we force it to build
            // layers on all the workspace pages, so that transitioning to Launcher from other
            // apps is nice and speedy.
            AutoPtr<IOnDrawListener> lis = new MyOnDrawListener(this);
            observer->AddOnDrawListener(lis);
        }
        // When Launcher comes back to foreground, a different Activity might be responsible for
        // the app market intent, so refresh the icon
        UpdateAppMarketIcon();
        ClearTypedText();
    }
    return NOERROR;
}

void Launcher::SendAdvanceMessage(
    /* [in] */ Int64 delay)
{
    mHandler->RemoveMessages(ADVANCE_MSG);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(ADVANCE_MSG, (IMessage**)&msg);
    Boolean res;
    mHandler->SendMessageDelayed(msg, delay, &res);
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mAutoAdvanceSentTime);
}

void Launcher::UpdateRunning()
{
    Boolean res;
    Boolean autoAdvanceRunning = mVisible && mUserPresent && (mWidgetsToAdvance->IsEmpty(&res), !res);
    if (autoAdvanceRunning != mAutoAdvanceRunning) {
        mAutoAdvanceRunning = autoAdvanceRunning;
        if (autoAdvanceRunning) {
            Int64 delay = mAutoAdvanceTimeLeft == -1 ? mAdvanceInterval : mAutoAdvanceTimeLeft;
            SendAdvanceMessage(delay);
        }
        else {
            Boolean res;
            mWidgetsToAdvance->IsEmpty(&res);
            if (!res) {
                AutoPtr<ISystem> system;
                CSystem::AcquireSingleton((ISystem**)&system);
                Int64 ctime;
                system->GetCurrentTimeMillis(&ctime);
                mAutoAdvanceTimeLeft = Elastos::Core::Math::Max((Int64)0, mAdvanceInterval -
                        (ctime - mAutoAdvanceSentTime));
            }
            mHandler->RemoveMessages(ADVANCE_MSG);
            mHandler->RemoveMessages(0); // Remove messages sent using postDelayed()
        }
    }
}

ECode Launcher::AddWidgetToAutoAdvanceIfNeeded(
    /* [in] */ IView* hostView,
    /* [in] */ IAppWidgetProviderInfo* appWidgetInfo)
{
    if (appWidgetInfo == NULL) return NOERROR;
    Int32 id;
    appWidgetInfo->GetAutoAdvanceViewId(&id);
    if (id == -1) return NOERROR;
    AutoPtr<IView> v;
    hostView->FindViewById(id, (IView**)&v);
    if (IAdvanceable::Probe(v) != NULL) {
        mWidgetsToAdvance->Put(TO_IINTERFACE(hostView), TO_IINTERFACE(appWidgetInfo));
        IAdvanceable::Probe(v)->FyiWillBeAdvancedByHostKThx();
        UpdateRunning();
    }
    return NOERROR;
}

ECode Launcher::RemoveWidgetToAutoAdvance(
    /* [in] */ IView* hostView)
{
    Boolean res;
    mWidgetsToAdvance->ContainsKey(hostView, &res);
    if (res) {
        mWidgetsToAdvance->Remove(hostView);
        UpdateRunning();
    }
    return NOERROR;
}

ECode Launcher::RemoveAppWidget(
    /* [in] */ ILauncherAppWidgetInfo* launcherInfo)
{
    LauncherAppWidgetInfo* info = (LauncherAppWidgetInfo*)launcherInfo;
    RemoveWidgetToAutoAdvance(IView::Probe(info->mHostView));
    info->mHostView = NULL;
    return NOERROR;
}

ECode Launcher::ShowOutOfSpaceMessage(
    /* [in] */ Boolean isHotseatLayout)
{
    Int32 strId = (isHotseatLayout ?
            Elastos::Droid::Launcher2::R::string::hotseat_out_of_space :
            Elastos::Droid::Launcher2::R::string::out_of_space);
    String str;
    GetString(strId, &str);

    AutoPtr<IToastHelper> helper;
    CToastHelper::AcquireSingleton((IToastHelper**)&helper);
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
    AutoPtr<IToast> toast;
    helper->MakeText((IContext*)this, cchar, IToast::LENGTH_SHORT, (IToast**)&toast);

    return toast->Show();
}

ECode Launcher::GetAppWidgetHost(
    /* [out] */ ILauncherAppWidgetHost** host)
{
    VALIDATE_NOT_NULL(host);

    *host = mAppWidgetHost;
    REFCOUNT_ADD(*host);
    return NOERROR;
}

ECode Launcher::GetModel(
    /* [out] */ ILauncherModel** mode)
{
    VALIDATE_NOT_NULL(mode);

    *mode = mModel;
    REFCOUNT_ADD(*mode);
    return NOERROR;
}

ECode Launcher::CloseSystemDialogs()
{
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    window->CloseAllPanels();

    // Whatever we were doing is hereby canceled.
    mWaitingForResult = FALSE;
    return NOERROR;
}

ECode Launcher::OnNewIntent(
    /* [in] */ IIntent* intent)
{
    Int64 startTime = 0;
    if (DEBUG_RESUME_TIME) {
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        system->GetCurrentTimeMillis(&startTime);
    }
    Activity::OnNewIntent(intent);

    // Close the menu
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_MAIN.Equals(action)) {
        // also will cancel mWaitingForResult.
        CloseSystemDialogs();

        Int32 flags;
        intent->GetFlags(&flags);
        Boolean alreadyOnHome =
                ((flags & IIntent::FLAG_ACTIVITY_BROUGHT_TO_FRONT)
                    != IIntent::FLAG_ACTIVITY_BROUGHT_TO_FRONT);

        AutoPtr<IRunnable> processIntent = new MyRunnable6(this, alreadyOnHome);
        Boolean res;
        if (alreadyOnHome && (IView::Probe(mWorkspace)->HasWindowFocus(&res), !res)) {
            // Delay processing of the intent to allow the status bar animation to finish
            // first in order to avoid janky animations.
            IView::Probe(mWorkspace)->PostDelayed(processIntent, 350, &res);
        }
        else {
            // Process the intent immediately.
            processIntent->Run();
        }

    }
    if (DEBUG_RESUME_TIME) {
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        Int64 ctime;
        system->GetCurrentTimeMillis(&ctime);
        Slogger::D(TAG, "Time spent in onNewIntent: " + (ctime - startTime));
    }
    return NOERROR;
}

ECode Launcher::OnRestoreInstanceState(
    /* [in] */ IBundle* state)
{
    Activity::OnRestoreInstanceState(state);
    Int32 size;
    mSynchronouslyBoundPages->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mSynchronouslyBoundPages->Get(i, (IInterface**)&obj);
        AutoPtr<IInteger32> intObj = IInteger32::Probe(obj);
        Int32 page;
        intObj->GetValue(&page);

        mWorkspace->RestoreInstanceStateForChild(page);
    }
    return NOERROR;
}

ECode Launcher::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    Int32 page;
    IPagedView::Probe(mWorkspace)->GetNextPage(&page);
    outState->PutInt32(RUNTIME_STATE_CURRENT_SCREEN, page);
    Activity::OnSaveInstanceState(outState);

    Int32 ordinal = OrdinalOfState(mState);
    outState->PutInt32(RUNTIME_STATE, ordinal);
    // We close any open folder since it will not be re-opened, and we need to make sure
    // this state is reflected.
    CloseFolder();

    if (mPendingAddInfo->mContainer != IItemInfo::NO_ID && mPendingAddInfo->mScreen > -1 &&
            mWaitingForResult) {
        outState->PutInt64(RUNTIME_STATE_PENDING_ADD_CONTAINER, mPendingAddInfo->mContainer);
        outState->PutInt32(RUNTIME_STATE_PENDING_ADD_SCREEN, mPendingAddInfo->mScreen);
        outState->PutInt32(RUNTIME_STATE_PENDING_ADD_CELL_X, mPendingAddInfo->mCellX);
        outState->PutInt32(RUNTIME_STATE_PENDING_ADD_CELL_Y, mPendingAddInfo->mCellY);
        outState->PutInt32(RUNTIME_STATE_PENDING_ADD_SPAN_X, mPendingAddInfo->mSpanX);
        outState->PutInt32(RUNTIME_STATE_PENDING_ADD_SPAN_Y, mPendingAddInfo->mSpanY);
        outState->PutParcelable(RUNTIME_STATE_PENDING_ADD_WIDGET_INFO, IParcelable::Probe(mPendingAddWidgetInfo));
        outState->PutInt32(RUNTIME_STATE_PENDING_ADD_WIDGET_ID, mPendingAddWidgetId);
    }

    if (mFolderInfo != NULL && mWaitingForResult) {
        outState->PutBoolean(RUNTIME_STATE_PENDING_FOLDER_RENAME, TRUE);
        outState->PutInt64(RUNTIME_STATE_PENDING_FOLDER_RENAME_ID, ((FolderInfo*)mFolderInfo.Get())->mId);
    }

    // Save the current AppsCustomize tab
    if (mAppsCustomizeTabHost != NULL) {
        String currentTabTag;
        ITabHost::Probe(mAppsCustomizeTabHost)->GetCurrentTabTag(&currentTabTag);
        if (!currentTabTag.IsNull()) {
            outState->PutString(String("apps_customize_currentTab"), currentTabTag);
        }
        Int32 currentIndex;
        mAppsCustomizeContent->GetSaveInstanceStateIndex(&currentIndex);
        outState->PutInt32(String("apps_customize_currentIndex"), currentIndex);
    }
    return NOERROR;
}

ECode Launcher::OnDestroy()
{
    Activity::OnDestroy();

    // Remove all pending runnables
    mHandler->RemoveMessages(ADVANCE_MSG);
    mHandler->RemoveMessages(0);
    Boolean res;
    IView::Probe(mWorkspace)->RemoveCallbacks(mBuildLayersRunnable, &res);

    // Stop callbacks from LauncherModel
    AutoPtr<IApplication> appl;
    GetApplication((IApplication**)&appl);
    AutoPtr<ILauncherApplication> app = ILauncherApplication::Probe(appl);
    mModel->StopLoader();
    AutoPtr<ILauncherModel> mode;
    app->SetLauncher(NULL, (ILauncherModel**)&mode);

    //try {
    if ((ECode)E_NULL_POINTER_EXCEPTION == IAppWidgetHost::Probe(mAppWidgetHost)->StopListening()){
        Slogger::W(TAG, "problem while stopping AppWidgetHost during Launcher destruction");
    }
    //} catch (NullPointerException ex) {
        //Log.w(TAG, "problem while stopping AppWidgetHost during Launcher destruction", ex);
    //}
    mAppWidgetHost = NULL;

    mWidgetsToAdvance->Clear();

    AutoPtr<ITextKeyListenerHelper> listenerHelper;
    CTextKeyListenerHelper::AcquireSingleton((ITextKeyListenerHelper**)&listenerHelper);
    AutoPtr<ITextKeyListener> listener;
    listenerHelper->GetInstance((ITextKeyListener**)&listener);
    listener->ReleaseListener();

    // Disconnect any of the callbacks and drawables associated with ItemInfos on the workspace
    // to prevent leaking Launcher activities on orientation change.
    if (mModel != NULL) {
        mModel->UnbindItemInfosAndClearQueuedBindRunnables();
    }

    AutoPtr<IContentResolver> resolver;
    GetContentResolver((IContentResolver**)&resolver);
    resolver->UnregisterContentObserver(mWidgetObserver);
    UnregisterReceiver(mCloseSystemDialogsReceiver);

    mDragLayer->ClearAllResizeFrames();
    AutoPtr<IViewParent> p;
    IView::Probe(mWorkspace)->GetParent((IViewParent**)&p);
    IViewGroup::Probe(p)->RemoveAllViews();

    IViewGroup::Probe(mWorkspace)->RemoveAllViews();
    mWorkspace = NULL;
    mDragController = NULL;

    LauncherAnimUtils::OnDestroyActivity();
    return NOERROR;
}

ECode Launcher::GetDragController(
    /* [out] */ IDragController** controller)
{
    VALIDATE_NOT_NULL(controller);

    *controller = mDragController;
    REFCOUNT_ADD(*controller);
    return NOERROR;
}

ECode Launcher::StartActivityForResult(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 requestCode)
{
    if (requestCode >= 0) {
        mWaitingForResult = TRUE;
    }
    return Activity::StartActivityForResult(intent, requestCode);
}

ECode Launcher::StartSearch(
    /* [in] */ const String& initialQuery,
    /* [in] */ Boolean selectInitialQuery,
    /* [in] */ IBundle* appSearchData,
    /* [in] */ Boolean globalSearch)
{
    ShowWorkspace(TRUE);

    String _initialQuery;
    if (initialQuery.IsNull()) {
        // Use any text typed in the launcher as the initial query
        _initialQuery = GetTypedText();
    }
    if (appSearchData == NULL) {
        CBundle::New((IBundle**)&appSearchData);
        //import com.android.common.Search;
        //public final static String SOURCE = "source";
        appSearchData->PutString(/*Search.SOURCE*/ String("source"), String("launcher-search"));
    }
    AutoPtr<IRect> sourceBounds;
    CRect::New((IRect**)&sourceBounds);
    if (mSearchDropTargetBar != NULL) {
        mSearchDropTargetBar->GetSearchBarBounds((IRect**)&sourceBounds);
    }

    return StartGlobalSearch(_initialQuery, selectInitialQuery, appSearchData, sourceBounds);
}

ECode Launcher::StartGlobalSearch(
    /* [in] */ const String& initialQuery,
    /* [in] */ Boolean selectInitialQuery,
    /* [in] */ IBundle* appSearchData,
    /* [in] */ IRect* sourceBounds)
{
    AutoPtr<IInterface> obj;
    GetSystemService(IContext::SEARCH_SERVICE, (IInterface**)&obj);
    AutoPtr<ISearchManager> searchManager = ISearchManager::Probe(obj);
    AutoPtr<IComponentName> globalSearchActivity;
    searchManager->GetGlobalSearchActivity((IComponentName**)&globalSearchActivity);
    if (globalSearchActivity == NULL) {
        Slogger::W(TAG, "No global search activity found.");
        return NOERROR;
    }
    AutoPtr<IIntent> intent;
    CIntent::New(ISearchManager::INTENT_ACTION_GLOBAL_SEARCH, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    intent->SetComponent(globalSearchActivity);
    // Make sure that we have a Bundle to put source in
    if (appSearchData == NULL) {
        CBundle::New((IBundle**)&appSearchData);
    }
    else {
        CBundle::New(appSearchData, (IBundle**)&appSearchData);
    }
    // Set source to package name of app that starts global search, if not set already.
    Boolean res;
    appSearchData->ContainsKey(String("source"), &res);
    if (!res) {
        String pname;
        GetPackageName(&pname);
        appSearchData->PutString(String("source"), pname);
    }
    intent->PutExtra(ISearchManager::APP_DATA, appSearchData);
    if (!TextUtils::IsEmpty(initialQuery)) {
        intent->PutExtra(ISearchManager::QUERY, initialQuery);
    }
    if (selectInitialQuery) {
        intent->PutExtra(ISearchManager::EXTRA_SELECT_QUERY, selectInitialQuery);
    }
    intent->SetSourceBounds(sourceBounds);
    //try {
    if ((ECode)E_ACTIVITY_NOT_FOUND_EXCEPTION ==
            Activity::StartActivity(intent)) {
        Slogger::E(TAG, String("Global search activity not found: ") + TO_STR(globalSearchActivity));
    }
    //} catch (ActivityNotFoundException ex) {
        //Log.e(TAG, "Global search activity not found: " + globalSearchActivity);
    //}
    return NOERROR;
}

ECode Launcher::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean res;
    IsWorkspaceLocked(&res);
    if (res) {
        *result = FALSE;
        return NOERROR;
    }

    Activity::OnCreateOptionsMenu(menu, &res);

    AutoPtr<IIntent> manageApps;
    CIntent::New(ISettings::ACTION_MANAGE_ALL_APPLICATIONS_SETTINGS, (IIntent**)&manageApps);
    manageApps->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
            | IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
    AutoPtr<IIntent> settings;
    CIntent::New(ISettings::ACTION_SETTINGS, (IIntent**)&settings);
    settings->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
            | IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED);
    String helpUrl;
    GetString(Elastos::Droid::Launcher2::R::string::help_url, &helpUrl);

    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> uri;
    helper->Parse(helpUrl, (IUri**)&uri);
    AutoPtr<IIntent> help;
    CIntent::New(IIntent::ACTION_VIEW, uri, (IIntent**)&help);
    help->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
            | IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);

    AutoPtr<IMenuItem> item;
    menu->Add(MENU_GROUP_WALLPAPER, MENU_WALLPAPER_SETTINGS,
            0, Elastos::Droid::Launcher2::R::string::menu_wallpaper, (IMenuItem**)&item);
    item->SetIcon(Elastos::Droid::R::drawable::ic_menu_gallery);
    item->SetAlphabeticShortcut('W');

    AutoPtr<IMenuItem> item2;
    menu->Add(0, MENU_MANAGE_APPS, 0,
            Elastos::Droid::Launcher2::R::string::menu_manage_apps, (IMenuItem**)&item2);
    item2->SetIcon(Elastos::Droid::R::drawable::ic_menu_manage);
    item2->SetIntent(manageApps);
    item2->SetAlphabeticShortcut('M');

    AutoPtr<IMenuItem> item3;
    menu->Add(0, MENU_SYSTEM_SETTINGS, 0,
            Elastos::Droid::Launcher2::R::string::menu_settings, (IMenuItem**)&item3);
    item3->SetIcon(Elastos::Droid::R::drawable::ic_menu_preferences);
    item3->SetIntent(settings);
    item3->SetAlphabeticShortcut('P');

    if (!helpUrl.IsEmpty()) {
        AutoPtr<IMenuItem> item4;
        menu->Add(0, MENU_HELP, 0,
                Elastos::Droid::Launcher2::R::string::menu_help, (IMenuItem**)&item4);
        item4->SetIcon(Elastos::Droid::R::drawable::ic_menu_help);
        item4->SetIntent(help);
        item4->SetAlphabeticShortcut('H');
    }
    *result = TRUE;
    return NOERROR;
}

ECode Launcher::OnPrepareOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean res;
    Activity::OnPrepareOptionsMenu(menu, &res);
    mAppsCustomizeTabHost->IsTransitioning(&res);
    if (res) {
        *result = FALSE;
        return NOERROR;
    }

    Int32 visibility;
    IView::Probe(mAppsCustomizeTabHost)->GetVisibility(&visibility);
    Boolean allAppsVisible = (visibility == IView::VISIBLE);
    menu->SetGroupVisible(MENU_GROUP_WALLPAPER, !allAppsVisible);

    *result = TRUE;
    return NOERROR;
}

ECode Launcher::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 id;
    item->GetItemId(&id);
    switch (id) {
        case MENU_WALLPAPER_SETTINGS:
            StartWallpaper();
            *result = TRUE;
            return NOERROR;
    }

    return Activity::OnOptionsItemSelected(item, result);
}

ECode Launcher::OnSearchRequested(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    StartSearch(String(NULL), FALSE, NULL, TRUE);
    // Use a custom animation for launching search
    *result = TRUE;
    return NOERROR;
}

ECode Launcher::IsWorkspaceLocked(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mWorkspaceLoading || mWaitingForResult;
    return NOERROR;
}

void Launcher::ResetAddInfo()
{
    mPendingAddInfo->mContainer = IItemInfo::NO_ID;
    mPendingAddInfo->mScreen = -1;
    mPendingAddInfo->mCellX = mPendingAddInfo->mCellY = -1;
    mPendingAddInfo->mSpanX = mPendingAddInfo->mSpanY = -1;
    mPendingAddInfo->mMinSpanX = mPendingAddInfo->mMinSpanY = -1;
    mPendingAddInfo->mDropPos = NULL;
    return;
}

ECode Launcher::AddAppWidgetImpl(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IItemInfo* info,
    /* [in] */ IAppWidgetHostView* boundWidget,
    /* [in] */ IAppWidgetProviderInfo* appWidgetInfo)
{
    AutoPtr<IComponentName> name;
    appWidgetInfo->GetConfigure((IComponentName**)&name);
    if (name != NULL) {
        mPendingAddWidgetInfo = appWidgetInfo;
        mPendingAddWidgetId = appWidgetId;

        // Launch over to configure widget, if needed
        return StartAppWidgetConfigureActivitySafely(appWidgetId);
    }
    else {
        // Otherwise just add it
        ItemInfo* _info = (ItemInfo*)info;
        CompleteAddAppWidget(appWidgetId, _info->mContainer, _info->mScreen, boundWidget,
                appWidgetInfo);
        // Exit spring loaded mode if necessary after adding the widget
        return ExitSpringLoadedDragModeDelayed(TRUE, FALSE, NULL);
    }
    return NOERROR;
}

ECode Launcher::ProcessShortcutFromDrop(
    /* [in] */ IComponentName* componentName,
    /* [in] */ Int64 container,
    /* [in] */ Int32 screen,
    /* [in] */ ArrayOf<Int32>* cell,
    /* [in] */ ArrayOf<Int32>* loc)
{
    ResetAddInfo();
    mPendingAddInfo->mContainer = container;
    mPendingAddInfo->mScreen = screen;
    mPendingAddInfo->mDropPos = loc;

    if (cell != NULL) {
        mPendingAddInfo->mCellX = (*cell)[0];
        mPendingAddInfo->mCellY = (*cell)[1];
    }

    AutoPtr<IIntent> createShortcutIntent;
    CIntent::New(IIntent::ACTION_CREATE_SHORTCUT, (IIntent**)&createShortcutIntent);
    createShortcutIntent->SetComponent(componentName);
    return ProcessShortcut(createShortcutIntent);
}

ECode Launcher::AddAppWidgetFromDrop(
    /* [in] */ IPendingAddWidgetInfo* info,
    /* [in] */ Int64 container,
    /* [in] */ Int32 screen,
    /* [in] */ ArrayOf<Int32>* cell,
    /* [in] */ ArrayOf<Int32>* span,
    /* [in] */ ArrayOf<Int32>* loc)
{
    ResetAddInfo();
    PendingAddWidgetInfo* _info = (PendingAddWidgetInfo*)info;
    mPendingAddInfo->mContainer = _info->mContainer = container;
    mPendingAddInfo->mScreen = _info->mScreen = screen;
    mPendingAddInfo->mDropPos = loc;
    mPendingAddInfo->mMinSpanX = _info->mMinSpanX;
    mPendingAddInfo->mMinSpanY = _info->mMinSpanY;

    if (cell != NULL) {
        mPendingAddInfo->mCellX = (*cell)[0];
        mPendingAddInfo->mCellY = (*cell)[1];
    }
    if (span != NULL) {
        mPendingAddInfo->mSpanX = (*span)[0];
        mPendingAddInfo->mSpanY = (*span)[1];
    }

    AutoPtr<IAppWidgetHostView> hostView = _info->mBoundWidget;
    Int32 appWidgetId;
    if (hostView != NULL) {
        hostView->GetAppWidgetId(&appWidgetId);
        AddAppWidgetImpl(appWidgetId, IItemInfo::Probe(info), hostView, _info->mInfo);
    }
    else {
        // In this case, we either need to start an activity to get permission to bind
        // the widget, or we need to start an activity to configure the widget, or both.
        AutoPtr<ILauncherAppWidgetHost> host;
        GetAppWidgetHost((ILauncherAppWidgetHost**)&host);
        IAppWidgetHost::Probe(host)->AllocateAppWidgetId(&appWidgetId);

        AutoPtr<IUserHandle> handle;
        _info->mInfo->GetProfile((IUserHandle**)&handle);
        Boolean success;
        mAppWidgetManager->BindAppWidgetIdIfAllowed(appWidgetId,
                handle, _info->mComponentName, _info->mBindOptions, &success);
        if (success) {
            AddAppWidgetImpl(appWidgetId, IItemInfo::Probe(info), NULL, _info->mInfo);
        }
        else {
            mPendingAddWidgetInfo = _info->mInfo;
            AutoPtr<IIntent> intent;
            CIntent::New(IAppWidgetManager::ACTION_APPWIDGET_BIND, (IIntent**)&intent);
            intent->PutExtra(IAppWidgetManager::EXTRA_APPWIDGET_ID, appWidgetId);
            intent->PutExtra(IAppWidgetManager::EXTRA_APPWIDGET_PROVIDER,
                    IParcelable::Probe(_info->mComponentName));
            AutoPtr<IUserHandle> handle2;
            _info->mInfo->GetProfile((IUserHandle**)&handle2);
            intent->PutExtra(IAppWidgetManager::EXTRA_APPWIDGET_PROVIDER_PROFILE,
                    IParcelable::Probe(handle2));
            // TODO: we need to make sure that this accounts for the options bundle.
            // intent.putExtra(AppWidgetManager.EXTRA_APPWIDGET_OPTIONS, options);
            StartActivityForResult(intent, REQUEST_BIND_APPWIDGET);
        }
    }
    return NOERROR;
}

ECode Launcher::ProcessShortcut(
    /* [in] */ IIntent* intent)
{
    // Handle case where user selected "Applications"
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    String applicationName;
    resources->GetString(Elastos::Droid::Launcher2::R::string::group_applications,
            &applicationName);
    String shortcutName;
    intent->GetStringExtra(IIntent::EXTRA_SHORTCUT_NAME, &shortcutName);

    if (!applicationName.IsNull() && (applicationName.Equals(shortcutName))) {
        AutoPtr<IIntent> mainIntent;
        CIntent::New(IIntent::ACTION_MAIN, NULL, (IIntent**)&mainIntent);
        mainIntent->AddCategory(IIntent::CATEGORY_LAUNCHER);

        AutoPtr<IIntent> pickIntent;
        CIntent::New(IIntent::ACTION_PICK_ACTIVITY, (IIntent**)&pickIntent);
        pickIntent->PutExtra(IIntent::EXTRA_INTENT, IParcelable::Probe(mainIntent));
        AutoPtr<ICharSequence> cchar;
        GetText(Elastos::Droid::Launcher2::R::string::title_select_application, (ICharSequence**)&cchar);
        pickIntent->PutExtra(IIntent::EXTRA_TITLE, cchar);
        return StartActivityForResultSafely(pickIntent, REQUEST_PICK_APPLICATION);
    }
    else {
        return StartActivityForResultSafely(intent, REQUEST_CREATE_SHORTCUT);
    }
    return NOERROR;
}

ECode Launcher::ProcessWallpaper(
    /* [in] */ IIntent* intent)
{
    return StartActivityForResult(intent, REQUEST_PICK_WALLPAPER);
}

ECode Launcher::AddFolder(
    /* [in] */ ICellLayout* layout,
    /* [in] */ Int64 container,
    /* [in] */ Int32 screen,
    /* [in] */ Int32 cellX,
    /* [in] */ Int32 cellY,
    /* [out] */ IFolderIcon** icon)
{
    VALIDATE_NOT_NULL(icon);

    AutoPtr<FolderInfo> folderInfo = new FolderInfo();
    GetText(Elastos::Droid::Launcher2::R::string::folder_name,
        (ICharSequence**)&(folderInfo->mTitle));

    // Update the model
    LauncherModel::AddItemToDatabase(this, folderInfo, container, screen, cellX, cellY,
            FALSE);
    AutoPtr<IInteger64> id = CoreUtils::Convert(folderInfo->mId);
    sFolders->Put(TO_IINTERFACE(id), TO_IINTERFACE(folderInfo));

    // Create the view
    AutoPtr<IFolderIcon> newFolder;
    FolderIcon::FromXml(Elastos::Droid::Launcher2::R::layout::folder_icon, ILauncher::Probe(this),
            IViewGroup::Probe(layout), IFolderInfo::Probe(folderInfo), mIconCache,
            (IFolderIcon**)&newFolder);
    Boolean res;
    IsWorkspaceLocked(&res);
    mWorkspace->AddInScreen(IView::Probe(newFolder), container, screen, cellX, cellY, 1, 1, res);
    *icon = newFolder;
    REFCOUNT_ADD(*icon);
    return NOERROR;
}

ECode Launcher::RemoveFolder(
    /* [in] */ IFolderInfo* folder)
{
    FolderInfo* _folder = (FolderInfo*)folder;
    AutoPtr<IInteger64> id = CoreUtils::Convert(_folder->mId);
    return sFolders->Remove(TO_IINTERFACE(id));
}

void Launcher::StartWallpaper()
{
    ShowWorkspace(TRUE);
    AutoPtr<IIntent> pickWallpaper;
    CIntent::New(IIntent::ACTION_SET_WALLPAPER, (IIntent**)&pickWallpaper);

    AutoPtr<ICharSequence> cchar;
    GetText(Elastos::Droid::Launcher2::R::string::chooser_wallpaper, (ICharSequence**)&cchar);

    AutoPtr<IIntentHelper> helper;
    CIntentHelper::AcquireSingleton((IIntentHelper**)&helper);
    AutoPtr<IIntent> chooser;
    helper->CreateChooser(pickWallpaper, cchar, (IIntent**)&chooser);
    // NOTE: Adds a configure option to the chooser if the wallpaper supports it
    //       Removed in Eclair MR1
//        WallpaperManager wm = (WallpaperManager)
//                getSystemService(Context.WALLPAPER_SERVICE);
//        WallpaperInfo wi = wm.getWallpaperInfo();
//        if (wi != null && wi.getSettingsActivity() != null) {
//            LabeledIntent li = new LabeledIntent(getPackageName(),
//                    R.string.configure_wallpaper, 0);
//            li.setClassName(wi.getPackageName(), wi.getSettingsActivity());
//            chooser.putExtra(Intent.EXTRA_INITIAL_INTENTS, new Intent[] { li });
//        }
    StartActivityForResult(chooser, REQUEST_PICK_WALLPAPER);
    return;
}

void Launcher::RegisterContentObservers()
{
    AutoPtr<IContentResolver> resolver;
    GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<IUri> uri;
    LauncherProvider::GetCONTENT_APPWIDGET_RESET_URI((IUri**)&uri);
    resolver->RegisterContentObserver(uri, TRUE, mWidgetObserver);
}

ECode Launcher::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 action;
    event->GetAction(&action);
    if (action == IKeyEvent::ACTION_DOWN) {
        Int32 code;
        event->GetKeyCode(&code);
        switch (code) {
            case IKeyEvent::KEYCODE_HOME:
                *result = TRUE;
                return NOERROR;
            case IKeyEvent::KEYCODE_VOLUME_DOWN:
            {
                if (IsPropertyEnabled(DUMP_STATE_PROPERTY)) {
                    DumpState();
                    *result = TRUE;
                    return NOERROR;
                }
                break;
            }
        }
    }
    else if (action == IKeyEvent::ACTION_UP) {
        Int32 code;
        event->GetKeyCode(&code);
        switch (code) {
            case IKeyEvent::KEYCODE_HOME:
                *result = TRUE;
                return NOERROR;
        }
    }

    return Activity::DispatchKeyEvent(event, result);
}

ECode Launcher::OnBackPressed()
{
    Boolean res;
    IsAllAppsVisible(&res);
    AutoPtr<IFolder> folder;
    mWorkspace->GetOpenFolder((IFolder**)&folder);
    if (res) {
        ShowWorkspace(TRUE);
    }
    else if (folder != NULL) {
        AutoPtr<IFolder> openFolder = folder;
        Boolean res;
        openFolder->IsEditingName(&res);
        if (res) {
            return openFolder->DismissEditingName();
        }
        else {
            return CloseFolder();
        }
    }
    else {
        mWorkspace->ExitWidgetResizeMode();

        // Back button is a no-op here, but give at least some feedback for the button press
        return mWorkspace->ShowOutlinesTemporarily();
    }
    return NOERROR;
}

void Launcher::OnAppWidgetReset()
{
    if (mAppWidgetHost != NULL) {
        IAppWidgetHost::Probe(mAppWidgetHost)->StartListening();
    }
}

ECode Launcher::OnClick(
    /* [in] */ IView* v)
{
Slogger::E("Launcher", "=================OnClick 1");
    // Make sure that rogue clicks don't get through while allapps is launching, or after the
    // view has detached (it's possible for this to happen if the view is removed mid touch).
    AutoPtr<IBinder> token;
    v->GetWindowToken((IBinder**)&token);
    if (token == NULL) {
        return NOERROR;
    }
Slogger::E("Launcher", "=================OnClick 2");
    Boolean res;
    mWorkspace->IsFinishedSwitchingState(&res);
    if (!res) {
        return NOERROR;
    }

Slogger::E("Launcher", "============================Launcher add temporary by snow begin");
    Int32 id;
    v->GetId(&id);
    switch (id) {
        case R::id::cling_dismiss_button:
Slogger::E("Launcher", "=================OnClick calll DismissAllAppsCling");
            DismissAllAppsCling(v);
            break;
        default:
            break;
    }
Slogger::E("Launcher", "============================Launcher add temporary by snow end");

    AutoPtr<IInterface> tag;
    v->GetTag((IInterface**)&tag);
    if (IShortcutInfo::Probe(tag) != NULL) {
Slogger::E("Launcher", "=================OnClick 4");
        // Open shortcut
        ShortcutInfo* info = (ShortcutInfo*)IShortcutInfo::Probe(tag);
        AutoPtr<IIntent> intent = info->mIntent;
        AutoPtr<ArrayOf<Int32> > pos = ArrayOf<Int32>::Alloc(2);
        v->GetLocationOnScreen(pos);
        Int32 width;
        v->GetWidth(&width);
        Int32 height;
        v->GetHeight(&height);
        AutoPtr<IRect> tmp;
        CRect::New((*pos)[0], (*pos)[1],
                (*pos)[0] + width, (*pos)[1] + height, (IRect**)&tmp);
        intent->SetSourceBounds(tmp);

        Boolean success;
        StartActivitySafely(v, intent, tag, &success);

        if (success && IBubbleTextView::Probe(v) != NULL) {
            mWaitingForResume = IBubbleTextView::Probe(v);
            mWaitingForResume->SetStayPressed(TRUE);
        }
    }
    else if (IFolderInfo::Probe(tag) != NULL) {
Slogger::E("Launcher", "=================OnClick 5");
        if (IFolderIcon::Probe(v) != NULL) {
            AutoPtr<IFolderIcon> fi = IFolderIcon::Probe(v);
            HandleFolderClick(fi);
        }
    }
    else if (TO_IINTERFACE(v) == TO_IINTERFACE(mAllAppsButton)) {
Slogger::E("Launcher", "=================OnClick 6");
        Boolean res;
        IsAllAppsVisible(&res);
        if (res) {
            ShowWorkspace(TRUE);
        }
        else {
            OnClickAllAppsButton(v);
        }
    }
    return NOERROR;
}

ECode Launcher::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // this is an intercepted event being forwarded from mWorkspace;
    // clicking anywhere on the workspace causes the customization drawer to slide down
    ShowWorkspace(TRUE);
    *result = FALSE;
    return NOERROR;
}

ECode Launcher::OnClickSearchButton(
    /* [in] */ IView* v)
{
    Boolean res;
    v->PerformHapticFeedback(IHapticFeedbackConstants::VIRTUAL_KEY, &res);
    OnSearchRequested(&res);
    return NOERROR;
}

ECode Launcher::OnClickVoiceButton(
    /* [in] */ IView* v)
{
    Boolean res;
    v->PerformHapticFeedback(IHapticFeedbackConstants::VIRTUAL_KEY, &res);

    // try
    ECode ec;
    {
        AutoPtr<IInterface> obj;
        FAIL_GOTO(ec = GetSystemService(IContext::SEARCH_SERVICE, (IInterface**)&obj), ERROR)
        AutoPtr<ISearchManager> searchManager = ISearchManager::Probe(obj);
        AutoPtr<IComponentName> activityName;
        FAIL_GOTO(ec = searchManager->GetGlobalSearchActivity((IComponentName**)&activityName), ERROR)
        AutoPtr<IIntent> intent;
        CIntent::New(IRecognizerIntent::ACTION_WEB_SEARCH, (IIntent**)&intent);
        FAIL_GOTO(ec = intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK), ERROR)
        if (activityName != NULL) {
            String pname;
            FAIL_GOTO(ec = activityName->GetPackageName(&pname), ERROR)
            FAIL_GOTO(ec = intent->SetPackage(pname), ERROR)
        }
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(String("onClickVoiceButton"));
        FAIL_GOTO(ec = StartActivity(NULL, intent, cchar, &res), ERROR)
    // } catch (ActivityNotFoundException e) {
    }
ERROR:
    if (ec == (ECode)E_ACTIVITY_NOT_FOUND_EXCEPTION) {
        AutoPtr<IIntent> intent;
        CIntent::New(IRecognizerIntent::ACTION_WEB_SEARCH, (IIntent**)&intent);
        intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(String("onClickVoiceButton"));
        Boolean res;
        StartActivitySafely(NULL, intent, cchar, &res);
    }
    // }
    return NOERROR;
}

ECode Launcher::OnClickAllAppsButton(
    /* [in] */ IView* v)
{
Slogger::E("Launcher", "=================OnClickAllAppsButton======ShowAllApps");
    return ShowAllApps(TRUE);
}

ECode Launcher::OnTouchDownAllAppsButton(
    /* [in] */ IView* v)
{
    // Provide the same haptic feedback that the system offers for virtual keys.
    Boolean res;
    return v->PerformHapticFeedback(IHapticFeedbackConstants::VIRTUAL_KEY, &res);
}

ECode Launcher::OnClickAppMarketButton(
    /* [in] */ IView* v)
{
    if (mAppMarketIntent != NULL) {
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(String("app market"));
        Boolean res;
        return StartActivitySafely(v, mAppMarketIntent, cchar, &res);
    }
    else {
        Slogger::E(TAG, "Invalid app market intent.");
    }
    return NOERROR;
}

ECode Launcher::StartApplicationDetailsActivity(
    /* [in] */ IComponentName* componentName,
    /* [in] */ IUserHandle* user)
{
    AutoPtr<IInterface> obj;
    GetSystemService(IContext::LAUNCHER_APPS_SERVICE, (IInterface**)&obj);
    AutoPtr<ILauncherApps> launcherApps = ILauncherApps::Probe(obj);
    //try {
    ECode ec = launcherApps->StartAppDetailsActivity(componentName, user, NULL, NULL);
    //} catch (SecurityException e) {
    if (ec == (ECode)E_SECURITY_EXCEPTION) {
        AutoPtr<IToastHelper> helper;
        CToastHelper::AcquireSingleton((IToastHelper**)&helper);
        AutoPtr<IToast> toast;
        helper->MakeText((IContext*)this,
                Elastos::Droid::Launcher2::R::string::activity_not_found,
                IToast::LENGTH_SHORT, (IToast**)&toast);
        toast->Show();
        Slogger::E(TAG, "Launcher does not have permission to launch settings");
    }
    //} catch (ActivityNotFoundException e) {
    if (ec == (ECode)E_ACTIVITY_NOT_FOUND_EXCEPTION) {
        AutoPtr<IToastHelper> helper;
        CToastHelper::AcquireSingleton((IToastHelper**)&helper);
        AutoPtr<IToast> toast;
        helper->MakeText((IContext*)this,
            Elastos::Droid::Launcher2::R::string::activity_not_found,
            IToast::LENGTH_SHORT, (IToast**)&toast);
        toast->Show();
        Slogger::E(TAG, "Unable to launch settings");
    }
    //}
    return NOERROR;
}

ECode Launcher::StartApplicationUninstallActivity(
    /* [in] */ IApplicationInfo* appInfo,
    /* [in] */ IUserHandle* user)
{
    ApplicationInfo* info = (ApplicationInfo*)appInfo;
    if ((info->mFlags & IApplicationInfo::DOWNLOADED_FLAG) == 0) {
        // System applications cannot be installed. For now, show a toast explaining that.
        // We may give them the option of disabling apps this way.
        AutoPtr<IToastHelper> helper;
        CToastHelper::AcquireSingleton((IToastHelper**)&helper);
        AutoPtr<IToast> toast;
        helper->MakeText((IContext*)this,
                Elastos::Droid::Launcher2::R::string::uninstall_system_app_text,
                IToast::LENGTH_SHORT, (IToast**)&toast);
        return toast->Show();
    }
    else {
        String packageName;
        info->mComponentName->GetPackageName(&packageName);
        String className;
        info->mComponentName->GetClassName(&className);
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> uri;
        helper->FromParts(String("package"), packageName, className, (IUri**)&uri);
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_DELETE, uri, (IIntent**)&intent);
        intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK |
                IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
        if (user != NULL) {
            intent->PutExtra(IIntent::EXTRA_USER, IParcelable::Probe(user));
        }
        return Activity::StartActivity(intent);
    }
    return NOERROR;
}

ECode Launcher::StartActivity(
    /* [in] */ IView* v,
    /* [in] */ IIntent* intent,
    /* [in] */ IInterface* tag,
    /* [out] */ Boolean* result)

{
    VALIDATE_NOT_NULL(result);
    intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);

    //try {
    // Only launch using the new animation if the shortcut has not opted out (this is a
    // private contract between launcher and may be ignored in the future).
    ECode ec;
    {
        Boolean res;
        Boolean useLaunchAnimation = (v != NULL) &&
                (intent->HasExtra(INTENT_EXTRA_IGNORE_LAUNCH_ANIMATION, &res), !res);
        AutoPtr<IParcelable> parcelable;
        FAIL_GOTO(ec = intent->GetParcelableExtra(IItemInfo::EXTRA_PROFILE,
                (IParcelable**)&parcelable), ERROR)
        AutoPtr<IUserHandle> user = IUserHandle::Probe(parcelable);
        AutoPtr<IInterface> obj;
        FAIL_GOTO(ec = GetSystemService(IContext::LAUNCHER_APPS_SERVICE, (IInterface**)&obj), ERROR)
        AutoPtr<ILauncherApps> launcherApps = ILauncherApps::Probe(obj);
        if (useLaunchAnimation) {
            Int32 width;
            FAIL_GOTO(ec = v->GetMeasuredWidth(&width), ERROR)
            Int32 height;
            FAIL_GOTO(ec = v->GetMeasuredHeight(&height), ERROR)

            AutoPtr<IActivityOptionsHelper> helper;
            CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&helper);
            AutoPtr<IActivityOptions> opts;
            FAIL_GOTO(ec = helper->MakeScaleUpAnimation(v, 0, 0, width, height,
                    (IActivityOptions**)&opts), ERROR)

            AutoPtr<IUserHandle> _user;
            FAIL_GOTO(ec = Process::MyUserHandle((IUserHandle**)&_user), ERROR)
            Boolean res;
            if (user == NULL || (IObject::Probe(user)->Equals(_user, &res), res)) {
                // Could be launching some bookkeeping activity
                AutoPtr<IBundle> bundle;
                FAIL_GOTO(ec = opts->ToBundle((IBundle**)&bundle), ERROR)
                FAIL_GOTO(ec = Activity::StartActivity(intent, bundle), ERROR)
            }
            else {
                AutoPtr<IComponentName> componentName;
                FAIL_GOTO(ec = intent->GetComponent((IComponentName**)&componentName), ERROR)
                AutoPtr<IRect> sourceBounds;
                FAIL_GOTO(ec = intent->GetSourceBounds((IRect**)&sourceBounds), ERROR)
                AutoPtr<IBundle> bundle;
                FAIL_GOTO(ec = opts->ToBundle((IBundle**)&bundle), ERROR)
                FAIL_GOTO(ec = launcherApps->StartMainActivity(componentName, user, sourceBounds, bundle), ERROR)
            }
        }
        else {
            AutoPtr<IUserHandle> _user;
            FAIL_GOTO(ec = Process::MyUserHandle((IUserHandle**)&_user), ERROR)
            Boolean res;
            if (user == NULL || (user->Equals(_user, &res), res)) {
                FAIL_GOTO(ec = Activity::StartActivity(intent), ERROR)
            }
            else {
                AutoPtr<IComponentName> componentName;
                FAIL_GOTO(ec = intent->GetComponent((IComponentName**)&componentName), ERROR)
                AutoPtr<IRect> sourceBounds;
                FAIL_GOTO(ec = intent->GetSourceBounds((IRect**)&sourceBounds), ERROR)
                FAIL_GOTO(ec = launcherApps->StartMainActivity(componentName, user, sourceBounds, NULL), ERROR)
            }
        }
        *result = TRUE;
        return NOERROR;
    }
ERROR:
    //} catch (SecurityException e) {
    if (ec == (ECode)E_SECURITY_EXCEPTION) {
        AutoPtr<IToastHelper> helper;
        CToastHelper::AcquireSingleton((IToastHelper**)&helper);
        AutoPtr<IToast> toast;
        helper->MakeText((IContext*)this,
                Elastos::Droid::Launcher2::R::string::activity_not_found,
                IToast::LENGTH_SHORT, (IToast**)&toast);
        toast->Show();
        StringBuilder sb;
        sb += "Launcher does not have the permission to launch ";
        sb += TO_STR(intent);
        sb += ". Make sure to create a MAIN intent-filter for the corresponding activity ";
        sb += "or use the exported attribute for this activity. ";
        sb += "tag=";
        sb += TO_STR(tag);
        sb += " intent=";
        sb += TO_STR(intent);
        sb += ec;
        Slogger::E(TAG, sb.ToString());
    }
    //}
    *result = FALSE;
    return NOERROR;
}

ECode Launcher::StartActivitySafely(
    /* [in] */ IView* v,
    /* [in] */ IIntent* intent,
    /* [in] */ IInterface* tag,
    /* [out] */ Boolean* result)
{
    Slogger::I(TAG, " >> StartActivitySafely %s, intent:%s, tag:%s",
        TO_CSTR(v), TO_CSTR(intent), TO_CSTR(tag));
    VALIDATE_NOT_NULL(result);

    Boolean success = FALSE;
    ECode ec = StartActivity(v, intent, tag, &success);
    if (FAILED(ec) || !success ) {
        AutoPtr<IToastHelper> helper;
        CToastHelper::AcquireSingleton((IToastHelper**)&helper);
        AutoPtr<IToast> toast;
        helper->MakeText((IContext*)this,
            Elastos::Droid::Launcher2::R::string::activity_not_found,
            IToast::LENGTH_SHORT, (IToast**)&toast);
        toast->Show();

        StringBuilder sb;
        sb += "Unable to launch. tag=";
        sb += TO_STR(tag);
        sb +=" intent=";
        sb += TO_STR(intent);
        sb += "ec=";
        sb += StringUtils::ToHexString(ec);
        Slogger::E(TAG, sb.ToString());
    }

    *result = success;
    return NOERROR;
}

ECode Launcher::StartAppWidgetConfigureActivitySafely(
    /* [in] */ Int32 appWidgetId)
{
    if ((ECode)E_ACTIVITY_NOT_FOUND_EXCEPTION ==
            IAppWidgetHost::Probe(mAppWidgetHost)->StartAppWidgetConfigureActivityForResult(
                IActivity::Probe(this), appWidgetId, 0, REQUEST_CREATE_APPWIDGET, NULL)) {
        AutoPtr<IToastHelper> helper;
        CToastHelper::AcquireSingleton((IToastHelper**)&helper);
        AutoPtr<IToast> toast;
        helper->MakeText((IContext*)this,
                Elastos::Droid::Launcher2::R::string::activity_not_found,
                IToast::LENGTH_SHORT, (IToast**)&toast);
        return toast->Show();
    }
    return NOERROR;
}

ECode Launcher::StartActivityForResultSafely(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 requestCode)
{
    //try {
    ECode ec = StartActivityForResult(intent, requestCode);
    //} catch (ActivityNotFoundException e) {
    if (ec == (ECode)E_ACTIVITY_NOT_FOUND_EXCEPTION) {
        AutoPtr<IToastHelper> helper;
        CToastHelper::AcquireSingleton((IToastHelper**)&helper);
        AutoPtr<IToast> toast;
        helper->MakeText((IContext*)this,
                Elastos::Droid::Launcher2::R::string::activity_not_found,
                IToast::LENGTH_SHORT, (IToast**)&toast);
        toast->Show();
    }
    //} catch (SecurityException e) {
    if (ec == (ECode)E_SECURITY_EXCEPTION) {
        AutoPtr<IToastHelper> helper;
        CToastHelper::AcquireSingleton((IToastHelper**)&helper);
        AutoPtr<IToast> toast;
        helper->MakeText((IContext*)this,
                Elastos::Droid::Launcher2::R::string::activity_not_found,
                IToast::LENGTH_SHORT, (IToast**)&toast);
        toast->Show();

        StringBuilder sb;
        sb += "Launcher does not have the permission to launch ";
        sb += TO_STR(intent);
        sb += ". Make sure to create a MAIN intent-filter for the corresponding activity ";
        sb += "or use the exported attribute for this activity.";
        sb += ec;
        Slogger::E(TAG, sb.ToString());
    }
    //}
    return NOERROR;
}

void Launcher::HandleFolderClick(
    /* [in] */ IFolderIcon* folderIcon)
{
    AutoPtr<IFolderInfo> info;
    folderIcon->GetFolderInfo((IFolderInfo**)&info);
    AutoPtr<IFolder> openFolder;
    mWorkspace->GetFolderForTag(info, (IFolder**)&openFolder);

    // If the folder info reports that the associated folder is open, then verify that
    // it is actually opened. There have been a few instances where this gets out of sync.
    FolderInfo* _info = (FolderInfo*)info.Get();
    if (_info->mOpened && openFolder == NULL) {
        StringBuilder sb;
        sb += "Folder info marked as open, but associated folder is not open. Screen: ";
        sb += _info->mScreen;
        sb += " (";
        sb += _info->mCellX;
        sb += ", ";
        sb += _info->mCellY;
        sb += ")";
        Slogger::D(TAG, sb.ToString());
        _info->mOpened = FALSE;
    }

    AutoPtr<IFolder> folder;
    folderIcon->GetFolder((IFolder**)&folder);
    Boolean res;
    folder->IsDestroyed(&res);
    if (!_info->mOpened && !res) {
        // Close any open folder
        CloseFolder();
        // Open the requested folder
        OpenFolder(folderIcon);
    }
    else {
        // Find the open folder...
        Int32 folderScreen;
        if (openFolder != NULL) {
            IPagedView::Probe(mWorkspace)->GetPageForView(IView::Probe(openFolder),
                    &folderScreen);
            // .. and close it
            CloseFolder(openFolder);
            Int32 page;
            IPagedView::Probe(mWorkspace)->GetCurrentPage(&page);
            if (folderScreen != page) {
                // Close any folder open on the current screen
                CloseFolder();
                // Pull the folder onto this screen
                OpenFolder(folderIcon);
            }
        }
    }
}

void Launcher::CopyFolderIconToImage(
    /* [in] */ IFolderIcon* fi)
{
    Int32 width;
    IView::Probe(fi)->GetMeasuredWidth(&width);
    Int32 height;
    IView::Probe(fi)->GetMeasuredHeight(&height);

    // Lazy load ImageView, Bitmap and Canvas
    if (mFolderIconImageView == NULL) {
        CImageView::New(this, (IImageView**)&mFolderIconImageView);
    }
    Int32 width2;
    mFolderIconBitmap->GetWidth(&width2);
    Int32 height2;
    mFolderIconBitmap->GetHeight(&height2);
    if (mFolderIconBitmap == NULL || width2 != width || height2 != height) {
        AutoPtr<IBitmapHelper> helper;
        CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
        helper->CreateBitmap(width, height, BitmapConfig_ARGB_8888, (IBitmap**)&mFolderIconBitmap);
        CCanvas::New(mFolderIconBitmap, (ICanvas**)&mFolderIconCanvas);
    }

    AutoPtr<DragLayer::DragLayerLayoutParams> lp;
    AutoPtr<IViewGroupLayoutParams> para;
    IView::Probe(mFolderIconImageView)->GetLayoutParams((IViewGroupLayoutParams**)&para);
    if (IDragLayerLayoutParams::Probe(para) != NULL) {
        lp = (DragLayer::DragLayerLayoutParams*)IDragLayerLayoutParams::Probe(para);
    }
    else {
        lp = new DragLayer::DragLayerLayoutParams();
        lp->constructor(width, height);
    }

    // The layout from which the folder is being opened may be scaled, adjust the starting
    // view size by this scale factor.
    Float scale;
    mDragLayer->GetDescendantRectRelativeToSelf(IView::Probe(fi), mRectForFolderAnimation, &scale);
    lp->mCustomPosition = TRUE;
    Int32 left;
    mRectForFolderAnimation->GetLeft(&left);
    lp->mX = left;
    Int32 top;
    mRectForFolderAnimation->GetTop(&top);
    lp->mY = top;
    lp->mWidth = (Int32)(scale * width);
    lp->mHeight = (Int32)(scale * height);

    mFolderIconCanvas->DrawColor(0, PorterDuffMode_CLEAR);
    IView::Probe(fi)->Draw(mFolderIconCanvas);
    mFolderIconImageView->SetImageBitmap(mFolderIconBitmap);

    AutoPtr<IFolder> folder;
    fi->GetFolder((IFolder**)&folder);
    if (folder != NULL) {
        Float x;
        folder->GetPivotXForIconAnimation(&x);
        IView::Probe(mFolderIconImageView)->SetPivotX(x);
        Float y;
        folder->GetPivotYForIconAnimation(&y);
        IView::Probe(mFolderIconImageView)->SetPivotY(y);
    }
    // Just in case this image view is still in the drag layer from a previous animation,
    // we remove it and re-add it.
    Int32 index;
    IViewGroup::Probe(mDragLayer)->IndexOfChild(IView::Probe(mFolderIconImageView), &index);
    if (index != -1) {
        IViewGroup::Probe(mDragLayer)->RemoveView(IView::Probe(mFolderIconImageView));
    }
    IViewGroup::Probe(mDragLayer)->AddView(IView::Probe(mFolderIconImageView),
            IViewGroupLayoutParams::Probe(lp));
    if (folder != NULL) {
        IView::Probe(folder)->BringToFront();
    }
}

void Launcher::GrowAndFadeOutFolderIcon(
    /* [in] */ IFolderIcon* fi)
{
    if (fi == NULL) return;

    AutoPtr<IPropertyValuesHolderHelper> helper;
    CPropertyValuesHolderHelper::AcquireSingleton((IPropertyValuesHolderHelper**)&helper);
    AutoPtr<ArrayOf<Float> > arr = ArrayOf<Float>::Alloc(1);
    (*arr)[0] = 0.0f;
    AutoPtr<IPropertyValuesHolder> alpha;
    helper->OfFloat(String("alpha"), arr, (IPropertyValuesHolder**)&alpha);

    AutoPtr<ArrayOf<Float> > arr2 = ArrayOf<Float>::Alloc(1);
    (*arr2)[0] = 1.5f;
    AutoPtr<IPropertyValuesHolder> scaleX;
    helper->OfFloat(String("scaleX"), arr2, (IPropertyValuesHolder**)&scaleX);

    AutoPtr<ArrayOf<Float> > arr3 = ArrayOf<Float>::Alloc(1);
    (*arr3)[0] = 1.5f;
    AutoPtr<IPropertyValuesHolder> scaleY;
    helper->OfFloat(String("scaleY"), arr3, (IPropertyValuesHolder**)&scaleY);

    AutoPtr<IInterface> obj;
    IView::Probe(fi)->GetTag((IInterface**)&obj);
    FolderInfo* info = (FolderInfo*)IFolderInfo::Probe(obj);
    if (info->mContainer == LauncherSettings::Favorites::CONTAINER_HOTSEAT) {
        AutoPtr<IViewParent> parent;
        IView::Probe(fi)->GetParent((IViewParent**)&parent);
        AutoPtr<IViewParent> parent2;
        parent->GetParent((IViewParent**)&parent2);
        AutoPtr<ICellLayout> cl = ICellLayout::Probe(parent2);

        AutoPtr<IViewGroupLayoutParams> params;
        IView::Probe(fi)->GetLayoutParams((IViewGroupLayoutParams**)&params);
        CellLayout::CellLayoutLayoutParams* lp = (CellLayout::CellLayoutLayoutParams*)ICellLayoutLayoutParams::Probe(params);
        cl->SetFolderLeaveBehindCell(lp->mCellX, lp->mCellY);
    }

    // Push an ImageView copy of the FolderIcon into the DragLayer and hide the original
    CopyFolderIconToImage(fi);
    IView::Probe(fi)->SetVisibility(IView::INVISIBLE);

    AutoPtr<ArrayOf<IPropertyValuesHolder*> > array = ArrayOf<IPropertyValuesHolder*>::Alloc(3);
    array->Set(0, alpha);
    array->Set(1, scaleX);
    array->Set(2, scaleY);
    AutoPtr<IObjectAnimator> oa = LauncherAnimUtils::OfPropertyValuesHolder(
            IView::Probe(mFolderIconImageView), array);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    Int32 num;
    resources->GetInteger(Elastos::Droid::Launcher2::R::integer::config_folderAnimDuration,
            &num);
    IAnimator::Probe(oa)->SetDuration(num);
    IAnimator::Probe(oa)->Start();
}

void Launcher::ShrinkAndFadeInFolderIcon(
    /* [in] */ IFolderIcon* fi)
{
    if (fi == NULL) return;
    AutoPtr<IPropertyValuesHolderHelper> helper;
    CPropertyValuesHolderHelper::AcquireSingleton((IPropertyValuesHolderHelper**)&helper);

    AutoPtr<ArrayOf<Float> > arr = ArrayOf<Float>::Alloc(1);
    (*arr)[0] = 1.0f;
    AutoPtr<IPropertyValuesHolder> alpha;
    helper->OfFloat(String("alpha"), arr, (IPropertyValuesHolder**)&alpha);
    AutoPtr<IPropertyValuesHolder> scaleX;
    helper->OfFloat(String("scaleX"), arr, (IPropertyValuesHolder**)&scaleX);
    AutoPtr<IPropertyValuesHolder> scaleY;
    helper->OfFloat(String("scaleY"), arr, (IPropertyValuesHolder**)&scaleY);

    AutoPtr<IViewParent> parent;
    IView::Probe(fi)->GetParent((IViewParent**)&parent);
    AutoPtr<IViewParent> parent2;
    parent->GetParent((IViewParent**)&parent2);
    AutoPtr<ICellLayout> cl = ICellLayout::Probe(parent2);

    // We remove and re-draw the FolderIcon in-case it has changed
    IViewGroup::Probe(mDragLayer)->RemoveView(IView::Probe(mFolderIconImageView));
    CopyFolderIconToImage(fi);
    AutoPtr<ArrayOf<IPropertyValuesHolder*> > array = ArrayOf<IPropertyValuesHolder*>::Alloc(3);
    array->Set(0, alpha);
    array->Set(1, scaleX);
    array->Set(2, scaleY);
    AutoPtr<IObjectAnimator> oa = LauncherAnimUtils::OfPropertyValuesHolder(
            IView::Probe(mFolderIconImageView), array);
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    Int32 tmp;
    resources->GetInteger(
            Elastos::Droid::Launcher2::R::integer::config_folderAnimDuration, &tmp);
    IAnimator::Probe(oa)->SetDuration(tmp);
    AutoPtr<IAnimatorListener> lis = new MyAnimatorListenerAdapter(this, cl, fi);
Slogger::D("Launcher", "=======Launcher::ShrinkAndFadeInFolderIcon lis=%p",lis.Get());
    IAnimator::Probe(oa)->AddListener(lis);
    IAnimator::Probe(oa)->Start();
}

ECode Launcher::OpenFolder(
    /* [in] */ IFolderIcon* folderIcon)
{
    AutoPtr<IFolder> folder;
    folderIcon->GetFolder((IFolder**)&folder);
    AutoPtr<IFolderInfo> info = ((Folder*)folder.Get())->mInfo;

    ((FolderInfo*)info.Get())->mOpened = TRUE;

    // Just verify that the folder hasn't already been added to the DragLayer.
    // There was a one-off crash where the folder had a parent already.
    AutoPtr<IViewParent> parent;
    IView::Probe(folder)->GetParent((IViewParent**)&parent);
    if (parent == NULL) {
        IViewGroup::Probe(mDragLayer)->AddView(IView::Probe(folder));
        mDragController->AddDropTarget(IDropTarget::Probe(folder));
    }
    else {
        StringBuilder sb;
        sb += "Opening folder (";
        sb += TO_STR(folder);
        sb += ") which already has a parent (";
        sb += TO_STR(parent);
        sb += ").";
        Slogger::W(TAG, sb.ToString());
    }
    folder->AnimateOpen();
    GrowAndFadeOutFolderIcon(folderIcon);

    // Notify the accessibility manager that this folder "window" has appeared and occluded
    // the workspace items
    IView::Probe(folder)->SendAccessibilityEvent(IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED);
    AutoPtr<IDragLayer> dragLayer;
    GetDragLayer((IDragLayer**)&dragLayer);
    IView::Probe(dragLayer)->SendAccessibilityEvent(IAccessibilityEvent::TYPE_WINDOW_CONTENT_CHANGED);
    return NOERROR;
}

ECode Launcher::CloseFolder()
{
    AutoPtr<IFolder> folder;
    mWorkspace->GetOpenFolder((IFolder**)&folder);
    if (folder != NULL) {
        Boolean res;
        folder->IsEditingName(&res);
        if (res) {
            folder->DismissEditingName();
        }
        CloseFolder(folder);

        // Dismiss the folder cling
        DismissFolderCling(NULL);
    }
    return NOERROR;
}

ECode Launcher::CloseFolder(
    /* [in] */ IFolder* folder)
{
    AutoPtr<IFolderInfo> info;
    folder->GetInfo((IFolderInfo**)&info);
    ((FolderInfo*)info.Get())->mOpened = FALSE;

    AutoPtr<IViewParent> parent1;
    IView::Probe(folder)->GetParent((IViewParent**)&parent1);
    AutoPtr<IViewParent> parent2;
    parent1->GetParent((IViewParent**)&parent2);
    AutoPtr<IViewGroup> parent = IViewGroup::Probe(parent2);

    if (parent != NULL) {
        AutoPtr<IView> view;
        mWorkspace->GetViewForTag(((Folder*)folder)->mInfo, (IView**)&view);
        AutoPtr<IFolderIcon> fi = IFolderIcon::Probe(view);
        ShrinkAndFadeInFolderIcon(fi);
    }
    folder->AnimateClosed();

    // Notify the accessibility manager that this folder "window" has disappeard and no
    // longer occludeds the workspace items
    AutoPtr<IDragLayer> dragLayer;
    GetDragLayer((IDragLayer**)&dragLayer);
    IView::Probe(dragLayer)->SendAccessibilityEvent(IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED);
    return NOERROR;
}

ECode Launcher::OnLongClick(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
Slogger::D("Launcher", "=====================================OnLongClick");
    VALIDATE_NOT_NULL(result);

    Boolean res;
    if (IsDraggingEnabled(&res), !res) {
        *result = FALSE;
        return NOERROR;
    }
    if (IsWorkspaceLocked(&res), res) {
        *result = FALSE;
        return NOERROR;
    }
    if (mState != Launcher_WORKSPACE) {
        *result = FALSE;
        return NOERROR;
    }

    if (ICellLayout::Probe(v) == NULL) {
        AutoPtr<IViewParent> parent1;
        v->GetParent((IViewParent**)&parent1);
        AutoPtr<IViewParent> parent2;
        parent1->GetParent((IViewParent**)&parent2);
        v = IView::Probe(parent2);
    }

    ResetAddInfo();
    AutoPtr<IInterface> tag;
    v->GetTag((IInterface**)&tag);
    AutoPtr<CellLayout::CellInfo> longClickCellInfo =
            (CellLayout::CellInfo*)ICellLayoutCellInfo::Probe(tag);
    // This happens when long clicking an item with the dpad/trackball
    if (longClickCellInfo == NULL) {
        *result = TRUE;
        return NOERROR;
    }

    // The hotseat touch handling does not go through Workspace, and we always allow long press
    // on hotseat items.
    AutoPtr<IView> itemUnderLongClick = longClickCellInfo->mCell;
    Boolean tmp1, tmp2;
    IsHotseatLayout(v, &tmp1);
    IPagedView::Probe(mWorkspace)->AllowLongPress(&tmp2);
    Boolean allowLongPress = tmp1 || tmp2;
    if (allowLongPress && (mDragController->IsDragging(&res), !res)) {
        if (itemUnderLongClick == NULL) {
            // User long pressed on empty space
Slogger::D("Launcher", "=============OnLongClick: User long pressed on empty space ");
            Boolean res;
            IView::Probe(mWorkspace)->PerformHapticFeedback(IHapticFeedbackConstants::LONG_PRESS,
                    IHapticFeedbackConstants::FLAG_IGNORE_VIEW_SETTING, &res);
Slogger::D("Launcher", "=============OnLongClick: StartWallpaper ");
            StartWallpaper();
        }
        else {
            if (IFolder::Probe(itemUnderLongClick) == NULL) {
                // User long pressed on an item
                mWorkspace->StartDrag(longClickCellInfo);
            }
        }
    }
    *result = TRUE;
    return NOERROR;
}

ECode Launcher::IsHotseatLayout(
    /* [in] */ IView* layout,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICellLayout> lay;
    if (mHotseat != NULL) {
        mHotseat->GetLayout((ICellLayout**)&lay);
    }

    *result = mHotseat != NULL && layout != NULL &&
            (ICellLayout::Probe(layout) != NULL) && (TO_IINTERFACE(layout) == TO_IINTERFACE(lay));
    return NOERROR;
}

ECode Launcher::GetHotseat(
    /* [out] */ IHotseat** seat)
{
    VALIDATE_NOT_NULL(seat);

    *seat = mHotseat;
    REFCOUNT_ADD(*seat);
    return NOERROR;
}

ECode Launcher::GetSearchBar(
    /* [out] */ ISearchDropTargetBar** bar)
{
    VALIDATE_NOT_NULL(bar);

    *bar = mSearchDropTargetBar;
    REFCOUNT_ADD(*bar);
    return NOERROR;
}

ECode Launcher::GetCellLayout(
    /* [in] */ Int64 container,
    /* [in] */ Int32 screen,
    /* [out] */ ICellLayout** layout)
{
    VALIDATE_NOT_NULL(layout);

    if (container == LauncherSettings::Favorites::CONTAINER_HOTSEAT) {
        if (mHotseat != NULL) {
            return mHotseat->GetLayout(layout);
        } else {
            *layout = NULL;
            return NOERROR;
        }
    }
    else {
        AutoPtr<IView> view;
        IViewGroup::Probe(mWorkspace)->GetChildAt(screen, (IView**)&view);
        AutoPtr<ICellLayout> tmp = ICellLayout::Probe(view);
        *layout = ICellLayout::Probe(view);
        REFCOUNT_ADD(*layout);
        return NOERROR;
    }
    return NOERROR;
}

ECode Launcher::GetWorkspace(
    /* [out] */ IWorkspace** workspace)
{
    VALIDATE_NOT_NULL(workspace);

    *workspace = mWorkspace;
    REFCOUNT_ADD(*workspace);
    return NOERROR;
}

ECode Launcher::IsAllAppsVisible(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (mState == Launcher_APPS_CUSTOMIZE) || (mOnResumeState == Launcher_APPS_CUSTOMIZE);
    return NOERROR;
}

ECode Launcher::IsAllAppsButtonRank(
    /* [in] */ Int32 rank,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    return mHotseat->IsAllAppsButtonRank(rank, result);
}

void Launcher::SetPivotsForZoom(
    /* [in] */ IView* view,
    /* [in] */ Float scaleFactor)
{
    Int32 width;
    view->GetWidth(&width);
    view->SetPivotX(width / 2.0f);
    Int32 height;
    view->GetHeight(&height);
    view->SetPivotY(height / 2.0f);
}

ECode Launcher::DisableWallpaperIfInAllApps()
{
    // Only disable it if we are in all apps
    Boolean res;
    IsAllAppsVisible(&res);
    if (res) {
        Boolean tmp;
        if (mAppsCustomizeTabHost != NULL &&
                (mAppsCustomizeTabHost->IsTransitioning(&tmp), !tmp)) {
            UpdateWallpaperVisibility(FALSE);
        }
    }
    return NOERROR;
}

void Launcher::SetWorkspaceBackground(
    /* [in] */ Boolean workspace)
{
    mLauncherView->SetBackground(workspace ?
            mWorkspaceBackgroundDrawable : NULL);
}

ECode Launcher::UpdateWallpaperVisibility(
    /* [in] */ Boolean visible)
{
    Int32 wpflags = visible ? IWindowManagerLayoutParams::FLAG_SHOW_WALLPAPER : 0;

    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    AutoPtr<IWindowManagerLayoutParams> params;
    window->GetAttributes((IWindowManagerLayoutParams**)&params);
    Int32 flags;
    params->GetFlags(&flags);
    Int32 curflags = flags & IWindowManagerLayoutParams::FLAG_SHOW_WALLPAPER;

    if (wpflags != curflags) {
        window->SetFlags(wpflags, IWindowManagerLayoutParams::FLAG_SHOW_WALLPAPER);
    }
    SetWorkspaceBackground(visible);
    return NOERROR;
}

void Launcher::DispatchOnLauncherTransitionPrepare(
    /* [in] */ IView* v,
    /* [in] */ Boolean animated,
    /* [in] */ Boolean toWorkspace)
{
    if (ILauncherTransitionable::Probe(v) != NULL) {
        ILauncherTransitionable::Probe(v)->OnLauncherTransitionPrepare(this, animated,
                toWorkspace);
    }
}

void Launcher::DispatchOnLauncherTransitionStart(
    /* [in] */ IView* v,
    /* [in] */ Boolean animated,
    /* [in] */ Boolean toWorkspace)
{
    if (ILauncherTransitionable::Probe(v) != NULL) {
        ILauncherTransitionable::Probe(v)->OnLauncherTransitionStart(this, animated,
                toWorkspace);
    }

    // Update the workspace transition step as well
    DispatchOnLauncherTransitionStep(v, 0.0f);
}

void Launcher::DispatchOnLauncherTransitionStep(
    /* [in] */ IView* v,
    /* [in] */ Float t)
{
    if (ILauncherTransitionable::Probe(v) != NULL) {
        ILauncherTransitionable::Probe(v)->OnLauncherTransitionStep(this, t);
    }
}

void Launcher::DispatchOnLauncherTransitionEnd(
    /* [in] */ IView* v,
    /* [in] */ Boolean animated,
    /* [in] */ Boolean toWorkspace)
{
    if (ILauncherTransitionable::Probe(v) != NULL) {
        ILauncherTransitionable::Probe(v)->OnLauncherTransitionEnd(this, animated, toWorkspace);
    }

    // Update the workspace transition step as well
    DispatchOnLauncherTransitionStep(v, 1.0f);
}

void Launcher::ShowAppsCustomizeHelper(
    /* [in] */ Boolean animated,
    /* [in] */ Boolean springLoaded)
{
Slogger::D("Launcher", "========================Launcher::ShowAppsCustomizeHelper 1");
    if (mStateAnimation != NULL) {
        IAnimator::Probe(mStateAnimation)->SetDuration(0);
        IAnimator::Probe(mStateAnimation)->Cancel();
        mStateAnimation = NULL;
    }
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);

    Int32 duration;
    res->GetInteger(
            Elastos::Droid::Launcher2::R::integer::config_appsCustomizeZoomInTime,
            &duration);
    Int32 fadeDuration;
    res->GetInteger(
            Elastos::Droid::Launcher2::R::integer::config_appsCustomizeFadeInTime,
            &fadeDuration);
    Int32 tmp;
    res->GetInteger(
            Elastos::Droid::Launcher2::R::integer::config_appsCustomizeZoomScaleFactor,
            &tmp);
    Float scale = (Float)tmp;
    AutoPtr<IView> fromView = IView::Probe(mWorkspace);
    AutoPtr<IAppsCustomizeTabHost> toView = mAppsCustomizeTabHost;
    Int32 startDelay;
    res->GetInteger(
            Elastos::Droid::Launcher2::R::integer::config_workspaceAppsCustomizeAnimationStagger,
            &startDelay);

    SetPivotsForZoom(IView::Probe(toView), scale);

    // Shrink workspaces away if going to AppsCustomize from workspace
    AutoPtr<IAnimator> workspaceAnim;
    mWorkspace->GetChangeStateAnimation(State_SMALL, animated, (IAnimator**)&workspaceAnim);
Slogger::D("Launcher", "========================Launcher::ShowAppsCustomizeHelper 2 animated=%d",animated);
    if (animated) {
        IView::Probe(toView)->SetScaleX(scale);
        IView::Probe(toView)->SetScaleY(scale);
Slogger::D("Launcher", "========================Launcher::ShowAppsCustomizeHelper 3 new scaleAnim");
        AutoPtr<LauncherViewPropertyAnimator> scaleAnim =
                new LauncherViewPropertyAnimator(IView::Probe(toView));
Slogger::D("Launcher", "========================Launcher::ShowAppsCustomizeHelper 4 scaleAnim->mListeners=%p",(scaleAnim->mListeners).Get());
        scaleAnim->ScaleX(1.0f);
        scaleAnim->ScaleY(1.0f);
        scaleAnim->SetDuration(duration);
        AutoPtr<ITimeInterpolator> polator = new Workspace::ZoomOutInterpolator();
        scaleAnim->SetInterpolator(polator);
Slogger::D("Launcher", "========================Launcher::ShowAppsCustomizeHelper 5");
        IView::Probe(toView)->SetVisibility(IView::VISIBLE);
        IView::Probe(toView)->SetAlpha(0.0f);
        AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(2);
        (*array)[0] = 0.0f;
        (*array)[1] = 1.0f;
        AutoPtr<IObjectAnimator> alphaAnim = LauncherAnimUtils::OfFloat(IView::Probe(toView),
                String("alpha"), array);
        IAnimator::Probe(alphaAnim)->SetDuration(fadeDuration);
        AutoPtr<ITimeInterpolator> value;
        CDecelerateInterpolator::New(1.5f, (ITimeInterpolator**)&value);
        IAnimator::Probe(alphaAnim)->SetInterpolator(value);
        AutoPtr<IAnimatorUpdateListener> lis = new MyAnimatorUpdateListener(this, fromView, toView);
        IValueAnimator::Probe(alphaAnim)->AddUpdateListener(lis);
Slogger::D("Launcher", "========================Launcher::ShowAppsCustomizeHelper 6");
        // toView should appear right at the end of the workspace shrink
        // animation
        mStateAnimation = LauncherAnimUtils::CreateAnimatorSet();
        AutoPtr<IAnimatorSetBuilder> builder;
        mStateAnimation->Play(scaleAnim, (IAnimatorSetBuilder**)&builder);
        builder->After(startDelay);
        AutoPtr<IAnimatorSetBuilder> builder2;
        mStateAnimation->Play(IAnimator::Probe(alphaAnim), (IAnimatorSetBuilder**)&builder2);
        builder2->After(startDelay);
Slogger::D("Launcher", "========================Launcher::ShowAppsCustomizeHelper 7");
        AutoPtr<IAnimatorListener> listener = new MyAnimatorListenerAdapter2(this,
                fromView, toView, animated, springLoaded);
Slogger::D("Launcher", "========================Launcher::ShowAppsCustomizeHelper listener=%p",listener.Get());
        IAnimator::Probe(mStateAnimation)->AddListener(listener);

        if (workspaceAnim != NULL) {
            AutoPtr<IAnimatorSetBuilder> builder;
            mStateAnimation->Play(IAnimator::Probe(workspaceAnim), (IAnimatorSetBuilder**)&builder);
        }
Slogger::D("Launcher", "========================Launcher::ShowAppsCustomizeHelper 8");
        Boolean delayAnim = FALSE;

        DispatchOnLauncherTransitionPrepare(IView::Probe(fromView), animated, FALSE);
        DispatchOnLauncherTransitionPrepare(IView::Probe(toView), animated, FALSE);
Slogger::D("Launcher", "========================Launcher::ShowAppsCustomizeHelper 9");
        // If any of the objects being animated haven't been measured/laid out
        // yet, delay the animation until we get a layout pass
        AutoPtr<ILauncherTransitionable> transitionable = ILauncherTransitionable::Probe(toView);
        AutoPtr<IView> view;
        transitionable->GetContent((IView**)&view);
        Int32 width1;
        view->GetMeasuredWidth(&width1);
        Int32 width2;
        IView::Probe(mWorkspace)->GetMeasuredWidth(&width2);
        Int32 width3;
        IView::Probe(toView)->GetMeasuredWidth(&width3);
        if ((width1 == 0) || (width2 == 0) || (width3 == 0)) {
            delayAnim = TRUE;
        }
Slogger::D("Launcher", "========================Launcher::ShowAppsCustomizeHelper 10");
        AutoPtr<IAnimatorSet> stateAnimation = mStateAnimation;
        AutoPtr<IRunnable> startAnimRunnable = new MyRunnable7(this, stateAnimation,
                fromView, toView, animated, scale);
        if (delayAnim) {
            AutoPtr<IViewTreeObserver> observer;
            IView::Probe(toView)->GetViewTreeObserver((IViewTreeObserver**)&observer);
            AutoPtr<IOnGlobalLayoutListener> listener2 = new MyOnGlobalLayoutListener(startAnimRunnable,
                    toView);
            observer->AddOnGlobalLayoutListener(listener2);
        }
        else {
            startAnimRunnable->Run();
        }
Slogger::D("Launcher", "========================Launcher::ShowAppsCustomizeHelper 11");
    }
    else {
        IView::Probe(toView)->SetTranslationX(0.0f);
        IView::Probe(toView)->SetTranslationY(0.0f);
        IView::Probe(toView)->SetScaleX(1.0f);
        IView::Probe(toView)->SetScaleY(1.0f);
        IView::Probe(toView)->SetVisibility(IView::VISIBLE);
        IView::Probe(toView)->BringToFront();

        Boolean res;
        if (!springLoaded && (LauncherApplication::IsScreenLarge(&res), !res)) {
            // Hide the workspace scrollbar
            IPagedView::Probe(mWorkspace)->HideScrollingIndicator(TRUE);
            HideDockDivider();

            // Hide the search bar
            if (mSearchDropTargetBar != NULL) {
                mSearchDropTargetBar->HideSearchBar(FALSE);
            }
        }
        DispatchOnLauncherTransitionPrepare(IView::Probe(fromView), animated, FALSE);
        DispatchOnLauncherTransitionStart(IView::Probe(fromView), animated, FALSE);
        DispatchOnLauncherTransitionEnd(IView::Probe(fromView), animated, FALSE);
        DispatchOnLauncherTransitionPrepare(IView::Probe(toView), animated, FALSE);
        DispatchOnLauncherTransitionStart(IView::Probe(toView), animated, FALSE);
        DispatchOnLauncherTransitionEnd(IView::Probe(toView), animated, FALSE);
        UpdateWallpaperVisibility(FALSE);
    }
Slogger::D("Launcher", "========================Launcher::ShowAppsCustomizeHelper 12 return");
}

void Launcher::HideAppsCustomizeHelper(
    /* [in] */ LauncherState toState,
    /* [in] */ Boolean animated,
    /* [in] */ Boolean springLoaded,
    /* [in] */ IRunnable* onCompleteRunnable)
{
Slogger::D("Launcher", "========================Launcher::HideAppsCustomizeHelper 1");
    if (mStateAnimation != NULL) {
        IAnimator::Probe(mStateAnimation)->SetDuration(0);
        IAnimator::Probe(mStateAnimation)->Cancel();
        mStateAnimation = NULL;
    }
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);

    Int32 duration;
    res->GetInteger(
            Elastos::Droid::Launcher2::R::integer::config_appsCustomizeZoomOutTime,
            &duration);
    Int32 fadeOutDuration;
    res->GetInteger(
            Elastos::Droid::Launcher2::R::integer::config_appsCustomizeFadeOutTime,
            &fadeOutDuration);
    Int32 tmp;
    res->GetInteger(
            Elastos::Droid::Launcher2::R::integer::config_appsCustomizeZoomScaleFactor,
            &tmp);
    Float scaleFactor = (Float)tmp;
    AutoPtr<IView> fromView = IView::Probe(mAppsCustomizeTabHost);
    AutoPtr<IView> toView = IView::Probe(mWorkspace);
    AutoPtr<IAnimator> workspaceAnim;

    if (toState == Launcher_WORKSPACE) {
        Int32 stagger;
        res->GetInteger(
                Elastos::Droid::Launcher2::R::integer::config_appsCustomizeWorkspaceAnimationStagger,
                &stagger);
        mWorkspace->GetChangeStateAnimation(
                State_NORMAL, animated, stagger, (IAnimator**)&workspaceAnim);
    }
    else if (toState == Launcher_APPS_CUSTOMIZE_SPRING_LOADED) {
        mWorkspace->GetChangeStateAnimation(
                State_SPRING_LOADED, animated, (IAnimator**)&workspaceAnim);
    }

    SetPivotsForZoom(fromView, scaleFactor);
    UpdateWallpaperVisibility(TRUE);
    ShowHotseat(animated);
    if (animated) {
        AutoPtr<LauncherViewPropertyAnimator> scaleAnim =
                new LauncherViewPropertyAnimator(fromView);
        scaleAnim->ScaleX(scaleFactor);
        scaleAnim->ScaleY(scaleFactor);
        scaleAnim->SetDuration(duration);
        AutoPtr<ITimeInterpolator> polator = new Workspace::ZoomInInterpolator();
        scaleAnim->SetInterpolator(polator);

        AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(2);
        (*array)[0] = 1.0f;
        (*array)[1] = 0.0f;
        AutoPtr<IObjectAnimator> alphaAnim = LauncherAnimUtils::OfFloat(fromView, String("alpha"), array);
        IAnimator::Probe(alphaAnim)->SetDuration(fadeOutDuration);
        AutoPtr<IAccelerateDecelerateInterpolator> value;
        CAccelerateDecelerateInterpolator::New((IAccelerateDecelerateInterpolator**)&value);
        IAnimator::Probe(alphaAnim)->SetInterpolator(ITimeInterpolator::Probe(value));

        AutoPtr<IAnimatorUpdateListener> listener = new MyAnimatorUpdateListener2(this, fromView, toView);
        IValueAnimator::Probe(alphaAnim)->AddUpdateListener(listener);

        mStateAnimation = LauncherAnimUtils::CreateAnimatorSet();

        DispatchOnLauncherTransitionPrepare(fromView, animated, TRUE);
        DispatchOnLauncherTransitionPrepare(toView, animated, TRUE);
        IPagedView::Probe(mAppsCustomizeContent)->PauseScrolling();

        AutoPtr<IAnimatorListener> listener2 = new MyAnimatorListenerAdapter3(this, fromView,
                toView, animated, onCompleteRunnable);
Slogger::D("Launcher", "=======Launcher::HideAppsCustomizeHelper listener2=%p",listener2.Get());
        IAnimator::Probe(mStateAnimation)->AddListener(listener2);

        AutoPtr<ArrayOf<IAnimator*> > array2 = ArrayOf<IAnimator*>::Alloc(2);
        array2->Set(0, IAnimator::Probe(scaleAnim));
        array2->Set(1, IAnimator::Probe(alphaAnim));
        mStateAnimation->PlayTogether(array2);
        if (workspaceAnim != NULL) {
            AutoPtr<IAnimatorSetBuilder> builder;
            mStateAnimation->Play(workspaceAnim, (IAnimatorSetBuilder**)&builder);
        }
        DispatchOnLauncherTransitionStart(fromView, animated, TRUE);
        DispatchOnLauncherTransitionStart(toView, animated, TRUE);
        LauncherAnimUtils::StartAnimationAfterNextDraw(IAnimator::Probe(mStateAnimation), toView);
    }
    else {
        fromView->SetVisibility(IView::GONE);
        DispatchOnLauncherTransitionPrepare(fromView, animated, TRUE);
        DispatchOnLauncherTransitionStart(fromView, animated, TRUE);
        DispatchOnLauncherTransitionEnd(fromView, animated, TRUE);
        DispatchOnLauncherTransitionPrepare(toView, animated, TRUE);
        DispatchOnLauncherTransitionStart(toView, animated, TRUE);
        DispatchOnLauncherTransitionEnd(toView, animated, TRUE);
        IPagedView::Probe(mWorkspace)->HideScrollingIndicator(FALSE);
    }
Slogger::D("Launcher", "========================Launcher::HideAppsCustomizeHelper return");
}

ECode Launcher::OnTrimMemory(
    /* [in] */ Int32 level)
{
    Activity::OnTrimMemory(level);
    if (level >= IComponentCallbacks2::TRIM_MEMORY_MODERATE) {
        return mAppsCustomizeTabHost->OnTrimMemory();
    }
    return NOERROR;
}

ECode Launcher::OnWindowFocusChanged(
    /* [in] */ Boolean hasFocus)
{
    if (!hasFocus) {
        // When another window occludes launcher (like the notification shade, or recents),
        // ensure that we enable the wallpaper flag so that transitions are done correctly.
        return UpdateWallpaperVisibility(true);
    }
    else {
        // When launcher has focus again, disable the wallpaper if we are in AllApps
        AutoPtr<IRunnable> r = new MyRunnable8(this);
        Boolean res;
        return IView::Probe(mWorkspace)->PostDelayed(r, 500, &res);
    }
    return NOERROR;
}

ECode Launcher::ShowWorkspace(
    /* [in] */ Boolean animated)
{
    return ShowWorkspace(animated, NULL);
}

ECode Launcher::ShowWorkspace(
    /* [in] */ Boolean animated,
    /* [in] */ IRunnable* onCompleteRunnable)
{
    if (mState != Launcher_WORKSPACE) {
        Boolean wasInSpringLoadedMode = (mState == Launcher_APPS_CUSTOMIZE_SPRING_LOADED);
        IView::Probe(mWorkspace)->SetVisibility(IView::VISIBLE);
        HideAppsCustomizeHelper(Launcher_WORKSPACE, animated, FALSE, onCompleteRunnable);

        // Show the search bar (only animate if we were showing the drop target bar in spring
        // loaded mode)
        if (mSearchDropTargetBar != NULL) {
            mSearchDropTargetBar->ShowSearchBar(wasInSpringLoadedMode);
        }

        // We only need to animate in the dock divider if we're going from spring loaded mode
        ShowDockDivider(animated && wasInSpringLoadedMode);

        // Set focus to the AppsCustomize button
        if (mAllAppsButton != NULL) {
            Boolean res;
            mAllAppsButton->RequestFocus(&res);
        }
    }

    IPagedView::Probe(mWorkspace)->FlashScrollingIndicator(animated);

    // Change the state *after* we've called all the transition code
    mState = Launcher_WORKSPACE;

    // Resume the auto-advance of widgets
    mUserPresent = TRUE;
    UpdateRunning();

    // Send an accessibility event to announce the context change
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    AutoPtr<IView> v;
    window->GetDecorView((IView**)&v);
    return v->SendAccessibilityEvent(IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED);
}

ECode Launcher::ShowAllApps(
    /* [in] */ Boolean animated)
{
Slogger::D("Launcher", "========================Launcher::ShowAllApps 1");
    if (mState != Launcher_WORKSPACE) return NOERROR;

    ShowAppsCustomizeHelper(animated, FALSE);
Slogger::D("Launcher", "========================Launcher::ShowAllApps 2");
    Boolean res;
    IView::Probe(mAppsCustomizeTabHost)->RequestFocus(&res);
Slogger::D("Launcher", "========================Launcher::ShowAllApps 3");
    // Change the state *after* we've called all the transition code
    mState = Launcher_APPS_CUSTOMIZE;
Slogger::D("Launcher", "========================Launcher::ShowAllApps 4");
    // Pause the auto-advance of widgets until we are out of AllApps
    mUserPresent = FALSE;
    UpdateRunning();
    CloseFolder();
Slogger::D("Launcher", "========================Launcher::ShowAllApps 5");
    // Send an accessibility event to announce the context change
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    AutoPtr<IView> v;
    window->GetDecorView((IView**)&v);
Slogger::D("Launcher", "========================Launcher::ShowAllApps 6");
    return v->SendAccessibilityEvent(IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED);
}

ECode Launcher::EnterSpringLoadedDragMode()
{
    Boolean res;
    IsAllAppsVisible(&res);
    if (res) {
        HideAppsCustomizeHelper(Launcher_APPS_CUSTOMIZE_SPRING_LOADED, TRUE, TRUE, NULL);
        HideDockDivider();
        mState = Launcher_APPS_CUSTOMIZE_SPRING_LOADED;
    }
    return NOERROR;
}

ECode Launcher::ExitSpringLoadedDragModeDelayed(
    /* [in] */ Boolean successfulDrop,
    /* [in] */ Boolean extendedDelay,
    /* [in] */ IRunnable* onCompleteRunnable)
{
    if (mState != Launcher_APPS_CUSTOMIZE_SPRING_LOADED) return NOERROR;

    AutoPtr<IRunnable> r = new MyRunnable9(this, successfulDrop, onCompleteRunnable);
    Boolean res;
    mHandler->PostDelayed(r, (Int64)(extendedDelay ?
            EXIT_SPRINGLOADED_MODE_LONG_TIMEOUT :
            EXIT_SPRINGLOADED_MODE_SHORT_TIMEOUT), &res);
    return NOERROR;
}

ECode Launcher::ExitSpringLoadedDragMode()
{
    if (mState == Launcher_APPS_CUSTOMIZE_SPRING_LOADED) {
        Boolean animated = TRUE;
        Boolean springLoaded = TRUE;
        ShowAppsCustomizeHelper(animated, springLoaded);
        mState = Launcher_APPS_CUSTOMIZE;
    }
    return NOERROR;
    // Otherwise, we are not in spring loaded mode, so don't do anything.
}

ECode Launcher::HideDockDivider()
{
    if (mQsbDivider != NULL && mDockDivider != NULL) {
        mQsbDivider->SetVisibility(IView::INVISIBLE);
        return mDockDivider->SetVisibility(IView::INVISIBLE);
    }
    return NOERROR;
}

ECode Launcher::ShowDockDivider(
    /* [in] */ Boolean animated)
{
    if (mQsbDivider != NULL && mDockDivider != NULL) {
        mQsbDivider->SetVisibility(IView::VISIBLE);
        mDockDivider->SetVisibility(IView::VISIBLE);
        if (mDividerAnimator != NULL) {
            IAnimator::Probe(mDividerAnimator)->Cancel();
            mQsbDivider->SetAlpha(1.0f);
            mDockDivider->SetAlpha(1.0f);
            mDividerAnimator = NULL;
        }
        if (animated) {
            mDividerAnimator = LauncherAnimUtils::CreateAnimatorSet();
            AutoPtr<ArrayOf<Float> > array1 = ArrayOf<Float>::Alloc(1);
            (*array1)[0] = 1.0f;
            AutoPtr<IObjectAnimator> animator1 = LauncherAnimUtils::OfFloat(mQsbDivider,
                    String("alpha"), array1);
            AutoPtr<ArrayOf<Float> > array2 = ArrayOf<Float>::Alloc(1);
            (*array2)[0] = 1.0f;
            AutoPtr<IObjectAnimator> animator2 = LauncherAnimUtils::OfFloat(mDockDivider,
                    String("alpha"), array2);

            AutoPtr<ArrayOf<IAnimator*> > array3 = ArrayOf<IAnimator*>::Alloc(2);
            array3->Set(0, IAnimator::Probe(animator1));
            array3->Set(1, IAnimator::Probe(animator2));
            mDividerAnimator->PlayTogether(array3);
            Int32 duration = 0;
            if (mSearchDropTargetBar != NULL) {
                mSearchDropTargetBar->GetTransitionInDuration(&duration);
            }
            IAnimator::Probe(mDividerAnimator)->SetDuration(duration);
            IAnimator::Probe(mDividerAnimator)->Start();
        }
    }
    return NOERROR;
}

ECode Launcher::LockAllApps()
{
    return NOERROR;
}

ECode Launcher::UnlockAllApps()
{
    return NOERROR;
}

ECode Launcher::ShowHotseat(
    /* [in] */ Boolean animated)
{
    Boolean res;
    LauncherApplication::IsScreenLarge(&res);
    if (!res) {
        if (animated) {
            Float tmp;
            IView::Probe(mHotseat)->GetAlpha(&tmp);
            if (tmp != 1.0f) {
                Int32 duration = 0;
                if (mSearchDropTargetBar != NULL) {
                    mSearchDropTargetBar->GetTransitionInDuration(&duration);
                }
                AutoPtr<IViewPropertyAnimator> res;
                IView::Probe(mHotseat)->Animate((IViewPropertyAnimator**)&res);
                res->Alpha(1.0f);
                res->SetDuration(duration);
            }
        }
        else {
            IView::Probe(mHotseat)->SetAlpha(1.0f);
        }
    }
    return NOERROR;
}

ECode Launcher::HideHotseat(
    /* [in] */ Boolean animated)
{
    Boolean res;
    LauncherApplication::IsScreenLarge(&res);
    if (!res) {
        if (animated) {
            Float tmp;
            IView::Probe(mHotseat)->GetAlpha(&tmp);
            if (tmp != 0.0f) {
                Int32 duration = 0;
                if (mSearchDropTargetBar != NULL) {
                    mSearchDropTargetBar->GetTransitionOutDuration(&duration);
                }
                AutoPtr<IViewPropertyAnimator> res;
                IView::Probe(mHotseat)->Animate((IViewPropertyAnimator**)&res);
                res->Alpha(0.0f);
                res->SetDuration(duration);
            }
        }
        else {
            IView::Probe(mHotseat)->SetAlpha(0.0f);
        }
    }
    return NOERROR;
}

ECode Launcher::AddExternalItemToScreen(
    /* [in] */ IItemInfo* itemInfo,
    /* [in] */ ICellLayout* layout)
{
    Boolean res;
    mWorkspace->AddExternalItemToScreen(itemInfo, layout, &res);
    if (!res) {
        Boolean tmp;
        IsHotseatLayout(IView::Probe(layout), &tmp);
        return ShowOutOfSpaceMessage(tmp);
    }
    return NOERROR;
}

Int32 Launcher::GetCurrentOrientationIndexForGlobalIcons()
{
    // default - 0, landscape - 1
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<IConfiguration> config;
    resources->GetConfiguration((IConfiguration**)&config);
    Int32 orientation;
    config->GetOrientation(&orientation);
    switch (orientation) {
        case IConfiguration::ORIENTATION_LANDSCAPE:
            return 1;
        default:
            return 0;
    }
    return 0;
}

AutoPtr<IDrawable> Launcher::GetExternalPackageToolbarIcon(
    /* [in] */ IComponentName* activityName,
    /* [in] */ const String& resourceName)
{
//     //try {
//     ECode ec;
//     AutoPtr<IPackageManager> packageManager;
//     FAIL_GOTO(ec = GetPackageManager((IPackageManager**)&packageManager), ERROR)
//     // Look for the toolbar icon specified in the activity meta-data
//     AutoPtr<IActivityInfo> info;
//     AutoPtr<IBundle> metaData;
//     FAIL_GOTO(ec = packageManager->GetActivityInfo(
//             activityName, IPackageManager::GET_META_DATA, (IActivityInfo**)&info), ERROR)
//     IPackageItemInfo::Probe(info)->GetMetaData((IBundle**)&metaData);
//     if (metaData != NULL) {
//         Int32 iconResId;
//         FAIL_GOTO(ec = metaData->GetInt32(resourceName, &iconResId), ERROR)
//         if (iconResId != 0) {
//             AutoPtr<IResources> res;
//             FAIL_GOTO(ec = packageManager->GetResourcesForActivity(activityName,
//                     (IResources**)&res), ERROR)
//             AutoPtr<IDrawable> tmp;
//             FAIL_GOTO(ec = res->GetDrawable(iconResId, (IDrawable**)&tmp), ERROR)
//             return tmp;
//         }
//     }
// ERROR:
//     //} catch (NameNotFoundException e) {
//     if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
//         // This can happen if the activity defines an invalid drawable
//         String name;
//         activityName->FlattenToShortString(&name);
//         StringBuilder sb;
//         sb += "Failed to load toolbar icon; ";
//         sb += name;
//         sb += " not found";
//         sb += ec;
//         Slogger::W(TAG, sb.ToString());
//     }
//     //} catch (Resources.NotFoundException nfe) {
//     if (ec == (ECode)E_RESOURCES_NOT_FOUND_EXCEPTION ) {
//         // This can happen if the activity defines an invalid drawable
//         String name;
//         activityName->FlattenToShortString(&name);
//         StringBuilder sb;
//         sb += "Failed to load toolbar icon from ";
//         sb += name;
//         sb += ec;
//         Slogger::W(TAG, sb.ToString());
//     }
//     //}
    return NULL;
}

AutoPtr<IDrawableConstantState> Launcher::UpdateTextButtonWithIconFromExternalActivity(
    /* [in] */ Int32 buttonId,
    /* [in] */ IComponentName* activityName,
    /* [in] */ Int32 fallbackDrawableId,
    /* [in] */ const String& toolbarResourceName)
{
    AutoPtr<IDrawable> toolbarIcon = GetExternalPackageToolbarIcon(activityName, toolbarResourceName);
    AutoPtr<IResources> r;
    GetResources((IResources**)&r);
    Int32 w;
    r->GetDimensionPixelSize(
            Elastos::Droid::Launcher2::R::dimen::toolbar_external_icon_width, &w);
    Int32 h;
    r->GetDimensionPixelSize(
            Elastos::Droid::Launcher2::R::dimen::toolbar_external_icon_height, &h);

    AutoPtr<IView> view;
    FindViewById(buttonId, (IView**)&view);
    AutoPtr<ITextView> button = ITextView::Probe(view);
    // If we were unable to find the icon via the meta-data, use a generic one
    if (toolbarIcon == NULL) {
        r->GetDrawable(fallbackDrawableId, (IDrawable**)&toolbarIcon);
        toolbarIcon->SetBounds(0, 0, w, h);
        if (button != NULL) {
            button->SetCompoundDrawables(toolbarIcon, NULL, NULL, NULL);
        }
        return NULL;
    }
    else {
        toolbarIcon->SetBounds(0, 0, w, h);
        if (button != NULL) {
            button->SetCompoundDrawables(toolbarIcon, NULL, NULL, NULL);
        }
        AutoPtr<IDrawableConstantState> tmp;
        toolbarIcon->GetConstantState((IDrawableConstantState**)&tmp);
        return tmp;
    }
}

AutoPtr<IDrawableConstantState> Launcher::UpdateButtonWithIconFromExternalActivity(
    /* [in] */ Int32 buttonId,
    /* [in] */ IComponentName* activityName,
    /* [in] */ Int32 fallbackDrawableId,
    /* [in] */ const String& toolbarResourceName)
{
    AutoPtr<IView> view;
    FindViewById(buttonId, (IView**)&view);
    AutoPtr<IImageView> button = IImageView::Probe(view);
    AutoPtr<IDrawable> toolbarIcon = GetExternalPackageToolbarIcon(activityName, toolbarResourceName);

    if (button != NULL) {
        // If we were unable to find the icon via the meta-data, use a
        // generic one
        if (toolbarIcon == NULL) {
            button->SetImageResource(fallbackDrawableId);
        }
        else {
            button->SetImageDrawable(toolbarIcon);
        }
    }

    if (toolbarIcon != NULL) {
        AutoPtr<IDrawableConstantState> tmp;
        toolbarIcon->GetConstantState((IDrawableConstantState**)&tmp);
        return tmp;
    }
    else {
        return NULL;
    }
}

void Launcher::UpdateTextButtonWithDrawable(
    /* [in] */ Int32 buttonId,
    /* [in] */ IDrawable* d)
{
    AutoPtr<IView> view;
    FindViewById(buttonId, (IView**)&view);
    AutoPtr<ITextView> button = ITextView::Probe(view);
    button->SetCompoundDrawables(d, NULL, NULL, NULL);
}

void Launcher::UpdateButtonWithDrawable(
    /* [in] */ Int32 buttonId,
    /* [in] */ IDrawableConstantState* d)
{
    AutoPtr<IView> view;
    FindViewById(buttonId, (IView**)&view);
    AutoPtr<IImageView> button = IImageView::Probe(view);
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<IDrawable> drawable;
    d->NewDrawable(resources, (IDrawable**)&drawable);
    button->SetImageDrawable(drawable);
}

void Launcher::InvalidatePressedFocusedStates(
    /* [in] */ IView* container,
    /* [in] */ IView* button)
{
    if (IHolographicLinearLayout::Probe(container) != NULL) {
        AutoPtr<IHolographicLinearLayout> layout = IHolographicLinearLayout::Probe(container);
        layout->InvalidatePressedFocusedStates();
    }
    else if (IHolographicImageView::Probe(button) != NULL) {
        AutoPtr<IHolographicImageView> view = IHolographicImageView::Probe(button);
        view->InvalidatePressedFocusedStates();
    }
    return;
}

Boolean Launcher::UpdateGlobalSearchIcon()
{
    AutoPtr<IView> searchButtonContainer;
    FindViewById(Elastos::Droid::Launcher2::R::id::search_button_container,
            (IView**)&searchButtonContainer);
    AutoPtr<IView> tmp;
    FindViewById(Elastos::Droid::Launcher2::R::id::search_button,
            (IView**)&tmp);
    AutoPtr<IImageView> searchButton = IImageView::Probe(tmp);
    AutoPtr<IView> voiceButtonContainer;
    FindViewById(Elastos::Droid::Launcher2::R::id::voice_button_container,
            (IView**)&voiceButtonContainer);
    AutoPtr<IView> voiceButton;
    FindViewById(Elastos::Droid::Launcher2::R::id::voice_button,
            (IView**)&voiceButton);
    AutoPtr<IView> voiceButtonProxy;
    FindViewById(Elastos::Droid::Launcher2::R::id::voice_button_proxy,
            (IView**)&voiceButtonProxy);

    AutoPtr<IInterface> obj;
    GetSystemService(IContext::SEARCH_SERVICE, (IInterface**)&obj);
    AutoPtr<ISearchManager> searchManager = ISearchManager::Probe(obj);
Slogger::E("Launcher", "[snow]=================TODO=========need search service ");
    // AutoPtr<IComponentName> activityName;
    // searchManager->GetGlobalSearchActivity((IComponentName**)&activityName);
    // if (activityName != NULL) {
    //     Int32 coi = GetCurrentOrientationIndexForGlobalIcons();
    //     sGlobalSearchIcon->Set(coi, UpdateButtonWithIconFromExternalActivity(
    //             Elastos::Droid::Launcher2::R::id::search_button, activityName,
    //             Elastos::Droid::Launcher2::R::drawable::ic_home_search_normal_holo,
    //             TOOLBAR_SEARCH_ICON_METADATA_NAME));
    //     if ((*sGlobalSearchIcon)[coi] == NULL) {
    //         sGlobalSearchIcon->Set(coi, UpdateButtonWithIconFromExternalActivity(
    //                 Elastos::Droid::Launcher2::R::id::search_button, activityName,
    //                 Elastos::Droid::Launcher2::R::drawable::ic_home_search_normal_holo,
    //                 TOOLBAR_ICON_METADATA_NAME));
    //     }

    //     if (searchButtonContainer != NULL) {
    //         searchButtonContainer->SetVisibility(IView::VISIBLE);
    //     }
    //     IView::Probe(searchButton)->SetVisibility(IView::VISIBLE);
    //     InvalidatePressedFocusedStates(searchButtonContainer, IView::Probe(searchButton));
    //     return TRUE;
    // }
    // else {
    //     // We disable both search and voice search when there is no global search provider
    //     if (searchButtonContainer != NULL) {
    //         searchButtonContainer->SetVisibility(IView::GONE);
    //     }
    //     if (voiceButtonContainer != NULL) {
    //         voiceButtonContainer->SetVisibility(IView::GONE);
    //     }
    //     IView::Probe(searchButton)->SetVisibility(IView::GONE);
    //     voiceButton->SetVisibility(IView::GONE);
    //     if (voiceButtonProxy != NULL) {
    //         IView::Probe(voiceButtonProxy)->SetVisibility(IView::GONE);
    //     }
    //     return FALSE;
    // }
return FALSE;
}

void Launcher::UpdateGlobalSearchIcon(
    /* [in] */ IDrawableConstantState* d)
{
    AutoPtr<IView> searchButtonContainer;
    FindViewById(
            Elastos::Droid::Launcher2::R::id::search_button_container,
            (IView**)&searchButtonContainer);
    AutoPtr<IView> searchButton;
    FindViewById(
            Elastos::Droid::Launcher2::R::id::search_button,
            (IView**)&searchButton);
    UpdateButtonWithDrawable(Elastos::Droid::Launcher2::R::id::search_button, d);
    InvalidatePressedFocusedStates(searchButtonContainer, searchButton);
}

Boolean Launcher::UpdateVoiceSearchIcon(
    /* [in] */ Boolean searchVisible)
{
    AutoPtr<IView> voiceButtonContainer;
    FindViewById(
            Elastos::Droid::Launcher2::R::id::voice_button_container,
            (IView**)&voiceButtonContainer);
    AutoPtr<IView> voiceButton;
    FindViewById(
            Elastos::Droid::Launcher2::R::id::voice_button,
            (IView**)&voiceButton);
    AutoPtr<IView> voiceButtonProxy;
    FindViewById(
            Elastos::Droid::Launcher2::R::id::voice_button_proxy,
            (IView**)&voiceButtonProxy);

    // We only show/update the voice search icon if the search icon is enabled as well
    AutoPtr<IInterface> obj;
    GetSystemService(IContext::SEARCH_SERVICE, (IInterface**)&obj);
    AutoPtr<ISearchManager> searchManager = ISearchManager::Probe(obj);
    AutoPtr<IComponentName> globalSearchActivity;
    //searchManager->GetGlobalSearchActivity((IComponentName**)&globalSearchActivity);
Slogger::D("Launcher", "[snow]========================TODO==============nedd search service");
    AutoPtr<IComponentName> activityName;
    // if (globalSearchActivity != NULL) {
    //     // Check if the global search activity handles voice search
    //     AutoPtr<IIntent> intent;
    //     CIntent::New(IRecognizerIntent::ACTION_WEB_SEARCH, (IIntent**)&intent);
    //     String str;
    //     globalSearchActivity->GetPackageName(&str);
    //     intent->SetPackage(str);
    //     AutoPtr<IPackageManager> packageManage;
    //     GetPackageManager((IPackageManager**)&packageManage);
    //     intent->ResolveActivity(packageManage, (IComponentName**)&activityName);
    // }

    if (activityName == NULL) {
        // Fallback: check if an activity other than the global search activity
        // resolves this
        AutoPtr<IIntent> intent;
        CIntent::New(IRecognizerIntent::ACTION_WEB_SEARCH, (IIntent**)&intent);
        AutoPtr<IPackageManager> packageManage;
        GetPackageManager((IPackageManager**)&packageManage);
        intent->ResolveActivity(packageManage, (IComponentName**)&activityName);
    }
    if (searchVisible && activityName != NULL) {
        Int32 coi = GetCurrentOrientationIndexForGlobalIcons();
        sVoiceSearchIcon->Set(coi, UpdateButtonWithIconFromExternalActivity(
                Elastos::Droid::Launcher2::R::id::voice_button, activityName,
                Elastos::Droid::Launcher2::R::drawable::ic_home_voice_search_holo,
                TOOLBAR_VOICE_SEARCH_ICON_METADATA_NAME));
        if ((*sVoiceSearchIcon)[coi] == NULL) {
            sVoiceSearchIcon->Set(coi, UpdateButtonWithIconFromExternalActivity(
                    Elastos::Droid::Launcher2::R::id::voice_button, activityName,
                    Elastos::Droid::Launcher2::R::drawable::ic_home_voice_search_holo,
                    TOOLBAR_ICON_METADATA_NAME));
        }
        if (voiceButtonContainer != NULL) {
            voiceButtonContainer->SetVisibility(IView::VISIBLE);
        }
        voiceButton->SetVisibility(IView::VISIBLE);
        if (voiceButtonProxy != NULL) {
            voiceButtonProxy->SetVisibility(IView::VISIBLE);
        }
        InvalidatePressedFocusedStates(voiceButtonContainer, voiceButton);
        return TRUE;
    }
    else {
        if (voiceButtonContainer != NULL) {
            voiceButtonContainer->SetVisibility(IView::GONE);
        }
        voiceButton->SetVisibility(IView::GONE);
        if (voiceButtonProxy != NULL) {
            voiceButtonProxy->SetVisibility(IView::GONE);
        }
        return FALSE;
    }
}

void Launcher::UpdateVoiceSearchIcon(
    /* [in] */ IDrawableConstantState* d)
{
    AutoPtr<IView> voiceButtonContainer;
    FindViewById(
            Elastos::Droid::Launcher2::R::id::voice_button_container,
            (IView**)&voiceButtonContainer);
    AutoPtr<IView> voiceButton;
    FindViewById(
            Elastos::Droid::Launcher2::R::id::voice_button,
            (IView**)&voiceButton);
    UpdateButtonWithDrawable(Elastos::Droid::Launcher2::R::id::voice_button, d);
    InvalidatePressedFocusedStates(voiceButtonContainer, voiceButton);
}

void Launcher::UpdateAppMarketIcon()
{
    AutoPtr<IView> marketButton;
    FindViewById(
            Elastos::Droid::Launcher2::R::id::market_button,
            (IView**)&marketButton);
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&intent);
    intent->AddCategory(IIntent::CATEGORY_APP_MARKET);
    // Find the app market activity by resolving an intent.
    // (If multiple app markets are installed, it will return the ResolverActivity.)
    AutoPtr<IPackageManager> packageManage;
    GetPackageManager((IPackageManager**)&packageManage);
    AutoPtr<IComponentName> activityName;
    intent->ResolveActivity(packageManage, (IComponentName**)&activityName);
    if (activityName != NULL) {
        Int32 coi = GetCurrentOrientationIndexForGlobalIcons();
        mAppMarketIntent = intent;
        sAppMarketIcon->Set(coi, UpdateTextButtonWithIconFromExternalActivity(
                Elastos::Droid::Launcher2::R::id::market_button, activityName,
                Elastos::Droid::Launcher2::R::drawable::ic_launcher_market_holo,
                TOOLBAR_ICON_METADATA_NAME));
        marketButton->SetVisibility(IView::VISIBLE);
    }
    else {
        // We should hide and disable the view so that we don't try and restore the visibility
        // of it when we swap between drag & normal states from IconDropTarget subclasses.
        marketButton->SetVisibility(IView::GONE);
        marketButton->SetEnabled(FALSE);
    }
}

void Launcher::UpdateAppMarketIcon(
    /* [in] */ IDrawableConstantState* d)
{
    // Ensure that the new drawable we are creating has the approprate toolbar icon bounds
    AutoPtr<IResources> r;
    GetResources((IResources**)&r);
    AutoPtr<IDrawable> marketIconDrawable;
    d->NewDrawable(r, (IDrawable**)&marketIconDrawable);
    Int32 w;
    r->GetDimensionPixelSize(
            Elastos::Droid::Launcher2::R::dimen::toolbar_external_icon_width, &w);
    Int32 h;
    r->GetDimensionPixelSize(
            Elastos::Droid::Launcher2::R::dimen::toolbar_external_icon_height, &h);
    marketIconDrawable->SetBounds(0, 0, w, h);

    UpdateTextButtonWithDrawable(Elastos::Droid::Launcher2::R::id::market_button, marketIconDrawable);
}

ECode Launcher::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* outresult)
{
    VALIDATE_NOT_NULL(outresult);

    Boolean result;
    Activity::DispatchPopulateAccessibilityEvent(event, &result);
    AutoPtr<IList> text;
    IAccessibilityRecord::Probe(event)->GetText((IList**)&text);
    text->Clear();
    // Populate event with a fake title based on the current state.
    if (mState == Launcher_APPS_CUSTOMIZE) {
        String str;
        GetString(
                Elastos::Droid::Launcher2::R::string::all_apps_button_label,
                &str);
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
        text->Add(TO_IINTERFACE(cchar));
    }
    else {
        String str;
        GetString(
                Elastos::Droid::Launcher2::R::string::all_apps_home_button_label,
                &str);
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
        text->Add(TO_IINTERFACE(cchar));
    }
    *outresult = result;
    return NOERROR;
}

Boolean Launcher::WaitUntilResume(
    /* [in] */ IRunnable* run,
    /* [in] */ Boolean deletePreviousRunnables)
{
    if (mPaused) {
        Slogger::I(TAG, "Deferring update until onResume");
        if (deletePreviousRunnables) {
            Boolean res;
            while (mOnResumeCallbacks->Remove(TO_IINTERFACE(run), &res), res) {
            }
        }
        mOnResumeCallbacks->Add(TO_IINTERFACE(run));
        return TRUE;
    }
    else {
        return FALSE;
    }
}

Boolean Launcher::WaitUntilResume(
    /* [in] */ IRunnable* run)
{
    return WaitUntilResume(run, FALSE);
}

ECode Launcher::SetLoadOnResume(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (mPaused) {
        Slogger::I(TAG, "setLoadOnResume");
        mOnResumeNeedsLoad = TRUE;
        return TRUE;
    } else {
        return FALSE;
    }
}

ECode Launcher::GetCurrentWorkspaceScreen(
    /* [out] */ Int32* screen)
{
    VALIDATE_NOT_NULL(screen);

    if (mWorkspace != NULL) {
        return IPagedView::Probe(mWorkspace)->GetCurrentPage(screen);
    }
    else {
        *screen = SCREEN_COUNT / 2;
        return NOERROR;
    }
    return NOERROR;
}

ECode Launcher::StartBinding()
{
    // If we're starting binding all over again, clear any bind calls we'd postponed in
    // the past (see waitUntilResume) -- we don't need them since we're starting binding
    // from scratch again
    mOnResumeCallbacks->Clear();

    AutoPtr<IWorkspace> workspace = mWorkspace;
    mNewShortcutAnimatePage = -1;
    mNewShortcutAnimateViews->Clear();
    mWorkspace->ClearDropTargets();
    Int32 count;
    IViewGroup::Probe(workspace)->GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        // Use removeAllViewsInLayout() to avoid an extra requestLayout() and invalidate().
        AutoPtr<IView> view;
        IViewGroup::Probe(workspace)->GetChildAt(i, (IView**)&view);
        AutoPtr<ICellLayout> layoutParent = ICellLayout::Probe(view);
        IViewGroup::Probe(layoutParent)->RemoveAllViewsInLayout();
    }
    mWidgetsToAdvance->Clear();
    if (mHotseat != NULL) {
        mHotseat->ResetLayout();
    }
    return NOERROR;
}

ECode Launcher::BindItems(
    /* [in] */ IArrayList* shortcuts,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    AutoPtr<IRunnable> r = new MyRunnable10(this, shortcuts, start, end);
    if (WaitUntilResume(r)) {
        return NOERROR;
    }

    // Get the list of added shortcuts and intersect them with the set of shortcuts here
    AutoPtr<ISet> newApps;
    CHashSet::New((ISet**)&newApps);
    mSharedPrefs->GetStringSet(
            IInstallShortcutReceiver::NEW_APPS_LIST_KEY, newApps, (ISet**)&newApps);

    AutoPtr<IWorkspace> workspace = mWorkspace;
    for (Int32 i = start; i < end; i++) {
        AutoPtr<IInterface> obj;
        shortcuts->Get(i, (IInterface**)&obj);
        AutoPtr<IItemInfo> item = IItemInfo::Probe(obj);
        ItemInfo* _item = (ItemInfo*)item.Get();

        // Short circuit if we are loading dock items for a configuration which has no dock
        if (_item->mContainer == LauncherSettings::Favorites::CONTAINER_HOTSEAT &&
                mHotseat == NULL) {
            continue;
        }

        switch (_item->mItemType) {
            case LauncherSettings::Favorites::ITEM_TYPE_APPLICATION:
            case LauncherSettings::Favorites::ITEM_TYPE_SHORTCUT:
            {
                ShortcutInfo* info = (ShortcutInfo*)IShortcutInfo::Probe(item);
                String uri;
                info->mIntent->ToUri(0, &uri);
                AutoPtr<IView> shortcut;
                CreateShortcut(info, (IView**)&shortcut);
                workspace->AddInScreen(shortcut, _item->mContainer, _item->mScreen, _item->mCellX,
                        _item->mCellY, 1, 1, false);
                Boolean animateIconUp = FALSE;
                {
                    AutoLock lock(newApps);
                    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(uri);
                    Boolean res;
                    newApps->Contains(TO_IINTERFACE(cchar), &res);
                    if (res) {
                        newApps->Remove(TO_IINTERFACE(cchar), &animateIconUp);
                    }
                }
                if (animateIconUp) {
                    // Prepare the view to be animated up
                    shortcut->SetAlpha(0.0f);
                    shortcut->SetScaleX(0.0f);
                    shortcut->SetScaleY(0.0f);
                    mNewShortcutAnimatePage = _item->mScreen;
                    Boolean res;
                    mNewShortcutAnimateViews->Contains(shortcut, &res);
                    if (!res) {
                        mNewShortcutAnimateViews->Add(TO_IINTERFACE(shortcut));
                    }
                }
                break;
            }
            case LauncherSettings::Favorites::ITEM_TYPE_FOLDER:
            {
                Int32 page;
                IPagedView::Probe(workspace)->GetCurrentPage(&page);
                AutoPtr<IView> view;
                IViewGroup::Probe(workspace)->GetChildAt(page, (IView**)&view);
                AutoPtr<IFolderIcon> newFolder;
                FolderIcon::FromXml(
                        Elastos::Droid::Launcher2::R::layout::folder_icon, this,
                        IViewGroup::Probe(view), IFolderInfo::Probe(item), mIconCache,
                        (IFolderIcon**)&newFolder);
                workspace->AddInScreen(IView::Probe(newFolder), _item->mContainer, _item->mScreen, _item->mCellX,
                        _item->mCellY, 1, 1, FALSE);
                break;
            }
        }
    }

    IView::Probe(workspace)->RequestLayout();
    return NOERROR;
}

ECode Launcher::BindFolders(
    /* [in] */ IHashMap* folders)
{
    AutoPtr<IRunnable> r = new MyRunnable11(this, folders);
    if (WaitUntilResume(r)) {
        return NOERROR;
    }
    sFolders->Clear();
    sFolders->PutAll(IMap::Probe(folders));
    return NOERROR;
}

ECode Launcher::BindAppWidget(
    /* [in] */ ILauncherAppWidgetInfo* item)
{
    AutoPtr<IRunnable> r = new MyRunnable12(this, item);
    if (WaitUntilResume(r)) {
        return NOERROR;
    }

    Int64 start = DEBUG_WIDGETS ? SystemClock::GetUptimeMillis() : 0;
    if (DEBUG_WIDGETS) {
        StringBuilder sb;
        sb += "bindAppWidget: ";
        sb += TO_STR(item);
        Slogger::D(TAG, sb.ToString());
    }
    AutoPtr<IWorkspace> workspace = mWorkspace;

    LauncherAppWidgetInfo* _item = (LauncherAppWidgetInfo*)item;
    Int32 appWidgetId = _item->mAppWidgetId;
    AutoPtr<IAppWidgetProviderInfo> appWidgetInfo;
    mAppWidgetManager->GetAppWidgetInfo(appWidgetId, (IAppWidgetProviderInfo**)&appWidgetInfo);
    if (DEBUG_WIDGETS) {
        AutoPtr<IComponentName> name;
        appWidgetInfo->GetProvider((IComponentName**)&name);
        StringBuilder sb;
        sb += "bindAppWidget: id=";
        sb += _item->mAppWidgetId;
        sb += " belongs to component ";
        sb += TO_STR(name);
        Slogger::D(TAG, sb.ToString());
    }

    IAppWidgetHost::Probe(mAppWidgetHost)->CreateView(this, appWidgetId,
            appWidgetInfo, (IAppWidgetHostView**)&(_item->mHostView));

    IView::Probe(_item->mHostView)->SetTag(TO_IINTERFACE(item));
    item->OnBindAppWidget(ILauncher::Probe(this));

    workspace->AddInScreen(IView::Probe(_item->mHostView), _item->mContainer, _item->mScreen, _item->mCellX,
            _item->mCellY, _item->mSpanX, _item->mSpanY, FALSE);
    AddWidgetToAutoAdvanceIfNeeded(IView::Probe(_item->mHostView), appWidgetInfo);

    IView::Probe(workspace)->RequestLayout();

    if (DEBUG_WIDGETS) {
        StringBuilder sb;
        sb += "bound widget id=";
        sb += _item->mAppWidgetId;
        sb += " in ";
        sb += SystemClock::GetUptimeMillis() - start;
        sb += "ms";
        Slogger::D(TAG, sb.ToString());
    }
    return NOERROR;
}

ECode Launcher::OnPageBoundSynchronously(
    /* [in] */ Int32 page)
{
    AutoPtr<IInteger32> p = CoreUtils::Convert(page);
    return mSynchronouslyBoundPages->Add(TO_IINTERFACE(p));
}

ECode Launcher::FinishBindingItems()
{
    AutoPtr<IRunnable> r = new MyRunnable13(this);
    if (WaitUntilResume(r)) {
        return NOERROR;
    }
    if (mSavedState != NULL) {
        Boolean res;
        IView::Probe(mWorkspace)->HasFocus(&res);
        if (!res) {
            Int32 page;
            IPagedView::Probe(mWorkspace)->GetCurrentPage(&page);
            AutoPtr<IView> view;
            IViewGroup::Probe(mWorkspace)->GetChildAt(page, (IView**)&view);
            Boolean tmp;
            view->RequestFocus(&tmp);
        }
        mSavedState = NULL;
    }

    mWorkspace->RestoreInstanceStateForRemainingPages();

    // If we received the result of any pending adds while the loader was running (e.g. the
    // widget configuration forced an orientation change), process them now.
    Int32 size;
    sPendingAddList->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        sPendingAddList->Get(i, (IInterface**)&obj);
        CompleteAdd((PendingAddArguments*)IObject::Probe(obj));
    }
    sPendingAddList->Clear();

    // Update the market app icon as necessary (the other icons will be managed in response to
    // package changes in bindSearchablesChanged()
    UpdateAppMarketIcon();

    // Animate up any icons as necessary
    if (mVisible || mWorkspaceLoading) {
        AutoPtr<IRunnable> newAppsRunnable = new MyRunnable14(this);
        Int32 page;
        IPagedView::Probe(mWorkspace)->GetCurrentPage(&page);
        Boolean willSnapPage = mNewShortcutAnimatePage > -1 &&
                mNewShortcutAnimatePage != page;
        if (CanRunNewAppsAnimation()) {
            // If the user has not interacted recently, then either snap to the new page to show
            // the new-apps animation or just run them if they are to appear on the current page
            if (willSnapPage) {
                mWorkspace->SnapToPage(mNewShortcutAnimatePage, newAppsRunnable);
            }
            else {
                RunNewAppsAnimation(FALSE);
            }
        }
        else {
            // If the user has interacted recently, then just add the items in place if they
            // are on another page (or just normally if they are added to the current page)
            RunNewAppsAnimation(willSnapPage);
        }
    }

    mWorkspaceLoading = FALSE;
    return NOERROR;
}

Boolean Launcher::CanRunNewAppsAnimation()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 ctime;
    system->GetCurrentTimeMillis(&ctime);

    Int64 time;
    mDragController->GetLastGestureUpTime(&time);
    Int64 diff = ctime - time;
    return diff > (NEW_APPS_ANIMATION_INACTIVE_TIMEOUT_SECONDS * 1000);
}

void Launcher::RunNewAppsAnimation(
    /* [in] */ Boolean immediate)
{
    AutoPtr<IAnimatorSet> anim = LauncherAnimUtils::CreateAnimatorSet();
    AutoPtr<ICollection> bounceAnims;
    CArrayList::New((ICollection**)&bounceAnims);

    // Order these new views spatially so that they animate in order
    AutoPtr<ICollections> coll;
    CCollections::AcquireSingleton((ICollections**)&coll);
    AutoPtr<IComparator> c = new MyComparator();
    coll->Sort(IList::Probe(mNewShortcutAnimateViews), c);

    // Animate each of the views in place (or show them immediately if requested)
    if (immediate) {
        Int32 size;
        mNewShortcutAnimateViews->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mNewShortcutAnimateViews->Get(i, (IInterface**)&obj);
            AutoPtr<IView> v = IView::Probe(obj);

            v->SetAlpha(1.0f);
            v->SetScaleX(1.0f);
            v->SetScaleY(1.0f);
        }
    }
    else {
        Int32 size;
        mNewShortcutAnimateViews->GetSize(&size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> obj;
            mNewShortcutAnimateViews->Get(i, (IInterface**)&obj);
            AutoPtr<IView> v = IView::Probe(obj);

            AutoPtr<IPropertyValuesHolderHelper> helper;
            CPropertyValuesHolderHelper::AcquireSingleton((IPropertyValuesHolderHelper**)&helper);

            AutoPtr<ArrayOf<Float> > values1 = ArrayOf<Float>::Alloc(1);
            (*values1)[0] = 1.0f;
            AutoPtr<IPropertyValuesHolder> holder1;
            helper->OfFloat(String("alpha"), values1, (IPropertyValuesHolder**)&holder1);

            AutoPtr<ArrayOf<Float> > values2 = ArrayOf<Float>::Alloc(1);
            (*values2)[0] = 1.0f;
            AutoPtr<IPropertyValuesHolder> holder2;
            helper->OfFloat(String("scaleX"), values2, (IPropertyValuesHolder**)&holder2);

            AutoPtr<ArrayOf<Float> > values3 = ArrayOf<Float>::Alloc(1);
            (*values3)[0] = 1.0f;
            AutoPtr<IPropertyValuesHolder> holder3;
            helper->OfFloat(String("scaleY"), values3, (IPropertyValuesHolder**)&holder3);

            AutoPtr<ArrayOf<IPropertyValuesHolder*> > array = ArrayOf<IPropertyValuesHolder*>::Alloc(3);
            array->Set(0, holder1);
            array->Set(1, holder2);
            array->Set(2, holder3);
            AutoPtr<IObjectAnimator> _bounceAnim = LauncherAnimUtils::OfPropertyValuesHolder(v, array);
            AutoPtr<IValueAnimator> bounceAnim = IValueAnimator::Probe(_bounceAnim);
            bounceAnim->SetDuration(IInstallShortcutReceiver::NEW_SHORTCUT_BOUNCE_DURATION);
            IAnimator::Probe(bounceAnim)->SetStartDelay(i * IInstallShortcutReceiver::NEW_SHORTCUT_STAGGER_DELAY);
            AutoPtr<IInterpolator> polator = new SmoothPagedView::OvershootInterpolator();
            IAnimator::Probe(bounceAnim)->SetInterpolator(ITimeInterpolator::Probe(polator));
            bounceAnims->Add(TO_IINTERFACE(bounceAnim));
        }
        anim->PlayTogether(bounceAnims);
        AutoPtr<IAnimatorListener> listener = new MyAnimatorListenerAdapter4(this);
Slogger::D("Launcher", "=======Launcher::RunNewAppsAnimation listener=%p",listener.Get());
        IAnimator::Probe(anim)->AddListener(listener);
        IAnimator::Probe(anim)->Start();
    }

    // Clean up
    mNewShortcutAnimatePage = -1;
    mNewShortcutAnimateViews->Clear();
    AutoPtr<IThread> t = new MyThread3(this, String("clearNewAppsThread"));
    t->Start();
}

ECode Launcher::BindSearchablesChanged()
{
    Boolean searchVisible = UpdateGlobalSearchIcon();
    Boolean voiceVisible = UpdateVoiceSearchIcon(searchVisible);
    if (mSearchDropTargetBar != NULL) {
        return mSearchDropTargetBar->OnSearchPackagesChanged(searchVisible, voiceVisible);
    }
    return NOERROR;
}

ECode Launcher::BindAllApplications(
    /* [in] */ IArrayList* apps)
{
    AutoPtr<IRunnable> setAllAppsRunnable = new MyRunnable15(this, apps);

    // Remove the progress bar entirely; we could also make it GONE
    // but better to remove it since we know it's not going to be used
    AutoPtr<IView> progressBar;
    IView::Probe(mAppsCustomizeTabHost)->FindViewById(
            Elastos::Droid::Launcher2::R::id::apps_customize_progress_bar,
            (IView**)&progressBar);
    if (progressBar != NULL) {
        AutoPtr<IViewParent> p;
        progressBar->GetParent((IViewParent**)&p);
        IViewGroup::Probe(p)->RemoveView(progressBar);

        // We just post the call to setApps so the user sees the progress bar
        // disappear-- otherwise, it just looks like the progress bar froze
        // which doesn't look great
        Boolean res;
        IView::Probe(mAppsCustomizeTabHost)->Post(setAllAppsRunnable, &res);
    }
    else {
        // If we did not initialize the spinner in onCreate, then we can directly set the
        // list of applications without waiting for any progress bars views to be hidden.
        setAllAppsRunnable->Run();
    }
    return NOERROR;
}

ECode Launcher::BindAppsAdded(
    /* [in] */ IArrayList* apps)
{
    AutoPtr<IRunnable> r = new MyRunnable16(this, apps);
    if (WaitUntilResume(r)) {
        return NOERROR;
    }

    if (mAppsCustomizeContent != NULL) {
        mAppsCustomizeContent->AddApps(apps);
    }
    return NOERROR;
}

ECode Launcher::BindAppsUpdated(
    /* [in] */ IArrayList* apps)
{
    AutoPtr<IRunnable> r = new MyRunnable17(this, apps);
    if (WaitUntilResume(r)) {
        return NOERROR;
    }

    if (mWorkspace != NULL) {
        mWorkspace->UpdateShortcuts(apps);
    }

    if (mAppsCustomizeContent != NULL) {
        mAppsCustomizeContent->UpdateApps(apps);
    }
    return NOERROR;
}

ECode Launcher::BindComponentsRemoved(
    /* [in] */ IArrayList* packageNames,
    /* [in] */ IArrayList* appInfos,
    /* [in] */ Boolean matchPackageNamesOnly,
    /* [in] */ IUserHandle* user)
{
    AutoPtr<IRunnable> r = new MyRunnable18(this, packageNames, appInfos,
            matchPackageNamesOnly, user);
    if (WaitUntilResume(r)) {
        return NOERROR;
    }

    if (matchPackageNamesOnly) {
        mWorkspace->RemoveItemsByPackageName(packageNames, user);
    }
    else {
        mWorkspace->RemoveItemsByApplicationInfo(appInfos, user);
    }

    if (mAppsCustomizeContent != NULL) {
        mAppsCustomizeContent->RemoveApps(appInfos);
    }

    // Notify the drag controller
    return mDragController->OnAppsRemoved(appInfos, this);
}

ECode Launcher::BindPackagesUpdated(
    /* [in] */ IArrayList* widgetsAndShortcuts)
{
    if (WaitUntilResume(mBindPackagesUpdatedRunnable, TRUE)) {
        mWidgetsAndShortcuts = widgetsAndShortcuts;
        return NOERROR;
    }

    if (mAppsCustomizeContent != NULL) {
        mAppsCustomizeContent->OnPackagesUpdated(widgetsAndShortcuts);
    }
    return NOERROR;
}

Int32 Launcher::MapConfigurationOriActivityInfoOri(
    /* [in] */ Int32 configOri)
{
    AutoPtr<IWindowManager> windowManager;
    GetWindowManager((IWindowManager**)&windowManager);
    AutoPtr<IDisplay> d;
    windowManager->GetDefaultDisplay((IDisplay**)&d);
    Int32 naturalOri = IConfiguration::ORIENTATION_LANDSCAPE;

    Int32 rotation;
    d->GetRotation(&rotation);
    switch (rotation) {
        case ISurface::ROTATION_0:
        case ISurface::ROTATION_180:
            // We are currently in the same basic orientation as the natural orientation
            naturalOri = configOri;
            break;
        case ISurface::ROTATION_90:
        case ISurface::ROTATION_270:
            // We are currently in the other basic orientation to the natural orientation
            naturalOri = (configOri == IConfiguration::ORIENTATION_LANDSCAPE) ?
                    IConfiguration::ORIENTATION_PORTRAIT : IConfiguration::ORIENTATION_LANDSCAPE;
            break;
    }

    AutoPtr<ArrayOf<Int32> > oriMap = ArrayOf<Int32>::Alloc(4);
    (*oriMap)[0] = IActivityInfo::SCREEN_ORIENTATION_PORTRAIT;
    (*oriMap)[1] = IActivityInfo::SCREEN_ORIENTATION_LANDSCAPE;
    (*oriMap)[2] = IActivityInfo::SCREEN_ORIENTATION_REVERSE_PORTRAIT;
    (*oriMap)[3] = IActivityInfo::SCREEN_ORIENTATION_REVERSE_LANDSCAPE;
    // Since the map starts at portrait, we need to offset if this device's natural orientation
    // is landscape.
    Int32 indexOffset = 0;
    if (naturalOri == IConfiguration::ORIENTATION_LANDSCAPE) {
        indexOffset = 1;
    }
    Int32 rotation2;
    d->GetRotation(&rotation2);
    return (*oriMap)[(rotation2 + indexOffset) % 4];
}

Int32 Launcher::IsRotationEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean enableRotation;
    if (sForceEnableRotation) {
        enableRotation = sForceEnableRotation;
        return enableRotation;
    }

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    resources->GetBoolean(
            Elastos::Droid::Launcher2::R::bool_::allow_rotation,
            &enableRotation);
    return enableRotation;
}

ECode Launcher::LockScreenOrientation()
{
    Boolean res;
    IsRotationEnabled(&res);
    if (res) {
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        AutoPtr<IConfiguration> config;
        resources->GetConfiguration((IConfiguration**)&config);
        Int32 orientation;
        config->GetOrientation(&orientation);
        return SetRequestedOrientation(MapConfigurationOriActivityInfoOri(orientation));
    }
    return NOERROR;
}

ECode Launcher::UnlockScreenOrientation(
    /* [in] */ Boolean immediate)
{
    Boolean res;
    IsRotationEnabled(&res);
    if (res) {
        if (immediate) {
            SetRequestedOrientation(IActivityInfo::SCREEN_ORIENTATION_UNSPECIFIED);
        }
        else {
            AutoPtr<IRunnable> r = new MyRunnable20(this);
            Boolean res;
            mHandler->PostDelayed(r, mRestoreScreenOrientationDelay, &res);
        }
    }
    return NOERROR;
}

Boolean Launcher::IsClingsEnabled()
{
    // disable clings when running in a test harness
    Boolean res;
    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    helper->IsRunningInTestHarness(&res);
    if(res) {
        return FALSE;
    }

    // Restricted secondary users (child mode) will potentially have very few apps
    // seeded when they start up for the first time. Clings won't work well with that
    Boolean supportsLimitedUsers =
            Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN_MR2;

    AutoPtr<IAccountManagerHelper> helper2;
    CAccountManagerHelper::AcquireSingleton((IAccountManagerHelper**)&helper2);
    AutoPtr<IAccountManager> accountManager;
    helper2->Get((IContext*)this, (IAccountManager**)&accountManager);
    AutoPtr<ArrayOf<IAccount*> > accounts;
    accountManager->GetAccounts((ArrayOf<IAccount*>**)&accounts);
    if (supportsLimitedUsers && accounts->GetLength() == 0) {
        AutoPtr<IInterface> obj;
        GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
        AutoPtr<IUserManager> um = IUserManager::Probe(obj);
        AutoPtr<IBundle> restrictions;
        um->GetUserRestrictions((IBundle**)&restrictions);
        Boolean res;
        restrictions->GetBoolean(IUserManager::DISALLOW_MODIFY_ACCOUNTS, FALSE, &res);
        if (res) {
           return FALSE;
        }
    }
    // Check if the system has requested skipping of first-use hints.
    AutoPtr<IContentResolver> cr;
    GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsSecure> ss;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&ss);
    Int32 value;
    ss->GetInt32(cr, ISettingsSecure::SKIP_FIRST_USE_HINTS, 0, &value);
    if (value == 1) {
        return FALSE;
    }
    return TRUE;
}

AutoPtr<ICling> Launcher::InitCling(
    /* [in] */ Int32 clingId,
    /* [in] */ ArrayOf<Int32>* positionData,
    /* [in] */ Boolean animate,
    /* [in] */ Int32 delay)
{
    AutoPtr<IView> view;
    FindViewById(clingId, (IView**)&view);
    AutoPtr<ICling> cling = ICling::Probe(view);
    if (cling != NULL) {
        cling->CCling_Init(ILauncher::Probe(this), positionData);
        IView::Probe(cling)->SetVisibility(IView::VISIBLE);
        IView::Probe(cling)->SetLayerType(IView::LAYER_TYPE_HARDWARE, NULL);
        if (animate) {
            IView::Probe(cling)->BuildLayer();
            IView::Probe(cling)->SetAlpha(0.0f);
            AutoPtr<IViewPropertyAnimator> res;
            IView::Probe(cling)->Animate((IViewPropertyAnimator**)&res);
            res->Alpha(1.0f);
            AutoPtr<ITimeInterpolator> interpolator;
            CAccelerateInterpolator::New((ITimeInterpolator**)&interpolator);
            res->SetInterpolator(interpolator);
            res->SetDuration(SHOW_CLING_DURATION);
            res->SetStartDelay(delay);
            res->Start();
        }
        else {
            IView::Probe(cling)->SetAlpha(1.0f);
        }
        IView::Probe(cling)->SetFocusableInTouchMode(TRUE);
        AutoPtr<IRunnable> r = new MyRunnable21(this, cling);
        Boolean res;
        IView::Probe(cling)->Post(r, &res);
        mHideFromAccessibilityHelper->SetImportantForAccessibilityToNo(
                IView::Probe(mDragLayer), clingId == Elastos::Droid::Launcher2::R::id::all_apps_cling);
    }
    return cling;
}

void Launcher::DismissCling(
    /* [in] */ ICling* cling,
    /* [in] */ const String& flag,
    /* [in] */ Int32 duration)
{
    // To catch cases where siblings of top-level views are made invisible, just check whether
    // the cling is directly set to GONE before dismissing it.
    Int32 visibility;
    IView::Probe(cling)->GetVisibility(&visibility);
    if (cling != NULL && visibility != IView::GONE) {
        AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
        (*array)[0] = 0.0f;
        AutoPtr<IObjectAnimator> anim = LauncherAnimUtils::OfFloat(IView::Probe(cling), String("alpha"), array);
        IAnimator::Probe(anim)->SetDuration(duration);
        AutoPtr<IAnimatorListener> listener = new MyAnimatorListenerAdapter5(this, cling, flag);
Slogger::D("Launcher", "=======Launcher::DismissCling listener=%p",listener.Get());
        IAnimator::Probe(anim)->AddListener(listener);
        IAnimator::Probe(anim)->Start();
        mHideFromAccessibilityHelper->RestoreImportantForAccessibility(IView::Probe(mDragLayer));
    }
}

void Launcher::RemoveCling(
    /* [in] */ Int32 id)
{
    AutoPtr<IView> cling;
    FindViewById(id, (IView**)&cling);
    if (cling != NULL) {
        AutoPtr<IViewParent> p;
        cling->GetParent((IViewParent**)&p);
        AutoPtr<IViewGroup> parent = IViewGroup::Probe(p);
        AutoPtr<IRunnable> r = new MyRunnable22(parent, cling);
        Boolean res;
        IView::Probe(parent)->Post(r, &res);
        mHideFromAccessibilityHelper->RestoreImportantForAccessibility(IView::Probe(mDragLayer));
    }
}

Boolean Launcher::SkipCustomClingIfNoAccounts()
{
    AutoPtr<IView> view;
    FindViewById(
            Elastos::Droid::Launcher2::R::id::workspace_cling, (IView**)&view);
    AutoPtr<ICling> cling = ICling::Probe(view);
    String identifier;
    cling->GetDrawIdentifier(&identifier);
    Boolean customCling = identifier.Equals(String("workspace_custom"));
    if (customCling) {

        AutoPtr<IAccountManagerHelper> helper;
        CAccountManagerHelper::AcquireSingleton((IAccountManagerHelper**)&helper);
        AutoPtr<IAccountManager> am;
        helper->Get((IContext*)this, (IAccountManager**)&am);
        AutoPtr<ArrayOf<IAccount*> > accounts;
        am->GetAccountsByType(String("com.google"), (ArrayOf<IAccount*>**)&accounts);
        return accounts->GetLength() == 0;
    }
    return FALSE;
}

ECode Launcher::ShowFirstRunWorkspaceCling()
{
Slogger::E("Launcher", "============================Launcher::ShowFirstRunWorkspaceCling 1");
    // Enable the clings only if they have not been dismissed before
    Boolean res;
    mSharedPrefs->GetBoolean(ICling::WORKSPACE_CLING_DISMISSED_KEY, FALSE, &res);
    if (IsClingsEnabled() && !res && !SkipCustomClingIfNoAccounts() ) {
        // If we're not using the default workspace layout, replace workspace cling
        // with a custom workspace cling (usually specified in an overlay)
        // For now, only do this on tablets
        Int32 id;
        mSharedPrefs->GetInt32(ILauncherProvider::DEFAULT_WORKSPACE_RESOURCE_ID, 0, &id);
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        Boolean res3;
        resources->GetBoolean(
                Elastos::Droid::Launcher2::R::bool_::config_useCustomClings, &res3);
        if (id != 0 && res3) {
            // Use a custom cling
            AutoPtr<IView> cling;
            FindViewById(
                    Elastos::Droid::Launcher2::R::id::workspace_cling, (IView**)&cling);
            AutoPtr<IViewParent> p;
            cling->GetParent((IViewParent**)&p);
            AutoPtr<IViewGroup> clingParent = IViewGroup::Probe(p);
            Int32 clingIndex;
            clingParent->IndexOfChild(cling, &clingIndex);
            clingParent->RemoveViewAt(clingIndex);
            AutoPtr<IView> customCling;
            mInflater->Inflate(
                    Elastos::Droid::Launcher2::R::layout::custom_workspace_cling,
                    clingParent, FALSE, (IView**)&customCling);
            clingParent->AddView(customCling, clingIndex);
            customCling->SetId(
                    Elastos::Droid::Launcher2::R::id::workspace_cling);
        }
        InitCling(
                Elastos::Droid::Launcher2::R::id::workspace_cling, NULL, FALSE, 0);
    }
    else {
        RemoveCling(Elastos::Droid::Launcher2::R::id::workspace_cling);
    }
Slogger::E("Launcher", "============================Launcher::ShowFirstRunWorkspaceCling return");
    return NOERROR;
}

ECode Launcher::ShowFirstRunAllAppsCling(
    /* [in] */ ArrayOf<Int32>* position)
{
    // Enable the clings only if they have not been dismissed before
    Boolean res;
    mSharedPrefs->GetBoolean(ICling::ALLAPPS_CLING_DISMISSED_KEY, FALSE, &res);
    if (IsClingsEnabled() && !res) {
        InitCling(
                Elastos::Droid::Launcher2::R::id::all_apps_cling, position, TRUE, 0);
    }
    else {
        RemoveCling(Elastos::Droid::Launcher2::R::id::all_apps_cling);
    }
    return NOERROR;
}

ECode Launcher::ShowFirstRunFoldersCling(
    /* [out] */ ICling** cling)
{
    VALIDATE_NOT_NULL(cling);

    // Enable the clings only if they have not been dismissed before
    Boolean res;
    mSharedPrefs->GetBoolean(ICling::FOLDER_CLING_DISMISSED_KEY, FALSE, &res);
    if (IsClingsEnabled() && !res) {
        AutoPtr<ICling> c = InitCling(
                Elastos::Droid::Launcher2::R::id::folder_cling, NULL, TRUE, 0);
        *cling = c;
        REFCOUNT_ADD(*cling);
        return NOERROR;
    }
    else {
        RemoveCling(Elastos::Droid::Launcher2::R::id::folder_cling);
        *cling = NULL;
        return NOERROR;
    }
    return NOERROR;
}

ECode Launcher::IsFolderClingVisible(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IView> view;
    FindViewById(
            Elastos::Droid::Launcher2::R::id::folder_cling, (IView**)&view);
    AutoPtr<ICling> cling = ICling::Probe(view);
    if (cling != NULL) {
        Int32 visibility;
        IView::Probe(cling)->GetVisibility(&visibility);
        *result = visibility == IView::VISIBLE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode Launcher::DismissWorkspaceCling(
    /* [in] */ IView* v)
{
    AutoPtr<IView> view;
    FindViewById(
            Elastos::Droid::Launcher2::R::id::workspace_cling, (IView**)&view);
    AutoPtr<ICling> cling = ICling::Probe(view);
    DismissCling(cling, ICling::WORKSPACE_CLING_DISMISSED_KEY, DISMISS_CLING_DURATION);
    return NOERROR;
}

ECode Launcher::DismissAllAppsCling(
    /* [in] */ IView* v)
{
Slogger::E("Launcher", "===============Launcher::DismissAllAppsCling");
    AutoPtr<IView> view;
    FindViewById(
            Elastos::Droid::Launcher2::R::id::all_apps_cling, (IView**)&view);
    AutoPtr<ICling> cling = ICling::Probe(view);
    DismissCling(cling, ICling::ALLAPPS_CLING_DISMISSED_KEY, DISMISS_CLING_DURATION);
    return NOERROR;
}

ECode Launcher::DismissFolderCling(
    /* [in] */ IView* v)
{
    AutoPtr<IView> view;
    FindViewById(
            Elastos::Droid::Launcher2::R::id::folder_cling, (IView**)&view);
    AutoPtr<ICling> cling = ICling::Probe(view);
    DismissCling(cling, ICling::FOLDER_CLING_DISMISSED_KEY, DISMISS_CLING_DURATION);
    return NOERROR;
}

ECode Launcher::DumpState()
{
    Slogger::D(TAG, String("BEGIN launcher2 dump state for launcher ") + TO_STR(this));
    Slogger::D(TAG, String("mSavedState=") + TO_STR(mSavedState));
    StringBuilder sb, sb1, sb2, sb3;
    sb += "mWorkspaceLoading=";
    sb += mWorkspaceLoading;
    Slogger::D(TAG, sb.ToString());
    sb1 += "mRestoring=";
    sb1 += mRestoring;
    Slogger::D(TAG, sb1.ToString());
    sb2 += "mWaitingForResult=";
    sb2 += mWaitingForResult;
    Slogger::D(TAG, sb2.ToString());
    Slogger::D(TAG, String("mSavedInstanceState=") + TO_STR(mSavedInstanceState));
    Int32 size;
    sFolders->GetSize(&size);
    sb3 += "sFolders.size=";
    sb3 += size;
    Slogger::D(TAG, sb3.ToString());
    mModel->DumpState();

    if (mAppsCustomizeContent != NULL) {
        mAppsCustomizeContent->DumpState();
    }
    Slogger::D(TAG, "END launcher2 dump state");
    return NOERROR;
}

ECode Launcher::Dump(
    /* [in] */ const String& prefix,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* writer,
    /* [in] */ ArrayOf<String>* args)
{
    Activity::Dump(prefix, fd, writer, args);
    writer->Println(String(" "));
    writer->Println(String("Debug logs: "));
    Int32 size;
    sDumpLogs->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        sDumpLogs->Get(i, (IInterface**)&obj);
        AutoPtr<ICharSequence> cchar = ICharSequence::Probe(obj);
        String str;
        cchar->ToString(&str);
        StringBuilder sb;
        sb += "  ";
        sb += str;
        writer->Println(sb.ToString());
    }
    return NOERROR;
}

ECode Launcher::DumpDebugLogsToConsole()
{
    Slogger::D(TAG, String(""));
    Slogger::D(TAG, String("*********************"));
    Slogger::D(TAG, String("Launcher debug logs: "));
    Int32 size;
    sDumpLogs->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        sDumpLogs->Get(i, (IInterface**)&obj);
        AutoPtr<ICharSequence> cchar = ICharSequence::Probe(obj);
        String str;
        cchar->ToString(&str);
        StringBuilder sb;
        sb += "  ";
        sb += str;
        Slogger::D(TAG, sb.ToString());
    }
    Slogger::D(TAG, String("*********************"));
    Slogger::D(TAG, String(""));
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos