
#include "elastos/droid/dialer/interactions/PhoneNumberInteraction.h"
#include "elastos/droid/dialer/contact/ContactUpdateService.h"
#include "elastos/droid/dialer/util/DialerUtils.h"
#include <elastos/droid/R.h>
#include "Elastos.Droid.Provider.h"
#include "Elastos.CoreLibrary.IO.h"
#include "R.h"

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::Content::CCursorLoader;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::EIID_ILoaderOnLoadCompleteListener;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::IContactsContractContactsData;
using Elastos::Droid::Provider::IContactsContractDataColumns;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsPhone;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsSipAddress;
using Elastos::Droid::Provider::IContactsContractRawContactsColumns;
using Elastos::Droid::Provider::IContactsContractSyncColumns;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsCommonColumns;
using Elastos::Droid::Provider::IContactsContractContacts;
using Elastos::Droid::Provider::CContactsContractContacts;
using Elastos::Droid::Provider::IContactsContractData;
using Elastos::Droid::Provider::CContactsContractData;
using Elastos::Droid::Widget::ICheckable;
using Elastos::IO::ICloseable;
using Elastos::Utility::CArrayList;
using Elastos::Apps::Dialer::Contact::ContactUpdateService;
using Elastos::Apps::Dialer::Util::DialerUtils;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Interactions {

//=================================================================
// PhoneNumberInteraction::PhoneItem
//=================================================================
// TODO:
CAR_INTERFACE_IMPL(PhoneNumberInteraction::PhoneItem, Object, IParcelable/*, ICollapserCollapsible*/);

PhoneNumberInteraction::PhoneItem::PhoneItem()
{}

ECode PhoneNumberInteraction::PhoneItem::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt64(mId);
    dest->WriteString(mPhoneNumber);
    dest->WriteString(mAccountType);
    dest->WriteString(mDataSet);
    dest->WriteInt64(mType);
    dest->WriteString(mLabel);
    dest->WriteString(mMimeType);

    return NOERROR;
}

ECode PhoneNumberInteraction::PhoneItem::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt64(&mId);
    source->ReadString(&mPhoneNumber);
    source->ReadString(&mAccountType);
    source->ReadString(&mDataSet);
    source->ReadInt64(&mType);
    source->ReadString(&mLabel);
    source->ReadString(&mMimeType);
    return NOERROR;
}

ECode PhoneNumberInteraction::PhoneItem::CollapseWith(
    /* [in] */ IInterface* phoneItem)
{
    // Just keep the number and id we already have.
    return NOERROR;
}

ECode PhoneNumberInteraction::PhoneItem::ShouldCollapseWith(
    /* [in] */ IInterface* phoneItem,
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    assert(0 && "TODO");
    // *result = MoreContactUtils::ShouldCollapse(
    //         IContactsContractCommonDataKindsPhone::CONTENT_ITEM_TYPE, mPhoneNumber,
    //         IContactsContractCommonDataKindsPhone::CONTENT_ITEM_TYPE,
    //         ((PhoneItem*)phoneItem)->mPhoneNumber);
    return NOERROR;
}

ECode PhoneNumberInteraction::PhoneItem::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = mPhoneNumber;
    return NOERROR;
}

//=================================================================
// PhoneNumberInteraction::PhoneItemAdapter
//=================================================================
ECode PhoneNumberInteraction::PhoneItemAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IList* list,
    /* [in] */ Int32 interactionType)
{
    ArrayAdapter::constructor(context,
            R::layout::phone_disambig_item, Elastos::Droid::R::id::text2, list);
    mInteractionType = interactionType;
    return NOERROR;
}

ECode PhoneNumberInteraction::PhoneItemAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IView> view;
    ArrayAdapter::GetView(position, convertView, parent, (IView**)&view);

    AutoPtr<IInterface> temp;
    GetItem(position, (IInterface**)&temp);
    PhoneItem* item = (PhoneItem*)IObject::Probe(temp);

    AutoPtr<IView> typeView;
    view->FindViewById(Elastos::Droid::R::id::text1, (IView**)&typeView);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    assert(0 && "TODO");
    // AutoPtr<ICharSequence> value = ContactDisplayUtils::GetLabelForCallOrSms(
    //         (Int32)item->mType, item->mLabel, mInteractionType, context);

    // ITextView::Probe(typeView)->SetText(value);
    *result = view;
    REFCOUNT_ADD(*result);

    return NOERROR;
}

//=================================================================
// PhoneNumberInteraction::PhoneDisambiguationDialogFragment
//=================================================================
CAR_INTERFACE_IMPL_2(PhoneNumberInteraction::PhoneDisambiguationDialogFragment, DialogFragment,
        IPhoneDisambiguationDialogFragment, IDialogInterfaceOnClickListener);

const String PhoneNumberInteraction::PhoneDisambiguationDialogFragment::ARG_PHONE_LIST("phoneList");
const String PhoneNumberInteraction::PhoneDisambiguationDialogFragment::ARG_INTERACTION_TYPE("interactionType");
const String PhoneNumberInteraction::PhoneDisambiguationDialogFragment::ARG_CALL_ORIGIN("callOrigin");

void PhoneNumberInteraction::PhoneDisambiguationDialogFragment::Show(
    /* [in] */ IFragmentManager* fragmentManager,
    /* [in] */ IArrayList* phoneList,
    /* [in] */ Int32 interactionType,
    /* [in] */ const String& callOrigin)
{
    AutoPtr<PhoneDisambiguationDialogFragment> fragment = new PhoneDisambiguationDialogFragment();
    AutoPtr<IBundle> bundle;
    CBundle::New((IBundle**)&bundle);
    bundle->PutParcelableArrayList(ARG_PHONE_LIST, phoneList);
    assert(0 && "TODO");
    // bundle->PutSerializable(ARG_INTERACTION_TYPE, interactionType);
    bundle->PutString(ARG_CALL_ORIGIN, callOrigin);
    fragment->SetArguments(bundle);
    IDialogFragment::Probe(fragment)->Show(fragmentManager, PhoneNumberInteraction::TAG);
}

ECode PhoneNumberInteraction::PhoneDisambiguationDialogFragment::OnCreateDialog(
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IDialog** dialog)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IBundle> arguments;
    GetArguments((IBundle**)&arguments);
    AutoPtr<IArrayList> list;
    arguments->GetParcelableArrayList(ARG_PHONE_LIST, (IArrayList**)&list);
    mPhoneList = IList::Probe(list);
    arguments->GetInt32(ARG_INTERACTION_TYPE, &mInteractionType);
    arguments->GetString(ARG_CALL_ORIGIN, &mCallOrigin);

    AutoPtr<PhoneItemAdapter> adapter = new PhoneItemAdapter();
    adapter->constructor(IContext::Probe(activity), mPhoneList, mInteractionType);
    mPhonesAdapter = adapter;
    AutoPtr<ILayoutInflater> inflater;
    activity->GetLayoutInflater((ILayoutInflater**)&inflater);
    AutoPtr<IView> setPrimaryView;
    inflater->Inflate(R::layout::set_primary_checkbox,
            NULL, (IView**)&setPrimaryView);

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(IContext::Probe(activity), (IAlertDialogBuilder**)&builder);
    builder->SetAdapter(mPhonesAdapter, this);
    assert(0 && "TODO");
    // builder->SetTitle(mInteractionType == IContactDisplayUtils::INTERACTION_SMS
    //                 ? R::string::sms_disambig_title : R::string::call_disambig_title);
    builder->SetView(setPrimaryView);

    AutoPtr<IAlertDialog> alertDialog;
    builder->Create((IAlertDialog**)&alertDialog);
    *dialog = IDialog::Probe(alertDialog);
    REFCOUNT_ADD(*dialog);
    return NOERROR;
}

ECode PhoneNumberInteraction::PhoneDisambiguationDialogFragment::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    if (activity == NULL) {
        return NOERROR;
    }

    AutoPtr<IAlertDialog> alertDialog = IAlertDialog::Probe(dialog);
    Int32 size;
    mPhoneList->GetSize(&size);
    if (size > which && which >= 0) {
        AutoPtr<IInterface> item;
        mPhoneList->Get(which, (IInterface**)&item);
        PhoneItem* phoneItem = (PhoneItem*)IObject::Probe(item);
        AutoPtr<IView> checkBox;
        IView::Probe(alertDialog)->FindViewById(R::id::setPrimary, (IView**)&checkBox);
        Boolean isChecked;
        if (ICheckable::Probe(checkBox)->IsChecked(&isChecked), isChecked) {
            // Request to mark the data as primary in the background.
            AutoPtr<IIntent> serviceIntent = ContactUpdateService::CreateSetSuperPrimaryIntent(
                    IContext::Probe(activity), phoneItem->mId);
            AutoPtr<IComponentName> name;
            IContext::Probe(activity)->StartService(serviceIntent, (IComponentName**)&name);
        }

        PhoneNumberInteraction::PerformAction(IContext::Probe(activity), phoneItem->mPhoneNumber,
                mInteractionType, mCallOrigin);
    }
    else {
        dialog->Dismiss();
    }

    return NOERROR;
}

//=================================================================
// PhoneNumberInteraction
//=================================================================
const String PhoneNumberInteraction::TAG("PhoneNumberInteraction");

const String PhoneNumberInteraction::PHONE_NUMBER_PROJECTION[] = {
    IBaseColumns::ID,                      // 0
    IContactsContractCommonDataKindsPhone::NUMBER,                   // 1
    IContactsContractDataColumns::IS_SUPER_PRIMARY,         // 2
    IContactsContractSyncColumns::ACCOUNT_TYPE,       // 3
    IContactsContractRawContactsColumns::DATA_SET,           // 4
    IContactsContractCommonDataKindsCommonColumns::TYPE,                     // 5
    IContactsContractCommonDataKindsCommonColumns::LABEL,                    // 6
    IContactsContractDataColumns::MIMETYPE,                 // 7
    IContactsContractRawContactsColumns::CONTACT_ID                // 8
};

const Int32 PhoneNumberInteraction::_ID = 0;
const Int32 PhoneNumberInteraction::NUMBER = 1;
const Int32 PhoneNumberInteraction::IS_SUPER_PRIMARY = 2;
const Int32 PhoneNumberInteraction::ACCOUNT_TYPE = 3;
const Int32 PhoneNumberInteraction::DATA_SET = 4;
const Int32 PhoneNumberInteraction::TYPE = 5;
const Int32 PhoneNumberInteraction::LABEL = 6;
const Int32 PhoneNumberInteraction::MIMETYPE = 7;
const Int32 PhoneNumberInteraction::CONTACT_ID = 8;

const String PhoneNumberInteraction::PHONE_NUMBER_SELECTION =
        IContactsContractDataColumns::MIMETYPE + " IN ('"
        + IContactsContractCommonDataKindsPhone::CONTENT_ITEM_TYPE + "', "
        + "'" + IContactsContractCommonDataKindsSipAddress::CONTENT_ITEM_TYPE
        + "') AND " + IContactsContractDataColumns::DATA1 + " NOT NULL";

const Int32 PhoneNumberInteraction::UNKNOWN_CONTACT_ID = -1;

CAR_INTERFACE_IMPL(PhoneNumberInteraction, Object, ILoaderOnLoadCompleteListener);

PhoneNumberInteraction::PhoneNumberInteraction(
    /* [in] */ IContext* context,
    /* [in] */ Int32 interactionType,
    /* [in] */ IDialogInterfaceOnDismissListener* dismissListener)
{
    PhoneNumberInteraction(context, interactionType, dismissListener, String(NULL));
}

PhoneNumberInteraction::PhoneNumberInteraction(
    /* [in] */ IContext* context,
    /* [in] */ Int32 interactionType,
    /* [in] */ IDialogInterfaceOnDismissListener* dismissListener,
    /* [in] */ const String& callOrigin)
    : mContext(context)
    , mDismissListener(dismissListener)
    , mInteractionType(interactionType)
    , mContactId(UNKNOWN_CONTACT_ID)
{
    mCallOrigin = callOrigin;
}

void PhoneNumberInteraction::PerformAction(
    /* [in] */ const String& phoneNumber)
{
    PhoneNumberInteraction::PerformAction(
            mContext, phoneNumber, mInteractionType, mCallOrigin);
}

void PhoneNumberInteraction::PerformAction(
    /* [in] */ IContext* context,
    /* [in] */ const String& phoneNumber,
    /* [in] */ Int32 interactionType,
    /* [in] */ const String& callOrigin)
{
    AutoPtr<IIntent> intent;
    assert(0 && "TODO");
    // switch (interactionType) {
    //     case IContactDisplayUtils::INTERACTION_SMS:
    //         AutoPtr<IUri> uri;
    //         Uri::FromParts(String("sms"), phoneNumber, String(NULL), (IUri**)&uri);
    //         CIntent::New(
    //                 IIntent::ACTION_SENDTO, uri, (IIntent**)&intent);
    //         break;
    //     default:
    //         intent = CallUtil::GetCallIntent(phoneNumber, callOrigin);
    //         break;
    // }
    DialerUtils::StartActivityWithErrorToast(context, intent);
}

void PhoneNumberInteraction::StartInteraction(
    /* [in] */ IUri* uri)
{
    StartInteraction(uri, TRUE);
}

ECode PhoneNumberInteraction::StartInteraction(
    /* [in] */ IUri* uri,
    /* [in] */ Boolean useDefault)
{
    if (mLoader != NULL) {
        ILoader::Probe(mLoader)->Reset();
    }
    mUseDefault = useDefault;
    AutoPtr<IUri> queryUri;
    String inputUriAsString;
    IObject::Probe(uri)->ToString(&inputUriAsString);

    AutoPtr<IContactsContractContacts> contacts;
    CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&contacts);
    AutoPtr<IUri> contentUri;
    contacts->GetCONTENT_URI((IUri**)&contentUri);
    String str;
    IObject::Probe(contentUri)->ToString(&str);

    AutoPtr<IContactsContractData> data;
    CContactsContractData::AcquireSingleton((IContactsContractData**)&data);
    AutoPtr<IUri> dataContentUri;
    data->GetCONTENT_URI((IUri**)&dataContentUri);
    String str1;
    IObject::Probe(dataContentUri)->ToString(&str1);

    if (inputUriAsString.StartWith(str)) {
        if (!inputUriAsString.EndWith(IContactsContractContactsData::CONTENT_DIRECTORY)) {
            AutoPtr<IUriHelper> helper;
            CUriHelper::AcquireSingleton((IUriHelper**)&helper);
            helper->WithAppendedPath(uri,
                    IContactsContractContactsData::CONTENT_DIRECTORY, (IUri**)&queryUri);
        }
        else {
            queryUri = uri;
        }
    }
    else if (inputUriAsString.StartWith(str1)) {
        queryUri = uri;
    }
    else {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
        // throw new UnsupportedOperationException(
        //         "Input Uri must be contact Uri or data Uri (input: \"" + uri + "\")");
    }

    AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(
        (String*)PHONE_NUMBER_PROJECTION, 9);
    CCursorLoader::New(mContext,
            queryUri,
            projection,
            PHONE_NUMBER_SELECTION,
            NULL,
            String(NULL), (ICursorLoader**)&mLoader);
    ILoader::Probe(mLoader)->RegisterListener(0, this);
    ILoader::Probe(mLoader)->StartLoading();

    return NOERROR;
}

ECode PhoneNumberInteraction::OnLoadComplete(
    /* [in] */ ILoader* loader,
    /* [in] */ IInterface* data)
{
    if (data == NULL) {
        OnDismiss();
        return NOERROR;
    }

    ICursor* cursor = ICursor::Probe(data);

    // try {
    AutoPtr<IArrayList> phoneList;
    CArrayList::New((IArrayList**)&phoneList);
    String primaryPhone(NULL);
    if (!IsSafeToCommitTransactions()) {
        OnDismiss();
        return NOERROR;
    }

    Boolean succeeded;
    while (cursor->MoveToNext(&succeeded), succeeded) {
        AutoPtr<PhoneItem> item;
        if (mContactId == UNKNOWN_CONTACT_ID) {
            FAIL_GOTO(cursor->GetInt64(CONTACT_ID, &mContactId), exit);
        }

        Int32 value;
        FAIL_GOTO(cursor->GetInt32(IS_SUPER_PRIMARY, &value), exit);
        if (mUseDefault && value != 0) {
            // Found super primary, call it.
            FAIL_GOTO(cursor->GetString(NUMBER, &primaryPhone), exit);
        }

        item = new PhoneItem();
        cursor->GetInt64(_ID, &item->mId);
        cursor->GetString(NUMBER, &item->mPhoneNumber);
        cursor->GetString(ACCOUNT_TYPE, &item->mAccountType);
        cursor->GetString(DATA_SET, &item->mDataSet);
        cursor->GetInt32(TYPE, (Int32*)&item->mType);
        cursor->GetString(LABEL, &item->mLabel);
        cursor->GetString(MIMETYPE, &item->mMimeType);

        phoneList->Add((IObject*)item);
    }

    if (mUseDefault && !primaryPhone.IsNull()) {
        PerformAction(primaryPhone);
        OnDismiss();
        return NOERROR;
    }

    assert(0 && "TODO");
    // Collapser::CollapseList(phoneList, mContext);
    Int32 size;
    phoneList->GetSize(&size);
    if (size == 0) {
        OnDismiss();
    }
    else if (size == 1) {
        AutoPtr<IInterface> item;
        phoneList->Get(0, (IInterface**)&item);
        OnDismiss();
        PerformAction(((PhoneItem*)IObject::Probe(item))->mPhoneNumber);
    }
    else {
        // There are multiple candidates. Let the user choose one.
        ShowDisambiguationDialog(phoneList);
    }

exit:
    ICloseable::Probe(cursor)->Close();
    return NOERROR;
    // } finally {
    //     cursor.close();
    // }
}

Boolean PhoneNumberInteraction::IsSafeToCommitTransactions()
{
    if (ITransactionSafeActivity::Probe(mContext)) {
        Boolean result;
        ITransactionSafeActivity::Probe(mContext)->IsSafeToCommitTransactions(&result);
        return result;
    }
    else {
        return TRUE;
    }
}

void PhoneNumberInteraction::OnDismiss()
{
    if (mDismissListener != NULL) {
        mDismissListener->OnDismiss(NULL);
    }
}

void PhoneNumberInteraction::StartInteractionForPhoneCall(
    /* [in] */ ITransactionSafeActivity* activity,
    /* [in] */ IUri* uri)
{
    assert(0 && "TODO");
    // (new PhoneNumberInteraction(activity, IContactDisplayUtils::INTERACTION_CALL, NULL))
    //             .StartInteraction(uri, TRUE);
}

void PhoneNumberInteraction::StartInteractionForPhoneCall(
    /* [in] */ ITransactionSafeActivity* activity,
    /* [in] */ IUri* uri,
    /* [in] */ Boolean useDefault)
{
    assert(0 && "TODO");
    // (new PhoneNumberInteraction(activity, IContactDisplayUtils::INTERACTION_CALL, NULL))
    //             .StartInteraction(uri, useDefault);
}

void PhoneNumberInteraction::StartInteractionForPhoneCall(
    /* [in] */ ITransactionSafeActivity* activity,
    /* [in] */ IUri* uri,
    /* [in] */ const String& callOrigin)
{
    assert(0 && "TODO");
    // (new PhoneNumberInteraction(activity, IContactDisplayUtils::INTERACTION_CALL, NULL, callOrigin))
    //             .StartInteraction(uri, TRUE);
}

void PhoneNumberInteraction::StartInteractionForTextMessage(
    /* [in] */ ITransactionSafeActivity* activity,
    /* [in] */ IUri* uri)
{
    assert(0 && "TODO");
    // (new PhoneNumberInteraction(activity, IContactDisplayUtils::INTERACTION_SMS, NULL))
    //             .StartInteraction(uri, TRUE);
}

AutoPtr<ICursorLoader> PhoneNumberInteraction::GetLoader()
{
    return mLoader;
}

void PhoneNumberInteraction::ShowDisambiguationDialog(
    /* [in] */ IArrayList* phoneList)
{
    AutoPtr<IFragmentManager> manager;
    IActivity::Probe(mContext)->GetFragmentManager((IFragmentManager**)&manager);
    PhoneDisambiguationDialogFragment::Show(manager,
                phoneList, mInteractionType, mCallOrigin);
}

} // Interactions
} // Dialer
} // Droid
} // Elastos