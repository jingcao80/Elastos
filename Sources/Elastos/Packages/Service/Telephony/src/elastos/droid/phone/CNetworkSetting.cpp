
#include "elastos/droid/phone/CNetworkSetting.h"

namespace Elastos {
namespace Droid {
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
    AutoPtr<IAsyncResult> ar;

    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case EVENT_NETWORK_SCAN_COMPLETED:
            NetworksListLoaded ((List<OperatorInfo>) msg.obj, msg.arg1);
            break;

        case EVENT_NETWORK_SELECTION_DONE:
        {
            if (DBG) mHost->Log(String("hideProgressPanel"));
            RemoveDialog(DIALOG_NETWORK_SELECTION);

            AutoPtr<IPreferenceScreen> screen;
            GetPreferenceScreen((IPreferenceScreen**)&screen);
            screen->SetEnabled(TRUE);

            ar = (AsyncResult) msg.obj;
            if (ar.exception != null) {
                if (DBG) mHost->Log(String("manual network selection: failed!"));
                DisplayNetworkSelectionFailed(ar.exception);
            }
            else {
                if (DBG) mHost->Log(String("manual network selection: succeeded!"));
                DisplayNetworkSelectionSucceeded();
            }
            break;
        }
        case EVENT_AUTO_SELECT_DONE:
        {
            if (DBG) mHost->Log(String("hideProgressPanel"));

            // Always try to dismiss the dialog because activity may
            // be moved to background after dialog is shown.
            //try {
            ECode ec = DismissDialog(DIALOG_NETWORK_AUTO_SELECT);
            //} catch (IllegalArgumentException e) {
            if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
            //    // "auto select" is always trigged in foreground, so "auto select" dialog
                //  should be shown when "auto select" is trigged. Should NOT get
                // this exception, and Log it.
                Logger::W(LOG_TAG, "[NetworksList] Fail to dismiss auto select dialog %d", ec);
            }
            //}
            AutoPtr<IPreferenceScreen> screen;
            GetPreferenceScreen((IPreferenceScreen**)&screen);
            screen->SetEnabled(TRUE);

            ar = (AsyncResult) msg.obj;
            if (ar.exception != null) {
                if (DBG) mHost->Log(String("automatic network selection: failed!"));
                DisplayNetworkSelectionFailed(ar.exception);
            }
            else {
                if (DBG) mHost->Log(String("automatic network selection: succeeded!"));
                DisplayNetworkSelectionSucceeded();
            }
            break;
        }
    }

    return NOERROR;
}

ECode CNetworkSetting::MyServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* className,
    /* [in] */ IBinder* service)
{
    if (DBG) mHost->Log(String("connection created, binding local service."));
    INetworkQueryServiceLocalBinder::Probe(service)->GetService(mHost->mNetworkQueryService);
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

const String CNetworkSetting::LOG_TAG("phone");
const Boolean CNetworkSetting::DBG = FALSE;

const Int32 CNetworkSetting::EVENT_NETWORK_SCAN_COMPLETED = 100;
const Int32 CNetworkSetting::EVENT_NETWORK_SELECTION_DONE = 200;
const Int32 CNetworkSetting::EVENT_AUTO_SELECT_DONE = 300;

const Int32 CNetworkSetting::DIALOG_NETWORK_SELECTION = 100;
const Int32 CNetworkSetting::DIALOG_NETWORK_LIST_LOAD = 200;
const Int32 CNetworkSetting::DIALOG_NETWORK_AUTO_SELECT = 300;

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
        Logger::E(LOG_TAG, "RuntimeException");
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
        SetContentView(R.layout.telephony_disallowed_preference_screen);
        mUnavailable = TRUE;
        return NOERROR;
    }

    AddPreferencesFromResource(R.xml.carrier_select);

    mPhone = PhoneGlobals::GetPhone((IPhone**)&mPhone);

    AutoPtr<IPreferenceScreen> screen;
    GetPreferenceScreen((IPreferenceScreen**)&screen);
    AutoPtr<IPreference> preference;
    screen->FindPreference(LIST_NETWORKS_KEY, (IPreference**)&preference);

    mNetworkList = IPreferenceGroup::Probe(preference);
    CHashMap::New((HashMap**)&mNetworkMap);

    screen->FindPreference(BUTTON_SRCH_NETWRKS_KEY, (IPreference**)&mSearchButton);
    screen->FindPreference(BUTTON_AUTO_SELECT_KEY, (IPreference**)&mAutoSelect);

    // Start the Network Query service, and bind it.
    // The OS knows to start he service only once and keep the instance around (so
    // long as startService is called) until a stopservice request is made.  Since
    // we want this service to just stay in the background until it is killed, we
    // don't bother stopping it from our end.
    AutoPtr<IIntent> intent;
    CIntent::New(this, NetworkQueryService.class, (IIntent**)&intent);
    StartService (intent);

    AutoPtr<IIntent> intent2;
    CIntent::New(this, NetworkQueryService.class, (IIntent**)&intent2):
    return BindService (intent2, mNetworkQueryServiceConnection, IContext::BIND_AUTO_CREATE);
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

ECode CNetworkSetting::OnCreateDialog(
    /* [in] */ Int32 id,
    /* [out] */ IDialog** outdialog)
{
    VALIDATE_NOT_NULL(outdialog)

    if ((id == DIALOG_NETWORK_SELECTION) || (id == DIALOG_NETWORK_LIST_LOAD) ||
            (id == DIALOG_NETWORK_AUTO_SELECT)) {
        AutoPtr<IProgressDialog> dialog;
        CProgressDialog::New(this, (IProgressDialog**)&dialog);
        switch (id) {
            case DIALOG_NETWORK_SELECTION:
                // It would be more efficient to reuse this dialog by moving
                // this setMessage() into onPreparedDialog() and NOT use
                // removeDialog().  However, this is not possible since the
                // message is rendered only 2 times in the ProgressDialog -
                // after show() and before onCreate.
                dialog->SetMessage(mNetworkSelectMsg);
                dialog->SetCancelable(FALSE);
                dialog->SetIndeterminate(TRUE);
                break;
            case DIALOG_NETWORK_AUTO_SELECT:
            {
                AutoPtr<IResources> resources;
                GetResources((IResources**)&resources);
                String str;
                resources->GetString(R.string.register_automatically, &str);
                dialog->SetMessage(str);
                dialog->SetCancelable(FALSE);
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
                resources->GetString(R.string.load_networks_progress, &str);
                dialog->SetMessage(str);
                dialog->SetCanceledOnTouchOutside(FALSE);
                dialog->SetOnCancelListener(this);
                break;
            }
        }
        *outdialog = dialog;
        REFCOUNT_ADD(*outdialog)
        return NOERROR;
    }
    *outdialog = NULL;
    return NOERROR;
}

ECode CNetworkSetting::OnPrepareDialog(
    /* [in] */ Int32 id,
    /* [in] */ IDialog* dialog)
{
    if ((id == DIALOG_NETWORK_SELECTION) || (id == DIALOG_NETWORK_LIST_LOAD) ||
            (id == DIALOG_NETWORK_AUTO_SELECT)) {
        // when the dialogs come up, we'll need to indicate that
        // we're in a busy state to dissallow further input.
        AutoPtr<IPreferenceScreen> screen;
        GetPreferenceScreen((IPreferenceScreen**)&screen);
        screen->SetEnabled(FALSE);
    }
    return NOERROR;
}

void CNetworkSetting::DisplayEmptyNetworkList(
        /* [in] */ Boolean flag)
{
    mNetworkList->SetTitle(flag ? R.string.empty_networks_list : R.string.label_available);
}

void CNetworkSetting::DisplayNetworkSeletionInProgress(
    /* [in] */ const String& networkStr)
{
    // TODO: use notification manager?
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    resources->GetString(R.string.register_on_network, networkStr, &mNetworkSelectMsg);

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
    resources->GetString(R.string.network_query_error, &status);

    AutoPtr<IPhoneGlobals> app = PhoneGlobals::GetInstance();
    app->mNotificationMgr->PostTransientNotification(
            INotificationMgr::NETWORK_SELECTION_NOTIFICATION, status);
}

void CNetworkSetting::DisplayNetworkSelectionFailed(
    /* [in] */ IThrowable* ex)
{
    String status;

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    if ((ex != NULL && ex instanceof CommandException) &&
            ((CommandException)ex).getCommandError()
              == CommandException.Error.ILLEGAL_SIM_OR_ME) {
        resources->GetString(R.string.not_allowed, &status);
    }
    else {
        resources->GetString(R.string.connect_later, &status);
    }

    AutoPtr<IPhoneGlobals> app = PhoneGlobals::GetInstance();
    app->mNotificationMgr->PostTransientNotification(
            INotificationMgr::NETWORK_SELECTION_NOTIFICATION, status);
}

void CNetworkSetting::DisplayNetworkSelectionSucceeded()
{
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    String status;
    resources->GetString(R.string.registration_done, &status);

    AutoPtr<IPhoneGlobals> app = PhoneGlobals::GetInstance();
    app->mNotificationMgr->PostTransientNotification(
            INotificationMgr::NETWORK_SELECTION_NOTIFICATION, status);

    AutoPtr<IRunnable> r = new MyRunnable(this);
    mHandler->PostDelayed(r, 3000);
}

void CNetworkSetting::LoadNetworksList()
{
    if (DBG) Log("load networks list...");

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
    screen->SetEnabled(TRUE);
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
                carrier->SetTitle(GetNetworkTitle(ni));
                carrier->SetPersistent(FALSE);
                mNetworkList->AddPreference(carrier);
                mNetworkMap->Put(carrier, ni);

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
        AutoPtr<IPreference> p = IPreference::Probe(ko)
        mNetworkList->RemovePreference(p);
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
    Logger::D(LOG_TAG, sb.ToString());
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos