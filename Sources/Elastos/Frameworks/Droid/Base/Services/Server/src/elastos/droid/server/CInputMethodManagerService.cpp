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

#include "elastos/droid/server/CInputMethodManagerService.h"
#include "elastos/droid/server/SystemConfig.h"
#include "elastos/droid/server/CInputMethodManagerServiceMethodCallback.h"
#include "elastos/droid/server/CInputMethodManagerServiceUserSwitchObserver.h"
#include "elastos/droid/server/wm/CWindowManagerService.h"
#include <elastos/droid/R.h>
#include <elastos/droid/Manifest.h>
#include <elastos/droid/app/AppGlobals.h>
#include <elastos/droid/app/ActivityManagerNative.h>
#include <elastos/droid/os/ServiceManager.h>
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/droid/os/ServiceManager.h>
#include <elastos/droid/provider/Settings.h>
#include <elastos/droid/internal/os/SomeArgs.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/droid/utility/Xml.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Database.h>
#include <Elastos.Droid.InputMethodService.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Internal.h>
#include <Elastos.Droid.Utility.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::R;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::CNotification;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Database::EIID_IContentObserver;
using Elastos::Droid::InputMethodService::IInputMethodService;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::IUserManagerHelper;
using Elastos::Droid::Os::CUserManagerHelper;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CBinder;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::EIID_IHandlerCallback;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::IInputChannelHelper;
using Elastos::Droid::View::CInputChannelHelper;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::View::CWindowManagerLayoutParamsHelper;
using Elastos::Droid::View::IWindowManagerLayoutParamsHelper;
using Elastos::Droid::View::InputMethod::IInputMethod;
using Elastos::Droid::View::InputMethod::CInputBinding;
using Elastos::Droid::View::InputMethod::CInputMethodInfo;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::View::InputMethod::CInputMethodSubtype;
using Elastos::Droid::View::InputMethod::IInputMethodSubtypeBuilder;
using Elastos::Droid::View::InputMethod::CInputMethodSubtypeBuilder;
using Elastos::Droid::View::InputMethod::IInputMethodSubtypeHelper;
using Elastos::Droid::View::InputMethod::CInputMethodSubtypeHelper;
using Elastos::Droid::Internal::Os::SomeArgs;
using Elastos::Droid::Internal::Os::ISomeArgs;
using Elastos::Droid::Internal::Os::CHandlerCaller;
using Elastos::Droid::Internal::Os::EIID_IHandlerCallerCallback;
using Elastos::Droid::Internal::View::CInputBindResult;
using Elastos::Droid::Internal::View::EIID_IIInputMethodManager;
using Elastos::Droid::Internal::Utility::CFastXmlSerializer;
using Elastos::Droid::Internal::Utility::IFastXmlSerializer;
using Elastos::Droid::Internal::InputMethod::CInputMethodSettings;
using Elastos::Droid::Internal::InputMethod::CInputMethodUtils;
using Elastos::Droid::Internal::InputMethod::IInputMethodSubtypeSwitchingControllerHelper;
using Elastos::Droid::Internal::InputMethod::CInputMethodSubtypeSwitchingControllerHelper;
using Elastos::Droid::Widget::EIID_IArrayAdapter;
using Elastos::Droid::Widget::EIID_IBaseAdapter;
using Elastos::Droid::Widget::EIID_IListAdapter;
using Elastos::Droid::Widget::EIID_IAdapter;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IRadioButton;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Utility::CAtomicFile;
using Elastos::Droid::Utility::CParcelableList;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Server::Wm::EIID_IOnHardKeyboardStatusChangeListener;

using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::Math;
using Elastos::Core::EIID_IComparable;
using Elastos::IO::ICloseable;
using Elastos::IO::CFile;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IFileInputStream;
using Elastos::Utility::IMap;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

const String CInputMethodManagerService::TAG("CInputMethodManagerService");
const Boolean CInputMethodManagerService::DEBUG = FALSE;
const Int32 CInputMethodManagerService::MSG_SHOW_IM_PICKER;
const Int32 CInputMethodManagerService::MSG_SHOW_IM_SUBTYPE_PICKER;
const Int32 CInputMethodManagerService::MSG_SHOW_IM_SUBTYPE_ENABLER;
const Int32 CInputMethodManagerService::MSG_SHOW_IM_CONFIG;
const Int32 CInputMethodManagerService::MSG_UNBIND_INPUT;
const Int32 CInputMethodManagerService::MSG_BIND_INPUT;
const Int32 CInputMethodManagerService::MSG_SHOW_SOFT_INPUT;
const Int32 CInputMethodManagerService::MSG_HIDE_SOFT_INPUT;
const Int32 CInputMethodManagerService::MSG_ATTACH_TOKEN;
const Int32 CInputMethodManagerService::MSG_CREATE_SESSION;
const Int32 CInputMethodManagerService::MSG_START_INPUT;
const Int32 CInputMethodManagerService::MSG_RESTART_INPUT;
const Int32 CInputMethodManagerService::MSG_UNBIND_METHOD;
const Int32 CInputMethodManagerService::MSG_BIND_METHOD;
const Int32 CInputMethodManagerService::MSG_SET_ACTIVE;
const Int32 CInputMethodManagerService::MSG_SET_USER_ACTION_NOTIFICATION_SEQUENCE_NUMBER;
const Int32 CInputMethodManagerService::MSG_HARD_KEYBOARD_SWITCH_CHANGED;
const Int64 CInputMethodManagerService::TIME_TO_RECONNECT;
const Int32 CInputMethodManagerService::SECURE_SUGGESTION_SPANS_MAX_SIZE;
const Int32 CInputMethodManagerService::NOT_A_SUBTYPE_ID = IInputMethodUtils::NOT_A_SUBTYPE_ID;
String CInputMethodManagerService::TAG_TRY_SUPPRESSING_IME_SWITCHER("TrySuppressingImeSwitcher");

//============================================================================
// CInputMethodManagerService::MyHandlerCallerCallback
//============================================================================
CAR_INTERFACE_IMPL(CInputMethodManagerService::MyHandlerCallerCallback, Object, IHandlerCallerCallback)

CInputMethodManagerService::MyHandlerCallerCallback::MyHandlerCallerCallback(
    /* [in] */ IWeakReference* host)
    : mWeakHost(host)
{}

ECode CInputMethodManagerService::MyHandlerCallerCallback::ExecuteMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IHandlerCallback> cb;
    mWeakHost->Resolve(EIID_IHandlerCallback, (IInterface**)&cb);
    if (cb != NULL) {
        Boolean result;
        return cb->HandleMessage(msg, &result);
    }
    return NOERROR;
}

//============================================================================
// CInputMethodManagerService::VisibleServiceConnection
//============================================================================
CAR_INTERFACE_IMPL(CInputMethodManagerService::VisibleServiceConnection, Object, IServiceConnection)

ECode CInputMethodManagerService::VisibleServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    return NOERROR;
}

ECode CInputMethodManagerService::VisibleServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    return NOERROR;
}


//============================================================================
// CInputMethodManagerService::SessionState
//============================================================================
CInputMethodManagerService::SessionState::SessionState(
    /* [in] */ ClientState* client,
    /* [in] */ IIInputMethod* method,
    /* [in] */ IIInputMethodSession* session,
    /* [in] */ IInputChannel* channel)
    : mClient(client)
    , mMethod(method)
    , mSession(session)
    , mChannel(channel)
{}

ECode CInputMethodManagerService::SessionState::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("SessionState{uid ");
    sb += mClient->mUid;
    sb += " pid ";
    sb += mClient->mPid;
    sb += " method ";
    sb += TO_CSTR(mMethod);
    sb += " session ";
    sb += TO_CSTR(mSession);
    sb += " channel ";
    sb += TO_CSTR(mChannel);
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

//============================================================================
// CInputMethodManagerService::ClientState
//============================================================================

CInputMethodManagerService::ClientState::ClientState(
    /* [in] */ IInputMethodClient* client,
    /* [in] */ IIInputContext* inputContext,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid)
    : mClient(client)
    , mInputContext(inputContext)
    , mUid(uid)
    , mPid(pid)
    , mSessionRequested(FALSE)
{
    assert(mInputContext != NULL);
    ASSERT_SUCCEEDED(CInputBinding::New(NULL, IBinder::Probe(mInputContext),
            mUid, mPid, (IInputBinding**)&mBinding));
}

CInputMethodManagerService::ClientState::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("ClientState{");
    sb += StringUtils::ToHexString((Int32)this);
    sb += " uid ";
    sb += mUid;
    sb += " pid ";
    sb += mPid;
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

//============================================================================
// CInputMethodManagerService::SettingsObserver
//============================================================================
CInputMethodManagerService::SettingsObserver::SettingsObserver()
    : mLastEnabled("")
{}

ECode CInputMethodManagerService::SettingsObserver::constructor(
    /* [in] */ IHandler* handler,
    /* [in] */ CInputMethodManagerService* host)
{
    FAIL_RETURN(ContentObserver::constructor(handler))
    mHost = host;

    AutoPtr<IContentResolver> resolver;
    mHost->mContext->GetContentResolver((IContentResolver**)&resolver);
    assert(resolver != NULL);

    IContentObserver* co = this;
    AutoPtr<IUri> uri;
    Settings::Secure::GetUriFor(ISettingsSecure::DEFAULT_INPUT_METHOD, (IUri**)&uri);
    assert(uri != NULL);
    resolver->RegisterContentObserver(uri, FALSE, co);

    uri = NULL;
    Settings::Secure::GetUriFor(ISettingsSecure::ENABLED_INPUT_METHODS, (IUri**)&uri);
    assert(uri != NULL);
    resolver->RegisterContentObserver(uri, FALSE, co);

    uri = NULL;
    Settings::Secure::GetUriFor(ISettingsSecure::SELECTED_INPUT_METHOD_SUBTYPE, (IUri**)&uri);
    assert(uri != NULL);
    resolver->RegisterContentObserver(uri, FALSE, co);

    uri = NULL;
    Settings::Secure::GetUriFor(ISettingsSecure::SHOW_IME_WITH_HARD_KEYBOARD, (IUri**)&uri);
    assert(uri != NULL);
    resolver->RegisterContentObserver(uri, FALSE, co);

    uri = NULL;
    Settings::System::GetUriFor(ISettingsSystem::STATUS_BAR_IME_SWITCHER, (IUri**)&uri);
    assert(uri != NULL);
    AutoPtr<UpdateFromSettingsLockObserver> observer = new UpdateFromSettingsLockObserver();
    observer->constructor(mHandler, mHost);
    resolver->RegisterContentObserver(uri, FALSE, observer);

    return NOERROR;
}

ECode CInputMethodManagerService::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    AutoPtr<IUri> showImeUri;
    Settings::Secure::GetUriFor(ISettingsSecure::SHOW_IME_WITH_HARD_KEYBOARD, (IUri**)&showImeUri);
    {    AutoLock syncLock(mHost->mMethodMap.Get());
        if (Object::Equals(showImeUri, uri)) {
            mHost->UpdateKeyboardFromSettingsLocked();
        }
        else {
            Boolean enabledChanged = FALSE;
            String newEnabled;
            mHost->mSettings->GetEnabledInputMethodsStr(&newEnabled);
            if (!mLastEnabled.Equals(newEnabled)) {
                mLastEnabled = newEnabled;
                enabledChanged = TRUE;
            }
            mHost->UpdateInputMethodsFromSettingsLocked(enabledChanged);
        }
    }

    return NOERROR;
}

//============================================================================
// CInputMethodManagerService::UpdateFromSettingsLockObserver
//============================================================================
CInputMethodManagerService::UpdateFromSettingsLockObserver::UpdateFromSettingsLockObserver()
{}

ECode CInputMethodManagerService::UpdateFromSettingsLockObserver::constructor(
    /* [in] */ IHandler* handler,
    /* [in] */ CInputMethodManagerService* host)
{
    FAIL_RETURN(ContentObserver::constructor(handler))
    mHost = host;
    return NOERROR;
}

ECode CInputMethodManagerService::UpdateFromSettingsLockObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    mHost->UpdateFromSettingsLocked(TRUE);
    return NOERROR;
}

//============================================================================
// CInputMethodManagerService::ImmsBroadcastReceiver
//============================================================================

ECode CInputMethodManagerService::ImmsBroadcastReceiver::constructor(
    /* [in] */ CInputMethodManagerService* host)
{
    mHost = host;
    BroadcastReceiver::constructor();
    return NOERROR;
}

void CInputMethodManagerService::ImmsBroadcastReceiver::UpdateActive()
{
   // Inform the current client of the change in active status
   if (mHost->mCurClient != NULL && mHost->mCurClient->mClient != NULL) {
        AutoPtr<IMessage> msg;
        mHost->mCaller->ObtainMessageIO(
            CInputMethodManagerService::MSG_SET_ACTIVE,
            mHost->mScreenOn ? 1 : 0, TO_IINTERFACE(mHost->mCurClient), (IMessage**)&msg);
        mHost->ExecuteOrSendMessage(mHost->mCurClient->mClient, msg);
   }
}

ECode CInputMethodManagerService::ImmsBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_SCREEN_ON.Equals(action)) {
        mHost->mScreenOn = TRUE;
        mHost->RefreshImeWindowVisibilityLocked();
        UpdateActive();
        return NOERROR;
    }
    else if (IIntent::ACTION_SCREEN_OFF.Equals(action)) {
        mHost->mScreenOn = FALSE;
        mHost->SetImeWindowVisibilityStatusHiddenLocked();
        UpdateActive();
        return NOERROR;
    }
    else if (IIntent::ACTION_CLOSE_SYSTEM_DIALOGS.Equals(action)) {
        mHost->HideInputMethodMenu();
        // No need to updateActive
        return NOERROR;
    }
    else if (IIntent::ACTION_USER_ADDED.Equals(action)
        || IIntent::ACTION_USER_REMOVED.Equals(action)) {
        mHost->UpdateCurrentProfileIds();
        return NOERROR;
    }
    else {
        Slogger::W(TAG, "Unexpected intent %s", TO_CSTR(intent));
    }

    return NOERROR;
}


//============================================================================
// CInputMethodManagerService::MyPackageMonitor
//============================================================================

ECode CInputMethodManagerService::MyPackageMonitor::constructor(
    /* [in] */ CInputMethodManagerService* host)
{
    mHost = host;
    PackageMonitor::constructor();
    return NOERROR;
}

Boolean CInputMethodManagerService::MyPackageMonitor::IsChangingPackagesOfCurrentUser()
{
    Int32 userId = 0, curUid;
    GetChangingUserId(&userId);
    mHost->mSettings->GetCurrentUserId(&curUid);
    Boolean retval = userId == curUid;
    if (DEBUG) {
        if (!retval) {
            Slogger::D(TAG, "--- ignore this call back from a background user: %d", userId);
        }
    }
    return retval;
}

ECode CInputMethodManagerService::MyPackageMonitor::OnHandleForceStop(
    /* [in] */ IIntent* intent,
    /* [in] */ ArrayOf<String>* packages,
    /* [in] */ Int32 uid,
    /* [in] */ Boolean doit,
    /* [out] */ Boolean* result)
{
    assert(result != NULL);
    if (!IsChangingPackagesOfCurrentUser()) {
        *result = FALSE;
        return NOERROR;
    }

    if (packages != NULL) {
        AutoLock lock(mHost->mMethodMap.Get());
        String curInputMethodId;
        mHost->mSettings->GetSelectedInputMethod(&curInputMethodId);

        if (!curInputMethodId.IsNull()) {
            AutoPtr<IIterator> it;
            mHost->mMethodList->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                AutoPtr<IInputMethodInfo> imi = IInputMethodInfo::Probe(obj);

                String id;
                imi->GetId(&id);
                if (id.Equals(curInputMethodId)) {
                    for (Int32 i = 0; i < packages->GetLength(); i++) {
                        String pkg = (*packages)[i];

                        String name;
                        imi->GetPackageName(&name);
                        if (name.Equals(pkg)) {
                            if (!doit) {
                                *result = TRUE;
                                return NOERROR;
                            }

                            mHost->ResetSelectedInputMethodAndSubtypeLocked(String(""));
                            mHost->ChooseNewDefaultIMELocked();
                            *result = TRUE;
                            return NOERROR;
                        }
                    }
                }
            }
        }
    }

    *result = FALSE;
    return NOERROR;
}

ECode CInputMethodManagerService::MyPackageMonitor::OnSomePackagesChanged()
{
    if (!IsChangingPackagesOfCurrentUser()) {
        return NOERROR;
    }

    {
        AutoLock lock(mHost->mMethodMap.Get());
        AutoPtr<IInputMethodInfo> curIm;
        String curInputMethodId;
        mHost->mSettings->GetSelectedInputMethod(&curInputMethodId);
        if (!curInputMethodId.IsNull()) {
            AutoPtr<IIterator> it;
            mHost->mMethodList->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                AutoPtr<IInputMethodInfo> imi = IInputMethodInfo::Probe(obj);

                String imiId;
                imi->GetId(&imiId);
                if (imiId.Equals(curInputMethodId)) {
                    curIm = imi;
                }

                Int32 change = 0;
                String name;
                imi->GetPackageName(&name);
                IsPackageDisappearing(name, &change);

                Boolean isModified = FALSE;
                if (IsPackageModified(name, &isModified), isModified) {
                    mHost->mFileManager->DeleteAllInputMethodSubtypes(imiId);
                }

                if (change == PACKAGE_TEMPORARY_CHANGE
                        || change == PACKAGE_PERMANENT_CHANGE) {
                    // Slog.i(TAG, "Input method uninstalled, disabling: "
                    //         + imi.getComponent());
                    mHost->SetInputMethodEnabledLocked(imiId, FALSE);
                }
            }
        }

        mHost->BuildInputMethodListLocked(
            mHost->mMethodList, mHost->mMethodMap, FALSE /* resetDefaultEnabledIme */);

        Boolean changed = FALSE;

        if (curIm != NULL) {
            Int32 change = 0;
            String name;
            curIm->GetPackageName(&name);
            IsPackageDisappearing(name, &change);
            if (change == PACKAGE_TEMPORARY_CHANGE
                    || change == PACKAGE_PERMANENT_CHANGE) {
                AutoPtr<IServiceInfo> si;
                // try {

                AutoPtr<IComponentName> compName;
                curIm->GetComponent((IComponentName**)&compName);
                Int32 curUid;
                mHost->mSettings->GetCurrentUserId(&curUid);
                mHost->mIPackageManager->GetServiceInfo(compName, 0,
                    curUid, (IServiceInfo**)&si);
                // } catch (RemoteException ex) {
                // }
                if (si == NULL) {
                    // Uh oh, current input method is no longer around!
                    // Pick another one...
                    // Slog.i(TAG, "Current input method removed: " + curInputMethodId);
                    mHost->SetImeWindowVisibilityStatusHiddenLocked();
                    if (!mHost->ChooseNewDefaultIMELocked()) {
                        changed = TRUE;
                        curIm = NULL;
                        // Slog.i(TAG, "Unsetting current input method");
                        mHost->ResetSelectedInputMethodAndSubtypeLocked(String(""));
                    }
                }
            }
        }

        if (curIm == NULL) {
            // We currently don't have a default input method... is
            // one now available?
            changed = mHost->ChooseNewDefaultIMELocked();
        }

        if (changed) {
            mHost->UpdateFromSettingsLocked(FALSE);
        }
    }

    return NOERROR;
}


//============================================================================
// CInputMethodManagerService::HardKeyboardListener
//============================================================================

CAR_INTERFACE_IMPL(CInputMethodManagerService::HardKeyboardListener, Object, IOnHardKeyboardStatusChangeListener)

CInputMethodManagerService::HardKeyboardListener::HardKeyboardListener(
    /* [in] */ CInputMethodManagerService* host)
    : mHost(host)
{
}

ECode CInputMethodManagerService::HardKeyboardListener::OnHardKeyboardStatusChange(
    /* [in] */ Boolean available)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(
        CInputMethodManagerService::MSG_HARD_KEYBOARD_SWITCH_CHANGED,
        available ? 1 : 0, 0, (IMessage**)&msg);
    Boolean result;
    mHost->mHandler->SendMessage(msg, &result);
    return NOERROR;
}

ECode CInputMethodManagerService::HardKeyboardListener::HandleHardKeyboardStatusChange(
    /* [in] */ Boolean available)
{
    if (DEBUG) {
        Slogger::W(TAG, "HardKeyboardStatusChanged: available = %d", available);
    }
    {
        AutoLock lock(mHost->mMethodMap.Get());

        Boolean showing = FALSE;
        if (mHost->mSwitchingDialog != NULL && mHost->mSwitchingDialogTitleView != NULL
                && (IDialog::Probe(mHost->mSwitchingDialog)->IsShowing(&showing), showing)) {
            AutoPtr<IView> view;
            mHost->mSwitchingDialogTitleView->FindViewById(
                R::id::hard_keyboard_section, (IView**)&view);
            assert(view != NULL);
            view->SetVisibility(available ? IView::VISIBLE : IView::GONE);
        }
    }
    return NOERROR;
}

//============================================================================
// CInputMethodManagerService::CheckReceiver
//============================================================================

ECode CInputMethodManagerService::CheckReceiver::constructor(
    /* [in] */ CInputMethodManagerService* host)
{
    BroadcastReceiver::constructor();
    mHost = host;
    return NOERROR;
}

ECode CInputMethodManagerService::CheckReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    AutoLock lock(mHost->mMethodMap.Get());
    mHost->ResetStateIfCurrentLocaleChangedLocked();
    return NOERROR;
}

//============================================================================
// CInputMethodManagerService::ImeSubtypeListAdapter
//============================================================================

CInputMethodManagerService::ImeSubtypeListAdapter::ImeSubtypeListAdapter()
    : mCheckedItem(0)
    , mTextViewResourceId(0)
{}

CInputMethodManagerService::ImeSubtypeListAdapter::~ImeSubtypeListAdapter()
{
    mItemsList = NULL;
}

ECode CInputMethodManagerService::ImeSubtypeListAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 textViewResourceId,
    /* [in] */ IList* itemsList,
    /* [in] */ Int32 checkedItem)
{
    mCheckedItem = checkedItem;
    mTextViewResourceId = textViewResourceId;
    ArrayAdapter::constructor(context, textViewResourceId, itemsList);
    mItemsList = itemsList;
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    mInflater = ILayoutInflater::Probe(obj);
    return NOERROR;
}

ECode CInputMethodManagerService::ImeSubtypeListAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IView> view;
    if (convertView != NULL) {
        view = convertView;
    }
    else {
        mInflater->Inflate(mTextViewResourceId, NULL, (IView**)&view);
    }

    Int32 size;
    mItemsList->GetSize(&size);
    if (position < 0 || position >= size) {
        *result = view;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    AutoPtr<IInterface> obj;
    mItemsList->Get(position, (IInterface**)&obj);
    AutoPtr<IImeSubtypeListItem> item = IImeSubtypeListItem::Probe(obj);
    assert(item != NULL);
    AutoPtr<ICharSequence> imeName, subtypeName;
    item->GetImeName((ICharSequence**)&imeName);
    item->GetSubtypeName((ICharSequence**)&subtypeName);
    AutoPtr<IView> v1, v2, v3;
    view->FindViewById(R::id::text1, (IView**)&v1);
    view->FindViewById(R::id::text2, (IView**)&v2);
    ITextView* firstTextView = ITextView::Probe(v1);
    ITextView* secondTextView = ITextView::Probe(v2);
    if (TextUtils::IsEmpty(subtypeName)) {
        firstTextView->SetText(imeName);
        v2->SetVisibility(IView::GONE);
    }
    else {
        firstTextView->SetText(subtypeName);
        secondTextView->SetText(imeName);
        v2->SetVisibility(IView::VISIBLE);
    }
    view->FindViewById(R::id::radio, (IView**)&v3);
    IRadioButton* radioButton = IRadioButton::Probe(v3);
    assert(ICheckable::Probe(radioButton) != NULL);
    ICheckable::Probe(radioButton)->SetChecked(position == mCheckedItem);
    *result = view;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//============================================================================
// CInputMethodManagerService::InputMethodFileManager
//============================================================================

const String CInputMethodManagerService::InputMethodFileManager::SYSTEM_PATH("system");
const String CInputMethodManagerService::InputMethodFileManager::INPUT_METHOD_PATH("inputmethod");
const String CInputMethodManagerService::InputMethodFileManager::ADDITIONAL_SUBTYPES_FILE_NAME("subtypes.xml");
const String CInputMethodManagerService::InputMethodFileManager::NODE_SUBTYPES("subtypes");
const String CInputMethodManagerService::InputMethodFileManager::NODE_SUBTYPE("subtype");
const String CInputMethodManagerService::InputMethodFileManager::NODE_IMI("imi");
const String CInputMethodManagerService::InputMethodFileManager::ATTR_ID("id");
const String CInputMethodManagerService::InputMethodFileManager::ATTR_LABEL("label");
const String CInputMethodManagerService::InputMethodFileManager::ATTR_ICON("icon");
const String CInputMethodManagerService::InputMethodFileManager::ATTR_IME_SUBTYPE_LOCALE("imeSubtypeLocale");
const String CInputMethodManagerService::InputMethodFileManager::ATTR_IME_SUBTYPE_MODE("imeSubtypeMode");
const String CInputMethodManagerService::InputMethodFileManager::ATTR_IME_SUBTYPE_EXTRA_VALUE("imeSubtypeExtraValue");
const String CInputMethodManagerService::InputMethodFileManager::ATTR_IS_AUXILIARY("isAuxiliary");

CInputMethodManagerService::InputMethodFileManager::InputMethodFileManager(
    /* [in] */ IHashMap* methodMap,
    /* [in] */ Int32 userId,
    /* [in] */ CInputMethodManagerService*  host)
    : mHost(host)
{
    if (methodMap == NULL) {
        assert(0);
        // throw new NullPointerException("methodMap is NULL");
    }
    mMethodMap = methodMap;
    CHashMap::New((IHashMap**)&mAdditionalSubtypesMap);
    AutoPtr<IFile> systemDir;
    if (userId == IUserHandle::USER_OWNER) {
        AutoPtr<IEnvironment> env;
        CEnvironment::AcquireSingleton((IEnvironment**)&env);
        AutoPtr<IFile> dataDir;
        env->GetDataDirectory((IFile**)&dataDir);
        CFile::New(dataDir, SYSTEM_PATH, (IFile**)&systemDir);
    }
    else {
        AutoPtr<IEnvironment> env;
        CEnvironment::AcquireSingleton((IEnvironment**)&env);
        AutoPtr<IFile> sysDir;
        env->GetUserSystemDirectory(userId, (IFile**)&systemDir);
    }
    AutoPtr<IFile> inputMethodDir;
    CFile::New(systemDir, INPUT_METHOD_PATH, (IFile**)&inputMethodDir);
    Boolean result = FALSE;
    if (!(inputMethodDir->Mkdirs(&result), result)) {
        // Slogger::W(TAG, "Couldn't create dir.: " + inputMethodDir.getAbsolutePath());
    }
    AutoPtr<IFile> subtypeFile;
    CFile::New(inputMethodDir, ADDITIONAL_SUBTYPES_FILE_NAME, (IFile**)&subtypeFile);
    CAtomicFile::New(subtypeFile, (IAtomicFile**)&mAdditionalInputMethodSubtypeFile);
    if (!(subtypeFile->Exists(&result), result)) {
        // If "subtypes.xml" doesn't exist, create a blank file.
        WriteAdditionalInputMethodSubtypes(
            mAdditionalSubtypesMap, mAdditionalInputMethodSubtypeFile, methodMap);
    }
    else {
        ReadAdditionalInputMethodSubtypes(
            mAdditionalSubtypesMap, mAdditionalInputMethodSubtypeFile);
    }
}

void CInputMethodManagerService::InputMethodFileManager::DeleteAllInputMethodSubtypes(
    /* [in] */ const String& imiId)
{
    AutoLock lock(mMethodMap.Get());

    AutoPtr<ICharSequence> csq = CoreUtils::Convert(imiId);
    mAdditionalSubtypesMap->Remove(TO_IINTERFACE(csq));
    WriteAdditionalInputMethodSubtypes(
        mAdditionalSubtypesMap, mAdditionalInputMethodSubtypeFile, mMethodMap);
}

void CInputMethodManagerService::InputMethodFileManager::AddInputMethodSubtypes(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ ArrayOf<IInputMethodSubtype*>* additionalSubtypes)
{
    {
        AutoLock lock(mMethodMap.Get());

        AutoPtr<IArrayList> subtypes;
        CArrayList::New((IArrayList**)&subtypes);
        const Int32 N = additionalSubtypes->GetLength();
        Boolean bval;
        for (Int32 i = 0; i < N; ++i) {
            IInterface* item = TO_IINTERFACE((*additionalSubtypes)[i]);
            subtypes->Contains(item, &bval);
            if (!bval) {
                subtypes->Add(item);
            }
            else {
                Slogger::W(TAG, "Duplicated subtype definition found: %s", TO_CSTR(item));
                    // + subtype.getLocale() + ", " + subtype.getMode());
            }
        }
        String id;
        imi->GetId(&id);
        AutoPtr<ICharSequence> csq = CoreUtils::Convert(id);
        mAdditionalSubtypesMap->Put(TO_IINTERFACE(csq), subtypes.Get());
        WriteAdditionalInputMethodSubtypes(
            mAdditionalSubtypesMap, mAdditionalInputMethodSubtypeFile, mMethodMap);
    }
}

AutoPtr<IHashMap>
CInputMethodManagerService::InputMethodFileManager::GetAllAdditionalInputMethodSubtypes()
{
    AutoLock lock(mMethodMap.Get());
    return mAdditionalSubtypesMap;
}

void CInputMethodManagerService::InputMethodFileManager::WriteAdditionalInputMethodSubtypes(
    /* [in] */ IHashMap* allSubtypes,
    /* [in] */ IAtomicFile* subtypesFile,
    /* [in] */ IHashMap* methodMap)
{
    // Safety net for the case that this function is called before methodMap is set.
    Boolean bval;
    Boolean isSetMethodMap = methodMap != NULL && (methodMap->IsEmpty(&bval), !bval);
    AutoPtr<IFileOutputStream> fos;
    String nullStr;

    // try {
    subtypesFile->StartWrite((IFileOutputStream**)&fos);
    AutoPtr<IXmlSerializer> out;
    CFastXmlSerializer::New((IXmlSerializer**)&out);
    assert(out != NULL);

    out->SetOutput(IOutputStream::Probe(fos), String("utf-8"));
    out->StartDocument(nullStr, TRUE);
    out->SetFeature(String("http://xmlpull.org/v1/doc/features.html#indent-output"), TRUE);
    out->WriteStartTag(nullStr, NODE_SUBTYPES);

    assert(allSubtypes != NULL);
    AutoPtr<ISet> set;
    allSubtypes->GetKeySet((ISet**)&set);
    AutoPtr<IIterator> it;
    set->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> keyObj, valueObj;
        it->GetNext((IInterface**)&keyObj);
        Boolean containsKey = FALSE;
        methodMap->ContainsKey(keyObj, &containsKey);
        if (isSetMethodMap && !containsKey) {
            // Slogger::W(TAG, "IME uninstalled or not valid.: " + imiId);
            continue;
        }

        String imiId = TO_STR(keyObj);
        out->WriteStartTag(nullStr, NODE_IMI);
        out->WriteAttribute(nullStr, ATTR_ID, imiId);

        allSubtypes->Get(keyObj, (IInterface**)&valueObj);
        AutoPtr<IList> subtypesList = IList::Probe(valueObj);

        AutoPtr<IIterator> lit;
        subtypesList->GetIterator((IIterator**)&lit);
        Boolean bval;
        while (lit->HasNext(&bval), bval) {
            AutoPtr<IInterface> subtypeObj;
            lit->GetNext((IInterface**)&subtypeObj);
            AutoPtr<IInputMethodSubtype> subtype = IInputMethodSubtype::Probe(subtypeObj);
            out->WriteStartTag(nullStr, NODE_SUBTYPE);

            Int32 id = 0;
            out->WriteAttribute(nullStr, ATTR_ICON, StringUtils::ToString(subtype->GetIconResId(&id), id));
            out->WriteAttribute(nullStr, ATTR_LABEL, StringUtils::ToString(subtype->GetNameResId(&id), id));

            String value;
            out->WriteAttribute(nullStr, ATTR_IME_SUBTYPE_LOCALE, (subtype->GetLocale(&value), value));
            out->WriteAttribute(nullStr, ATTR_IME_SUBTYPE_MODE, (subtype->GetMode(&value), value));
            out->WriteAttribute(nullStr, ATTR_IME_SUBTYPE_EXTRA_VALUE, (subtype->GetExtraValue(&value), value));

            Boolean isAuxiliary = FALSE;
            out->WriteAttribute(nullStr, ATTR_IS_AUXILIARY,
                    StringUtils::ToString((subtype->IsAuxiliary(&isAuxiliary), isAuxiliary) ? 1 : 0));
            out->WriteEndTag(nullStr, NODE_SUBTYPE);
        }

        out->WriteEndTag(nullStr, NODE_IMI);
    }

    out->WriteEndTag(nullStr, NODE_SUBTYPES);
    out->EndDocument();
    subtypesFile->FinishWrite(fos);
    // } catch (java.io.IOException e) {
    //     Slogger::W(TAG, "Error writing subtypes", e);
    //     if (fos != NULL) {
    //         subtypesFile.failWrite(fos);
    //     }
    // }
}

ECode CInputMethodManagerService::InputMethodFileManager::ReadAdditionalInputMethodSubtypes(
    /* [in] */ IHashMap* allSubtypes,
    /* [in] */ IAtomicFile* subtypesFile)
{
    if (allSubtypes == NULL || subtypesFile == NULL) return NOERROR;

    allSubtypes->Clear();
    AutoPtr<IFileInputStream> fis;
    String nullStr;
    // // try {
    subtypesFile->OpenRead((IFileInputStream**)&fis);
    AutoPtr<IXmlPullParser> parser;
    Xml::NewPullParser((IXmlPullParser**)&parser);
    parser->SetInput(IInputStream::Probe(fis), String(NULL));
    Int32 type = 0;
    parser->GetEventType(&type);
    // Skip parsing until START_TAG
    while ((parser->Next(&type), type) != IXmlPullParser::START_TAG
            && type != IXmlPullParser::END_DOCUMENT) {}

    String firstNodeName;
    parser->GetName(&firstNodeName);
    if (!NODE_SUBTYPES.Equals(firstNodeName)) {
        Slogger::E(TAG, "Xml doesn't start with subtypes");
        if (fis != NULL) {
            ICloseable::Probe(fis)->Close();
        }
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    Int32 depth = 0, nextDepth;
    parser->GetDepth(&depth);
    String currentImiId;
    AutoPtr<IArrayList> tempSubtypesArray;
    while (((parser->Next(&type), type) != IXmlPullParser::END_TAG
            || (parser->GetDepth(&nextDepth), nextDepth) > depth) && type != IXmlPullParser::END_DOCUMENT) {
        if (type != IXmlPullParser::START_TAG)
            continue;

        String nodeName;
        parser->GetName(&nodeName);
        if (NODE_IMI.Equals(nodeName) == 0) {
            parser->GetAttributeValue(String(NULL)/*NULL*/, ATTR_ID, &currentImiId);

            if (TextUtils::IsEmpty(currentImiId)) {
                Slogger::W(TAG, "Invalid imi id found in subtypes.xml");
                continue;
            }

            tempSubtypesArray = NULL;
            CArrayList::New((IArrayList**)&tempSubtypesArray);
            AutoPtr<ICharSequence> key = CoreUtils::Convert(currentImiId);
            allSubtypes->Put(TO_IINTERFACE(key), tempSubtypesArray);
        }
        else if (NODE_SUBTYPE.Equals(nodeName) == 0) {
            if (TextUtils::IsEmpty(currentImiId) || tempSubtypesArray == NULL) {
                // Slogger::W(TAG, "IME uninstalled or not valid.: " + currentImiId);
                continue;
            }

            String value;
            parser->GetAttributeValue(nullStr, ATTR_ICON, &value);
            const Int32 icon = StringUtils::ParseInt32(value);

            parser->GetAttributeValue(nullStr, ATTR_LABEL, &value);
            const Int32 label = StringUtils::ParseInt32(value);
            String imeSubtypeLocale;
            parser->GetAttributeValue(nullStr, ATTR_IME_SUBTYPE_LOCALE, &imeSubtypeLocale);

            String imeSubtypeMode;
            parser->GetAttributeValue(nullStr, ATTR_IME_SUBTYPE_MODE, &imeSubtypeMode);

            String imeSubtypeExtraValue;
            parser->GetAttributeValue(nullStr, ATTR_IME_SUBTYPE_EXTRA_VALUE, &imeSubtypeExtraValue);

            Boolean isAuxiliary = String("1").Equals(
                (parser->GetAttributeValue(nullStr, ATTR_IS_AUXILIARY, &value), value)) == 0;

            AutoPtr<IInputMethodSubtypeBuilder> builder;
            CInputMethodSubtypeBuilder::New((IInputMethodSubtypeBuilder**)&builder);
            builder->SetSubtypeNameResId(label);
            builder->SetSubtypeIconResId(icon);
            builder->SetSubtypeLocale(imeSubtypeLocale);
            builder->SetSubtypeMode(imeSubtypeMode);
            builder->SetSubtypeExtraValue(imeSubtypeExtraValue);
            builder->SetIsAuxiliary(isAuxiliary);
            AutoPtr<IInputMethodSubtype> subtype;
            builder->Build((IInputMethodSubtype**)&subtype);

            tempSubtypesArray->Add(subtype.Get());
        }
    }
    // // } catch (XmlPullParserException e) {
    // //     Slogger::W(TAG, "Error reading subtypes: " + e);
    // //     return;
    // // } catch (java.io.IOException e) {
    // //     Slogger::W(TAG, "Error reading subtypes: " + e);
    // //     return;
    // // } catch (NumberFormatException e) {
    // //     Slogger::W(TAG, "Error reading subtypes: " + e);
    // //     return;
    // // }
    // // finally {
    if (fis != NULL) {
        // try {
        ICloseable::Probe(fis)->Close();
        // } catch (java.io.IOException e1) {
        //     Slogger::W(TAG, "Failed to close.");
        // }
    }
    // // }
    return NOERROR;
}


//============================================================================
// CInputMethodManagerService
//============================================================================

CAR_INTERFACE_IMPL_4(CInputMethodManagerService, Object, IIInputMethodManager, \
    IBinder, IServiceConnection, IHandlerCallback)

CAR_OBJECT_IMPL(CInputMethodManagerService)

CInputMethodManagerService::CInputMethodManagerService()
    : mHasFeature(FALSE)
    , mVisibleBound(FALSE)
    , mClients(50)
    , mSystemReady(FALSE)
    , mCurSeq(0)
    , mHaveConnection(FALSE)
    , mShowRequested(FALSE)
    , mShowExplicitlyRequested(FALSE)
    , mShowForced(FALSE)
    , mInputShown(FALSE)
    , mLastBindTime(0)
    , mBoundToMethod(FALSE)
    , mScreenOn(TRUE)
    , mCurUserActionNotificationSequenceNumber(0)
    , mBackDisposition(IInputMethodService::BACK_DISPOSITION_DEFAULT)
    , mImeWindowVis(0)
    , mWindowManagerService(NULL)
    , mSecureSuggestionSpans(20)
    , mShowOngoingImeSwitcherForPhones(FALSE)
    , mNotificationShown(FALSE)
    , mImeSelectedOnBoot(FALSE)
    , mCurClientInKeyguard(FALSE)
    , mShowImeWithHardKeyboard(FALSE)
    , mInputBoundToKeyguard(FALSE)
{
}

CInputMethodManagerService::~CInputMethodManagerService()
{
    mMethodList = NULL;
    mMethodMap = NULL;
    mClients.Clear();
    mSecureSuggestionSpans.Clear();
    mShortcutInputMethodsAndSubtypes = NULL;
}

ECode CInputMethodManagerService::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IIWindowManager* windowManager)
{
    mVisibleConnection =  new VisibleServiceConnection();
    CArrayList::New((IArrayList**)&mMethodList);
    CHashMap::New((IHashMap**)&mShortcutInputMethodsAndSubtypes);
    CHashMap::New((IHashMap**)&mMethodMap);
    CInputBindResult::New(NULL, NULL, String(NULL), -1, -1, (IInputBindResult**)&mNoBinding);

    mMyPackageMonitor = new MyPackageMonitor();
    mMyPackageMonitor->constructor(this);

    mIPackageManager = AppGlobals::GetPackageManager();
    mContext = context;
    context->GetResources((IResources**)&mRes);
    CHandler::New(this, FALSE, (IHandler**)&mHandler);
    mIWindowManager = IIWindowManager::Probe(ServiceManager::GetService(IContext::WINDOW_SERVICE));
    AutoPtr<ILooper> looper;
    context->GetMainLooper((ILooper**)&looper);
    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    AutoPtr<IHandlerCallerCallback> callback = new MyHandlerCallerCallback(wr);
    CHandlerCaller::New(context, NULL, callback, TRUE/*asyncHandler*/,
        TRUE, (IHandlerCaller**)&mCaller);
    mWindowManagerService = (CWindowManagerService*)windowManager;
    AutoPtr<IInterface> service;
    mContext->GetSystemService(IContext::APP_OPS_SERVICE, (IInterface**)&service);
    mAppOpsManager = IAppOpsManager::Probe(service);
    mHardKeyboardListener = new HardKeyboardListener(this);
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    pm->HasSystemFeature(IPackageManager::FEATURE_INPUT_METHODS, &mHasFeature);

    CNotification::New((INotification**)&mImeSwitcherNotification);
    mImeSwitcherNotification->SetIcon(R::drawable::ic_notification_ime_default);
    mImeSwitcherNotification->SetWhen(0);
    mImeSwitcherNotification->SetFlags(INotification::FLAG_ONGOING_EVENT);
    mImeSwitcherNotification->SetTickerText(NULL);
    mImeSwitcherNotification->SetDefaults(0); // please be quiet
    mImeSwitcherNotification->SetSound(NULL);
    mImeSwitcherNotification->SetVibrate(NULL);

    // Tag this notification specially so SystemUI knows it's important
    AutoPtr<IBundle> extras;
    mImeSwitcherNotification->GetExtras((IBundle**)&extras);
    extras->PutBoolean(INotification::EXTRA_ALLOW_DURING_SETUP, TRUE);
    mImeSwitcherNotification->SetCategory(INotification::CATEGORY_SYSTEM);

    AutoPtr<IIntent> intent;
    CIntent::New(ISettings::ACTION_SHOW_INPUT_METHOD_PICKER, (IIntent**)&intent);
    AutoPtr<IPendingIntentHelper> pendingHelper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pendingHelper);
    pendingHelper->GetBroadcast(mContext, 0, intent, 0, (IPendingIntent**)&mImeSwitchPendingIntent);

    mShowOngoingImeSwitcherForPhones = FALSE;

    AutoPtr<IIntentFilter> broadcastFilter;
    CIntentFilter::New((IIntentFilter**)&broadcastFilter);
    broadcastFilter->AddAction(IIntent::ACTION_SCREEN_ON);
    broadcastFilter->AddAction(IIntent::ACTION_SCREEN_OFF);
    broadcastFilter->AddAction(IIntent::ACTION_CLOSE_SYSTEM_DIALOGS);
    broadcastFilter->AddAction(IIntent::ACTION_USER_ADDED);
    broadcastFilter->AddAction(IIntent::ACTION_USER_REMOVED);
    AutoPtr<IIntent> stickyIntent;
    AutoPtr<ImmsBroadcastReceiver> receiver = new ImmsBroadcastReceiver();
    receiver->constructor(this);
    mContext->RegisterReceiver(receiver.Get(), broadcastFilter, (IIntent**)&stickyIntent);

    mNotificationShown = FALSE;
    Int32 userId = 0;
    // try {
    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    AutoPtr<IIUserSwitchObserver> observer;
    CInputMethodManagerServiceUserSwitchObserver::New(this, (IIUserSwitchObserver**)&observer);
    am->RegisterUserSwitchObserver(observer);
    AutoPtr<IUserInfo> user;
    am->GetCurrentUser((IUserInfo**)&user);
    user->GetId(&userId);
    // } catch (RemoteException e) {
    //     Slogger::W(TAG, "Couldn't get current user ID; guessing it's 0", e);
    // }
    AutoPtr<IUserHandle> allUserHande;
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    helper->GetALL((IUserHandle**)&allUserHande);
    mMyPackageMonitor->Register(mContext, NULL, allUserHande, TRUE);

    // mSettings should be created before buildInputMethodListLocked
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    CInputMethodSettings::New(mRes, resolver, mMethodMap, mMethodList, userId, (IInputMethodSettings**)&mSettings);
    UpdateCurrentProfileIds();
    mFileManager = new InputMethodFileManager(mMethodMap, userId, this);

    {    AutoLock syncLock(mMethodMap.Get());
        AutoPtr<IInputMethodSubtypeSwitchingControllerHelper> helper;
        CInputMethodSubtypeSwitchingControllerHelper::AcquireSingleton(
            (IInputMethodSubtypeSwitchingControllerHelper**)&helper);
        helper->CreateInstanceLocked(mSettings, context,
            (IInputMethodSubtypeSwitchingController**)&mSwitchingController);
    }

    // Just checking if defaultImiId is empty or not
    String defaultImiId;
    mSettings->GetSelectedInputMethod(&defaultImiId);
    if (DEBUG) {
        Slogger::D(TAG, "Initial default ime = %s", defaultImiId.string());
    }
    mImeSelectedOnBoot = !TextUtils::IsEmpty(defaultImiId);

    {    AutoLock syncLock(mMethodMap.Get());
        BuildInputMethodListLocked(mMethodList, mMethodMap,
            !mImeSelectedOnBoot /* resetDefaultEnabledIme */);
    }
    mSettings->EnableAllIMEsIfThereIsNoEnabledIME();

   if (!mImeSelectedOnBoot) {
        Slogger::W(TAG, "No IME selected. Choose the most applicable IME.");
        {    AutoLock syncLock(mMethodMap.Get());
            ResetDefaultImeLocked(context);
        }
   }
    mSettingsObserver = new SettingsObserver();
    mSettingsObserver->constructor(mHandler, this);
    {    AutoLock syncLock(mMethodMap.Get());
        UpdateFromSettingsLocked(TRUE);
    }

    // IMMS wants to receive Intent.ACTION_LOCALE_CHANGED in order to update the current IME
    // according to the new system locale.
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_LOCALE_CHANGED);
    AutoPtr<CheckReceiver> checkReceiver = new CheckReceiver();
    checkReceiver->constructor(this);

    stickyIntent = NULL;
    return mContext->RegisterReceiver(checkReceiver.Get(), filter, (IIntent**)&stickyIntent);
}

void CInputMethodManagerService::ResetDefaultImeLocked(
    /* [in] */ IContext* context)
{
    AutoPtr<IInputMethodUtils> imUtils;
    CInputMethodUtils::AcquireSingleton((IInputMethodUtils**)&imUtils);
    Boolean bval;

    // Do not reset the default (current) IME when it is a 3rd-party IME
    if (!mCurMethodId.IsNull()) {
        AutoPtr<IInputMethodInfo> method = GetMethodInfoFromMethodMap(mCurMethodId);
        if (method != NULL) {
            imUtils->IsSystemIme(method, &bval);
            if (!bval) return;
        }
    }

    AutoPtr<IInputMethodInfo> defIm;
    AutoPtr<IIterator> it;
    mMethodList->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        IInputMethodInfo* imi = IInputMethodInfo::Probe(obj);
        if (defIm == NULL) {
            imUtils->IsValidSystemDefaultIme(mSystemReady, imi, context, &bval);
            if (bval) {
                defIm = imi;
                String id ;
                imi->GetId(&id);
                Slogger::I(TAG, "Selected default: %s", id.string());
            }
        }
    }

    if (defIm == NULL && (mMethodList->IsEmpty(&bval), !bval)) {
        AutoPtr<IList> list;
        mSettings->GetEnabledInputMethodListLocked((IList**)&list);
        imUtils->GetMostApplicableDefaultIME(list, (IInputMethodInfo**)&defIm);
        String id;
        if (defIm != NULL) defIm->GetId(&id);
        Slogger::I(TAG, "No default found, using %s", id.string());
    }
    if (defIm != NULL) {
        SetSelectedInputMethodAndSubtypeLocked(defIm, NOT_A_SUBTYPE_ID, FALSE);
    }
}

void CInputMethodManagerService::ResetAllInternalStateLocked(
    /* [in] */ Boolean updateOnlyWhenLocaleChanged,
    /* [in] */ Boolean resetDefaultEnabledIme)
{
    if (!mSystemReady) {
        // not system ready
        return;
    }

    AutoPtr<ILocale> newLocale;
    AutoPtr<IConfiguration> config;
    mRes->GetConfiguration((IConfiguration**)&config);
    config->GetLocale((ILocale**)&newLocale);
    Boolean result = FALSE;
    if (!updateOnlyWhenLocaleChanged
            || (newLocale != NULL && !(newLocale->Equals(mLastSystemLocale, &result), result))) {
        if (!updateOnlyWhenLocaleChanged) {
            HideCurrentInputLocked(0, NULL);
            mCurMethodId = NULL;
            UnbindCurrentMethodLocked(TRUE, FALSE);
        }
        if (DEBUG) {
            String str;
            IObject::Probe(newLocale)->ToString(&str);
            Slogger::I(TAG, "Locale has been changed to %s", str.string());
        }
        // InputMethodAndSubtypeListManager should be reset when the locale is changed.
        BuildInputMethodListLocked(mMethodList, mMethodMap, resetDefaultEnabledIme);
        if (!updateOnlyWhenLocaleChanged) {
            String selectedImiId;
            mSettings->GetSelectedInputMethod(&selectedImiId);
            if (selectedImiId.IsNullOrEmpty()) {
                // This is the first time of the user switch and
                // set the current ime to the proper one.
                ResetDefaultImeLocked(mContext);
            }
        }
        else {
            // If the locale is changed, needs to reset the default ime
            ResetDefaultImeLocked(mContext);
        }
        UpdateFromSettingsLocked(TRUE);
        mLastSystemLocale = newLocale;
        if (!updateOnlyWhenLocaleChanged) {
            // try {
            AutoPtr<IInputBindResult> result;
            StartInputInnerLocked((IInputBindResult**)&result);
            // } catch (RuntimeException e) {
            //     Slogger::W(TAG, "Unexpected exception", e);
            // }
        }
    }
}

void CInputMethodManagerService::ResetStateIfCurrentLocaleChangedLocked()
{
    ResetAllInternalStateLocked(
        TRUE /* updateOnlyWhenLocaleChanged */,
        TRUE /* resetDefaultImeLocked */);
}

void CInputMethodManagerService::SwitchUserLocked(
    /* [in] */ Int32 newUserId)
{
    mSettings->SetCurrentUserId(newUserId);
    UpdateCurrentProfileIds();
    // InputMethodFileManager should be reset when the user is changed
    mFileManager = new InputMethodFileManager(mMethodMap, newUserId, this);

    String defaultImiId;
    mSettings->GetSelectedInputMethod(&defaultImiId);
    // For secondary users, the list of enabled IMEs may not have been updated since the
    // callbacks to PackageMonitor are ignored for the secondary user. Here, defaultImiId may
    // not be empty even if the IME has been uninstalled by the primary user.
    // Even in such cases, IMMS works fine because it will find the most applicable
    // IME for that user.
    Boolean initialUserSwitch = TextUtils::IsEmpty(defaultImiId);
    if (DEBUG) {
        Slogger::D(TAG, "Switch user: %d current ime = %s",
            newUserId, defaultImiId.string());
    }
    ResetAllInternalStateLocked(
        FALSE  /* updateOnlyWhenLocaleChanged */,
        initialUserSwitch /* needsToResetDefaultIme */);

    if (initialUserSwitch) {
        AutoPtr<IInputMethodUtils> imUtils;
        CInputMethodUtils::AcquireSingleton((IInputMethodUtils**)&imUtils);
        AutoPtr<IPackageManager> pm;
        mContext->GetPackageManager((IPackageManager**)&pm);
        AutoPtr<IList> list;
        mSettings->GetEnabledInputMethodListLocked((IList**)&list);
        imUtils->SetNonSelectedSystemImesDisabledUntilUsed(pm, list);
    }
}

void CInputMethodManagerService::UpdateCurrentProfileIds()
{
    AutoPtr<IUserManagerHelper> helper;
    CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&helper);
    AutoPtr<IUserManager> um;
    helper->Get(mContext, (IUserManager**)&um);

    Int32 curUid;
    mSettings->GetCurrentUserId(&curUid);
    AutoPtr<IList> profiles; //List<UserInfo>
    um->GetProfiles(curUid, (IList**)&profiles);
    Int32 size;
    profiles->GetSize(&size);
    AutoPtr<ArrayOf<Int32> > currentProfileIds = ArrayOf<Int32>::Alloc(size); // profiles will not be null

    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        profiles->Get(i, (IInterface**)&obj);
        IUserInfo::Probe(obj)->GetId(&(*currentProfileIds)[i]);
    }
    mSettings->SetCurrentProfileIds(currentProfileIds);
}

ECode CInputMethodManagerService::SystemRunning(
    /* [in] */ IIStatusBarService* statusBar)
{
    AutoLock lock(mMethodMap.Get());

    if (DEBUG) {
        Slogger::D(TAG, "--- systemReady");
    }
    if (!mSystemReady) {
        mSystemReady = TRUE;

        // {
        //     //TODO Added by [wanli]: because com.android.provider.settings is not a "system" provider.
        //     String defaultImiId;
        //     mSettings->GetSelectedInputMethod(&defaultImiId);
        //     mImeSelectedOnBoot = !defaultImiId.IsNullOrEmpty();

        //     BuildInputMethodListLocked(mMethodList, mMethodMap, FALSE);
        //     mSettings->EnableAllIMEsIfThereIsNoEnabledIME();

        //     AutoPtr<IInputMethodInfo> info;
        //     if (mImeSelectedOnBoot) {
        //         info = GetMethodInfoFromMethodMap(defaultImiId);
        //     }

        //     if (!mImeSelectedOnBoot || info == NULL) {
        //         // Slogger::W(TAG, "No IME selected. Choose the most applicable IME.");
        //         ResetDefaultImeLocked(mContext);
        //     }

        //     UpdateFromSettingsLocked(TRUE);
        // }

        AutoPtr<IInterface> obj;
        mContext->GetSystemService(IContext::KEYGUARD_SERVICE, (IInterface**)&obj);
        mKeyguardManager = IKeyguardManager::Probe(obj);
        obj = NULL;
        mContext->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&obj);
        mNotificationManager = INotificationManager::Probe(obj);
        mStatusBar = (CStatusBarManagerService*)statusBar;
        if (mStatusBar) {
            mStatusBar->SetIconVisibility(String("ime"), FALSE);
        }
        UpdateImeWindowStatusLocked();
        if (mShowOngoingImeSwitcherForPhones) {
           mWindowManagerService->SetOnHardKeyboardStatusChangeListener(mHardKeyboardListener);
        }
        BuildInputMethodListLocked(mMethodList, mMethodMap,
            !mImeSelectedOnBoot /* resetDefaultEnabledIme */);
        if (!mImeSelectedOnBoot) {
            Slogger::W(TAG, "Reset the default IME as \"Resource\" is ready here.");
            ResetStateIfCurrentLocaleChangedLocked();
            AutoPtr<IInputMethodUtils> imUtils;
            CInputMethodUtils::AcquireSingleton((IInputMethodUtils**)&imUtils);

            AutoPtr<IPackageManager> pm;
            mContext->GetPackageManager((IPackageManager**)&pm);
            AutoPtr<IList> list;
            mSettings->GetEnabledInputMethodListLocked((IList**)&list);
            imUtils->SetNonSelectedSystemImesDisabledUntilUsed(pm, list);
        }
        AutoPtr<IConfiguration> config;
        mRes->GetConfiguration((IConfiguration**)&config);
        config->GetLocale((ILocale**)&mLastSystemLocale);
        //try {
        AutoPtr<IInputBindResult> result;
        StartInputInnerLocked((IInputBindResult**)&result);
        // } catch (RuntimeException e) {
        //     Slogger::W(TAG, "Unexpected exception", e);
        // }
    }
    return NOERROR;
}

void CInputMethodManagerService::SetImeWindowVisibilityStatusHiddenLocked()
{
    mImeWindowVis = 0;
    UpdateImeWindowStatusLocked();
}

void CInputMethodManagerService::RefreshImeWindowVisibilityLocked()
{
    AutoPtr<IConfiguration> conf;
    mRes->GetConfiguration((IConfiguration**)&conf);
    Int32 keyboard = 0;
    conf->GetKeyboard(&keyboard);
    Boolean haveHardKeyboard = keyboard != IConfiguration::KEYBOARD_NOKEYS;
    Int32 hardKeyboardHidden = 0;
    conf->GetHardKeyboardHidden(&hardKeyboardHidden);
    Boolean hardKeyShown = haveHardKeyboard
            && hardKeyboardHidden != IConfiguration::HARDKEYBOARDHIDDEN_YES;

    Boolean inputActive = !IsKeyguardLocked() && (mInputShown || hardKeyShown);
    // We assume the softkeyboard is shown when the input is active as long as the
    // hard keyboard is not shown.
    Boolean inputVisible = inputActive && !hardKeyShown;
    mImeWindowVis = (inputActive ? IInputMethodService::IME_ACTIVE : 0)
            | (inputVisible ? IInputMethodService::IME_VISIBLE : 0);
    UpdateImeWindowStatusLocked();
}

void CInputMethodManagerService::UpdateImeWindowStatusLocked()
{
    SetImeWindowStatus(mCurToken, mImeWindowVis, mBackDisposition);
}

Boolean CInputMethodManagerService::CalledFromValidUser()
{
    const Int32 uid = Binder::GetCallingUid();
    const Int32 userId = UserHandle::GetUserId(uid);
    if (DEBUG) {
        Int32 curUid;
        mSettings->GetCurrentUserId(&curUid);
        Slogger::D(TAG, "--- calledFromForegroundUserOrSystemProcess ? calling uid = %d"
            " system uid = %d calling userId = %d, foreground user id = %d, calling pid = %d",
            uid, IProcess::SYSTEM_UID, userId, curUid, Binder::GetCallingPid());
    }

    Boolean bval;
    if (uid == IProcess::SYSTEM_UID || (mSettings->IsCurrentProfile(userId, &bval), bval)) {
        return TRUE;
    }

    // Caveat: A process which has INTERACT_ACROSS_USERS_FULL gets results for the
    // foreground user, not for the user of that process. Accordingly InputMethodManagerService
    // must not manage background users' states in any functions.
    // Note that privacy-sensitive IPCs, such as setInputMethod, are still securely guarded
    // by a token.
    Int32 per = 0;
    mContext->CheckCallingOrSelfPermission(Manifest::permission::INTERACT_ACROSS_USERS_FULL, &per);
    if (per == IPackageManager::PERMISSION_GRANTED) {
       if (DEBUG) {
           Slogger::D(TAG, "--- Access granted because the calling process has "
                "the INTERACT_ACROSS_USERS_FULL permission");
       }
       return TRUE;
    }

    Slogger::W(TAG, "--- IPC called from background users. Ignore. \n");
    return FALSE;
}

Boolean CInputMethodManagerService::CalledWithValidToken(
    /* [in] */ IBinder* token)
{
    if (token == NULL || mCurToken.Get() != token) {
        return FALSE;
    }
    return TRUE;
}

Boolean CInputMethodManagerService::BindCurrentInputMethodService(
    /* [in] */ IIntent* service,
    /* [in] */ IServiceConnection* conn,
    /* [in] */ Int32 flags)
{
    if (service == NULL || conn == NULL) {
        Slogger::E(TAG, "--- bind failed: service = %p, conn = %p", service, conn);
        return FALSE;
    }
    Boolean succeeded = FALSE;
    Int32 uid;
    mSettings->GetCurrentUserId(&uid);
    AutoPtr<IUserHandle> uh;
    CUserHandle::New(uid, (IUserHandle**)&uh);
    mContext->BindServiceAsUser(service, conn, flags, uh, &succeeded);
    return succeeded;
}

ECode CInputMethodManagerService::GetInputMethodList(
    /* [out] */ IList** infos)
{
    VALIDATE_NOT_NULL(infos);
    *infos = NULL;

    // TODO: Make this work even for non-current users?
    if (!CalledFromValidUser()) {
        return NOERROR;
    }

    AutoLock lock(mMethodMap.Get());
    return CParcelableList::New(IList::Probe(mMethodList), infos);
}

ECode CInputMethodManagerService::GetEnabledInputMethodList(
    /* [out] */ IList** infos)
{
    VALIDATE_NOT_NULL(infos);
    *infos = NULL;

    // TODO: Make this work even for non-current users?
    if (!CalledFromValidUser()) {
        AutoPtr<ICollections> collections;
        CCollections::AcquireSingleton((ICollections**)&collections);
        return collections->GetEmptyList(infos);
    }

    AutoLock lock(mMethodMap.Get());
    return mSettings->GetEnabledInputMethodListLocked(infos);
}

ECode CInputMethodManagerService::GetEnabledInputMethodSubtypeList(
    /* [in] */ const String& imiId,
    /* [in] */ Boolean allowsImplicitlySelectedSubtypes,
    /* [out] */ IList** infos)
{
    VALIDATE_NOT_NULL(infos);
    *infos = NULL;

    // TODO: Make this work even for non-current users?
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    if (!CalledFromValidUser()) {
        return collections->GetEmptyList(infos);
    }

    AutoLock lock(mMethodMap.Get());

    String id;
    if (imiId == NULL && mCurMethodId != NULL) {
        id = mCurMethodId;
    }
    else {
        id = imiId;
    }

    AutoPtr<IInputMethodInfo> imi = GetMethodInfoFromMethodMap(id);
    if (imi == NULL) {
        return collections->GetEmptyList(infos);
    }

    return mSettings->GetEnabledInputMethodSubtypeListLocked(
        mContext, imi, allowsImplicitlySelectedSubtypes, infos);
}

ECode CInputMethodManagerService::AddClient(
    /* [in] */ IInputMethodClient* client,
    /* [in] */ IIInputContext* inputContext,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid)
{
    if (!CalledFromValidUser()) {
        return NOERROR;
    }
    AutoLock lock(mMethodMap.Get());
    mClients[IBinder::Probe(client)] = new ClientState(client, inputContext, uid, pid);
    return NOERROR;
}

ECode CInputMethodManagerService::RemoveClient(
    /* [in] */ IInputMethodClient* client)
{
    if (!CalledFromValidUser()) {
        return NOERROR;
    }
    AutoLock lock(mMethodMap.Get());

    AutoPtr<IBinder> key = IBinder::Probe(client);

    HashMap<AutoPtr<IBinder>, AutoPtr<ClientState> >::Iterator it = mClients.Find(key);
    if (it != mClients.End()) {
        AutoPtr<ClientState> cs = it->mSecond;
        if (cs != NULL) {
            ClearClientSessionLocked(cs);
        }
        mClients.Erase(it);
    }
    return NOERROR;
}

ECode CInputMethodManagerService::ExecuteOrSendMessage(
    /* [in] */ IInterface* target,
    /* [in] */ IMessage* msg)
{
    IBinder* binder = IBinder::Probe(target);
    if (binder) {
        mCaller->SendMessage(msg);
    }
    else {
        Boolean result;
        HandleMessage(msg, &result);
        msg->Recycle();
    }
    return NOERROR;
}

void CInputMethodManagerService::UnbindCurrentClientLocked()
{
    if (mCurClient != NULL) {
        if (DEBUG) Slogger::V(TAG, "unbindCurrentInputLocked: client = %p", mCurClient->mClient.Get());
        if (mBoundToMethod) {
            mBoundToMethod = FALSE;
            if (mCurMethod != NULL) {
                AutoPtr<IMessage> msg;
                mCaller->ObtainMessageO(MSG_UNBIND_INPUT, mCurMethod.Get(), (IMessage**)&msg);
                ExecuteOrSendMessage(mCurMethod, msg);
            }
        }

        AutoPtr<IMessage> msg1;
        mCaller->ObtainMessageIO(MSG_SET_ACTIVE, 0, TO_IINTERFACE(mCurClient), (IMessage**)&msg1);
        ExecuteOrSendMessage(mCurClient->mClient, msg1);

        AutoPtr<IMessage> msg2;
        mCaller->ObtainMessageIO(MSG_UNBIND_METHOD, mCurSeq, mCurClient->mClient.Get(), (IMessage**)&msg2);
        ExecuteOrSendMessage(mCurClient->mClient, msg2);

        mCurClient->mSessionRequested = FALSE;
        mCurClient = NULL;

        HideInputMethodMenuLocked();
    }
}

Int32 CInputMethodManagerService::GetImeShowFlags()
{
    Int32 flags = 0;
    if (mShowForced) {
        flags |= IInputMethod::SHOW_FORCED | IInputMethod::SHOW_EXPLICIT;
    }
    else if (mShowExplicitlyRequested) {
        flags |= IInputMethod::SHOW_EXPLICIT;
    }
    return flags;
}

Int32 CInputMethodManagerService::GetAppShowFlags()
{
    Int32 flags = 0;
    if (mShowForced) {
        flags |= IInputMethodManager::SHOW_FORCED;
    }
    else if (!mShowExplicitlyRequested) {
        flags |= IInputMethodManager::SHOW_IMPLICIT;
    }
    return flags;
}

ECode CInputMethodManagerService::AttachNewInputLocked(
    /* [in] */ Boolean initial,
    /* [out] */ IInputBindResult** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (!mBoundToMethod) {
        AutoPtr<IMessage> msg;
        mCaller->ObtainMessageOO(MSG_BIND_INPUT,
            mCurMethod.Get(), mCurClient->mBinding.Get(),
            (IMessage**)&msg);
        ExecuteOrSendMessage(mCurMethod, msg);
        mBoundToMethod = TRUE;
    }

    SessionState* session = mCurClient->mCurSession.Get();
    if (initial) {
        AutoPtr<IMessage> msg;
        mCaller->ObtainMessageOOO(MSG_START_INPUT,
            TO_IINTERFACE(session), mCurInputContext.Get(), mCurAttribute.Get(),
            (IMessage**)&msg);
        ExecuteOrSendMessage(session->mMethod, msg);
    }
    else {
        AutoPtr<IMessage> msg;
        mCaller->ObtainMessageOOO(MSG_RESTART_INPUT,
            TO_IINTERFACE(session), mCurInputContext.Get(), mCurAttribute.Get(),
            (IMessage**)&msg);
        ExecuteOrSendMessage(session->mMethod, msg);
    }

    if (mShowRequested) {
        if (DEBUG) Slogger::V(TAG, "Attach new input asks to show input");
        ShowCurrentInputLocked(GetAppShowFlags(), NULL);
    }

    AutoPtr<IInputChannel> channel;
    if (session->mChannel != NULL) {
        session->mChannel->Dup((IInputChannel**)&channel);
    }

    return CInputBindResult::New(session->mSession, channel,
        mCurId, mCurSeq, mCurUserActionNotificationSequenceNumber, result);
}

ECode CInputMethodManagerService::StartInputLocked(
    /* [in] */ IInputMethodClient* client,
    /* [in] */ IIInputContext* inputContext,
    /* [in] */ IEditorInfo* attribute,
    /* [in] */ Int32 controlFlags,
    /* [out] */ IInputBindResult** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    // If no method is currently selected, do nothing.
    if (mCurMethodId.IsNull()) {
        *result = mNoBinding;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    AutoPtr<ClientState> cs;
    HashMap<AutoPtr<IBinder>, AutoPtr<ClientState> >::Iterator it = mClients.Find(IBinder::Probe(client));
    if (it != mClients.End()) {
        cs = it->mSecond;
    }
    if (cs == NULL) {
        // throw new IllegalArgumentException("unknown client "
        //         + client.asBinder());
        Slogger::E(TAG, "IllegalArgumentException unknown client : %p", client);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // try{
    Boolean ret = FALSE;
    if (mIWindowManager->InputMethodClientHasFocus(cs->mClient, &ret), !ret) {
        // Check with the window manager to make sure this client actually
        // has a window with focus.  If not, reject.  This is thread safe
        // because if the focus changes some time before or after, the
        // next client receiving focus that has any interest in input will
        // be calling through here after that change happens.
        Slogger::W(TAG, "Starting input on non-focused client %p (uid=%d pid=%d)",
            cs->mClient.Get(), cs->mUid, cs->mPid);
        return NOERROR;
    }
    // } catch(RemoteException e) {
    // }

    return StartInputUncheckedLocked(cs, inputContext, attribute, controlFlags, result);
}

ECode CInputMethodManagerService::StartInputUncheckedLocked(
    /* [in] */ ClientState* cs,
    /* [in] */ IIInputContext* inputContext,
    /* [in] */ IEditorInfo* attribute,
    /* [in] */ Int32 controlFlags,
    /* [out] */ IInputBindResult** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    // If no method is currently selected, do nothing.
    if (mCurMethodId.IsNull()) {
        *result = mNoBinding;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    if (mCurClient.Get() != cs) {
        // Was the keyguard locked when switching over to the new client?
        mCurClientInKeyguard = IsKeyguardLocked();

        // If the client is changing, we need to switch over to the new
        // one.
        UnbindCurrentClientLocked();
        if (DEBUG) Slogger::V(TAG, "switching to client: client = %s keyguard=%d",
            TO_CSTR(cs), mCurClientInKeyguard);

        // If the screen is on, inform the new client it is active
        if (mScreenOn) {
            AutoPtr<IMessage> msg;
            mCaller->ObtainMessageIO(MSG_SET_ACTIVE, mScreenOn ? 1 : 0,
                TO_IINTERFACE(cs), (IMessage**)&msg);
            ExecuteOrSendMessage(cs->mClient, msg);
        }
    }

    // Bump up the sequence for this client and attach it.
    mCurSeq++;
    if (mCurSeq <= 0) mCurSeq = 1;
    mCurClient = cs;
    mCurInputContext = inputContext;
    mCurAttribute = attribute;

    // Check if the input method is changing.
    if (!mCurId.IsNull() && mCurId.Equals(mCurMethodId)) {
        if (cs->mCurSession != NULL) {
            // Fast case: if we are already connected to the input method,
            // then just return it.
            return AttachNewInputLocked(
                    (controlFlags & IInputMethodManager::CONTROL_START_INITIAL) != 0, result);
        }
        if (mHaveConnection) {
            if (mCurMethod != NULL) {
                // Return to client, and we will get back with it when
                // we have had a session made for it.
                RequestClientSessionLocked(cs);
                return CInputBindResult::New(NULL, NULL, mCurId, mCurSeq,
                    mCurUserActionNotificationSequenceNumber, result);
            }
            else if (SystemClock::GetUptimeMillis() < (mLastBindTime + TIME_TO_RECONNECT)) {
                // In this case we have connected to the service, but
                // don't yet have its interface.  If it hasn't been too
                // long since we did the connection, we'll return to
                // the client and wait to get the service interface so
                // we can report back.  If it has been too long, we want
                // to fall through so we can try a disconnect/reconnect
                // to see if we can get back in touch with the service.
                return CInputBindResult::New(NULL, NULL, mCurId, mCurSeq,
                    mCurUserActionNotificationSequenceNumber, result);
            }
            else {
                // EventLog.writeEvent(EventLogTags.IMF_FORCE_RECONNECT_IME,
                //         mCurMethodId, SystemClock::UptimeMillis()-mLastBindTime, 0);
            }
        }
    }

    ECode ec = StartInputInnerLocked(result);
    if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        Slogger::W(TAG, "Unexpected exception");
        *result = NULL;
        return ec;
    }

    return NOERROR;
}

ECode CInputMethodManagerService::StartInputInnerLocked(
    /* [out] */ IInputBindResult** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (mCurMethodId.IsNull()) {
        *result = mNoBinding;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    if (!mSystemReady) {
        // If the system is not yet ready, we shouldn't be running third
        // party code.
        return CInputBindResult::New(NULL, NULL, mCurMethodId, mCurSeq,
            mCurUserActionNotificationSequenceNumber, result);
    }

    AutoPtr<IInputMethodInfo> info = GetMethodInfoFromMethodMap(mCurMethodId);
    if (info == NULL) {
        //throw new IllegalArgumentException("Unknown id: " + mCurMethodId);
        Slogger::E(TAG, "Unknown id: %s", mCurMethodId.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    UnbindCurrentMethodLocked(FALSE, TRUE);

    mCurIntent = NULL;
    CIntent::New(IInputMethod::SERVICE_INTERFACE, (IIntent**)&mCurIntent);
    AutoPtr<IComponentName> component;
    info->GetComponent((IComponentName**)&component);
    mCurIntent->SetComponent(component);
    mCurIntent->PutExtra(IIntent::EXTRA_CLIENT_LABEL, R::string::input_method_binding_label);
    AutoPtr<IIntent> intentPara;
    CIntent::New(ISettings::ACTION_INPUT_METHOD_SETTINGS, (IIntent**)&intentPara);
    AutoPtr<IPendingIntent> pIntentPara;
    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    helper->GetActivity(mContext, 0, intentPara, 0, (IPendingIntent**)&pIntentPara);
    AutoPtr<IParcelable> p = IParcelable::Probe(pIntentPara);

    mCurIntent->PutExtra(IIntent::EXTRA_CLIENT_INTENT, p);
    if (BindCurrentInputMethodService(mCurIntent, this,
        IContext::BIND_AUTO_CREATE | IContext::BIND_NOT_VISIBLE
        | IContext::BIND_NOT_FOREGROUND | IContext::BIND_SHOWING_UI)) {
        mLastBindTime = SystemClock::GetUptimeMillis();
        mHaveConnection = TRUE;
        info->GetId(&mCurId);
        mCurToken = NULL;
        CBinder::New((IBinder**)&mCurToken);
        // try {
        if (DEBUG) Slogger::V(TAG, "Adding window token: %p", mCurToken.Get());
        mIWindowManager->AddWindowToken(mCurToken, IWindowManagerLayoutParams::TYPE_INPUT_METHOD);
        // } catch (RemoteException e) {
        // }
        return CInputBindResult::New(NULL, NULL, mCurId, mCurSeq,
            mCurUserActionNotificationSequenceNumber, result);
    }
    else {
        mCurIntent = NULL;
        Slogger::W(TAG, "Failure connecting to input method service: ");
    }

    return NOERROR;
}

ECode CInputMethodManagerService::StartInput(
    /* [in] */ IInputMethodClient* client,
    /* [in] */ IIInputContext* inputContext,
    /* [in] */ IEditorInfo* attribute,
    /* [in] */ Int32 controlFlags,
    /* [out] */ IInputBindResult** res)
{
    VALIDATE_NOT_NULL(res);
    *res = NULL;

    if (!CalledFromValidUser()) {
        return NOERROR;
    }
    AutoLock lock(mMethodMap.Get());

    Int64 ident = Binder::ClearCallingIdentity();
    ECode ec = StartInputLocked(client, inputContext, attribute, controlFlags, res);
    Binder::RestoreCallingIdentity(ident);
    return ec;
}

ECode CInputMethodManagerService::FinishInput(
    /* [in] */ IInputMethodClient* client)
{
    return NOERROR;
}

ECode CInputMethodManagerService::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    AutoLock lock(mMethodMap.Get());

    if (mCurIntent != NULL) {
        AutoPtr<IComponentName> component;
        mCurIntent->GetComponent((IComponentName**)&component);
        if (Object::Equals(name, component)) {
            mCurMethod = IIInputMethod::Probe(service);
            if (mCurToken == NULL) {
                Slogger::W(TAG, "Service connected without a token!");
                UnbindCurrentMethodLocked(FALSE, FALSE);
                return NOERROR;
            }

            if (DEBUG) {
                String info;
                name->ToString(&info);
                Slogger::V(TAG, "Initiating attach with token: %p, name: %s", mCurToken.Get(), info.string());
            }

            AutoPtr<IMessage> msg;
            mCaller->ObtainMessageOO(MSG_ATTACH_TOKEN, mCurMethod.Get(), mCurToken.Get(), (IMessage**)&msg);
            ExecuteOrSendMessage(mCurMethod, msg);

            if (mCurClient != NULL) {
                ClearClientSessionLocked(mCurClient);
                RequestClientSessionLocked(mCurClient);
            }
        }
    }
    return NOERROR;
}

ECode CInputMethodManagerService::OnSessionCreated(
    /* [in] */ IIInputMethod* method,
    /* [in] */ IIInputMethodSession* session,
    /* [in] */ IInputChannel* channel)
{
    AutoLock lock(mMethodMap.Get());

    if (mCurMethod != NULL && method != NULL
            && IBinder::Probe(mCurMethod) == IBinder::Probe(method)) {
        if (mCurClient != NULL) {
            ClearClientSessionLocked(mCurClient);
            mCurClient->mCurSession = new SessionState(mCurClient, method, session, channel);
            mCurClient->mSessionRequested = FALSE;

            AutoPtr<IInputBindResult> res;
            AttachNewInputLocked(TRUE, (IInputBindResult**)&res);
            AutoPtr<IIInputMethodSession> m;
            res->GetMethod((IIInputMethodSession**)&m);
            if (m != NULL) {
                AutoPtr<IMessage> msg;
                mCaller->ObtainMessageOO(MSG_BIND_METHOD,
                    mCurClient->mClient.Get(), res.Get(), (IMessage**)&msg);
                ExecuteOrSendMessage(mCurClient->mClient.Get(), msg);
            }
            return NOERROR;
        }
    }

    // Session abandoned.  Close its associated input channel.
    channel->Dispose();
    return NOERROR;
}

ECode CInputMethodManagerService::UnbindCurrentMethodLocked(
    /* [in] */ Boolean reportToClient,
    /* [in] */ Boolean savePosition)
{
    if (mVisibleBound) {
        mContext->UnbindService(mVisibleConnection);
        mVisibleBound = FALSE;
    }

    if (mHaveConnection) {
        mContext->UnbindService(this);
        mHaveConnection = FALSE;
    }

    if (mCurToken != NULL) {
        // try {
        if (DEBUG) Slogger::V(TAG, "Removing window token: %p", mCurToken.Get());
        if ((mImeWindowVis & IInputMethodService::IME_ACTIVE) != 0 && savePosition) {
            // The current IME is shown. Hence an IME switch (transition) is happening.
            mWindowManagerService->SaveLastInputMethodWindowForTransition();
        }
        mIWindowManager->RemoveWindowToken(mCurToken);
        // } catch (RemoteException e) {
        // }
        mCurToken = NULL;
    }

    mCurId = NULL;
    ClearCurMethodLocked();

    if (reportToClient && mCurClient != NULL) {
        AutoPtr<IMessage> msg;
        mCaller->ObtainMessageIO(MSG_UNBIND_METHOD,
            mCurSeq, mCurClient->mClient.Get(), (IMessage**)&msg);
        ExecuteOrSendMessage(mCurClient->mClient.Get(), msg);
    }
    return NOERROR;
}

ECode CInputMethodManagerService::RequestClientSessionLocked(
    /* [in] */ ClientState* cs)
{
    if (!cs->mSessionRequested) {
        if (DEBUG) Slogger::V(TAG, "Creating new session for client %s", TO_CSTR(cs));
        AutoPtr<IInputChannelHelper> helper;
        CInputChannelHelper::AcquireSingleton((IInputChannelHelper**)&helper);
        AutoPtr<ArrayOf<IInputChannel*> > channels;
        helper->OpenInputChannelPair(TO_STR(cs), (ArrayOf<IInputChannel*>**)&channels);
        cs->mSessionRequested = TRUE;

        AutoPtr<IIInputSessionCallback> cb;
        CInputMethodManagerServiceMethodCallback::New(
            this, mCurMethod, (*channels)[0], (IIInputSessionCallback**)&cb);

        AutoPtr<IMessage> msg;
        mCaller->ObtainMessageOOO(MSG_CREATE_SESSION,
            mCurMethod.Get(), (*channels)[1], cb.Get(), (IMessage**)&msg);
        ExecuteOrSendMessage(mCurMethod, msg);
    }
    return NOERROR;
}

ECode CInputMethodManagerService::ClearClientSessionLocked(
    /* [in] */ ClientState* cs)
{
    FinishSessionLocked(cs->mCurSession);
    cs->mCurSession = NULL;
    cs->mSessionRequested = FALSE;
    return NOERROR;
}

void CInputMethodManagerService::FinishSessionLocked(
    /* [in] */ SessionState* sessionState)
{
    if (sessionState != NULL) {

        if (sessionState->mSession != NULL) {
            ECode ec = sessionState->mSession->FinishSession();
            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                Slogger::W(TAG, "Session failed to close due to remote exception");
                SetImeWindowVisibilityStatusHiddenLocked();
            }
            sessionState->mSession = NULL;
        }
        if (sessionState->mChannel != NULL) {
            sessionState->mChannel->Dispose();
            sessionState->mChannel = NULL;
        }
    }
}

ECode CInputMethodManagerService::ClearCurMethodLocked()
{
    if (mCurMethod != NULL) {
        HashMap<AutoPtr<IBinder>, AutoPtr<ClientState> >::Iterator it;
        for (it = mClients.Begin(); it != mClients.End(); ++it) {
            AutoPtr<ClientState> cs = it->mSecond;
            ClearClientSessionLocked(cs);
        }

        FinishSessionLocked(mEnabledSession);
        mEnabledSession = NULL;
        mCurMethod = NULL;
    }
    if (mStatusBar != NULL) {
        mStatusBar->SetIconVisibility(String("ime"), FALSE);
    }
    return NOERROR;
}

ECode CInputMethodManagerService::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    AutoLock lock(mMethodMap.Get());

    if (DEBUG) {
        String nameStr, curStr;
        name->ToString(&nameStr);
        if (mCurIntent != NULL) {
            AutoPtr<IComponentName> component;
            mCurIntent->GetComponent((IComponentName**)&component);
            component->ToString(&curStr);
        }
        Slogger::V(TAG, "Service disconnected: %s mCurIntent=%s", nameStr.string(), curStr.string());
    }

    if (mCurMethod != NULL && mCurIntent != NULL) {
        AutoPtr<IComponentName> component;
        mCurIntent->GetComponent((IComponentName**)&component);
        if (Object::Equals(name, component)) {
            ClearCurMethodLocked();
            // We consider this to be a new bind attempt, since the system
            // should now try to restart the service for us.
            mLastBindTime = SystemClock::GetUptimeMillis();
            mShowRequested = mInputShown;
            mInputShown = FALSE;
            if (mCurClient != NULL) {
                AutoPtr<IMessage> msg;
                mCaller->ObtainMessageIO(MSG_UNBIND_METHOD,
                    mCurSeq, mCurClient->mClient.Get(), (IMessage**)&msg);
                ExecuteOrSendMessage(mCurClient->mClient, msg);
            }
        }
    }
    return NOERROR;
}

ECode CInputMethodManagerService::UpdateStatusIcon(
    /* [in] */ IBinder* token,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 iconId)
{
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    ECode ec = NOERROR;
    {
        AutoLock lock(mMethodMap.Get());

        if (!CalledWithValidToken(token)) {
            Int32 uid = Binder::GetCallingUid();
            Slogger::E(TAG, "Ignoring updateStatusIcon due to an invalid token. uid:%d token:%s",
                uid, TO_CSTR(token));
            return NOERROR;
        }

        String ime("ime");
        if (iconId == 0) {
            if (DEBUG) Slogger::D(TAG, "hide the small icon for the input method");
            if (mStatusBar != NULL) {
                ec = mStatusBar->SetIconVisibility(ime, FALSE);
            }
        }
        else if (!packageName.IsNull()) {
            if (DEBUG) Slogger::D(TAG, "show a small icon for the input method");
            AutoPtr<ICharSequence> contentDescription;
            // try {
            // Use PackageManager to load label
            AutoPtr<IPackageManager> packageManager;
            mContext->GetPackageManager((IPackageManager**)&packageManager);
            Int32 curUserId;
            mSettings->GetCurrentUserId(&curUserId);
            AutoPtr<IApplicationInfo> appInfo;
            mIPackageManager->GetApplicationInfo(packageName, 0, curUserId, (IApplicationInfo**)&appInfo);
            packageManager->GetApplicationLabel(appInfo, (ICharSequence**)&contentDescription);
            // } catch (RemoteException e) {
            //     /* ignore */
            // }

            if (mStatusBar != NULL) {
                String des;
                if (contentDescription != NULL) {
                    contentDescription->ToString(&des);
                }
                mStatusBar->SetIcon(ime, packageName, iconId, 0, des);
                ec = mStatusBar->SetIconVisibility(ime, TRUE);
            }
        }
    }
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
    Binder::RestoreCallingIdentity(ident);
    return ec;
}

Boolean CInputMethodManagerService::NeedsToShowImeSwitchOngoingNotification()
{
    if (!mShowOngoingImeSwitcherForPhones) return FALSE;
    if (mSwitchingDialog != NULL) return FALSE;
    if (IsScreenLocked()) return FALSE;

    AutoLock lock(mMethodMap.Get());
    Int32 nonAuxCount = 0;
    Int32 auxCount = 0;
    AutoPtr<IInputMethodSubtype> nonAuxSubtype;
    AutoPtr<IInputMethodSubtype> auxSubtype;

    AutoPtr<IList> imis;// List<InputMethodInfo>;
    mSettings->GetEnabledInputMethodListLocked((IList**)&imis);
    if (imis != NULL) {
        Int32 N;
        imis->GetSize(&N);
        if (N > 2) return TRUE;
        if (N < 1) return FALSE;


        for(Int32 i = 0; i < N; ++i) {
            AutoPtr<IInterface> obj;
            imis->Get(i, (IInterface**)&obj);
            AutoPtr<IInputMethodInfo> imi = IInputMethodInfo::Probe(obj);
            AutoPtr<IList> subtypes; //List<InputMethodSubtype>
            mSettings->GetEnabledInputMethodSubtypeListLocked(
                mContext, imi, TRUE, (IList**)&subtypes);
            Int32 subtypeCount;
            subtypes->GetSize(&subtypeCount);
            if (subtypeCount == 0) {
                ++nonAuxCount;
            }
            else {
                for (int j = 0; j < subtypeCount; ++j) {
                    AutoPtr<IInterface> typeObj;
                    subtypes->Get(j, (IInterface**)&typeObj);
                    AutoPtr<IInputMethodSubtype> subtype = IInputMethodSubtype::Probe(typeObj);
                    Boolean auxil = FALSE;
                    if (subtype->IsAuxiliary(&auxil), !auxil) {
                        ++nonAuxCount;
                        nonAuxSubtype = subtype;
                    }
                    else {
                        ++auxCount;
                        auxSubtype = subtype;
                    }
                }
            }
        }
    }

    if (nonAuxCount > 1 || auxCount > 1) {
        return TRUE;
    }
    else if (nonAuxCount == 1 && auxCount == 1) {
        if (nonAuxSubtype != NULL && auxSubtype != NULL) {
            String nonLocale, auxLocale;
            nonAuxSubtype->GetLocale(&nonLocale);
            auxSubtype->GetLocale(&auxLocale);
            Boolean auxOverrides = FALSE, nonOverrides = FALSE, contains = FALSE;
            if ((nonLocale.Equals(auxLocale)
                    || (auxSubtype->OverridesImplicitlyEnabledSubtype(&auxOverrides), auxOverrides)
                    || (nonAuxSubtype->OverridesImplicitlyEnabledSubtype(&nonOverrides), nonOverrides))
                    && (nonAuxSubtype->ContainsExtraValueKey(TAG_TRY_SUPPRESSING_IME_SWITCHER, &contains), contains)) {
                return FALSE;
            }
        }
        return TRUE;
    }
    return FALSE;
}

Boolean CInputMethodManagerService::IsKeyguardLocked()
{
    Boolean bval = FALSE;
    if (mKeyguardManager != NULL)
        mKeyguardManager->IsKeyguardLocked(&bval);
    return bval;
}

AutoPtr<IInputMethodInfo> CInputMethodManagerService::GetMethodInfoFromMethodMap(
    /* [in] */ const String& inputMethodId)
{
    AutoPtr<ICharSequence> csq = CoreUtils::Convert(inputMethodId);
    AutoPtr<IInterface> valueObj;
    mMethodMap->Get(csq, (IInterface**)&valueObj);
    AutoPtr<IInputMethodInfo> imi = IInputMethodInfo::Probe(valueObj);
    return imi;
}

ECode CInputMethodManagerService::SetImeWindowStatus(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 inVis,
    /* [in] */ Int32 backDisposition)
{
    Int32 vis = inVis;
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    if (!CalledWithValidToken(token)) {
        Int32 uid = Binder::GetCallingUid();
        Slogger::E(TAG, "Ignoring setImeWindowStatus due to an invalid token. uid:%d token:%s",
            uid, TO_CSTR(token));
        return NOERROR;
    }

    {
        AutoLock lock(mMethodMap.Get());

        // apply policy for binder calls
        if (vis != 0 && IsKeyguardLocked() && !mCurClientInKeyguard) {
            vis = 0;
        }

        mImeWindowVis = vis;
        mBackDisposition = backDisposition;
        Boolean iconVisibility = ((vis & (IInputMethodService::IME_ACTIVE)) != 0)
            && (mWindowManagerService->IsHardKeyboardAvailable()
                || (vis & (IInputMethodService::IME_VISIBLE)) != 0);
        Boolean needsToShowImeSwitcher = iconVisibility && NeedsToShowImeSwitchOngoingNotification();
        if (mStatusBar != NULL) {
            mStatusBar->SetImeWindowStatus(token, vis, backDisposition, needsToShowImeSwitcher);
        }

        AutoPtr<IInputMethodInfo> imi = GetMethodInfoFromMethodMap(mCurMethodId);
        if (imi != NULL && needsToShowImeSwitcher) {
            // Used to load label
            AutoPtr<IPackageManager> pm;
            mContext->GetPackageManager((IPackageManager**)&pm);

            AutoPtr<ICharSequence> title, summary;
            mRes->GetText(R::string::select_input_method, (ICharSequence**)&title);

            AutoPtr<IInputMethodUtils> imUtils;
            CInputMethodUtils::AcquireSingleton((IInputMethodUtils**)&imUtils);
            imUtils->GetImeAndSubtypeDisplayName(
                mContext, imi, mCurrentSubtype, (ICharSequence**)&summary);

            AutoPtr<IResources> res;
            mContext->GetResources((IResources**)&res);
            Int32 color;
            res->GetColor(R::color::system_notification_accent_color, &color);
            mImeSwitcherNotification->SetColor(color);
            mImeSwitcherNotification->SetLatestEventInfo(mContext, title, summary, mImeSwitchPendingIntent);
            Boolean bval;
            if (mNotificationManager != NULL
                && (mWindowManagerService->HasNavigationBar(&bval), !bval)) {
                if (DEBUG) {
                    Slogger::D(TAG, "--- show notification:summary = %s", TO_CSTR(summary));
                }
                mNotificationManager->NotifyAsUser(String(NULL), R::string::select_input_method, mImeSwitcherNotification, UserHandle::ALL);
                mNotificationShown = TRUE;
            }
        }
        else {
            if (mNotificationShown && mNotificationManager != NULL) {
                if (DEBUG) {
                    Slogger::D(TAG, "--- hide notification");
                }
                mNotificationManager->CancelAsUser(String(NULL), R::string::select_input_method, UserHandle::ALL);
                mNotificationShown = FALSE;
            }
        }
    }
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    //}
    return NOERROR;
}

ECode CInputMethodManagerService::RegisterSuggestionSpansForNotification(
    /* [in] */ ArrayOf<ISuggestionSpan*>* spans)
{
    if (!CalledFromValidUser()) {
        return NOERROR;
    }
    {
        AutoLock lock(mMethodMap.Get());

        AutoPtr<IInputMethodInfo> currentImi = GetMethodInfoFromMethodMap(mCurMethodId);

        for (Int32 i = 0; i < spans->GetLength(); ++i) {
            AutoPtr<ISuggestionSpan> ss = (*spans)[i];
            String name;
            ss->GetNotificationTargetClassName(&name);
            if (!name.IsNullOrEmpty()) {
                mSecureSuggestionSpans[ss] = currentImi;
            }
        }
    }
    return NOERROR;
}

ECode CInputMethodManagerService::NotifySuggestionPicked(
    /* [in] */ ISuggestionSpan* span,
    /* [in] */ const String& originalString,
    /* [in] */ Int32 index,
    /* [out] */ Boolean* picked)
{
    VALIDATE_NOT_NULL(picked);

    if (!CalledFromValidUser()) {
        *picked = FALSE;
        return NOERROR;
    }

    {
        AutoLock lock(mMethodMap.Get());

        AutoPtr<IInputMethodInfo> targetImi;
        HashMap<AutoPtr<ISuggestionSpan>, AutoPtr<IInputMethodInfo> >::Iterator it =
                mSecureSuggestionSpans.Find(span);
        if(it != mSecureSuggestionSpans.End()) {
            targetImi = it->mSecond;
        }
        // TODO: Do not send the intent if the process of the targetImi is already dead.
        if (targetImi != NULL) {
            AutoPtr< ArrayOf<String> > suggestions;
            span->GetSuggestions((ArrayOf<String>**)&suggestions);
            if (index < 0 || index >= suggestions->GetLength()) {
                *picked = FALSE;
                return NOERROR;
            }
            String className;
            span->GetNotificationTargetClassName(&className);
            AutoPtr<IIntent> intent;
            CIntent::New((IIntent**)&intent);
            // Ensures that only a class in the original IME package will receive the
            // notification.
            String pNname;
            targetImi->GetPackageName(&pNname);
            intent->SetClassName(pNname, className);
            intent->SetAction(ISuggestionSpan::ACTION_SUGGESTION_PICKED);
            intent->PutExtra(ISuggestionSpan::SUGGESTION_SPAN_PICKED_BEFORE, originalString);
            intent->PutExtra(ISuggestionSpan::SUGGESTION_SPAN_PICKED_AFTER, (*suggestions)[index]);
            Int32 code = 0;
            span->GetHashCode(&code);
            intent->PutExtra(ISuggestionSpan::SUGGESTION_SPAN_PICKED_HASHCODE, code);
            Int64 ident = Binder::ClearCallingIdentity();
            // try {
            mContext->SendBroadcastAsUser(intent, UserHandle::CURRENT);
            // } finally {
            Binder::RestoreCallingIdentity(ident);
            // }
            *picked = TRUE;
            return NOERROR;
        }
    }
    *picked = FALSE;
    return NOERROR;
}

void CInputMethodManagerService::UpdateFromSettingsLocked(
    /* [in] */ Boolean enabledMayChange)
{
    UpdateInputMethodsFromSettingsLocked(enabledMayChange);
    UpdateKeyboardFromSettingsLocked();
}

void CInputMethodManagerService::UpdateInputMethodsFromSettingsLocked(
    /* [in] */ Boolean enabledMayChange)
{
    if (enabledMayChange) {
        AutoPtr<IList> enabled;//List<InputMethodInfo>
        mSettings->GetEnabledInputMethodListLocked((IList**)&enabled);
        Int32 curUid, size;
        mSettings->GetCurrentUserId(&curUid);
        enabled->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            // We allow the user to select "disabled until used" apps, so if they
            // are enabling one of those here we now need to make it enabled.
            AutoPtr<IInterface> obj;
            enabled->Get(i, (IInterface**)&obj);
            AutoPtr<IInputMethodInfo> imm = IInputMethodInfo::Probe(obj);
            String pkgName;
            imm->GetPackageName(&pkgName);
            AutoPtr<IApplicationInfo> ai;
            mIPackageManager->GetApplicationInfo(pkgName,
                IPackageManager::GET_DISABLED_UNTIL_USED_COMPONENTS,
                curUid, (IApplicationInfo**)&ai);
            if (ai != NULL) {
                Int32 enabledSetting;
                ai->GetEnabledSetting(&enabledSetting);
                if (enabledSetting == IPackageManager::COMPONENT_ENABLED_STATE_DISABLED_UNTIL_USED) {
                    if (DEBUG) {
                        String id;
                        imm->GetId(&id);
                        Slogger::D(TAG, "Update state(%s): DISABLED_UNTIL_USED -> DEFAULT", id.string());
                    }
                    String bpn;
                    mContext->GetBasePackageName(&bpn);
                    mIPackageManager->SetApplicationEnabledSetting(pkgName,
                        IPackageManager::COMPONENT_ENABLED_STATE_DEFAULT,
                        IPackageManager::DONT_KILL_APP, curUid,
                        bpn);
                }
            }
        }
    }

    // We are assuming that whoever is changing DEFAULT_INPUT_METHOD and
    // ENABLED_INPUT_METHODS is taking care of keeping them correctly in
    // sync, so we will never have a DEFAULT_INPUT_METHOD that is not
    // enabled.
    String id;
    mSettings->GetSelectedInputMethod(&id);
    if (id.IsNullOrEmpty() && ChooseNewDefaultIMELocked()) {
        mSettings->GetSelectedInputMethod(&id);
    }
    if (!id.IsNullOrEmpty()) {
        // try {
        Int32 subtypeId;
        mSettings->GetSelectedInputMethodSubtypeId(id, &subtypeId);
        ECode ec = SetInputMethodLocked(id, subtypeId);
        if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
            mCurMethodId = NULL;
            UnbindCurrentMethodLocked(TRUE, FALSE);
        }
        // } catch (IllegalArgumentException e) {
        //     Slogger::W(TAG, "Unknown input method from prefs: " + id, e);
        //     mCurMethodId = NULL;
        //     UnbindCurrentMethodLocked(TRUE);
        // }
        mShortcutInputMethodsAndSubtypes->Clear();
    }
    else {
        // There is no longer an input method set, so stop any current one.
        mCurMethodId = NULL;
        UnbindCurrentMethodLocked(TRUE, FALSE);
    }

    // code to disable the CM Phone IME switcher with config_show_cmIMESwitcher set = false
    // try {
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Int32 switcher;
    Settings::System::GetInt32(resolver,
            ISettingsSystem::STATUS_BAR_IME_SWITCHER, &switcher);
    mShowOngoingImeSwitcherForPhones = switcher == 1;
    // } catch (SettingNotFoundException e) {
    //     mShowOngoingImeSwitcherForPhones = mRes.getBoolean(
    //     com.android.internal.R.bool.config_show_cmIMESwitcher);
    // }

    // Here is not the perfect place to reset the switching controller. Ideally
    // mSwitchingController and mSettings should be able to share the same state.
    // TODO: Make sure that mSwitchingController and mSettings are sharing the
    // the same enabled IMEs list.
    mSwitchingController->ResetCircularListLocked(mContext);
}

ECode CInputMethodManagerService::UpdateKeyboardFromSettingsLocked()
{
    mSettings->IsShowImeWithHardKeyboardEnabled(&mShowImeWithHardKeyboard);
    Boolean bval;
    if (mSwitchingDialog != NULL
        && mSwitchingDialogTitleView != NULL
        && (IDialog::Probe(mSwitchingDialog)->IsShowing(&bval), bval)) {
        AutoPtr<IView> view;
        mSwitchingDialogTitleView->FindViewById(R::id::hard_keyboard_switch, (IView**)&view);
        ICheckable* hardKeySwitch = ICheckable::Probe(view);
        hardKeySwitch->SetChecked(mShowImeWithHardKeyboard);
    }
    return NOERROR;
}

ECode CInputMethodManagerService::SetInputMethodLocked(
    /* [in] */ const String& id,
    /* [in] */ Int32 subtypeId)
{
    AutoPtr<IInputMethodInfo> info = GetMethodInfoFromMethodMap(id);
    if (info == NULL) {
        Slogger::E(TAG, "Unknown id: %s", id.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mCurClient != NULL && mCurAttribute != NULL) {
        Int32 uid = mCurClient->mUid;
        String packageName;
        mCurAttribute->GetPackageName(&packageName);
        AutoPtr<SystemConfig> systemConfig = SystemConfig::GetInstance();
        AutoPtr<HashSet<String> > imeApps = systemConfig->GetFixedImeApps();
        HashSet<String>::Iterator it = imeApps->Find(packageName);
        if (it != imeApps->End()) {
            AutoPtr<IInputMethodUtils> imUtils;
            CInputMethodUtils::AcquireSingleton((IInputMethodUtils**)&imUtils);
            Boolean bval;
            imUtils->CheckIfPackageBelongsToUid(mAppOpsManager, uid, packageName, &bval);
            if (bval) {
                return NOERROR;
            }
            // TODO: Do we need to lock the input method when the application reported an
            // incorrect package name?
            Slogger::E(TAG, "Ignoring FixedImeApps due to the validation failure. uid=%d package=%s",
                uid, packageName.string());
        }
    }

    // See if we need to notify a subtype change within the same IME.
    if (id.Equals(mCurMethodId)) {
        Int32 subtypeCount = 0;
        info->GetSubtypeCount(&subtypeCount);
        if (subtypeCount <= 0) {
            return NOERROR;
        }
        AutoPtr<IInputMethodSubtype> oldSubtype = mCurrentSubtype;
        AutoPtr<IInputMethodSubtype> newSubtype;
        if (subtypeId >= 0 && subtypeId < subtypeCount) {
            info->GetSubtypeAt(subtypeId, (IInputMethodSubtype**)&newSubtype);
        }
        else {
            // If subtype is NULL, try to find the most applicable one from
            // getCurrentInputMethodSubtype.
            newSubtype = GetCurrentInputMethodSubtypeLocked();
        }
        if (newSubtype == NULL || oldSubtype == NULL) {
            // Slogger::W(TAG, "Illegal subtype state: old subtype = " + oldSubtype
            //         + ", new subtype = " + newSubtype);
            return NOERROR;
        }
        if (newSubtype != oldSubtype) {
            SetSelectedInputMethodAndSubtypeLocked(info, subtypeId, TRUE);
            if (mCurMethod != NULL) {
                // try {
                RefreshImeWindowVisibilityLocked();
                mCurMethod->ChangeInputMethodSubtype(newSubtype);
                // } catch (RemoteException e) {
                //     Slogger::W(TAG, "Failed to call changeInputMethodSubtype");
                // }
            }
        }
        return NOERROR;
    }

    // Changing to a different IME.
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    // Set a subtype to this input method.
    // subtypeId the name of a subtype which will be set.
    SetSelectedInputMethodAndSubtypeLocked(info, subtypeId, FALSE);
    // mCurMethodId should be updated after setSelectedInputMethodAndSubtypeLocked()
    // because mCurMethodId is stored as a history in
    // setSelectedInputMethodAndSubtypeLocked().
    mCurMethodId = id;

    if (ActivityManagerNative::IsSystemReady()) {
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_INPUT_METHOD_CHANGED, (IIntent**)&intent);
        intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
        intent->PutExtra(String("input_method_id"), id);
        AutoPtr<IUserHandleHelper> userHandleHelper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&userHandleHelper);
        AutoPtr<IUserHandle> CURRENT;
        userHandleHelper->GetCURRENT((IUserHandle**)&CURRENT);
        mContext->SendBroadcastAsUser(intent, CURRENT);
    }
    UnbindCurrentClientLocked();
    Binder::RestoreCallingIdentity(ident);
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode CInputMethodManagerService::ShowSoftInput(
    /* [in] */ IInputMethodClient* client,
    /* [in] */ Int32 flags,
    /* [in] */ IResultReceiver* resultReceiver,
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state);
    if (!CalledFromValidUser()) {
        *state = FALSE;
        return NOERROR;
    }
    Int32 uid = Binder::GetCallingUid();
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    {
        AutoLock lock(mMethodMap.Get());

        if (mCurClient == NULL || client == NULL
                || IBinder::Probe(mCurClient->mClient) != IBinder::Probe(client)) {
            // try {
            // We need to check if this is the current client with
            // focus in the window manager, to allow this call to
            // be made before input is started in it.
            Boolean result = FALSE;
            if (mIWindowManager->InputMethodClientHasFocus(client, &result), !result) {
                Slogger::W(TAG, "Ignoring showSoftInput of uid %d : %s", uid, TO_CSTR(client));
                *state = FALSE;
                Binder::RestoreCallingIdentity(ident);
                return NOERROR;
            }
            // } catch (RemoteException e) {
            //     return FALSE;
            // }
        }

        if (DEBUG) Slogger::V(TAG, "Client requesting input be shown");
        *state = ShowCurrentInputLocked(flags, resultReceiver);
    }
    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}

Boolean CInputMethodManagerService::ShowCurrentInputLocked(
    /* [in] */ Int32 flags,
    /* [in] */ IResultReceiver* resultReceiver)
{
    mShowRequested = TRUE;
    if ((flags & IInputMethodManager::SHOW_IMPLICIT) == 0) {
        mShowExplicitlyRequested = TRUE;
    }
    if ((flags & IInputMethodManager::SHOW_FORCED) != 0) {
        mShowExplicitlyRequested = TRUE;
        mShowForced = TRUE;
    }

    if (!mSystemReady) {
        return FALSE;
    }

    Boolean res = FALSE;
    if (mCurMethod != NULL) {
        if (DEBUG) Slogger::D(TAG, "showCurrentInputLocked: mCurToken=%s", TO_CSTR(mCurToken));

        AutoPtr<IMessage> msg;
        mCaller->ObtainMessageIOO(
            MSG_SHOW_SOFT_INPUT, GetImeShowFlags(),
            mCurMethod.Get(), resultReceiver, (IMessage**)&msg);
        ExecuteOrSendMessage(mCurMethod, msg);

        mInputShown = TRUE;
        if (mHaveConnection && !mVisibleBound) {
            BindCurrentInputMethodService(
                mCurIntent, mVisibleConnection,
                IContext::BIND_AUTO_CREATE | IContext::BIND_TREAT_LIKE_ACTIVITY);
            mVisibleBound = TRUE;
        }
        res = TRUE;
    }
    else if (mHaveConnection && SystemClock::GetUptimeMillis() >= (mLastBindTime + TIME_TO_RECONNECT)) {
        // The client has asked to have the input method shown, but
        // we have been sitting here too long with a connection to the
        // service and no interface received, so let's disconnect/connect
        // to try to prod things along.
        // EventLog.writeEvent(EventLogTags.IMF_FORCE_RECONNECT_IME, mCurMethodId,
        //         SystemClock::UptimeMillis()-mLastBindTime,1);
        // Slogger::W(TAG, "Force disconnect/connect to the IME in showCurrentInputLocked()");
        mContext->UnbindService(this);
        BindCurrentInputMethodService(mCurIntent, this, IContext::BIND_AUTO_CREATE
                    | IContext::BIND_NOT_VISIBLE);
    }
    else {
        if (DEBUG) {
            Slogger::D(TAG, "Can't show input: connection = %d, time = %lld", mHaveConnection,
                    ((mLastBindTime + TIME_TO_RECONNECT) - SystemClock::GetUptimeMillis()));
        }
    }

    return res;
}

ECode CInputMethodManagerService::HideSoftInput(
    /* [in] */ IInputMethodClient* client,
    /* [in] */ Int32 flags,
    /* [in] */ IResultReceiver* resultReceiver,
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state);

    if (!CalledFromValidUser()) {
        *state = FALSE;
        return NOERROR;
    }
    Int32 uid = Binder::GetCallingUid();
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    {
        AutoLock lock(mMethodMap.Get());

        if (mCurClient == NULL || client == NULL
                || IBinder::Probe(mCurClient->mClient) != IBinder::Probe(client)) {
            // try {
            // We need to check if this is the current client with
            // focus in the window manager, to allow this call to
            // be made before input is started in it.
            Boolean ret = FALSE;
            mIWindowManager->InputMethodClientHasFocus(client, &ret);
            if (!ret) {
                if (DEBUG) Slogger::W(TAG, "Ignoring hideSoftInput of uid %d: 0x%08x",
                        uid, client);
                SetImeWindowVisibilityStatusHiddenLocked();
                *state = FALSE;
                Binder::RestoreCallingIdentity(ident);
                return NOERROR;
            }
            // } catch (RemoteException e) {
            //     SetImeWindowVisibilityStatusHiddenLocked();
            //     return FALSE;
            // }
        }

        if (DEBUG) Slogger::V(TAG, "Client requesting input be hidden");
        *state = HideCurrentInputLocked(flags, resultReceiver);
    }
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}

Boolean CInputMethodManagerService::HideCurrentInputLocked(
    /* [in] */ Int32 flags,
    /* [in] */ IResultReceiver* resultReceiver)
{
    if ((flags & IInputMethodManager::HIDE_IMPLICIT_ONLY) != 0
            && (mShowExplicitlyRequested || mShowForced)) {
        if (DEBUG) Slogger::V(TAG,
            "Not hiding: explicit show not cancelled by non-explicit hide");
        return FALSE;
    }
    if (mShowForced && (flags & IInputMethodManager::HIDE_NOT_ALWAYS) != 0) {
        if (DEBUG) Slogger::V(TAG,
            "Not hiding: forced show not cancelled by not-always hide");
        return FALSE;
    }
    Boolean res = FALSE;
    if (mInputShown && mCurMethod != NULL) {
        AutoPtr<IMessage> msg;
        mCaller->ObtainMessageOO(MSG_HIDE_SOFT_INPUT,
            mCurMethod, resultReceiver, (IMessage**)&msg);
        ExecuteOrSendMessage(mCurMethod, msg);
        res = TRUE;
    }
    else {
        res = FALSE;
    }
    if (mHaveConnection && mVisibleBound) {
        mContext->UnbindService(mVisibleConnection);
        mVisibleBound = FALSE;
    }
    mInputShown = FALSE;
    mShowRequested = FALSE;
    mShowExplicitlyRequested = FALSE;
    mShowForced = FALSE;
    return res;
}

ECode CInputMethodManagerService::WindowGainedFocus(
    /* [in] */ IInputMethodClient* client,
    /* [in] */ IBinder* windowToken,
    /* [in] */ Int32 controlFlags,
    /* [in] */ Int32 softInputMode,
    /* [in] */ Int32 windowFlags,
    /* [in] */ IEditorInfo* attribute,
    /* [in] */ IIInputContext* inputContext,
    /* [out] */ IInputBindResult** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    // Needs to check the validity before clearing calling identity
    Boolean calledFromValidUser = CalledFromValidUser();

    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    {
        AutoLock lock(mMethodMap.Get());
        if (DEBUG) Slogger::V(TAG, "windowGainedFocus: 0x%08x  controlFlags=#%s  softInputMode=#%s windowFlags=#",
                client, StringUtils::ToHexString(controlFlags).string(), StringUtils::ToHexString(softInputMode).string(),
                StringUtils::ToHexString(windowFlags).string());

        AutoPtr<ClientState> cs;
        HashMap<AutoPtr<IBinder>, AutoPtr<ClientState> >::Iterator it =
                mClients.Find(IBinder::Probe(client));
        if (it != mClients.End()) {
            cs = it->mSecond;
        }
        if (cs == NULL) {
            // throw new IllegalArgumentException("unknown client "
            //         + client.asBinder());
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        // try {
        Boolean hasFocus = FALSE;
        if (!(mIWindowManager->InputMethodClientHasFocus(cs->mClient, &hasFocus), hasFocus)) {
            // Check with the window manager to make sure this client actually
            // has a window with focus.  If not, reject.  This is thread safe
            // because if the focus changes some time before or after, the
            // next client receiving focus that has any interest in input will
            // be calling through here after that change happens.
            // Slogger::W(TAG, "Focus gain on non-focused client " + cs.client
            //         + " (uid=" + cs.uid + " pid=" + cs.pid + ")");
            Binder::RestoreCallingIdentity(ident);
            *result = NULL;
            return NOERROR;
        }
        // } catch (RemoteException e) {
        // }

        if (!calledFromValidUser) {
            // Slogger::W(TAG, "A background user is requesting window. Hiding IME.");
            // Slogger::W(TAG, "If you want to interect with IME, you need "
            //         + "Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS_FULL");
            HideCurrentInputLocked(0, NULL);
            *result = NULL;
            return NOERROR;
        }

        if (mCurFocusedWindow.Get() == windowToken) {
            // Slogger::W(TAG, "Window already focused, ignoring focus gain of: " + client
            //         + " attribute=" + attribute + ", token = " + windowToken);
            if (attribute != NULL) {
                ECode ec = StartInputUncheckedLocked(cs, inputContext, attribute,
                        controlFlags, result);
                Binder::RestoreCallingIdentity(ident);
                return ec;
            }
            Binder::RestoreCallingIdentity(ident);
            *result = NULL;
            return NOERROR;
        }
        mCurFocusedWindow = windowToken;

        // Should we auto-show the IME even if the caller has not
        // specified what should be done with it?
        // We only do this automatically if the window can resize
        // to accommodate the IME (so what the user sees will give
        // them good context without input information being obscured
        // by the IME) or if running on a large screen where th==ere
        // is more room for the target window + IME.
        AutoPtr<IConfiguration> config;
        mRes->GetConfiguration((IConfiguration**)&config);
        Boolean isLayoutSizeAtLeast = FALSE;
        Boolean doAutoShow =
                (softInputMode & IWindowManagerLayoutParams::SOFT_INPUT_MASK_ADJUST)
                        == IWindowManagerLayoutParams::SOFT_INPUT_ADJUST_RESIZE
                || (config->IsLayoutSizeAtLeast(IConfiguration::SCREENLAYOUT_SIZE_LARGE, &isLayoutSizeAtLeast),
                        isLayoutSizeAtLeast);
        Boolean isTextEditor =
                (controlFlags & IInputMethodManager::CONTROL_WINDOW_IS_TEXT_EDITOR) != 0;

        // We want to start input before showing the IME, but after closing
        // it.  We want to do this after closing it to help the IME disappear
        // more quickly (not get stuck behind it initializing itself for the
        // new focused input, even if its window wants to hide the IME).
        Boolean didStart = FALSE;

        switch (softInputMode & IWindowManagerLayoutParams::SOFT_INPUT_MASK_STATE) {
            case IWindowManagerLayoutParams::SOFT_INPUT_STATE_UNSPECIFIED:
                if (!isTextEditor || !doAutoShow) {
                    AutoPtr<IWindowManagerLayoutParamsHelper> wmHelper;
                    CWindowManagerLayoutParamsHelper::AcquireSingleton((IWindowManagerLayoutParamsHelper**)&wmHelper);
                    Boolean mayUse = FALSE;
                    wmHelper->MayUseInputMethod(windowFlags, &mayUse);
                    if (mayUse) {
                        // There is no focus view, and this window will
                        // be behind any soft input window, so hide the
                        // soft input window if it is shown.
                        if (DEBUG) Slogger::V(TAG, "Unspecified window will hide input");
                        HideCurrentInputLocked(IInputMethodManager::HIDE_NOT_ALWAYS, NULL);
                    }
                }
                else if (isTextEditor && doAutoShow && (softInputMode &
                        IWindowManagerLayoutParams::SOFT_INPUT_IS_FORWARD_NAVIGATION) != 0) {
                    // There is a focus view, and we are navigating forward
                    // into the window, so show the input window for the user.
                    // We only do this automatically if the window can resize
                    // to accommodate the IME (so what the user sees will give
                    // them good context without input information being obscured
                    // by the IME) or if running on a large screen where there
                    // is more room for the target window + IME.
                    if (DEBUG) Slogger::V(TAG, "Unspecified window will show input");
                    if (attribute != NULL) {
                        StartInputUncheckedLocked(cs, inputContext, attribute,
                                controlFlags, result);
                        didStart = TRUE;
                    }
                    ShowCurrentInputLocked(IInputMethodManager::SHOW_IMPLICIT, NULL);
                }
                break;
            case IWindowManagerLayoutParams::SOFT_INPUT_STATE_UNCHANGED:
                // Do nothing.
                break;
            case IWindowManagerLayoutParams::SOFT_INPUT_STATE_HIDDEN:
                if ((softInputMode &
                        IWindowManagerLayoutParams::SOFT_INPUT_IS_FORWARD_NAVIGATION) != 0) {
                    if (DEBUG) Slogger::V(TAG, "Window asks to hide input going forward");
                    HideCurrentInputLocked(0, NULL);
                }
                break;
            case IWindowManagerLayoutParams::SOFT_INPUT_STATE_ALWAYS_HIDDEN:
                if (DEBUG) Slogger::V(TAG, "Window asks to hide input");
                HideCurrentInputLocked(0, NULL);
                break;
            case IWindowManagerLayoutParams::SOFT_INPUT_STATE_VISIBLE:
                if ((softInputMode &
                        IWindowManagerLayoutParams::SOFT_INPUT_IS_FORWARD_NAVIGATION) != 0) {
                    if (DEBUG) Slogger::V(TAG, "Window asks to show input going forward");
                    if (attribute != NULL) {
                        StartInputUncheckedLocked(cs, inputContext, attribute,
                                controlFlags, result);
                        didStart = TRUE;
                    }
                    ShowCurrentInputLocked(IInputMethodManager::SHOW_IMPLICIT, NULL);
                }
                break;
            case IWindowManagerLayoutParams::SOFT_INPUT_STATE_ALWAYS_VISIBLE:
                if (DEBUG) Slogger::V(TAG, "Window asks to always show input");
                if (attribute != NULL) {
                    StartInputUncheckedLocked(cs, inputContext, attribute,
                            controlFlags, result);
                    didStart = TRUE;
                }
                ShowCurrentInputLocked(IInputMethodManager::SHOW_IMPLICIT, NULL);
                break;
        }

        if (!didStart && attribute != NULL) {
            StartInputUncheckedLocked(cs, inputContext, attribute,
                    controlFlags, result);
        }
    }
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }

    return NOERROR;
}

ECode CInputMethodManagerService::ShowInputMethodPickerFromClient(
    /* [in] */ IInputMethodClient* client)
{
    if (!CalledFromValidUser()) {
        return NOERROR;
    }
    AutoLock lock(mMethodMap.Get());

    if (mCurClient == NULL || client == NULL
            || IBinder::Probe(mCurClient->mClient) != IBinder::Probe(client)) {
        Slogger::W(TAG, "Ignoring showInputMethodPickerFromClient of uid %d: %p"
                , Binder::GetCallingUid() , client);
    }

    // Always call subtype picker, because subtype picker is a superset of input method
    // picker.
    Boolean result;
    return mHandler->SendEmptyMessage(MSG_SHOW_IM_SUBTYPE_PICKER, &result);
}

ECode CInputMethodManagerService::SetInputMethod(
    /* [in] */ IBinder* token,
    /* [in] */ const String& id)
{
    if (!CalledFromValidUser()) {
        return NOERROR;
    }
    SetInputMethodWithSubtypeIdLocked(token, id, NOT_A_SUBTYPE_ID);
    return NOERROR;
}

ECode CInputMethodManagerService::SetInputMethodAndSubtype(
    /* [in] */ IBinder* token,
    /* [in] */ const String& id,
    /* [in] */ IInputMethodSubtype* subtype)
{
    if (!CalledFromValidUser()) {
        return NOERROR;
    }
    {
        AutoLock lock(mMethodMap.Get());

        if (subtype != NULL) {
            Int32 subtypeId;

            AutoPtr<IInputMethodUtils> imUtils;
            CInputMethodUtils::AcquireSingleton((IInputMethodUtils**)&imUtils);

            AutoPtr<IInputMethodInfo> info = GetMethodInfoFromMethodMap(id);
            Int32 hashCode = Object::GetHashCode(subtype);
            imUtils->GetSubtypeIdFromHashCode(info, hashCode, &subtypeId);
            SetInputMethodWithSubtypeIdLocked(token, id, subtypeId);
        }
        else {
            SetInputMethod(token, id);
        }
    }
    return NOERROR;
}

ECode CInputMethodManagerService::ShowInputMethodAndSubtypeEnablerFromClient(
    /* [in] */ IInputMethodClient* client,
    /* [in] */ const String& inputMethodId)
{
    if (!CalledFromValidUser()) {
        return NOERROR;
    }
    {
        AutoLock lock(mMethodMap.Get());

        if (mCurClient == NULL || client == NULL
            || IBinder::Probe(mCurClient->mClient) != IBinder::Probe(client)) {
            // Slogger::W(TAG, "Ignoring showInputMethodAndSubtypeEnablerFromClient of: " + client);
        }

        AutoPtr<ICharSequence> seq = CoreUtils::Convert(inputMethodId);
        AutoPtr<IMessage> msg;
        mCaller->ObtainMessageO(MSG_SHOW_IM_SUBTYPE_ENABLER,
            seq, (IMessage**)&msg);
        ExecuteOrSendMessage(mCurMethod, msg);
    }

    return NOERROR;
}

ECode CInputMethodManagerService::SwitchToLastInputMethod(
    /* [in] */ IBinder* token,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (!CalledFromValidUser()) {
        return NOERROR;
    }
    {
        AutoLock lock(mMethodMap.Get());

        AutoPtr<IPair> lastIme;
        mSettings->GetLastInputMethodAndSubtypeLocked((IPair**)&lastIme);
        AutoPtr<IInputMethodInfo> lastImi;
        String first, second;
        if (lastIme != NULL) {
            AutoPtr<IInterface> f, s;
            lastIme->GetFirst((IInterface**)&f);
            lastIme->GetSecond((IInterface**)&s);
            first = Object::ToString(f);
            second = Object::ToString(s);
            lastImi = GetMethodInfoFromMethodMap(first);
        }
        else {
            lastImi = NULL;
        }

        AutoPtr<IInputMethodUtils> imUtils;
        CInputMethodUtils::AcquireSingleton((IInputMethodUtils**)&imUtils);
        Boolean bval;
        String targetLastImiId;
        Int32 subtypeId = NOT_A_SUBTYPE_ID;
        if (lastIme != NULL && lastImi != NULL) {
            String id;
            lastImi->GetId(&id);
            Boolean imiIdIsSame = id.Equals(mCurMethodId);
            const Int32 lastSubtypeHash = StringUtils::ParseInt32(second);
            Int32 hashCode = Object::GetHashCode(mCurrentSubtype);
            const Int32 currentSubtypeHash = mCurrentSubtype == NULL ? NOT_A_SUBTYPE_ID
                    : hashCode;
            // If the last IME is the same as the current IME and the last subtype is not
            // defined, there is no need to switch to the last IME.
            if (!imiIdIsSame || lastSubtypeHash != currentSubtypeHash) {
                targetLastImiId = first;
                imUtils->GetSubtypeIdFromHashCode(lastImi, lastSubtypeHash, &subtypeId);
            }
        }

        if (TextUtils::IsEmpty(targetLastImiId)
            && (imUtils->CanAddToLastInputMethod(mCurrentSubtype, &bval), !bval)) {
            // This is a safety net. If the currentSubtype can't be added to the history
            // and the framework couldn't find the last ime, we will make the last ime be
            // the most applicable enabled keyboard subtype of the system imes.
            AutoPtr<IList> enabled;
            mSettings->GetEnabledInputMethodListLocked((IList**)&enabled);
            if (enabled != NULL) {
                String locale;
                if (mCurrentSubtype == NULL) {
                    AutoPtr<IConfiguration> config;
                    mRes->GetConfiguration((IConfiguration**)&config);
                    AutoPtr<ILocale> l;
                    config->GetLocale((ILocale**)&l);
                    l->ToString(&locale);
                }
                else {
                    mCurrentSubtype->GetLocale(&locale);
                }
                AutoPtr<IIterator> ator;
                enabled->GetIterator((IIterator**)&ator);
                Boolean hasNext;
                while (ator->HasNext(&hasNext), hasNext) {
                    AutoPtr<IInterface> obj;
                    ator->GetNext((IInterface**)&obj);
                    AutoPtr<IInputMethodInfo> imi = IInputMethodInfo::Probe(obj);
                    Int32 count = 0;
                    imi->GetSubtypeCount(&count);
                    if (count > 0 && (imUtils->IsSystemIme(imi, &bval), bval)) {
                        AutoPtr<IArrayList> subtypes;
                        imUtils->GetSubtypes(imi, (IArrayList**)&subtypes);
                        AutoPtr<IInputMethodSubtype> keyboardSubtype;
                        imUtils->FindLastResortApplicableSubtypeLocked(mRes,
                            IList::Probe(subtypes), IInputMethodUtils::SUBTYPE_MODE_KEYBOARD,
                            locale, TRUE, (IInputMethodSubtype**)&keyboardSubtype);
                        if (keyboardSubtype != NULL) {
                            imi->GetId(&targetLastImiId);
                            Int32 hashCode = Object::GetHashCode(keyboardSubtype);
                            imUtils->GetSubtypeIdFromHashCode(imi, hashCode, &subtypeId);
                            String keyL;
                            keyboardSubtype->GetLocale(&keyL);
                            if(keyL.Equals(locale)) {
                                break;
                            }
                        }
                    }
                }

                enabled = NULL;
            }
        }

        if (!targetLastImiId.IsNullOrEmpty()) {
            if (DEBUG) {
                String id;
                lastImi->GetId(&id);
                Slogger::D(TAG, "Switch to: %s, %s, from: %s, %d",
                    id.string(), second.string(), mCurMethodId.string(), subtypeId);
            }
            SetInputMethodWithSubtypeIdLocked(token, targetLastImiId, subtypeId);
            *result = TRUE;
        }
    }

    return NOERROR;
}

ECode CInputMethodManagerService::SwitchToNextInputMethod(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean onlyCurrentIme,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (!CalledFromValidUser()) {
        return NOERROR;
    }
    {
        AutoLock lock(mMethodMap.Get());

        if (!CalledWithValidToken(token)) {
            Int32 uid = Binder::GetCallingUid();
            Slogger::E(TAG, "Ignoring switchToNextInputMethod due to an invalid token. uid:%d token:%s",
                uid, TO_CSTR(token));
            return NOERROR;
        }

        AutoPtr<IInputMethodInfo> info = GetMethodInfoFromMethodMap(mCurMethodId);
        AutoPtr<IImeSubtypeListItem> nextSubtype;
        mSwitchingController->GetNextInputMethodLocked(
            onlyCurrentIme, info, mCurrentSubtype, (IImeSubtypeListItem**)&nextSubtype);
        if (nextSubtype == NULL) {
            *result = FALSE;
            return NOERROR;
        }

        AutoPtr<IInputMethodInfo> imi;
        nextSubtype->GetImi((IInputMethodInfo**)&imi);
        String id;
        imi->GetId(&id);
        Int32 subtypeId;
        nextSubtype->GetSubtypeId(&subtypeId);
        SetInputMethodWithSubtypeIdLocked(token, id, subtypeId);
        *result = TRUE;
    }
    return NOERROR;
}

ECode CInputMethodManagerService::ShouldOfferSwitchingToNextInputMethod(
    /* [in] */ IBinder* token,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (!CalledFromValidUser()) {
        return NOERROR;
    }
    {    AutoLock syncLock(mMethodMap.Get());
        if (!CalledWithValidToken(token)) {
            Int32 uid = Binder::GetCallingUid();
            Slogger::E(TAG, "Ignoring shouldOfferSwitchingToNextInputMethod due to an invalid "
                "token. uid:%d token:%s", uid, TO_CSTR(token));
            return NOERROR;
        }

        AutoPtr<IInputMethodInfo> info = GetMethodInfoFromMethodMap(mCurMethodId);
        AutoPtr<IImeSubtypeListItem> nextSubtype;
        mSwitchingController->GetNextInputMethodLocked(
            FALSE /* onlyCurrentIme */, info, mCurrentSubtype,
            (IImeSubtypeListItem**)&nextSubtype);
        if (nextSubtype == NULL) {
            return NOERROR;
        }
        *result = TRUE;
    }
    return NOERROR;
}

ECode CInputMethodManagerService::GetLastInputMethodSubtype(
    /* [out] */ IInputMethodSubtype** subtype)
{
    VALIDATE_NOT_NULL(subtype);
    *subtype = NULL;

    if (!CalledFromValidUser()) {
        return NOERROR;
    }

    {
        AutoLock lock(mMethodMap.Get());

        AutoPtr<IPair> lastIme;
        mSettings->GetLastInputMethodAndSubtypeLocked((IPair**)&lastIme);
        String first, second;
        if (lastIme != NULL) {
            AutoPtr<IInterface> f, s;
            lastIme->GetFirst((IInterface**)&f);
            lastIme->GetSecond((IInterface**)&s);
            first = Object::ToString(f);
            second = Object::ToString(s);
        }
        // TODO: Handle the case of the last IME with no subtypes
        if (lastIme == NULL || TextUtils::IsEmpty(first) || TextUtils::IsEmpty(second)) {
            return NOERROR;
        }

        AutoPtr<IInputMethodInfo> lastImi = GetMethodInfoFromMethodMap(first);
        if (lastImi == NULL) {
            return NOERROR;
        }

        // try {
        AutoPtr<IInputMethodUtils> imUtils;
        CInputMethodUtils::AcquireSingleton((IInputMethodUtils**)&imUtils);

        Int32 lastSubtypeHash = StringUtils::ParseInt32(second);
        Int32 lastSubtypeId;
        imUtils->GetSubtypeIdFromHashCode(lastImi, lastSubtypeHash, &lastSubtypeId);
        Int32 subtypeCount = 0;

        lastImi->GetSubtypeCount(&subtypeCount);
        if (lastSubtypeId < 0 || lastSubtypeId >= subtypeCount) {
            return NOERROR;
        }
        return lastImi->GetSubtypeAt(lastSubtypeId, subtype);
        // } catch (NumberFormatException e) {
            // return NULL;
        // }
    }
    return NOERROR;
}

ECode CInputMethodManagerService::SetAdditionalInputMethodSubtypes(
    /* [in] */ const String& imiId,
    /* [in] */ ArrayOf<IInputMethodSubtype*>* subtypes)
{
    if (!CalledFromValidUser()) {
        return NOERROR;
    }
    // By this IPC call, only a process which shares the same uid with the IME can add

    if (imiId.IsNullOrEmpty() || subtypes == NULL || subtypes->GetLength() == 0) return NOERROR;

    {
        AutoLock lock(mMethodMap.Get());

        AutoPtr<IInputMethodInfo> imi = GetMethodInfoFromMethodMap(imiId);
        if (imi == NULL) return NOERROR;
        AutoPtr<ArrayOf<String> > packageInfos;
        if (FAILED(mIPackageManager->GetPackagesForUid(Binder::GetCallingUid(), (ArrayOf<String>**)&packageInfos)))
        {
            SLOGGERE(TAG, "Failed to get package infos")
            return NOERROR;
        }

        if (packageInfos != NULL) {
            Int32 packageNum = packageInfos->GetLength();
            for (Int32 i = 0; i < packageNum; ++i) {
                String pName;
                imi->GetPackageName(&pName);
                if ((*packageInfos)[i].Equals(pName)) {
                    mFileManager->AddInputMethodSubtypes(imi, subtypes);
                    Int64 ident = Binder::ClearCallingIdentity();
                    // try {
                    BuildInputMethodListLocked(mMethodList, mMethodMap,
                        FALSE /* resetDefaultEnabledIme */);
                    // } finally {
                    Binder::RestoreCallingIdentity(ident);
                    // }
                    return NOERROR;
                }
            }
        }
    }
    return NOERROR;
}


ECode CInputMethodManagerService::GetInputMethodWindowVisibleHeight(
    /* [out] */ Int32* result)
{
    *result = mWindowManagerService->GetInputMethodWindowVisibleHeight();
    return NOERROR;
}

ECode CInputMethodManagerService::NotifyUserAction(
        /* [in] */ Int32 sequenceNumber)
{
    if (DEBUG) {
        Slogger::D(TAG, "Got the notification of a user action. sequenceNumber:%d", sequenceNumber);
    }
    {    AutoLock syncLock(mMethodMap.Get());
        if (mCurUserActionNotificationSequenceNumber != sequenceNumber) {
            if (DEBUG) {
                Slogger::D(TAG, "Ignoring the user action notification due to the sequence number "
                    "mismatch. expected:%d actual: %d",
                    mCurUserActionNotificationSequenceNumber, sequenceNumber);
            }
            return NOERROR;
        }

        AutoPtr<IInputMethodInfo> imi = GetMethodInfoFromMethodMap(mCurMethodId);
        if (imi != NULL) {
            return mSwitchingController->OnUserActionLocked(imi, mCurrentSubtype);
        }
    }
    return NOERROR;
}

ECode CInputMethodManagerService::SetInputMethodWithSubtypeId(
    /* [in] */ IBinder* token,
    /* [in] */ const String& id,
    /* [in] */ Int32 subtypeId)
{
    {    AutoLock syncLock(mMethodMap.Get());
        return SetInputMethodWithSubtypeIdLocked(token, id, subtypeId);
    }
    return NOERROR;
}

ECode CInputMethodManagerService::SetInputMethodWithSubtypeIdLocked(
    /* [in] */ IBinder* token,
    /* [in] */ const String& id,
    /* [in] */ Int32 subtypeId)
{
    AutoLock lock(mMethodMap.Get());

    if (token == NULL) {
        Int32 perm = 0;
        FAIL_RETURN(mContext->CheckCallingOrSelfPermission(
                Elastos::Droid::Manifest::permission::WRITE_SECURE_SETTINGS,
                &perm));
        if (perm != IPackageManager::PERMISSION_GRANTED) {
            // throw new SecurityException(
            //         "Using NULL token requires permission "
            //         + Elastos::Droid::Manifest::permission::WRITE_SECURE_SETTINGS);
            return E_SECURITY_EXCEPTION;
        }
    }
    else if (mCurToken.Get() != token) {
        // Slogger::W(TAG, "Ignoring setInputMethod of uid " + Binder::GetCallingUid()
        //         + " token: " + token);
        return NOERROR;
    }

    Int64 ident = Binder::ClearCallingIdentity();
    ECode ec = SetInputMethodLocked(id, subtypeId);
    Binder::RestoreCallingIdentity(ident);
    return ec;
}

ECode CInputMethodManagerService::HideMySoftInput(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 flags)
{
    if (!CalledFromValidUser()) {
        return NOERROR;
    }

    AutoLock lock(mMethodMap.Get());

    if (!CalledWithValidToken(token)) {
        Int32 uid = Binder::GetCallingUid();
        Slogger::E(TAG, "Ignoring hideInputMethod due to an invalid token. uid:%d token:%s",
            uid, TO_CSTR(token));
        return NOERROR;
    }

    Int64 ident = Binder::ClearCallingIdentity();
    HideCurrentInputLocked(flags, NULL);
    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}

ECode CInputMethodManagerService::ShowMySoftInput(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 flags)
{
    if (!CalledFromValidUser()) {
        return NOERROR;
    }
    AutoLock lock(mMethodMap.Get());

    if (!CalledWithValidToken(token)) {
        Int32 uid = Binder::GetCallingUid();
        Slogger::E(TAG, "Ignoring showMySoftInput due to an invalid token. uid:%d token:%s",
            uid, TO_CSTR(token));
        return NOERROR;
    }

    Int64 ident = Binder::ClearCallingIdentity();
    ShowCurrentInputLocked(flags, NULL);
    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}

void CInputMethodManagerService::SetEnabledSessionInMainThread(
    /* [in] */ SessionState* session)
{
    if (mEnabledSession.Get() != session) {
        if (mEnabledSession != NULL && mEnabledSession->mSession != NULL) {
            // try {
            if (DEBUG) Slogger::V(TAG, "Disabling: %s", TO_CSTR(mEnabledSession));
            mEnabledSession->mMethod->SetSessionEnabled(mEnabledSession->mSession, FALSE);
            // } catch (RemoteException e) {
            // }
        }

        mEnabledSession = session;

        if (mEnabledSession != NULL && mEnabledSession->mSession != NULL) {
            if (DEBUG) Slogger::V(TAG, "Enabling: %s", TO_CSTR(mEnabledSession));
            mEnabledSession->mMethod->SetSessionEnabled(mEnabledSession->mSession, TRUE);
        }
    }
}

Boolean CInputMethodManagerService::ChooseNewDefaultIMELocked()
{
    AutoPtr<IInputMethodInfo> imi;
    AutoPtr<IList> list;
    mSettings->GetEnabledInputMethodListLocked((IList**)&list);

    AutoPtr<IInputMethodUtils> imUtils;
    CInputMethodUtils::AcquireSingleton((IInputMethodUtils**)&imUtils);
    imUtils->GetMostApplicableDefaultIME(list, (IInputMethodInfo**)&imi);
    if (imi != NULL) {
        String id;
        imi->GetId(&id);
        if (DEBUG) {
            Slogger::D(TAG, "New default IME was selected: %s", id.string());
        }
        ResetSelectedInputMethodAndSubtypeLocked(id);
        return TRUE;
    }

    return FALSE;
}

void CInputMethodManagerService::BuildInputMethodListLocked(
    /* [in] */ IArrayList* list,
    /* [in] */ IHashMap* map, //HashMap<String, InputMethodInfo>
    /* [in] */ Boolean resetDefaultEnabledIme)
{
    if (DEBUG) {
        Slogger::D(TAG, "--- re-buildInputMethodList reset = %d \n ------ \n",
            resetDefaultEnabledIme);
    }
    list->Clear();
    map->Clear();

    // Use for queryIntentServicesAsUser
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    String disabledSysImes;
    mSettings->GetDisabledSystemInputMethods(&disabledSysImes);
    if (disabledSysImes.IsNull()) disabledSysImes = "";
    Int32 uid;
    mSettings->GetCurrentUserId(&uid);

    AutoPtr<IIntent> intent;// List<ResolveInfo>
    CIntent::New(IInputMethod::SERVICE_INTERFACE, (IIntent**)&intent);
    Int32 curUid;
    mSettings->GetCurrentUserId(&curUid);
    AutoPtr<IList> query;
    pm->QueryIntentServicesAsUser(intent,
        IPackageManager::GET_META_DATA | IPackageManager::GET_DISABLED_UNTIL_USED_COMPONENTS,
        curUid, (IList**)&query);

    AutoPtr<IIterator> svItem;
    query->GetIterator((IIterator**)&svItem);
    Boolean hasNext = FALSE;
    AutoPtr<IHashMap> additionalSubtypes = mFileManager->GetAllAdditionalInputMethodSubtypes();
    IMap* subtypes = IMap::Probe(additionalSubtypes);

    String pkgName, name, permission, strCompName, id;
    while(svItem->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        svItem->GetNext((IInterface**)&obj);
        AutoPtr<IResolveInfo> ri = IResolveInfo::Probe(obj);

        AutoPtr<IServiceInfo> si;
        ri->GetServiceInfo((IServiceInfo**)&si);
        IPackageItemInfo* pii = IPackageItemInfo::Probe(si);
        pii->GetPackageName(&pkgName);
        pii->GetName(&name);
        si->GetPermission(&permission);
        AutoPtr<IComponentName> compName;
        CComponentName::New(pkgName, name, (IComponentName**)&compName);
        compName->ToString(&strCompName);
        if (!Elastos::Droid::Manifest::permission::BIND_INPUT_METHOD.Equals(permission)) {
            Slogger::W(TAG, "Skipping input method compName: %s:" \
                " it does not require the permission android.permission.BIND_INPUT_METHOD",
                strCompName.string());
            continue;
        }

        if (DEBUG) Slogger::D(TAG, "Checking %s", strCompName.string());

        //try {
        AutoPtr<IInputMethodInfo> p;
        CInputMethodInfo::New(mContext, ri, subtypes, (IInputMethodInfo**)&p);
        list->Add(p.Get());
        p->GetId(&id);
        AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(id);
        map->Put(TO_IINTERFACE(keyObj), p.Get());

        if (DEBUG) {
            Slogger::D(TAG, "Found an input method %s", id.string());
        }

        // } catch (XmlPullParserException e) {
        //     Slogger::W(TAG, "Unable to load input method " + compName, e);
        // } catch (IOException e) {
        //     Slogger::W(TAG, "Unable to load input method " + compName, e);
        // }
    }

    if (resetDefaultEnabledIme) {
        AutoPtr<IInputMethodUtils> imUtils;
        CInputMethodUtils::AcquireSingleton((IInputMethodUtils**)&imUtils);
        AutoPtr<IArrayList> defaultEnabledIme; // ArrayList<InputMethodInfo>
        imUtils->GetDefaultEnabledImes(mContext, mSystemReady, list, (IArrayList**)&defaultEnabledIme);
        Int32 size;
        defaultEnabledIme->GetSize(&size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> obj;
            defaultEnabledIme->Get(i, (IInterface**)&obj);
            IInputMethodInfo* imi = IInputMethodInfo::Probe(obj);
            if (DEBUG) {
                Slogger::D(TAG, "--- enable ime = %s", TO_CSTR(imi));
            }
            String id;
            imi->GetId(&id);
            SetInputMethodEnabledLocked(id, TRUE);
        }
    }

    String defaultImiId;
    mSettings->GetSelectedInputMethod(&defaultImiId);
    if (!TextUtils::IsEmpty(defaultImiId)) {
        AutoPtr<ICharSequence> csq = CoreUtils::Convert(defaultImiId);
        Boolean bval;
        map->ContainsKey(TO_IINTERFACE(csq), &bval);
        if (!bval) {
            Slogger::W(TAG, "Default IME is uninstalled. Choose new default IME.");
            if (ChooseNewDefaultIMELocked()) {
                UpdateInputMethodsFromSettingsLocked(TRUE);
            }
        }
        else {
            // Double check that the default IME is certainly enabled.
            SetInputMethodEnabledLocked(defaultImiId, TRUE);
        }
    }

    // Here is not the perfect place to reset the switching controller. Ideally
    // mSwitchingController and mSettings should be able to share the same state.
    // TODO: Make sure that mSwitchingController and mSettings are sharing the
    // the same enabled IMEs list.
    mSwitchingController->ResetCircularListLocked(mContext);
}

void CInputMethodManagerService::ShowInputMethodMenu()
{
    ShowInputMethodMenuInternal(FALSE);
}

void CInputMethodManagerService::ShowInputMethodSubtypeMenu()
{
    ShowInputMethodMenuInternal(TRUE);
}

void CInputMethodManagerService::ShowInputMethodAndSubtypeEnabler(
    /* [in] */ const String& inputMethodId)
{
    AutoPtr<IIntent> intent;
    CIntent::New(ISettings::ACTION_INPUT_METHOD_SUBTYPE_SETTINGS, (IIntent**)&intent);
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
            | IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED
            | IIntent::FLAG_ACTIVITY_CLEAR_TOP);
    if (!TextUtils::IsEmpty(inputMethodId)) {
        intent->PutExtra(ISettings::EXTRA_INPUT_METHOD_ID, inputMethodId);
    }
    AutoPtr<IUserHandleHelper> userHandleHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&userHandleHelper);
    AutoPtr<IUserHandle> CURRENT;
    userHandleHelper->GetCURRENT((IUserHandle**)&CURRENT);
    mContext->StartActivityAsUser(intent, NULL, CURRENT);
}

void CInputMethodManagerService::ShowConfigureInputMethods()
{
    AutoPtr<IIntent> intent;
    CIntent::New(ISettings::ACTION_INPUT_METHOD_SETTINGS, (IIntent**)&intent);
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
            | IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED
            | IIntent::FLAG_ACTIVITY_CLEAR_TOP);
    AutoPtr<IUserHandleHelper> userHandleHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&userHandleHelper);
    AutoPtr<IUserHandle> CURRENT;
    userHandleHelper->GetCURRENT((IUserHandle**)&CURRENT);
    mContext->StartActivityAsUser(intent, NULL, CURRENT);
}

Boolean CInputMethodManagerService::IsScreenLocked()
{
    Boolean rst = FALSE;
    if (mKeyguardManager != NULL) {
        Boolean isKeyguardLocked, isKeyguardSecure;
        mKeyguardManager->IsKeyguardLocked(&isKeyguardLocked);
        mKeyguardManager->IsKeyguardSecure(&isKeyguardSecure);
        if (isKeyguardLocked && isKeyguardSecure) {
            rst = TRUE;
        }
    }
    return rst;
}

void CInputMethodManagerService::ShowInputMethodMenuInternal(
    /* [in] */ Boolean showSubtypes)
{
    Slogger::E(TAG, "// TODO: ShowInputMethodMenuInternal() E_NOT_IMPLEMENTED Line: %d", __LINE__);

    // if (DEBUG) Slog.v(TAG, "Show switching menu");

    // final Context context = mContext;
    // final boolean isScreenLocked = isScreenLocked();

    // final String lastInputMethodId = mSettings.getSelectedInputMethod();
    // int lastInputMethodSubtypeId = mSettings.getSelectedInputMethodSubtypeId(lastInputMethodId);
    // if (DEBUG) Slog.v(TAG, "Current IME: " + lastInputMethodId);

    // {    AutoLock syncLock(mMethodMap);
    //     final HashMap<InputMethodInfo, List<InputMethodSubtype>> immis =
    //             mSettings.getExplicitlyOrImplicitlyEnabledInputMethodsAndSubtypeListLocked(
    //                     mContext);
    //     if (immis == null || immis.size() == 0) {
    //         return;
    //     }

    //     hideInputMethodMenuLocked();

    //     final List<IImeSubtypeListItem> imList =
    //             mSwitchingController.getSortedInputMethodAndSubtypeListLocked(
    //                     showSubtypes, mInputShown, isScreenLocked);

    //     if (lastInputMethodSubtypeId == NOT_A_SUBTYPE_ID) {
    //         final InputMethodSubtype currentSubtype = getCurrentInputMethodSubtypeLocked();
    //         if (currentSubtype != null) {
    //             final InputMethodInfo currentImi = mMethodMap.get(mCurMethodId);
    //             lastInputMethodSubtypeId = InputMethodUtils.getSubtypeIdFromHashCode(
    //                     currentImi, currentSubtype.hashCode());
    //         }
    //     }

    //     final int N = imList.size();
    //     mIms = new InputMethodInfo[N];
    //     mSubtypeIds = new int[N];
    //     int checkedItem = 0;
    //     for (int i = 0; i < N; ++i) {
    //         final IImeSubtypeListItem item = imList.get(i);
    //         mIms[i] = item.mImi;
    //         mSubtypeIds[i] = item.mSubtypeId;
    //         if (mIms[i].getId().equals(lastInputMethodId)) {
    //             int subtypeId = mSubtypeIds[i];
    //             if ((subtypeId == NOT_A_SUBTYPE_ID)
    //                     || (lastInputMethodSubtypeId == NOT_A_SUBTYPE_ID && subtypeId == 0)
    //                     || (subtypeId == lastInputMethodSubtypeId)) {
    //                 checkedItem = i;
    //             }
    //         }
    //     }
    //     final Context themedContext = new ContextThemeWrapper(context,
    //             android.R.style.Theme_DeviceDefault_Settings);
    //     mDialogBuilder = new AlertDialog.Builder(themedContext);
    //     final TypedArray a = themedContext.obtainStyledAttributes(null,
    //             com.android.internal.R.styleable.DialogPreference,
    //             com.android.internal.R.attr.alertDialogStyle, 0);
    //     mDialogBuilder.setIcon(a.getDrawable(
    //             com.android.internal.R.styleable.DialogPreference_dialogIcon));
    //     a.recycle();
    //     mDialogBuilder.setOnCancelListener(new OnCancelListener() {
    //         @Override
    //         public void onCancel(DialogInterface dialog) {
    //             hideInputMethodMenu();
    //         }
    //     });
    //     final LayoutInflater inflater =
    //             (LayoutInflater)themedContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
    //     final View tv = inflater.inflate(
    //             com.android.internal.R.layout.input_method_switch_dialog_title, null);
    //     mDialogBuilder.setCustomTitle(tv);

    //     // Setup layout for a toggle switch of the hardware keyboard
    //     mSwitchingDialogTitleView = tv;
    //     mSwitchingDialogTitleView
    //             .findViewById(com.android.internal.R.id.hard_keyboard_section)
    //             .setVisibility(mWindowManagerService.isHardKeyboardAvailable()
    //                     ? View.VISIBLE : View.GONE);
    //     final Switch hardKeySwitch = (Switch)mSwitchingDialogTitleView.findViewById(
    //             com.android.internal.R.id.hard_keyboard_switch);
    //     hardKeySwitch.setChecked(mShowImeWithHardKeyboard);
    //     hardKeySwitch.setOnCheckedChangeListener(new OnCheckedChangeListener() {
    //         @Override
    //         public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
    //             mSettings.setShowImeWithHardKeyboard(isChecked);
    //             // Ensure that the input method dialog is dismissed when changing
    //             // the hardware keyboard state.
    //             hideInputMethodMenu();
    //         }
    //     });

    //     final ImeSubtypeListAdapter adapter = new ImeSubtypeListAdapter(themedContext,
    //             com.android.internal.R.layout.input_method_switch_item, imList, checkedItem);
    //     final OnClickListener choiceListener = new OnClickListener() {
    //         @Override
    //         public void onClick(final DialogInterface dialog, final int which) {
    //             {    AutoLock syncLock(mMethodMap);
    //                 if (mIms == null || mIms.length <= which || mSubtypeIds == null
    //                         || mSubtypeIds.length <= which) {
    //                     return;
    //                 }
    //                 final InputMethodInfo im = mIms[which];
    //                 int subtypeId = mSubtypeIds[which];
    //                 adapter.mCheckedItem = which;
    //                 adapter.notifyDataSetChanged();
    //                 hideInputMethodMenu();
    //                 if (im != null) {
    //                     if (subtypeId < 0 || subtypeId >= im.getSubtypeCount()) {
    //                         subtypeId = NOT_A_SUBTYPE_ID;
    //                     }
    //                     setInputMethodLocked(im.getId(), subtypeId);
    //                 }
    //             }
    //         }
    //     };
    //     mDialogBuilder.setSingleChoiceItems(adapter, checkedItem, choiceListener);

    //     if (showSubtypes && !isScreenLocked) {
    //         final OnClickListener positiveListener = new OnClickListener() {
    //             @Override
    //             public void onClick(DialogInterface dialog, int whichButton) {
    //                 showConfigureInputMethods();
    //             }
    //         };
    //         mDialogBuilder.setPositiveButton(
    //                 com.android.internal.R.string.configure_input_methods, positiveListener);
    //     }
    //     mSwitchingDialog = mDialogBuilder.create();
    //     mSwitchingDialog.setCanceledOnTouchOutside(true);
    //     mSwitchingDialog.getWindow().setType(
    //             WindowManager.LayoutParams.TYPE_INPUT_METHOD_DIALOG);
    //     mSwitchingDialog.getWindow().getAttributes().privateFlags |=
    //             WindowManager.LayoutParams.PRIVATE_FLAG_SHOW_FOR_ALL_USERS;
    //     mSwitchingDialog.getWindow().getAttributes().setTitle("Select input method");
    //     updateImeWindowStatusLocked();
    //     mSwitchingDialog.show();
    // }
}

void CInputMethodManagerService::HideInputMethodMenu()
{
    AutoLock lock(mMethodMap.Get());
    HideInputMethodMenuLocked();
}

void CInputMethodManagerService::HideInputMethodMenuLocked()
{
    if (DEBUG) Slogger::V(TAG, "Hide switching menu");

    if (mSwitchingDialog != NULL) {
        IDialogInterface::Probe(mSwitchingDialog)->Dismiss();
        mSwitchingDialog = NULL;
    }

    UpdateImeWindowStatusLocked();
    mDialogBuilder = NULL;
    mIms = NULL;
}

ECode CInputMethodManagerService::SetInputMethodEnabled(
    /* [in] */ const String& id,
    /* [in] */ Boolean enabled,
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state);

    // TODO: Make this work even for non-current users?
    if (!CalledFromValidUser()) {
        *state = FALSE;
        return NOERROR;
    }
    AutoLock lock(mMethodMap.Get());

    Int32 perm = 0;
    FAIL_RETURN(mContext->CheckCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::WRITE_SECURE_SETTINGS,
            &perm));
    if (perm != IPackageManager::PERMISSION_GRANTED) {
        // throw new SecurityException(
        //         "Requires permission "
        //         + Elastos::Droid::Manifest::permission::WRITE_SECURE_SETTINGS);
        return E_SECURITY_EXCEPTION;
    }

    Int64 ident = Binder::ClearCallingIdentity();
    *state = SetInputMethodEnabledLocked(id, enabled);
    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}

Boolean CInputMethodManagerService::SetInputMethodEnabledLocked(
    /* [in] */ const String& id,
    /* [in] */ Boolean enabled)
{
    // Make sure this is a valid input method.
    AutoPtr<ICharSequence> csq = CoreUtils::Convert(id);
    IInterface* key = TO_IINTERFACE(csq);
    AutoPtr<IInterface> obj;
    mMethodMap->Get(key, (IInterface**)&obj);
    AutoPtr<IInputMethodInfo> imm = IInputMethodInfo::Probe(obj);
    if (imm == NULL) {
        // throw new IllegalArgumentException("Unknown id: " + mCurMethodId);
        Slogger::E(TAG, "Unknown id: %s", mCurMethodId.string());
        return FALSE;
    }

    AutoPtr<IList> enabledInputMethodsList; //List<Pair<String, ArrayList<String>>>
    mSettings->GetEnabledInputMethodsAndSubtypeListLocked((IList**)&enabledInputMethodsList);

    if (enabled) {
        AutoPtr<IIterator> it;
        enabledInputMethodsList->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj, fo;
            it->GetNext((IInterface**)&obj);
            IPair* pair = IPair::Probe(obj);
            pair->GetFirst((IInterface**)&fo);
            String first = TO_STR(fo);
            if (first.Equals(id)) {
                // We are enabling this input method, but it is already enabled.
                // Nothing to do. The previous state was enabled.
                return true;
            }
        }
        mSettings->AppendAndPutEnabledInputMethodLocked(id, FALSE);
        // Previous state was disabled.
        return FALSE;
    }
    else {
        AutoPtr<IStringBuilder> builder = (IStringBuilder*)new StringBuilder();
        Boolean bval;
        mSettings->BuildAndPutEnabledInputMethodsStrRemovingIdLocked(
            builder, enabledInputMethodsList, id, &bval);
        if (bval) {
            // Disabled input method is currently selected, switch to another one.
            String selId;
            mSettings->GetSelectedInputMethod(&selId);
            if (id.Equals(selId) && !ChooseNewDefaultIMELocked()) {
                Slogger::I(TAG, "Can't find new IME, unsetting the current input method.");
                ResetSelectedInputMethodAndSubtypeLocked(String(""));
            }
            // Previous state was enabled.
            return TRUE;
        }
        else {
            // We are disabling the input method but it is already disabled.
            // Nothing to do.  The previous state was disabled.
            return FALSE;
        }
    }
    return FALSE;
}

void CInputMethodManagerService::SetSelectedInputMethodAndSubtypeLocked(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ Int32 subtypeId,
    /* [in] */ Boolean setSubtypeOnly)
{
    // Update the history of InputMethod and Subtype
    mSettings->SaveCurrentInputMethodAndSubtypeToHistory(mCurMethodId, mCurrentSubtype);

    mCurUserActionNotificationSequenceNumber =
        Elastos::Core::Math::Max(mCurUserActionNotificationSequenceNumber + 1, 1);
    if (DEBUG) {
        Slogger::D(TAG, "Bump mCurUserActionNotificationSequenceNumber:%d",
            mCurUserActionNotificationSequenceNumber);
    }

    if (mCurClient != NULL && mCurClient->mClient != NULL) {
        AutoPtr<IMessage> msg;
        mCaller->ObtainMessageIO(MSG_SET_USER_ACTION_NOTIFICATION_SEQUENCE_NUMBER,
            mCurUserActionNotificationSequenceNumber, TO_IINTERFACE(mCurClient), (IMessage**)&msg);
        ExecuteOrSendMessage(mCurClient->mClient, msg);
    }

    // Set Subtype here
    if (imi == NULL || subtypeId < 0) {
        mSettings->PutSelectedSubtype(NOT_A_SUBTYPE_ID);
        mCurrentSubtype = NULL;
    }
    else {
        Int32 count = 0;
        if (subtypeId < (imi->GetSubtypeCount(&count), count)) {
            AutoPtr<IInputMethodSubtype> subtype;
            imi->GetSubtypeAt(subtypeId, (IInputMethodSubtype**)&subtype);
            assert(subtype != NULL);
            Int32 hashCode = Object::GetHashCode(subtype);
            mSettings->PutSelectedSubtype(hashCode);
            mCurrentSubtype = subtype;
        }
        else {
            mSettings->PutSelectedSubtype(NOT_A_SUBTYPE_ID);
            // If the subtype is not specified, choose the most applicable one
            mCurrentSubtype = GetCurrentInputMethodSubtypeLocked();
        }
    }

    // Workaround.
    // ASEC is not ready in the IMMS constructor. Accordingly, forward-locked
    // IMEs are not recognized and considered uninstalled.
    // Actually, we can't move everything after SystemReady because
    // IMMS needs to run in the encryption lock screen. So, we just skip changing
    // the default IME here and try cheking the default IME again in systemReady().
    // TODO: Do nothing before system ready and implement a separated logic for
    // the encryption lock screen.
    // TODO: ASEC should be ready before IMMS is instantiated.
    if (mSystemReady && !setSubtypeOnly) {
        // Set InputMethod here
        String id;
        mSettings->PutSelectedInputMethod(imi != NULL ? (imi->GetId(&id), id) : String(""));
    }
}

void CInputMethodManagerService::ResetSelectedInputMethodAndSubtypeLocked(
    /* [in] */ const String& newDefaultIme)
{
    AutoPtr<ICharSequence> csq = CoreUtils::Convert(newDefaultIme);
    IInterface* key = TO_IINTERFACE(csq);

    AutoPtr<IInterface> obj;
    mMethodMap->Get(key, (IInterface**)&obj);
    AutoPtr<IInputMethodInfo> imi = IInputMethodInfo::Probe(obj);

    Int32 lastSubtypeId = NOT_A_SUBTYPE_ID;
    // newDefaultIme is empty when there is no candidate for the selected IME.
    if (imi != NULL && !TextUtils::IsEmpty(newDefaultIme)) {
        String subtypeHashCode;
        mSettings->GetLastSubtypeForInputMethodLocked(newDefaultIme, &subtypeHashCode);
        if (!subtypeHashCode.IsNull()) {
            // try {
            AutoPtr<IInputMethodUtils> imUtils;
            CInputMethodUtils::AcquireSingleton((IInputMethodUtils**)&imUtils);
            imUtils->GetSubtypeIdFromHashCode(
                imi, StringUtils::ParseInt32(subtypeHashCode), &lastSubtypeId);
            // } catch (NumberFormatException e) {
            //     Slogger::W(TAG, "GetHashCode for subtype looks broken: " + subtypeHashCode, e);
            // }
        }
    }
    SetSelectedInputMethodAndSubtypeLocked(imi, lastSubtypeId, FALSE);
}

AutoPtr<IPair> /* Pair<InputMethodInfo, InputMethodSubtype> */
CInputMethodManagerService::FindLastResortApplicableShortcutInputMethodAndSubtypeLocked(
    /* [in] */ const String& mode)
{
// If there are no selected shortcuts, tries finding the most applicable ones.
// private Pair<InputMethodInfo, InputMethodSubtype>
//         FindLastResortApplicableShortcutInputMethodAndSubtypeLocked(String mode) {
//     List<InputMethodInfo> imis = mSettings.getEnabledInputMethodListLocked();
//     InputMethodInfo mostApplicableIMI = NULL;
//     InputMethodSubtype mostApplicableSubtype = NULL;
//     boolean foundInSystemIME = false;

//     // Search applicable subtype for each InputMethodInfo
//     for (InputMethodInfo imi: imis) {
//         final String imiId = imi.getId();
//         if (foundInSystemIME && !imiId.equals(mCurMethodId)) {
//             continue;
//         }
//         InputMethodSubtype subtype = NULL;
//         final List<InputMethodSubtype> enabledSubtypes =
//                 getEnabledInputMethodSubtypeListLocked(imi, true);
//         // 1. Search by the current subtype's locale from enabledSubtypes.
//         if (mCurrentSubtype != NULL) {
//             subtype = findLastResortApplicableSubtypeLocked(
//                     mRes, enabledSubtypes, mode, mCurrentSubtype.getLocale(), false);
//         }
//         // 2. Search by the system locale from enabledSubtypes.
//         // 3. Search the first enabled subtype matched with mode from enabledSubtypes.
//         if (subtype == NULL) {
//             subtype = findLastResortApplicableSubtypeLocked(
//                     mRes, enabledSubtypes, mode, NULL, true);
//         }
//         final ArrayList<InputMethodSubtype> overridingImplicitlyEnabledSubtypes =
//                 getOverridingImplicitlyEnabledSubtypes(imi, mode);
//         final ArrayList<InputMethodSubtype> subtypesForSearch =
//                 overridingImplicitlyEnabledSubtypes.isEmpty()
//                         ? getSubtypes(imi) : overridingImplicitlyEnabledSubtypes;
//         // 4. Search by the current subtype's locale from all subtypes.
//         if (subtype == NULL && mCurrentSubtype != NULL) {
//             subtype = findLastResortApplicableSubtypeLocked(
//                     mRes, subtypesForSearch, mode, mCurrentSubtype.getLocale(), false);
//         }
//         // 5. Search by the system locale from all subtypes.
//         // 6. Search the first enabled subtype matched with mode from all subtypes.
//         if (subtype == NULL) {
//             subtype = findLastResortApplicableSubtypeLocked(
//                     mRes, subtypesForSearch, mode, NULL, true);
//         }
//         if (subtype != NULL) {
//             if (imiId.equals(mCurMethodId)) {
//                 // The current input method is the most applicable IME.
//                 mostApplicableIMI = imi;
//                 mostApplicableSubtype = subtype;
//                 break;
//             } else if (!foundInSystemIME) {
//                 // The system input method is 2nd applicable IME.
//                 mostApplicableIMI = imi;
//                 mostApplicableSubtype = subtype;
//                 if ((imi.getServiceInfo().applicationInfo.flags
//                         & ApplicationInfo.FLAG_SYSTEM) != 0) {
//                     foundInSystemIME = true;
//                 }
//             }
//         }
//     }
//     if (DEBUG) {
//         if (mostApplicableIMI != NULL) {
//             Slogger::W(TAG, "Most applicable shortcut input method was:"
//                     + mostApplicableIMI.getId());
//             if (mostApplicableSubtype != NULL) {
//                 Slogger::W(TAG, "Most applicable shortcut input method subtype was:"
//                         + "," + mostApplicableSubtype.getMode() + ","
//                         + mostApplicableSubtype.getLocale());
//             }
//         }
//     }
//     if (mostApplicableIMI != NULL) {
//         return new Pair<InputMethodInfo, InputMethodSubtype> (mostApplicableIMI,
//                 mostApplicableSubtype);
//     } else {
//         return NULL;
//     }
// }
    return NULL;
}

ECode CInputMethodManagerService::GetCurrentInputMethodSubtype(
    /* [out] */ IInputMethodSubtype** subtype)
{
    VALIDATE_NOT_NULL(subtype);

    // TODO: Make this work even for non-current users?
    if (!CalledFromValidUser()) {
        *subtype = NULL;
        return NOERROR;
    }
    AutoLock lock(mMethodMap.Get());

    AutoPtr<IInputMethodSubtype> s = GetCurrentInputMethodSubtypeLocked();
    *subtype = s;
    REFCOUNT_ADD(*subtype);
    return NOERROR;
}

AutoPtr<IInputMethodSubtype> CInputMethodManagerService::GetCurrentInputMethodSubtypeLocked()
{
    if (mCurMethodId.IsNull()) {
        return NULL;
    }

    AutoPtr<IInputMethodUtils> imUtils;
    CInputMethodUtils::AcquireSingleton((IInputMethodUtils**)&imUtils);
    Boolean subtypeIsSelected;
    mSettings->IsSubtypeSelected(&subtypeIsSelected);

    AutoPtr<IInputMethodInfo> imi = GetMethodInfoFromMethodMap(mCurMethodId);
    Int32 count = 0;
    if (imi == NULL || (imi->GetSubtypeCount(&count), count) == 0) {
        return NULL;
    }

    Boolean isValidSubtypeId = FALSE;
    if (mCurrentSubtype != NULL) {
        Int32 hashCode = Object::GetHashCode(mCurrentSubtype);
        imUtils->IsValidSubtypeId(imi, hashCode, &isValidSubtypeId);
    }
    if (!subtypeIsSelected || mCurrentSubtype == NULL || !isValidSubtypeId) {
        Int32 subtypeId;
        mSettings->GetSelectedInputMethodSubtypeId(mCurMethodId, &subtypeId);
        if (subtypeId == NOT_A_SUBTYPE_ID) {
            // If there are no selected subtypes, the framework will try to find
            // the most applicable subtype from explicitly or implicitly enabled
            // subtypes.
            AutoPtr<IList> explicitlyOrImplicitlyEnabledSubtypes;
            mSettings->GetEnabledInputMethodSubtypeListLocked(
                mContext, imi, TRUE, (IList**)&explicitlyOrImplicitlyEnabledSubtypes);
            // If there is only one explicitly or implicitly enabled subtype,
            // just returns it.
            Int32 size = 0;
            explicitlyOrImplicitlyEnabledSubtypes->GetSize(&size);
            if (size == 1) {
                AutoPtr<IInterface> obj;
                explicitlyOrImplicitlyEnabledSubtypes->Get(0, (IInterface**)&obj);
                mCurrentSubtype = IInputMethodSubtype::Probe(obj);
            }
            else if (size > 1) {
                imUtils->FindLastResortApplicableSubtypeLocked(
                    mRes, explicitlyOrImplicitlyEnabledSubtypes,
                    IInputMethodUtils::SUBTYPE_MODE_KEYBOARD, String(NULL), TRUE,
                    (IInputMethodSubtype**)&mCurrentSubtype);
                if (mCurrentSubtype == NULL) {
                    imUtils->FindLastResortApplicableSubtypeLocked(
                        mRes, explicitlyOrImplicitlyEnabledSubtypes, String(NULL), String(NULL),
                        TRUE, (IInputMethodSubtype**)&mCurrentSubtype);
                }
            }
        }
        else {
            AutoPtr<IArrayList> subtypes;
            imUtils->GetSubtypes(imi, (IArrayList**)&subtypes);
            AutoPtr<IInterface> obj;
            subtypes->Get(subtypeId, (IInterface**)&obj);
            mCurrentSubtype = IInputMethodSubtype::Probe(obj);
        }
    }
    return mCurrentSubtype;
}

void CInputMethodManagerService::AddShortcutInputMethodAndSubtypes(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ IInputMethodSubtype* subtype)
{
    IInterface* key = TO_IINTERFACE(imi);
    Boolean bval = FALSE;
    mShortcutInputMethodsAndSubtypes->ContainsKey(key, &bval);
    if (bval) {
        AutoPtr<IInterface> value;
        mShortcutInputMethodsAndSubtypes->Get(imi, (IInterface**)&value);
        IArrayList* subtypes = IArrayList::Probe(value);
        subtypes->Add(TO_IINTERFACE(subtype));
    }
    else {
        AutoPtr<IArrayList> subtypes;
        CArrayList::New((IArrayList**)&subtypes);
        subtypes->Add(TO_IINTERFACE(subtype));
        mShortcutInputMethodsAndSubtypes->Put(key, subtypes.Get());
    }
}

ECode CInputMethodManagerService::GetShortcutInputMethodsAndSubtypes(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoLock lock(mMethodMap.Get());

    AutoPtr<IArrayList> ret;
    CArrayList::New((IArrayList**)&ret);
    Boolean isEmpty;
    mShortcutInputMethodsAndSubtypes->IsEmpty(&isEmpty);
    if (isEmpty) {
        // If there are no selected shortcut subtypes, the framework will try to find
        // the most applicable subtype from all subtypes whose mode is
        // SUBTYPE_MODE_VOICE. This is an exceptional case, so we will hardcode the mode.
        AutoPtr<IPair> info; //Pair<InputMethodInfo, InputMethodSubtype>
        FindLastResortApplicableShortcutInputMethodAndSubtypeLocked(
            IInputMethodUtils::SUBTYPE_MODE_VOICE);
        if (info != NULL) {
            AutoPtr<IInterface> f, s;
            info->GetFirst((IInterface**)&f);
            info->GetSecond((IInterface**)&s);
            ret->Add(f);
            ret->Add(s);
        }

        *result = IList::Probe(ret);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    AutoPtr<ISet> set;
    mShortcutInputMethodsAndSubtypes->GetKeySet((ISet**)&set);
    AutoPtr<IIterator> it;
    set->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj, value;
        it->GetNext((IInterface**)&obj);
        // IInputMethodInfo* imi = IInputMethodInfo::Probe(obj);
        ret->Add(obj);

        mShortcutInputMethodsAndSubtypes->Get(obj, (IInterface**)&value);
        IArrayList* subtypes = IArrayList::Probe(value);
        AutoPtr<IIterator> sit;
        subtypes->GetIterator((IIterator**)&sit);
        Boolean bval;
        while (sit->HasNext(&bval), bval) {
            AutoPtr<IInterface> subtypeObj;
            sit->GetNext((IInterface**)&subtypeObj);
            ret->Add(subtypeObj);
        }
    }

    *result = IList::Probe(ret);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CInputMethodManagerService::SetCurrentInputMethodSubtype(
    /* [in] */ IInputMethodSubtype* subtype,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    // TODO: Make this work even for non-current users?
    if (!CalledFromValidUser()) {
        return NOERROR;
    }

    AutoLock lock(mMethodMap.Get());
    if (subtype != NULL && !mCurMethodId.IsNull()) {
        AutoPtr<IInputMethodInfo> imi = GetMethodInfoFromMethodMap(mCurMethodId);
        Int32 hashCode = Object::GetHashCode(subtype);
        Int32 subtypeId;
        AutoPtr<IInputMethodUtils> imUtils;
        CInputMethodUtils::AcquireSingleton((IInputMethodUtils**)&imUtils);
        imUtils->GetSubtypeIdFromHashCode(imi, hashCode, &subtypeId);
        if (subtypeId != NOT_A_SUBTYPE_ID) {
            SetInputMethodLocked(mCurMethodId, subtypeId);
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CInputMethodManagerService::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

// // ----------------------------------------------------------------------
// // Utilities for debug
// @Override
// protected void dump(FileDescriptor fd, PrintWriter pw, String[] args) {
//     if (mContext.checkCallingOrSelfPermission(Elastos::Droid::Manifest::permission::DUMP)
//             != PackageManager.PERMISSION_GRANTED) {

//         pw.println("Permission Denial: can't dump InputMethodManager from from pid="
//                 + Binder.getCallingPid()
//                 + ", uid=" + Binder.getCallingUid());
//         return;
//     }

//     IInputMethod method;
//     ClientState client;

//     final Printer p = new PrintWriterPrinter(pw);

//     {    AutoLock syncLock(mMethodMap);
//         p.println("Current Input Method Manager state:");
//         int N = mMethodList->size();
//         p.println("  Input Methods:");
//         for (int i=0; i<N; i++) {
//             InputMethodInfo info = mMethodList->get(i);
//             p.println("  InputMethod #" + i + ":");
//             info.dump(p, "    ");
//         }
//         p.println("  Clients:");
//         for (ClientState ci : mClients.values()) {
//             p.println("  Client " + ci + ":");
//             p.println("    client=" + ci.client);
//             p.println("    inputContext=" + ci.inputContext);
//             p.println("    sessionRequested=" + ci.sessionRequested);
//             p.println("    curSession=" + ci.curSession);
//         }
//         p.println("  mCurMethodId=" + mCurMethodId);
//         client = mCurClient;
//         p.println("  mCurClient=" + client + " mCurSeq=" + mCurSeq);
//         p.println("  mCurFocusedWindow=" + mCurFocusedWindow);
//         p.println("  mCurId=" + mCurId + " mHaveConnect=" + mHaveConnection
//                 + " mBoundToMethod=" + mBoundToMethod);
//         p.println("  mCurToken=" + mCurToken);
//         p.println("  mCurIntent=" + mCurIntent);
//         method = mCurMethod;
//         p.println("  mCurMethod=" + mCurMethod);
//         p.println("  mEnabledSession=" + mEnabledSession);
//         p.println("  mShowRequested=" + mShowRequested
//                 + " mShowExplicitlyRequested=" + mShowExplicitlyRequested
//                 + " mShowForced=" + mShowForced
//                 + " mInputShown=" + mInputShown);
            // p.println("  mCurUserActionNotificationSequenceNumber="
            //         + mCurUserActionNotificationSequenceNumber);
            // p.println("  mSystemReady=" + mSystemReady + " mInteractive=" + mScreenOn);
//     }

//     p.println(" ");
//     if (client != null) {
//         pw.flush();
//         try {
//             client.client.asBinder().dump(fd, args);
//         } catch (RemoteException e) {
//             p.println("Input method client dead: " + e);
//         }
//     } else {
//         p.println("No input method client.");
//     }

//     p.println(" ");
//     if (method != null) {
//         pw.flush();
//         try {
//             method.asBinder().dump(fd, args);
//         } catch (RemoteException e) {
//             p.println("Input method service dead: " + e);
//         }
//     } else {
//         p.println("No input method service.");
//     }
// }

ECode CInputMethodManagerService::HandleMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    Int32 what;
    msg->GetWhat(&what);

    AutoPtr<SomeArgs> args;
    switch (what) {
        case MSG_SHOW_IM_PICKER:
            ShowInputMethodMenu();
            *result = TRUE;
            break;

        case MSG_SHOW_IM_SUBTYPE_PICKER:
            ShowInputMethodSubtypeMenu();
            *result = TRUE;
            break;

        case MSG_SHOW_IM_SUBTYPE_ENABLER: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            args = (SomeArgs*)ISomeArgs::Probe(obj);

            String str;
            AutoPtr<ICharSequence> seq = ICharSequence::Probe(args->mArg1);
            seq->ToString(&str);
            ShowInputMethodAndSubtypeEnabler(str);
            args->Recycle();
            *result = TRUE;
            break;
        }

        case MSG_SHOW_IM_CONFIG:
            ShowConfigureInputMethods();
            *result = TRUE;
            break;

        // ---------------------------------------------------------

        case MSG_UNBIND_INPUT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            IIInputMethod* im = IIInputMethod::Probe(obj);
            im->UnbindInput();
            *result = TRUE;
            break;
        }
        case MSG_BIND_INPUT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            args = (SomeArgs*)ISomeArgs::Probe(obj);
            IIInputMethod* im = IIInputMethod::Probe(args->mArg1);
            IInputBinding* ib = IInputBinding::Probe(args->mArg2);
            im->BindInput(ib);
            args->Recycle();
            *result = TRUE;
            break;
        }
        case MSG_SHOW_SOFT_INPUT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            args = (SomeArgs*)ISomeArgs::Probe(obj);
            Int32 arg1;
            msg->GetArg1(&arg1);
            IIInputMethod* im = IIInputMethod::Probe(args->mArg1);
            IResultReceiver* rr = IResultReceiver::Probe(args->mArg2);
            if (DEBUG) Slogger::V(TAG, "Calling %s.ShowSoftInput(%d, %s)",
                TO_CSTR(im), arg1, TO_CSTR(rr));
            im->ShowSoftInput(arg1, rr);
            args->Recycle();
            *result = TRUE;
            break;
        }

        case MSG_HIDE_SOFT_INPUT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            args = (SomeArgs*)ISomeArgs::Probe(obj);
            IIInputMethod* im = IIInputMethod::Probe(args->mArg1);
            IResultReceiver* rr = IResultReceiver::Probe(args->mArg2);
            if (DEBUG) Slogger::V(TAG, "Calling %s.HideSoftInput(%s)",
                TO_CSTR(im), TO_CSTR(rr));
            im->HideSoftInput(0, rr);
            args->Recycle();
            *result = TRUE;
            break;
        }

        case MSG_ATTACH_TOKEN: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            args = (SomeArgs*)ISomeArgs::Probe(obj);
            IIInputMethod* im = IIInputMethod::Probe(args->mArg1);
            IBinder* binder = IBinder::Probe(args->mArg2);
            im->AttachToken(binder);
            args->Recycle();
            *result = TRUE;
            break;
        }

        case MSG_CREATE_SESSION: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            args = (SomeArgs*)ISomeArgs::Probe(obj);
            IIInputMethod* method = IIInputMethod::Probe(args->mArg1);
            IInputChannel* channel = IInputChannel::Probe(args->mArg2);
            IIInputSessionCallback* callback = IIInputSessionCallback::Probe(args->mArg3);
            method->CreateSession(channel, callback);
            // Dispose the channel if the input method is not local to this process
            // because the remote proxy will get its own copy when unparceled.
            IProxy* proxy = (IProxy*)method->Probe(EIID_IProxy);
            if (channel != NULL && proxy != NULL) {
                channel->Dispose();
            }
            args->Recycle();
            *result = TRUE;
            break;
        }

        // ---------------------------------------------------------

        case MSG_START_INPUT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            args = (SomeArgs*)ISomeArgs::Probe(obj);

            SessionState* session = (SessionState*)IObject::Probe(args->mArg1);
            SetEnabledSessionInMainThread(session);
            IIInputContext* ic = IIInputContext::Probe(args->mArg2);
            IEditorInfo* ei = IEditorInfo::Probe(args->mArg3);
            session->mMethod->StartInput(ic, ei);
            args->Recycle();
            *result = TRUE;
            break;
        }

        case MSG_RESTART_INPUT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            args = (SomeArgs*)ISomeArgs::Probe(obj);

            SessionState* session = (SessionState*)IObject::Probe(args->mArg1);
            SetEnabledSessionInMainThread(session);
            IIInputContext* ic = IIInputContext::Probe(args->mArg2);
            IEditorInfo* ei = IEditorInfo::Probe(args->mArg3);
            session->mMethod->RestartInput(ic, ei);
            args->Recycle();
            *result = TRUE;
            break;
        }

        // ---------------------------------------------------------

        case MSG_UNBIND_METHOD: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            IInputMethodClient* imc = IInputMethodClient::Probe(obj);
            Int32 arg1;
            msg->GetArg1(&arg1);
            imc->OnUnbindMethod(arg1);
            *result = TRUE;
            break;
        }

        case MSG_BIND_METHOD:  {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            args = (SomeArgs*)ISomeArgs::Probe(obj);
            IInputMethodClient* client = IInputMethodClient::Probe(args->mArg1);
            IInputBindResult* res = IInputBindResult::Probe(args->mArg2);
            ECode ec = client->OnBindMethod(res);
            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                Slogger::W(TAG, "Client died receiving input method %s", TO_CSTR(res));
            }
            // Dispose the channel if the input method is not local to this process
            // because the remote proxy will get its own copy when unparceled.
            AutoPtr<IInputChannel> channel;
            res->GetChannel((IInputChannel**)&channel);
            IProxy* proxy = (IProxy*)client->Probe(EIID_IProxy);
            if (channel != NULL && proxy != NULL) {
                channel->Dispose();
            }
            args->Recycle();
            *result = TRUE;
            break;
        }

        case MSG_SET_ACTIVE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ClientState* cs = (ClientState*)IObject::Probe(obj);
            Int32 arg1;
            msg->GetArg1(&arg1);
            cs->mClient->SetActive(arg1 != 0);
            *result = TRUE;
            break;
        }

        case MSG_SET_USER_ACTION_NOTIFICATION_SEQUENCE_NUMBER: {
            Int32 sequenceNumber;
            msg->GetArg1(&sequenceNumber);
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ClientState* clientState = (ClientState*)IObject::Probe(obj);
            ECode ec = clientState->mClient->SetUserActionNotificationSequenceNumber(sequenceNumber);
            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                Slogger::W(TAG, "Got RemoteException sending "
                    "setUserActionNotificationSequenceNumber(%d) notification to pid %d uid %d",
                    sequenceNumber, clientState->mPid, clientState->mUid);
            }
            *result = TRUE;
            break;
        }

        // --------------------------------------------------------------
        case MSG_HARD_KEYBOARD_SWITCH_CHANGED: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHardKeyboardListener->HandleHardKeyboardStatusChange(arg1 != 1);
            *result = TRUE;
            break;
        }

        default:
            *result = FALSE;
            Slogger::W(TAG, "CInputMethodManagerService::Unknown message: %d", what);
            break;
    }

    return NOERROR;
}

}// namespace Server
}// namespace Droid
}// namespace Elastos
