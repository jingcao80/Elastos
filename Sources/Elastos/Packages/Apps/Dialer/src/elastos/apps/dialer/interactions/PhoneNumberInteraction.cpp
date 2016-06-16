
#include "interactions/PhoneNumberInteraction.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Interactions {

//=================================================================
// PhoneNumberInteraction::PhoneItem
//=================================================================
CAR_INTERFACE_IMPL_2(PhoneNumberInteraction::PhoneItem, Object, IParcelable, ICollapserCollapsible);

PhoneNumberInteraction::PhoneItem::PhoneItem()
{}

ECode PhoneNumberInteraction::PhoneItem::WriteToParcel(
    /* [in] */ IParcel* dest,
    /* [in] */ Int32 flags)
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
    VALUE_NOT_NULL(result);
    *result = MoreContactUtils::ShouldCollapse(
            IContactsContractCommonDataKindsPhone::CONTENT_ITEM_TYPE, mPhoneNumber,
            IContactsContractCommonDataKindsPhone::CONTENT_ITEM_TYPE,
            ((PhoneItem*)phoneItem)->mPhoneNumber);
    return NOERROR;
}

ECode PhoneNumberInteraction::PhoneItem::ToString(
    /* [out] */ String* str)
{
    VALUE_NOT_NULL(str);
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
            R::layout::phone_disambig_item, Elastos::R::id::text2, list);
    mInteractionType = interactionType;
    return NOERROR;
}

ECode PhoneNumberInteraction::PhoneItemAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** result)
{
    VALUE_NOT_NULL(result);
    AutoPtr<IView> view;
    ArrayAdapter::GetView(position, convertView, parent, (IView**)&view);

    AutoPtr<IInterface> temp;
    GetItem(position, (IInterface**)&temp);
    PhoneItem* item = (PhoneItem*)temp;

    AutoPtr<IView> typeView;
    view->FindViewById(Elastos::R::id::text1, (IView**)&typeView);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<ICharSequence> value = ContactDisplayUtils::GetLabelForCallOrSms(
            (Int32)item->mType, item->mLabel, mInteractionType, context);

    ITextView::Probe(typeView)->SetText(value);
    *result = view;
    REFCOUNT_ADD(*result);

    return NOERROR;
}

//=================================================================
// PhoneNumberInteraction::PhoneDisambiguationDialogFragment
//=================================================================
CAR_INTERFACE_IMPL_3(PhoneNumberInteraction::PhoneDisambiguationDialogFragment, DialogFragment,
        IPhoneDisambiguationDialogFragment, IDialogInterfaceOnClickListener, IDialogInterfaceOnDismissListener);

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
    bundle->PutSerializable(ARG_INTERACTION_TYPE, interactionType);
    bundle->PutString(ARG_CALL_ORIGIN, callOrigin);
    fragment->SetArguments(bundle);
    fragment->Show(fragmentManager, TAG);
}

ECode PhoneNumberInteraction::PhoneDisambiguationDialogFragment::OnCreateDialog(
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IDialog** dialog)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IBundle> arguments;
    GetArguments((IBundle**)&arguments);
    arguments->GetParcelableArrayList(ARG_PHONE_LIST, &mPhoneList);
    arguments->GetInt32(ARG_INTERACTION_TYPE, &mInteractionType);
    arguments->GetString(ARG_CALL_ORIGIN, &mCallOrigin);

    mPhonesAdapter = new PhoneItemAdapter();
    mPhonesAdapter->constructor(activity, mPhoneList, mInteractionType);
    AutoPtr<ILayoutInflater> inflater;
    activity->GetLayoutInflater((ILayoutInflater**)&inflater);
    AutoPtr<IView> setPrimaryView;
    inflater->Inflate(R::layout::set_primary_checkbox,
            String(NULL), (IView**)&setPrimaryView);

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(activity, (IAlertDialogBuilder**)&builder);
    builder->SetAdapter(mPhonesAdapter, this);
    builder->SetTitle(mInteractionType == IContactDisplayUtils::INTERACTION_SMS
                    ? R::string::sms_disambig_title : R::string::call_disambig_title);
    builder->SetView(setPrimaryView);
    return builder->Create(dialog);
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
        PhoneItem* phoneItem = (PhoneItem*)item;
        AutoPtr<IView> checkBox;
        alertDialog->FindViewById(R.id.setPrimary, &checkBox);
        Boolean isChecked;
        if (ICheckBox::Probe(checkBox)->IsChecked(&isChecked), isChecked) {
            // Request to mark the data as primary in the background.
            AutoPtr<IIntent> serviceIntent = ContactUpdateService::CreateSetSuperPrimaryIntent(
                    activity, phoneItem->mId);
            activity->StartService(serviceIntent);
        }

        PhoneNumberInteraction::PerformAction(activity, phoneItem->mPhoneNumber,
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

const String PhoneNumberInteraction::PHONE_NUMBER_PROJECTION[] = {
    Phone._ID,                      // 0
    IContactsContractCommonDataKindsPhone::NUMBER,                   // 1
    Phone.IS_SUPER_PRIMARY,         // 2
    RawContacts.ACCOUNT_TYPE,       // 3
    RawContacts.DATA_SET,           // 4
    Phone.TYPE,                     // 5
    Phone.LABEL,                    // 6
    Phone.MIMETYPE,                 // 7
    Phone.CONTACT_ID                // 8
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
        Data.MIMETYPE + " IN ('"
        + Phone.CONTENT_ITEM_TYPE + "', "
        + "'" + SipAddress.CONTENT_ITEM_TYPE + "') AND "
        + Data.DATA1 + " NOT NULL";

const Int32 PhoneNumberInteraction::UNKNOWN_CONTACT_ID = -1;

PhoneNumberInteraction::PhoneNumberInteraction(
    /* [in] */ IContext* context,
    /* [in] */ Int32 interactionType,
    /* [in] */ IDialogInterfaceOnDismissListener* dismissListener)
{
    PhoneNumberInteraction(context, interactionType, dismissListener, null);
}

PhoneNumberInteraction::PhoneNumberInteraction(
    /* [in] */ IContext* context,
    /* [in] */ Int32 interactionType,
    /* [in] */ IDialogInterfaceOnDismissListener* dismissListener,
    /* [in] */ const String& callOrigin)
    : mContext(context)
    , mInteractionType(interactionType)
    , mDismissListener(dismissListener)
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
    switch (interactionType) {
        case IContactDisplayUtils::INTERACTION_SMS:
            AutoPtr<IUri> uri;
            Uri::FromParts(String("sms"), phoneNumber, String(NULL), (IUri**)&uri);
            CIntent::New(
                    IIntent::ACTION_SENDTO, uri, (IIntent**)&intent);
            break;
        default:
            intent = CallUtil::GetCallIntent(phoneNumber, callOrigin);
            break;
    }
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
        mLoader->Reset();
    }
    mUseDefault = useDefault;
    AutoPtr<IUri> queryUri;
    String inputUriAsString;
    uri->ToString(&inputUriAsString);

    AutoPtr<IContactsContractContacts> contacts;
    CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&contacts);
    AutoPtr<IUri> contentUri;
    contacts->GetCONTENT_URI((IUri**)&contentUri);
    String str;
    contentUri->ToString(&str);

    AutoPtr<IContactsContractData> data;
    CContactsContractData::AcquireSingleton((IContactsContractData**)&data);
    AutoPtr<IUri> dataContentUri;
    data->GetCONTENT_URI((IUri**)&dataContentUri);
    String str1;
    dataContentUri->ToString(&str1);

    if (inputUriAsString.StartsWith(str)) {
        if (!inputUriAsString.EndsWith(IContactsContractContactsData::CONTENT_DIRECTORY)) {
            queryUri = Uri::WithAppendedPath(uri,
                    IContactsContractContactsData::CONTENT_DIRECTORY);
        }
        else {
            queryUri = uri;
        }
    }
    else if (inputUriAsString.StartsWith(str1)) {
        queryUri = uri;
    }
    else {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
        // throw new UnsupportedOperationException(
        //         "Input Uri must be contact Uri or data Uri (input: \"" + uri + "\")");
    }

    CCursorLoader::New(mContext,
            queryUri,
            PHONE_NUMBER_PROJECTION,
            PHONE_NUMBER_SELECTION,
            NULL,
            NULL, (CursorLoader**)&mLoader);
    mLoader->RegisterListener(0, this);
    mLoader->StartLoading();

    return NOERROR;
}

ECode PhoneNumberInteraction::OnLoadComplete(
    /* [in] */ ILoader* loader,
    /* [in] */ ICursor* cursor)
{
    if (cursor == NULL) {
        OnDismiss();
        return NOERROR;
    }
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
        if (mContactId == UNKNOWN_CONTACT_ID) {
            FAIL_GOTO(cursor->GetInt64(CONTACT_ID, &mContactId), exit);
        }

        Int32 value;
        FAIL_GOTO(cursor->GetInt32(IS_SUPER_PRIMARY, &value), exit);
        if (mUseDefault && value != 0) {
            // Found super primary, call it.
            FAIL_GOTO(cursor->GetString(NUMBER, &primaryPhone), exit);
        }

        AutoPtr<PhoneItem> item = new PhoneItem();
        cursor->GetInt64(_ID, &item->mId);
        cursor->GetString(NUMBER, &item->mPhoneNumber);
        cursor->GetString(ACCOUNT_TYPE, &item->mAccountType);
        cursor->GetString(DATA_SET, &item->mDataSet);
        cursor->GetInt32(TYPE, &item->mType);
        cursor->GetString(LABEL, &item->mLabel);
        cursor->GetString(MIMETYPE, &item->mMimeType);

        phoneList->Add((IInterface*)item);
    }

    if (mUseDefault && !primaryPhone.IsNull()) {
        PerformAction(primaryPhone);
        OnDismiss();
        return NOERROR;
    }

    Collapser::CollapseList(phoneList, mContext);
    Int32 size;
    phoneList->GetSize(&size);
    if (size == 0) {
        OnDismiss();
    }
    else if (size == 1) {
        AutoPtr<IInterface> item;
        phoneList->Get(0, (IInterface**)&item);
        OnDismiss();
        PerformAction(((phoneList*)item)->mPhoneNumber);
    }
    else {
        // There are multiple candidates. Let the user choose one.
        ShowDisambiguationDialog(phoneList);
    }

exit:
    cursor->Close();
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
    (new PhoneNumberInteraction(activity, IContactDisplayUtils::INTERACTION_CALL, NULL))
                .StartInteraction(uri, TRUE);
}

void PhoneNumberInteraction::StartInteractionForPhoneCall(
    /* [in] */ ITransactionSafeActivity* activity,
    /* [in] */ IUri* uri,
    /* [in] */ Boolean useDefault)
{
    (new PhoneNumberInteraction(activity, IContactDisplayUtils::INTERACTION_CALL, NULL))
                .StartInteraction(uri, useDefault);
}

void PhoneNumberInteraction::StartInteractionForPhoneCall(
    /* [in] */ ITransactionSafeActivity* activity,
    /* [in] */ IUri* uri,
    /* [in] */ const String& callOrigin)
{
    (new PhoneNumberInteraction(activity, IContactDisplayUtils::INTERACTION_CALL, NULL, callOrigin))
                .StartInteraction(uri, TRUE);
}

void PhoneNumberInteraction::StartInteractionForTextMessage(
    /* [in] */ ITransactionSafeActivity* activity,
    /* [in] */ IUri* uri)
{
    (new PhoneNumberInteraction(activity, IContactDisplayUtils::INTERACTION_SMS, NULL))
                .StartInteraction(uri, TRUE);
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
} // Apps
} // Elastos