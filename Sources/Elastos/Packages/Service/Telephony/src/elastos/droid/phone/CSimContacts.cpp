
#include "elastos/droid/phone/CSimContacts.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CSimContacts::NamePhoneTypePair::NamePhoneTypePair(
    /* [in] */ const String& nameWithPhoneType)
{
    // Look for /W /H /M or /O at the end of the name signifying the type
    Int32 nameLen = nameWithPhoneType.GetLength();
    if (nameLen - 2 >= 0 && nameWithPhoneType.GetChar(nameLen - 2) == '/') {
        Char32 c = Character::ToUpperCase(nameWithPhoneType.GetChar(nameLen - 1));
        if (c == 'W') {
            phoneType = IPhone::TYPE_WORK;
        }
        else if (c == 'M' || c == 'O') {
            phoneType = IPhone::TYPE_MOBILE;
        }
        else if (c == 'H') {
            phoneType = IPhone::TYPE_HOME;
        }
        else {
            phoneType = IPhone::TYPE_OTHER;
        }
        name = nameWithPhoneType.substring(0, nameLen - 2);
    }
    else {
        phoneType = IPhone::TYPE_OTHER;
        name = nameWithPhoneType;
    }
}

CSimContacts::ImportAllSimContactsThread::ImportAllSimContactsThread()
    : mCanceled(FALSE)
{
    Thread::constructor(String("ImportAllSimContactsThread"));
}

ECode CSimContacts::ImportAllSimContactsThread::Run()
{
    AutoPtr<IContentValues> emptyContentValues;
    CContentValues::New((IContentValues**)&emptyContentValues);
    AutoPtr<IContentResolver> resolver;
    GetContentResolver((IContentResolver**)&resolver);

    mCursor->MoveToPosition(-1);
    Boolean res;
    while (!mCanceled && (mCursor->MoveToNext(&res), res)) {
        ActuallyImportOneSimContact(mCursor, resolver, mAccount);
        mProgressDialog->IncrementProgressBy(1);
    }

    mProgressDialog->Dismiss();
    return Finish();
}

ECode CSimContacts::ImportAllSimContactsThread::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    mCanceled = TRUE;
    return NOERROR;
}

ECode CSimContacts::ImportAllSimContactsThread::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    if (which == IDialogInterface::BUTTON_NEGATIVE) {
        mCanceled = TRUE;
        mProgressDialog->Dismiss();
    }
    else {
        String str;
        dialog->ToString(&str);
        Logger::E(LOG_TAG, "Unknown button event has come: %s", str.string());
    }
    return NOERROR;
}

static const String CSimContacts::LOG_TAG("SimContacts");

static AutoPtr<IContentValues> initEmptyContentValues()
{
    AutoPtr<IContentValues> value;
    CContentValues::New((IContentValues**)&value);
    return value;
}

static const AutoPtr<IContentValues> CSimContacts::sEmptyContentValues = initEmptyContentValues();

static const Int32 CSimContacts::MENU_IMPORT_ONE = 1;
static const Int32 CSimContacts::MENU_IMPORT_ALL = 2;

CAR_INTERFACE_IMPL(CSimContacts, ADNList, ISimContacts)

CAR_OBJECT_IMPL(CSimContacts)

ECode CSimContacts::constructor()
{
    return ADNList::constructor();
}

void CSimContacts::ActuallyImportOneSimContact(
    /* [in] */ ICursor* cursor,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IAccount* account)
{
    String column;
    cursor->GetString(NAME_COLUMN, &column);
    AutoPtr<INamePhoneTypePair> namePhoneTypePair;
    CNamePhoneTypePair::New(column, (INamePhoneTypePair**)&namePhoneTypePair);
    String name;
    namePhoneTypePair->GetName(&name);
    Int32 phoneType;
    namePhoneTypePair->GetPhoneType(&phoneType);

    String phoneNumber;
    cursor->GetString(NUMBER_COLUMN, &phoneNumber);
    String emailAddresses;
    cursor->GetString(EMAILS_COLUMN);
    AutoPtr<ArrayOf<String> > emailAddressArray;
    if (!TextUtils::IsEmpty(emailAddresses)) {
        emailAddressArray = emailAddresses.split(",");
    }
    else {
        emailAddressArray = NULL;
    }

    AutoPtr<IArrayList> operationList;
    CArrayList::New((IArrayList**)&operationList);

    AutoPtr<IContentProviderOperationHelper> helper;
    CContentProviderOperationHelper::New((IContentProviderOperationHelper**)&helper);
    AutoPtr<IContentProviderOperationBuilder> builder;
    helper->NewInsert(IRawContacts::CONTENT_URI, (IContentProviderOperationBuilder**)&builder);

    String myGroupsId;
    if (account != NULL) {
        builder->WithValue(IRawContacts::ACCOUNT_NAME, account.name);
        builder->WithValue(IRawContacts::ACCOUNT_TYPE, account.type);
    }
    else {
        builder->WithValues(sEmptyContentValues);
    }
    AutoPtr<IContentProviderOperation> operation;
    builder->Build((IContentProviderOperation**)&operation);
    operationList->Add(TO_IINTERFACE(operation));

    builder = NULL;
    helper->NewInsert(IData::CONTENT_URI, (IContentProviderOperationBuilder**)&builder);
    builder->WithValueBackReference(IStructuredName::RAW_CONTACT_ID, 0);
    builder->WithValue(IData::MIMETYPE, IStructuredName::CONTENT_ITEM_TYPE);
    builder->WithValue(IStructuredName::DISPLAY_NAME, name);
    AutoPtr<IContentProviderOperation> operation2;
    builder->Build((IContentProviderOperation**)&operation2);
    operationList->Add(TO_IINTERFACE(operation2));

    builder = NULL;
    helper->NewInsert(IData::CONTENT_URI, (IContentProviderOperationBuilder**)&builder);
    builder->WithValueBackReference(IPhone::RAW_CONTACT_ID, 0);
    builder->WithValue(IData::MIMETYPE, IPhone::CONTENT_ITEM_TYPE);
    builder->WithValue(IPhone::TYPE, phoneType);
    builder->WithValue(IPhone::NUMBER, phoneNumber);
    builder->WithValue(IData::IS_PRIMARY, 1);
    AutoPtr<IContentProviderOperation> operation3;
    builder->Build((IContentProviderOperation**)&operation3);
    operationList->Add(TO_IINTERFACE(operation3));

    if (emailAddresses != NULL) {
        for (Int32 i = 0; i < emailAddressArray->GetLength(), i++) {
            String emailAddress = (*emailAddressArray)[i];

            builder = NULL;
            helper->NewInsert(IData::CONTENT_URI, (IContentProviderOperationBuilder**)&builder);
            builder->WithValueBackReference(IEmail::RAW_CONTACT_ID, 0);
            builder->WithValue(IData::MIMETYPE, IEmail::CONTENT_ITEM_TYPE);
            builder->WithValue(IEmail::TYPE, IEmail::TYPE_MOBILE);
            builder->WithValue(IEmail::DATA, emailAddress);
            AutoPtr<IContentProviderOperation> operation4;
            builder->Build((IContentProviderOperation**)&operation4);
            operationList->Add(TO_IINTERFACE(operation4));
        }
    }

    if (myGroupsId != NULL) {
        builder = NULL;
        helper->NewInsert(IData::CONTENT_URI, (IContentProviderOperationBuilder**)&builder);
        builder->WithValueBackReference(IGroupMembership::RAW_CONTACT_ID, 0);
        builder->WithValue(IData::MIMETYPE, IGroupMembership::CONTENT_ITEM_TYPE);
        builder->WithValue(IGroupMembership::GROUP_SOURCE_ID, myGroupsId);
        AutoPtr<IContentProviderOperation> operation5;
        builder->Build((IContentProviderOperation**)&operation5);
        operationList->Add(TO_IINTERFACE(operation5));
    }

    //try {
    ECode ec = resolver->ApplyBatch(IContactsContract::AUTHORITY, operationList);
    //} catch (RemoteException e) {
    if (ec == (ECode)RemoteException) {
        Logger::E(LOG_TAG, String.format("%s: %s", e.toString(), e.getMessage()));
    }
    //} catch (OperationApplicationException e) {
    if (ec == (ECode)OperationApplicationException) {
        Logger::E(LOG_TAG, String.format("%s: %s", e.toString(), e.getMessage()));
    }
    //}
}

void CSimContacts::ImportOneSimContact(
    /* [in] */ Int32 position)
{
    AutoPtr<IContentResolver> resolver;
    GetContentResolver((IContentResolver**)&resolver);

    Boolean res;
    if (mCursor->MoveToPosition(position, &res), res) {
        ActuallyImportOneSimContact(mCursor, resolver, mAccount);
    }
    else {
        StringBuilder sb;
        sb += "Failed to move the cursor to the position \"";
        sb += position;
        sb += "\"";
        Logger::E(LOG_TAG, sb.ToString());
    }
}

ECode CSimContacts::OnCreate(
    /* [in] */ IBundle* icicle)
{
    ADNList::OnCreate(icicle);

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    if (intent != NULL) {
        String accountName;
        intent->GetStringExtra(String("account_name"), &accountName);
        String accountType;
        intent->GetStringExtra(String("account_type"), &accountType);
        if (!TextUtils::IsEmpty(accountName) && !TextUtils::IsEmpty(accountType)) {
            CAccount::New(accountName, accountType, (Account**)&mAccount);
        }
    }

    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    RegisterForContextMenu(listView);

    AutoPtr<IActionBar> actionBar;
    GetActionBar((IActionBar**)&actionBar);
    if (actionBar != NULL) {
        // android.R.id.home will be triggered in onOptionsItemSelected()
        actionBar->SetDisplayHomeAsUpEnabled(TRUE);
    }
    return NOERROR;
}

ECode CSimContacts::NewAdapter(
    /* [out] */ ICursorAdapter** adaptor)
{
    VALIDATE_NOT_NULL(adaptor)

    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    array->Set(0, String("name"));

    AutoPtr<ArrayOf<Int32> > array2 = ArrayOf<Int32>::Alloc(1);
    (*array)[0] = android.R.id.text1;

    AutoPtr<ISimpleCursorAdapter> adapter;
    CSimpleCursorAdapter::New(this, R.layout.sim_import_list_entry, mCursor,
                array, array2, (ISimpleCursorAdapter**)&adapter);

    *adaptor = ICursorAdapter::Probe(adapter);
    REFCOUNT_ADD(*adaptor);
    return NOERROR;
}

ECode CSimContacts::ResolveIntent(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);

    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> _data;
    helper->Parse(String("content://icc/adn"), (IUri**)&_data);
    intent->SetData(_data);

    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_PICK.Equals(action)) {
        // "index" is 1-based
        Int32 tmp;
        intent->GetIntExtra(String("index"), 0, &tmp);
        mInitialSelection = tmp - 1;
    }
    return intent->GetData(uri);
}

ECode CSimContacts::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    ADNList::OnCreateOptionsMenu(menu);
    menu->Add(0, MENU_IMPORT_ALL, 0, R.string.importAllSimEntries);
    *result = TRUE;
    return NOERROR;
}

ECode CSimContacts::OnPrepareOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IMenuItem> item;
    menu->FindItem(MENU_IMPORT_ALL, (IMenuItem**)&item);
    if (item != NULL) {
        Int32 count;
        item->SetVisible(mCursor != NULL && (mCursor->GetCount(&count), count) > 0);
    }
    return NOERROR;
}

ECode CSimContacts::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 id;
    item->GetItemId(&id);
    switch (id) {
        case android.R.id.home:
            OnBackPressed();
            *result = TRUE;
            return NOERROR;
        case MENU_IMPORT_ALL:
        {
            AutoPtr<ICharSequence> title;
            GetString(R.string.importAllSimEntries, (ICharSequence**)&title);
            AutoPtr<ICharSequence> message;
            GetString(R.string.importingSimContacts, (ICharSequence**)&message);

            AutoPtr<IThread> thread = new ImportAllSimContactsThread();

            // TODO: need to show some error dialog.
            if (mCursor == NULL) {
                Logger::E(LOG_TAG, "cursor is null. Ignore silently.");
                break;
            }
            CProgressDialog::New(this, (IProgressDialog**)&mProgressDialog);
            mProgressDialog->SetTitle(title);
            mProgressDialog->SetMessage(message);
            mProgressDialog->SetProgressStyle(IProgressDialog::STYLE_HORIZONTAL);
            AutoPtr<ICharSequence> cancel;
            getString(R.string.cancel, (ICharSequence**)&cancel);
            mProgressDialog->SetButton(IDialogInterface::BUTTON_NEGATIVE,
                    cancel, thread);
            mProgressDialog->SetProgress(0);
            Int32 count;
            mCursor->GetCount(&count);
            mProgressDialog->SetMax(count);
            mProgressDialog->Show();

            thread->Start();

            *result = TRUE;
            return NOERROR;
        }
    }
    return ADNList::OnOptionsItemSelected(item, result);
}

ECode CSimContacts::OnContextItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 id;
    item->GetItemId(&id);
    switch (id) {
        case MENU_IMPORT_ONE:
            AutoPtr<IContextMenuInfo** menuInfo> menuInfo;
            item->GetMenuInfo((IContextMenuInfo**)&menuInfo);
            if (IAdapterContextMenuInfo::Probe(menuInfo) ! = NULL) {
                Int32 position;
                IAdapterContextMenuInfo::Probe(menuInfo)->GetPosition(&position);
                ImportOneSimContact(position);
                *result = TRUE;
                return NOERROR;
            }
    }
    return ADNList::OnContextItemSelected(item, result);
}

ECode CSimContacts::OnCreateContextMenu(
    /* [in] */ IContextMenu* menu,
    /* [in] */ IView* v,
    /* [in] */ IContextMenuContextMenuInfo* menuInfo)
{
    if (IAdapterContextMenuInfo::Probe(menuInfo) != NULL) {
        AutoPtr<IAdapterContextMenuInfo> itemInfo = IAdapterContextMenuInfo::Probe(menuInfo);

        AutoPtr<IView> targetView;
        itemInfo->GetTargetView((IView**)&targetView);
        AutoPtr<IView> view;
        targetView->FindViewById(android.R.id.text1, (IView**)&view);
        AutoPtr<ITextView> textView = ITextView::Probe(view);
        if (textView != NULL) {
            AutoPtr<ICharSequence> title;
            textView->GetText((ICharSequence**)&title);
            menu->SetHeaderTitle(title);
        }
        menu->Add(0, MENU_IMPORT_ONE, 0, R.string.importSimEntry);
    }
    return NOERROR;
}

ECode CSimContacts::OnListItemClick(
    /* [in] */ IListView* l,
    /* [in] */ IView* v,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    return ImportOneSimContact(position);
}

ECode CSimContacts::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    switch (keyCode) {
        case IKeyEvent::KEYCODE_CALL:
        {
            Int32 position;
            GetSelectedItemPosition(&position);
            Boolean res;
            if (mCursor != NULL && (mCursor->MoveToPosition(position, &res), res)) {
                String phoneNumber;
                mCursor->GetString(NUMBER_COLUMN, &phoneNumber);
                if (phoneNumber == NULL || !TextUtils::IsGraphic(phoneNumber)) {
                    // There is no number entered.
                    //TODO play error sound or something...
                    *result = TRUE;
                    return NOERROR;
                }

                AutoPtr<IUriHelper> helper;
                CUriHelper::AcquireSingleton((IUriHelper**)&helper);
                AutoPtr<IUri> _data;
                helper->FromParts(IPhoneAccount::SCHEME_TEL, phoneNumber, String(NULL), (IUri**)&_data);
                AutoPtr<IIntent> intent;
                CIntent::New(IIntent::ACTION_CALL_PRIVILEGED, _data, (IIntent**)&intent);
                intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
                                      | IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
                StartActivity(intent);
                Finish();
                *result = TRUE;
                return NOERROR;
            }
        }
    }
    return ADNList::OnKeyDown(keyCode, event, result);
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos