
#include "CInputMethodManagerService.h"
#include "CInputMethodManagerServiceMethodCallback.h"
#include "CInputMethodManagerServiceUserSwitchObserver.h"
#include "pm/CPackageManagerService.h"
#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/HandlerBase.h"
#include "elastos/droid/os/SomeArgs.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/UserHandle.h"
#include "util/Xml.h"
#include "elastos/droid/R.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Core::Math;
using Elastos::Core::CBoolean;
using Elastos::Core::EIID_IComparable;
using Elastos::Core::CString;
using Elastos::IO::CFile;
using Elastos::IO::IFileOutputStream;
using Elastos::Core::CObjectContainer;
using Elastos::Utility::CObjectStringMap;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::CLocale;
using Elastos::Droid::R;
using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Database::EIID_IContentObserver;
using Elastos::Droid::InputMethodService::IInputMethodService;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::EIID_IHandlerCallback;
using Elastos::Droid::Os::EIID_IHandlerCallerCallback;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Utility::CAtomicFile;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Utility::CParcelableObjectContainer;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::View::CInputBindResult;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::View::CWindowManagerLayoutParamsHelper;
using Elastos::Droid::View::IWindowManagerLayoutParamsHelper;
using Elastos::Droid::View::InputMethod::CInputBinding;
using Elastos::Droid::View::InputMethod::CInputMethodInfo;
using Elastos::Droid::View::InputMethod::CInputMethodSubtype;
using Elastos::Droid::View::InputMethod::CInputMethodSubtypeHelper;
using Elastos::Droid::View::InputMethod::IInputMethod;
using Elastos::Droid::View::InputMethod::IInputMethodSubtypeHelper;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::Widget::EIID_IArrayAdapter;
using Elastos::Droid::Widget::EIID_IBaseAdapter;
using Elastos::Droid::Widget::EIID_IListAdapter;
using Elastos::Droid::Widget::EIID_IAdapter;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IRadioButton;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Utility::CFastXmlSerializer;
using Elastos::Droid::Utility::IFastXmlSerializer;

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
const Int32 CInputMethodManagerService::MSG_HARD_KEYBOARD_SWITCH_CHANGED;
const Int64 CInputMethodManagerService::TIME_TO_RECONNECT;
const Int32 CInputMethodManagerService::SECURE_SUGGESTION_SPANS_MAX_SIZE;
const Int32 CInputMethodManagerService::NOT_A_SUBTYPE_ID;
String CInputMethodManagerService::NOT_A_SUBTYPE_ID_STR("-1");
String CInputMethodManagerService::SUBTYPE_MODE_KEYBOARD("keyboard");
String CInputMethodManagerService::SUBTYPE_MODE_VOICE("voice");
String CInputMethodManagerService::TAG_TRY_SUPPRESSING_IME_SWITCHER("TrySuppressingImeSwitcher");
String CInputMethodManagerService::TAG_ENABLED_WHEN_DEFAULT_IS_NOT_ASCII_CAPABLE("EnabledWhenDefaultIsNotAsciiCapable");
String CInputMethodManagerService::TAG_ASCII_CAPABLE("AsciiCapable");
AutoPtr<ILocale> CInputMethodManagerService::ENGLISH_LOCALE;

///////////////////////////////////////////////////////////
// CInputMethodManagerService::MyHandlerCallerCallback
///////////////////////////////////////////////////////////
CAR_INTERFACE_IMPL(CInputMethodManagerService::MyHandlerCallerCallback, IHandlerCallerCallback)

CInputMethodManagerService::MyHandlerCallerCallback::MyHandlerCallerCallback(
    /* [in] */ IWeakReference* host)
    : mHost(host)
{}

ECode CInputMethodManagerService::MyHandlerCallerCallback::ExecuteMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IHandlerCallback> cb;
    mHost->Resolve(EIID_IHandlerCallback, (IInterface**)&cb);
    if (cb != NULL) {
        Boolean result;
        return cb->HandleMessage(msg, &result);
    }
    return NOERROR;
}

///////////////////////////////////////////////////////////
// CInputMethodManagerService::VisibleServiceConnection
///////////////////////////////////////////////////////////
CAR_INTERFACE_IMPL(CInputMethodManagerService::VisibleServiceConnection, IServiceConnection)

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


///////////////////////////////////////////////////////////
// CInputMethodManagerService::SessionState
///////////////////////////////////////////////////////////
CAR_INTERFACE_IMPL(CInputMethodManagerService::SessionState, IInterface)

CInputMethodManagerService::SessionState::SessionState(
    /* [in] */ ClientState* client,
    /* [in] */ IIInputMethod* method,
    /* [in] */ IIInputMethodSession* session)
    : mClient(client)
    , mMethod(method)
    , mSession(session)
{}


///////////////////////////////////////////////////////////
// CInputMethodManagerService::ClientState
///////////////////////////////////////////////////////////

CAR_INTERFACE_IMPL(CInputMethodManagerService::ClientState, IInterface)

CInputMethodManagerService::ClientState::ClientState(
    /* [in] */ IInputMethodClient* client,
    /* [in] */ IInputContext* inputContext,
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

CInputMethodManagerService::ClientState::~ClientState()
{
}


///////////////////////////////////////////////////////////
// CInputMethodManagerService::SettingsObserver
///////////////////////////////////////////////////////////
CInputMethodManagerService::SettingsObserver::SettingsObserver(
    /* [in] */ IHandler* handler,
    /* [in] */ CInputMethodManagerService* host)
    : ContentObserver(handler)
    , mHost(host)
{
    AutoPtr<IContentResolver> resolver;
    mHost->mContext->GetContentResolver((IContentResolver**)&resolver);
    assert(resolver != NULL);

    AutoPtr<IUri> uri;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->GetUriFor(ISettingsSecure::DEFAULT_INPUT_METHOD, (IUri**)&uri);
    assert(uri != NULL);
    resolver->RegisterContentObserver(uri, FALSE, THIS_PROBE(IContentObserver));

    uri = NULL;
    settingsSecure->GetUriFor(ISettingsSecure::ENABLED_INPUT_METHODS, (IUri**)&uri);
    assert(uri != NULL);
    resolver->RegisterContentObserver(uri, FALSE, THIS_PROBE(IContentObserver));

    uri = NULL;
    settingsSecure->GetUriFor(ISettingsSecure::SELECTED_INPUT_METHOD_SUBTYPE, (IUri**)&uri);
    assert(uri != NULL);
    resolver->RegisterContentObserver(uri,
                FALSE, THIS_PROBE(IContentObserver));
}

ECode CInputMethodManagerService::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    AutoLock lock(mHost->mMethodMapLock);
    mHost->UpdateFromSettingsLocked();
    return NOERROR;
}


///////////////////////////////////////////////////////////
// CInputMethodManagerService::ImmsBroadcastReceiver
///////////////////////////////////////////////////////////

CInputMethodManagerService::ImmsBroadcastReceiver::ImmsBroadcastReceiver(
    /* [in] */ CInputMethodManagerService* host)
    : mHost(host)
{}

void CInputMethodManagerService::ImmsBroadcastReceiver::UpdateActive()
{
   // Inform the current client of the change in active status
   if (mHost->mCurClient != NULL && mHost->mCurClient->mClient != NULL) {
        AutoPtr<IMessage> msg;
        mHost->mCaller->ObtainMessage(
            CInputMethodManagerService::MSG_SET_ACTIVE,
            mHost->mScreenOn ? 1 : 0, mHost->mCurClient, (IMessage**)&msg);
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
    else {
        // Slog.w(TAG, "Unexpected intent " + intent);
    }

    return NOERROR;
}


///////////////////////////////////////////////////////////
// CInputMethodManagerService::MyPackageMonitor
///////////////////////////////////////////////////////////

CInputMethodManagerService::MyPackageMonitor::MyPackageMonitor(
    /* [in] */ CInputMethodManagerService* host)
    : mHost(host)
{
}

Boolean CInputMethodManagerService::MyPackageMonitor::IsChangingPackagesOfCurrentUser()
{
    Int32 userId = 0;
    GetChangingUserId(&userId);
    Boolean retval = userId == mHost->mSettings->GetCurrentUserId();
    // if (DEBUG) {
            if (!retval) {
    //     Slog.d(TAG, "--- ignore this call back from a background user: " + userId);
            }
    // }
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
        AutoLock lock(mHost->mMethodMapLock);
        String curInputMethodId = mHost->mSettings->GetSelectedInputMethod();

        if (!curInputMethodId.IsNull()) {
            List<AutoPtr<IInputMethodInfo> >::Iterator ator = mHost->mMethodList->Begin();
            for (; ator != mHost->mMethodList->End(); ++ator) {
                AutoPtr<IInputMethodInfo> imi = *ator;

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
        AutoLock lock(mHost->mMethodMapLock);
        AutoPtr<IInputMethodInfo> curIm;
        String curInputMethodId;
        curInputMethodId = mHost->mSettings->GetSelectedInputMethod();
        if (!curInputMethodId.IsNull()) {
            List<AutoPtr<IInputMethodInfo> >::Iterator ator = mHost->mMethodList->Begin();
            for (; ator != mHost->mMethodList->End(); ++ator) {
                AutoPtr<IInputMethodInfo> imi = *ator;
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

        mHost->BuildInputMethodListLocked(mHost->mMethodList, mHost->mMethodMap);
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
                mHost->mIPackageManager->GetServiceInfo(compName, 0,
                        mHost->mSettings->GetCurrentUserId(), (IServiceInfo**)&si);
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
            mHost->UpdateFromSettingsLocked();
        }
    }

    return NOERROR;
}


///////////////////////////////////////////////////////////
// CInputMethodManagerService::HardKeyboardListener
///////////////////////////////////////////////////////////

CAR_INTERFACE_IMPL(CInputMethodManagerService::HardKeyboardListener, IInterface)

CInputMethodManagerService::HardKeyboardListener::HardKeyboardListener(
    /* [in] */ CInputMethodManagerService* host)
    : mHost(host)
{
}

ECode CInputMethodManagerService::HardKeyboardListener::OnHardKeyboardStatusChange(
    /* [in] */ Boolean available,
    /* [in] */ Boolean enabled)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(
        CInputMethodManagerService::MSG_HARD_KEYBOARD_SWITCH_CHANGED,
        available ? 1 : 0, enabled ? 1 : 0, (IMessage**)&msg);
    Boolean result;
    mHost->mHandler->SendMessage(msg, &result);
    return NOERROR;
}

void CInputMethodManagerService::HardKeyboardListener::HandleHardKeyboardStatusChange(
    /* [in] */ Boolean available,
    /* [in] */ Boolean enabled)
{
    // if (DEBUG) {
    //     Slog.w(TAG, "HardKeyboardStatusChanged: available = " + available + ", enabled = "
    //             + enabled);
    // }
    {
        AutoLock lock(mHost->mMethodMapLock);

        Boolean showing = FALSE;
        if (mHost->mSwitchingDialog != NULL && mHost->mSwitchingDialogTitleView != NULL
                && (mHost->mSwitchingDialog->IsShowing(&showing), showing)) {
            AutoPtr<IView> view;
            mHost->mSwitchingDialogTitleView->FindViewById(
                R::id::hard_keyboard_section, (IView**)&view);
            assert(view != NULL);
            view->SetVisibility(available ? IView::VISIBLE : IView::GONE);
        }
    }
}


///////////////////////////////////////////////////////////
// CInputMethodManagerService::CheckReceiver
///////////////////////////////////////////////////////////

CInputMethodManagerService::CheckReceiver::CheckReceiver(
    /* [in] */ CInputMethodManagerService* host)
    : mHost(host)
{
}

ECode CInputMethodManagerService::CheckReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    {
        AutoLock lock(mHost->mMethodMapLock);

        mHost->CheckCurrentLocaleChangedLocked();
    }
    return NOERROR;
}


///////////////////////////////////////////////////////////
// CInputMethodManagerService::ImeSubtypeListItem
///////////////////////////////////////////////////////////

CInputMethodManagerService::ImeSubtypeListItem::ImeSubtypeListItem(
    /* [in] */ ICharSequence* imeName,
    /* [in] */ ICharSequence* subtypeName,
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ Int32 subtypeId,
    /* [in] */ const String& subtypeLocale,
    /* [in] */ const String& systemLocale)
    : mImeName(imeName)
    , mSubtypeName(subtypeName)
    , mImi(imi)
    , mSubtypeId(subtypeId)
{
    if (subtypeLocale.IsNullOrEmpty()) {
        mIsSystemLocale = FALSE;
        mIsSystemLanguage = FALSE;
    }
    else {
        mIsSystemLocale = subtypeLocale.Equals(systemLocale);
        mIsSystemLanguage = mIsSystemLocale
                || subtypeLocale.StartWith(systemLocale.Substring(0, 2));
    }
}

UInt32 CInputMethodManagerService::ImeSubtypeListItem::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CInputMethodManagerService::ImeSubtypeListItem::Release()
{
    return ElRefBase::Release();
}

PInterface CInputMethodManagerService::ImeSubtypeListItem::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IComparable*)this;
    }
    else if (riid == EIID_IComparable) {
        return (IComparable*)this;
    }

    return NULL;
}

ECode CInputMethodManagerService::ImeSubtypeListItem::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    VALIDATE_NOT_NULL(pIID);

    if (pObject == (IInterface*)(IComparable*)this) {
        *pIID = EIID_IComparable;
    }
    else {
        return E_INVALID_ARGUMENT;
    }

    return NOERROR;
}

ECode CInputMethodManagerService::ImeSubtypeListItem::CompareTo(
    /* [in] */ IInterface* _other,
    /* [out] */ Int32* result)
{
    assert(result != NULL);
    AutoPtr<ImeSubtypeListItem> other = (ImeSubtypeListItem*)_other;
    if (TextUtils::IsEmpty(mImeName)) {
        *result = 1;
        return NOERROR;
    }
    if (TextUtils::IsEmpty(other->mImeName)) {
        *result = -1;
        return NOERROR;
    }
    if (!TextUtils::Equals(mImeName, other->mImeName)) {
        String str, otherStr;
        mImeName->ToString(&str);
        other->mImeName->ToString(&otherStr);
        *result = str.Compare(otherStr);
        return NOERROR;
    }
    if (TextUtils::Equals(mSubtypeName, other->mSubtypeName)) {
        *result = 0;
        return NOERROR;
    }
    if (mIsSystemLocale) {
        *result = -1;
        return NOERROR;
    }
    if (other->mIsSystemLocale) {
        *result = 1;
        return NOERROR;
    }
    if (mIsSystemLanguage) {
        *result = -1;
        return NOERROR;
    }
    if (other->mIsSystemLanguage) {
        *result = 1;
        return NOERROR;
    }
    if (TextUtils::IsEmpty(mSubtypeName)) {
        *result = 1;
        return NOERROR;
    }
    if (TextUtils::IsEmpty(other->mSubtypeName)) {
        *result = -1;
        return NOERROR;
    }

    String str, otherStr;
    mSubtypeName->ToString(&str);
    other->mSubtypeName->ToString(&otherStr);
    *result = str.Compare(otherStr);
    return NOERROR;
}


///////////////////////////////////////////////////////////
// CInputMethodManagerService::ImeSubtypeListAdapter
///////////////////////////////////////////////////////////

CInputMethodManagerService::ImeSubtypeListAdapter::ImeSubtypeListAdapter(
    /* [in] */ IContext* context,
    /* [in] */ Int32 textViewResourceId,
    /* [in] */ List< AutoPtr<ImeSubtypeListItem> >* itemsList,
    /* [in] */ Int32 checkedItem)
//     : ArrayAdapter(Context, textViewResourceId, itemsList)
//     , mTextViewResourceId(textViewResourceId)
//     , mItemsList(itemsList)
//     , mCheckedItem(checkedItem)
{
    assert(0);
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&mInflater);
}

CInputMethodManagerService::ImeSubtypeListAdapter::~ImeSubtypeListAdapter()
{
    mItemsList = NULL;
}

UInt32 CInputMethodManagerService::ImeSubtypeListAdapter::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CInputMethodManagerService::ImeSubtypeListAdapter::Release()
{
    return ElRefBase::Release();
}

PInterface CInputMethodManagerService::ImeSubtypeListAdapter::Probe(
    /* [in] */ REIID riid)
{
    if (EIID_IArrayAdapter == riid) {
        return (IArrayAdapter*)this;
    }
    else if (EIID_IBaseAdapter == riid) {
        return (IBaseAdapter*)this;
    }
    else if (EIID_IListAdapter == riid) {
        return (IListAdapter*)this;
    }
    else if (EIID_IAdapter == riid) {
        return (IAdapter*)this;
    }

    return NULL;
}

ECode CInputMethodManagerService::ImeSubtypeListAdapter::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    VALIDATE_NOT_NULL(pIID);

    if (pObject == (IInterface*)(IArrayAdapter*)this) {
        *pIID = EIID_IArrayAdapter;
    }
    else if (pObject == (IInterface*)(IBaseAdapter*)this) {
        *pIID = EIID_IBaseAdapter;
    }
    else if (pObject == (IInterface*)(IListAdapter*)this) {
        *pIID = EIID_IListAdapter;
    }
    else if (pObject == (IInterface*)(IAdapter*)this) {
        *pIID = EIID_IAdapter;
    }
    else {
        return E_INVALID_ARGUMENT;
    }

    return NOERROR;
}

AutoPtr<IView> CInputMethodManagerService::ImeSubtypeListAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent)
{
    AutoPtr<IView> view;
    if (convertView != NULL) {
        view = convertView;
    }
    else {
        mInflater->Inflate(mTextViewResourceId, NULL, (IView**)&view);
    }
    if (position < 0 || position >= mItemsList->GetSize()) return view;
    AutoPtr<ImeSubtypeListItem> item = (*mItemsList)[position];
    assert(item != NULL);
    AutoPtr<ICharSequence> imeName = item->mImeName;
    AutoPtr<ICharSequence> subtypeName = item->mSubtypeName;
    AutoPtr<ITextView> firstTextView;
    view->FindViewById(R::id::text1, (IView**)&firstTextView);
    AutoPtr<ITextView> secondTextView;
    view->FindViewById(R::id::text2, (IView**)&secondTextView);
    if (TextUtils::IsEmpty(subtypeName)) {
        firstTextView->SetText(imeName);
        secondTextView->SetVisibility(IView::GONE);
    }
    else {
        firstTextView->SetText(subtypeName);
        secondTextView->SetText(imeName);
        secondTextView->SetVisibility(IView::VISIBLE);
    }
    AutoPtr<IRadioButton> radioButton;
    view->FindViewById(R::id::radio, (IView**)&radioButton);
    assert(ICheckable::Probe(radioButton) != NULL);
    ICheckable::Probe(radioButton)->SetChecked(position == mCheckedItem);
    return view;
}


///////////////////////////////////////////////////////////
// CInputMethodManagerService::InputMethodAndSubtypeListManager
///////////////////////////////////////////////////////////

CInputMethodManagerService::InputMethodAndSubtypeListManager::InputMethodAndSubtypeListManager(
    /* [in] */ IContext* context,
    /* [in] */ CInputMethodManagerService* imms)
    : mContext(context)
    , mImms(imms)
{
    context->GetPackageManager((IPackageManager**)&mPm);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IConfiguration> config;
    res->GetConfiguration((IConfiguration**)&config);
    AutoPtr<ILocale> locale;
    config->GetLocale((ILocale**)&locale);
    if (locale != NULL) {
        locale->ToString(&mSystemLocaleStr);
    }
    else {
        mSystemLocaleStr = "";
    }
}

AutoPtr<CInputMethodManagerService::ImeSubtypeListItem>
CInputMethodManagerService::InputMethodAndSubtypeListManager::GetNextInputMethod(
    /* [in] */ Boolean onlyCurrentIme,
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ IInputMethodSubtype* subtype)
{
    if (imi == NULL) {
        return NULL;
    }

    AutoPtr< List< AutoPtr<ImeSubtypeListItem> > > imList = GetSortedInputMethodAndSubtypeList();
    if (imList->GetSize() <= 1) {
        return NULL;
    }

    const Int32 N = imList->GetSize();
    Int32 hashCode;
    const Int32 currentSubtypeId = subtype != NULL
            ? mImms->GetSubtypeIdFromHashCode(imi, (subtype->GetHashCode(&hashCode), hashCode))
            : NOT_A_SUBTYPE_ID;
    List< AutoPtr<ImeSubtypeListItem> >::Iterator it;
    Int32 i;
    for (it = imList->Begin(), i = 0; it != imList->End(); ++it, ++i) {
        AutoPtr<ImeSubtypeListItem> isli = *it;
        Boolean e = FALSE;
        if ((isli->mImi->Equals(imi, &e), e) && isli->mSubtypeId == currentSubtypeId) {
            if (!onlyCurrentIme) {
                return (*imList)[(i + 1) % N];
            }
            for (Int32 j = 0; j < N - 1; ++j) {
                AutoPtr<ImeSubtypeListItem> candidate = (*imList)[(i + j + 1) % N];
                if (candidate->mImi->Equals(imi, &e), e) {
                    return candidate;
                }
            }
            return NULL;
        }
    }
    return NULL;
}

AutoPtr< List< AutoPtr<CInputMethodManagerService::ImeSubtypeListItem> > >
CInputMethodManagerService::InputMethodAndSubtypeListManager::GetSortedInputMethodAndSubtypeList()
{
    return GetSortedInputMethodAndSubtypeList(TRUE, FALSE, FALSE);
}

AutoPtr< List< AutoPtr<CInputMethodManagerService::ImeSubtypeListItem> > >
CInputMethodManagerService::InputMethodAndSubtypeListManager::GetSortedInputMethodAndSubtypeList(
    /* [in] */ Boolean showSubtypes,
    /* [in] */ Boolean inputShown,
    /* [in] */ Boolean isScreenLocked)
{
    assert(0);
    //TODO
    // final ArrayList<ImeSubtypeListItem> imList = new ArrayList<ImeSubtypeListItem>();
    // final HashMap<InputMethodInfo, List<InputMethodSubtype>> immis =
    //         mImms.getExplicitlyOrImplicitlyEnabledInputMethodsAndSubtypeListLocked();
    // if (immis == NULL || immis.size() == 0) {
    //     return Collections.emptyList();
    // }
    // mSortedImmis.clear();
    // mSortedImmis.putAll(immis);
    // for (InputMethodInfo imi : mSortedImmis.keySet()) {
    //     if (imi == NULL) continue;
    //     List<InputMethodSubtype> explicitlyOrImplicitlyEnabledSubtypeList = immis.get(imi);
    //     HashSet<String> enabledSubtypeSet = new HashSet<String>();
    //     for (InputMethodSubtype subtype: explicitlyOrImplicitlyEnabledSubtypeList) {
    //         enabledSubtypeSet.add(String.valueOf(subtype.hashCode()));
    //     }
    //     ArrayList<InputMethodSubtype> subtypes = getSubtypes(imi);
    //     final CharSequence imeLabel = imi.loadLabel(mPm);
    //     if (showSubtypes && enabledSubtypeSet.size() > 0) {
    //         final int subtypeCount = imi.getSubtypeCount();
    //         if (DEBUG) {
    //             Slog.v(TAG, "Add subtypes: " + subtypeCount + ", " + imi.getId());
    //         }
    //         for (int j = 0; j < subtypeCount; ++j) {
    //             final InputMethodSubtype subtype = imi.getSubtypeAt(j);
    //             final String subtypeHashCode = String.valueOf(subtype.hashCode());
    //             // We show all enabled IMEs and subtypes when an IME is shown.
    //             if (enabledSubtypeSet.contains(subtypeHashCode)
    //                     && ((inputShown && !isScreenLocked) || !subtype.isAuxiliary())) {
    //                 final CharSequence subtypeLabel =
    //                         subtype.overridesImplicitlyEnabledSubtype() ? NULL
    //                                 : subtype.getDisplayName(mContext, imi.getPackageName(),
    //                                         imi.getServiceInfo().applicationInfo);
    //                 imList.add(new ImeSubtypeListItem(imeLabel, subtypeLabel, imi, j,
    //                         subtype.getLocale(), mSystemLocaleStr));

    //                 // Removing this subtype from enabledSubtypeSet because we no longer
    //                 // need to add an entry of this subtype to imList to avoid duplicated
    //                 // entries.
    //                 enabledSubtypeSet.remove(subtypeHashCode);
    //             }
    //         }
    //     } else {
    //         imList.add(new ImeSubtypeListItem(imeLabel, NULL, imi, NOT_A_SUBTYPE_ID,
    //                 NULL, mSystemLocaleStr));
    //     }
    // }
    // Collections.sort(imList);
    // return imList;

    return NULL;
}


///////////////////////////////////////////////////////////
// CInputMethodManagerService::InputMethodSettings
///////////////////////////////////////////////////////////

const Char32 CInputMethodManagerService::InputMethodSettings::INPUT_METHOD_SEPARATER = ':';
const Char32 CInputMethodManagerService::InputMethodSettings::INPUT_METHOD_SUBTYPE_SEPARATER = ';';

CInputMethodManagerService::InputMethodSettings::InputMethodSettings(
    /* [in] */ IResources* res,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ HashMap<String, AutoPtr<IInputMethodInfo> >* methodMap,
    /* [in] */ InputMethodInfoList* methodList,
    /* [in] */ Int32 userId,
    /* [in] */ CInputMethodManagerService* host)
    : mRes(res)
    , mResolver(resolver)
    , mMethodMap(methodMap)
    , mMethodList(methodList)
    , mCurrentUserId(0)
    , mHost(host)
{
    SetCurrentUserId(userId);
    mInputMethodSplitter = new TextUtils::SimpleStringSplitter(INPUT_METHOD_SEPARATER);
    mSubtypeSplitter = new TextUtils::SimpleStringSplitter(INPUT_METHOD_SUBTYPE_SEPARATER);
}

CInputMethodManagerService::InputMethodSettings::~InputMethodSettings()
{
}

void CInputMethodManagerService::InputMethodSettings::BuildEnabledInputMethodsSettingString(
    /* [in] */ StringBuilder& builder,
    /* [in] */ StringStringListPair* pair)
{
    String id = pair->mFirst;
    AutoPtr<List<String> > subtypes = pair->mSecond;
    builder.AppendString(id);
    // Inputmethod and subtypes are saved in the settings as follows:
    // ime0;subtype0;subtype1:ime1;subtype0:ime2:ime3;subtype0;subtype1
    List<String>::Iterator it;
    for (it = subtypes->Begin(); it != subtypes->End(); ++it) {
        String subtypeId = *it;
        builder.AppendChar(INPUT_METHOD_SUBTYPE_SEPARATER);
        builder.AppendString(subtypeId);
    }
}

void CInputMethodManagerService::InputMethodSettings::SetCurrentUserId(
    /* [in] */ Int32 userId)
{
    // if (DEBUG) {
    //     Slog.d(TAG, "--- Swtich the current user from " + mCurrentUserId + " to "
    //             + userId + ", new ime = " + getSelectedInputMethod());
    // }
    // IMMS settings are kept per user, so keep track of current user
    mCurrentUserId = userId;
}

AutoPtr< InputMethodInfoList >
CInputMethodManagerService::InputMethodSettings::GetEnabledInputMethodListLocked()
{
    AutoPtr< StringStringListPairList > isList = GetEnabledInputMethodsAndSubtypeListLocked();
    AutoPtr< InputMethodInfoList > imList = CreateEnabledInputMethodListLocked(isList);
    return imList;
}

AutoPtr< InputMethodInfoStingListPairList >
CInputMethodManagerService::InputMethodSettings::GetEnabledInputMethodAndSubtypeHashCodeListLocked()
{
    AutoPtr< StringStringListPairList > isList = GetEnabledInputMethodsAndSubtypeListLocked();
    AutoPtr< InputMethodInfoStingListPairList > imList = CreateEnabledInputMethodAndSubtypeHashCodeListLocked(isList);
    return imList;
}

AutoPtr< InputMethodSubtypeList >
CInputMethodManagerService::InputMethodSettings::GetEnabledInputMethodSubtypeListLocked(
    /* [in] */ IInputMethodInfo* imi)
{
    AutoPtr< StringStringListPairList > imsList = GetEnabledInputMethodsAndSubtypeListLocked();
    AutoPtr< InputMethodSubtypeList > enabledSubtypes = new InputMethodSubtypeList();
    if (imi != NULL) {
        assert(imsList != NULL);
        StringStringListPairListIterator it;
        for (it = imsList->Begin(); it != imsList->End(); ++it) {
            AutoPtr<StringStringListPair> imsPair = *it;
            AutoPtr<IInputMethodInfo> info;
            HashMap<String, AutoPtr<IInputMethodInfo> >::Iterator mmit = mMethodMap->Find(imsPair->mFirst);
            if (mmit != mMethodMap->End()) {
                info = mmit->mSecond;
            }
            String id1, id2;
            if (info != NULL && (info->GetId(&id1), imi->GetId(&id2), id1.Equals(id2))) {
                Int32 subtypeCount;
                info->GetSubtypeCount(&subtypeCount);
                for (Int32 i = 0; i < subtypeCount; ++i) {
                    AutoPtr<IInputMethodSubtype> ims;
                    info->GetSubtypeAt(i, (IInputMethodSubtype**)&ims);
                    List<String>::Iterator lit;
                    for (lit = imsPair->mSecond->Begin(); lit != imsPair->mSecond->End(); ++lit) {
                        String s = *lit;
                        Int32 hashCode;
                        ims->GetHashCode(&hashCode);
                        if (StringUtils::Int32ToString(hashCode).Equals(s)) {
                            enabledSubtypes->PushBack(ims);
                        }
                    }
                }
                break;
            }
        }
    }

    return enabledSubtypes;
}

void CInputMethodManagerService::InputMethodSettings::EnableAllIMEsIfThereIsNoEnabledIME()
{
    String enabledInputMethodsStr = GetEnabledInputMethodsStr();
    if (enabledInputMethodsStr.IsNullOrEmpty()) {
        StringBuilder sb;
        InputMethodInfoListIterator it;
        for (it = mMethodList->Begin(); it != mMethodList->End(); ++it) {
            AutoPtr<IInputMethodInfo> imi = *it;
            // Slog.i(TAG, "Adding: " + imi.getId());
            if (it != mMethodList->Begin()) sb.AppendChar(':');
            String id;
            imi->GetId(&id);
            sb.Append(id);
        }
        PutEnabledInputMethodsStr(sb.ToString());
    }
}

AutoPtr< StringStringListPairList >
CInputMethodManagerService::InputMethodSettings::GetEnabledInputMethodsAndSubtypeListLocked()
{
    AutoPtr< StringStringListPairList > imsList = new StringStringListPairList();
    String enabledInputMethodsStr = GetEnabledInputMethodsStr();
    if (enabledInputMethodsStr.IsNullOrEmpty()) {
        return imsList;
    }
    mInputMethodSplitter->SetString(enabledInputMethodsStr);
    Boolean has = FALSE;
    while (mInputMethodSplitter->HasNext(&has), has) {
        String nextImsStr;
        mInputMethodSplitter->Next(&nextImsStr);
        mSubtypeSplitter->SetString(nextImsStr);
        if (mSubtypeSplitter->HasNext(&has), has) {
            AutoPtr<List<String> > subtypeHashes = new List<String>();
            // The first element is ime id.
            String imeId;
            mSubtypeSplitter->Next(&imeId);
            while (mSubtypeSplitter->HasNext(&has), has) {
                String str;
                mSubtypeSplitter->Next(&str);
                subtypeHashes->PushBack(str);
            }
            AutoPtr<StringStringListPair> pair = new StringStringListPair(imeId, subtypeHashes);
            imsList->PushBack(pair);
        }
    }
    return imsList;
}

void CInputMethodManagerService::InputMethodSettings::AppendAndPutEnabledInputMethodLocked(
    /* [in] */ const String& id,
    /* [in] */ Boolean reloadInputMethodStr)
{
    if (reloadInputMethodStr) {
        GetEnabledInputMethodsStr();
    }
    if (mEnabledInputMethodsStrCache.IsNullOrEmpty()) {
        // Add in the newly enabled input method.
        PutEnabledInputMethodsStr(id);
    }
    else {
        PutEnabledInputMethodsStr(
            mEnabledInputMethodsStrCache + StringUtils::Int32ToString(INPUT_METHOD_SEPARATER) + id);
    }
}

Boolean CInputMethodManagerService::InputMethodSettings::BuildAndPutEnabledInputMethodsStrRemovingIdLocked(
    /* [in] */ StringBuilder& builder,
    /* [in] */ StringStringListPairList * imsList,
    /* [in] */ const String& id)
{
    Boolean isRemoved = FALSE;
    Boolean needsAppendSeparator = FALSE;
    StringStringListPairListIterator it;
    for (it = imsList->Begin(); it != imsList->End(); ++it) {
        AutoPtr<StringStringListPair> ims = *it;
        String curId = ims->mFirst;
        if (curId.Equals(id)) {
            // We are disabling this input method, and it is
            // currently enabled.  Skip it to remove from the
            // new list.
            isRemoved = TRUE;
        }
        else {
            if (needsAppendSeparator) {
                builder.AppendChar(INPUT_METHOD_SEPARATER);
            }
            else {
                needsAppendSeparator = TRUE;
            }
            BuildEnabledInputMethodsSettingString(builder, ims);
        }
    }
    if (isRemoved) {
        // Update the setting with the new list of input methods.
        PutEnabledInputMethodsStr(builder.ToString());
    }
    return isRemoved;
}

AutoPtr< InputMethodInfoList >
CInputMethodManagerService::InputMethodSettings::CreateEnabledInputMethodListLocked(
    /* [in] */ StringStringListPairList * imsList)
{
    AutoPtr< InputMethodInfoList > res = new InputMethodInfoList();
    StringStringListPairListIterator it;
    for (it = imsList->Begin(); it != imsList->End(); ++it) {
        AutoPtr<StringStringListPair> ims = *it;
        HashMap<String, AutoPtr<IInputMethodInfo> >::Iterator mmit = mMethodMap->Find(ims->mFirst);
        if (mmit != mMethodMap->End()) {
            res->PushBack(mmit->mSecond);
        }
    }
    return res;
}

AutoPtr< InputMethodInfoStingListPairList >
CInputMethodManagerService::InputMethodSettings::CreateEnabledInputMethodAndSubtypeHashCodeListLocked(
    /* [in] */ StringStringListPairList * imsList)
{
    AutoPtr< InputMethodInfoStingListPairList > res = new InputMethodInfoStingListPairList();
    StringStringListPairListIterator it;
    for (it = imsList->Begin(); it != imsList->End(); ++it) {
        AutoPtr<StringStringListPair> ims = *it;
        HashMap<String, AutoPtr<IInputMethodInfo> >::Iterator mmit = mMethodMap->Find(ims->mFirst);
        if (mmit != mMethodMap->End()) {
            AutoPtr<InputMethodInfoStingListPair> pair = new InputMethodInfoStingListPair(mmit->mSecond, ims->mSecond);
            res->PushBack(pair);
        }
    }
    return res;
}

void CInputMethodManagerService::InputMethodSettings::PutEnabledInputMethodsStr(
    /* [in] */ const String& str)
{
    Boolean tmp = FALSE;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->PutStringForUser(
            mResolver, ISettingsSecure::ENABLED_INPUT_METHODS,
            str, mCurrentUserId, &tmp);
    mEnabledInputMethodsStrCache = str;
    // if (DEBUG) {
    //     Slog.d(TAG, "putEnabledInputMethodStr: " + str);
    // }
}

String CInputMethodManagerService::InputMethodSettings::GetEnabledInputMethodsStr()
{
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->GetStringForUser(
            mResolver, ISettingsSecure::ENABLED_INPUT_METHODS,
            mCurrentUserId, &mEnabledInputMethodsStrCache);
    // if (DEBUG) {
    //     Slog.d(TAG, "getEnabledInputMethodsStr: " + mEnabledInputMethodsStrCache
    //             + ", " + mCurrentUserId);
    // }

    return mEnabledInputMethodsStrCache;
}

void CInputMethodManagerService::InputMethodSettings::SaveSubtypeHistory(
    /* [in] */ StringStringPairList * savedImes,
    /* [in] */ const String& newImeId,
    /* [in] */ const String& newSubtypeId)
{
    StringBuilder builder;
    Boolean isImeAdded = FALSE;
    if (!newImeId.IsNullOrEmpty() && !newSubtypeId.IsNullOrEmpty()) {
        builder.AppendString(newImeId);
        builder.AppendChar(INPUT_METHOD_SUBTYPE_SEPARATER);
        builder.AppendString(newSubtypeId);
        isImeAdded = TRUE;
    }
    StringStringPairListIterator it;
    for (it = savedImes->Begin(); it != savedImes->End(); ++it) {
        AutoPtr<StringStringPair> ime = *it;
        String imeId = ime->mFirst;
        String subtypeId = ime->mSecond;
        if (subtypeId.IsNullOrEmpty()) {
            subtypeId = NOT_A_SUBTYPE_ID_STR;
        }
        if (isImeAdded) {
            builder.AppendChar(INPUT_METHOD_SEPARATER);
        }
        else {
            isImeAdded = TRUE;
        }
        builder.AppendString(imeId);
        builder.AppendChar(INPUT_METHOD_SUBTYPE_SEPARATER);
        builder.AppendString(subtypeId);
    }
    // Remove the last INPUT_METHOD_SEPARATER
    PutSubtypeHistoryStr(builder.ToString());
}

void CInputMethodManagerService::InputMethodSettings::AddSubtypeToHistory(
    /* [in] */ const String& imeId,
    /* [in] */ const String& subtypeId)
{
    AutoPtr< StringStringPairList > subtypeHistory = LoadInputMethodAndSubtypeHistoryLocked();
    if (subtypeHistory != NULL) {
        StringStringPairListIterator it;
        for (it = subtypeHistory->Begin(); it != subtypeHistory->End(); ++it) {
            AutoPtr<StringStringPair> ime = *it;
            if (ime->mFirst.Equals(imeId)) {
                // if (DEBUG) {
                //     Slog.v(TAG, "Subtype found in the history: " + imeId + ", "
                //             + ime.second);
                // }
                // We should break here
                subtypeHistory->Erase(it);
                break;
            }
        }

        // if (DEBUG) {
        //     Slog.v(TAG, "Add subtype to the history: " + imeId + ", " + subtypeId);
        // }
        SaveSubtypeHistory(subtypeHistory, imeId, subtypeId);
    }
}

void CInputMethodManagerService::InputMethodSettings::PutSubtypeHistoryStr(
    /* [in] */ const String& str)
{
    // if (DEBUG) {
    //     Slog.d(TAG, "putSubtypeHistoryStr: " + str);
    // }
    Boolean tmp = FALSE;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->PutStringForUser(
            mResolver, String("input_methods_subtype_history") /*ISettingsSecure::INPUT_METHODS_SUBTYPE_HISTORY*/,
            str, mCurrentUserId, &tmp);
}

AutoPtr<StringStringPair>
CInputMethodManagerService::InputMethodSettings::GetLastInputMethodAndSubtypeLocked()
{
    // Gets the first one from the history
    return GetLastSubtypeForInputMethodLockedInternal(String(NULL));
}

String CInputMethodManagerService::InputMethodSettings::GetLastSubtypeForInputMethodLocked(
    /* [in] */ const String& imeId)
{
    AutoPtr<StringStringPair> ime = GetLastSubtypeForInputMethodLockedInternal(imeId);
    String result;
    if (ime != NULL) {
        result = ime->mSecond;
    }

    return result;
}

AutoPtr<StringStringPair>
CInputMethodManagerService::InputMethodSettings::GetLastSubtypeForInputMethodLockedInternal(
    /* [in] */ const String& imeId)
{
    AutoPtr<StringStringPair> result;
    AutoPtr< StringStringListPairList > enabledImes = GetEnabledInputMethodsAndSubtypeListLocked();
    AutoPtr< StringStringPairList > subtypeHistory = LoadInputMethodAndSubtypeHistoryLocked();
    StringStringPairListIterator it;
    for (it = subtypeHistory->Begin(); it != subtypeHistory->End(); ++it) {
        AutoPtr<StringStringPair> imeAndSubtype = *it;
        String imeInTheHistory = imeAndSubtype->mFirst;
        // If imeId is empty, returns the first IME and subtype in the history
        if (imeId.IsNullOrEmpty() || imeInTheHistory.Equals(imeId)) {
            String subtypeInTheHistory = imeAndSubtype->mSecond;
            String subtypeHashCode = GetEnabledSubtypeHashCodeForInputMethodAndSubtypeLocked(
                    enabledImes, imeInTheHistory, subtypeInTheHistory);
            if (!subtypeHashCode.IsNullOrEmpty()) {
                // if (DEBUG) {
                //     Slog.d(TAG, "Enabled subtype found in the history: " + subtypeHashCode);
                // }
                result = new StringStringPair(imeInTheHistory, subtypeHashCode);
            }
        }
    }

    // if (DEBUG) {
    //     Slog.d(TAG, "No enabled IME found in the history");
    // }
    return result;
}

String CInputMethodManagerService::InputMethodSettings::GetEnabledSubtypeHashCodeForInputMethodAndSubtypeLocked(
    /* [in] */ StringStringListPairList * enabledImes,
    /* [in] */ const String& imeId,
    /* [in] */ const String& subtypeHashCode)
{
    StringStringListPairListIterator it;
    for (it = enabledImes->Begin(); it != enabledImes->End(); ++it) {
        AutoPtr<StringStringListPair> enabledIme = *it;
        if (enabledIme->mFirst.Equals(imeId)) {
            AutoPtr< List<String> > explicitlyEnabledSubtypes = enabledIme->mSecond;
            AutoPtr<IInputMethodInfo> imi;
            HashMap<String, AutoPtr<IInputMethodInfo> >::Iterator mmit = mMethodMap->Find(imeId);
            if (mmit != mMethodMap->End()) {
                imi = mmit->mSecond;
            }
            if (explicitlyEnabledSubtypes->IsEmpty()) {
                // If there are no explicitly enabled subtypes, applicable subtypes are
                // enabled implicitly.
                // If IME is enabled and no subtypes are enabled, applicable subtypes
                // are enabled implicitly, so needs to treat them to be enabled.
                Int32 subtypeCount = 0;
                if (imi != NULL && (imi->GetSubtypeCount(&subtypeCount), subtypeCount) > 0) {
                    AutoPtr< InputMethodSubtypeList > implicitlySelectedSubtypes =
                            GetImplicitlyApplicableSubtypesLocked(mRes, imi);
                    if (implicitlySelectedSubtypes != NULL) {
                        InputMethodSubtypeListIterator ssit;
                        for (ssit = implicitlySelectedSubtypes->Begin(); ssit != implicitlySelectedSubtypes->End(); ++ssit) {
                            AutoPtr<IInputMethodSubtype> st = *ssit;
                            Int32 hashCode;
                            st->GetHashCode(&hashCode);
                            if (StringUtils::Int32ToString(hashCode).Equals(subtypeHashCode)) {
                                return subtypeHashCode;
                            }
                        }
                    }
                }
            }
            else {
                List<String>::Iterator sit;
                for (sit = explicitlyEnabledSubtypes->Begin(); sit != explicitlyEnabledSubtypes->End(); ++sit) {
                    String s = *sit;
                    if (s.Equals(subtypeHashCode)) {
                        // If both imeId and subtypeId are enabled, return subtypeId.
                        // try {
                        const Int32 hashCode = StringUtils::ParseInt32(subtypeHashCode);
                        // Check whether the subtype id is valid or not
                        if (IsValidSubtypeId(imi, hashCode)) {
                            return s;
                        }
                        else {
                            return NOT_A_SUBTYPE_ID_STR;
                        }
                        // } catch (NumberFormatException e) {
                        //     return NOT_A_SUBTYPE_ID_STR;
                        // }
                    }
                }
            }
            // If imeId was enabled but subtypeId was disabled.
            return NOT_A_SUBTYPE_ID_STR;
        }
    }
    // If both imeId and subtypeId are disabled, return NULL
    return String(NULL);
}

AutoPtr< StringStringPairList >
CInputMethodManagerService::InputMethodSettings::LoadInputMethodAndSubtypeHistoryLocked()
{
    AutoPtr< StringStringPairList > imsList = new StringStringPairList();
    String subtypeHistoryStr = GetSubtypeHistoryStr();
    if (subtypeHistoryStr.IsNullOrEmpty()) {
        return imsList;
    }
    mInputMethodSplitter->SetString(subtypeHistoryStr);
    Boolean hasNext = FALSE;
    while (mInputMethodSplitter->HasNext(&hasNext), hasNext) {
        String nextImsStr;
        mInputMethodSplitter->Next(&nextImsStr);
        mSubtypeSplitter->SetString(nextImsStr);
        if (mSubtypeSplitter->HasNext(&hasNext), hasNext) {
            String subtypeId = NOT_A_SUBTYPE_ID_STR;
            // The first element is ime id.
            String imeId;
            mSubtypeSplitter->Next(&imeId);
            while (mSubtypeSplitter->HasNext(&hasNext), hasNext) {
                mSubtypeSplitter->Next(&subtypeId);
                break;
            }
            AutoPtr<StringStringPair> pair = new StringStringPair(imeId, subtypeId);
            imsList->PushBack(pair);
        }
    }
    return imsList;
}

String CInputMethodManagerService::InputMethodSettings::GetSubtypeHistoryStr()
{
    // if (DEBUG) {
    //     Slog.d(TAG, "getSubtypeHistoryStr: " + Settings.Secure.getStringForUser(
    //             mResolver, Settings.Secure.INPUT_METHODS_SUBTYPE_HISTORY, mCurrentUserId));
    // }
    String str;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->GetStringForUser(
            mResolver, String("input_methods_subtype_history") /*ISettingsSecure::INPUT_METHODS_SUBTYPE_HISTORY*/,
            mCurrentUserId, &str);
    return str;
}

void CInputMethodManagerService::InputMethodSettings::PutSelectedInputMethod(
    /* [in] */ const String& imeId)
{
    // if (DEBUG) {
    //     Slog.d(TAG, "putSelectedInputMethodStr: " + imeId + ", "
    //             + mCurrentUserId);
    // }
    Boolean tmp = FALSE;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->PutStringForUser(
            mResolver, ISettingsSecure::DEFAULT_INPUT_METHOD,
            imeId, mCurrentUserId, &tmp);
}

void CInputMethodManagerService::InputMethodSettings::PutSelectedSubtype(
    /* [in] */ Int32 subtypeId)
{
    // if (DEBUG) {
    //     Slog.d(TAG, "putSelectedInputMethodSubtypeStr: " + subtypeId + ", "
    //             + mCurrentUserId);
    // }
    Boolean tmp = FALSE;
    AutoPtr<ISettingsSystem> settingsSystem;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSystem);
    settingsSystem->PutInt32ForUser(mResolver, String("selected_input_method_subtype") /*ISettingsSecure::SELECTED_INPUT_METHOD_SUBTYPE*/,
            subtypeId, mCurrentUserId, &tmp);
}

String CInputMethodManagerService::InputMethodSettings::GetDisabledSystemInputMethods()
{
    String inputMethods;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->GetStringForUser(
            mResolver, ISettingsSecure::DISABLED_SYSTEM_INPUT_METHODS,
            mCurrentUserId, &inputMethods);
    return inputMethods;
}

String CInputMethodManagerService::InputMethodSettings::GetSelectedInputMethod()
{
    // if (DEBUG) {
    //     Slog.d(TAG, "getSelectedInputMethodStr: " + Settings.Secure.getStringForUser(
    //             mResolver, Settings.Secure.DEFAULT_INPUT_METHOD, mCurrentUserId)
    //             + ", " + mCurrentUserId);
    // }
    String inputMethod;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->GetStringForUser(
            mResolver, ISettingsSecure::DEFAULT_INPUT_METHOD,
            mCurrentUserId, &inputMethod);
    return inputMethod;
}

Int32 CInputMethodManagerService::InputMethodSettings::GetSelectedInputMethodSubtypeHashCode()
{
    // try {
    Int32 value = 0;
    AutoPtr<ISettingsSystem> settingsSystem;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSystem);
    ECode ec = settingsSystem->GetInt32ForUser(
            mResolver, ISettingsSecure::SELECTED_INPUT_METHOD_SUBTYPE,
            mCurrentUserId, &value);
    if (FAILED(ec)/*SettingNotFoundException*/) {
        return NOT_A_SUBTYPE_ID;
    }
    return value;
    // } catch (SettingNotFoundException e) {
    //     return NOT_A_SUBTYPE_ID;
    // }
}

Int32 CInputMethodManagerService::InputMethodSettings::GetCurrentUserId()
{
    return mCurrentUserId;
}


///////////////////////////////////////////////////////////
// CInputMethodManagerService::InputMethodFileManager
///////////////////////////////////////////////////////////

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
    /* [in] */ HashMap<String, AutoPtr<IInputMethodInfo> >* methodMap,
    /* [in] */ Int32 userId,
    /* [in] */ CInputMethodManagerService*  host)
    : mHost(host)
{
    if (methodMap == NULL) {
        assert(0);
        // throw new NullPointerException("methodMap is NULL");
    }
    mMethodMap = methodMap;
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
        // Slog.w(TAG, "Couldn't create dir.: " + inputMethodDir.getAbsolutePath());
    }
    AutoPtr<IFile> subtypeFile;
    CFile::New(inputMethodDir, ADDITIONAL_SUBTYPES_FILE_NAME, (IFile**)&subtypeFile);
    CAtomicFile::New(subtypeFile, (IAtomicFile**)&mAdditionalInputMethodSubtypeFile);
    if (!(subtypeFile->Exists(&result), result)) {
        // If "subtypes.xml" doesn't exist, create a blank file.
        WriteAdditionalInputMethodSubtypes(
                &mAdditionalSubtypesMap, mAdditionalInputMethodSubtypeFile, methodMap);
    }
    else {
        ReadAdditionalInputMethodSubtypes(
                &mAdditionalSubtypesMap, mAdditionalInputMethodSubtypeFile);
    }
}

void CInputMethodManagerService::InputMethodFileManager::DeleteAllInputMethodSubtypes(
    /* [in] */ const String& imiId)
{
    {
        AutoLock lock(mMethodMapLock);

        mAdditionalSubtypesMap.Erase(imiId);
        WriteAdditionalInputMethodSubtypes(
                &mAdditionalSubtypesMap, mAdditionalInputMethodSubtypeFile, mMethodMap);
    }
}

void CInputMethodManagerService::InputMethodFileManager::AddInputMethodSubtypes(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ ArrayOf<IInputMethodSubtype*>* additionalSubtypes)
{
    {
        AutoLock lock(mMethodMapLock);

        AutoPtr< InputMethodSubtypeList > subtypes = new InputMethodSubtypeList();
        const Int32 N = additionalSubtypes->GetLength();
        for (Int32 i = 0; i < N; ++i) {
            AutoPtr<IInputMethodSubtype> subtype = (*additionalSubtypes)[i];
            InputMethodSubtypeListIterator it =
                    Find(subtypes->Begin(), subtypes->End(), subtype);
            if (it == subtypes->End()) {
                subtypes->PushBack(subtype);
            }
            else {
                // Slog.w(TAG, "Duplicated subtype definition found: "
                //         + subtype.getLocale() + ", " + subtype.getMode());
            }
        }
        String id;
        imi->GetId(&id);
        mAdditionalSubtypesMap[id] = subtypes;
        WriteAdditionalInputMethodSubtypes(
                &mAdditionalSubtypesMap, mAdditionalInputMethodSubtypeFile, mMethodMap);
    }
}

AutoPtr<IObjectStringMap>
CInputMethodManagerService::InputMethodFileManager::GetAllAdditionalInputMethodSubtypes()
{
    AutoLock lock(mMethodMapLock);
    AutoPtr<IObjectStringMap> map;
    CObjectStringMap::New((IObjectStringMap**)&map);

    String key;
    InputMethodSubtypeList* list;
    IInputMethodSubtype* subtype;
    HashMap<String, AutoPtr< InputMethodSubtypeList > >::Iterator it;
    InputMethodSubtypeListIterator lit;
    for (it = mAdditionalSubtypesMap.Begin(); it != mAdditionalSubtypesMap.End(); ++it) {
        key = it->mFirst;
        list = it->mSecond;

        AutoPtr<IObjectContainer> value;
        CObjectContainer::New((IObjectContainer**)&value);
        for (lit = list->Begin(); lit != list->End(); ++lit) {
            subtype = *lit;
            value->Add(subtype->Probe(EIID_IInterface));
        }

        map->Put(key, value->Probe(EIID_IInterface));
    }
    return map;
}

void CInputMethodManagerService::InputMethodFileManager::WriteAdditionalInputMethodSubtypes(
    /* [in] */ HashMap<String, AutoPtr< InputMethodSubtypeList > >* allSubtypes,
    /* [in] */ IAtomicFile* subtypesFile,
    /* [in] */ HashMap<String, AutoPtr<IInputMethodInfo> >* methodMap)
{
    // Safety net for the case that this function is called before methodMap is set.
    Boolean isSetMethodMap = methodMap != NULL && methodMap->IsEmpty() == FALSE;
    AutoPtr<IFileOutputStream> fos;

    // try {
    subtypesFile->StartWrite((IFileOutputStream**)&fos);
    AutoPtr<IFastXmlSerializer> out;
    CFastXmlSerializer::New((IFastXmlSerializer**)&out);
    assert(out != NULL);

    out->SetOutput(fos, String("utf-8"));

    AutoPtr<IBoolean> standalone;
    CBoolean::New(TRUE, (IBoolean**)&standalone);
    out->StartDocument(String(NULL), standalone);
    out->SetFeature(String("http://xmlpull.org/v1/doc/features.html#indent-output"), TRUE);
    out->WriteStartTag(String(NULL), NODE_SUBTYPES);

    assert(allSubtypes != NULL);
    HashMap<String, AutoPtr< InputMethodSubtypeList > >::Iterator itKey = allSubtypes->Begin();

    for (; itKey != allSubtypes->End(); itKey++) {
        String imiId = itKey->mFirst;

        Boolean containsKey = FALSE;
        HashMap<String, AutoPtr<IInputMethodInfo> >::Iterator ator = methodMap->Find(imiId);
        if (ator != methodMap->End()) {
            containsKey = TRUE;
        }

        if (isSetMethodMap && !containsKey) {
            // Slog.w(TAG, "IME uninstalled or not valid.: " + imiId);
            continue;
        }

        out->WriteStartTag(String(NULL), NODE_IMI);
        out->WriteAttribute(String(NULL), ATTR_ID, imiId);

        AutoPtr< InputMethodSubtypeList > subtypesList = (*allSubtypes)[imiId];

        InputMethodSubtypeList::Iterator it = subtypesList->Begin();
        for (; it != subtypesList->End(); it++) {
            AutoPtr<IInputMethodSubtype> subtype = *it;

            out->WriteStartTag(String(NULL), NODE_SUBTYPE);

            Int32 id = 0;
            out->WriteAttribute(String(NULL), ATTR_ICON, StringUtils::Int32ToString(subtype->GetIconResId(&id), id));
            out->WriteAttribute(String(NULL), ATTR_LABEL, StringUtils::Int32ToString(subtype->GetNameResId(&id), id));

            String value;
            out->WriteAttribute(String(NULL), ATTR_IME_SUBTYPE_LOCALE, (subtype->GetLocale(&value), value));
            out->WriteAttribute(String(NULL), ATTR_IME_SUBTYPE_MODE, (subtype->GetMode(&value), value));
            out->WriteAttribute(String(NULL), ATTR_IME_SUBTYPE_EXTRA_VALUE, (subtype->GetExtraValue(&value), value));

            Boolean isAuxiliary = FALSE;
            out->WriteAttribute(String(NULL), ATTR_IS_AUXILIARY,
                    StringUtils::Int32ToString((subtype->IsAuxiliary(&isAuxiliary), isAuxiliary) ? 1 : 0));
            out->WriteEndTag(String(NULL), NODE_SUBTYPE);
        }

        out->WriteEndTag(String(NULL), NODE_IMI);
    }

    out->WriteEndTag(String(NULL), NODE_SUBTYPES);
    out->EndDocument();
    subtypesFile->FinishWrite(fos);
    // } catch (java.io.IOException e) {
    //     Slog.w(TAG, "Error writing subtypes", e);
    //     if (fos != NULL) {
    //         subtypesFile.failWrite(fos);
    //     }
    // }
}

void CInputMethodManagerService::InputMethodFileManager::ReadAdditionalInputMethodSubtypes(
    /* [in] */ HashMap<String, AutoPtr< InputMethodSubtypeList > >* allSubtypes,
    /* [in] */ IAtomicFile* subtypesFile)
{
    if (allSubtypes == NULL || subtypesFile == NULL) return;

    allSubtypes->Clear();
    AutoPtr<IFileInputStream> fis;
    // // try {
    subtypesFile->OpenRead((IFileInputStream**)&fis);
    AutoPtr<IXmlPullParser> parser = Xml::NewPullParser();
    parser->SetInput(fis, String(NULL));
    Int32 type = 0;
    parser->GetEventType(&type);
    // Skip parsing until START_TAG
    Int32 nextType = 0;
    while ((parser->Next(&type), type) != IXmlPullParser::START_TAG
            && type != IXmlPullParser::END_DOCUMENT) {}

    String firstNodeName;
    parser->GetName(&firstNodeName);
    if (!NODE_SUBTYPES.Equals(firstNodeName)) {
        assert(0);
        // throw new XmlPullParserException("Xml doesn't start with subtypes");
    }

    Int32 depth = 0, nextDepth;
    parser->GetDepth(&depth);
    String currentImiId;
    AutoPtr< InputMethodSubtypeList > tempSubtypesArray;
    while (((parser->Next(&type), type) != IXmlPullParser::END_TAG
            || (parser->GetDepth(&nextDepth), nextDepth) > depth) && type != IXmlPullParser::END_DOCUMENT) {
        if (type != IXmlPullParser::START_TAG)
            continue;

        String nodeName;
        parser->GetName(&nodeName);
        if (NODE_IMI.Equals(nodeName) == 0) {
            parser->GetAttributeValue(String(NULL)/*NULL*/, ATTR_ID, &currentImiId);

            AutoPtr<ICharSequence> tmp;
            CString::New(currentImiId, (ICharSequence**)&tmp);
            if (TextUtils::IsEmpty(tmp.Get())) {
                // Slog.w(TAG, "Invalid imi id found in subtypes.xml");
                continue;
            }

            tempSubtypesArray = new InputMethodSubtypeList();
            (*allSubtypes)[currentImiId] = tempSubtypesArray;
        } else if (NODE_SUBTYPE.Equals(nodeName) == 0) {
            AutoPtr<ICharSequence> tmp;
            CString::New(currentImiId, (ICharSequence**)&tmp);

            if (TextUtils::IsEmpty(tmp.Get()) || tempSubtypesArray == NULL) {
                // Slog.w(TAG, "IME uninstalled or not valid.: " + currentImiId);
                continue;
            }

            String value;
            parser->GetAttributeValue(String(NULL), ATTR_ICON, &value);
            const Int32 icon = StringUtils::ParseInt32(value);

            parser->GetAttributeValue(String(NULL), ATTR_LABEL, &value);
            const Int32 label = StringUtils::ParseInt32(value);
            String imeSubtypeLocale;
            parser->GetAttributeValue(String(NULL), ATTR_IME_SUBTYPE_LOCALE, &imeSubtypeLocale);

            String imeSubtypeMode;
            parser->GetAttributeValue(String(NULL), ATTR_IME_SUBTYPE_MODE, &imeSubtypeMode);

            String imeSubtypeExtraValue;
            parser->GetAttributeValue(String(NULL), ATTR_IME_SUBTYPE_EXTRA_VALUE, &imeSubtypeExtraValue);

            Boolean isAuxiliary = String("1").Equals(
                (parser->GetAttributeValue(String(NULL), ATTR_IS_AUXILIARY, &value), value)) == 0;

            AutoPtr<IInputMethodSubtype> subtype;
            CInputMethodSubtype::New(label, icon, imeSubtypeLocale,
                    imeSubtypeMode, imeSubtypeExtraValue, isAuxiliary, (IInputMethodSubtype**)&subtype);
            tempSubtypesArray->PushBack(subtype);
        }
    }
    // // } catch (XmlPullParserException e) {
    // //     Slog.w(TAG, "Error reading subtypes: " + e);
    // //     return;
    // // } catch (java.io.IOException e) {
    // //     Slog.w(TAG, "Error reading subtypes: " + e);
    // //     return;
    // // } catch (NumberFormatException e) {
    // //     Slog.w(TAG, "Error reading subtypes: " + e);
    // //     return;
    // // }
    // // finally {
    if (fis != NULL) {
        // try {
        fis->Close();
        // } catch (java.io.IOException e1) {
        //     Slog.w(TAG, "Failed to close.");
        // }
    }
    // // }
}


///////////////////////////////////////////////////////////
// CInputMethodManagerService
///////////////////////////////////////////////////////////

CInputMethodManagerService::CInputMethodManagerService()
    : mVisibleConnection(new VisibleServiceConnection())
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
    , mBackDisposition(IInputMethodService::BACK_DISPOSITION_DEFAULT)
    , mImeWindowVis(0)
    , mWindowManagerService(NULL)
    , mSecureSuggestionSpans(20)
    , mShowOngoingImeSwitcherForPhones(FALSE)
    , mNotificationShown(FALSE)
    , mImeSelectedOnBoot(FALSE)
    , mShortcutInputMethodsAndSubtypes(20)
    , mInputBoundToKeyguard(FALSE)
{
    mMethodList = new InputMethodInfoList();
    mMethodMap = new HashMap<String, AutoPtr<IInputMethodInfo> >(10);
    ASSERT_SUCCEEDED(CInputBindResult::New(NULL, String(NULL), -1, (IInputBindResult**)&mNoBinding));
    mMyPackageMonitor = new MyPackageMonitor(this);
}

CInputMethodManagerService::~CInputMethodManagerService()
{
    mMethodList = NULL;
    mMethodMap = NULL;
    mClients.Clear();
    mSecureSuggestionSpans.Clear();
    mShortcutInputMethodsAndSubtypes.Clear();
}

ECode CInputMethodManagerService::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Handle32 windowManager)
{
    mIPackageManager = AppGlobals::GetPackageManager();
    mContext = context;
    context->GetResources((IResources**)&mRes);
    CHandler::New(THIS_PROBE(IHandlerCallback), FALSE, (IHandler**)&mHandler);
    mIWindowManager = IIWindowManager::Probe(ServiceManager::GetService(IContext::WINDOW_SERVICE));
    AutoPtr<ILooper> looper;
    context->GetMainLooper((ILooper**)&looper);
    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    AutoPtr<IHandlerCallerCallback> callback = new MyHandlerCallerCallback(wr);
    mCaller = new HandlerCaller(context, callback, TRUE);
    mWindowManagerService = (CWindowManagerService*)windowManager;
    mHardKeyboardListener = new HardKeyboardListener(this);

    CNotification::New((INotification**)&mImeSwitcherNotification);
    mImeSwitcherNotification->SetIcon(R::drawable::ic_notification_ime_default);
    mImeSwitcherNotification->SetWhen(0);
    mImeSwitcherNotification->SetFlags(INotification::FLAG_ONGOING_EVENT);
    mImeSwitcherNotification->SetTickerText(NULL);
    mImeSwitcherNotification->SetDefaults(0); // please be quiet
    mImeSwitcherNotification->SetSound(NULL);
    mImeSwitcherNotification->SetVibrate(NULL);

    // Tag this notification specially so SystemUI knows it's important
    AutoPtr< ArrayOf<String> > kind = ArrayOf<String>::Alloc(1);
    (*kind)[0] = "android.system.imeswitcher";
    mImeSwitcherNotification->SetKind(kind);

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
    AutoPtr<IIntent> stickyIntent;
    AutoPtr<IBroadcastReceiver> receiver = new ImmsBroadcastReceiver(this);
    mContext->RegisterReceiver(receiver, broadcastFilter, (IIntent**)&stickyIntent);

    mNotificationShown = FALSE;
    Int32 userId = 0;
    // try {
    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    AutoPtr<IUserSwitchObserver> observer;
    CInputMethodManagerServiceUserSwitchObserver::New((Handle32)this, (IUserSwitchObserver**)&observer);
    am->RegisterUserSwitchObserver(observer);
    AutoPtr<IUserInfo> user;
    am->GetCurrentUser((IUserInfo**)&user);
    user->GetId(&userId);
    // } catch (RemoteException e) {
    //     Slog.w(TAG, "Couldn't get current user ID; guessing it's 0", e);
    // }
    AutoPtr<IUserHandle> allUserHande;
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    helper->GetALL((IUserHandle**)&allUserHande);
    mMyPackageMonitor->Register(mContext, NULL, allUserHande, TRUE);

    // mSettings should be created before buildInputMethodListLocked
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    mSettings = new InputMethodSettings(mRes, resolver, mMethodMap, mMethodList, userId, this);
    mFileManager = new InputMethodFileManager(mMethodMap, userId, this);
    mImListManager = new InputMethodAndSubtypeListManager(context, this);

    // Just checking if defaultImiId is empty or not
    String defaultImiId = mSettings->GetSelectedInputMethod();
    mImeSelectedOnBoot = !defaultImiId.IsNullOrEmpty();

    BuildInputMethodListLocked(mMethodList, mMethodMap);
    mSettings->EnableAllIMEsIfThereIsNoEnabledIME();

   if (!mImeSelectedOnBoot) {
       // Slog.w(TAG, "No IME selected. Choose the most applicable IME.");
       ResetDefaultImeLocked(context);
   }
    mSettingsObserver = new SettingsObserver(mHandler, this);
    UpdateFromSettingsLocked();

    // IMMS wants to receive Intent.ACTION_LOCALE_CHANGED in order to update the current IME
    // according to the new system locale.
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_LOCALE_CHANGED);
    stickyIntent = NULL;
    AutoPtr<IBroadcastReceiver> checkReceiver = new CheckReceiver(this);
    return mContext->RegisterReceiver(checkReceiver, filter, (IIntent**)&stickyIntent);
}

void CInputMethodManagerService::ResetDefaultImeLocked(
    /* [in] */ IContext* context)
{
    // Do not reset the default (current) IME when it is a 3rd-party IME
    if (!mCurMethodId.IsNull()) {
        AutoPtr<IInputMethodInfo> method;
        HashMap<String, AutoPtr<IInputMethodInfo> >::Iterator it = mMethodMap->Find(mCurMethodId);
        if (it != mMethodMap->End()) {
            method = it->mSecond;
        }
        if (!IsSystemIme(method)) return;
    }

    AutoPtr<IInputMethodInfo> defIm;
    InputMethodInfoListIterator mlit;
    for (mlit = mMethodList->Begin(); mlit != mMethodList->End(); ++mlit) {
        AutoPtr<IInputMethodInfo> imi = *mlit;
        if (defIm == NULL) {
            if (IsValidSystemDefaultIme(imi, context)) {
                defIm = imi;
                // Slog.i(TAG, "Selected default: " + imi.getId());
            }
        }
    }
    if (defIm == NULL && !mMethodList->IsEmpty()) {
        defIm = GetMostApplicableDefaultIMELocked();
        // Slog.i(TAG, "No default found, using " + defIm.getId());
    }
    if (defIm != NULL) {
        SetSelectedInputMethodAndSubtypeLocked(defIm, NOT_A_SUBTYPE_ID, FALSE);
    }
}

void CInputMethodManagerService::ResetAllInternalStateLocked(
    /* [in] */ Boolean updateOnlyWhenLocaleChanged)
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
        // if (DEBUG) {
        //     Slog.i(TAG, "Locale has been changed to " + newLocale);
        // }
        // InputMethodAndSubtypeListManager should be reset when the locale is changed.
        mImListManager = new InputMethodAndSubtypeListManager(mContext, this);
        BuildInputMethodListLocked(mMethodList, mMethodMap);
        if (!updateOnlyWhenLocaleChanged) {
            String selectedImiId = mSettings->GetSelectedInputMethod();
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
        UpdateFromSettingsLocked();
        mLastSystemLocale = newLocale;
        if (!updateOnlyWhenLocaleChanged) {
            // try {
            AutoPtr<IInputBindResult> result;
            StartInputInnerLocked((IInputBindResult**)&result);
            // } catch (RuntimeException e) {
            //     Slog.w(TAG, "Unexpected exception", e);
            // }
        }
    }
}

void CInputMethodManagerService::CheckCurrentLocaleChangedLocked()
{
    ResetAllInternalStateLocked(TRUE);
}

void CInputMethodManagerService::SwitchUserLocked(
    /* [in] */ Int32 newUserId)
{
    mSettings->SetCurrentUserId(newUserId);
    // InputMethodFileManager should be reset when the user is changed
    mFileManager = new InputMethodFileManager(mMethodMap, newUserId, this);
    ResetAllInternalStateLocked(FALSE);
}

Boolean CInputMethodManagerService::IsValidSystemDefaultIme(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ IContext* context)
{
    if (!mSystemReady) {
        return FALSE;
    }
    if (!IsSystemIme(imi)) {
        return FALSE;
    }
    Int32 resID = 0;
    if ((imi->GetIsDefaultResourceId(&resID), resID) != 0) {
        // try {
        AutoPtr<IResources> res;
        AutoPtr<IContext> packageContext;
        String pNname;
        imi->GetPackageName(&pNname);
        context->CreatePackageContext(pNname, 0, (IContext**)&packageContext);
        packageContext->GetResources((IResources**)&res);

        AutoPtr<IResources> tmpRes;
        context->GetResources((IResources**)&tmpRes);
        AutoPtr<IConfiguration> config;
        tmpRes->GetConfiguration((IConfiguration**)&config);
        AutoPtr<ILocale> locale;
        config->GetLocale((ILocale**)&locale);
        String language;
        locale->GetLanguage(&language);
        Boolean value = FALSE;
        if ((res->GetBoolean(resID, &value), value)
                && ContainsSubtypeOf(imi, language)) {
            return TRUE;
        }
        // } catch (PackageManager.NameNotFoundException ex) {
        // } catch (Resources.NotFoundException ex) {
        // }
    }
    Int32 count = 0;
    if ((imi->GetSubtypeCount(&count), count) == 0) {
        // Slog.w(TAG, "Found no subtypes in a system IME: " + imi.getPackageName());
    }
    return FALSE;
}

AutoPtr<ILocale> CInputMethodManagerService::GetEnglishLocale()
{
    if (CInputMethodManagerService::ENGLISH_LOCALE.Get() == NULL) {
        AutoPtr<ILocale> locale;
        CLocale::New(String("en"), (ILocale**)&locale);
        CInputMethodManagerService::ENGLISH_LOCALE = locale;
    }
    return CInputMethodManagerService::ENGLISH_LOCALE;
}

Boolean CInputMethodManagerService::IsSystemImeThatHasEnglishSubtype(
    /* [in] */ IInputMethodInfo* imi)
{
    if (!IsSystemIme(imi)) {
        return FALSE;
    }
    String language;
    GetEnglishLocale()->GetLanguage(&language);
    return ContainsSubtypeOf(imi, language);
}

Boolean CInputMethodManagerService::ContainsSubtypeOf(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ const String& language)
{
    Int32 N = 0;
    imi->GetSubtypeCount(&N);
    for (Int32 i = 0; i < N; ++i) {
        AutoPtr<IInputMethodSubtype> subtype;
        imi->GetSubtypeAt(i, (IInputMethodSubtype**)&subtype);
        String locale;
        subtype->GetLocale(&locale);
        if (locale.StartWith(language)) {
            return TRUE;
        }
    }
    return FALSE;
}

void CInputMethodManagerService::SystemReady(
    /* [in] */ CStatusBarManagerService* statusBar)
{
    AutoLock lock(mMethodMapLock);

    if (DEBUG) {
        Slogger::D(TAG, "--- systemReady");
    }
    if (!mSystemReady) {
        mSystemReady = TRUE;

        {
            //TODO Added by [wanli]: because com.android.provider.settings is not a "system" provider.
            String defaultImiId = mSettings->GetSelectedInputMethod();
            mImeSelectedOnBoot = !defaultImiId.IsNullOrEmpty();

            BuildInputMethodListLocked(mMethodList, mMethodMap);
            mSettings->EnableAllIMEsIfThereIsNoEnabledIME();

            AutoPtr<IInputMethodInfo> info;
            HashMap<String, AutoPtr<IInputMethodInfo> >::Iterator it = mMethodMap->Find(defaultImiId);
            if (it != mMethodMap->End()) {
                info = it->mSecond;
            }

            if (!mImeSelectedOnBoot || info == NULL) {
                // Slog.w(TAG, "No IME selected. Choose the most applicable IME.");
                ResetDefaultImeLocked(mContext);
            }

            UpdateFromSettingsLocked();
        }

        mContext->GetSystemService(IContext::KEYGUARD_SERVICE, (IInterface**)&mKeyguardManager);
        AutoPtr<IInterface> obj;
        mContext->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&obj);
        mNotificationManager = INotificationManager::Probe(obj.Get());
        assert(statusBar != NULL);
        mStatusBar = statusBar;
        mStatusBar->SetIconVisibility(String("ime"), FALSE);
        UpdateImeWindowStatusLocked();
        mRes->GetBoolean(R::bool_::show_ongoing_ime_switcher, &mShowOngoingImeSwitcherForPhones);
        if (mShowOngoingImeSwitcherForPhones) {
           mWindowManagerService->SetOnHardKeyboardStatusChangeListener(mHardKeyboardListener);
        }
        BuildInputMethodListLocked(mMethodList, mMethodMap);
        if (!mImeSelectedOnBoot) {
           // Slog.w(TAG, "Reset the default IME as \"Resource\" is ready here.");
           CheckCurrentLocaleChangedLocked();
        }
        AutoPtr<IConfiguration> config;
        mRes->GetConfiguration((IConfiguration**)&config);
        config->GetLocale((ILocale**)&mLastSystemLocale);
        //try {
        AutoPtr<IInputBindResult> result;
        StartInputInnerLocked((IInputBindResult**)&result);
        // } catch (RuntimeException e) {
        //     Slog.w(TAG, "Unexpected exception", e);
        // }
    }
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

    Boolean isScreenLocked = FALSE;
    Boolean secure = FALSE;
    if (mKeyguardManager != NULL) {
        mKeyguardManager->IsKeyguardLocked(&isScreenLocked);
        mKeyguardManager->IsKeyguardSecure(&secure);
    }
    Boolean isScreenSecurelyLocked = isScreenLocked && secure;
    Boolean inputShown = mInputShown && (!isScreenLocked || mInputBoundToKeyguard);
    mImeWindowVis = (!isScreenSecurelyLocked && (inputShown || hardKeyShown)) ?
        (IInputMethodService::IME_ACTIVE | IInputMethodService::IME_VISIBLE) : 0;
    UpdateImeWindowStatusLocked();
}

void CInputMethodManagerService::UpdateImeWindowStatusLocked()
{
    SetImeWindowStatus(mCurToken, mImeWindowVis, mBackDisposition);
}

Boolean CInputMethodManagerService::CalledFromValidUser()
{
//    const Int32 uid = Binder::GetCallingUid();
//    const Int32 userId = UserHandle::GetUserId(uid);
//    // if (DEBUG) {
//    //     Slog.d(TAG, "--- calledFromForegroundUserOrSystemProcess ? "
//    //             + "calling uid = " + uid + " system uid = " + Process.SYSTEM_UID
//    //             + " calling userId = " + userId + ", foreground user id = "
//    //             + mSettings.getCurrentUserId() + ", calling pid = " + Binder.getCallingPid());
//    // }
//    if (uid == IProcess::SYSTEM_UID || userId == mSettings->GetCurrentUserId()) {
//        return TRUE;
//    }
//
//    // Caveat: A process which has INTERACT_ACROSS_USERS_FULL gets results for the
//    // foreground user, not for the user of that process. Accordingly InputMethodManagerService
//    // must not manage background users' states in any functions.
//    // Note that privacy-sensitive IPCs, such as setInputMethod, are still securely guarded
//    // by a token.
//    assert(0);
//    // Int32 per = 0;
//    // if ((mContext->CheckCallingOrSelfPermission(
//    //         Manifest::permission::INTERACT_ACROSS_USERS_FULL, &per), per)
//    //                 == IPackageManager::PERMISSION_GRANTED)
//    // {
//    //     // if (DEBUG) {
//    //     //     Slog.d(TAG, "--- Access granted because the calling process has "
//    //     //             + "the INTERACT_ACROSS_USERS_FULL permission");
//    //     // }
//    //     return TRUE;
//    // }
//    //Slog.w(TAG, "--- IPC called from background users. Ignore. \n" + getStackTrace());
//    return FALSE;
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
    mContext->BindService(service, conn, flags, mSettings->GetCurrentUserId(), &succeeded);
    return succeeded;
}

ECode CInputMethodManagerService::GetInputContext(
    /* [out] */ IInputContext** context)
{
    VALIDATE_NOT_NULL(context);
    *context = mCurInputContext;
    REFCOUNT_ADD(*context);
    return NOERROR;
}

ECode CInputMethodManagerService::GetInputMethodList(
    /* [out] */ IObjectContainer** infos)
{
    VALIDATE_NOT_NULL(infos);
    *infos = NULL;

    FAIL_RETURN(CParcelableObjectContainer::New(infos));
    // TODO: Make this work even for non-current users?
    if (!CalledFromValidUser()) {
        return NOERROR;
    }

    AutoLock lock(mMethodMapLock);

    InputMethodInfoListIterator it;
    for (it = mMethodList->Begin(); it != mMethodList->End(); ++it) {
        AutoPtr<IInputMethodInfo> item = *it;
        (*infos)->Add(item);
    }
    return NOERROR;
}

ECode CInputMethodManagerService::GetEnabledInputMethodList(
    /* [out] */ IObjectContainer** infos)
{
    VALIDATE_NOT_NULL(infos);
    *infos = NULL;

    FAIL_RETURN(CParcelableObjectContainer::New(infos));
    // TODO: Make this work even for non-current users?
    if (!CalledFromValidUser()) {
        return NOERROR;
    }

    AutoLock lock(mMethodMapLock);

    AutoPtr< InputMethodInfoList > lists = mSettings->GetEnabledInputMethodListLocked();
    if (lists != NULL) {
        InputMethodInfoListIterator it;
        for (it = lists->Begin(); it != lists->End(); ++it) {
            AutoPtr<IInputMethodInfo> item = *it;
            (*infos)->Add(item.Get());
        }
    }

    return NOERROR;
}

AutoPtr< InputMethodInfoSubtypeMap >
CInputMethodManagerService::GetExplicitlyOrImplicitlyEnabledInputMethodsAndSubtypeListLocked()
{
    AutoPtr< InputMethodInfoSubtypeMap > enabledInputMethodAndSubtypes = new InputMethodInfoSubtypeMap();
    AutoPtr< InputMethodInfoList > lists = mSettings->GetEnabledInputMethodListLocked();
    if (lists != NULL) {
        InputMethodInfoListIterator it;
        for (it = lists->Begin(); it != lists->End(); ++it) {
            AutoPtr<IInputMethodInfo> imi = *it;
            AutoPtr<IObjectContainer> subtypes;
            GetEnabledInputMethodSubtypeListLocked(imi, TRUE, (IObjectContainer**)&subtypes);
            AutoPtr< InputMethodSubtypeList > list = new InputMethodSubtypeList();
            AutoPtr<IObjectEnumerator> enumerator;
            subtypes->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
            Boolean hasNext;
            while (enumerator->MoveNext(&hasNext), hasNext) {
                AutoPtr<IInputMethodSubtype> subtype;
                enumerator->Current((IInterface**)&subtype);
                list->PushBack(subtype);
            }
            (*enabledInputMethodAndSubtypes)[imi] = list;
        }
    }

    return enabledInputMethodAndSubtypes;
}

ECode CInputMethodManagerService::GetEnabledInputMethodSubtypeListLocked(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ Boolean allowsImplicitlySelectedSubtypes,
    /* [out] */ IObjectContainer** subtypes)
{
    VALIDATE_NOT_NULL(subtypes);
    *subtypes = NULL;

    if (imi == NULL && !mCurMethodId.IsNull()) {
        HashMap<String, AutoPtr<IInputMethodInfo> >::Iterator it =
                mMethodMap->Find(mCurMethodId);
        if (it != mMethodMap->End()) {
            imi = it->mSecond;
        }
    }
    AutoPtr< InputMethodSubtypeList > enabledSubtypes =
            mSettings->GetEnabledInputMethodSubtypeListLocked(imi);
    if (allowsImplicitlySelectedSubtypes && enabledSubtypes->Begin() == enabledSubtypes->End()) {
        enabledSubtypes = GetImplicitlyApplicableSubtypesLocked(mRes, imi);
    }
    AutoPtr<IObjectContainer> inSubtyps;
    CObjectContainer::New((IObjectContainer**)&inSubtyps);
    InputMethodSubtypeListIterator eit;
    for (eit = enabledSubtypes->Begin(); eit != enabledSubtypes->End(); ++eit) {
        inSubtyps->Add(*eit);
    }

    AutoPtr<IInputMethodSubtypeHelper> helper;
    CInputMethodSubtypeHelper::AcquireSingleton((IInputMethodSubtypeHelper**)&helper);
    return helper->Sort(mContext, 0, imi, inSubtyps, subtypes);
}

ECode CInputMethodManagerService::GetEnabledInputMethodSubtypeList(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ Boolean allowsImplicitlySelectedSubtypes,
    /* [out] */ IObjectContainer** infos)
{
    VALIDATE_NOT_NULL(infos);
    *infos = NULL;

    // TODO: Make this work even for non-current users?
    if (!CalledFromValidUser()) {
        return CObjectContainer::New(infos);
    }
    AutoLock lock(mMethodMapLock);
    return GetEnabledInputMethodSubtypeListLocked(imi, allowsImplicitlySelectedSubtypes, infos);
}

ECode CInputMethodManagerService::AddClient(
    /* [in] */ IInputMethodClient* client,
    /* [in] */ IInputContext* inputContext,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid)
{
    if (!CalledFromValidUser()) {
        return NOERROR;
    }
    AutoLock lock(mMethodMapLock);
    mClients[IBinder::Probe(client)] = new ClientState(client, inputContext, uid, pid);
    return NOERROR;
}

ECode CInputMethodManagerService::RemoveClient(
    /* [in] */ IInputMethodClient* client)
{
    if (!CalledFromValidUser()) {
        return NOERROR;
    }
    AutoLock lock(mMethodMapLock);
    mClients.Erase(IBinder::Probe(client));
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
                mCaller->ObtainMessage(MSG_UNBIND_INPUT, mCurMethod, (IMessage**)&msg);
                ExecuteOrSendMessage(mCurMethod, msg);
            }
        }

        AutoPtr<IMessage> msg1;
        mCaller->ObtainMessage(MSG_SET_ACTIVE, 0, mCurClient, (IMessage**)&msg1);
        ExecuteOrSendMessage(mCurClient->mClient, msg1);

        AutoPtr<IMessage> msg2;
        mCaller->ObtainMessage(MSG_UNBIND_METHOD, mCurSeq, mCurClient->mClient, (IMessage**)&msg2);
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
        mCaller->ObtainMessage(MSG_BIND_INPUT,
            mCurMethod, mCurClient->mBinding,
            (IMessage**)&msg);
        ExecuteOrSendMessage(mCurMethod, msg);
        mBoundToMethod = TRUE;
    }

    SessionState* session = mCurClient->mCurSession.Get();
    if (initial) {
        AutoPtr<IMessage> msg;
        mCaller->ObtainMessage(MSG_START_INPUT,
            session, mCurInputContext, mCurAttribute,
            (IMessage**)&msg);
        ExecuteOrSendMessage(session->mMethod, msg);
    }
    else {
        AutoPtr<IMessage> msg;
        mCaller->ObtainMessage(MSG_RESTART_INPUT,
            session, mCurInputContext, mCurAttribute,
            (IMessage**)&msg);
        ExecuteOrSendMessage(session->mMethod, msg);
    }

    if (mShowRequested) {
        if (DEBUG) Slogger::V(TAG, "Attach new input asks to show input");
        ShowCurrentInputLocked(GetAppShowFlags(), NULL);
    }
    return CInputBindResult::New(mCurClient->mCurSession->mSession, mCurId, mCurSeq, result);
}

ECode CInputMethodManagerService::StartInputLocked(
    /* [in] */ IInputMethodClient* client,
    /* [in] */ IInputContext* inputContext,
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
    /* [in] */ IInputContext* inputContext,
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

    if (mCurClient == NULL) {
        mInputBoundToKeyguard = FALSE;
        if (mKeyguardManager != NULL) {
            mKeyguardManager->IsKeyguardLocked(&mInputBoundToKeyguard);
        }
        if (DEBUG) {
            Slogger::V(TAG, "New bind. keyguard = %d",  mInputBoundToKeyguard);
        }
    }

    if (mCurClient.Get() != cs) {
        // If the client is changing, we need to switch over to the new
        // one.
        UnbindCurrentClientLocked();
        if (DEBUG) Slogger::V(TAG, "switching to client: client = %p", cs->mClient.Get());

        // If the screen is on, inform the new client it is active
        if (mScreenOn) {
            AutoPtr<IMessage> msg;
            mCaller->ObtainMessage(MSG_SET_ACTIVE, mScreenOn ? 1 : 0, cs, (IMessage**)&msg);
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
                if (!cs->mSessionRequested) {
                    cs->mSessionRequested = TRUE;
                    if (DEBUG) Slogger::V(TAG, "Creating new session for client %p", cs->mClient.Get());

                    AutoPtr<IInputMethodCallback> methodCallback;
                    CInputMethodManagerServiceMethodCallback::New(
                        mCurMethod, (Handle32)this, (IInputMethodCallback**)&methodCallback);

                    AutoPtr<IMessage> msg;
                    mCaller->ObtainMessage(MSG_CREATE_SESSION, mCurMethod, methodCallback, (IMessage**)&msg);
                    ExecuteOrSendMessage(mCurMethod, msg);
                }
                // Return to client, and we will get back with it when
                // we have had a session made for it.
                return CInputBindResult::New(NULL, mCurId, mCurSeq, result);
            }
            else if (SystemClock::GetUptimeMillis() < (mLastBindTime + TIME_TO_RECONNECT)) {
                // In this case we have connected to the service, but
                // don't yet have its interface.  If it hasn't been too
                // long since we did the connection, we'll return to
                // the client and wait to get the service interface so
                // we can report back.  If it has been too long, we want
                // to fall through so we can try a disconnect/reconnect
                // to see if we can get back in touch with the service.
                return CInputBindResult::New(NULL, mCurId, mCurSeq, result);
            }
            else {
                // EventLog.writeEvent(EventLogTags.IMF_FORCE_RECONNECT_IME,
                //         mCurMethodId, SystemClock::UptimeMillis()-mLastBindTime, 0);
            }
        }
    }

    return StartInputInnerLocked(result);
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
        return CInputBindResult::New(NULL, mCurMethodId, mCurSeq, result);
    }

    AutoPtr<IInputMethodInfo> info;
    HashMap<String, AutoPtr<IInputMethodInfo> >::Iterator it = mMethodMap->Find(mCurMethodId);
    if (it != mMethodMap->End()) {
        info = it->mSecond;
    }
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
    if (BindCurrentInputMethodService(mCurIntent, this, IContext::BIND_AUTO_CREATE | IContext::BIND_NOT_VISIBLE)) {
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
        return CInputBindResult::New(NULL, mCurId, mCurSeq, result);
    }
    else {
        mCurIntent = NULL;
        Slogger::W(TAG, "Failure connecting to input method service: ");
    }

    return NOERROR;
}

ECode CInputMethodManagerService::StartInput(
    /* [in] */ IInputMethodClient* client,
    /* [in] */ IInputContext* inputContext,
    /* [in] */ IEditorInfo* attribute,
    /* [in] */ Int32 controlFlags,
    /* [out] */ IInputBindResult** res)
{
    VALIDATE_NOT_NULL(res);
    *res = NULL;

    if (!CalledFromValidUser()) {
        return NOERROR;
    }
    AutoLock lock(mMethodMapLock);

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
    AutoLock lock(mMethodMapLock);

    if (mCurIntent != NULL) {
        AutoPtr<IComponentName> component;
        mCurIntent->GetComponent((IComponentName**)&component);
        Boolean isEqual = FALSE;
        if (name->Equals(component, &isEqual), isEqual) {
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
            mCaller->ObtainMessage(MSG_ATTACH_TOKEN, mCurMethod, mCurToken, (IMessage**)&msg);
            ExecuteOrSendMessage(mCurMethod, msg);

            if (mCurClient != NULL) {
                if (DEBUG) Slogger::V(TAG, "Creating first session while with client %p", mCurClient.Get());
                AutoPtr<IInputMethodCallback> methodCallback;
                CInputMethodManagerServiceMethodCallback::New(
                    mCurMethod, (Handle32)this, (IInputMethodCallback**)&methodCallback);

                AutoPtr<IMessage> msg;
                mCaller->ObtainMessage(MSG_CREATE_SESSION, mCurMethod, methodCallback, (IMessage**)&msg);
                ExecuteOrSendMessage(mCurMethod, msg);
            }
        }
    }
    return NOERROR;
}

void CInputMethodManagerService::OnSessionCreated(
    /* [in] */ IIInputMethod* method,
    /* [in] */ IIInputMethodSession* session)
{
    AutoLock lock(mMethodMapLock);

    if (mCurMethod != NULL && method != NULL
            && IBinder::Probe(mCurMethod) == IBinder::Probe(method)) {
        if (mCurClient != NULL) {
            mCurClient->mCurSession = new SessionState(mCurClient, method, session);
            mCurClient->mSessionRequested = FALSE;

            AutoPtr<IInputBindResult> res;
            AttachNewInputLocked(TRUE, (IInputBindResult**)&res);
            AutoPtr<IInputMethodSession> m;
            res->GetIIMSession((IIInputMethodSession**)&m);
            if (m != NULL) {
                AutoPtr<IMessage> msg;
                mCaller->ObtainMessage(MSG_BIND_METHOD, mCurClient->mClient, res, (IMessage**)&msg);
                ExecuteOrSendMessage(mCurClient->mClient, msg);
            }
        }
    }
}

void CInputMethodManagerService::UnbindCurrentMethodLocked(
    /* [in] */ Boolean reportToClient,
    /* [in] */ Boolean savePosition)
{
    if (mVisibleBound) {
        mContext->UnbindService(mVisibleConnection);
        mVisibleBound = FALSE;
    }

    if (mHaveConnection) {
        mContext->UnbindService((IServiceConnection*)this->Probe(EIID_IServiceConnection));
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
        mCaller->ObtainMessage(MSG_UNBIND_METHOD,
            mCurSeq, mCurClient->mClient, (IMessage**)&msg);
        ExecuteOrSendMessage(mCurClient->mClient, msg);
    }
}

void CInputMethodManagerService::FinishSession(
    /* [in] */ SessionState* sessionState)
{
    if (sessionState != NULL && sessionState->mSession != NULL) {
        // try {
        ECode ec = sessionState->mSession->FinishSession();
        if (FAILED(ec)) {
            SetImeWindowVisibilityStatusHiddenLocked();
        }
        // } catch (RemoteException e) {
        //     Slog.w(TAG, "Session failed to close due to remote exception", e);
        // }
    }
}

void CInputMethodManagerService::ClearCurMethodLocked()
{
    if (mCurMethod != NULL) {
        HashMap<AutoPtr<IBinder>, AutoPtr<ClientState> >::Iterator it;
        for (it = mClients.Begin(); it != mClients.End(); ++it) {
            AutoPtr<ClientState> cs = it->mSecond;
            cs->mSessionRequested = FALSE;
            FinishSession(cs->mCurSession);
            cs->mCurSession = NULL;
        }

        FinishSession(mEnabledSession);
        mEnabledSession = NULL;
        mCurMethod = NULL;
    }
    if (mStatusBar != NULL) {
        mStatusBar->SetIconVisibility(String("ime"), FALSE);
    }
}

ECode CInputMethodManagerService::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    AutoLock lock(mMethodMapLock);

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
        Boolean equal = FALSE;
        if (name->Equals(component, &equal), equal) {
            ClearCurMethodLocked();
            // We consider this to be a new bind attempt, since the system
            // should now try to restart the service for us.
            mLastBindTime = SystemClock::GetUptimeMillis();
            mShowRequested = mInputShown;
            mInputShown = FALSE;
            if (mCurClient != NULL) {
                AutoPtr<IMessage> msg;
                mCaller->ObtainMessage(MSG_UNBIND_METHOD,
                    mCurSeq, mCurClient->mClient, (IMessage**)&msg);
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
    Int32 uid = Binder::GetCallingUid();
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    if (token == NULL || mCurToken.Get() != token) {
        Slogger::W(TAG, "Ignoring setInputMethod of uid %d, token %p", uid, token);
        Binder::RestoreCallingIdentity(ident);
        return NOERROR;
    }

    ECode ec = NOERROR;
    {
        AutoLock lock(mMethodMapLock);
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
            Int32 curUserId = mSettings->GetCurrentUserId();
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
    if (IsScreenLocked()) return FALSE;

    AutoLock lock(mMethodMapLock);

    Int32 nonAuxCount = 0;
    Int32 auxCount = 0;
    AutoPtr<IInputMethodSubtype> nonAuxSubtype;
    AutoPtr<IInputMethodSubtype> auxSubtype;

    AutoPtr< InputMethodInfoList > imis = mSettings->GetEnabledInputMethodListLocked();
    if (imis != NULL) {
        const Int32 N = imis->GetSize();
        if (N > 2) return TRUE;
        if (N < 1) return FALSE;

        InputMethodInfoListIterator it;
        for(it = imis->Begin(); it != imis->End(); ++it) {
            AutoPtr<IInputMethodInfo> imi = *it;
            AutoPtr<IObjectContainer> subtypes;
            GetEnabledInputMethodSubtypeListLocked(imi, TRUE, (IObjectContainer**)&subtypes);
            Int32 subtypeCount = 0;
            subtypes->GetObjectCount(&subtypeCount);
            if (subtypeCount == 0) {
                ++nonAuxCount;
            }
            else {
                Boolean hasNext = FALSE;
                AutoPtr<IObjectEnumerator> enumAtor;
                subtypes->GetObjectEnumerator((IObjectEnumerator**)&enumAtor);
                while(enumAtor->MoveNext(&hasNext), hasNext) {
                    AutoPtr<IInputMethodSubtype> subtype;
                    enumAtor->Current((IInterface**)&subtype);
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

ECode CInputMethodManagerService::SetImeWindowStatus(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 vis,
    /* [in] */ Int32 backDisposition)
{
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    if (token == NULL || mCurToken.Get() != token) {
        // Int32 uid = Binder.getCallingUid();
        // Slog.w(TAG, "Ignoring setImeWindowStatus of uid " + uid + " token: " + token);
        Binder::RestoreCallingIdentity(ident);
        return NOERROR;
    }

    {
        AutoLock lock(mMethodMapLock);

        mImeWindowVis = vis;
        mBackDisposition = backDisposition;
        if (mStatusBar != NULL) {
            mStatusBar->SetImeWindowStatus(token, vis, backDisposition);
        }
        Boolean iconVisibility = (vis & IInputMethodService::IME_ACTIVE) != 0;
        AutoPtr<IInputMethodInfo> imi = (*mMethodMap)[mCurMethodId];
        if (imi != NULL && iconVisibility && NeedsToShowImeSwitchOngoingNotification()) {
            // Used to load label
            AutoPtr<IPackageManager> pm;
            mContext->GetPackageManager((IPackageManager**)&pm);

            AutoPtr<ICharSequence> title;
            mRes->GetText(R::string::select_input_method, (ICharSequence**)&title);

            AutoPtr<ICharSequence> imiLabel;
            imi->LoadLabel(pm, (ICharSequence**)&imiLabel);

            AutoPtr<ICharSequence> summary;
            if (mCurrentSubtype != NULL) {
                String pkgName;
                imi->GetPackageName(&pkgName);
                AutoPtr<IServiceInfo> serviceInfo;
                imi->GetServiceInfo((IServiceInfo**)&serviceInfo);
                AutoPtr<IApplicationInfo> appInfo;
                serviceInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
                AutoPtr<ICharSequence> name;
                mCurrentSubtype->GetDisplayName(mContext, pkgName, appInfo, (ICharSequence**)&name);
                AutoPtr<ICharSequence> name1;
                if (TextUtils::IsEmpty(imiLabel)) {
                    CString::New(String(""), (ICharSequence**)&name1);
                } else {
                    String tempStr("-");
                    String labelStr;
                    imiLabel->ToString(&labelStr);
                    tempStr.Append(labelStr);
                    CString::New(tempStr, (ICharSequence**)&name1);
                }
                AutoPtr<ArrayOf<ICharSequence*> > texts = ArrayOf<ICharSequence*>::Alloc(2);
                texts->Set(0, name);
                texts->Set(1, name1);
                summary = TextUtils::Concat(texts);
            } else {
                summary = imiLabel;
            }

            mImeSwitcherNotification->SetLatestEventInfo(mContext, title, summary, mImeSwitchPendingIntent);
            if (mNotificationManager != NULL) {
                if (DEBUG) {
                    String labelStr, sumStr;
                    imiLabel->ToString(&labelStr);
                    summary->ToString(&sumStr);
                    Slogger::D(TAG, "--- show notification: label =  %s, summary = %s", labelStr.string(), sumStr.string());
                }
                mNotificationManager->NotifyAsUser(String(NULL), R::string::select_input_method, mImeSwitcherNotification, UserHandle::ALL);
                mNotificationShown = true;
            }
        } else {
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
        AutoLock lock(mMethodMapLock);

        AutoPtr<IInputMethodInfo> currentImi;
        HashMap<String, AutoPtr<IInputMethodInfo> >::Iterator it = mMethodMap->Find(mCurMethodId);
        if (it != mMethodMap->End()) {
            currentImi = it->mSecond;
        }
        for (Int32 i = 0; i < spans->GetLength(); ++i) {
            AutoPtr<ISuggestionSpan> ss = (*spans)[i];
            String name;
            ss->GetNotificationTargetClassName(&name);
            if (!name.IsNullOrEmpty()) {
                mSecureSuggestionSpans[ss] = currentImi;
                // AutoPtr<IInputMethodInfo> targetImi = mSecureSuggestionSpans[ss];
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
        AutoLock lock(mMethodMapLock);

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

void CInputMethodManagerService::UpdateFromSettingsLocked()
{
    // We are assuming that whoever is changing DEFAULT_INPUT_METHOD and
    // ENABLED_INPUT_METHODS is taking care of keeping them correctly in
    // sync, so we will never have a DEFAULT_INPUT_METHOD that is not
    // enabled.
    String id = mSettings->GetSelectedInputMethod();
    if (id.IsNullOrEmpty() && ChooseNewDefaultIMELocked()) {
        id = mSettings->GetSelectedInputMethod();
    }
    if (!id.IsNullOrEmpty()) {
        // try {
        ECode ec = SetInputMethodLocked(id, GetSelectedInputMethodSubtypeId(id));
        if (ec == (Int32)E_ILLEGAL_ARGUMENT_EXCEPTION) {
            mCurMethodId = NULL;
            UnbindCurrentMethodLocked(TRUE, FALSE);
        }
        // } catch (IllegalArgumentException e) {
        //     Slog.w(TAG, "Unknown input method from prefs: " + id, e);
        //     mCurMethodId = NULL;
        //     UnbindCurrentMethodLocked(TRUE);
        // }
        mShortcutInputMethodsAndSubtypes.Clear();
    }
    else {
        // There is no longer an input method set, so stop any current one.
        mCurMethodId = NULL;
        UnbindCurrentMethodLocked(TRUE, FALSE);
    }
}

ECode CInputMethodManagerService::SetInputMethodLocked(
    /* [in] */ const String& id,
    /* [in] */ Int32 subtypeId)
{
    AutoPtr<IInputMethodInfo> info;
    HashMap<String, AutoPtr<IInputMethodInfo> >::Iterator it =
            mMethodMap->Find(id);
    if (it != mMethodMap->End()) {
        info = it->mSecond;
    }
    if (info == NULL) {
        Slogger::E(TAG, "Unknown id: %s", id.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
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
            // Slog.w(TAG, "Illegal subtype state: old subtype = " + oldSubtype
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
                //     Slog.w(TAG, "Failed to call changeInputMethodSubtype");
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
        AutoLock lock(mMethodMapLock);

        if (mCurClient == NULL || client == NULL
                || IBinder::Probe(mCurClient->mClient) != IBinder::Probe(client)) {
            // try {
            // We need to check if this is the current client with
            // focus in the window manager, to allow this call to
            // be made before input is started in it.
            Boolean result = FALSE;
            if (mIWindowManager->InputMethodClientHasFocus(client, &result), !result) {
                // Slog.w(TAG, "Ignoring showSoftInput of uid " + uid + ": " + client);
                *state = FALSE;
                Binder::RestoreCallingIdentity(ident);
                return NOERROR;
            }
            // } catch (RemoteException e) {
            //     return FALSE;
            // }
        }

        // if (DEBUG) Slog.v(TAG, "Client requesting input be shown");
        *state = ShowCurrentInputLocked(flags, resultReceiver);
    }
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
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
        AutoPtr<IMessage> msg;
        mCaller->ObtainMessage(
            MSG_SHOW_SOFT_INPUT, GetImeShowFlags(),
            mCurMethod, resultReceiver, (IMessage**)&msg);
        ExecuteOrSendMessage(mCurMethod, msg);

        mInputShown = TRUE;
        if (mHaveConnection && !mVisibleBound) {
            BindCurrentInputMethodService(
                    mCurIntent, mVisibleConnection, IContext::BIND_AUTO_CREATE);
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
        // Slog.w(TAG, "Force disconnect/connect to the IME in showCurrentInputLocked()");
        mContext->UnbindService((IServiceConnection*)this->Probe(EIID_IServiceConnection));
        BindCurrentInputMethodService(mCurIntent, this, IContext::BIND_AUTO_CREATE
                    | IContext::BIND_NOT_VISIBLE);
    }
    else {
        // if (DEBUG) {
        //     Slog.d(TAG, "Can't show input: connection = " + mHaveConnection + ", time = "
        //             + ((mLastBindTime+TIME_TO_RECONNECT) - SystemClock.uptimeMillis()));
        // }
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
        AutoLock lock(mMethodMapLock);

        if (mCurClient == NULL || client == NULL
                || IBinder::Probe(mCurClient->mClient) != IBinder::Probe(client)) {
            // try {
            // We need to check if this is the current client with
            // focus in the window manager, to allow this call to
            // be made before input is started in it.
            Boolean ret = FALSE;
            mIWindowManager->InputMethodClientHasFocus(client, &ret);
            if (!ret) {
                // if (DEBUG) Slog.w(TAG, "Ignoring hideSoftInput of uid "
                //         + uid + ": " + client);
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

        // if (DEBUG) Slog.v(TAG, "Client requesting input be hidden");
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
        // if (DEBUG) Slog.v(TAG,
        //         "Not hiding: explicit show not cancelled by non-explicit hide");
        return FALSE;
    }
    if (mShowForced && (flags & IInputMethodManager::HIDE_NOT_ALWAYS) != 0) {
        // if (DEBUG) Slog.v(TAG,
        //         "Not hiding: forced show not cancelled by not-always hide");
        return FALSE;
    }
    Boolean res = FALSE;
    if (mInputShown && mCurMethod != NULL) {
        AutoPtr<IMessage> msg;
        mCaller->ObtainMessage(MSG_HIDE_SOFT_INPUT,
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
    /* [in] */ IInputContext* inputContext,
    /* [out] */ IInputBindResult** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    // Needs to check the validity before clearing calling identity
    Boolean calledFromValidUser = CalledFromValidUser();

    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    {
        AutoLock lock(mMethodMapLock);
        // if (DEBUG) Slog.v(TAG, "windowGainedFocus: " + client.asBinder()
        //         + " controlFlags=#" + Integer.toHexString(controlFlags)
        //         + " softInputMode=#" + Integer.toHexString(softInputMode)
        //         + " windowFlags=#" + Integer.toHexString(windowFlags));

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
            // Slog.w(TAG, "Focus gain on non-focused client " + cs.client
            //         + " (uid=" + cs.uid + " pid=" + cs.pid + ")");
            Binder::RestoreCallingIdentity(ident);
            *result = NULL;
            return NOERROR;
        }
        // } catch (RemoteException e) {
        // }

        if (!calledFromValidUser) {
            // Slog.w(TAG, "A background user is requesting window. Hiding IME.");
            // Slog.w(TAG, "If you want to interect with IME, you need "
            //         + "Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS_FULL");
            HideCurrentInputLocked(0, NULL);
            *result = NULL;
            return NOERROR;
        }

        if (mCurFocusedWindow.Get() == windowToken) {
            // Slog.w(TAG, "Window already focused, ignoring focus gain of: " + client
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
                        // if (DEBUG) Slog.v(TAG, "Unspecified window will hide input");
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
                    // if (DEBUG) Slog.v(TAG, "Unspecified window will show input");
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
                    // if (DEBUG) Slog.v(TAG, "Window asks to hide input going forward");
                    HideCurrentInputLocked(0, NULL);
                }
                break;
            case IWindowManagerLayoutParams::SOFT_INPUT_STATE_ALWAYS_HIDDEN:
                // if (DEBUG) Slog.v(TAG, "Window asks to hide input");
                HideCurrentInputLocked(0, NULL);
                break;
            case IWindowManagerLayoutParams::SOFT_INPUT_STATE_VISIBLE:
                if ((softInputMode &
                        IWindowManagerLayoutParams::SOFT_INPUT_IS_FORWARD_NAVIGATION) != 0) {
                    // if (DEBUG) Slog.v(TAG, "Window asks to show input going forward");
                    if (attribute != NULL) {
                        StartInputUncheckedLocked(cs, inputContext, attribute,
                                controlFlags, result);
                        didStart = TRUE;
                    }
                    ShowCurrentInputLocked(IInputMethodManager::SHOW_IMPLICIT, NULL);
                }
                break;
            case IWindowManagerLayoutParams::SOFT_INPUT_STATE_ALWAYS_VISIBLE:
                // if (DEBUG) Slog.v(TAG, "Window asks to always show input");
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
    AutoLock lock(mMethodMapLock);

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
    SetInputMethodWithSubtypeId(token, id, NOT_A_SUBTYPE_ID);
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
        AutoLock lock(mMethodMapLock);

        if (subtype != NULL) {
            AutoPtr<IInputMethodInfo> info;
            HashMap<String, AutoPtr<IInputMethodInfo> >::Iterator it = mMethodMap->Find(id);
            if (it != mMethodMap->End()) {
                info = it->mSecond;
            }
            Int32 hashCode;
            subtype->GetHashCode(&hashCode);
            SetInputMethodWithSubtypeId(token, id, GetSubtypeIdFromHashCode(
                    info, hashCode));
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
        AutoLock lock(mMethodMapLock);

        if (mCurClient == NULL || client == NULL
            || IBinder::Probe(mCurClient->mClient) != IBinder::Probe(client)) {
            // Slog.w(TAG, "Ignoring showInputMethodAndSubtypeEnablerFromClient of: " + client);
        }

        AutoPtr<ICharSequence> seq;
        CString::New(inputMethodId, (ICharSequence**)&seq);
        AutoPtr<IMessage> msg;
        mCaller->ObtainMessage(MSG_SHOW_IM_SUBTYPE_ENABLER,
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

    if (!CalledFromValidUser()) {
        *result = FALSE;
        return NOERROR;
    }
    {
        AutoLock lock(mMethodMapLock);

        AutoPtr<StringStringPair> lastIme = mSettings->GetLastInputMethodAndSubtypeLocked();
        AutoPtr<IInputMethodInfo> lastImi;
        String first, second;
        if (lastIme != NULL) {
            HashMap<String, AutoPtr<IInputMethodInfo> >::Iterator it =
                    mMethodMap->Find(lastIme->mFirst);
            if (it != mMethodMap->End()) {
                lastImi = it->mSecond;
            }
        }
        else {
            lastImi = NULL;
        }

        String targetLastImiId;
        Int32 subtypeId = NOT_A_SUBTYPE_ID;
        if (lastIme != NULL && lastImi != NULL) {
            String id;
            lastImi->GetId(&id);
            Boolean imiIdIsSame = id.Equals(mCurMethodId);
            const Int32 lastSubtypeHash = StringUtils::ParseInt32(lastIme->mSecond);
            Int32 hashCode;
            mCurrentSubtype->GetHashCode(&hashCode);
            const Int32 currentSubtypeHash = mCurrentSubtype == NULL ? NOT_A_SUBTYPE_ID
                    : hashCode;
            // If the last IME is the same as the current IME and the last subtype is not
            // defined, there is no need to switch to the last IME.
            if (!imiIdIsSame || lastSubtypeHash != currentSubtypeHash) {
                targetLastImiId = lastIme->mFirst;
                subtypeId = GetSubtypeIdFromHashCode(lastImi, lastSubtypeHash);
            }
        }

        if (lastIme != NULL) lastIme = NULL;

        if (targetLastImiId.IsNullOrEmpty() && !CanAddToLastInputMethod(mCurrentSubtype)) {
            // This is a safety net. If the currentSubtype can't be added to the history
            // and the framework couldn't find the last ime, we will make the last ime be
            // the most applicable enabled keyboard subtype of the system imes.
            AutoPtr<InputMethodInfoList> enabled = mSettings->GetEnabledInputMethodListLocked();
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
                List<AutoPtr<IInputMethodInfo> >::Iterator ator;
                for (ator = enabled->Begin(); ator != enabled->End(); ++ator) {
                    AutoPtr<IInputMethodInfo> imi = *ator;
                    Int32 count = 0;
                    if ((imi->GetSubtypeCount(&count), count) > 0 && IsSystemIme(imi)) {
                        AutoPtr< InputMethodSubtypeList > subtypes = GetSubtypes(imi);
                        AutoPtr<IInputMethodSubtype> keyboardSubtype =
                                FindLastResortApplicableSubtypeLocked(mRes, subtypes,
                                        SUBTYPE_MODE_KEYBOARD, locale, TRUE);
                        if (keyboardSubtype != NULL) {
                            imi->GetId(&targetLastImiId);
                            Int32 hashCode;
                            keyboardSubtype->GetHashCode(&hashCode);
                            subtypeId = GetSubtypeIdFromHashCode(imi, hashCode);
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
            // if (DEBUG) {
            //     Slog.d(TAG, "Switch to: " + lastImi.getId() + ", " + lastIme.second
            //             + ", from: " + mCurMethodId + ", " + subtypeId);
            // }
            SetInputMethodWithSubtypeId(token, targetLastImiId, subtypeId);
            *result = TRUE;
            return NOERROR;
        }
        else {
            *result = FALSE;
            return NOERROR;
        }
    }
}

ECode CInputMethodManagerService::SwitchToNextInputMethod(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean onlyCurrentIme,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (!CalledFromValidUser()) {
        *result = FALSE;
        return NOERROR;
    }
    {
        AutoLock lock(mMethodMapLock);

        AutoPtr<IInputMethodInfo> info;
        HashMap<String, AutoPtr<IInputMethodInfo> >::Iterator it = mMethodMap->Find(mCurMethodId);
        if (it != mMethodMap->End()) {
            info = it->mSecond;
        }
        AutoPtr<ImeSubtypeListItem> nextSubtype = mImListManager->GetNextInputMethod(
                onlyCurrentIme, info, mCurrentSubtype);
        if (nextSubtype == NULL) {
            *result = FALSE;
            return NOERROR;
        }
        String id;
        nextSubtype->mImi->GetId(&id);
        SetInputMethodWithSubtypeId(token, id, nextSubtype->mSubtypeId);
        *result = TRUE;
        return NOERROR;
    }
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
        AutoLock lock(mMethodMapLock);

        AutoPtr<StringStringPair> lastIme = mSettings->GetLastInputMethodAndSubtypeLocked();
        // TODO: Handle the case of the last IME with no subtypes
        if (lastIme == NULL || lastIme->mFirst.IsNullOrEmpty()
                || lastIme->mSecond.IsNullOrEmpty()) {
            return NOERROR;
        }

        AutoPtr<IInputMethodInfo> lastImi;
        HashMap<String, AutoPtr<IInputMethodInfo> >::Iterator it = mMethodMap->Find(lastIme->mFirst);
        if (it != mMethodMap->End()) {
            lastImi = it->mSecond;
        }

        if (lastImi == NULL) {
            return NOERROR;
        }

        // try {
        Int32 lastSubtypeHash = StringUtils::ParseInt32(lastIme->mSecond);
        Int32 lastSubtypeId = GetSubtypeIdFromHashCode(lastImi, lastSubtypeHash);
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
}

ECode CInputMethodManagerService::SetAdditionalInputMethodSubtypes(
    /* [in] */ const String& imiId,
    /* [in] */ ArrayOf<IInputMethodSubtype*>* subtypes)
{
    if (!CalledFromValidUser()) {
        return NOERROR;
    }
    // By this IPC call, only a process which shares the same uid with the IME can add
    // additional input method subtypes to the IME.
    AutoPtr<ICharSequence> tmp;
    CString::New(imiId, (ICharSequence**)&tmp);
    if (imiId.IsNullOrEmpty() || subtypes == NULL || subtypes->GetLength() == 0) return NOERROR;
    {
        AutoLock lock(mMethodMapLock);

        AutoPtr<IInputMethodInfo> imi;
        HashMap<String, AutoPtr<IInputMethodInfo> >::Iterator it = mMethodMap->Find(imiId);
        if (it != mMethodMap->End()) {
            imi = it->mSecond;
        }
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
                        BuildInputMethodListLocked(mMethodList, mMethodMap);
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

ECode CInputMethodManagerService::SetInputMethodWithSubtypeId(
    /* [in] */ IBinder* token,
    /* [in] */ const String& id,
    /* [in] */ Int32 subtypeId)
{
    AutoLock lock(mMethodMapLock);

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
        // Slog.w(TAG, "Ignoring setInputMethod of uid " + Binder::GetCallingUid()
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
    // if (!CalledFromValidUser()) {
    //     return NOERROR;
    // }
    AutoLock lock(mMethodMapLock);

    if (token == NULL || mCurToken.Get() != token) {
        // if (DEBUG) Slog.w(TAG, "Ignoring hideInputMethod of uid "
        //         + Binder::GetCallingUid() + " token: " + token);
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
    AutoLock lock(mMethodMapLock);

    if (token == NULL || mCurToken.Get() != token) {
        // Slog.w(TAG, "Ignoring showMySoftInput of uid "
        //         + Binder::GetCallingUid() + " token: " + token);
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
        if (mEnabledSession != NULL) {
            // try {
            if (DEBUG) Slogger::V(TAG, "Disabling: %p", mEnabledSession.Get());
            mEnabledSession->mMethod->SetSessionEnabled(mEnabledSession->mSession, FALSE);
            // } catch (RemoteException e) {
            // }
        }
        mEnabledSession = session;
        // try {
        if (DEBUG) Slogger::V(TAG, "Enabling: %p", mEnabledSession.Get());
        session->mMethod->SetSessionEnabled(session->mSession, TRUE);
        // } catch (RemoteException e) {
        // }
    }
}

Boolean CInputMethodManagerService::IsSystemIme(
    /* [in] */ IInputMethodInfo* inputMethod)
{
    AutoPtr<IServiceInfo> serInfo;
    inputMethod->GetServiceInfo((IServiceInfo**)&serInfo);
    AutoPtr<IApplicationInfo> appInfo;
    serInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
    Int32 flag = 0;
    appInfo->GetFlags(&flag);
    return (flag & IApplicationInfo::FLAG_SYSTEM) != 0;
}

AutoPtr< InputMethodSubtypeList > CInputMethodManagerService::GetSubtypes(
    /* [in] */ IInputMethodInfo* imi)
{
    AutoPtr< InputMethodSubtypeList > subtypes = new InputMethodSubtypeList();
    Int32 subtypeCount = 0;
    imi->GetSubtypeCount(&subtypeCount);
    for (Int32 i = 0; i < subtypeCount; ++i) {
        AutoPtr<IInputMethodSubtype> subtype;
        imi->GetSubtypeAt(i, (IInputMethodSubtype**)&subtype);
        subtypes->PushBack(subtype);
    }
    return subtypes;
}

AutoPtr< InputMethodSubtypeList > CInputMethodManagerService::GetOverridingImplicitlyEnabledSubtypes(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ const String& mode)
{
    AutoPtr< InputMethodSubtypeList > subtypes = new InputMethodSubtypeList();
    Int32 subtypeCount = 0;
    imi->GetSubtypeCount(&subtypeCount);
    for (Int32 i = 0; i < subtypeCount; ++i) {
        AutoPtr<IInputMethodSubtype> subtype;
        imi->GetSubtypeAt(i, (IInputMethodSubtype**)&subtype);
        Boolean result = FALSE;
        String subtypeMode;
        subtype->GetMode(&subtypeMode);
        if ((subtype->OverridesImplicitlyEnabledSubtype(&result), result)
                && (subtypeMode.Equals(mode))) {
            subtypes->PushBack(subtype);
        }
    }
    return subtypes;
}

AutoPtr<IInputMethodInfo> CInputMethodManagerService::GetMostApplicableDefaultIMELocked()
{
    AutoPtr< InputMethodInfoList > enabled = mSettings->GetEnabledInputMethodListLocked();
    if (enabled != NULL && enabled->Begin() != enabled->End()) {
        // We'd prefer to fall back on a system IME, since that is safer.
        InputMethodInfoListReverseIterator rit = enabled->RBegin();
        InputMethodInfoListReverseIterator firstFoundSystemIme = enabled->REnd();
        while (rit != enabled->REnd()) {
            AutoPtr<IInputMethodInfo> imi = *rit;
            Boolean result = FALSE;
            if (IsSystemImeThatHasEnglishSubtype(imi) && !(imi->IsAuxiliaryIme(&result), result)) {
                return imi;
            }
            if (firstFoundSystemIme == enabled->REnd() && IsSystemIme(imi) && !(imi->IsAuxiliaryIme(&result), result)) {
                firstFoundSystemIme = rit;
            }
            ++rit;
        }
        return firstFoundSystemIme != enabled->REnd() ? *firstFoundSystemIme : *enabled->Begin();
    }

    return NULL;
}

Boolean CInputMethodManagerService::ChooseNewDefaultIMELocked()
{
    AutoPtr<IInputMethodInfo> imi = GetMostApplicableDefaultIMELocked();
    if (imi != NULL) {
        // if (DEBUG) {
        //     Slog.d(TAG, "New default IME was selected: " + imi.getId());
        // }
        String id;
        imi->GetId(&id);
        ResetSelectedInputMethodAndSubtypeLocked(id);
        return TRUE;
    }

    return FALSE;
}

void CInputMethodManagerService::BuildInputMethodListLocked(
    /* [in] */ InputMethodInfoList* list,
    /* [in] */ HashMap<String, AutoPtr<IInputMethodInfo> >* map)
{
    if (DEBUG) {
        Slogger::D(TAG, "--- re-buildInputMethodList, \n ------ \n");
    }
    list->Clear();
    map->Clear();

    // Use for queryIntentServicesAsUser
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IConfiguration> config;
    mRes->GetConfiguration((IConfiguration**)&config);
    Int32 keyboard = 0;
    config->GetKeyboard(&keyboard);
    const Boolean haveHardKeyboard = keyboard == IConfiguration::KEYBOARD_QWERTY;
    String disabledSysImes = mSettings->GetDisabledSystemInputMethods();
    if (disabledSysImes.IsNull()) disabledSysImes = "";

    AutoPtr<IIntent> intent;
    CIntent::New(IInputMethod::SERVICE_INTERFACE, (IIntent**)&intent);
    AutoPtr<IObjectContainer> query;
    pm->QueryIntentServicesAsUser(intent, IPackageManager::GET_META_DATA, mSettings->GetCurrentUserId(),
            (IObjectContainer**)&query);

    AutoPtr<IObjectEnumerator> svItem;
    query->GetObjectEnumerator((IObjectEnumerator**)&svItem);
    Boolean hasNext = FALSE;

    AutoPtr<IObjectStringMap> additionalSubtypes = mFileManager->GetAllAdditionalInputMethodSubtypes();

    String pkgName, name, permission, strCompName, id;
    while(svItem->MoveNext(&hasNext), hasNext) {
        AutoPtr<IResolveInfo> ri;
        svItem->Current((IInterface**)&ri);
        AutoPtr<IServiceInfo> si;
        ri->GetServiceInfo((IServiceInfo**)&si);
        si->GetPackageName(&pkgName);
        si->GetName(&name);
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
        CInputMethodInfo::New(mContext, ri, additionalSubtypes, (IInputMethodInfo**)&p);
        list->PushBack(p);
        p->GetId(&id);
        (*map)[id] = p;

        // Valid system default IMEs and IMEs that have English subtypes are enabled
        // by default
        if ((IsValidSystemDefaultIme(p, mContext) || IsSystemImeThatHasEnglishSubtype(p))) {
            SetInputMethodEnabledLocked(id, TRUE);
        }

        if (DEBUG) {
            Slogger::D(TAG, "Found a third-party input method %s", id.string());
        }

        // } catch (XmlPullParserException e) {
        //     Slog.w(TAG, "Unable to load input method " + compName, e);
        // } catch (IOException e) {
        //     Slog.w(TAG, "Unable to load input method " + compName, e);
        // }
    }

    String defaultImiId = mSettings->GetSelectedInputMethod();
    if (!defaultImiId.IsNullOrEmpty()) {
        if (map->Find(defaultImiId) == map->End()) {
            Slogger::W(TAG, "Default IME is uninstalled. Choose new default IME.");
            if (ChooseNewDefaultIMELocked()) {
                UpdateFromSettingsLocked();
            }
        }
        else {
            // Double check that the default IME is certainly enabled.
            SetInputMethodEnabledLocked(defaultImiId, TRUE);
        }
    }
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
    // assert(0);
    Slogger::E(TAG, "// TODO: ShowInputMethodMenuInternal() E_NOT_IMPLEMENTED Line: %d", __LINE__);
    // if (DEBUG) Slog.v(TAG, "Show switching menu");

    // final Context context = mContext;
    // final boolean isScreenLocked = isScreenLocked();

    // final String lastInputMethodId = mSettings.getSelectedInputMethod();
    // int lastInputMethodSubtypeId = getSelectedInputMethodSubtypeId(lastInputMethodId);
    // if (DEBUG) Slog.v(TAG, "Current IME: " + lastInputMethodId);

    // synchronized(mMethodMap) {
    //     final HashMap<InputMethodInfo, List<InputMethodSubtype>> immis =
    //             getExplicitlyOrImplicitlyEnabledInputMethodsAndSubtypeListLocked();
    //     if (immis == NULL || immis.size() == 0) {
    //         return;
    //     }

    //     hideInputMethodMenuLocked();

    //     final List<ImeSubtypeListItem> imList =
    //             mImListManager.getSortedInputMethodAndSubtypeList(
    //                     showSubtypes, mInputShown, isScreenLocked);

    //     if (lastInputMethodSubtypeId == NOT_A_SUBTYPE_ID) {
    //         final InputMethodSubtype currentSubtype = getCurrentInputMethodSubtypeLocked();
    //         if (currentSubtype != NULL) {
    //             final InputMethodInfo currentImi = mMethodMap->get(mCurMethodId);
    //             lastInputMethodSubtypeId =
    //                     getSubtypeIdFromHashCode(currentImi, currentSubtype.hashCode());
    //         }
    //     }

    //     final int N = imList.size();
    //     mIms = new InputMethodInfo[N];
    //     mSubtypeIds = new int[N];
    //     int checkedItem = 0;
    //     for (int i = 0; i < N; ++i) {
    //         final ImeSubtypeListItem item = imList.get(i);
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
    //     final TypedArray a = context.obtainStyledAttributes(NULL,
    //             com.android.internal.R.styleable.DialogPreference,
    //             com.android.internal.R.attr.alertDialogStyle, 0);
    //     mDialogBuilder = new AlertDialog.Builder(context)
    //             .setOnCancelListener(new OnCancelListener() {
    //                 @Override
    //                 public void onCancel(DialogInterface dialog) {
    //                     hideInputMethodMenu();
    //                 }
    //             })
    //             .setIcon(a.getDrawable(
    //                     com.android.internal.R.styleable.DialogPreference_dialogTitle));
    //     a.recycle();
    //     final LayoutInflater inflater =
    //             (LayoutInflater)mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
    //     final View tv = inflater.inflate(
    //             com.android.internal.R.layout.input_method_switch_dialog_title, NULL);
    //     mDialogBuilder.setCustomTitle(tv);

    //     // Setup layout for a toggle switch of the hardware keyboard
    //     mSwitchingDialogTitleView = tv;
    //     mSwitchingDialogTitleView.findViewById(
    //             com.android.internal.R.id.hard_keyboard_section).setVisibility(
    //                     mWindowManagerService.isHardKeyboardAvailable() ?
    //                             View.VISIBLE : View.GONE);
    //     final Switch hardKeySwitch =  ((Switch)mSwitchingDialogTitleView.findViewById(
    //             com.android.internal.R.id.hard_keyboard_switch));
    //     hardKeySwitch.setChecked(mWindowManagerService.isHardKeyboardEnabled());
    //     hardKeySwitch.setOnCheckedChangeListener(
    //             new OnCheckedChangeListener() {
    //                 @Override
    //                 public void onCheckedChanged(
    //                         CompoundButton buttonView, boolean isChecked) {
    //                     mWindowManagerService.setHardKeyboardEnabled(isChecked);
    //                 }
    //             });

    //     final ImeSubtypeListAdapter adapter = new ImeSubtypeListAdapter(context,
    //             com.android.internal.R.layout.simple_list_item_2_single_choice, imList,
    //             checkedItem);

    //     mDialogBuilder.setSingleChoiceItems(adapter, checkedItem,
    //             new AlertDialog.OnClickListener() {
    //                 @Override
    //                 public void onClick(DialogInterface dialog, int which) {
    //                     synchronized(mMethodMap) {
    //                         if (mIms == NULL || mIms.length <= which
    //                                 || mSubtypeIds == NULL || mSubtypeIds.length <= which) {
    //                             return;
    //                         }
    //                         InputMethodInfo im = mIms[which];
    //                         int subtypeId = mSubtypeIds[which];
    //                         hideInputMethodMenu();
    //                         if (im != NULL) {
    //                             if ((subtypeId < 0)
    //                                     || (subtypeId >= im.getSubtypeCount())) {
    //                                 subtypeId = NOT_A_SUBTYPE_ID;
    //                             }
    //                             setInputMethodLocked(im.getId(), subtypeId);
    //                         }
    //                     }
    //                 }
    //             });

    //     if (showSubtypes && !isScreenLocked) {
    //         mDialogBuilder.setPositiveButton(
    //                 com.android.internal.R.string.configure_input_methods,
    //                 new DialogInterface.OnClickListener() {
    //                     @Override
    //                     public void onClick(DialogInterface dialog, int whichButton) {
    //                         showConfigureInputMethods();
    //                     }
    //                 });
    //     }
    //     mSwitchingDialog = mDialogBuilder.create();
    //     mSwitchingDialog.setCanceledOnTouchOutside(true);
    //     mSwitchingDialog.getWindow().setType(
    //             WindowManager.LayoutParams.TYPE_INPUT_METHOD_DIALOG);
    //     mSwitchingDialog.getWindow().getAttributes().privateFlags |=
    //             WindowManager.LayoutParams.PRIVATE_FLAG_SHOW_FOR_ALL_USERS;
    //     mSwitchingDialog.getWindow().getAttributes().setTitle("Select input method");
    //     mSwitchingDialog.show();
    // }
}

void CInputMethodManagerService::HideInputMethodMenu()
{
    AutoLock lock(mMethodMapLock);
    HideInputMethodMenuLocked();
}

void CInputMethodManagerService::HideInputMethodMenuLocked()
{
    // if (DEBUG) Slog.v(TAG, "Hide switching menu");

    if (mSwitchingDialog != NULL) {
        mSwitchingDialog->Dismiss();
        mSwitchingDialog = NULL;
    }

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
    AutoLock lock(mMethodMapLock);

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
    // HashMap<String, IInputMethodInfo*>::Iterator it =
    //         mMethodMap->Find(id);
    // if (it == mMethodMap->End()) {
    //     assert(0);
    //     //throw new IllegalArgumentException("Unknown id: " + mCurMethodId);
    // }

    return TRUE;

    //TODO
    // List<Pair<String, ArrayList<String>>> enabledInputMethodsList =
    //         mSettings->GetEnabledInputMethodsAndSubtypeListLocked();

    // if (enabled) {
    //     for (Pair<String, ArrayList<String>> pair: enabledInputMethodsList) {
    //         if (pair.first.equals(id)) {
    //             // We are enabling this input method, but it is already enabled.
    //             // Nothing to do. The previous state was enabled.
    //             return true;
    //         }
    //     }
    //     mSettings->AppendAndPutEnabledInputMethodLocked(id, FALSE);
    //     // Previous state was disabled.
    //     return FALSE;
    // } else {
    //     StringBuilder builder = new StringBuilder();
    //     if (mSettings.buildAndPutEnabledInputMethodsStrRemovingIdLocked(
    //             builder, enabledInputMethodsList, id)) {
    //         // Disabled input method is currently selected, switch to another one.
    //         final String selId = mSettings.getSelectedInputMethod();
    //         if (id.equals(selId) && !chooseNewDefaultIMELocked()) {
    //             Slog.i(TAG, "Can't find new IME, unsetting the current input method.");
    //             resetSelectedInputMethodAndSubtypeLocked("");
    //         }
    //         // Previous state was enabled.
    //         return true;
    //     } else {
    //         // We are disabling the input method but it is already disabled.
    //         // Nothing to do.  The previous state was disabled.
    //         return false;
    //     }
    // }
}

Boolean CInputMethodManagerService::CanAddToLastInputMethod(
    /* [in] */ IInputMethodSubtype* subtype)
{
    if (subtype == NULL) return TRUE;
    Boolean can = FALSE;
    subtype->IsAuxiliary(&can);
    return !can;
}

void CInputMethodManagerService::SaveCurrentInputMethodAndSubtypeToHistory()
{
    String subtypeId = NOT_A_SUBTYPE_ID_STR;
    if (mCurrentSubtype != NULL) {
        Int32 hashCode = 0;
        mCurrentSubtype->GetHashCode(&hashCode);
        subtypeId = StringUtils::ToString(hashCode);
    }
    if (CanAddToLastInputMethod(mCurrentSubtype)) {
        mSettings->AddSubtypeToHistory(mCurMethodId, subtypeId);
    }
}

void CInputMethodManagerService::SetSelectedInputMethodAndSubtypeLocked(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ Int32 subtypeId,
    /* [in] */ Boolean setSubtypeOnly)
{
    // Update the history of InputMethod and Subtype
    SaveCurrentInputMethodAndSubtypeToHistory();

    // Set Subtype here
    if (imi == NULL || subtypeId < 0) {
        mSettings->PutSelectedSubtype(NOT_A_SUBTYPE_ID);
        mCurrentSubtype = NULL;
    } else {
        Int32 count = 0;
        if (subtypeId < (imi->GetSubtypeCount(&count), count)) {
            AutoPtr<IInputMethodSubtype> subtype;
            imi->GetSubtypeAt(subtypeId, (IInputMethodSubtype**)&subtype);
            assert(subtype != NULL);
            Int32 hashCode;
            subtype->GetHashCode(&hashCode);
            mSettings->PutSelectedSubtype(hashCode);
            mCurrentSubtype = subtype;
        } else {
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
    AutoPtr<IInputMethodInfo> imi;
    HashMap<String, AutoPtr<IInputMethodInfo> >::Iterator it = mMethodMap->Find(newDefaultIme);
    if (it != mMethodMap->End()) {
        imi = it->mSecond;
    }
    Int32 lastSubtypeId = NOT_A_SUBTYPE_ID;
    // newDefaultIme is empty when there is no candidate for the selected IME.
    if (imi != NULL && !newDefaultIme.IsNullOrEmpty()) {
        String subtypeHashCode = mSettings->GetLastSubtypeForInputMethodLocked(newDefaultIme);
        if (!subtypeHashCode.IsNull()) {
            // try {
            lastSubtypeId = GetSubtypeIdFromHashCode(
                    imi, StringUtils::ParseInt32(subtypeHashCode));
            // } catch (NumberFormatException e) {
            //     Slog.w(TAG, "GetHashCode for subtype looks broken: " + subtypeHashCode, e);
            // }
        }
    }
    SetSelectedInputMethodAndSubtypeLocked(imi, lastSubtypeId, FALSE);
}

Int32 CInputMethodManagerService::GetSelectedInputMethodSubtypeId(
    /* [in] */ const String& id)
{
    AutoPtr<IInputMethodInfo> imi;
    HashMap<String, AutoPtr<IInputMethodInfo> >::Iterator it = mMethodMap->Find(id);
    if (it != mMethodMap->End()) {
        imi = it->mSecond;
    }
    if (imi == NULL) {
        return NOT_A_SUBTYPE_ID;
    }
    Int32 subtypeHashCode = mSettings->GetSelectedInputMethodSubtypeHashCode();
    return GetSubtypeIdFromHashCode(imi, subtypeHashCode);
}

Boolean CInputMethodManagerService::IsValidSubtypeId(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ Int32 subtypeHashCode)
{
    return GetSubtypeIdFromHashCode(imi, subtypeHashCode) != NOT_A_SUBTYPE_ID;
}

Int32 CInputMethodManagerService::GetSubtypeIdFromHashCode(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ Int32 subtypeHashCode)
{
    if (imi != NULL) {
        Int32 subtypeCount = 0;
        imi->GetSubtypeCount(&subtypeCount);
        for (Int32 i = 0; i < subtypeCount; ++i) {
            AutoPtr<IInputMethodSubtype> ims;
            imi->GetSubtypeAt(i, (IInputMethodSubtype**)&ims);
            Int32 hashCode;
            ims->GetHashCode(&hashCode);
            if (subtypeHashCode == hashCode) {
                return i;
            }
        }
    }
    return NOT_A_SUBTYPE_ID;
}

AutoPtr< InputMethodSubtypeList > CInputMethodManagerService::GetImplicitlyApplicableSubtypesLocked(
    /* [in] */ IResources* res,
    /* [in] */ IInputMethodInfo* imi)
{
    // final List<InputMethodSubtype> subtypes = getSubtypes(imi);
    // final String systemLocale = res.getConfiguration().locale.toString();
    // if (TextUtils::IsEmpty(systemLocale)) return new ArrayList<InputMethodSubtype>();
    // final HashMap<String, InputMethodSubtype> applicableModeAndSubtypesMap =
    //         new HashMap<String, InputMethodSubtype>();
    // final int N = subtypes.size();
    // for (int i = 0; i < N; ++i) {
    //     // scan overriding implicitly enabled subtypes.
    //     InputMethodSubtype subtype = subtypes.get(i);
    //     if (subtype.overridesImplicitlyEnabledSubtype()) {
    //         final String mode = subtype.getMode();
    //         if (!applicableModeAndSubtypesMap.containsKey(mode)) {
    //             applicableModeAndSubtypesMap.put(mode, subtype);
    //         }
    //     }
    // }
    // if (applicableModeAndSubtypesMap.size() > 0) {
    //     return new ArrayList<InputMethodSubtype>(applicableModeAndSubtypesMap.values());
    // }
    // for (int i = 0; i < N; ++i) {
    //     final InputMethodSubtype subtype = subtypes.get(i);
    //     final String locale = subtype.getLocale();
    //     final String mode = subtype.getMode();
    //     // When system locale starts with subtype's locale, that subtype will be applicable
    //     // for system locale
    //     // For instance, it's clearly applicable for cases like system locale = en_US and
    //     // subtype = en, but it is not necessarily considered applicable for cases like system
    //     // locale = en and subtype = en_US.
    //     // We just call systemLocale.startsWith(locale) in this function because there is no
    //     // need to find applicable subtypes aggressively unlike
    //     // findLastResortApplicableSubtypeLocked.
    //     if (systemLocale.startsWith(locale)) {
    //         final InputMethodSubtype applicableSubtype = applicableModeAndSubtypesMap.get(mode);
    //         // If more applicable subtypes are contained, skip.
    //         if (applicableSubtype != NULL) {
    //             if (systemLocale.equals(applicableSubtype.getLocale())) continue;
    //             if (!systemLocale.equals(locale)) continue;
    //         }
    //         applicableModeAndSubtypesMap.put(mode, subtype);
    //     }
    // }
    // final InputMethodSubtype keyboardSubtype
    //         = applicableModeAndSubtypesMap.get(SUBTYPE_MODE_KEYBOARD);
    // final ArrayList<InputMethodSubtype> applicableSubtypes = new ArrayList<InputMethodSubtype>(
    //         applicableModeAndSubtypesMap.values());
    // if (keyboardSubtype != NULL && !keyboardSubtype.containsExtraValueKey(TAG_ASCII_CAPABLE)) {
    //     for (int i = 0; i < N; ++i) {
    //         final InputMethodSubtype subtype = subtypes.get(i);
    //         final String mode = subtype.getMode();
    //         if (SUBTYPE_MODE_KEYBOARD.equals(mode) && subtype.containsExtraValueKey(
    //                 TAG_ENABLED_WHEN_DEFAULT_IS_NOT_ASCII_CAPABLE)) {
    //             applicableSubtypes.add(subtype);
    //         }
    //     }
    // }
    // if (keyboardSubtype == NULL) {
    //     InputMethodSubtype lastResortKeyboardSubtype = findLastResortApplicableSubtypeLocked(
    //             res, subtypes, SUBTYPE_MODE_KEYBOARD, systemLocale, true);
    //     if (lastResortKeyboardSubtype != NULL) {
    //         applicableSubtypes.add(lastResortKeyboardSubtype);
    //     }
    // }
    // return applicableSubtypes;
    PFL_EX("TODO:")
    AutoPtr<InputMethodSubtypeList> retList = new InputMethodSubtypeList();
    return retList;
}

AutoPtr<IInputMethodSubtype> CInputMethodManagerService::FindLastResortApplicableSubtypeLocked(
    /* [in] */ IResources* res,
    /* [in] */ InputMethodSubtypeList* subtypes,
    /* [in] */ const String& mode,
    /* [in] */ const String& locale,
    /* [in] */ Boolean canIgnoreLocaleAsLastResort)
{
    // if (subtypes == NULL || subtypes.size() == 0) {
    //     return NULL;
    // }
    // if (TextUtils::IsEmpty(locale)) {
    //     locale = res.getConfiguration().locale.toString();
    // }
    // final String language = locale.substring(0, 2);
    // boolean partialMatchFound = false;
    // InputMethodSubtype applicableSubtype = NULL;
    // InputMethodSubtype firstMatchedModeSubtype = NULL;
    // final int N = subtypes.size();
    // for (int i = 0; i < N; ++i) {
    //     InputMethodSubtype subtype = subtypes.get(i);
    //     final String subtypeLocale = subtype.getLocale();
    //     // An applicable subtype should match "mode". If mode is NULL, mode will be ignored,
    //     // and all subtypes with all modes can be candidates.
    //     if (mode == NULL || subtypes.get(i).getMode().equalsIgnoreCase(mode)) {
    //         if (firstMatchedModeSubtype == NULL) {
    //             firstMatchedModeSubtype = subtype;
    //         }
    //         if (locale.equals(subtypeLocale)) {
    //             // Exact match (e.g. system locale is "en_US" and subtype locale is "en_US")
    //             applicableSubtype = subtype;
    //             break;
    //         } else if (!partialMatchFound && subtypeLocale.startsWith(language)) {
    //             // Partial match (e.g. system locale is "en_US" and subtype locale is "en")
    //             applicableSubtype = subtype;
    //             partialMatchFound = true;
    //         }
    //     }
    // }

    // if (applicableSubtype == NULL && canIgnoreLocaleAsLastResort) {
    //     return firstMatchedModeSubtype;
    // }

    // // The first subtype applicable to the system locale will be defined as the most applicable
    // // subtype.
    // if (DEBUG) {
    //     if (applicableSubtype != NULL) {
    //         Slog.d(TAG, "Applicable InputMethodSubtype was found: "
    //                 + applicableSubtype.getMode() + "," + applicableSubtype.getLocale());
    //     }
    // }
    // return applicableSubtype;
    assert(0);
    return NULL;
}

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
//             Slog.w(TAG, "Most applicable shortcut input method was:"
//                     + mostApplicableIMI.getId());
//             if (mostApplicableSubtype != NULL) {
//                 Slog.w(TAG, "Most applicable shortcut input method subtype was:"
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


ECode CInputMethodManagerService::GetCurrentInputMethodSubtype(
    /* [out] */ IInputMethodSubtype** subtype)
{
    VALIDATE_NOT_NULL(subtype);

    // TODO: Make this work even for non-current users?
    if (!CalledFromValidUser()) {
        *subtype = NULL;
        return NOERROR;
    }
    AutoLock lock(mMethodMapLock);

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
    Boolean subtypeIsSelected =
            mSettings->GetSelectedInputMethodSubtypeHashCode() != NOT_A_SUBTYPE_ID;
    AutoPtr<IInputMethodInfo> imi;
    HashMap<String, AutoPtr<IInputMethodInfo> >::Iterator it = mMethodMap->Find(mCurMethodId);
    if (it != mMethodMap->End()) {
        imi = it->mSecond;
    }
    Int32 count = 0;
    if (imi == NULL || (imi->GetSubtypeCount(&count), count) == 0) {
        return NULL;
    }
    Int32 hashCode;
    if (!subtypeIsSelected || mCurrentSubtype == NULL
            || !IsValidSubtypeId(imi, (mCurrentSubtype->GetHashCode(&hashCode), hashCode))) {
        Int32 subtypeId = GetSelectedInputMethodSubtypeId(mCurMethodId);
        if (subtypeId == NOT_A_SUBTYPE_ID) {
            // If there are no selected subtypes, the framework will try to find
            // the most applicable subtype from explicitly or implicitly enabled
            // subtypes.
            AutoPtr<IObjectContainer> explicitlyOrImplicitlyEnabledSubtypes;
            GetEnabledInputMethodSubtypeListLocked(imi, TRUE, (IObjectContainer**)&explicitlyOrImplicitlyEnabledSubtypes);
            // If there is only one explicitly or implicitly enabled subtype,
            // just returns it.
            Int32 size = 0;
            if ((explicitlyOrImplicitlyEnabledSubtypes->GetObjectCount(&size), size) == 1) {
                Boolean hasNext = FALSE;
                AutoPtr<IObjectEnumerator> enumAtor;
                explicitlyOrImplicitlyEnabledSubtypes->GetObjectEnumerator((IObjectEnumerator**)&enumAtor);
                enumAtor->MoveNext(&hasNext);
                if (hasNext) {
                    mCurrentSubtype = NULL;
                    enumAtor->Current((IInterface**)&mCurrentSubtype);
                }
            }
            else if (size > 1) {
                assert(0);
                // mCurrentSubtype = FindLastResortApplicableSubtypeLocked(
                //         mRes, explicitlyOrImplicitlyEnabledSubtypes,
                //         SUBTYPE_MODE_KEYBOARD, String(NULL), TRUE);
                if (mCurrentSubtype == NULL) {
                    // mCurrentSubtype = FindLastResortApplicableSubtypeLocked(
                    //         mRes, explicitlyOrImplicitlyEnabledSubtypes, String(NULL),
                    //         String(NULL), TRUE);
                }
            }
        }
        else {
            AutoPtr< InputMethodSubtypeList > subtypes = GetSubtypes(imi);
            mCurrentSubtype = (*subtypes)[subtypeId];
        }
    }
    return mCurrentSubtype;
}

void CInputMethodManagerService::AddShortcutInputMethodAndSubtypes(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ IInputMethodSubtype* subtype)
{
    // if (mShortcutInputMethodsAndSubtypes.containsKey(imi)) {
    //     mShortcutInputMethodsAndSubtypes.get(imi).add(subtype);
    // } else {
    //     ArrayList<InputMethodSubtype> subtypes = new ArrayList<InputMethodSubtype>();
    //     subtypes.add(subtype);
    //     mShortcutInputMethodsAndSubtypes.put(imi, subtypes);
    // }
}

ECode CInputMethodManagerService::GetShortcutInputMethodsAndSubtypes(
    /* [out] */ IObjectContainer** ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = NULL;

    AutoLock lock(mMethodMapLock);
    if (mShortcutInputMethodsAndSubtypes.IsEmpty()) {
        // If there are no selected shortcut subtypes, the framework will try to find
        // the most applicable subtype from all subtypes whose mode is
        // SUBTYPE_MODE_VOICE. This is an exceptional case, so we will hardcode the mode.
        // Pair<InputMethodInfo, InputMethodSubtype> info =
        //     FindLastResortApplicableShortcutInputMethodAndSubtypeLocked(
        //             SUBTYPE_MODE_VOICE);
        // if (info != NULL) {
        //     ret.add(info.first);
        //     ret.add(info.second);
        // }
        return NOERROR;
    }

//    HashMap<IInputMethodInfo*, IObjectContainer*>::Iterator ator = mShortcutInputMethodsAndSubtypes.Begin();
//    for (; ator != mShortcutInputMethodsAndSubtypes.End(); ++ator) {
//        AutoPtr<IInputMethodInfo> imi = ator->mFirst;
//        (*ret)->Add(imi);
//
//        AutoPtr<IObjectContainer> subtypes = ator->mSecond;
//        assert(subtypes != NULL);
//        AutoPtr<IObjectEnumerator> enumAtor;
//        subtypes->GetObjectEnumerator((IObjectEnumerator**)&enumAtor);
//
//        Boolean hasNext = FALSE;
//        while (enumAtor->MoveNext(&hasNext) ,hasNext) {
//            AutoPtr<IInputMethodSubtype> subtype;
//            enumAtor->Current((IInterface**)&subtype);
//
//            (*ret)->Add(subtype);
//        }
//    }

    return NOERROR;
}

ECode CInputMethodManagerService::SetCurrentInputMethodSubtype(
    /* [in] */ IInputMethodSubtype* subtype,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // TODO: Make this work even for non-current users?
    if (!CalledFromValidUser()) {
        *result = FALSE;
        return NOERROR;
    }
    AutoLock lock(mMethodMapLock);

    if (subtype != NULL && !mCurMethodId.IsNull()) {
        AutoPtr<IInputMethodInfo> imi;
        HashMap<String, AutoPtr<IInputMethodInfo> >::Iterator it = mMethodMap->Find(mCurMethodId);
        if (it != mMethodMap->End()) {
            imi = it->mSecond;
        }
        Int32 hashCode;
        subtype->GetHashCode(&hashCode);
        Int32 subtypeId = GetSubtypeIdFromHashCode(imi, hashCode);
        if (subtypeId != NOT_A_SUBTYPE_ID) {
            SetInputMethodLocked(mCurMethodId, subtypeId);
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

// // ----------------------------------------------------------------------
// // Utilities for debug
// private static String GetStackTrace() {
//     final StringBuilder sb = new StringBuilder();
//     try {
//         throw new RuntimeException();
//     } catch (RuntimeException e) {
//         final StackTraceElement[] frames = e.getStackTrace();
//         // Start at 1 because the first frame is here and we don't care about it
//         for (int j = 1; j < frames.length; ++j) {
//             sb.append(frames[j].toString() + "\n");
//         }
//     }
//     return sb.toString();
// }

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

//     synchronized(mMethodMap) {
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
//         p.println("  mSystemReady=" + mSystemReady + " mScreenOn=" + mScreenOn);
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
    *result = TRUE;

    Int32 what;
    msg->GetWhat(&what);

    AutoPtr<SomeArgs> args;
    switch (what) {
        case MSG_SHOW_IM_PICKER:
            ShowInputMethodMenu();
            break;

        case MSG_SHOW_IM_SUBTYPE_PICKER:
            ShowInputMethodSubtypeMenu();
            break;

        case MSG_SHOW_IM_SUBTYPE_ENABLER: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            args = (SomeArgs*)obj.Get();

            String str;
            AutoPtr<ICharSequence> seq = ICharSequence::Probe(args->mArg1);
            seq->ToString(&str);
            ShowInputMethodAndSubtypeEnabler(str);
            args->Recycle();
            break;
        }

        case MSG_SHOW_IM_CONFIG:
            ShowConfigureInputMethods();
            break;

        // ---------------------------------------------------------

        case MSG_UNBIND_INPUT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            IIInputMethod* im = IIInputMethod::Probe(obj);
            im->UnbindInput();
            break;
        }
        case MSG_BIND_INPUT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            args = (SomeArgs*)obj.Get();
            IIInputMethod* im = IIInputMethod::Probe(args->mArg1);
            IInputBinding* ib = IInputBinding::Probe(args->mArg2);
            im->BindInput(ib);
            args->Recycle();
            break;
        }
        case MSG_SHOW_SOFT_INPUT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            args = (SomeArgs*)obj.Get();
            Int32 arg1;
            msg->GetArg1(&arg1);
            IIInputMethod* im = IIInputMethod::Probe(args->mArg1);
            IResultReceiver* rr = IResultReceiver::Probe(args->mArg2);
            im->ShowSoftInput(arg1, rr);
            args->Recycle();
            break;
        }

        case MSG_HIDE_SOFT_INPUT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            args = (SomeArgs*)obj.Get();
            IIInputMethod* im = IIInputMethod::Probe(args->mArg1);
            IResultReceiver* rr = IResultReceiver::Probe(args->mArg2);
            im->HideSoftInput(0, rr);
            args->Recycle();
            break;
        }

        case MSG_ATTACH_TOKEN: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            args = (SomeArgs*)obj.Get();
            IIInputMethod* im = IIInputMethod::Probe(args->mArg1);
            IBinder* binder = IBinder::Probe(args->mArg2);
            im->AttachToken(binder);
            args->Recycle();
            break;
        }

        case MSG_CREATE_SESSION: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            args = (SomeArgs*)obj.Get();
            IIInputMethod* im = IIInputMethod::Probe(args->mArg1);
            IInputMethodCallback* callback = IInputMethodCallback::Probe(args->mArg2);
            im->CreateSession(callback);
            args->Recycle();
            break;
        }

        // ---------------------------------------------------------

        case MSG_START_INPUT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            args = (SomeArgs*)obj.Get();

            SessionState* session = (SessionState*)args->mArg1.Get();
            SetEnabledSessionInMainThread(session);
            IInputContext* ic = IInputContext::Probe(args->mArg2);
            IEditorInfo* ei = IEditorInfo::Probe(args->mArg3);
            session->mMethod->StartInput(ic, ei);
            args->Recycle();
            break;
        }

        case MSG_RESTART_INPUT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            args = (SomeArgs*)obj.Get();

            SessionState* session = (SessionState*)args->mArg1.Get();
            SetEnabledSessionInMainThread(session);
            IInputContext* ic = IInputContext::Probe(args->mArg2);
            IEditorInfo* ei = IEditorInfo::Probe(args->mArg3);
            session->mMethod->RestartInput(ic, ei);
            args->Recycle();
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
            break;
        }

        case MSG_BIND_METHOD:  {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            args = (SomeArgs*)obj.Get();
            IInputMethodClient* imc = IInputMethodClient::Probe(args->mArg1);
            IInputBindResult* ibr = IInputBindResult::Probe(args->mArg2);
            imc->OnBindMethod(ibr);
            args->Recycle();
            break;
        }

        case MSG_SET_ACTIVE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ClientState* cs = (ClientState*)obj.Get();
            Int32 arg1;
            msg->GetArg1(&arg1);
            cs->mClient->SetActive(arg1 != 0);
            break;
        }

        // --------------------------------------------------------------
        case MSG_HARD_KEYBOARD_SWITCH_CHANGED: {
            Int32 arg1, arg2;
            msg->GetArg1(&arg1);
            msg->GetArg2(&arg2);
            mHardKeyboardListener->HandleHardKeyboardStatusChange(arg1, arg2);
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
