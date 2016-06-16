
#include "elastos/droid/services/telephony/TelecomAccountRegistry.h"

namespace Elastos {
namespace Droid {
namespace Services {
namespace Telephony {

TelecomAccountRegistry::AccountEntry::AccountEntry(
    /* [in] */ IPhone* phone,
    /* [in] */ Boolean isEmergency,
    /* [in] */ Boolean isDummy)
    : mPhone(phone)
{
    mAccount = RegisterPstnPhoneAccount(isEmergency, isDummy);
    AutoPtr<IPhoneAccountHandle> handle;
    mAccount->GetAccountHandle((IPhoneAccountHandle**)&handle);
    Logger::D("TelecomAccountRegistry", "Registered phoneAccount: %s with handle: %s",
            TO_CSTR(mAccount), TO_CSTR(handle));
    mIncomingCallNotifier = new PstnIncomingCallNotifier(IPhoneProxy::Probe(mPhone));
}

ECode TelecomAccountRegistry::AccountEntry::Teardown()
{
    mIncomingCallNotifier->Teardown();
}

ECode TelecomAccountRegistry::AccountEntry::GetPhoneAccountHandle(
    /* [out] */ IPhoneAccountHandle** handle)
{
    VALIDATE_NOT_NULL(handle)

    if (mAccount != NULL) {
        mAccount->GetAccountHandle(handle);
    }
    else {
        *handle = NULL;
    }
    return NOERROR;
}

AutoPtr<IPhoneAccount> TelecomAccountRegistry::AccountEntry::RegisterPstnPhoneAccount(
    /* [in] */ Boolean isEmergency,
    /* [in] */ Boolean isDummyAccount)
{
    String dummyPrefix = isDummyAccount ? String("Dummy ") : String("");

    // Build the Phone account handle.
    AutoPtr<IPhoneAccountHandle> phoneAccountHandle =
            MakePstnPhoneAccountHandleWithPrefix(mPhone, dummyPrefix, isEmergency);

    // Populate the phone account data.
    Int64 subId;
    mPhone->GetSubId(&subId);
    Int32 slotId = ISubscriptionManager::INVALID_SLOT_ID;
    String line1Number;
    mTelephonyManager->GetLine1NumberForSubscriber(subId, &line1Number);
    if (line1Number.IsNull()) {
        line1Number = String("");
    }

    AutoPtr<IPhoneSubInfo> info;
    mPhone->GetPhoneSubInfo((IPhoneSubInfo**)&info);
    String subNumber;
    info->GetLine1Number(&subNumber);
    if (subNumber.IsNull()) {
        subNumber = String("");
    }

    String label;
    String description;
    Int32 count;
    if (isEmergency) {
        AutoPtr<IResources> resources;
        mContext->GetResources((IResources**)&resources);
        resources->GetString(R.string.sim_label_emergency_calls, &label);

        resources->GetString(R.string.sim_description_emergency_calls, &description);
    }
    else if ((mTelephonyManager->GetPhoneCount(&count), count) == 1) {
        // For single-SIM devices, we show the label and description as whatever the name of
        // the network is.
        mTelephonyManager->GetNetworkOperatorName(&label);
        description = label;
    } else {
        String subDisplayName;
        // We can only get the real slotId from the SubInfoRecord, we can't calculate the
        // slotId from the subId or the phoneId in all instances.
        AutoPtr<ISubInfoRecord> record = SubscriptionManager::GetSubInfoForSubscriber(subId);
        if (record != NULL) {
            subDisplayName = record.displayName;
            slotId = record.slotId;
        }

        String slotIdString;
        if (SubscriptionManager::IsValidSlotId(slotId)) {
            slotIdString = StringUtils::ToString(slotId);
        }
        else {
            AutoPtr<IResources> resources;
            mContext->GetResources((IResources**)&resources);
            resources->GetString(R.string.unknown, &slotIdString);
        }

        if (TextUtils::IsEmpty(subDisplayName)) {
            // Either the sub record is not there or it has an empty display name.
            Logger::W("TelecomAccountRegistry", "Could not get a display name for subid: %d", subId);
            AutoPtr<IResources> resources;
            mContext->GetResources((IResources**)&resources);
            resources->GetString(R.string.sim_description_default, slotIdString, &subDisplayName);
        }

        // The label is user-visible so let's use the display name that the user may
        // have set in Settings->Sim cards.
        label = dummyPrefix + subDisplayName;

        AutoPtr<IResources> resources;
        mContext->GetResources((IResources**)&resources);
        String tmp;
        resources->GetString(R.string.sim_description_default, slotIdString, &tmp);
        description = dummyPrefix + tmp;
    }

    // By default all SIM phone accounts can place emergency calls.
    Int32 capabilities = IPhoneAccount::CAPABILITY_SIM_SUBSCRIPTION |
            IPhoneAccount::CAPABILITY_CALL_PROVIDER |
            IPhoneAccount::CAPABILITY_PLACE_EMERGENCY_CALLS;

    AutoPtr<IPhoneAccountbuilder> builder;
    CPhoneAccountbuilder::New(phoneAccountHandle, label, (IPhoneAccountbuilder**)&builder);

    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> address;
    helper->FromParts(IPhoneAccount::SCHEME_TEL, line1Number, NULL, (IUri**)&address);
    builder->SetAddress(address);

    AutoPtr<IUri> address2;
    helper->FromParts(IPhoneAccount::SCHEME_TEL, subNumber, NULL, (IUri**)&address2);
    builder->SetSubscriptionAddress(address2);

    builder->SetCapabilities(capabilities);
    builder->SetIconResId(getPhoneAccountIcon(slotId));
    builder->SetShortDescription(description);

    builder->SetSupportedUriSchemes(Arrays.asList(PhoneAccount.SCHEME_TEL, PhoneAccount.SCHEME_VOICEMAIL))

    AutoPtr<IPhoneAccount> account;
    builder->Build((IPhoneAccount**)&account);

    // Register with Telecom and put into the account entry.
    mTelecomManager->RegisterPhoneAccount(account);
    return account;
}

ECode TelecomAccountRegistry::MyPhoneStateListener::OnServiceStateChanged(
    /* [in] */ IServiceState* serviceState)
{
    Int32 newState;
    serviceState->GetState(&newState);
    if (newState == IServiceState::STATE_IN_SERVICE && mServiceState != newState) {
        TearDownAccounts();
        SetupAccounts();
    }
    mServiceState = newState;
    return NOERROR;
}

ECode TelecomAccountRegistry::MyBroadcastReceiver::constructor(
    /* [in] */ ITelecomAccountRegistry* host)
{
    mHost = (TelecomAccountRegistry*)host;
    return BroadcastReceiver::constructor();
}

ECode TelecomAccountRegistry::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Boolean rebuildAccounts = FALSE;
    String action;
    intent->GetAction(&action);
    if (ITelephonyIntents::ACTION_SUBINFO_RECORD_UPDATED.Equals(action)) {
        Int32 status;
        intent->GetIntExtra(
                ISubscriptionManager::INTENT_KEY_DETECT_STATUS,
                ISubscriptionManager::EXTRA_VALUE_NOCHANGE, &status);
        Logger::I("TelecomAccountRegistry", "SUBINFO_RECORD_UPDATED : %d.", status);
        // Anytime the SIM state changes...rerun the setup
        // We rely on this notification even when the status is EXTRA_VALUE_NOCHANGE,
        // so we explicitly do not check for that here.
        rebuildAccounts = TRUE;
    }
    else if (ITelephonyIntents::ACTION_SUBINFO_CONTENT_CHANGE.Equals(action)) {
        String columnName;
        intent->GetStringExtra(ITelephonyIntents::EXTRA_COLUMN_NAME, &columnName);
        String stringContent;
        intent->GetStringExtra(ITelephonyIntents::EXTRA_STRING_CONTENT, &stringContent);
        Logger::V("TelecomAccountRegistry", "SUBINFO_CONTENT_CHANGE: Column: %s Content: %s",
                columnName.tostring(), stringContent.tostring());
        rebuildAccounts = TRUE;
    }
    if (rebuildAccounts) {
        TearDownAccounts();
        SetupAccounts();
    }
    return NOERROR;
}

const Boolean TelecomAccountRegistry::DBG = FALSE; /* STOP SHIP if true */

static AutoPtr<ArrayOf<Int32> > initsPhoneAccountIcons()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);

    (*array)[0] = R.drawable.ic_multi_sim1;
    (*array)[1] = R.drawable.ic_multi_sim2;
    (*array)[2] = R.drawable.ic_multi_sim3;
    (*array)[3] = R.drawable.ic_multi_sim4;
    return array;
}

const AutoPtr<ArrayOf<Int32> > TelecomAccountRegistry::sPhoneAccountIcons = initsPhoneAccountIcons();

// This icon is the one that is used when the Slot ID that we have for a particular SIM
// is not supported, i.e. SubscriptionManager.INVALID_SLOT_ID or the 5th SIM in a phone.
const Int32 TelecomAccountRegistry::defaultPhoneAccountIcon = R.drawable.ic_multi_sim;

AutoPtr<ITelecomAccountRegistry> TelecomAccountRegistry::sInstance;

CAR_INTERFACE_IMPL(TelecomAccountRegistry, Object, ITelecomAccountRegistry)

TelecomAccountRegistry::TelecomAccountRegistry(
    /* [in] */ IContext* context)
    : mContext(context)
    , mServiceState(IServiceState_STATE_POWER_OFF)
{
    CTelecomAccountRegistryBroadcastReceiver::New((IBroadcastReceiver**)&mReceiver);

    AutoPtr<IPhoneStateListener> mPhoneStateListener = new MyPhoneStateListener(this);

    CLinkedList::New((IList**)&mAccounts);

    AutoPtr<ITelecomManagerHelper> helper;
    CTelecomManagerHelper::AcquireSingleton((ITelecomManagerHelper**)&helper);
    helper->From(context, (ITelecomManager**)&mTelecomManager);

    AutoPtr<ITelephonyManagerHelper> helper2;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&helper);
    helper->From(context, (ITelephonyManager**)&mTelephonyManager);
}

AutoPtr<TelecomAccountRegistry> TelecomAccountRegistry::GetInstance(
    /* [in] */ IContext* context)
{
    {
        AutoLock syncLock(this);
        if (sInstance == NULL) {
            sInstance = new TelecomAccountRegistry(context);
        }
        return sInstance;
    }
}

ECode TelecomAccountRegistry::SetupOnBoot()
{
    // We need to register for both types of intents if we want to see added/removed Subs
    // along with changes to a given Sub.
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New((IIntentFilter**)&intentFilter);
    intentFilter->AddAction(ITelephonyIntents::ACTION_SUBINFO_RECORD_UPDATED);
    intentFilter->AddAction(ITelephonyIntents::ACTION_SUBINFO_CONTENT_CHANGE);
    mContext->RegisterReceiver(mReceiver, intentFilter);

    // We also need to listen for changes to the service state (e.g. emergency -> in service)
    // because this could signal a removal or addition of a SIM in a single SIM phone.
    return mTelephonyManager->Listen(mPhoneStateListener, IPhoneStateListener::LISTEN_SERVICE_STATE);
}

AutoPtr<IPhoneAccountHandle> TelecomAccountRegistry::MakePstnPhoneAccountHandle(
    /* [in] */ IPhone* phone)
{
    return MakePstnPhoneAccountHandleWithPrefix(phone, Srting(""), FALSE);
}

AutoPtr<IPhoneAccountHandle> TelecomAccountRegistry::MakePstnPhoneAccountHandleWithPrefix(
    /* [in] */ IPhone* phone,
    /* [in] */ const String& prefix,
    /* [in] */ Boolean isEmergency)
{
    AutoPtr<IComponentName> pstnConnectionServiceName;
    AutoPtr<IContext> context;
    phone->GetContext((IContext**)&context);
    CComponentName::New(context, TelephonyConnectionService.class);
    // TODO: Should use some sort of special hidden flag to decorate this account as
    // an emergency-only account
    String id;
    if (isEmergency) {
        id = String("E");
    }
    else {
        StringBuilder sb;
        sb += prefix;
        Int32 _id;
        phone->GetSubId(&_id);
        sb += _id;
        sb.ToString(&id);
    }

    AutoPtr<IPhoneAccountHandle> handle = new PhoneAccountHandle(pstnConnectionServiceName, id);
    return handle;
}

Boolean TelecomAccountRegistry::HasAccountEntryForPhoneAccount(
    /* [in] */ IPhoneAccountHandle* handle)
{
    Int32 size;
    mAccounts->GetSise(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mAccounts->Get(i, (IInterface**)&obj);
        AutoPtr<IAccountEntry> entry = IAccountEntry::Probe(obj);

        AutoPtr<IPhoneAccountHandle> _handle;
        entry->GetPhoneAccountHandle((IPhoneAccountHandle**)&_handle);
        Boolean res;
        if (IObject::Probe(_handle)>Equals(handle, &res), res) {
            return TRUE;
        }
    }
    return FALSE;
}

void TelecomAccountRegistry::CleanupPhoneAccounts()
{
    AutoPtr<IComponentName> telephonyComponentName;
    CComponentName::New(mContext, TelephonyConnectionService.class, (IComponentName**)&telephonyComponentName);
    AutoPtr<IList> accountHandles;
    mTelecomManager->GetAllPhoneAccountHandles((IList**)&accountHandles);

    Int32 size;
    accountHandles->GetSise(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        accountHandles->Get(i, (IInterface**)&obj);
        AutoPtr<IPhoneAccountHandle> handle = IPhoneAccountHandle::Probe(obj);

        AutoPtr<IComponentName> name;
        handle->GetComponentName((IComponentName**)&name);
        Boolean res;
        if ((IObject::Probe(telephonyComponentName)->Equals(name, &res), res) &&
                !HasAccountEntryForPhoneAccount(handle)) {
            Logger::D("TelecomAccountRegistry", "Unregistering phone account %s.", TO_CSTR(handle));
            mTelecomManager->UnregisterPhoneAccount(handle);
        }
    }
}

void TelecomAccountRegistry::SetupAccounts()
{
    // Go through SIM-based phones and register ourselves -- registering an existing account
    // will cause the existing entry to be replaced.
    AutoPtr<ArrayOf<IPhone> > phones;
    PhoneFactory::GetPhones((ArrayOf<IPhone>**)&phones);
    Logger::D("TelecomAccountRegistry", "Found %d phones.  Attempting to register.", phones->GetLength());

    for (Int32 i = 0; i < phones->GetLength(); i++) {
        AutoPtr<IPhone> phone = (*phones)[i];

        Int64 subscriptionId;
        phone->GetSubId(&subscriptionId);
        Logger::D("TelecomAccountRegistry", "Phone with subscription id %d", subscriptionId);
        if (subscriptionId >= 0) {
            AutoPtr<IAccountEntry> entry = new AccountEntry(phone, FALSE /* emergency */, FALSE /* isDummy */)
            mAccounts->Add(TO_IINTERFACE(entry));
        }

    }

    // If we did not list ANY accounts, we need to provide a "default" SIM account
    // for emergency numbers since no actual SIM is needed for dialing emergency
    // numbers but a phone account is.
    Boolean res;
    if (mAccounts->IsEmpty(&res), res) {
        AutoPtr<IPhone> phone;
        PhoneFactory::GetDefaultPhone((IPhone**)&phone);
        AutoPtr<IAccountEntry> entry = new AccountEntry(phone, TRUE /* emergency */,
                FALSE /* isDummy */)
        mAccounts->Add(TO_IINTERFACE(entry));
    }

    // Add a fake account entry.
    if (DBG && phones->GetLength() > 0 && String("TRUE").Equals(System.getProperty("dummy_sim"))) {
        AutoPtr<IAccountEntry> entry = new AccountEntry((*phones)[0], FALSE /* emergency */, TRUE /* isDummy */);
        mAccounts->Add(TO_IINTERFACE(entry));
    }

    // Clean up any PhoneAccounts that are no longer relevant
    CleanupPhoneAccounts();
}

Int32 TelecomAccountRegistry::GetPhoneAccountIcon(
    /* [in] */ Int32 index)
{
    // A valid slot id doesn't necessarily mean that we have an icon for it.
    Boolean res;
    if (SubscriptionManager::IsValidSlotId(index) &&
            index < TelecomAccountRegistry.phoneAccountIcons.length) {
        return TelecomAccountRegistry.phoneAccountIcons[index];
    }
    // Invalid indices get the default icon that has no number associated with it.
    return defaultPhoneAccountIcon;
}

void TelecomAccountRegistry::TearDownAccounts()
{
    Int32 size;
    mAccounts->GetSise(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mAccounts->Get(i, (IInterface**)&obj);
        AutoPtr<IAccountEntry> entry = IAccountEntry::Probe(obj);

        entry->Teardown();

    }
    mAccounts->Clear();
}

} // namespace Telephony
} // namespace Services
} // namespace Droid
} // namespace Elastos