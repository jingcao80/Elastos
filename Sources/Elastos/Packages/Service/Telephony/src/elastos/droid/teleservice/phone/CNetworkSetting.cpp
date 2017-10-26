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

#include "elastos/droid/teleservice/phone/CNetworkSetting.h"
#include "elastos/droid/teleservice/phone/CNetworkSettingNetworkQueryServiceCallback.h"
#include "elastos/droid/teleservice/phone/NotificationMgr.h"
#include "elastos/droid/teleservice/phone/PhoneGlobals.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/text/TextUtils.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/CoreUtils.h>
#include "R.h"

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::CProgressDialog;
using Elastos::Droid::App::IProgressDialog;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnCancelListener;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Preference::CPreference;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Internal::Telephony::ICommandException;
using Elastos::Core::StringBuilder;
using Elastos::Core::CoreUtils;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::CHashMap;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CNetworkSetting::MyHandler::MyHandler(
    /* [in] */ CNetworkSetting* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode CNetworkSetting::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<AsyncResult> ar;

    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case EVENT_NETWORK_SCAN_COMPLETED:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->NetworksListLoaded(IList::Probe(obj), arg1);
            break;
        }
        case EVENT_NETWORK_SELECTION_DONE:
        {
            if (DBG) mHost->Log(String("hideProgressPanel"));
            mHost->RemoveDialog(DIALOG_NETWORK_SELECTION);

            AutoPtr<IPreferenceScreen> screen;
            mHost->GetPreferenceScreen((IPreferenceScreen**)&screen);
            IPreference::Probe(screen)->SetEnabled(TRUE);

            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            if (ar->mException != NULL) {
                if (DBG) mHost->Log(String("manual network selection: failed!"));
                mHost->DisplayNetworkSelectionFailed(ar->mException);
            }
            else {
                if (DBG) mHost->Log(String("manual network selection: succeeded!"));
                mHost->DisplayNetworkSelectionSucceeded();
            }
            break;
        }
        case EVENT_AUTO_SELECT_DONE:
        {
            if (DBG) mHost->Log(String("hideProgressPanel"));

            // Always try to dismiss the dialog because activity may
            // be moved to background after dialog is shown.
            //try {
            ECode ec = mHost->DismissDialog(DIALOG_NETWORK_AUTO_SELECT);
            //} catch (IllegalArgumentException e) {
            if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
            //    // "auto select" is always trigged in foreground, so "auto select" dialog
                //  should be shown when "auto select" is trigged. Should NOT get
                // this exception, and Log it.
                Logger::W(TAG, "[NetworksList] Fail to dismiss auto select dialog %d", ec);
            }
            //}
            AutoPtr<IPreferenceScreen> screen;
            mHost->GetPreferenceScreen((IPreferenceScreen**)&screen);
            IPreference::Probe(screen)->SetEnabled(TRUE);

            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            if (ar->mException != NULL) {
                if (DBG) mHost->Log(String("automatic network selection: failed!"));
                mHost->DisplayNetworkSelectionFailed(ar->mException);
            }
            else {
                if (DBG) mHost->Log(String("automatic network selection: succeeded!"));
                mHost->DisplayNetworkSelectionSucceeded();
            }
            break;
        }
    }

    return NOERROR;
}

CAR_INTERFACE_IMPL(CNetworkSetting::MyServiceConnection, Object, IServiceConnection)

ECode CNetworkSetting::MyServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* className,
    /* [in] */ IBinder* service)
{
    if (DBG) mHost->Log(String("connection created, binding local service."));
    INetworkQueryServiceLocalBinder::Probe(service)->GetService((IINetworkQueryService**)&(mHost->mNetworkQueryService));
    // as soon as it is bound, run a query.
    mHost->LoadNetworksList();
    return NOERROR;
}

ECode CNetworkSetting::MyServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* className)
{
    if (DBG) mHost->Log(String("connection disconnected, cleaning local binding."));
    mHost->mNetworkQueryService = NULL;
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(CNetworkSetting::MyNetworkQueryServiceCallback, Object,
        IINetworkQueryServiceCallback, IBinder)

ECode CNetworkSetting::MyNetworkQueryServiceCallback::constructor(
    /* [in] */ INetworkSetting* host)
{
    mHost = (CNetworkSetting*)host;
    return NOERROR;
}

ECode CNetworkSetting::MyNetworkQueryServiceCallback::OnQueryComplete(
    /* [in] */ IList* networkInfoArray,
    /* [in] */ Int32 status)
{
    if (DBG) mHost->Log(String("notifying message loop of query completion."));
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(EVENT_NETWORK_SCAN_COMPLETED,
            status, 0, networkInfoArray, (IMessage**)&msg);
    return msg->SendToTarget();
}

ECode CNetworkSetting::MyRunnable::Run()
{
    return mHost->Finish();
}

const String CNetworkSetting::TAG("phone");
const Boolean CNetworkSetting::DBG = FALSE;

const String CNetworkSetting::LIST_NETWORKS_KEY("list_networks_key");
const String CNetworkSetting::BUTTON_SRCH_NETWRKS_KEY("button_srch_netwrks_key");
const String CNetworkSetting::BUTTON_AUTO_SELECT_KEY("button_auto_select_key");

CAR_INTERFACE_IMPL_2(CNetworkSetting, PreferenceActivity, INetworkSetting, IDialogInterfaceOnCancelListener)

CAR_OBJECT_IMPL(CNetworkSetting)

CNetworkSetting::CNetworkSetting()
    : mIsForeground(FALSE)
    , mUnavailable(FALSE)
    , mNetworkSelectMsg(NULL)
{
    mHandler = new MyHandler(this);

    mNetworkQueryServiceConnection = new MyServiceConnection(this);

    CNetworkSettingNetworkQueryServiceCallback::New(this, (IINetworkQueryServiceCallback**)&mCallback);
}

ECode CNetworkSetting::constructor()
{
    return PreferenceActivity::constructor();
}

ECode CNetworkSetting::OnPreferenceTreeClick(
    /* [in] */ IPreferenceScreen* preferenceScreen,
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean handled = FALSE;

    if (preference == mSearchButton) {
        LoadNetworksList();
        handled = TRUE;
    }
    else if (preference == mAutoSelect) {
        SelectNetworkAutomatic();
        handled = TRUE;
    }
    else {
        AutoPtr<IPreference> selectedCarrier = preference;

        AutoPtr<ICharSequence> title;
        selectedCarrier->GetTitle((ICharSequence**)&title);
        String networkStr;
        title->ToString(&networkStr);
        if (DBG) {
            StringBuilder sb;
            sb += "selected network: ";
            sb += networkStr;
            Log(sb.ToString());
        }

        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(EVENT_NETWORK_SELECTION_DONE, (IMessage**)&msg);
        AutoPtr<IInterface> obj;
        mNetworkMap->Get(TO_IINTERFACE(selectedCarrier), (IInterface**)&obj);
        AutoPtr<IOperatorInfo> info = IOperatorInfo::Probe(obj);
        mPhone->SelectNetworkManually(info, msg);

        DisplayNetworkSeletionInProgress(networkStr);

        handled = TRUE;
    }

    return handled;
}

ECode CNetworkSetting::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    // request that the service stop the query with this callback object.
    //try {
    ECode ec = mNetworkQueryService->StopNetworkQuery(mCallback);
    //} catch (RemoteException e) {
    if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        //throw new RuntimeException(e);
        Logger::E(TAG, "RuntimeException");
        return E_RUNTIME_EXCEPTION;
    }
    //}
    return Finish();
}

ECode CNetworkSetting::GetNormalizedCarrierName(
    /* [in] */ IOperatorInfo* ni,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)

    if (ni != NULL) {
        String alphaLong;
        ni->GetOperatorAlphaLong(&alphaLong);
        String numeric;
        ni->GetOperatorNumeric(&numeric);

        StringBuilder sb;
        sb += alphaLong;
        sb += " (";
        sb += numeric;
        sb += ")";
        return sb.ToString(name);
    }
    *name = NULL;
    return NOERROR;
}

ECode CNetworkSetting::OnCreate(
    /* [in] */ IBundle* icicle)
{
    PreferenceActivity::OnCreate(icicle);

    AutoPtr<IInterface> obj;
    GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    mUm = IUserManager::Probe(obj);

    Boolean res;
    if (mUm->HasUserRestriction(IUserManager::DISALLOW_CONFIG_MOBILE_NETWORKS, &res), res) {
        SetContentView(Elastos::Droid::TeleService::R::layout::telephony_disallowed_preference_screen);
        mUnavailable = TRUE;
        return NOERROR;
    }

    AddPreferencesFromResource(Elastos::Droid::TeleService::R::xml::carrier_select);

    mPhone = PhoneGlobals::GetPhone();

    AutoPtr<IPreferenceScreen> screen;
    GetPreferenceScreen((IPreferenceScreen**)&screen);
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(LIST_NETWORKS_KEY);
    AutoPtr<IPreference> preference;
    IPreferenceGroup::Probe(screen)->FindPreference(cchar, (IPreference**)&preference);

    mNetworkList = IPreferenceGroup::Probe(preference);
    CHashMap::New((IHashMap**)&mNetworkMap);

    AutoPtr<ICharSequence> cchar2 = CoreUtils::Convert(BUTTON_SRCH_NETWRKS_KEY);
    IPreferenceGroup::Probe(screen)->FindPreference(cchar2, (IPreference**)&mSearchButton);
    AutoPtr<ICharSequence> cchar3 = CoreUtils::Convert(BUTTON_AUTO_SELECT_KEY);
    IPreferenceGroup::Probe(screen)->FindPreference(cchar3, (IPreference**)&mAutoSelect);

    // Start the Network Query service, and bind it.
    // The OS knows to start he service only once and keep the instance around (so
    // long as startService is called) until a stopservice request is made.  Since
    // we want this service to just stay in the background until it is killed, we
    // don't bother stopping it from our end.
    AutoPtr<IIntent> intent;
    CIntent::New((IContext*)this, ECLSID_CNetworkQueryService, (IIntent**)&intent);
    AutoPtr<IComponentName> name;
    StartService(intent, (IComponentName**)&name);

    AutoPtr<IIntent> intent2;
    CIntent::New((IContext*)this, ECLSID_CNetworkQueryService, (IIntent**)&intent2);
    return BindService(intent2, mNetworkQueryServiceConnection, IContext::BIND_AUTO_CREATE, &res);
}

ECode CNetworkSetting::OnResume()
{
    PreferenceActivity::OnResume();
    mIsForeground = TRUE;
    return NOERROR;
}

ECode CNetworkSetting::OnPause()
{
    PreferenceActivity::OnPause();
    mIsForeground = FALSE;
    return NOERROR;
}

ECode CNetworkSetting::OnDestroy()
{
    if (!mUnavailable) {
        // unbind the service.
        UnbindService(mNetworkQueryServiceConnection);
    }
    return PreferenceActivity::OnDestroy();
}

AutoPtr<IDialog> CNetworkSetting::OnCreateDialog(
    /* [in] */ Int32 id)
{
    if ((id == DIALOG_NETWORK_SELECTION) || (id == DIALOG_NETWORK_LIST_LOAD) ||
            (id == DIALOG_NETWORK_AUTO_SELECT)) {
        AutoPtr<IProgressDialog> dialog;
        CProgressDialog::New(this, (IProgressDialog**)&dialog);
        switch (id) {
            case DIALOG_NETWORK_SELECTION:
            {
                // It would be more efficient to reuse this dialog by moving
                // this setMessage() into onPreparedDialog() and NOT use
                // removeDialog().  However, this is not possible since the
                // message is rendered only 2 times in the ProgressDialog -
                // after show() and before onCreate.
                AutoPtr<ICharSequence> cchar = CoreUtils::Convert(mNetworkSelectMsg);
                IAlertDialog::Probe(dialog)->SetMessage(cchar);
                IDialog::Probe(dialog)->SetCancelable(FALSE);
                dialog->SetIndeterminate(TRUE);
                break;
            }
            case DIALOG_NETWORK_AUTO_SELECT:
            {
                AutoPtr<IResources> resources;
                GetResources((IResources**)&resources);
                String str;
                resources->GetString(Elastos::Droid::TeleService::R::string::register_automatically, &str);
                AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
                IAlertDialog::Probe(dialog)->SetMessage(cchar);
                IDialog::Probe(dialog)->SetCancelable(FALSE);
                dialog->SetIndeterminate(TRUE);
                break;
            }
            case DIALOG_NETWORK_LIST_LOAD:
            default:
            {
                // reinstate the cancelablity of the dialog.
                AutoPtr<IResources> resources;
                GetResources((IResources**)&resources);
                String str;
                resources->GetString(Elastos::Droid::TeleService::R::string::load_networks_progress, &str);
                AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
                IAlertDialog::Probe(dialog)->SetMessage(cchar);
                IDialog::Probe(dialog)->SetCanceledOnTouchOutside(FALSE);
                IDialog::Probe(dialog)->SetOnCancelListener(this);
                break;
            }
        }
        return IDialog::Probe(dialog);
    }
    return NULL;
}

void CNetworkSetting::OnPrepareDialog(
    /* [in] */ Int32 id,
    /* [in] */ IDialog* dialog)
{
    if ((id == DIALOG_NETWORK_SELECTION) || (id == DIALOG_NETWORK_LIST_LOAD) ||
            (id == DIALOG_NETWORK_AUTO_SELECT)) {
        // when the dialogs come up, we'll need to indicate that
        // we're in a busy state to dissallow further input.
        AutoPtr<IPreferenceScreen> screen;
        GetPreferenceScreen((IPreferenceScreen**)&screen);
        IPreference::Probe(screen)->SetEnabled(FALSE);
    }
}

void CNetworkSetting::DisplayEmptyNetworkList(
        /* [in] */ Boolean flag)
{
    IPreference::Probe(mNetworkList)->SetTitle(flag ? Elastos::Droid::TeleService::R::string::empty_networks_list :
            Elastos::Droid::TeleService::R::string::label_available);
}

void CNetworkSetting::DisplayNetworkSeletionInProgress(
    /* [in] */ const String& networkStr)
{
    // TODO: use notification manager?
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);

    AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(networkStr);
    array->Set(0, TO_IINTERFACE(cchar));
    resources->GetString(Elastos::Droid::TeleService::R::string::register_on_network,
            array, &mNetworkSelectMsg);

    if (mIsForeground) {
        ShowDialog(DIALOG_NETWORK_SELECTION);
    }
}

void CNetworkSetting::DisplayNetworkQueryFailed(
    /* [in] */ Int32 error)
{
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    String status;
    resources->GetString(Elastos::Droid::TeleService::R::string::network_query_error, &status);

    AutoPtr<PhoneGlobals> app;
    PhoneGlobals::GetInstance((PhoneGlobals**)&app);
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(status);
    app->mNotificationMgr->PostTransientNotification(
            NotificationMgr::NETWORK_SELECTION_NOTIFICATION, cchar);
}

void CNetworkSetting::DisplayNetworkSelectionFailed(
    /* [in] */ IThrowable* ex)
{
    String status;

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    assert(0);
    // if ((ex != NULL && ICommandException::Probe(ec) != NULL) &&
    //         ((CommandException)ex).getCommandError()
    //           == CommandException.Error.ILLEGAL_SIM_OR_ME) {
    //     resources->GetString(Elastos::Droid::TeleService::R::string::not_allowed, &status);
    // }
    // else {
    //     resources->GetString(Elastos::Droid::TeleService::R::string::connect_later, &status);
    // }

    AutoPtr<PhoneGlobals> app;
    PhoneGlobals::GetInstance((PhoneGlobals**)&app);
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(status);
    app->mNotificationMgr->PostTransientNotification(
            NotificationMgr::NETWORK_SELECTION_NOTIFICATION, cchar);
}

void CNetworkSetting::DisplayNetworkSelectionSucceeded()
{
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    String status;
    resources->GetString(Elastos::Droid::TeleService::R::string::registration_done, &status);

    AutoPtr<PhoneGlobals> app;
    PhoneGlobals::GetInstance((PhoneGlobals**)&app);
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(status);
    app->mNotificationMgr->PostTransientNotification(
            NotificationMgr::NETWORK_SELECTION_NOTIFICATION, cchar);

    AutoPtr<IRunnable> r = new MyRunnable(this);
    Boolean res;
    mHandler->PostDelayed(r, 3000, &res);
}

void CNetworkSetting::LoadNetworksList()
{
    if (DBG) Log(String("load networks list..."));

    if (mIsForeground) {
        ShowDialog(DIALOG_NETWORK_LIST_LOAD);
    }

    // delegate query request to the service.
    //try {
    mNetworkQueryService->StartNetworkQuery(mCallback);
    //} catch (RemoteException e) {
    //}

    DisplayEmptyNetworkList(FALSE);
}

void CNetworkSetting::NetworksListLoaded(
    /* [in] */ IList* result,
    /* [in] */ Int32 status)
{
    if (DBG) Log(String("networks list loaded"));

    // update the state of the preferences.
    if (DBG) Log(String("hideProgressPanel"));


    // Always try to dismiss the dialog because activity may
    // be moved to background after dialog is shown.
    //try {
    ECode ec = DismissDialog(DIALOG_NETWORK_LIST_LOAD);
    //} catch (IllegalArgumentException e) {
    if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
        // It's not a error in following scenario, we just ignore it.
        // "Load list" dialog will not show, if NetworkQueryService is
        // connected after this activity is moved to background.
        if (DBG) Log(String("Fail to dismiss network load list dialog"));
    }
    //}

    AutoPtr<IPreferenceScreen> screen;
    GetPreferenceScreen((IPreferenceScreen**)&screen);
    IPreference::Probe(screen)->SetEnabled(TRUE);
    ClearList();

    if (status != INetworkQueryService::QUERY_OK) {
        if (DBG) Log(String("error while querying available networks"));
        DisplayNetworkQueryFailed(status);
        DisplayEmptyNetworkList(TRUE);
    }
    else {
        if (result != NULL){
            DisplayEmptyNetworkList(FALSE);

            // create a preference for each item in the list.
            // just use the operator name instead of the mildly
            // confusing mcc/mnc.
            Int32 size;
            result->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                result->Get(i, (IInterface**)&obj);
                AutoPtr<IOperatorInfo> ni = IOperatorInfo::Probe(obj);

                AutoPtr<IPreference> carrier;
                CPreference::New(this, NULL, (IPreference**)&carrier);
                AutoPtr<ICharSequence> cchar = CoreUtils::Convert(GetNetworkTitle(ni));
                carrier->SetTitle(cchar);
                carrier->SetPersistent(FALSE);
                Boolean res;
                mNetworkList->AddPreference(carrier, &res);
                mNetworkMap->Put(TO_IINTERFACE(carrier), TO_IINTERFACE(ni));

                StringBuilder sb;
                sb += "  ";
                sb += TO_CSTR(ni);
                if (DBG) Log(sb.ToString());
            }
        }
        else {
            DisplayEmptyNetworkList(TRUE);
        }
    }
}

String CNetworkSetting::GetNetworkTitle(
    /* [in] */ IOperatorInfo* ni)
{
    String str(NULL);
    if (!TextUtils::IsEmpty((ni->GetOperatorAlphaLong(&str), str))) {
        return str;
    }
    else if (!TextUtils::IsEmpty((ni->GetOperatorAlphaShort(&str), str))) {
        return str;
    }
    else {
        ni->GetOperatorNumeric(&str);
        return str;
    }
    return str;
}

void CNetworkSetting::ClearList()
{
    AutoPtr<ISet> keySet;
    mNetworkMap->GetKeySet((ISet**)&keySet);
    AutoPtr<IIterator> fit;
    keySet->GetIterator((IIterator**)&fit);

    Boolean hasNext;
    while (fit->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> ko;
        fit->GetNext((IInterface**)&ko);
        AutoPtr<IPreference> p = IPreference::Probe(ko);
        Boolean res;
        mNetworkList->RemovePreference(p, &res);
    }

    mNetworkMap->Clear();
}

void CNetworkSetting::SelectNetworkAutomatic()
{
    if (DBG) Log(String("select network automatically..."));
    if (mIsForeground) {
        ShowDialog(DIALOG_NETWORK_AUTO_SELECT);
    }

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_AUTO_SELECT_DONE, (IMessage**)&msg);
    mPhone->SetNetworkSelectionModeAutomatic(msg);
}

void CNetworkSetting::Log(
    /* [in] */ const String& msg)
{
    StringBuilder sb;
    sb += "[NetworksList] ";
    sb += msg;
    Logger::D(TAG, sb.ToString());
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos