
#include "elastos/droid/teleservice/services/telephony/TelecomAccountRegistry.h"
#include "elastos/droid/teleservice/services/telephony/PstnIncomingCallNotifier.h"
#include "elastos/droid/teleservice/services/telephony/CTelecomAccountRegistryBroadcastReceiver.h"
#include "elastos/droid/text/TextUtils.h"
#include "Elastos.Droid.Net.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include "Elastos.CoreLibrary.Core.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::IPhoneFactory;
using Elastos::Droid::Internal::Telephony::CPhoneFactory;
using Elastos::Droid::Internal::Telephony::IPhoneSubInfo;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::TeleService::R;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Telecom::CPhoneAccountHandle;
using Elastos::Droid::Telecom::IPhoneAccountHandle;
using Elastos::Droid::Telecom::CTelecomManagerHelper;
using Elastos::Droid::Telecom::ITelecomManagerHelper;
using Elastos::Droid::Telecom::CPhoneAccountBuilder;
using Elastos::Droid::Telecom::IPhoneAccountBuilder;
using Elastos::Droid::Telephony::ISubInfoRecord;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;
using Elastos::Droid::Telephony::CTelephonyManagerHelper;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::CSubscriptionManager;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::StringUtils;
using Elastos::Core::AutoLock;
using Elastos::Core::CoreUtils;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Arrays;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::ILinkedList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {

TelecomAccountRegistry::AccountEntry::AccountEntry(
    /* [in] */ TelecomAccountRegistry* host,
    /* [in] */ IPhone* phone,
    /* [in] */ Boolean isEmergency,
    /* [in] */ Boolean isDummy)
    : mHost(host)
    , mPhone(phone)
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
    return mIncomingCallNotifier->Teardown();
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
    mHost->mTelephonyManager->GetLine1NumberForSubscriber(subId, &line1Number);
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
        mHost->mContext->GetResources((IResources**)&resources);
        resources->GetString(R::string::sim_label_emergency_calls, &label);

        resources->GetString(R::string::sim_description_emergency_calls, &description);
    }
    else if ((mHost->mTelephonyManager->GetPhoneCount(&count), count) == 1) {
        // For single-SIM devices, we show the label and description as whatever the name of
        // the network is.
        mHost->mTelephonyManager->GetNetworkOperatorName(&label);
        description = label;
    }
    else {
        String subDisplayName;
        // We can only get the real slotId from the SubInfoRecord, we can't calculate the
        // slotId from the subId or the phoneId in all instances.
        AutoPtr<ISubscriptionManager> helper;
        CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&helper);
        AutoPtr<ISubInfoRecord> record;
        helper->GetSubInfoForSubscriber(subId, (ISubInfoRecord**)&record);
        if (record != NULL) {
            assert(0);
            // subDisplayName = record.displayName;
            // slotId = record.slotId;
        }

        String slotIdString;
        Boolean res;
        helper->IsValidSlotId(slotId, &res);
        if (helper->IsValidSlotId(slotId, &res), res) {
            slotIdString = StringUtils::ToString(slotId);
        }
        else {
            AutoPtr<IResources> resources;
            mHost->mContext->GetResources((IResources**)&resources);
            resources->GetString(R::string::unknown, &slotIdString);
        }

        if (TextUtils::IsEmpty(subDisplayName)) {
            // Either the sub record is not there or it has an empty display name.
            Logger::W("TelecomAccountRegistry", "Could not get a display name for subid: %d", subId);
            AutoPtr<IResources> resources;
            mHost->mContext->GetResources((IResources**)&resources);
            AutoPtr<ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(1);
            AutoPtr<ICharSequence> cchar = CoreUtils::Convert(slotIdString);
            formatArgs->Set(0, TO_IINTERFACE(cchar));
            resources->GetString(R::string::sim_description_default,
                    formatArgs, &subDisplayName);
        }

        // The label is user-visible so let's use the display name that the user may
        // have set in Settings->Sim cards.
        label = dummyPrefix + subDisplayName;

        AutoPtr<IResources> resources;
        mHost->mContext->GetResources((IResources**)&resources);
        AutoPtr<ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(1);
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(slotIdString);
        formatArgs->Set(0, TO_IINTERFACE(cchar));
        String tmp;
        resources->GetString(R::string::sim_description_default,
                formatArgs, &tmp);
        description = dummyPrefix + tmp;
    }

    // By default all SIM phone accounts can place emergency calls.
    Int32 capabilities = IPhoneAccount::CAPABILITY_SIM_SUBSCRIPTION |
            IPhoneAccount::CAPABILITY_CALL_PROVIDER |
            IPhoneAccount::CAPABILITY_PLACE_EMERGENCY_CALLS;

    AutoPtr<IPhoneAccountBuilder> builder;
    CPhoneAccountBuilder::New(phoneAccountHandle, CoreUtils::Convert(label),
            (IPhoneAccountBuilder**)&builder);

    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> address;
    helper->FromParts(IPhoneAccount::SCHEME_TEL, line1Number, String(NULL), (IUri**)&address);
    builder->SetAddress(address);

    AutoPtr<IUri> address2;
    helper->FromParts(IPhoneAccount::SCHEME_TEL, subNumber, String(NULL), (IUri**)&address2);
    builder->SetSubscriptionAddress(address2);

    builder->SetCapabilities(capabilities);
    builder->SetIconResId(mHost->GetPhoneAccountIcon(slotId));
    builder->SetShortDescription(CoreUtils::Convert(description));

    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(2);
    array->Set(0, IPhoneAccount::SCHEME_TEL);
    array->Set(1, IPhoneAccount::SCHEME_VOICEMAIL);
    AutoPtr<IList> list;
    Arrays::AsList(array, (IList**)&list);
    builder->SetSupportedUriSchemes(list);

    AutoPtr<IPhoneAccount> account;
    builder->Build((IPhoneAccount**)&account);

    // Register with Telecom and put into the account entry.
    mHost->mTelecomManager->RegisterPhoneAccount(account);
    return account;
}

ECode TelecomAccountRegistry::MyPhoneStateListener::OnServiceStateChanged(
    /* [in] */ IServiceState* serviceState)
{
    Int32 newState;
    serviceState->GetState(&newState);
    if (newState == IServiceState::STATE_IN_SERVICE && mHost->mServiceState != newState) {
        mHost->TearDownAccounts();
        mHost->SetupAccounts();
    }
    mHost->mServiceState = newState;
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
        intent->GetInt32Extra(
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
                columnName.string(), stringContent.string());
        rebuildAccounts = TRUE;
    }
    if (rebuildAccounts) {
        mHost->TearDownAccounts();
        mHost->SetupAccounts();
    }
    return NOERROR;
}

const Boolean TelecomAccountRegistry::DBG = FALSE; /* STOP SHIP if true */

static AutoPtr<ArrayOf<Int32> > initsPhoneAccountIcons()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);

    (*array)[0] = R::drawable::ic_multi_sim1;
    (*array)[1] = R::drawable::ic_multi_sim2;
    (*array)[2] = R::drawable::ic_multi_sim3;
    (*array)[3] = R::drawable::ic_multi_sim4;
    return array;
}

const AutoPtr<ArrayOf<Int32> > TelecomAccountRegistry::sPhoneAccountIcons = initsPhoneAccountIcons();

// This icon is the one that is used when the Slot ID that we have for a particular SIM
// is not supported, i.e. SubscriptionManager.INVALID_SLOT_ID or the 5th SIM in a phone.
const Int32 TelecomAccountRegistry::defaultPhoneAccountIcon =
        R::drawable::ic_multi_sim;

AutoPtr<TelecomAccountRegistry> TelecomAccountRegistry::sInstance;

Object TelecomAccountRegistry::sLock;

CAR_INTERFACE_IMPL(TelecomAccountRegistry, Object, ITelecomAccountRegistry)

TelecomAccountRegistry::TelecomAccountRegistry(
    /* [in] */ IContext* context)
    : mContext(context)
    , mServiceState(IServiceState::STATE_POWER_OFF)
{
    CTelecomAccountRegistryBroadcastReceiver::New(this, (IBroadcastReceiver**)&mReceiver);

    mPhoneStateListener = new MyPhoneStateListener(this);

    CLinkedList::New((IList**)&mAccounts);

    AutoPtr<ITelecomManagerHelper> helper;
    CTelecomManagerHelper::AcquireSingleton((ITelecomManagerHelper**)&helper);
    helper->From(context, (ITelecomManager**)&mTelecomManager);

    AutoPtr<ITelephonyManagerHelper> helper2;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&helper2);
    helper2->From(context, (ITelephonyManager**)&mTelephonyManager);
}

AutoPtr<TelecomAccountRegistry> TelecomAccountRegistry::GetInstance(
    /* [in] */ IContext* context)
{
    {
        AutoLock syncLock(sLock);
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

    AutoPtr<IIntent> tmp;
    mContext->RegisterReceiver(mReceiver, intentFilter, (IIntent**)&tmp);

    // We also need to listen for changes to the service state (e.g. emergency -> in service)
    // because this could signal a removal or addition of a SIM in a single SIM phone.
    return mTelephonyManager->Listen(mPhoneStateListener, IPhoneStateListener::LISTEN_SERVICE_STATE);
}

AutoPtr<IPhoneAccountHandle> TelecomAccountRegistry::MakePstnPhoneAccountHandle(
    /* [in] */ IPhone* phone)
{
    return MakePstnPhoneAccountHandleWithPrefix(phone, String(""), FALSE);
}

AutoPtr<IPhoneAccountHandle> TelecomAccountRegistry::MakePstnPhoneAccountHandleWithPrefix(
    /* [in] */ IPhone* phone,
    /* [in] */ const String& prefix,
    /* [in] */ Boolean isEmergency)
{
    AutoPtr<IComponentName> pstnConnectionServiceName;
    AutoPtr<IContext> context;
    phone->GetContext((IContext**)&context);
    //CComponentName::New(context, ECLSID_CTelephonyConnectionService, (IComponentName**)&pstnConnectionServiceName);
    CComponentName::New(context,
            String("Elastos.Droid.TeleService.Services.Telephony.CTelephonyConnectionService"),
            (IComponentName**)&pstnConnectionServiceName);
    // TODO: Should use some sort of special hidden flag to decorate this account as
    // an emergency-only account
    String id;
    if (isEmergency) {
        id = String("E");
    }
    else {
        StringBuilder sb;
        sb += prefix;
        Int64 _id;
        phone->GetSubId(&_id);
        sb += _id;
        sb.ToString(&id);
    }

    AutoPtr<IPhoneAccountHandle> handle;
    CPhoneAccountHandle::New(pstnConnectionServiceName, id, (IPhoneAccountHandle**)&handle);
    return handle;
}

Boolean TelecomAccountRegistry::HasAccountEntryForPhoneAccount(
    /* [in] */ IPhoneAccountHandle* handle)
{
    Int32 size;
    mAccounts->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mAccounts->Get(i, (IInterface**)&obj);
        AutoPtr<AccountEntry> entry = (AccountEntry*)IObject::Probe(obj);

        AutoPtr<IPhoneAccountHandle> _handle;
        entry->GetPhoneAccountHandle((IPhoneAccountHandle**)&_handle);
        Boolean res;
        if (IObject::Probe(_handle)->Equals(handle, &res), res) {
            return TRUE;
        }
    }
    return FALSE;
}

void TelecomAccountRegistry::CleanupPhoneAccounts()
{
    AutoPtr<IComponentName> telephonyComponentName;
    //CComponentName::New(mContext, ECLSID_CTelephonyConnectionService, (IComponentName**)&telephonyComponentName);
    CComponentName::New(mContext, String("Elastos.Droid.TeleService.Services.Telephony.CTelephonyConnectionService"),
            (IComponentName**)&telephonyComponentName);
    AutoPtr<IList> accountHandles;
    mTelecomManager->GetAllPhoneAccountHandles((IList**)&accountHandles);

    Int32 size;
    accountHandles->GetSize(&size);
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
    AutoPtr<IPhoneFactory> helper;
    CPhoneFactory::AcquireSingleton((IPhoneFactory**)&helper);
    AutoPtr<ArrayOf<IPhone*> > phones;
    helper->GetPhones((ArrayOf<IPhone*>**)&phones);
    Logger::D("TelecomAccountRegistry", "Found %d phones.  Attempting to register.", phones->GetLength());

    for (Int32 i = 0; i < phones->GetLength(); i++) {
        AutoPtr<IPhone> phone = (*phones)[i];

        Int64 subscriptionId;
        phone->GetSubId(&subscriptionId);
        Logger::D("TelecomAccountRegistry", "Phone with subscription id %d", subscriptionId);
        if (subscriptionId >= 0) {
            AutoPtr<AccountEntry> entry = new AccountEntry(this, phone, FALSE /* emergency */,
                    FALSE /* isDummy */);
            mAccounts->Add(TO_IINTERFACE(entry));
        }

    }

    // If we did not list ANY accounts, we need to provide a "default" SIM account
    // for emergency numbers since no actual SIM is needed for dialing emergency
    // numbers but a phone account is.
    Boolean res;
    if (mAccounts->IsEmpty(&res), res) {
        AutoPtr<IPhone> phone;
        helper->GetDefaultPhone((IPhone**)&phone);
        AutoPtr<AccountEntry> entry = new AccountEntry(this, phone, TRUE /* emergency */,
                FALSE /* isDummy */);
        mAccounts->Add(TO_IINTERFACE(entry));
    }

    // Add a fake account entry.
    AutoPtr<ISystem> helper2;
    CSystem::AcquireSingleton((ISystem**)&helper2);
    String value;
    helper2->GetProperty(String("dummy_sim"), &value);
    if (DBG && phones->GetLength() > 0 && value.Equals("TRUE")) {
        AutoPtr<AccountEntry> entry = new AccountEntry(this, (*phones)[0], FALSE /* emergency */,
                TRUE /* isDummy */);
        mAccounts->Add(TO_IINTERFACE(entry));
    }

    // Clean up any PhoneAccounts that are no longer relevant
    CleanupPhoneAccounts();
}

Int32 TelecomAccountRegistry::GetPhoneAccountIcon(
    /* [in] */ Int32 index)
{
    // A valid slot id doesn't necessarily mean that we have an icon for it.
    AutoPtr<ISubscriptionManager> helper;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&helper);
    Boolean res;
    if ((helper->IsValidSlotId(index, &res), res) &&
            index < sPhoneAccountIcons->GetLength()) {
        return (*sPhoneAccountIcons)[index];
    }
    // Invalid indices get the default icon that has no number associated with it.
    return defaultPhoneAccountIcon;
}

void TelecomAccountRegistry::TearDownAccounts()
{
    Int32 size;
    mAccounts->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mAccounts->Get(i, (IInterface**)&obj);
        AutoPtr<AccountEntry> entry = (AccountEntry*)IObject::Probe(obj);
        entry->Teardown();

    }
    mAccounts->Clear();
}

} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos
