#include "elastos/droid/app/Activity.h"
#include "elastos/droid/app/Fragment.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/app/CSearchManager.h"
#include "elastos/droid/app/CInstrumentationHelper.h"
#include "elastos/droid/app/SharedElementCallback.h"
#include "elastos/droid/app/CVoiceInteractor.h"
#include "elastos/droid/app/FragmentManagerImpl.h"
#include "elastos/droid/app/CApplication.h"
#include "elastos/droid/app/CPendingIntent.h"
#include "elastos/droid/app/CTaskStackBuilderHelper.h"
#include "elastos/droid/app/CActivityNonConfigurationInstances.h"
#include "elastos/droid/app/CActivityManagerTaskDescription.h"
#include "elastos/droid/app/CActivityManager.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/CUserHandle.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/view/CWindowManagerGlobal.h"
#include "elastos/droid/view/CMenuInflater.h"
#include "elastos/droid/text/Selection.h"
#include "elastos/droid/text/CSpannableStringBuilder.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/text/method/CTextKeyListenerHelper.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CIntentHelper.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/content/res/CResourcesHelper.h"
#include "elastos/droid/content/res/CConfiguration.h"
#include "elastos/droid/net/CUriHelper.h"
#include "elastos/droid/utility/CArrayMap.h"
#include "elastos/droid/internal/app/CWindowDecorActionBar.h"
#include "elastos/droid/internal/app/CToolbarActionBar.h"
#include "elastos/droid/internal/policy/CPolicyManager.h"
#include "elastos/droid/R.h"

#include <elastos/core/StringBuffer.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Thread.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/logging/Logger.h>

#include "Elastos.Droid.Internal.h"
#include <Elastos.CoreLibrary.IO.h>

using Elastos::Droid::R;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentHelper;
using Elastos::Droid::Content::CIntentHelper;
using Elastos::Droid::Content::IIIntentSender;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::EIID_IContextWrapper;
using Elastos::Droid::Content::EIID_IComponentCallbacks;
using Elastos::Droid::Content::EIID_IComponentCallbacks2;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::CConfiguration;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::CSpannableStringBuilder;
using Elastos::Droid::Text::Selection;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::Method::ITextKeyListenerHelper;
using Elastos::Droid::Text::Method::CTextKeyListenerHelper;
using Elastos::Droid::Text::Method::IKeyListener;
using Elastos::Droid::Text::Method::EIID_IKeyListener;
using Elastos::Droid::Text::Method::ITextKeyListener;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Droid::View::CWindowManagerGlobal;
using Elastos::Droid::View::CMenuInflater;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IDispatcherState;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::IWindowManagerGlobal;
using Elastos::Droid::View::EIID_IOnWindowDismissedCallback;
using Elastos::Droid::View::EIID_IWindowCallback;
using Elastos::Droid::View::EIID_IKeyEventCallback;
using Elastos::Droid::View::EIID_IContextThemeWrapper;
using Elastos::Droid::View::EIID_IViewOnCreateContextMenuListener;
using Elastos::Droid::View::EIID_ILayoutInflaterFactory;
using Elastos::Droid::View::EIID_ILayoutInflaterFactory2;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Internal::Policy::IPolicyManager;
using Elastos::Droid::Internal::Policy::CPolicyManager;
using Elastos::Droid::Internal::App::IWindowDecorActionBar;
using Elastos::Droid::Internal::App::CWindowDecorActionBar;
using Elastos::Droid::Internal::App::IToolbarActionBar;
using Elastos::Droid::Internal::App::CToolbarActionBar;

using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::IRunnable;
using Elastos::Core::StringBuffer;
using Elastos::Core::StringBuilder;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::Thread;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace App {

static const String TAG("Activity");

//=============================================================================
// Activity::FragmentContainerLocal
//=============================================================================

CAR_INTERFACE_IMPL(Activity::FragmentContainerLocal, Object, IFragmentContainer)

Activity::FragmentContainerLocal::FragmentContainerLocal(
    /* [in] */ Activity* host)
    : mHost(host)
{}

ECode Activity::FragmentContainerLocal::FindViewById(
    /* [in] */ Int32 id,
    /* [out] */ IView** view)
{
    return mHost->FindViewById(id, view);
}

ECode Activity::FragmentContainerLocal::HasView(
    /* [out] */ Boolean* hasView)
{
    VALIDATE_NOT_NULL(hasView)
    *hasView = FALSE;
    AutoPtr<IWindow> window;
    mHost->GetWindow((IWindow**)&window);

    if (window) {
        AutoPtr<IView> decorView;
        window->PeekDecorView((IView**)&decorView);
        if (decorView != NULL) {
            *hasView = TRUE;
        }
    }

    return NOERROR;
}

//=============================================================================
// Activity
//=============================================================================
const Boolean Activity::DEBUG_LIFECYCLE = FALSE;
const String Activity::FRAGMENTS_TAG("android:fragments");

const String Activity::WINDOW_HIERARCHY_TAG("android:viewHierarchyState");
const String Activity::SAVED_DIALOG_IDS_KEY("android:savedDialogIds");
const String Activity::SAVED_DIALOGS_TAG("android:savedDialogs");
const String Activity::SAVED_DIALOG_KEY_PREFIX("android:dialog_");
const String Activity::SAVED_DIALOG_ARGS_KEY_PREFIX("android:dialog_args_");

CAR_INTERFACE_IMPL_9(Activity, ContextThemeWrapper, IActivity, ILayoutInflaterFactory, ILayoutInflaterFactory2, IWindowCallback, IKeyEventCallback, IViewOnCreateContextMenuListener, IComponentCallbacks, IComponentCallbacks2, IOnWindowDismissedCallback)

Activity::Activity()
    : mCalled(FALSE)
    , mCheckedForLoaderManager(FALSE)
    , mLoadersStarted(FALSE)
    , mResumed(FALSE)
    , mStopped(FALSE)
    , mFinished(FALSE)
    , mStartedActivity(FALSE)
    , mTemporaryPause(FALSE)
    , mChangingConfigurations(FALSE)
    , mConfigChangeFlags(0)
    , mWindowAdded(FALSE)
    , mVisibleFromServer(FALSE)
    , mVisibleFromClient(FALSE)
    , mResultCode(IActivity::RESULT_CANCELED)
    , mChangeCanvasToTranslucent(FALSE)
    , mVisibleBehind(FALSE)
    , mIdent(0)
    , mDestroyed(FALSE)
    , mDoReportFullyDrawn(TRUE)
    , mEnableDefaultActionBarUp(FALSE)
    , mTitleColor(0)
    , mTitleReady(FALSE)
    , mDefaultKeyMode(IActivity::DEFAULT_KEYS_DISABLE)
    , mThemeResource(0)
{
    mFragments = new FragmentManagerImpl();
    mContainer = new FragmentContainerLocal(this);
    CHandler::New((IHandler**)&mHandler);
}

Activity::~Activity()
{
    mInflater = NULL;
    mManagedDialogs.Clear();
    mManagedCursors.Clear();
}

ECode Activity::constructor()
{
    return NOERROR;
}

ECode Activity::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = ToString();
    return NOERROR;
}

String Activity::ToString()
{
    StringBuilder sb("Activity:(");
    sb += mEmbeddedID;
    sb += ")";
    return sb.ToString();
}

ECode Activity::GetIntent(
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent);

    *intent = mIntent;
    REFCOUNT_ADD(*intent)
    return NOERROR;
}

ECode Activity::SetIntent(
    /* [in] */ IIntent* newIntent)
{
    mIntent = newIntent;
    return NOERROR;
}

ECode Activity::GetApplication(
    /* [out] */ IApplication** application)
{
    VALIDATE_NOT_NULL(application);

    *application = mApplication;
    REFCOUNT_ADD(*application)

    return NOERROR;
}

AutoPtr<IApplication> Activity::GetApplication()
{
    return mApplication;
}

ECode Activity::IsChild(
    /* [out] */ Boolean* isChild)
{
    VALIDATE_NOT_NULL(isChild);
    *isChild = mParent != NULL;
    return NOERROR;
}

ECode Activity::GetParent(
    /* [out] */ IActivity** parent)
{
    VALIDATE_NOT_NULL(parent);
    *parent = mParent.Get();
    REFCOUNT_ADD(*parent);
    return NOERROR;
}

ECode Activity::GetWindowManager(
    /* [out] */ IWindowManager** mgr)
{
    VALIDATE_NOT_NULL(mgr);
    *mgr = mWindowManager.Get();
    REFCOUNT_ADD(*mgr);
    return NOERROR;
}

AutoPtr<IWindowManager> Activity::GetWindowManager()
{
    return mWindowManager;
}

ECode Activity::GetWindow(
    /* [out] */ IWindow** window)
{
    VALIDATE_NOT_NULL(window);
    *window = mWindow.Get();
    REFCOUNT_ADD(*window);
    return NOERROR;
}

AutoPtr<IWindow> Activity::GetWindow()
{
    return mWindow;
}

ECode Activity::GetLoaderManager(
    /* [out] */ ILoaderManager** manager)
{
    VALIDATE_NOT_NULL(manager);

    if (mLoaderManager == NULL) {
        mCheckedForLoaderManager = TRUE;
        String str("(root)");
        mLoaderManager = GetLoaderManager(str, mLoadersStarted, TRUE);
    }

    *manager = ILoaderManager::Probe(mLoaderManager);
    REFCOUNT_ADD(*manager);
    return NOERROR;
}

AutoPtr<ILoaderManagerImpl> Activity::GetLoaderManager(
    /* [in] */ const String& who,
    /* [in] */ Boolean started,
    /* [in] */ Boolean create)
{
    if (mAllLoaderManagers == NULL) {
        CArrayMap::New((IArrayMap**)&mAllLoaderManagers);
    }

    AutoPtr<ICharSequence> key = CoreUtils::Convert(who);
    AutoPtr<ILoaderManagerImpl> lm;
    mAllLoaderManagers->Get(IInterface::Probe(key), (IInterface**)&lm);
    if (lm == NULL) {
        if (create) {
            // TODO
//            CLoaderManagerImpl::New(who,
//                    this, started, (ILoaderManagerImpl**)&lm);
//            mAllLoaderManagers->Put(who, (IInterface*)lm.Get());
        }
    }
    else {
        // LoaderManagerImpl* lmi = (LoaderManagerImpl*)lm.Get();
        // lmi->UpdateActivity(this);
    }

    return lm;
}

ECode Activity::GetCurrentFocus(
    /* [out] */ IView** focus)
{
    VALIDATE_NOT_NULL(focus);
    *focus = NULL;
    if (mWindow != NULL) {
        return mWindow->GetCurrentFocus(focus);
    }
    return NOERROR;
}

ECode Activity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    if (DEBUG_LIFECYCLE) {
        StringBuilder sb("onCreate ");
        sb.Append(ToString());
        String temp = Object::ToString(savedInstanceState);
        sb.Append(temp);
        Slogger::V(TAG, sb.ToString());
    }

    if (mLastNonConfigurationInstances != NULL) {
        mLastNonConfigurationInstances->GetLoaders((IArrayMap**)&mAllLoaderManagers);
    }

    String parentActivityName;
    FAIL_RETURN(mActivityInfo->GetParentActivityName(&parentActivityName));
    if (!parentActivityName.IsNull()) {
        if (mActionBar == NULL) {
            mEnableDefaultActionBarUp = TRUE;
        }
        else {
            FAIL_RETURN(mActionBar->SetDefaultDisplayHomeAsUpEnabled(TRUE));
        }
    }
    if (savedInstanceState != NULL) {
        AutoPtr<IParcelable> p;
        savedInstanceState->GetParcelable(FRAGMENTS_TAG, (IParcelable**)&p);

        if (mLastNonConfigurationInstances == NULL) {
            FAIL_RETURN(mFragments->RestoreAllState(p, NULL));
        }
        else {
            AutoPtr<IArrayList> fragments;
            mLastNonConfigurationInstances->GetFragments((IArrayList**)&fragments);
            FAIL_RETURN(mFragments->RestoreAllState(p, fragments));
        }
    }
    mFragments->DispatchCreate();

    AutoPtr<IApplication> app = GetApplication();
    assert(app != NULL);
    FAIL_RETURN(app->DispatchActivityCreated(this, savedInstanceState));

    if (mVoiceInteractor != NULL) {
        VoiceInteractor* vi = (VoiceInteractor*)mVoiceInteractor.Get();
        vi->AttachActivity(this);
    }

    mCalled = TRUE;
    return NOERROR;
}

ECode Activity::OnCreate(
    /* [in] */ IBundle* savedInstanceState,
    /* [in] */ IPersistableBundle* persistentState)
{
    return OnCreate(savedInstanceState);;
}

ECode Activity::PerformRestoreInstanceState(
    /* [in] */ IBundle* savedInstanceState)
{
    FAIL_RETURN(OnRestoreInstanceState(savedInstanceState));
    return RestoreManagedDialogs(savedInstanceState);
}

ECode Activity::PerformRestoreInstanceState(
    /* [in] */ IBundle* savedInstanceState,
    /* [in] */ IPersistableBundle* persistentState)
{
    FAIL_RETURN(OnRestoreInstanceState(savedInstanceState, persistentState))
    if (savedInstanceState != NULL) {
        RestoreManagedDialogs(savedInstanceState);
    }
    return NOERROR;
}

ECode Activity::OnRestoreInstanceState(
    /* [in] */ IBundle* savedInstanceState)
{
    VALIDATE_NOT_NULL(savedInstanceState);
    if (mWindow != NULL) {
        AutoPtr<IBundle> windowState;
        savedInstanceState->GetBundle(WINDOW_HIERARCHY_TAG, (IBundle**)&windowState);
        if (windowState != NULL) {
            mWindow->RestoreHierarchyState(windowState);
        }
    }
    return NOERROR;
}

ECode Activity::OnRestoreInstanceState(
    /* [in] */ IBundle* savedInstanceState,
    /* [in] */ IPersistableBundle* persistentState)
{
    if (savedInstanceState != NULL) {
        return OnRestoreInstanceState(savedInstanceState);
    }

    return NOERROR;
}

ECode Activity::RestoreManagedDialogs(
    /* [in] */ IBundle* savedInstanceState)
{
    VALIDATE_NOT_NULL(savedInstanceState);
    AutoPtr<IBundle> b;
    savedInstanceState->GetBundle(SAVED_DIALOGS_TAG, (IBundle**)&b);
    if (b == NULL) {
        return NOERROR;
    }

    AutoPtr< ArrayOf<Int32> > ids;
    b->GetInt32Array(SAVED_DIALOG_IDS_KEY, (ArrayOf<Int32>**)&ids);
    Int32 numDialogs = ids->GetLength();

    mManagedDialogs.Clear();

    for (Int32 i = 0; i < numDialogs; ++i) {
        Int32 dialogId = (*ids)[i];

        AutoPtr<IBundle> dialogState;
        b->GetBundle(SavedDialogKeyFor(dialogId), (IBundle**)&dialogState);
        if (dialogState != NULL) {
            // Calling onRestoreInstanceState() below will invoke dispatchOnCreate
            // so tell createDialog() not to do it, otherwise we get an exception
            AutoPtr<ManagedDialog> md = new ManagedDialog();
            b->GetBundle(SavedDialogArgsKeyFor(dialogId), (IBundle**)&md->mArgs);
            md->mDialog = CreateDialog(dialogId, dialogState, md->mArgs);
            if (md->mDialog != NULL) {
                mManagedDialogs.Insert(HashMap<Int32, AutoPtr<ManagedDialog> >::ValueType(dialogId, md));
                OnPrepareDialog(dialogId, md->mDialog, md->mArgs);
                md->mDialog->OnRestoreInstanceState(dialogState);
            }
        }
    }
    return NOERROR;
}

AutoPtr<IDialog> Activity::CreateDialog(
    /* [in] */ Int32 dialogId,
    /* [in] */ IBundle* state,
    /* [in] */ IBundle* args)
{
    AutoPtr<IDialog> dialog = OnCreateDialog(dialogId, args);
    if (dialog != NULL) {
        dialog->DispatchOnCreate(state);
    }
    return dialog;
}

String Activity::SavedDialogKeyFor(
    /* [in] */ Int32 key)
{
    StringBuilder sb(SAVED_DIALOG_KEY_PREFIX);
    sb.Append(key);
    return sb.ToString();
}

String Activity::SavedDialogArgsKeyFor(
        /* [in] */ Int32 key)
{
    StringBuilder sb(SAVED_DIALOG_ARGS_KEY_PREFIX);
    sb.Append(key);
    return sb.ToString();
}

ECode Activity::OnPostCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Boolean isChild;
    IsChild(&isChild);
    if (!isChild) {
        mTitleReady = TRUE;
        AutoPtr<ICharSequence> title;
        Int32 titleColor;
        GetTitle((ICharSequence**)&title);
        GetTitleColor(&titleColor);
        OnTitleChanged(title, titleColor);
    }
    mCalled = TRUE;
    return NOERROR;
}

ECode Activity::OnPostCreate(
    /* [in] */ IBundle* savedInstanceState,
    /* [in] */ IPersistableBundle* persistentState)
{
    return OnPostCreate(savedInstanceState);
}

ECode Activity::OnStart()
{
    if (DEBUG_LIFECYCLE) {
        StringBuilder sb("onStart ");
        sb.Append(ToString());
        Slogger::V(TAG, sb.ToString());
    }
    mCalled = TRUE;

    if (!mLoadersStarted) {
        mLoadersStarted = TRUE;
        if (mLoaderManager != NULL) {
            assert(0 && "TODO");
            // LoaderManagerImpl* lmi = (LoaderManagerImpl*)mLoaderManager.Get();
            // lmi->DoStart();
        }
        else if (!mCheckedForLoaderManager) {
            String str("(root)");
            mLoaderManager = GetLoaderManager(str, mLoadersStarted, FALSE);
        }
        mCheckedForLoaderManager = TRUE;
    }

    AutoPtr<IApplication> app = GetApplication();
    return app->DispatchActivityStarted(this);
}

ECode Activity::OnRestart()
{
    mCalled = TRUE;
    return NOERROR;
}

ECode Activity::OnResume()
{
    if (DEBUG_LIFECYCLE) {
        StringBuilder sb("onResume ");
        sb.Append(ToString());
        Slogger::V(TAG, sb.ToString());
    }
    AutoPtr<IApplication> app = GetApplication();
    FAIL_RETURN(app->DispatchActivityResumed(this));
    mActivityTransitionState->OnResume();
    mCalled = TRUE;
    return NOERROR;
}

ECode Activity::OnPostResume()
{
    AutoPtr<IWindow> win = GetWindow();
    if (win != NULL) {
        FAIL_RETURN(win->MakeActive());
    }
    if (mActionBar != NULL) {
        FAIL_RETURN(mActionBar->SetShowHideAnimationEnabled(TRUE));
    }
    mCalled = TRUE;
    return NOERROR;
}

ECode Activity::IsVoiceInteraction(
    /* [out] */ Boolean* bval)
{
    VALIDATE_NOT_NULL(bval)
    *bval = mVoiceInteractor != NULL;
    return NOERROR;
}

ECode Activity::GetVoiceInteractor(
    /* [out] */ IVoiceInteractor** vi)
{
    VALIDATE_NOT_NULL(vi)
    *vi = mVoiceInteractor;
    REFCOUNT_ADD(*vi)
    return NOERROR;
}

ECode Activity::OnNewIntent(
    /* [in] */ IIntent *intent)
{
    return NOERROR;
}

ECode Activity::PerformSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    FAIL_RETURN(OnSaveInstanceState(outState));
    FAIL_RETURN(SaveManagedDialogs(outState));
    FAIL_RETURN(mActivityTransitionState->SaveState(outState))

    if (DEBUG_LIFECYCLE) {
        StringBuilder sb("onSaveInstanceState ");
        sb.Append(ToString());
        sb.Append(": ");
        sb.Append(Object::ToString(outState));
        Slogger::V(TAG, sb.ToString());
    }
    return NOERROR;
}

ECode Activity::PerformSaveInstanceState(
    /* [in] */ IBundle* outState,
    /* [in] */ IPersistableBundle* outPersistentState)
{
    FAIL_RETURN(OnSaveInstanceState(outState, outPersistentState))
    FAIL_RETURN(SaveManagedDialogs(outState));

    if (DEBUG_LIFECYCLE) {
        StringBuilder sb("onSaveInstanceState ");
        sb.Append(ToString());
        sb.Append(": ");
        sb.Append(Object::ToString(outState));
        sb.Append(", ");
        sb.Append(Object::ToString(outPersistentState));
        Slogger::V(TAG, sb.ToString());
    }

    return NOERROR;
}

ECode Activity::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    VALIDATE_NOT_NULL(outState);
    AutoPtr<IBundle> bundle;
    FAIL_RETURN(mWindow->SaveHierarchyState((IBundle**)&bundle));
    FAIL_RETURN(outState->PutBundle(WINDOW_HIERARCHY_TAG, bundle));
    AutoPtr<IParcelable> p;
    FAIL_RETURN(mFragments->SaveAllState((IParcelable**)&p));
    if (p != NULL) {
        outState->PutParcelable(FRAGMENTS_TAG, p);
    }
    AutoPtr<IApplication> app = GetApplication();
    return app->DispatchActivitySaveInstanceState(this, outState);
}

ECode Activity::OnSaveInstanceState(
    /* [in] */ IBundle* outState,
    /* [in] */ IPersistableBundle* outPersistentState)
{
    return OnSaveInstanceState(outState);
}

ECode Activity::SaveManagedDialogs(
    /* [in] */ IBundle* outState)
{
    Int32 numDialogs = mManagedDialogs.GetSize();
    if (numDialogs == 0) {
        return NOERROR;
    }

    AutoPtr<IBundle> dialogState;
    FAIL_RETURN(CBundle::New((IBundle**)&dialogState));
    AutoPtr<ArrayOf<Int32> > ids = ArrayOf<Int32>::Alloc(numDialogs);

    // save each dialog's bundle, gather the ids
    ManagedDialogHashMapIter it = mManagedDialogs.Begin();
    for (Int32 i = 0; it != mManagedDialogs.End(); ++i, ++it) {
        Int32 key = it->mFirst;
        (*ids)[i] = key;
        ManagedDialog* md = it->mSecond;
        AutoPtr<IBundle> di;
        md->mDialog->OnSaveInstanceState((IBundle**)&di);
        String keyStr = SavedDialogKeyFor(key);
        dialogState->PutBundle(keyStr, di);
        if (md->mArgs != NULL) {
            dialogState->PutBundle(keyStr, md->mArgs);
        }
    }

    dialogState->PutInt32Array(SAVED_DIALOG_IDS_KEY, ids);
    return outState->PutBundle(SAVED_DIALOGS_TAG, dialogState);
}

ECode Activity::OnPause()
{
    if (DEBUG_LIFECYCLE) {
        StringBuilder sb("onPause ");
        sb.Append(ToString());
        Slogger::V(TAG, sb.ToString());
    }
    AutoPtr<IApplication> app = GetApplication();
    FAIL_RETURN(app->DispatchActivityPaused(this));
    mCalled = TRUE;
    return NOERROR;
}

ECode Activity::OnUserLeaveHint()
{
    return NOERROR;
}

ECode Activity::OnCreateThumbnail(
    /* [in] */ IBitmap* outBitmap,
    /* [in] */ ICanvas* canvas,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;
    return NOERROR;
}

ECode Activity::OnCreateDescription(
    /* [out] */ ICharSequence** description)
{
    VALIDATE_NOT_NULL(description);
    *description = NULL;
    return NOERROR;
}

ECode Activity::OnProvideAssistData(
    /* [out] */ IBundle* data)
{
    return NOERROR;
}

ECode Activity::OnStop()
{
    if (DEBUG_LIFECYCLE) {
        StringBuilder sb("onStop ");
        sb.Append(ToString());
        Slogger::V(TAG, sb.ToString());
    }
    if (mActionBar != NULL) {
        FAIL_RETURN(mActionBar->SetShowHideAnimationEnabled(FALSE));
    }
    FAIL_RETURN(mActivityTransitionState->OnStop())
    AutoPtr<IApplication> app = GetApplication();
    FAIL_RETURN(app->DispatchActivityDestroyed(this));
    mTranslucentCallback = NULL;
    mCalled = TRUE;
    return NOERROR;
}

ECode Activity::OnDestroy()
{
    if (DEBUG_LIFECYCLE) {
        StringBuilder sb("onDestroy ");
        sb.Append(ToString());
        Slogger::V(TAG, sb.ToString());
    }
    mCalled = TRUE;

    // dismiss any dialogs we are managing.
    if (!mManagedDialogs.IsEmpty()) {
        ManagedDialogHashMapIter iter = mManagedDialogs.Begin();
        for (; iter != mManagedDialogs.End(); ++iter) {
            Boolean isShowing;
            iter->mSecond->mDialog->IsShowing(&isShowing);
            if (isShowing) {
                IDialogInterface::Probe(iter->mSecond->mDialog)->Dismiss();
            }
        }
        mManagedDialogs.Clear();
    }

    // close any cursors we are managing.
    {
        AutoLock lock(mManagedCursorsLock);
        AutoPtr<ManagedCursor> cursor;
        List<AutoPtr<ManagedCursor> >::Iterator it = mManagedCursors.Begin();
        for (; it != mManagedCursors.End(); ++it) {
            cursor = *it;
            if (cursor != NULL) {
               ICloseable::Probe(cursor->mCursor)->Close();
            }
        }
        mManagedCursors.Clear();
    }

    // Close any open search dialog
    if (mSearchManager != NULL) {
        FAIL_RETURN(mSearchManager->StopSearch());
    }

    AutoPtr<IApplication> app = GetApplication();
    return app->DispatchActivityDestroyed(this);
}

ECode Activity::ReportFullyDrawn()
{
    if (mDoReportFullyDrawn) {
        mDoReportFullyDrawn = FALSE;
        // try {
        AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();
        return defaultAM->ReportActivityFullyDrawn(mToken);
        // } catch (RemoteException e) {
        // }
    }
    return NOERROR;
}

ECode Activity::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    if (DEBUG_LIFECYCLE) {
        String temp;
        newConfig->ToString(&temp);
        StringBuilder sb("onConfigurationChanged ");
        sb += ToString();
        sb += ": ";
        sb += temp;
        Slogger::V(TAG, sb.ToString());
    }
    mCalled = TRUE;

    mFragments->DispatchConfigurationChanged(newConfig);

    if (mWindow != NULL) {
        // Pass the configuration changed event to the window
        return mWindow->OnConfigurationChanged(newConfig);
    }

    if (mActionBar != NULL) {
        // Do this last; the action bar will need to access
        // view changes from above.
        return mActionBar->OnConfigurationChanged(newConfig);
    }
    return NOERROR;
}

ECode Activity::GetChangingConfigurations(
    /* [out] */ Int32* config)
{
    VALIDATE_NOT_NULL(config);
    *config = mConfigChangeFlags;
    return NOERROR;
}

ECode Activity::GetLastNonConfigurationInstance(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    *object = NULL;
    if (mLastNonConfigurationInstances != NULL) {
        return mLastNonConfigurationInstances->GetActivity(object);
    }
    return NOERROR;
}

ECode Activity::OnRetainNonConfigurationInstance(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    *object = NULL;
    return NOERROR;
}

ECode Activity::GetLastNonConfigurationChildInstances(
    /* [out] */ IHashMap** data)
{
    VALIDATE_NOT_NULL(data);
    *data = NULL;

    if (mLastNonConfigurationInstances != NULL) {
        mLastNonConfigurationInstances->GetChildren(data);
    }
    return NOERROR;
}

ECode Activity::OnRetainNonConfigurationChildInstances(
    /* [out] */ IHashMap** map)
{
    VALIDATE_NOT_NULL(map);
    *map = NULL;
    return NOERROR;
}

ECode Activity::RetainNonConfigurationInstances(
    /* [out] */ IActivityNonConfigurationInstances** instance)
{
    VALIDATE_NOT_NULL(instance);
    *instance = NULL;

    AutoPtr<IInterface> activity;
    OnRetainNonConfigurationInstance((IInterface**)&activity);

    AutoPtr<IHashMap> children;
    OnRetainNonConfigurationChildInstances((IHashMap**)&children);

    AutoPtr<IArrayList> fragments;
    FAIL_RETURN(mFragments->RetainNonConfig((IArrayList**)&fragments));

    Boolean retainLoaders = FALSE;

    if (mAllLoaderManagers != NULL) {
        // prune out any loader managers that were already stopped and so
        // have nothing useful to retain.
        Int32 N;
        mAllLoaderManagers->GetSize(&N);
        AutoPtr<ArrayOf<ILoaderManagerImpl*> > loaders = ArrayOf<ILoaderManagerImpl*>::Alloc(N);
        for (Int32 i = N - 1; i >= 0; i--) {
            AutoPtr<IInterface> obj;
            mAllLoaderManagers->GetValueAt(i, (IInterface**)&obj);
            loaders->Set(i, ILoaderManagerImpl::Probe(obj));
        }
        assert(0 && "TODO");
        // for (Int32 i = 0; i < N; i++) {
        //     LoaderManagerImpl* lm = (LoaderManagerImpl*)(*loaders)[i];
        //     lm->IsRetaining(&isRetaining);
        //     if (isRetaining) {
        //         retainLoaders = TRUE;
        //     }
        //     else {
        //         lm->DoDestroy();
        //         lm->GetName(&who);
        //         mAllLoaderManagers->Remove(who);
        //     }
        // }
    }

    if (activity == NULL && children == NULL && fragments == NULL
            && !retainLoaders && mVoiceInteractor == NULL) {
        return NOERROR;
    }

    return CActivityNonConfigurationInstances::New(activity, children,
            fragments, mAllLoaderManagers, mVoiceInteractor, instance);
}

ECode Activity::OnLowMemory()
{
    if (DEBUG_LIFECYCLE) {
        Slogger::V(TAG, "onLowMemory %s", ToString().string());
    }
    mCalled = TRUE;
    return mFragments->DispatchLowMemory();
}

ECode Activity::OnTrimMemory(
    /* [in] */ Int32 level)
{
    if (DEBUG_LIFECYCLE) {
        Slogger::V(TAG, "onTrimMemory %s %d", ToString().string(), level);
    }
     mCalled = TRUE;
     return mFragments->DispatchTrimMemory(level);
}

ECode Activity::GetFragmentManager(
    /* [out] */ IFragmentManager** fmanager)
{
    VALIDATE_NOT_NULL(fmanager);
    *fmanager = IFragmentManager::Probe(mFragments);
    REFCOUNT_ADD(*fmanager)
    return NOERROR;
}

ECode Activity::InvalidateFragment(
    /* [in] */ const String& who)
{
    //Log.v(TAG, "invalidateFragmentIndex: index=" + index);
    if (mAllLoaderManagers != NULL) {
        AutoPtr<ICharSequence> key = CoreUtils::Convert(who);
        IInterface* keyObj = IInterface::Probe(key);
        AutoPtr<ILoaderManagerImpl> lm;
        mAllLoaderManagers->Get(keyObj, (IInterface**)&lm);
        if (lm != NULL) {
            assert(0 && "TODO");
            // LoaderManagerImpl* lmi = (LoaderManagerImpl*)lm.Get();
            // Boolean retaining;
            // lmi->IsRetaining(&retaining);
            // if (!retaining) {
            //     lmi->DoDestroy();
            //     mAllLoaderManagers->Remove(keyObj);
            // }
        }
    }

    return NOERROR;
}

ECode Activity::OnAttachFragment(
    /* [in] */ IFragment* fragment)
{
    return NOERROR;
}

ECode Activity::ManagedQuery(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String&selection,
    /* [in] */ const String&sortOrder,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor);

    AutoPtr<IContentResolver> cResolver;
    GetContentResolver((IContentResolver**)&cResolver);
    AutoPtr<ICursor> c;
    cResolver->Query(uri, projection, selection, NULL, sortOrder, (ICursor**)&c);
    if (c != NULL) {
        StartManagingCursor(c);
    }
    *cursor = c;
    REFCOUNT_ADD(*cursor);
    return NOERROR;
}

ECode Activity::ManagedQuery(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String&selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String&sortOrder,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor);

    AutoPtr<IContentResolver> cResolver;
    GetContentResolver((IContentResolver**)&cResolver);
    AutoPtr<ICursor> c;
    cResolver->Query(uri, projection, selection, selectionArgs, sortOrder, (ICursor**)&c);
    if (c != NULL) {
        StartManagingCursor(c);
    }
    *cursor = c;
    REFCOUNT_ADD(*cursor);
    return NOERROR;
}

ECode Activity::StartManagingCursor(
    /* [in] */ ICursor* c)
{
    AutoLock lock(mManagedCursorsLock);
    AutoPtr<ManagedCursor> mc = new ManagedCursor(c);
    mManagedCursors.PushBack(mc);
    return NOERROR;
}

ECode Activity::StopManagingCursor(
    /* [in] */ ICursor* c)
{
    AutoLock lock(mManagedCursorsLock);
    List<AutoPtr<ManagedCursor> >::Iterator it;
    for (it = mManagedCursors.Begin(); it != mManagedCursors.End(); ++it) {
        AutoPtr<ManagedCursor> mc = *it;
        if (mc->mCursor.Get() == c) {
            mManagedCursors.Remove(*it);
            break;
        }
    }
    return NOERROR;
}

ECode Activity::SetPersistent(
    /* [in] */ Boolean isPersistent)
{
    return NOERROR;
}

ECode Activity::FindViewById(
    /* [in] */ Int32 id,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    return GetWindow()->FindViewById(id, view);
}

AutoPtr<IView> Activity::FindViewById(
    /* [in] */ Int32 id)
{
    AutoPtr<IView> view;
    GetWindow()->FindViewById(id, (IView**)&view);
    return view;
}

ECode Activity::SetActionBar(
    /* [in] */ IToolbar* toolbar)
{
    AutoPtr<IActionBar> ab;
    GetActionBar((IActionBar**)&ab);

    if (IWindowDecorActionBar::Probe(ab) != NULL) {
        Logger::E(TAG, "This Activity already has an action bar supplied "
                "by the window decor. Do not request Window.FEATURE_ACTION_BAR and set "
                "android:windowActionBar to false in your theme to use a Toolbar instead.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<ICharSequence> title;
    GetTitle((ICharSequence**)&title);
    AutoPtr<IToolbarActionBar> tbab;
    CToolbarActionBar::New(toolbar, title, this, (IToolbarActionBar**)&tbab);
    mActionBar = IActionBar::Probe(tbab);
    AutoPtr<IWindowCallback> cb;
    tbab->GetWrappedWindowCallback((IWindowCallback**)&cb);
    mWindow->SetCallback(cb);
    Boolean result;
    return mActionBar->InvalidateOptionsMenu(&result);
}

ECode Activity::GetActionBar(
    /* [out] */ IActionBar** actionbar)
{
    VALIDATE_NOT_NULL(actionbar);
    *actionbar = NULL;

    FAIL_RETURN(InitWindowDecorActionBar());
    *actionbar = mActionBar;
    REFCOUNT_ADD(*actionbar)
    return NOERROR;
}

ECode Activity::InitWindowDecorActionBar()
{
    AutoPtr<IWindow> window = GetWindow();

    // Initializing the window decor can change window feature flags.
    // Make sure that we have the correct set before performing the test below.
    AutoPtr<IView> d;
    FAIL_RETURN(window->GetDecorView((IView**)&d));

    Boolean isChild;
    IsChild(&isChild);
    if (isChild) {
        return NOERROR;
    }

    Boolean hasFeature;
    FAIL_RETURN(window->HasFeature(IWindow::FEATURE_ACTION_BAR, &hasFeature));
    if (!hasFeature) {
        return NOERROR;
    }

    if (mActionBar != NULL) {
        return NOERROR;
    }

    FAIL_RETURN(CWindowDecorActionBar::New(this, (IActionBar**)&mActionBar));
    FAIL_RETURN(mActionBar->SetDefaultDisplayHomeAsUpEnabled(mEnableDefaultActionBarUp))

    IComponentInfo* ci = IComponentInfo::Probe(mActivityInfo);
    Int32 iconRes, logoRes;
    ci->GetIconResource(&iconRes);
    ci->GetLogoResource(&logoRes);
    FAIL_RETURN(mWindow->SetDefaultIcon(iconRes))
    FAIL_RETURN(mWindow->SetDefaultLogo(logoRes))
    return NOERROR;
}

ECode Activity::SetContentView(
    /* [in] */ Int32 layoutResID)
{
    FAIL_RETURN(GetWindow()->SetContentView(layoutResID));
    return InitWindowDecorActionBar();
}

ECode Activity::SetContentView(
    /* [in] */ IView* view)
{
    FAIL_RETURN(GetWindow()->SetContentView(view));
    return InitWindowDecorActionBar();
}

ECode Activity::SetContentView(
    /* [in] */ IView* view,
    /* [in] */ IViewGroupLayoutParams* params)
{
    FAIL_RETURN(GetWindow()->SetContentView(view, params));
    return InitWindowDecorActionBar();
}

ECode Activity::AddContentView(
    /* [in] */ IView* view,
    /* [in] */ IViewGroupLayoutParams* params)
{
    FAIL_RETURN(GetWindow()->AddContentView(view, params));
    return InitWindowDecorActionBar();
}

ECode Activity::GetContentTransitionManager(
    /* [out] */ ITransitionManager** tm)
{
    VALIDATE_NOT_NULL(tm)
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    return window->GetTransitionManager(tm);
}

ECode Activity::SetContentTransitionManager(
    /* [in] */ ITransitionManager* tm)
{
    VALIDATE_NOT_NULL(tm)
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    window->SetTransitionManager(tm);
    return NOERROR;
}

ECode Activity::GetContentScene(
    /* [out] */ IScene** scene)
{
    VALIDATE_NOT_NULL(scene)
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    return window->GetContentScene(scene);
}

ECode Activity::SetFinishOnTouchOutside(
    /* [in] */ Boolean finish)
{
    return mWindow->SetCloseOnTouchOutside(finish);
}

ECode Activity::SetDefaultKeyMode(
    /* [in] */  Int32 mode)
{
    mDefaultKeyMode = mode;

    // Some modes use a SpannableStringBuilder to track & dispatch input events
    // This list must remain in sync with the switch in onKeyDown()
    switch (mode) {
    case IActivity::DEFAULT_KEYS_DISABLE:
    case IActivity::DEFAULT_KEYS_SHORTCUT:
        mDefaultKeySsb = NULL;      // not used in these modes
        break;
    case IActivity::DEFAULT_KEYS_DIALER:
    case IActivity::DEFAULT_KEYS_SEARCH_LOCAL:
    case IActivity::DEFAULT_KEYS_SEARCH_GLOBAL: {
        mDefaultKeySsb = NULL;
        CSpannableStringBuilder::New((ISpannableStringBuilder**)&mDefaultKeySsb);
        Selection::SetSelection(ISpannable::Probe(mDefaultKeySsb), 0);
        break;
    }
    default:
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NOERROR;
}

ECode Activity::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (keyCode == IKeyEvent::KEYCODE_BACK) {
        AutoPtr<IApplicationInfo> appInfo;
        FAIL_RETURN(GetApplicationInfo((IApplicationInfo**)&appInfo));
        Int32 targetSdkVersion;
        FAIL_RETURN(appInfo->GetTargetSdkVersion(&targetSdkVersion));

        if (targetSdkVersion >= Build::VERSION_CODES::ECLAIR) {
            FAIL_RETURN(event->StartTracking());
        }
        else {
            FAIL_RETURN(OnBackPressed());
        }

        *result = TRUE;
        return NOERROR;
    }

    if (mDefaultKeyMode == IActivity::DEFAULT_KEYS_DISABLE) {
        return NOERROR;
    }
    else if (mDefaultKeyMode == IActivity::DEFAULT_KEYS_SHORTCUT) {
        AutoPtr<IWindow> win = GetWindow();

        Boolean state;
        FAIL_RETURN(win->PerformPanelShortcut(IWindow::FEATURE_OPTIONS_PANEL,
                keyCode, event, IMenu::FLAG_ALWAYS_PERFORM_CLOSE, &state));

        if (state) {
            *result = TRUE;
        }
        else {
            *result = FALSE;
        }
        return NOERROR;
    }
    else {
        // Common code for DEFAULT_KEYS_DIALER & DEFAULT_KEYS_SEARCH_*
        Boolean clearSpannable = FALSE;
        Boolean handled = FALSE, isSystem;
        Int32 repeatCount;
        event->GetRepeatCount(&repeatCount);
        event->IsSystem(&isSystem);

        if ((repeatCount != 0) || isSystem) {
            clearSpannable = TRUE;
            handled = FALSE;
        }
        else {
            AutoPtr<ITextKeyListenerHelper> listenerHelper;
            CTextKeyListenerHelper::AcquireSingleton((ITextKeyListenerHelper**)&listenerHelper);
            AutoPtr<ITextKeyListener> listener;
            listenerHelper->GetInstance((ITextKeyListener**)&listener);
            IKeyListener* listenerK = IKeyListener::Probe(listener);
            FAIL_RETURN(listenerK->OnKeyDown(NULL, IEditable::Probe(mDefaultKeySsb), keyCode, event, &handled));
            Int32 length = 0;
            ICharSequence::Probe(mDefaultKeySsb)->GetLength(&length);
            if (handled && length > 0) {
                // something useable has been typed - dispatch it now.

                String str = Object::ToString(mDefaultKeySsb);
                clearSpannable = TRUE;

                switch (mDefaultKeyMode) {
                case IActivity::DEFAULT_KEYS_DIALER: {
                    StringBuilder sb("tel:");
                    sb += str;
                    AutoPtr<IUri> uri;
                    AutoPtr<IUriHelper> helper;
                    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
                    FAIL_RETURN(helper->Parse(sb.ToString(), (IUri**)&uri));

                    AutoPtr<IIntent> intent;
                    CIntent::New(IIntent::ACTION_DIAL, uri, (IIntent**)&intent);
                    FAIL_RETURN(intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK));
                    FAIL_RETURN(StartActivity(intent));
                    break;
                }

                case IActivity::DEFAULT_KEYS_SEARCH_LOCAL:
                    FAIL_RETURN(StartSearch(str, FALSE, NULL, FALSE));
                    break;
                case IActivity::DEFAULT_KEYS_SEARCH_GLOBAL:
                    FAIL_RETURN(StartSearch(str, FALSE, NULL, TRUE));
                    break;
                }
            }
        }
        if (clearSpannable) {
            IEditable* e = IEditable::Probe(mDefaultKeySsb);
            FAIL_RETURN(e->Clear());
            FAIL_RETURN(e->ClearSpans());
            Selection::SetSelection(ISpannable::Probe(mDefaultKeySsb), 0);
        }

        *result = handled;
        return NOERROR;
    }
}

ECode Activity::OnKeyLongPress(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode Activity::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    AutoPtr<IApplicationInfo> appInfo;
    FAIL_RETURN(GetApplicationInfo((IApplicationInfo**)&appInfo));
    Int32 targetSdkVersion;
    FAIL_RETURN(appInfo->GetTargetSdkVersion(&targetSdkVersion));

    if (targetSdkVersion >= Build::VERSION_CODES::ECLAIR) {
        Boolean tracking, canceled;
        FAIL_RETURN(event->IsTracking(&tracking));
        FAIL_RETURN(event->IsCanceled(&canceled));

        if (keyCode == IKeyEvent::KEYCODE_BACK && tracking && !canceled) {
            FAIL_RETURN(OnBackPressed());

            *result = TRUE;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode Activity::OnKeyMultiple(
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 count,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode Activity::OnBackPressed()
{
    Boolean actionBar = FALSE;
    if (mActionBar != NULL) {
        mActionBar->CollapseActionView(&actionBar);
    }
    if (actionBar) {
        return NOERROR;
    }

    Boolean val = FALSE;
    FAIL_RETURN(IFragmentManager::Probe(mFragments)->PopBackStackImmediate(&val));
    if (!val) {
        return FinishAfterTransition();
    }
    return NOERROR;
}

ECode Activity::OnKeyShortcut(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode Activity::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    Boolean val;
    FAIL_RETURN(mWindow->ShouldCloseOnTouch(this, event, &val));
    if (val) {
        FAIL_RETURN(Finish());
        *result = TRUE;
    }
    return NOERROR;
}

ECode Activity::OnTrackballEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode Activity::OnGenericMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode Activity::OnUserInteraction()
{
    return NOERROR;
}

ECode Activity::OnWindowAttributesChanged(
    /* [in] */ IWindowManagerLayoutParams* params)
{
    // Update window manager if: we have a view, that view is
    // attached to its parent (which will be a RootView), and
    // this activity is not embedded.
    if (mParent == NULL) {
        AutoPtr<IView> decor = mDecor;
        if (decor != NULL) {
            AutoPtr<IViewParent> parent;
            FAIL_RETURN(decor->GetParent((IViewParent**)&parent));
            if (parent != NULL) {
                AutoPtr<IWindowManager> winmanager = GetWindowManager();
                return IViewManager::Probe(winmanager)->UpdateViewLayout(
                    decor, IViewGroupLayoutParams::Probe(params));
            }
        }
    }

    return NOERROR;
}

ECode Activity::OnContentChanged()
{
    return NOERROR;
}

ECode Activity::OnWindowFocusChanged(
    /* [in] */ Boolean hasFocus)
{
    return NOERROR;
}

ECode Activity::OnAttachedToWindow()
{
    return NOERROR;
}

ECode Activity::OnDetachedFromWindow()
{
    return NOERROR;
}

ECode Activity::HasWindowFocus(
    /* [out] */ Boolean* hasFocus)
{
    VALIDATE_NOT_NULL(hasFocus);
    *hasFocus = FALSE;

    AutoPtr<IWindow> w = GetWindow();
    if (w != NULL) {
        AutoPtr<IView> d;
        FAIL_RETURN(w->GetDecorView((IView**)&d));
        if (d != NULL) {
            return d->HasWindowFocus(hasFocus);
        }
    }
    return NOERROR;
}

ECode Activity::OnWindowDismissed()
{
    return Finish();
}

ECode Activity::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* isConsumed)
{
    VALIDATE_NOT_NULL(isConsumed);
    *isConsumed = FALSE;

    FAIL_RETURN(OnUserInteraction());


    // Let action bars open menus in response to the menu key prioritized over
    // the window handling it
    Int32 keyCode;
    event->GetKeyCode(&keyCode);
    Boolean succeeded = FALSE;
    if (keyCode == IKeyEvent::KEYCODE_MENU
        && mActionBar != NULL && (mActionBar->OnMenuKeyEvent(event, &succeeded), succeeded)) {
        *isConsumed = TRUE;
        return NOERROR;;
    }


    AutoPtr<IWindow> win = GetWindow();
    FAIL_RETURN(win->SuperDispatchKeyEvent(event, &succeeded));
    if (succeeded) {
        *isConsumed = TRUE;
        return NOERROR;
    }

    AutoPtr<IView> decor = mDecor;
    if (decor == NULL) {
        FAIL_RETURN(win->GetDecorView((IView**)&decor));
    }

    AutoPtr<IDispatcherState> dispatcher;
    if (decor != NULL) {
        FAIL_RETURN(decor->GetKeyDispatcherState((IDispatcherState**)&dispatcher));
    }

    event->Dispatch(this, dispatcher,
            TO_IINTERFACE(this), isConsumed);
    return NOERROR;
}

ECode Activity::DispatchKeyShortcutEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    FAIL_RETURN(OnUserInteraction());
     AutoPtr<IWindow> win = GetWindow();
     Boolean succeeded = FALSE;
     FAIL_RETURN(win->SuperDispatchKeyShortcutEvent(event, &succeeded));
     if (succeeded) {
         *result = TRUE;
         return NOERROR;
     }
     Int32 keyCode = 0;
     event->GetKeyCode(&keyCode);
     return OnKeyShortcut(keyCode, event, result);
}

ECode Activity::DispatchTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* isConsumed)
{
    VALIDATE_NOT_NULL(isConsumed);
    *isConsumed = FALSE;

    Int32 action;
    event->GetAction(&action);
    if (action == IMotionEvent::ACTION_DOWN) {
        FAIL_RETURN(OnUserInteraction());
    }

    AutoPtr<IWindow> win = GetWindow();
    Boolean succeeded;
    FAIL_RETURN(win->SuperDispatchTouchEvent(event, &succeeded));

    if (succeeded) {
        *isConsumed = TRUE;
        return NOERROR;
    }

    return OnTouchEvent(event, isConsumed);
}

ECode Activity::DispatchTrackballEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* isConsumed)
{
    VALIDATE_NOT_NULL(isConsumed);
    *isConsumed = FALSE;

    OnUserInteraction();

    AutoPtr<IWindow> win = GetWindow();
    Boolean succeeded;
    FAIL_RETURN(win->SuperDispatchTrackballEvent(event, &succeeded));

    if (succeeded) {
        *isConsumed = TRUE;
        return NOERROR;
    }

    return OnTrackballEvent(event, isConsumed);
}

ECode Activity::DispatchGenericMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* isConsumed)
{
    VALIDATE_NOT_NULL(isConsumed);
    *isConsumed = FALSE;

     OnUserInteraction();

     AutoPtr<IWindow> win = GetWindow();
     Boolean succeeded;
     FAIL_RETURN(win->SuperDispatchGenericMotionEvent(event, &succeeded));

     if (succeeded) {
         *isConsumed = TRUE;
         return NOERROR;
     }

     return OnGenericMotionEvent(event, isConsumed);
}

ECode Activity::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* isConsumed)
{
    VALIDATE_NOT_NULL(isConsumed);
    IAccessibilityRecord* ar = IAccessibilityRecord::Probe(event);
    // TODO
    // event.setClassName(getClass().getName());
    String pkgName;
    GetPackageName(&pkgName);
    AutoPtr<ICharSequence> nameS;
    CString::New(pkgName, (ICharSequence**)&nameS);
    event->SetPackageName(nameS);

    AutoPtr<IWindowManagerLayoutParams> params;
    GetWindow()->GetAttributes((IWindowManagerLayoutParams**)&params);
    IViewGroupLayoutParams* vglp = IViewGroupLayoutParams::Probe(params);
    Int32 width, height;
    vglp->GetWidth(&width);
    vglp->GetHeight(&height);
    Boolean isFullScreen = (width == IViewGroupLayoutParams::MATCH_PARENT) &&
        (height == IViewGroupLayoutParams::MATCH_PARENT);
    ar->SetFullScreen(isFullScreen);

    AutoPtr<ICharSequence> title;
    GetTitle((ICharSequence**)&title);
    if (!TextUtils::IsEmpty(title)) {
        AutoPtr<IList> txtContainer;
        ar->GetText((IList**)&txtContainer);
        txtContainer->Add(IInterface::Probe(title));
    }

    *isConsumed = TRUE;
    return NOERROR;
}

ECode Activity::OnCreatePanelView(
    /* [in] */ Int32 featureId,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = NULL;
    return NOERROR;
}

ECode Activity::OnCreatePanelMenu(
    /* [in] */ Int32 featureId,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* allowToShow)
{
    VALIDATE_NOT_NULL(allowToShow);
    *allowToShow = FALSE;

    if (featureId == IWindow::FEATURE_OPTIONS_PANEL) {
        Boolean show;
        FAIL_RETURN(OnCreateOptionsMenu(menu, &show));

        AutoPtr<IMenuInflater> mi;
        FAIL_RETURN(GetMenuInflater((IMenuInflater**)&mi));
        Boolean result = FALSE;
        FAIL_RETURN(mFragments->DispatchCreateOptionsMenu(menu, mi, &result));
        show |= result;
        *allowToShow = show;
    }

    return NOERROR;
}

ECode Activity::OnPreparePanel(
    /* [in] */ Int32 featureId,
    /* [in] */ IView* view,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* allowToShow)
{
    VALIDATE_NOT_NULL(allowToShow);
    *allowToShow = TRUE;

    if (featureId == IWindow::FEATURE_OPTIONS_PANEL && menu != NULL) {
        Boolean goforit = FALSE;
        OnPrepareOptionsMenu(menu, &goforit);
        Boolean result = FALSE;
        FAIL_RETURN(mFragments->DispatchPrepareOptionsMenu(menu, &result));
        goforit |= result;
        *allowToShow = goforit;
        return NOERROR;
    }

    return NOERROR;
}

ECode Activity::OnMenuOpened(
    /* [in] */ Int32 featureId,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* allowToOpen)
{
    VALIDATE_NOT_NULL(allowToOpen);
    *allowToOpen = TRUE;

    if (featureId == IWindow::FEATURE_ACTION_BAR) {
        InitWindowDecorActionBar();
        if (mActionBar != NULL) {
            FAIL_RETURN(mActionBar->DispatchMenuVisibilityChanged(TRUE));
        }
        else {
            Logger::E(TAG, "Tried to open action bar menu with no action bar");
        }
    }

    return NOERROR;
}

ECode Activity::OnMenuItemSelected(
    /* [in] */ Int32 featureId,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* toFinish)
{
    VALIDATE_NOT_NULL(toFinish);
    *toFinish = FALSE;

    // AutoPtr<ICharSequence> titleCondensed;
    // item->GetTitleCondensed((ICharSequence**)&titleCondensed);
    switch(featureId) {
        case IWindow::FEATURE_OPTIONS_PANEL: {
            // Put event logging here so it gets called even if subclass
            // doesn't call through to superclass's implmeentation of each
            // of these methods below
            //EventLog.writeEvent(50000, 0, item.getTitleCondensed());
            Boolean val;
            FAIL_RETURN(OnOptionsItemSelected(item, &val));
            if (val) {
                *toFinish = TRUE;
                return NOERROR;
            }

            FAIL_RETURN(mFragments->DispatchOptionsItemSelected(item, &val));
            if (val) {
                *toFinish = TRUE;
                return NOERROR;
            }

            Int32 itemId;
            item->GetItemId(&itemId);
            Int32 options = 0;
            if (mActionBar != NULL) {
                FAIL_RETURN(mActionBar->GetDisplayOptions(&options));
            }

            if (itemId == R::id::home
                    && (options & IActionBar::DISPLAY_HOME_AS_UP) != 0) {
                if (mParent == NULL) {
                    return OnNavigateUp(toFinish);
                } else {
                    return mParent->OnNavigateUpFromChild(this, toFinish);
                }
            }
            *toFinish = FALSE;
            return NOERROR;
        }
        case IWindow::FEATURE_CONTEXT_MENU: {
//          EventLog.writeEvent(50000, 1, item.getTitleCondensed());
            Boolean isSelected;
            FAIL_RETURN(OnContextItemSelected(item, &isSelected));
            if (isSelected) {
                *toFinish = TRUE;
                return NOERROR;
            }
            return mFragments->DispatchContextItemSelected(item, toFinish);
            return NOERROR;
        }

        default:
            *toFinish = FALSE;
            break;
    }

    return NOERROR;
}

ECode Activity::OnPanelClosed(
    /* [in] */ Int32 featureId,
    /* [in] */ IMenu* menu)
{
    switch(featureId) {
        case IWindow::FEATURE_OPTIONS_PANEL: {
            FAIL_RETURN(mFragments->DispatchOptionsMenuClosed(menu));
            FAIL_RETURN(OnOptionsMenuClosed(menu));
            break;
        }
        case IWindow::FEATURE_CONTEXT_MENU: {
            FAIL_RETURN(OnContextMenuClosed(menu));
            break;
        }
        case IWindow::FEATURE_ACTION_BAR:
            FAIL_RETURN(InitWindowDecorActionBar());
            FAIL_RETURN(mActionBar->DispatchMenuVisibilityChanged(FALSE));
            break;
    }

    return NOERROR;
}

ECode Activity::InvalidateOptionsMenu()
{
    Boolean bval;
    if (mActionBar == NULL || (mActionBar->InvalidateOptionsMenu(&bval), !bval)) {
        return mWindow->InvalidatePanelMenu(IWindow::FEATURE_OPTIONS_PANEL);
    }
    return NOERROR;
}

ECode Activity::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* allowToShow)
{
    VALIDATE_NOT_NULL(allowToShow);
    *allowToShow = TRUE;

    if (mParent != NULL) {
        return mParent->OnCreateOptionsMenu(menu, allowToShow);
    }

    return NOERROR;
}

ECode Activity::OnPrepareOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;

    if (mParent != NULL) {
        return mParent->OnPrepareOptionsMenu(menu, result);
    }

    return NOERROR;
}

ECode Activity::OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (mParent != NULL) {
        return mParent->OnOptionsItemSelected(item, result);
    }

    return NOERROR;
}

ECode Activity::OnNavigateUp(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // Automatically handle hierarchical Up navigation if the proper
    // metadata is available.
    AutoPtr<IIntent> upIntent;
    GetParentActivityIntent((IIntent**)&upIntent);
    if (upIntent != NULL) {
        Boolean recreate;
        ShouldUpRecreateTask(upIntent, &recreate);
        String task;
        mActivityInfo->GetTaskAffinity(&task);
        if (task.IsNull()) {
            // Activities with a NULL affinity are special; they really shouldn't
            // specify a parent activity intent in the first place. Just finish
            // the current activity and call it a day.
            Finish();
        } else if (recreate) {
            AutoPtr<ITaskStackBuilderHelper> taskBHelper;
            CTaskStackBuilderHelper::AcquireSingleton((ITaskStackBuilderHelper**)&taskBHelper);
            AutoPtr<ITaskStackBuilder> b;
            taskBHelper->Create(this, (ITaskStackBuilder**)&b);
            OnCreateNavigateUpTaskStack(b);
            OnPrepareNavigateUpTaskStack(b);
            b->StartActivities();

            // We can't finishAffinity if we have a result.
            // Fall back and simply finish the current activity instead.
            if (mResultCode != RESULT_CANCELED || mResultData != NULL) {
                // Tell the developer what's going on to avoid hair-pulling.
                Slogger::I(TAG, "onNavigateUp only finishing topmost activity to return a result");
                Finish();
            } else {
                FinishAffinity();
            }
        } else {
            Boolean navigate;
            NavigateUpTo(upIntent, &navigate);
        }
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode Activity::OnNavigateUpFromChild(
    /* [in] */ IActivity* child,
    /* [out] */ Boolean* result)
{
    return OnNavigateUp(result);
}

ECode Activity::OnCreateNavigateUpTaskStack(
    /* [in] */ ITaskStackBuilder* builder)
{
    VALIDATE_NOT_NULL(builder);
    return builder->AddParentStack(this);
}

ECode Activity::OnPrepareNavigateUpTaskStack(
    /* [in] */ ITaskStackBuilder* builder)
{
    return NOERROR;
}

ECode Activity::OnOptionsMenuClosed(
    /* [in] */ IMenu* menu)
{
    if (mParent != NULL) {
        return mParent->OnOptionsMenuClosed(menu);
    }

    return NOERROR;
}

ECode Activity::OpenOptionsMenu()
{
    Boolean bval;
    if (mActionBar == NULL || (mActionBar->OpenOptionsMenu(&bval), !bval)) {
        return mWindow->OpenPanel(IWindow::FEATURE_OPTIONS_PANEL, NULL);
    }
    return NOERROR;
}

ECode Activity::CloseOptionsMenu()
{
    return mWindow->ClosePanel(IWindow::FEATURE_OPTIONS_PANEL);
}

ECode Activity::OnCreateContextMenu(
    /* [in] */ IContextMenu* menu,
    /* [in] */ IView* v,
    /* [in] */ IContextMenuInfo* menuInfo)
{
    return NOERROR;
}

ECode Activity::RegisterForContextMenu(
    /* [in] */ IView* view)
{
    VALIDATE_NOT_NULL(view);
    return view->SetOnCreateContextMenuListener(
            this);
}

ECode Activity::UnregisterForContextMenu(
    /* [in] */ IView* view)
{
    VALIDATE_NOT_NULL(view);
    return view->SetOnCreateContextMenuListener(NULL);
}

ECode Activity::OpenContextMenu(
    /* [in] */ IView* view)
{
    VALIDATE_NOT_NULL(view);
    Boolean res;
    return view->ShowContextMenu(&res);
}

ECode Activity::CloseContextMenu()
{
    return mWindow->ClosePanel(IWindow::FEATURE_CONTEXT_MENU);
}

ECode Activity::OnContextItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (mParent != NULL) {
        return mParent->OnContextItemSelected(item, result);
    }

    return NOERROR;
}

ECode Activity::OnContextMenuClosed(
        /* [in] */ IMenu* menu)
{
    if (mParent != NULL) {
        return mParent->OnContextMenuClosed(menu);
    }

    return NOERROR;
}

AutoPtr<IDialog> Activity::OnCreateDialog(
    /* [in] */ Int32 id)
{
    return NULL;
}

AutoPtr<IDialog> Activity::OnCreateDialog(
    /* [in] */ Int32 id,
    /* [in] */ IBundle* args)
{
    return OnCreateDialog(id);
}

void Activity::OnPrepareDialog(
    /* [in] */ Int32 id,
    /* [in] */ IDialog* dialog)
{
    if (dialog);
        dialog->SetOwnerActivity(this);
}

void Activity::OnPrepareDialog(
    /* [in] */ Int32 id,
    /* [in] */ IDialog* dialog,
    /* [in] */ IBundle* args)
{
    OnPrepareDialog(id, dialog);
}

ECode Activity::ShowDialog(
    /* [in] */ Int32 id)
{
    Boolean res;
    return ShowDialog(id, NULL, &res);
}

ECode Activity::ShowDialog(
    /* [in] */ Int32 id,
    /* [in] */ IBundle* args,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ManagedDialog> md;
    ManagedDialogHashMapIter find = mManagedDialogs.Find(id);
    if (find == mManagedDialogs.End()) {
        md = new ManagedDialog();
        md->mDialog = CreateDialog(id, NULL, args);
        if (md->mDialog == NULL) {
            *result = FALSE;
            return NOERROR;
        }
        mManagedDialogs[id] = md;
    }
    else {
        md = find->mSecond;
    }

    md->mArgs = args;
    OnPrepareDialog(id, md->mDialog, args);
    md->mDialog->Show();

    *result = TRUE;
    return NOERROR;
}

ECode Activity::DismissDialog(
    /* [in] */ Int32 id)
{
    if (mManagedDialogs.IsEmpty()) {
        //throw missingDialog(id);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    ManagedDialogHashMapIter find = mManagedDialogs.Find(id);
    if (find == mManagedDialogs.End()) {
        //throw missingDialog(id);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return IDialogInterface::Probe(find->mSecond->mDialog)->Dismiss();
}

ECode Activity::RemoveDialog(
    /* [in] */ Int32 id)
{
    if (!mManagedDialogs.IsEmpty()) {
        ManagedDialogHashMapIter find = mManagedDialogs.Find(id);
        if (find != mManagedDialogs.End()) {
            IDialogInterface::Probe(find->mSecond->mDialog)->Dismiss();
            mManagedDialogs.Erase(find);
        }
    }

    return NOERROR;
}

ECode Activity::OnSearchRequested(
    /* [out] */ Boolean* isLaunched)
{
    VALIDATE_NOT_NULL(isLaunched);
    *isLaunched = FALSE;

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IConfiguration> cfg;
    res->GetConfiguration((IConfiguration**)&cfg);
    Int32 uiMode;
    cfg->GetUiMode(&uiMode);
    if ((uiMode & IConfiguration::UI_MODE_TYPE_MASK) != IConfiguration::UI_MODE_TYPE_TELEVISION) {
        String nullStr;
        ECode ec = StartSearch(nullStr, FALSE, NULL, FALSE);
        *isLaunched = TRUE;
        return ec;
    }
    return NOERROR;
}

ECode Activity::StartSearch(
    /* [in] */ const String& initialQuery,
    /* [in] */ Boolean selectInitialQuery,
    /* [in] */ IBundle* appSearchData,
    /* [in] */ Boolean globalSearch)
{
    FAIL_RETURN(EnsureSearchManager());

    AutoPtr<IComponentName> cn;
    FAIL_RETURN(GetComponentName((IComponentName**)&cn));
    return mSearchManager->StartSearch(
            initialQuery, selectInitialQuery, cn, appSearchData, globalSearch);
}

ECode Activity::TriggerSearch(
    /* [in] */ const String& query,
    /* [in] */ IBundle* appSearchData)
{
    FAIL_RETURN(EnsureSearchManager());

    AutoPtr<IComponentName> cn;
    FAIL_RETURN(GetComponentName((IComponentName**)&cn));
    return mSearchManager->TriggerSearch(query, cn, appSearchData);
}

ECode Activity::TakeKeyEvents(
    /* [in] */ Boolean get)
{
    AutoPtr<IWindow> win = GetWindow();
    return win->TakeKeyEvents(get);
}

ECode Activity::RequestWindowFeature(
    /* [in] */ Int32 featureId,
    /* [out] */ Boolean* result)
{
    AutoPtr<IWindow> win = GetWindow();
    return win->RequestFeature(featureId, result);
}

ECode Activity::SetFeatureDrawableResource(
    /* [in] */ Int32 featureId,
    /* [in] */ Int32 resId)
{
    AutoPtr<IWindow> win = GetWindow();
    return win->SetFeatureDrawableResource(featureId, resId);
}

ECode Activity::SetFeatureDrawableUri(
    /* [in] */ Int32 featureId,
    /* [in] */ IUri* uri)
{
    AutoPtr<IWindow> win = GetWindow();
    return win->SetFeatureDrawableUri(featureId, uri);
}
ECode Activity::SetFeatureDrawable(
        /* [in] */ Int32 featureId,
        /* [in] */ IDrawable* drawable)
{
    AutoPtr<IWindow> win = GetWindow();
    return win->SetFeatureDrawable(featureId, drawable);
}

ECode Activity::SetFeatureDrawableAlpha(
        /* [in] */ Int32 featureId,
        /* [in] */ Int32 alpha)
{
    AutoPtr<IWindow> win = GetWindow();
    return win->SetFeatureDrawableAlpha(featureId, alpha);
}

ECode Activity::GetLayoutInflater(
    /* [out] */ ILayoutInflater** inflater)
{
    VALIDATE_NOT_NULL(inflater);
    AutoPtr<IWindow> win = GetWindow();
    return win->GetLayoutInflater(inflater);
}

ECode Activity::GetMenuInflater (
    /* [out] */ IMenuInflater** menuInflater)
{
    VALIDATE_NOT_NULL(menuInflater);

    // mMenuInflater and Activity have circular reference, modified by xihao
    //
    InitWindowDecorActionBar();
    if (mActionBar != NULL) {
        AutoPtr<IContext> context;
        mActionBar->GetThemedContext((IContext**)&context);
        return CMenuInflater::New(context, TO_IINTERFACE(this), menuInflater);
    }
    else {
        return CMenuInflater::New(this, menuInflater);
    }

    // //Make sure that action views can get an appropriate theme.
    // if (mMenuInflater == NULL) {
    //     InitWindowDecorActionBar();
    //     if (mActionBar != NULL) {
    //         AutoPtr<IContext> context;
    //         mActionBar->GetThemedContext((IContext**)&context);
    //         FAIL_RETURN(CMenuInflater::New(context, TO_IINTERFACE(this),
    //                (IMenuInflater**)&mMenuInflater));
    //     }
    //     else {
    //         FAIL_RETURN(CMenuInflater::New(this,
    //                (IMenuInflater**)&mMenuInflater));
    //     }
    // }
    // *menuInflater = mMenuInflater;
    // REFCOUNT_ADD(*menuInflater);
    return NOERROR;
}

ECode Activity::OnApplyThemeResource(
    /* [in] */ IResourcesTheme* theme,
    /* [in] */ Int32 resid,
    /* [in] */ Boolean first)
{
    VALIDATE_NOT_NULL(theme);
    if (mParent == NULL) {
        return theme->ApplyStyle(resid, TRUE);
    }
    else {
        AutoPtr<IResourcesTheme> parentTheme;
        FAIL_RETURN(IContext::Probe(mParent)->GetTheme((IResourcesTheme**)&parentTheme));
        theme->SetTo(parentTheme);
        FAIL_RETURN(theme->ApplyStyle(resid, FALSE));
    }

    // Get the primary color and update the TaskDescription for this activity
    if (theme != NULL) {
        AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::Theme),
            ArraySize(R::styleable::Theme));
        AutoPtr<ITypedArray> a;
        theme->ObtainStyledAttributes(attrIds, (ITypedArray**)&a);

        Int32 colorPrimary;
        a->GetColor(R::styleable::Theme_colorPrimary, 0, &colorPrimary);
        a->Recycle();
        if (colorPrimary != 0) {
            AutoPtr<IActivityManagerTaskDescription> v;
            CActivityManagerTaskDescription::New(String(NULL), NULL, colorPrimary, (IActivityManagerTaskDescription**)&v);
            SetTaskDescription(v);
        }
    }

    return NOERROR;
}

ECode Activity::StartActivityForResult(
    /* [in] */ IIntent *intent,
    /* [in] */ Int32 requestCode)
{
    return StartActivityForResult(intent, requestCode, NULL);
}

ECode Activity::StartActivityForResult(
    /* [in] */ IIntent *intent,
    /* [in] */ Int32 requestCode,
    /* [in] */ IBundle* options)
{
    if (mParent == NULL) {
        AutoPtr<IInstrumentationActivityResult> result;
        AutoPtr<IApplicationThread> at;
        FAIL_RETURN(mMainThread->GetApplicationThread((IApplicationThread**)&at));
        FAIL_RETURN(mInstrumentation->ExecStartActivity(
            this, IBinder::Probe(at), mToken, this,
            intent, requestCode, options, (IInstrumentationActivityResult**)&result))
        if (result != NULL) {
            Int32 resultCode;
            AutoPtr<IIntent> resultData;
            result->GetResultCode(&resultCode);
            result->GetResultData((IIntent**)&resultData);
            FAIL_RETURN(mMainThread->SendActivityResult(
                mToken, mEmbeddedID, requestCode, resultCode, resultData))
        }
        if (requestCode >= 0) {
            // If this start is requesting a result, we can avoid making
            // the activity visible until the result is received.  Setting
            // this code during onCreate(Bundle savedInstanceState) or onResume() will keep the
            // activity hidden during this time, to avoid flickering.
            // This can only be done when a result is requested because
            // that guarantees we will get information back when the
            // activity is finished, no matter what happens to it.
            mStartedActivity = TRUE;
        }

        AutoPtr<IView> decor;
        if (mWindow != NULL) {
            mWindow->PeekDecorView((IView**)&decor);
        }
        if (decor != NULL) {
            decor->CancelPendingInputEvents();
        }
        // TODO Consider clearing/flushing other event sources and events for child windows.
    }
    else {
        if (options != NULL) {
            return mParent->StartActivityFromChild(this, intent, requestCode, options);
        }
        else {
            // Note we want to go through this method for compatibility with
            // existing applications that may have overridden it.
            return mParent->StartActivityFromChild(this, intent, requestCode);
        }
    }

    Boolean bval;
    if (options != NULL && (IsTopOfTask(&bval), !bval)) {
        mActivityTransitionState->StartExitOutTransition(this, options);
    }

    return NOERROR;
}

ECode Activity::StartActivityForResultAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 requestCode,
    /* [in] */ IUserHandle* user)
{
    return StartActivityForResultAsUser(intent, requestCode, NULL, user);
}

ECode Activity::StartActivityForResultAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 requestCode,
    /* [in] */ IBundle* options,
    /* [in] */ IUserHandle* user)
{
    if (options != NULL) {
        mActivityTransitionState->StartExitOutTransition(this, options);
    }
    if (mParent != NULL) {
        Logger::E(TAG, "Can't be called from a child");
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<IApplicationThread> at;
    FAIL_RETURN(mMainThread->GetApplicationThread((IApplicationThread**)&at));

    AutoPtr<IInstrumentationActivityResult> ar;
    mInstrumentation->ExecStartActivity(
        this, IBinder::Probe(at), mToken, this,
        intent, requestCode, options, user, (IInstrumentationActivityResult**)&ar);
    if (ar != NULL) {
        Int32 code;
        ar->GetResultCode(&code);
        AutoPtr<IIntent> intent;
        ar->GetResultData((IIntent**)&intent);
        mMainThread->SendActivityResult(mToken, mEmbeddedID, requestCode, code, intent);
    }
    if (requestCode >= 0) {
        // If this start is requesting a result, we can avoid making
        // the activity visible until the result is received.  Setting
        // this code during onCreate(Bundle savedInstanceState) or onResume() will keep the
        // activity hidden during this time, to avoid flickering.
        // This can only be done when a result is requested because
        // that guarantees we will get information back when the
        // activity is finished, no matter what happens to it.
        mStartedActivity = TRUE;
    }

    AutoPtr<IView> decor;
    if (mWindow != NULL) {
        mWindow->PeekDecorView((IView**)&decor);
    }
    if (decor != NULL) {
        decor->CancelPendingInputEvents();
    }
    return NOERROR;
}

ECode Activity::StartIntentSenderForResult(
    /* [in] */ IIntentSender* intent,
    /* [in] */ Int32 requestCode,
    /* [in] */ IIntent* fillInIntent,
    /* [in] */ Int32 flagsMask,
    /* [in] */ Int32 flagsValues,
    /* [in] */ Int32 extraFlags)
{
    return StartIntentSenderForResult(intent, requestCode, fillInIntent, flagsMask,
            flagsValues, extraFlags, NULL);
}

ECode Activity::StartIntentSenderForResult(
    /* [in] */ IIntentSender* intent,
    /* [in] */ Int32 requestCode,
    /* [in] */ IIntent* fillInIntent,
    /* [in] */ Int32 flagsMask,
    /* [in] */ Int32 flagsValues,
    /* [in] */ Int32 extraFlags,
    /* [in] */ IBundle* options)
{
    if (mParent == NULL) {
        return StartIntentSenderForResultInner(intent, requestCode, fillInIntent,
                flagsMask, flagsValues, this, options);
    }
    else if (options != NULL) {
        return mParent->StartIntentSenderFromChild(this,
                intent, requestCode, fillInIntent, flagsMask, flagsValues, extraFlags, options);
    }
    else {
        // Note we want to go through this call for compatibility with
        // existing applications that may have overridden the method.
        return mParent->StartIntentSenderFromChild(this,
                intent, requestCode, fillInIntent, flagsMask, flagsValues, extraFlags);
    }
}

ECode Activity::StartIntentSenderForResultInner(
    /* [in] */ IIntentSender* intent,
    /* [in] */ Int32 requestCode,
    /* [in] */ IIntent* fillInIntent,
    /* [in] */ Int32 flagsMask,
    /* [in] */ Int32 flagsValues,
    /* [in] */ IActivity* activity,
    /* [in] */ IBundle* options)
{
    String resolvedType;
    if (fillInIntent != NULL) {
        AutoPtr<IContentResolver> resolver;
        GetContentResolver((IContentResolver**)&resolver);
        Boolean bval;
        FAIL_RETURN(fillInIntent->MigrateExtraStreamToClipData(&bval))
        FAIL_RETURN(fillInIntent->PrepareToLeaveProcess())
        FAIL_RETURN(fillInIntent->ResolveTypeIfNeeded(resolver, &resolvedType));
    }

    AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();

    Int32 result;
    AutoPtr<IApplicationThread> at;
    mMainThread->GetApplicationThread((IApplicationThread**)&at);

    String otherId;
    activity->GetID(&otherId);
    ECode ec = defaultAM->StartActivityIntentSender(at, intent,
        fillInIntent, resolvedType, mToken, otherId,
        requestCode, flagsMask, flagsValues, options, &result);
    if (result == IActivityManager::START_CANCELED) {
        //throw new IntentSender.SendIntentException();
        return E_SEND_INTENT_EXCEPTION;
    }
    FAIL_RETURN(ec);

    AutoPtr<IInstrumentationHelper> helper;
    CInstrumentationHelper::AcquireSingleton((IInstrumentationHelper**)&helper);
    FAIL_RETURN(helper->CheckStartActivityResult(result, NULL));

    if (requestCode >= 0) {
        // If this start is requesting a result, we can avoid making
        // the activity visible until the result is received.  Setting
        // this code during onCreate(Bundle savedInstanceState) or onResume() will keep the
        // activity hidden during this time, to avoid flickering.
        // This can only be done when a result is requested because
        // that guarantees we will get information back when the
        // activity is finished, no matter what happens to it.
        mStartedActivity = true;
    }
    return NOERROR;
}

ECode Activity::StartActivityIfNeeded(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 requestCode,
    /* [out] */ Boolean* started)
{
    return StartActivityIfNeeded(intent, requestCode, NULL, started);
}

ECode Activity::StartActivityIfNeeded(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 requestCode,
    /* [in] */ IBundle* options,
    /* [out] */ Boolean* started)
{
    VALIDATE_NOT_NULL(started);
    *started = FALSE;
    VALIDATE_NOT_NULL(intent);

    if (mParent == NULL) {
        Int32 result = IActivityManager::START_RETURN_INTENT_TO_CALLER;
        //try {
        Boolean bval;
        FAIL_RETURN(intent->MigrateExtraStreamToClipData(&bval))
        FAIL_RETURN(intent->PrepareToLeaveProcess())

        AutoPtr<IContentResolver> resolver;
        FAIL_RETURN(GetContentResolver((IContentResolver**)&resolver));
        String resolvedType;
        FAIL_RETURN(intent->ResolveTypeIfNeeded(resolver, &resolvedType));

        AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();
        AutoPtr<IApplicationThread> at;
        FAIL_RETURN(mMainThread->GetApplicationThread((IApplicationThread**)&at));

        String bpn;
        GetBasePackageName(&bpn);
        ECode ec = defaultAM->StartActivity(at, bpn, intent, resolvedType, mToken, mEmbeddedID,
                requestCode, IActivityManager::START_FLAG_ONLY_IF_NEEDED, NULL, options, &result);
        if(ec == (ECode)E_REMOTE_EXCEPTION) {
            ec = NOERROR;
        }
        FAIL_RETURN(ec);
//        } catch (RemoteException e) {
//        // Empty
//        }

        AutoPtr<IInstrumentationHelper> helper;
        CInstrumentationHelper::AcquireSingleton((IInstrumentationHelper**)&helper);
        FAIL_RETURN(helper->CheckStartActivityResult(result, intent));

        if (requestCode >= 0) {
            // If this start is requesting a result, we can avoid making
            // the activity visible until the result is received.  Setting
            // this code during onCreate(Bundle savedInstanceState) or onResume() will keep the
            // activity hidden during this time, to avoid flickering.
            // This can only be done when a result is requested because
            // that guarantees we will get information back when the
            // activity is finished, no matter what happens to it.
            mStartedActivity = TRUE;
        }
        *started = (result != IActivityManager::START_RETURN_INTENT_TO_CALLER);
    }

    Logger::E(TAG, "startActivityIfNeeded can only be called from a top-level activity");
//    throw new UnsupportedOperationException(
//            "startActivityIfNeeded can only be called from a top-level activity");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Activity::StartNextMatchingActivity(
    /* [in] */ IIntent* intent,
    /* [out] */ Boolean* started)
{
    return StartNextMatchingActivity(intent, NULL, started);
}

ECode Activity::StartNextMatchingActivity(
    /* [in] */ IIntent* intent,
    /* [in] */ IBundle* options,
    /* [out] */ Boolean* started)
{
    VALIDATE_NOT_NULL(started);
    *started = FALSE;

    if (mParent == NULL) {
//        try {
        Boolean bval;
        FAIL_RETURN(intent->MigrateExtraStreamToClipData(&bval))
        FAIL_RETURN(intent->PrepareToLeaveProcess())

        AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();

        ECode ec = defaultAM->StartNextMatchingActivity(mToken, intent, options, started);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            ec = NOERROR;
        }
//        } catch (RemoteException e) {
//        // Empty
//        }
        *started = FALSE;
        return ec;
    }

    Logger::E(TAG, "startNextMatchingActivity can only be called from a top-level activity");
//    throw new UnsupportedOperationException(
//            "startNextMatchingActivity can only be called from a top-level activity");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Activity::StartActivityFromChild(
    /* [in] */ IActivity* child,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 requestCode)
{
    return StartActivityFromChild(child, intent, requestCode, NULL);
}

ECode Activity::StartActivityFromChild(
    /* [in] */ IActivity* child,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 requestCode,
    /* [in] */ IBundle* options)
{
    AutoPtr<IInstrumentationActivityResult> result;
    AutoPtr<IApplicationThread> at;
    FAIL_RETURN(mMainThread->GetApplicationThread((IApplicationThread**)&at));

    FAIL_RETURN(mInstrumentation->ExecStartActivity(
            this, IBinder::Probe(at), mToken, child, intent, requestCode, options,
            (IInstrumentationActivityResult**)&result));
    if (result != NULL) {
        String id;
        Int32 resultCode;
        AutoPtr<IIntent> resultData;
        child->GetID(&id);
        result->GetResultCode(&resultCode);
        result->GetResultData((IIntent**)&resultData);
        return mMainThread->SendActivityResult(mToken, id,
                requestCode, resultCode, resultData);
    }
    return NOERROR;
}

ECode Activity::StartActivityFromFragment(
    /* [in] */ IFragment* fragment,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 requestCode)
{
    return StartActivityFromFragment(fragment, intent, requestCode, NULL);
}

ECode Activity::StartActivityFromFragment(
    /* [in] */ IFragment* fragment,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 requestCode,
    /* [in] */ IBundle* options)
{
    if (options != NULL) {
        mActivityTransitionState->StartExitOutTransition(this, options);
    }

    AutoPtr<IInstrumentationActivityResult> result;
    AutoPtr<IApplicationThread> at;
    FAIL_RETURN(mMainThread->GetApplicationThread((IApplicationThread**)&at));

    FAIL_RETURN(mInstrumentation->ExecStartActivity(
            this, IBinder::Probe(at), mToken, fragment, intent, requestCode, options,
            (IInstrumentationActivityResult**)&result));
    if (result != NULL) {
        Int32 resultCode;
        AutoPtr<IIntent> resultData;
        result->GetResultCode(&resultCode);
        result->GetResultData((IIntent**)&resultData);

        String who;
        fragment->GetWho(&who);
        mMainThread->SendActivityResult(mToken, who,
                requestCode, resultCode, resultData);
    }
    return NOERROR;
}

ECode Activity::StartIntentSenderFromChild(
    /* [in] */ IActivity* child,
    /* [in] */ IIntentSender* intent,
    /* [in] */ Int32 requestCode,
    /* [in] */ IIntent* fillInIntent,
    /* [in] */ Int32 flagsMask,
    /* [in] */ Int32 flagsValues,
    /* [in] */ Int32 extraFlags)
{
    return StartIntentSenderFromChild(child, intent, requestCode, fillInIntent,
            flagsMask, flagsValues, extraFlags, NULL);
}

ECode Activity::StartIntentSenderFromChild(
    /* [in] */ IActivity* child,
    /* [in] */ IIntentSender* intent,
    /* [in] */ Int32 requestCode,
    /* [in] */ IIntent* fillInIntent,
    /* [in] */ Int32 flagsMask,
    /* [in] */ Int32 flagsValues,
    /* [in] */ Int32 extraFlags,
    /* [in] */ IBundle* options)
{
    return StartIntentSenderForResultInner(intent, requestCode, fillInIntent,
            flagsMask, flagsValues, child, options);
}

ECode Activity::OverridePendingTransition(
    /* [in] */ Int32 enterAnim,
    /* [in] */ Int32 exitAnim)
{
//    try {
    AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();

    String pkg;
    GetPackageName(&pkg);
    ECode ec = defaultAM->OverridePendingTransition(mToken, pkg, enterAnim, exitAnim);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        ec = NOERROR;
    }
    return ec;
//    } catch (RemoteException e) {
//    }
}

ECode Activity::SetResult(
    /* [in] */ Int32 resultCode)
{
    mResultCode = resultCode;
    mResultData = NULL;

    return NOERROR;
}

ECode Activity::SetResult(
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    mResultCode = resultCode;
    mResultData = data;

    return NOERROR;
}

ECode Activity::GetCallingPackage(
    /* [out] */ String* package)
{
    VALIDATE_NOT_NULL(package);
//    try {
    AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();

    ECode ec = defaultAM->GetCallingPackage(mToken, package);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        ec = NOERROR;
    }
    return ec;
//    } catch (RemoteException e) {
//    }
}

ECode Activity::GetCallingActivity(
    /* [out] */ IComponentName** name)
{
    VALIDATE_NOT_NULL(name);
//    try {
    AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();

    ECode ec = defaultAM->GetCallingActivity(mToken, name);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        ec = NOERROR;
    }
    return ec;
//    } catch (RemoteException e) {
//    }
}

ECode Activity::SetVisible(
    /* [in] */ Boolean visible)
{
    if (mVisibleFromClient != visible) {
        mVisibleFromClient = visible;
        if (mVisibleFromServer) {
            if (visible) return MakeVisible();
            else return mDecor->SetVisibility(IView::INVISIBLE);
        }
    }

    return NOERROR;
}

ECode Activity::MakeVisible()
{
    if (!mWindowAdded) {
        AutoPtr<IWindowManagerLayoutParams> wmlp;
        FAIL_RETURN(GetWindow()->GetAttributes((IWindowManagerLayoutParams**)&wmlp))
        AutoPtr<IWindowManager> wm = GetWindowManager();
        IViewManager* vm = IViewManager::Probe(wm);
        FAIL_RETURN(vm->AddView(mDecor, IViewGroupLayoutParams::Probe(wmlp)))
        mWindowAdded = TRUE;
    }
    return mDecor->SetVisibility(IView::VISIBLE);
}

ECode Activity::IsFinishing(
    /* [out] */ Boolean* finished)
{
    VALIDATE_NOT_NULL(finished);
    *finished = mFinished;
    return NOERROR;
}

ECode Activity::IsDestroyed(
    /* [out] */ Boolean* destroyed)
{
    VALIDATE_NOT_NULL(destroyed);
    *destroyed = mDestroyed;
    return NOERROR;
}

ECode Activity::IsChangingConfigurations(
    /* [out] */ Boolean* changed)
{
    VALIDATE_NOT_NULL(changed);
    *changed = mChangingConfigurations;
    return NOERROR;
}

ECode Activity::Recreate()
{
    if (mParent != NULL) {
        Logger::E(TAG, "Can only be called on top-level activity");
        //throw new IllegalStateException("Can only be called on top-level activity");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<ILooper> myLooper = Looper::GetMyLooper();

    AutoPtr<ILooper> mainLooper;
    mMainThread->GetLooper((ILooper**)&mainLooper);
    if (myLooper != mainLooper) {
        Logger::E(TAG, "Must be called from main thread");
        //throw new IllegalStateException("Must be called from main thread");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return mMainThread->RequestRelaunchActivity(mToken, NULL, NULL, 0, FALSE, NULL, FALSE);
}

ECode Activity::Finish(
    /* [in] */ Boolean finishTask)
{
    if (mParent == NULL) {
        Int32 resultCode;
        AutoPtr<IIntent> resultData;
        {
            AutoLock lock(this);
            resultCode = mResultCode;
            resultData = mResultData;
        }

        if (FALSE) {
            String tokenStr;
            mToken->ToString(&tokenStr);
            StringBuilder sb("Finishing self: token=");
            sb += tokenStr;
            Logger::V(TAG, sb.ToString().string());
        }

        if (resultData != NULL) {
            FAIL_RETURN(resultData->PrepareToLeaveProcess())
        }

        AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();
        Boolean finished;
        ECode ec = defaultAM->FinishActivity(mToken, resultCode, resultData, finishTask, &finished);
        if (finished) {
            mFinished = TRUE;
        }

        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            ec = NOERROR;
        }
        return ec;
    }
    else {
        return mParent->FinishFromChild(this);
    }

    return NOERROR;
}

ECode Activity::Finish()
{
    return Finish(FALSE);
}

ECode Activity::FinishAffinity()
{
    if (mParent != NULL) {
        Logger::E(TAG, "Can not be called from an embedded activity");
        //throw new IllegalStateException("Can not be called from an embedded activity");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (mResultCode != IActivity::RESULT_CANCELED || mResultData != NULL) {
        Logger::E(TAG, "Can not be called to deliver a result");
        //throw new IllegalStateException("Can not be called to deliver a result");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    //try {
    AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();
    Boolean finished;
    ECode ec = defaultAM->FinishActivityAffinity(mToken,&finished);
    if (finished) {
        mFinished = TRUE;
    }

    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        ec = NOERROR;
    }
    return ec;
//    } catch (RemoteException e) {
//    // Empty
//    }
}

ECode Activity::FinishAfterTransition()
{
    Boolean bval;
    if (mActivityTransitionState->StartExitBackTransition(this, &bval), !bval) {
        Finish();
    }
    return NOERROR;
}

ECode Activity::FinishFromChild(
    /* [in] */ IActivity* child)
{
    return Finish();
}

ECode Activity::FinishActivity(
    /* [in] */ Int32 requestCode)
{
    if (mParent == NULL) {
//        try {
            AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();
            ECode ec = defaultAM->FinishSubActivity(mToken, mEmbeddedID, requestCode);
            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                ec = NOERROR;
            }
            return ec;
//        } catch (RemoteException e) {
//            // Empty
//        }
    } else {
        return mParent->FinishActivityFromChild(this, requestCode);
    }
}

ECode Activity::FinishActivityFromChild(
    /* [in] */ IActivity* child,
    /* [in] */ Int32 requestCode)
{
    VALIDATE_NOT_NULL(child);
//try {
    AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();
    String childID;
    child->GetID(&childID);
    ECode ec = defaultAM->FinishSubActivity(mToken, childID, requestCode);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        ec = NOERROR;
    }
    return ec;
//    } catch (RemoteException e) {
//    // Empty
//    }
}

ECode Activity::FinishAndRemoveTask()
{
    return Finish(TRUE);
}

ECode Activity::ReleaseInstance(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    // try {
    AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();
    return defaultAM->ReleaseActivityInstance(mToken, result);
    // } catch (RemoteException e) {
    //     // Empty
    // }
}

ECode Activity::OnActivityReenter(
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    return NOERROR;
}

ECode Activity::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    return NOERROR;
}

ECode Activity::CreatePendingResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ IIntent* data,
    /* [in] */ Int32 flags,
    /* [out] */ IPendingIntent** pIntent)
{
    VALIDATE_NOT_NULL(pIntent);
    *pIntent = NULL;
    VALIDATE_NOT_NULL(data);

    String packageName;
    GetPackageName(&packageName);

//    try {
    data->PrepareToLeaveProcess();

    Int32 myUserId = CUserHandle::GetMyUserId();
    AutoPtr<IBinder> token;
    if (mParent == NULL) {
        token = mToken;
    }
    else {
        mParent->GetToken((IBinder**)&token);
    }
    AutoPtr< ArrayOf<IIntent*> > intents = ArrayOf<IIntent*>::Alloc(1);
    intents->Set(0, data);

    AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();
    AutoPtr<IIIntentSender> target;
    ECode ec = defaultAM->GetIntentSender(
            IActivityManager::INTENT_SENDER_ACTIVITY_RESULT,
            packageName, token, mEmbeddedID, requestCode,
            intents, NULL, flags, NULL, myUserId,
            (IIIntentSender**)&target);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        ec = NOERROR;
    }
    FAIL_RETURN(ec);
    if (target != NULL) {
        return CPendingIntent::New(target, pIntent);
    }

//    } catch (RemoteException e) {
//    // Empty
//    }
    return NOERROR;
}

ECode Activity::SetRequestedOrientation(
    /* [int] */ Int32 requestedOrientation)
{
    if (mParent == NULL) {
        AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();

        ECode ec = defaultAM->SetRequestedOrientation(mToken, requestedOrientation);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            ec = NOERROR;
        }
        return ec;
    }
    else {
        return mParent->SetRequestedOrientation(requestedOrientation);
    }
}

ECode Activity::GetRequestedOrientation(
    /* [out] */ Int32* orientation)
{
    VALIDATE_NOT_NULL(orientation);
    *orientation = IActivityInfo::SCREEN_ORIENTATION_UNSPECIFIED;

    if (mParent == NULL) {
        AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();

        ECode ec = defaultAM->GetRequestedOrientation(mToken, orientation);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            ec = NOERROR;
        }
        return ec;
    }
    else {
        return mParent->GetRequestedOrientation(orientation);
    }

    return NOERROR;
}

ECode Activity::GetTaskId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = -1;

    AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();

    ECode ec = defaultAM->GetTaskForActivity(mToken, FALSE, id);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        ec = NOERROR;
    }
    return ec;
}

ECode Activity::IsTaskRoot(
    /* [out] */ Boolean* isRoot)
{
    VALIDATE_NOT_NULL(isRoot);
    *isRoot = FALSE;

    AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();

    Int32 task = -1;
    ECode ec = defaultAM->GetTaskForActivity(mToken, TRUE, &task);
    *isRoot = task >= 0;
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        *isRoot = FALSE;
        ec = NOERROR;
    }
    return ec;
}

ECode Activity::MoveTaskToBack(
    /* [in] */ Boolean nonRoot,
    /* [out] */ Boolean* succeed)
{
    VALIDATE_NOT_NULL(succeed);
    *succeed = FALSE;

    AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();
    ECode ec = defaultAM->MoveActivityTaskToBack(mToken, nonRoot, succeed);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        ec = NOERROR;
    }
    return ec;
}

ECode Activity::GetLocalClassName(
    /* [out] */ String* clsName)
{
    VALIDATE_NOT_NULL(clsName);

    String pkg, cls;
    FAIL_RETURN(GetPackageName(&pkg));
    FAIL_RETURN(mComponent->GetClassName(&cls));
    Int32 packageLen = pkg.GetLength();
    if (!cls.StartWith(pkg) || (Int32)cls.GetLength() <= packageLen
            || cls.GetChar(packageLen) != '.') {
        *clsName = cls;
        return NOERROR;
    }
    *clsName = cls.Substring(packageLen + 1);
    return NOERROR;
}

ECode Activity::GetComponentName(
    /* [out] */ IComponentName** name)
{
    VALIDATE_NOT_NULL(name);
    *name = mComponent;
    REFCOUNT_ADD(*name)
    return NOERROR;
}

ECode Activity::GetPreferences(
    /* [in] */ Int32 mode,
    /* [out] */ ISharedPreferences** perferences)
{
    VALIDATE_NOT_NULL(perferences);

    String clsName;
    GetLocalClassName(&clsName);
    return GetSharedPreferences(clsName, mode, perferences);
}

ECode Activity::EnsureSearchManager()
{
    if (mSearchManager == NULL) {
       return CSearchManager::New(this, NULL,
               (ISearchManager**)&mSearchManager);
    }

    return NOERROR;
}

ECode Activity::SetTitle(
    /* [in] */ ICharSequence* title)
{
    mTitle = title;
    FAIL_RETURN(OnTitleChanged(title, mTitleColor));

    if (mParent != NULL) {
        Activity* p = (Activity*)mParent.Get();
        return p->OnChildTitleChanged(this, title);
    }
    return NOERROR;
}

ECode Activity::SetTitle(
    /* [in] */ Int32 titleId)
{
    AutoPtr<ICharSequence> title;
    FAIL_RETURN(GetText(titleId, (ICharSequence**)&title));
    return SetTitle(title);
}

ECode Activity::SetTitleColor(
    /* [in] */ Int32 textColor)
{
    mTitleColor = textColor;
    OnTitleChanged(mTitle, textColor);
    return NOERROR;
}

ECode Activity::GetTitle(
    /* [out] */ ICharSequence** title)
{
    VALIDATE_NOT_NULL(title);
    *title = mTitle;
    REFCOUNT_ADD(*title)
    return NOERROR;
}

ECode Activity::GetTitleColor(
    /* [out] */ Int32* textColor)
{
    VALIDATE_NOT_NULL(textColor);
    *textColor = mTitleColor;
    return NOERROR;
}

ECode Activity::OnTitleChanged(
    /* [in] */ ICharSequence* title,
    /* [in] */ Int32 color)
{
    if (mTitleReady) {
        AutoPtr<IWindow> win = GetWindow();
        if (win != NULL) {
            FAIL_RETURN(win->SetTitle(title));
            if (color != 0) {
                FAIL_RETURN(win->SetTitleColor(color));
            }
        }

        if (mActionBar != NULL) {
            mActionBar->SetWindowTitle(title);
        }
    }
    return NOERROR;
}

ECode Activity::OnChildTitleChanged(
    /* [in] */ IActivity* childActivity,
    /* [in] */ ICharSequence* title)
{
    return NOERROR;
}

ECode Activity::SetTaskDescription(
    /* [in] */ IActivityManagerTaskDescription* taskDescription)
{
    AutoPtr<IActivityManagerTaskDescription> td;
    // Scale the icon down to something reasonable if it is provided
    String iconFilename;
    taskDescription->GetIconFilename(&iconFilename);
    AutoPtr<IBitmap> icon;
    taskDescription->GetIcon((IBitmap**)&icon);
    if (!iconFilename.IsNull() && icon != NULL) {
        Int32 size = CActivityManager::GetLauncherLargeIconSizeInner(this);
        AutoPtr<IBitmap> bmp;
        CBitmap::CreateScaledBitmap(icon, size, size, TRUE, (IBitmap**)&bmp);
        String label;
        taskDescription->GetLabel(&label);
        Int32 color;
        taskDescription->GetPrimaryColor(&color);
        CActivityManagerTaskDescription::New(label, icon, color, (IActivityManagerTaskDescription**)&td);
    } else {
        td = taskDescription;
    }

    // try {
    AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();
    return defaultAM->SetTaskDescription(mToken, td);
    // } catch (RemoteException e) {
    // }
}

ECode Activity::SetProgressBarVisibility(
    /* [in] */ Boolean visible)
{
    AutoPtr<IWindow> win = GetWindow();
    return win->SetFeatureInt(IWindow::FEATURE_PROGRESS,
            visible ? IWindow::PROGRESS_VISIBILITY_ON : IWindow::PROGRESS_VISIBILITY_OFF);
}

ECode Activity::SetProgressBarIndeterminateVisibility(
    /* [in] */ Boolean visible)
{
    AutoPtr<IWindow> win = GetWindow();
    return win->SetFeatureInt(IWindow::FEATURE_INDETERMINATE_PROGRESS,
            visible ? IWindow::PROGRESS_VISIBILITY_ON : IWindow::PROGRESS_VISIBILITY_OFF);

}

ECode Activity::SetProgressBarIndeterminate(
    /* [in] */ Boolean visible)
{
    AutoPtr<IWindow> win = GetWindow();
    return win->SetFeatureInt(IWindow::FEATURE_PROGRESS,
            visible ? IWindow::PROGRESS_INDETERMINATE_ON : IWindow::PROGRESS_INDETERMINATE_OFF);
}

ECode Activity::SetProgress(
    /* [in] */ Int32 progress)
{
    AutoPtr<IWindow> win = GetWindow();
    return win->SetFeatureInt(IWindow::FEATURE_PROGRESS,
            progress + IWindow::PROGRESS_START);
}

ECode Activity::SetSecondaryProgress(
    /* [in] */ Int32 secondaryProgress)
{
    AutoPtr<IWindow> win = GetWindow();
    return win->SetFeatureInt(IWindow::FEATURE_PROGRESS,
            secondaryProgress + IWindow::PROGRESS_SECONDARY_START);
}

ECode Activity::SetVolumeControlStream(
    /* [in] */ Int32 streamType)
{
    AutoPtr<IWindow> win = GetWindow();
    return win->SetVolumeControlStream(streamType);
}

ECode Activity::GetVolumeControlStream(
    /* [out] */ Int32* type)
{
    AutoPtr<IWindow> win = GetWindow();
    return win->GetVolumeControlStream(type);
}

ECode Activity::SetMediaController(
    /* [in] */ IMediaController* controller)
{
    AutoPtr<IWindow> win = GetWindow();
    return win->SetMediaController(controller);
}

ECode Activity::GetMediaController(
    /* [out] */ IMediaController** mc)
{
    AutoPtr<IWindow> win = GetWindow();
    return win->GetMediaController(mc);
}

ECode Activity::RunOnUiThread(
    /* [in] */ IRunnable* action)
{
    VALIDATE_NOT_NULL(action);

    if (Thread::GetCurrentThread() != mUiThread) {
        Boolean result;
        return mHandler->Post(action, &result);
    }
    else {
        return action->Run();
    }
}

ECode Activity::OnCreateView(
    /* [in] */ const String& name,
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = NULL;
    return NOERROR;
}

ECode Activity::OnCreateView(
    /* [in] */ IView* parent,
    /* [in] */ const String& name,
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = NULL;
    VALIDATE_NOT_NULL(attrs);

    if (!name.Equals("Fragment")) {
        return OnCreateView(name, context, attrs, view);
    }

    FragmentManagerImpl* fmi = (FragmentManagerImpl*)mFragments.Get();
    return fmi->OnCreateView(parent, name, context, attrs, view);
}

ECode Activity::Dump(
    /* [in] */ const String& prefix,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* writer,
    /* [in] */ ArrayOf<String>* args)
{
    return DumpInner(prefix, fd, writer, args);
}

ECode Activity::DumpInner(
    /* [in] */ const String& prefix,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* writer,
    /* [in] */ ArrayOf<String>* args)
{
    writer->Print(prefix); writer->Print(String("Local Activity "));
            //writer->Print(Integer.toHexString(System.identityHashCode(this)));
            writer->Println(String(" State:"));
    String innerPrefix = prefix + "  ";
    writer->Print(innerPrefix); writer->Print(String("mResumed="));
            writer->Print(mResumed); writer->Print(String(" mStopped="));
            writer->Print(mStopped); writer->Print(String(" mFinished="));
            writer->Println(mFinished);
    writer->Print(innerPrefix); writer->Print(String("mLoadersStarted="));
            writer->Println(mLoadersStarted);
    writer->Print(innerPrefix); writer->Print(String("mChangingConfigurations="));
            writer->Println(mChangingConfigurations);
    writer->Print(innerPrefix); writer->Print(String("mCurrentConfig="));
            String temp = Object::ToString(mCurrentConfig);
            writer->Println(temp);
    if (mLoaderManager != NULL) {
        writer->Print(prefix); writer->Print(String("Loader Manager "));
                //writer->print(Integer.toHexString(System.identityHashCode(mLoaderManager)));
                writer->Println(String(":"));
        String str(prefix);
        str += "  ";
        ILoaderManager::Probe(mLoaderManager)->Dump(str, fd, writer, args);
    }
    FAIL_RETURN(IFragmentManager::Probe(mFragments)->Dump(prefix, fd, writer, args));
    writer->Print(prefix); writer->Println(String("View Hierarchy:"));
    AutoPtr<IView> decorView;
    GetWindow()->GetDecorView((IView**)&decorView);
    DumpViewHierarchy(String(prefix + "  "), writer, decorView);
    return NOERROR;
}

ECode Activity::DumpViewHierarchy(
    /* [in] */ const String& prefix,
    /* [in] */ IPrintWriter* writer,
    /* [in] */ IView* view)
{
    writer->Print(prefix);
    if (view == NULL) {
        writer->Println(String("NULL"));
        return NOERROR;
    }

//    writer->println(view.toString());
    AutoPtr<IViewGroup> grp = IViewGroup::Probe(view);
    if (grp == NULL) {
        return NOERROR;
    }

    Int32 N;
    grp->GetChildCount(&N);
    if (N <= 0) {
        return NOERROR;
    }
    String prefixtemp = prefix + "  ";
    for (Int32 i=0; i<N; i++) {
        AutoPtr<IView> v;
        grp->GetChildAt(i, (IView**)&v);
        DumpViewHierarchy(prefixtemp, writer, v);
    }
    return NOERROR;
}

ECode Activity::IsImmersive(
    /* [out] */ Boolean* isImmersive)
{
    VALIDATE_NOT_NULL(isImmersive);
    *isImmersive = FALSE;
//    try {
    AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();
    ECode ec = defaultAM->IsImmersive(mToken, isImmersive);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        ec = NOERROR;
    }
    return ec;
//    } catch (RemoteException e) {
//        return false;
//    }
}

ECode Activity::ConvertFromTranslucent()
{
    // try {
    AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();
    mTranslucentCallback = NULL;
    Boolean bval;
    if (defaultAM->ConvertFromTranslucent(mToken, &bval), bval) {
        AutoPtr<IWindowManagerGlobal> wmg = CWindowManagerGlobal::GetInstance();
        return wmg->ChangeCanvasOpacity(mToken, TRUE);
    }

    return NOERROR;
    // } catch (RemoteException e) {
    //     // pass
    // }
}

ECode Activity::ConvertToTranslucent(
    /* [in] */ ITranslucentConversionListener* callback,
    /* [in] */ IActivityOptions* options,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean drawComplete;

    AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();
    mTranslucentCallback = callback;
    ECode ec = defaultAM->ConvertToTranslucent(mToken, options, &mChangeCanvasToTranslucent);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        // Make callback return as though it timed out.
        mChangeCanvasToTranslucent = FALSE;
        drawComplete = FALSE;
    }
    else {
        AutoPtr<IWindowManagerGlobal> wmg = CWindowManagerGlobal::GetInstance();
        wmg->ChangeCanvasOpacity(mToken, FALSE);
        drawComplete = TRUE;
    }

    if (!mChangeCanvasToTranslucent && mTranslucentCallback != NULL) {
        // Window is already translucent.
        mTranslucentCallback->OnTranslucentConversionComplete(drawComplete);
    }

    *result = mChangeCanvasToTranslucent;
    return NOERROR;
}

ECode Activity::OnTranslucentConversionComplete(
    /* [in] */ Boolean drawComplete)
{
    if (mTranslucentCallback != NULL) {
        mTranslucentCallback->OnTranslucentConversionComplete(drawComplete);
        mTranslucentCallback = NULL;
    }
    if (mChangeCanvasToTranslucent) {
        AutoPtr<IWindowManagerGlobal> wmg = CWindowManagerGlobal::GetInstance();
        wmg->ChangeCanvasOpacity(mToken, FALSE);
    }
    return NOERROR;
}

ECode Activity::OnNewActivityOptions(
    /* [in] */ IActivityOptions* options)
{
    mActivityTransitionState->SetEnterActivityOptions(this, options);
    if (!mStopped) {
        return mActivityTransitionState->EnterReady(this);
    }
    return NOERROR;
}

ECode Activity::GetActivityOptions(
    /* [out] */ IActivityOptions** options)
{
    VALIDATE_NOT_NULL(options)
    // try {
    AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();
    return defaultAM->GetActivityOptions(mToken, (IActivityOptions**)&options);
    // } catch (RemoteException e) {
    // }
}

ECode Activity::RequestVisibleBehind(
    /* [in] */ Boolean visible,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (!mResumed) {
        // Do not permit paused or stopped activities to do this.
        visible = false;
    }
    // try {
        AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();
        ECode ec = defaultAM->RequestVisibleBehind(mToken, visible, &mVisibleBehind);
        mVisibleBehind = mVisibleBehind && visible;
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            mVisibleBehind = FALSE;
            ec = NOERROR;
        }
    // } catch (RemoteException e) {
    //     mVisibleBehind = false;
    // }
    *result = mVisibleBehind;
    return ec;
}

ECode Activity::OnVisibleBehindCanceled()
{
    mCalled = TRUE;
    return NOERROR;
}

ECode Activity::IsBackgroundVisibleBehind(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();
    return defaultAM->IsBackgroundVisibleBehind(mToken, result);
    // } catch (RemoteException e) {
    // }
    // return false;
}

ECode Activity::OnBackgroundVisibleBehindChanged(
    /* [in] */ Boolean visible)
{
    return NOERROR;
}

ECode Activity::OnEnterAnimationComplete()
{
    return NOERROR;
}

ECode Activity::SetImmersive(
    /* [in] */ Boolean i)
{
//    try {
    AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();

    ECode ec = defaultAM->SetImmersive(mToken, i);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        ec = NOERROR;
    }
    return ec;
//    } catch (RemoteException e) {
//        // pass
//    }
}

ECode Activity::IsTopOfTask(
    /* [out] */ Boolean* top)
{
    VALIDATE_NOT_NULL(top)
    *top = FALSE;
    // try {
    AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();
    return defaultAM->IsTopOfTask(mToken, top);
    // } catch (RemoteException e) {
    //     return false;
    // }
}

ECode Activity::StartActionMode(
    /* [in] */ IActionModeCallback* callback,
    /* [out] */ IActionMode** mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = NULL;

    AutoPtr<IWindow> win = GetWindow();
    AutoPtr<IView> decor;
    FAIL_RETURN(win->GetDecorView((IView**)&decor));
    return decor->StartActionMode(callback, mode);
}

ECode Activity::OnWindowStartingActionMode(
    /* [in] */ IActionModeCallback* callback,
    /* [out] */ IActionMode** mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = NULL;

    FAIL_RETURN(InitWindowDecorActionBar());
    if (mActionBar) {
        return mActionBar->StartActionMode(callback, mode);
    }
    return NOERROR;
}

ECode Activity::OnActionModeStarted(
    /* [in] */ IActionMode* mode)
{
    return NOERROR;
}

ECode Activity::OnActionModeFinished(
    /* [in] */ IActionMode* mode)
{
    return NOERROR;
}

ECode Activity::ShouldUpRecreateTask(
    /* [in] */ IIntent* targetIntent,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(targetIntent);

//    try {
    AutoPtr<IPackageManager> pm;
    AutoPtr<IComponentName> cn;
    FAIL_RETURN(GetPackageManager((IPackageManager**)&pm));
    FAIL_RETURN(targetIntent->GetComponent((IComponentName**)&cn));
    if (cn == NULL) {
        FAIL_RETURN(targetIntent->ResolveActivity(pm, (IComponentName**)&cn));
    }

    AutoPtr<IActivityInfo> info;
    FAIL_RETURN(pm->GetActivityInfo(cn, 0, (IActivityInfo**)&info));
    String affinity;
    FAIL_RETURN(info->GetTaskAffinity(&affinity));
    if (affinity.IsNull()) {
        return NOERROR;
    }

    AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();

    Boolean val;
    ECode ec = defaultAM->ShouldUpRecreateTask(mToken, affinity, &val);
    *result = !val;
    if (ec == (ECode)E_REMOTE_EXCEPTION || ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
        *result = FALSE;
        ec = NOERROR;
    }
    return NOERROR;
//    } catch (RemoteException e) {
//        return false;
//    } catch (NameNotFoundException e) {
//        return false;
//    }
}

ECode Activity::NavigateUpTo(
    /* [in] */ IIntent* inUpIntent,
    /* [out] */ Boolean* success)
{
    VALIDATE_NOT_NULL(success);
    *success = FALSE;
    VALIDATE_NOT_NULL(inUpIntent);

    if (mParent == NULL) {
        AutoPtr<IIntent> upIntent = inUpIntent;

        AutoPtr<IComponentName> destInfo;
        upIntent->GetComponent((IComponentName**)&destInfo);
        if (destInfo == NULL) {
            AutoPtr<IPackageManager> pm;
            FAIL_RETURN(GetPackageManager((IPackageManager**)&pm));
            FAIL_RETURN(upIntent->ResolveActivity(pm, (IComponentName**)&destInfo));
            if (destInfo == NULL) {
                return NOERROR;
            }

            upIntent = NULL;
            CIntent::New(inUpIntent, (IIntent**)&upIntent);
            upIntent->SetComponent(destInfo);
            inUpIntent = upIntent;
        }
        Int32 resultCode;
        AutoPtr<IIntent> resultData;
        {
            AutoLock lock(this);
            resultCode = mResultCode;
            resultData = mResultData;
        }
        if (resultData != NULL) {
            resultData->PrepareToLeaveProcess();
        }
//        try {
        upIntent->PrepareToLeaveProcess();
        AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();
        return defaultAM->NavigateUpTo(mToken, inUpIntent, resultCode, resultData, success);
//        } catch (RemoteException e) {
//            return false;
//        }
    }
    else {
        return mParent->NavigateUpToFromChild(this, inUpIntent, success);
    }
}

ECode Activity::NavigateUpToFromChild(
    /* [in] */ IActivity* child,
    /* [in] */ IIntent* upIntent,
    /* [out] */ Boolean* success)
{
    return NavigateUpTo(upIntent, success);
}

ECode Activity::GetParentActivityIntent(
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent);
    *intent = NULL;

    String parentName;
    FAIL_RETURN(mActivityInfo->GetParentActivityName(&parentName));

    AutoPtr<ICharSequence> seq;
    FAIL_RETURN(CString::New(parentName, (ICharSequence**)&seq));

    if (TextUtils::IsEmpty(seq)) {
        return NOERROR;
    }

    // If the parent itself has no parent, generate a main activity intent.
    AutoPtr<IComponentName> target;
    CComponentName::New(this, parentName, (IComponentName**)&target);
//    try {
    AutoPtr<IPackageManager> pm;
    FAIL_RETURN(GetPackageManager((IPackageManager**)&pm));
    AutoPtr<IActivityInfo> parentInfo;
    FAIL_RETURN(pm->GetActivityInfo(target, 0, (IActivityInfo**)&parentInfo));
    String parentActivity;
    FAIL_RETURN(parentInfo->GetParentActivityName(&parentActivity));

    AutoPtr<IIntent> parentIntent;
    if (parentActivity.IsNull()) {
        AutoPtr<IIntentHelper> helper;
        CIntentHelper::AcquireSingleton((IIntentHelper**)&helper);
        FAIL_RETURN(helper->MakeMainActivity(target, (IIntent**)&parentIntent));
    }
    else {
        AutoPtr<IIntent> obj;
        CIntent::New((IIntent**)&obj);
        FAIL_RETURN(obj->SetComponent(target));
        parentIntent = obj;
    }

    *intent = parentIntent;
    REFCOUNT_ADD(*intent)
    return NOERROR;
//    } catch (NameNotFoundException e) {
//        Log.e(TAG, "getParentActivityIntent: bad parentActivityName '" + parentName +
//                "' in manifest");
//        return NULL;
//    }
}

ECode Activity::SetEnterSharedElementCallback(
    /* [in] */ ISharedElementCallback* callback)
{
    AutoPtr<ISharedElementCallback> cb = callback;
    if (cb == NULL) {
        cb = SharedElementCallback::NULL_CALLBACK;
    }
    mEnterTransitionListener = cb;
    return NOERROR;
}

ECode Activity::SetExitSharedElementCallback(
    /* [in] */ ISharedElementCallback* callback)
{
    AutoPtr<ISharedElementCallback> cb = callback;
    if (cb == NULL) {
        cb = SharedElementCallback::NULL_CALLBACK;
    }
    mExitTransitionListener = cb;
    return NOERROR;
}

ECode Activity::PostponeEnterTransition()
{
    return mActivityTransitionState->PostponeEnterTransition();
}

ECode Activity::StartPostponedEnterTransition()
{
    return mActivityTransitionState->StartPostponedEnterTransition();
}

ECode Activity::SetParent(
    /* [in] */ IActivity* parent)
{
    mParent = parent;
    return NOERROR;
}

ECode Activity::Attach(
    /* [in] */ IContext* context,
    /* [in] */ IActivityThread* aThread,
    /* [in] */ IInstrumentation* instr,
    /* [in] */ IBinder* token,
    /* [in] */ Int32 ident,
    /* [in] */ IApplication* application,
    /* [in] */ IIntent* intent,
    /* [in] */ IActivityInfo* info,
    /* [in] */ ICharSequence* title,
    /* [in] */ IActivity* parent,
    /* [in] */ const String& id,
    /* [in] */ IActivityNonConfigurationInstances* lastNonConfigurationInstances,
    /* [in] */ IConfiguration* config,
    /* [in] */ IIVoiceInteractor* voiceInteractor)
{
    Slogger::I(TAG, " >>> Activity::Attach");
    FAIL_RETURN(AttachBaseContext(context));
Slogger::I(TAG, "    Attach 1 ");
    FAIL_RETURN(mFragments->AttachActivity(this, mContainer, NULL));
Slogger::I(TAG, "    Attach 2 ");
    AutoPtr<IPolicyManager> pm;
    CPolicyManager::AcquireSingleton((IPolicyManager**)&pm);
    mWindow = NULL;
    FAIL_RETURN(pm->MakeNewWindow(this, (IWindow**)&mWindow));
    FAIL_RETURN(mWindow->SetCallback(this));
    FAIL_RETURN(mWindow->SetOnWindowDismissedCallback(this))
    AutoPtr<ILayoutInflater> inflater;
    mWindow->GetLayoutInflater((ILayoutInflater**)&inflater);
    inflater->SetPrivateFactory(this);

    Int32 softInputMode = 0;
    FAIL_RETURN(info->GetSoftInputMode(&softInputMode));
    if (softInputMode != IWindowManagerLayoutParams::SOFT_INPUT_STATE_UNSPECIFIED) {
        FAIL_RETURN(mWindow->SetSoftInputMode(softInputMode));
    }
    Int32 uiOptions = 0;
    info->GetUiOptions(&uiOptions);
    if (uiOptions != 0) {
        FAIL_RETURN(mWindow->SetUiOptions(uiOptions));
    }
Slogger::I(TAG, "    Attach 3 ");
    mUiThread = Thread::GetCurrentThread();

    mMainThread = aThread;
    mInstrumentation = instr;
    mToken = token;
    mIdent = ident;
    mApplication = application;
    mIntent = intent;
    mComponent = NULL;
    intent->GetComponent((IComponentName**)&mComponent);
    mActivityInfo = info;
    mTitle = title;
    mParent = parent;
    mEmbeddedID = id;
    mLastNonConfigurationInstances = lastNonConfigurationInstances;

    if (voiceInteractor != NULL) {
        mVoiceInteractor = NULL;
        if (mLastNonConfigurationInstances != NULL) {
            mLastNonConfigurationInstances->GetVoiceInteractor((IVoiceInteractor**)&mVoiceInteractor);
        }
        else {
            AutoPtr<ILooper> looper = Looper::GetMyLooper();
            CVoiceInteractor::New(voiceInteractor, this, this,
                looper, (IVoiceInteractor**)&mVoiceInteractor);
        }
    }
Slogger::I(TAG, "    Attach 4 ");
    AutoPtr<IInterface> service;
    FAIL_RETURN(context->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&service))
    AutoPtr<IWindowManager> wm = IWindowManager::Probe(service);
    String str;
    mComponent->FlattenToString(&str);
    Int32 flags;
    info->GetFlags(&flags);
    FAIL_RETURN(mWindow->SetWindowManager(wm, mToken, str,
            (flags & IActivityInfo::FLAG_HARDWARE_ACCELERATED) != 0));
Slogger::I(TAG, "    Attach 5 ");
    if (mParent != NULL) {
        AutoPtr<IWindow> pWindow;
        FAIL_RETURN(mParent->GetWindow((IWindow**)&pWindow));
        FAIL_RETURN(mWindow->SetContainer(pWindow));
    }
Slogger::I(TAG, "    Attach 6 ");
    mWindowManager = NULL;
    FAIL_RETURN(mWindow->GetWindowManager((IWindowManager**)&mWindowManager));
    mCurrentConfig = config;

    Slogger::I(TAG, " <<< Activity::Attach");
    return NOERROR;
}

ECode Activity::GetActivityToken(
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder);

    if (mParent != NULL) {
        return mParent->GetActivityToken(binder);
    }

    *binder = mToken.Get();
    REFCOUNT_ADD(*binder)
    return NOERROR;
}

ECode Activity::Activity::PerformCreateCommon()
{
    AutoPtr<ITypedArray> winStyle;
    FAIL_RETURN(mWindow->GetWindowStyle((ITypedArray**)&winStyle));
    Boolean noDisplay = FALSE;
    winStyle->GetBoolean(R::styleable::Window_windowNoDisplay, FALSE, &noDisplay);
    mVisibleFromClient = !noDisplay;
    FAIL_RETURN(mFragments->DispatchActivityCreated());
    AutoPtr<IActivityOptions> options;
    GetActivityOptions((IActivityOptions**)&options);
    return mActivityTransitionState->SetEnterActivityOptions(this, options);
}

ECode Activity::PerformCreate(
    /* [in] */ IBundle* icicle)
{
    FAIL_RETURN(OnCreate(icicle))
    mActivityTransitionState->ReadState(icicle);
    return PerformCreateCommon();
}

ECode Activity::PerformCreate(
    /* [in] */ IBundle* icicle,
    /* [in] */ IPersistableBundle* bundle)
{
    FAIL_RETURN(OnCreate(icicle, bundle))
    mActivityTransitionState->ReadState(icicle);
    return PerformCreateCommon();
}

ECode Activity::PerformStart()
{
    AutoPtr<IActivityOptions> options;
    GetActivityOptions((IActivityOptions**)&options);
    FAIL_RETURN(mActivityTransitionState->SetEnterActivityOptions(this, options))

    FAIL_RETURN(mFragments->NoteStateNotSaved());
    mCalled = FALSE;
    Boolean exected;
    FAIL_RETURN(mFragments->ExecPendingActions(&exected));
    FAIL_RETURN(mInstrumentation->CallActivityOnStart(this));
    if (!mCalled) {
        String temp;
        mComponent->ToShortString(&temp);
        Logger::E(TAG, "Activity %s did not call through to super.onStart()", temp.string());
//        throw new SuperNotCalledException(
//            "Activity " + mComponent.toShortString() +
//            " did not call through to super.onStart()");
        return E_SUPER_NOT_CALLED_EXCEPTION;
    }

    FAIL_RETURN(mFragments->DispatchStart());
    if (mAllLoaderManagers != NULL) {
        Int32 N;
        mAllLoaderManagers->GetSize(&N);
        AutoPtr<ArrayOf<ILoaderManagerImpl*> > loaders = ArrayOf<ILoaderManagerImpl*>::Alloc(N);
        for (Int32 i = N - 1; i >= 0; i--) {
            AutoPtr<IInterface> obj;
            mAllLoaderManagers->GetValueAt(i, (IInterface**)&obj);
            loaders->Set(i, ILoaderManagerImpl::Probe(obj));
        }
        for (Int32 i = 0; i < N; i++) {
            assert(0 && "TODO");
            // LoaderManagerImpl* lm = (LoaderManagerImpl*)(*loaders)[i];
            // lm->FinishRetain();
            // lm->DoReportStart();
        }
    }

    return mActivityTransitionState->EnterReady(this);
}

ECode Activity::PerformRestart()
{
    FAIL_RETURN(mFragments->NoteStateNotSaved());

    if (mStopped) {
        mStopped = FALSE;

        if (mToken != NULL && mParent == NULL) {
            FAIL_RETURN(CWindowManagerGlobal::GetInstance()->SetStoppedState(mToken, FALSE));
        }

        {
            AutoLock lock(mManagedCursorsLock);

            Int32 targetSdkVersion;
            AutoPtr<IApplicationInfo> appInfo;
            FAIL_RETURN(GetApplicationInfo((IApplicationInfo**)&appInfo));
            FAIL_RETURN(appInfo->GetTargetSdkVersion(&targetSdkVersion));

            AutoPtr<ManagedCursor> mc;
            Boolean bval;
            List<AutoPtr<ManagedCursor> >::Iterator it = mManagedCursors.Begin();
            for (; it != mManagedCursors.End(); ++it) {
                mc = *it;
                if (mc->mReleased || mc->mUpdated) {
                    mc->mCursor->Requery(&bval);
                    if (!bval) {
                        if (targetSdkVersion >= Build::VERSION_CODES::ICE_CREAM_SANDWICH) {
                            Slogger::E(TAG, "trying to requery an already closed cursor.");
                            return E_ILLEGAL_STATE_EXCEPTION;
                        }
                    }
                    mc->mReleased = FALSE;
                    mc->mUpdated = FALSE;
                }
            }
        }

        mCalled = FALSE;
        FAIL_RETURN(mInstrumentation->CallActivityOnRestart(this));
        if (!mCalled) {
//            throw new SuperNotCalledException(
//                "Activity " + mComponent.toShortString() +
//                " did not call through to super.onRestart()");
            String temp;
            mComponent->ToShortString(&temp);
            Logger::E(TAG, "Activity %s did not call through to super.onStart()", temp.string());
            return E_SUPER_NOT_CALLED_EXCEPTION;
        }
        FAIL_RETURN(PerformStart());
    }
    return NOERROR;
}

ECode Activity::PerformResume()
{
    FAIL_RETURN(PerformRestart());

    Boolean exected;
    FAIL_RETURN(mFragments->ExecPendingActions(&exected));

    mLastNonConfigurationInstances = NULL;

    mCalled = FALSE;
    // mResumed is set by the instrumentation
    FAIL_RETURN(mInstrumentation->CallActivityOnResume(this));
    if (!mCalled) {
//        throw new SuperNotCalledException(
//            "Activity " + mComponent.toShortString() +
//            " did not call through to super.onResume()");

        String temp;
        mComponent->ToShortString(&temp);
        Logger::E(TAG, "Activity %s did not call through to super.onResume()", temp.string());
        return E_SUPER_NOT_CALLED_EXCEPTION;
    }

    mCalled = FALSE;

    FAIL_RETURN(mFragments->DispatchResume());
    Boolean execresult;
    FAIL_RETURN(mFragments->ExecPendingActions(&execresult));
    FAIL_RETURN(OnPostResume());
    if (!mCalled) {
//        throw new SuperNotCalledException(
//            "Activity " + mComponent.toShortString() +
//            " did not call through to super.onPostResume()");
        StringBuilder sb("Activity ");
        String temp;
        mComponent->ToShortString(&temp);
        sb += temp;
        sb += String(" did not call through to super.onPostResume()");
        Logger::E(TAG, sb.ToString().string());
        return E_SUPER_NOT_CALLED_EXCEPTION;
    }
    return NOERROR;
}

ECode Activity::PerformPause()
{
    mDoReportFullyDrawn = FALSE;
    FAIL_RETURN(mFragments->DispatchPause());
    mCalled = FALSE;
    FAIL_RETURN(OnPause());
    mResumed = FALSE;
    Int32 targetSdkVersion;
    AutoPtr<IApplicationInfo> appInfo;
    FAIL_RETURN(GetApplicationInfo((IApplicationInfo**)&appInfo));
    FAIL_RETURN(appInfo->GetTargetSdkVersion(&targetSdkVersion));

    if (!mCalled && targetSdkVersion >= Build::VERSION_CODES::GINGERBREAD) {
//        throw new SuperNotCalledException(
//                "Activity " + mComponent.toShortString() +
//                " did not call through to super.onPause()");
        StringBuilder sb("Activity ");
        String temp;
        mComponent->ToShortString(&temp);
        sb += temp;
        sb += String(" did not call through to super.onPause()");
        Logger::E(TAG, sb.ToString().string());
        return E_SUPER_NOT_CALLED_EXCEPTION;
    }
    mResumed = FALSE;
    return NOERROR;
}

ECode Activity::PerformUserLeaving()
{
    FAIL_RETURN(OnUserInteraction());
    return OnUserLeaveHint();
}

ECode Activity::PerformStop()
{
    mDoReportFullyDrawn = FALSE;
    if (mLoadersStarted) {
        mLoadersStarted = FALSE;
        if (mLoaderManager != NULL) {
            assert(0 && "TODO");
            // LoaderManagerImpl* lmi = (LoaderManagerImpl*)mLoaderManager.Get();
            // if (!mChangingConfigurations) {
            //     lmi->DoStop();
            // } else {
            //     lmi->DoRetain();
            // }
        }
    }

    if (!mStopped) {
        if (mWindow != NULL) {
            mWindow->CloseAllPanels();
        }
        if (mToken != NULL && mParent == NULL) {
            FAIL_RETURN(CWindowManagerGlobal::GetInstance()->SetStoppedState(mToken, TRUE));
        }

        FAIL_RETURN(mFragments->DispatchStop());

        mCalled = FALSE;
        FAIL_RETURN(mInstrumentation->CallActivityOnStop(this));
        if (!mCalled) {
//            throw new SuperNotCalledException(
//                "Activity " + mComponent.toShortString() +
//                " did not call through to super.onStop()");
            StringBuilder sb("Activity ");
            String temp;
            mComponent->ToShortString(&temp);
            sb += temp;
            sb += String(" did not call through to super.onStop()");
            Logger::E(TAG, sb.ToString().string());
            return E_SUPER_NOT_CALLED_EXCEPTION;
        }

        {
            AutoLock lock(mManagedCursorsLock);
            AutoPtr<ManagedCursor> mc;
            List<AutoPtr<ManagedCursor> >::Iterator it = mManagedCursors.Begin();
            for (; it != mManagedCursors.End(); ++it) {
                mc = *it;
                if (!mc->mReleased) {
                    mc->mCursor->Deactivate();
                    mc->mReleased = TRUE;
                }
            }
        }

        mStopped = TRUE;
    }
    mResumed = FALSE;
    return NOERROR;
}

ECode Activity::PerformDestroy()
{
    mDestroyed = true;
    mWindow->Destroy();

    FragmentManagerImpl* fmi = (FragmentManagerImpl*)mFragments.Get();
    FAIL_RETURN(fmi->DispatchDestroy());
    OnDestroy();
    if (mLoaderManager != NULL) {
        assert(0 && "TODO");
        //LoaderManagerImpl* lmi = (LoaderManagerImpl*)mLoaderManager.Get();
        //lmi->DoDestroy();
    }
    if (mVoiceInteractor != NULL) {
        VoiceInteractor* vi = (VoiceInteractor*)mVoiceInteractor.Get();
        vi->DetachActivity();
    }
    return NOERROR;
}

ECode Activity::IsResumed(
    /* [out] */ Boolean* isResumed)
{
    VALIDATE_NOT_NULL(isResumed);
    *isResumed = mResumed;
    return NOERROR;
}

ECode Activity::DispatchActivityResult(
    /* [in] */ const String& who,
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    if (FALSE) {
        Logger::V(TAG, "Dispatching result: who=%s, reqCode=%d, resCode=%d, data=%p", who.string(), requestCode,
                resultCode, data);
    }

    FAIL_RETURN(mFragments->NoteStateNotSaved());
    if (who.IsNull()) {
        return OnActivityResult(requestCode, resultCode, data);
    }
    else {
        AutoPtr<IFragment> frag;
        FAIL_RETURN(mFragments->FindFragmentByWho(who, (IFragment**)&frag));
        if (frag != NULL) {
            return frag->OnActivityResult(requestCode, resultCode, data);
        }
    }
    return NOERROR;
}

ECode Activity::StartLockTask()
{
    // try {
    AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();
    return defaultAM->StartLockTaskMode(mToken);
    // } catch (RemoteException e) {
    // }
}

ECode Activity::StopLockTask()
{
    // try {
    AutoPtr<IIActivityManager> defaultAM = ActivityManagerNative::GetDefault();
    return defaultAM->StopLockTaskMode();
    // } catch (RemoteException e) {
    // }
}

ECode Activity::SetStartedActivity(
    /* [in] */ Boolean started)
{
    mStartedActivity = started;
    return NOERROR;
}

ECode Activity::DispatchNewIntent(
    /* [in] */ IIntent *intent)
{
    return OnNewIntent(intent);
}

ECode Activity::SetCalled(
    /* [in] */ Boolean called)
{
    mCalled = called;
    return NOERROR;
}

ECode Activity::IsCalled(
    /* [out] */ Boolean* called)
{
    VALIDATE_NOT_NULL(called);
    *called = mCalled;
    return NOERROR;
}

ECode Activity::SetFinishing(
    /* [in] */ Boolean finished)
{
    mFinished = finished;
    return NOERROR;
}


ECode Activity::SetConfigChangeFlags(
    /* [in] */ Int32 configChangeFlags)
{
    mConfigChangeFlags = configChangeFlags;
    return NOERROR;
}

ECode Activity::GetConfigChangeFlags(
    /* [out] */ Int32* configChangeFlags)
{
    VALIDATE_NOT_NULL(configChangeFlags);
    *configChangeFlags = mConfigChangeFlags;
    return NOERROR;
}

ECode Activity::InitializeTheme()
{
    Boolean first = mTheme == NULL;
    if (first) {
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        resources->NewTheme((IResourcesTheme**)&mTheme);
        AutoPtr<IResourcesTheme> theme;
        ContextThemeWrapper::GetTheme((IResourcesTheme**)&theme);
        if (theme != NULL) {
            mTheme->SetTo(theme);
        }
    }
    return OnApplyThemeResource(mTheme, mThemeResource, first);
}

ECode Activity::SetResumed(
    /* [in] */ Boolean resumed)
{
    mResumed = resumed;
    return NOERROR;
}

ECode Activity::GetID(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mEmbeddedID;
    return NOERROR;
}

ECode Activity::GetToken(
    /* [out] */ IBinder** token)
{
    VALIDATE_NOT_NULL(token);
    *token = mToken;
    REFCOUNT_ADD(*token)
    return NOERROR;
}

// internal use.
ECode Activity::IsStartedActivity(
    /* [out] */ Boolean* isStartedActivity)
{
    VALIDATE_NOT_NULL(isStartedActivity);
    *isStartedActivity = mStartedActivity;
    return NOERROR;
}

ECode Activity::SetDecorView(
    /* [in] */ IView* decor)
{
    mDecor = decor;
    return NOERROR;
}

ECode Activity::GetDecorView(
    /* [out] */ IView** decor)
{
    VALIDATE_NOT_NULL(decor);
    *decor = mDecor.Get();
    REFCOUNT_ADD(*decor)
    return NOERROR;
}

ECode Activity::SetVisibleFromServer(
    /* [in] */ Boolean visible)
{
    mVisibleFromServer = visible;
    return NOERROR;
}

ECode Activity::IsVisibleFromServer(
    /* [out] */ Boolean * visible)
{
    VALIDATE_NOT_NULL(visible);
    *visible = mVisibleFromServer;
    return NOERROR;
}

ECode Activity::IsVisibleFromClient(
    /* [out] */ Boolean* visible)
{
    VALIDATE_NOT_NULL(visible);
    *visible = mVisibleFromClient;
    return NOERROR;
}

ECode Activity::SetWindowAdded(
    /* [in] */ Boolean added)
{
    mWindowAdded = added;
    return NOERROR;
}

ECode Activity::IsWindowAdded(
    /* [out] */ Boolean* added)
{
    VALIDATE_NOT_NULL(added);
    *added = mWindowAdded;
    return NOERROR;
}

EXTERN_C ECode GrafixInit();

ECode Activity::Create(
    /* [in] */ IBundle* savedInstanceState)
{
    return OnCreate(savedInstanceState);
}

ECode Activity::PostCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    return OnPostCreate(savedInstanceState);
}

ECode Activity::Start()
{
    return OnStart();
}

ECode Activity::Restart()
{
    return OnRestart();
}

ECode Activity::Resume()
{
    return OnResume();
}

ECode Activity::Pause()
{
    return OnPause();
}

ECode Activity::Stop()
{
    return OnStop();
}

ECode Activity::Destroy()
{
    return OnDestroy();
}

//    @Override
ECode Activity::StartActivity(
    /* [in] */ IIntent *intent)
{
    return StartActivity(intent, NULL);
}

//    @Override
ECode Activity::StartActivity(
    /* [in] */ IIntent* intent,
    /* [in] */ IBundle* options)
{
     if (options != NULL) {
         return StartActivityForResult(intent, -1, options);
     }
     else {
         // Note we want to go through this call for compatibility with
         // applications that may have overridden the method.
         return StartActivityForResult(intent, -1);
     }
}

//    @Override
ECode Activity::StartActivityAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user)
{
    return StartActivityAsUser(intent, NULL, user);
}

//    @Override
ECode Activity::StartActivityAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IBundle* options,
    /* [in] */ IUserHandle* user)
{
    if (mParent != NULL) {
        return E_RUNTIME_EXCEPTION;
    }
    AutoPtr<IInstrumentationActivityResult> result;
    AutoPtr<IApplicationThread> at;
    mMainThread->GetApplicationThread((IApplicationThread**)&at);
    FAIL_RETURN(mInstrumentation->ExecStartActivity(
            this, IBinder::Probe(at), mToken, this,
            intent, -1, options, user,
            (IInstrumentationActivityResult**)&result));
    if (result != NULL) {
        Int32 resultCode;
        AutoPtr<IIntent> resultData;
        result->GetResultCode(&resultCode);
        result->GetResultData((IIntent**)&resultData);
        FAIL_RETURN(mMainThread->SendActivityResult(
                mToken, mEmbeddedID, -1, resultCode, resultData));
    }
    return NOERROR;
}

ECode Activity::StartActivityAsCaller(
    /* [in] */ IIntent* intent,
    /* [in] */ IBundle* options,
    /* [in] */ Int32 userId)
{
    if (mParent != NULL) {
        Logger::E(TAG, "Can't be called from a child");
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<IInstrumentationActivityResult> result;
    AutoPtr<IApplicationThread> at;
    mMainThread->GetApplicationThread((IApplicationThread**)&at);

    FAIL_RETURN(mInstrumentation->ExecStartActivityAsCaller(
        this, IBinder::Probe(at), mToken, this,
        intent, -1, options, userId, (IInstrumentationActivityResult**)&result));
    if (result != NULL) {
        Int32 resultCode;
        AutoPtr<IIntent> resultData;
        result->GetResultCode(&resultCode);
        result->GetResultData((IIntent**)&resultData);
        FAIL_RETURN(mMainThread->SendActivityResult(
                mToken, mEmbeddedID, -1, resultCode, resultData));
    }
    return NOERROR;
}

//    @Override
ECode Activity::StartActivities(
    /* [in] */ ArrayOf<IIntent*>* intents)
{
    return StartActivities(intents, NULL);
}

//    @Override
ECode Activity::StartActivities(
    /* [in] */ ArrayOf<IIntent*>* intents,
    /* [in] */ IBundle* options)
{
    AutoPtr<IApplicationThread> at;
    mMainThread->GetApplicationThread((IApplicationThread**)&at);
    return mInstrumentation->ExecStartActivities(
            this, IBinder::Probe(at.Get()), mToken, this,
            intents, options);
}

//    @Override
ECode Activity::StartActivitiesAsUser(
    /* [in] */ ArrayOf<IIntent*>* intents,
    /* [in] */ IBundle* options,
    /* [in] */ IUserHandle* userHandle)
{
    if (mParent != NULL) {
        return E_RUNTIME_EXCEPTION;
    }

    Int32 myUserId = CUserHandle::GetMyUserId();

    AutoPtr<IApplicationThread> at;
    mMainThread->GetApplicationThread((IApplicationThread**)&at);
    FAIL_RETURN(mInstrumentation->ExecStartActivitiesAsUser(
            this, IBinder::Probe(at.Get()), mToken, this,
            intents, options, myUserId));
    return NOERROR;
}

//    @Override
ECode Activity::StartIntentSender(
    /* [in] */ IIntentSender* intent,
    /* [in] */ IIntent* fillInIntent,
    /* [in] */ Int32 flagsMask,
    /* [in] */ Int32 flagsValues,
    /* [in] */ Int32 extraFlags)
{
    return StartIntentSender(intent, fillInIntent,
            flagsMask, flagsValues, extraFlags, NULL);
}

//    @Override
ECode Activity::StartIntentSender(
    /* [in] */ IIntentSender* intent,
    /* [in] */ IIntent* fillInIntent,
    /* [in] */ Int32 flagsMask,
    /* [in] */ Int32 flagsValues,
    /* [in] */ Int32 extraFlags,
    /* [in] */ IBundle* options)
{
    if (options != NULL) {
        return StartIntentSenderForResult(intent, -1, fillInIntent, flagsMask,
                flagsValues, extraFlags, options);
    }
    else {
        // Note we want to go through this call for compatibility with
        // applications that may have overridden the method.
        return StartIntentSenderForResult(intent, -1, fillInIntent, flagsMask,
                flagsValues, extraFlags);
    }
}

//    @Override
ECode Activity::GetSystemService(
    /* [in] */ const String& name,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    *object = NULL;

    AutoPtr<IContext> ctx;
    GetBaseContext((IContext**)&ctx);
    if (ctx == NULL) {
        Slogger::E(TAG, "System services not available to Activities before onCreate().");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (IContext::WINDOW_SERVICE.Equals(name)) {
        *object = mWindowManager;
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::SEARCH_SERVICE.Equals(name)) {
        EnsureSearchManager();
        *object = mSearchManager;
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    // else if (IContext::LAYOUT_INFLATER_SERVICE.Equals(name)) {
    //     if (mInflater == NULL) {
    //         AutoPtr<IInterface> service;
    //         FAIL_RETURN(ContextThemeWrapper::GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&service))
    //         AutoPtr<ILayoutInflater> temp = ILayoutInflater::Probe(service);
    //         temp->CloneInContext(this, (ILayoutInflater**)&mInflater);
    //     }
    //     *object = mInflater;
    //     REFCOUNT_ADD(*object);
    //     return NOERROR;
    // }

    return ContextThemeWrapper::GetSystemService(name, object);
}

} // namespace App
} // namespace Droid
} // namespace Elastos
