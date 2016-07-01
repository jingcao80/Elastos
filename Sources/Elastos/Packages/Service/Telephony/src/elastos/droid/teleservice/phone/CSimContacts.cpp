
#include "elastos/droid/teleservice/phone/CSimContacts.h"
#include "elastos/droid/text/TextUtils.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telecomm.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/core/Character.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/R.h"
#include "R.h"

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IActionBar;
using Elastos::Droid::App::CProgressDialog;
using Elastos::Droid::App::IProgressDialog;
using Elastos::Droid::Accounts::CAccount;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentProviderResult;
using Elastos::Droid::Content::IContentProviderOperation;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentProviderOperationHelper;
using Elastos::Droid::Content::CContentProviderOperationHelper;
using Elastos::Droid::Content::IContentProviderOperationBuilder;
using Elastos::Droid::Content::CContentProviderOperationBuilder;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsPhone;
using Elastos::Droid::Provider::IContactsContractRawContacts;
using Elastos::Droid::Provider::CContactsContractRawContacts;
using Elastos::Droid::Provider::IContactsContractSyncColumns;
using Elastos::Droid::Provider::IContactsContractDataColumns;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::IContactsContract;
using Elastos::Droid::Provider::IContactsContractDataColumns;
using Elastos::Droid::Provider::IContactsContractData;
using Elastos::Droid::Provider::CContactsContractData;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsStructuredName;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Telecomm::Telecom::IPhoneAccount;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IContextMenuInfo;
using Elastos::Droid::Widget::ISimpleCursorAdapter;
using Elastos::Droid::Widget::CSimpleCursorAdapter;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IAdapterContextMenuInfo;
using Elastos::Core::Character;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CSimContacts::NamePhoneTypePair::NamePhoneTypePair(
    /* [in] */ const String& nameWithPhoneType)
{
    // Look for /W /H /M or /O at the end of the name signifying the type
    Int32 nameLen = nameWithPhoneType.GetLength();
    if (nameLen - 2 >= 0 && nameWithPhoneType.GetChar(nameLen - 2) == '/') {
        Char32 c = Character::ToUpperCase(nameWithPhoneType.GetChar(nameLen - 1));
        if (c == 'W') {
            mPhoneType = IContactsContractCommonDataKindsPhone::TYPE_WORK;
        }
        else if (c == 'M' || c == 'O') {
            mPhoneType = IContactsContractCommonDataKindsPhone::TYPE_MOBILE;
        }
        else if (c == 'H') {
            mPhoneType = IContactsContractCommonDataKindsPhone::TYPE_HOME;
        }
        else {
            mPhoneType = IContactsContractCommonDataKindsPhone::TYPE_OTHER;
        }
        mName = nameWithPhoneType.Substring(0, nameLen - 2);
    }
    else {
        mPhoneType = IContactsContractCommonDataKindsPhone::TYPE_OTHER;
        mName = nameWithPhoneType;
    }
}

CAR_INTERFACE_IMPL_2(CSimContacts::ImportAllSimContactsThread, Thread,
        IDialogInterfaceOnCancelListener, IDialogInterfaceOnClickListener)

CSimContacts::ImportAllSimContactsThread::ImportAllSimContactsThread(
    /* [in] */ CSimContacts* host)
    : mHost(host)
    , mCanceled(FALSE)
{
    Thread::constructor(String("ImportAllSimContactsThread"));
}

ECode CSimContacts::ImportAllSimContactsThread::Run()
{
    AutoPtr<IContentValues> emptyContentValues;
    CContentValues::New((IContentValues**)&emptyContentValues);
    AutoPtr<IContentResolver> resolver;
    mHost->GetContentResolver((IContentResolver**)&resolver);

    Boolean res;
    mHost->mCursor->MoveToPosition(-1, &res);
    while (!mCanceled && (mHost->mCursor->MoveToNext(&res), res)) {
        ActuallyImportOneSimContact(mHost->mCursor, resolver, mHost->mAccount);
        mHost->mProgressDialog->IncrementProgressBy(1);
    }

    IDialogInterface::Probe(mHost->mProgressDialog)->Dismiss();
    return mHost->Finish();
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
        IDialogInterface::Probe(mHost->mProgressDialog)->Dismiss();
    }
    else {
        Logger::E(TAG, "Unknown button event has come: %s", TO_CSTR(dialog));
    }
    return NOERROR;
}

const String CSimContacts::TAG("SimContacts");

static AutoPtr<IContentValues> initEmptyContentValues()
{
    AutoPtr<IContentValues> value;
    CContentValues::New((IContentValues**)&value);
    return value;
}

const AutoPtr<IContentValues> CSimContacts::sEmptyContentValues = initEmptyContentValues();

const Int32 CSimContacts::MENU_IMPORT_ONE = 1;
const Int32 CSimContacts::MENU_IMPORT_ALL = 2;

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
    AutoPtr<NamePhoneTypePair> namePhoneTypePair = new NamePhoneTypePair(column);
    String name = namePhoneTypePair->mName;
    Int32 phoneType = namePhoneTypePair->mPhoneType;

    String phoneNumber;
    cursor->GetString(NUMBER_COLUMN, &phoneNumber);
    String emailAddresses;
    cursor->GetString(EMAILS_COLUMN, &emailAddresses);
    AutoPtr<ArrayOf<String> > emailAddressArray;
    if (!TextUtils::IsEmpty(emailAddresses)) {
        StringUtils::Split(emailAddresses, String(","), (ArrayOf<String>**)&emailAddressArray);
    }
    else {
        emailAddressArray = NULL;
    }

    AutoPtr<IArrayList> operationList;
    CArrayList::New((IArrayList**)&operationList);

    AutoPtr<IContactsContractRawContacts> contacts;
    CContactsContractRawContacts::AcquireSingleton((IContactsContractRawContacts**)&contacts);
    AutoPtr<IUri> uri;
    contacts->GetCONTENT_URI((IUri**)&uri);

    AutoPtr<IContentProviderOperationHelper> helper;
    CContentProviderOperationHelper::AcquireSingleton((IContentProviderOperationHelper**)&helper);
    AutoPtr<IContentProviderOperationBuilder> builder;
    helper->NewInsert(uri, (IContentProviderOperationBuilder**)&builder);

    String myGroupsId;
    if (account != NULL) {
        String name;
        account->GetName(&name);
        builder->WithValue(IContactsContractSyncColumns::ACCOUNT_NAME, name);

        String type;
        account->GetType(&type);
        builder->WithValue(IContactsContractSyncColumns::ACCOUNT_TYPE, type);
    }
    else {
        builder->WithValues(sEmptyContentValues);
    }
    AutoPtr<IContentProviderOperation> operation;
    builder->Build((IContentProviderOperation**)&operation);
    operationList->Add(TO_IINTERFACE(operation));

    builder = NULL;
    AutoPtr<IContactsContractData> data;
    CContactsContractData::AcquireSingleton((IContactsContractData**)&data);
    AutoPtr<IUri> uri2;
    data->GetCONTENT_URI((IUri**)&uri2);
    helper->NewInsert(uri2, (IContentProviderOperationBuilder**)&builder);
    builder->WithValueBackReference(IContactsContractDataColumns::RAW_CONTACT_ID, 0);
    builder->WithValue(IContactsContractDataColumns::MIMETYPE, IContactsContractCommonDataKindsStructuredName::CONTENT_ITEM_TYPE);
    builder->WithValue(IContactsContractCommonDataKindsStructuredName::DISPLAY_NAME, name);
    AutoPtr<IContentProviderOperation> operation2;
    builder->Build((IContentProviderOperation**)&operation2);
    operationList->Add(TO_IINTERFACE(operation2));

    assert(0);
    // builder = NULL;
    // helper->NewInsert(uri2, (IContentProviderOperationBuilder**)&builder);
    // builder->WithValueBackReference(IContactsContractDataColumns::RAW_CONTACT_ID, 0);
    // builder->WithValue(IContactsContractCommonDataKindsPhone::MIMETYPE, IContactsContractCommonDataKindsPhone::CONTENT_ITEM_TYPE);
    // builder->WithValue(IContactsContractCommonDataKindsPhone::TYPE, phoneType);
    // builder->WithValue(IContactsContractCommonDataKindsPhone::NUMBER, phoneNumber);
    // builder->WithValue(IContactsContractDataColumns::IS_PRIMARY, 1);
    // AutoPtr<IContentProviderOperation> operation3;
    // builder->Build((IContentProviderOperation**)&operation3);
    // operationList->Add(TO_IINTERFACE(operation3));

    // if (emailAddresses != NULL) {
    //     for (Int32 i = 0; i < emailAddressArray->GetLength(), i++) {
    //         String emailAddress = (*emailAddressArray)[i];

    //         builder = NULL;
    //         helper->NewInsert(uri2, (IContentProviderOperationBuilder**)&builder);
    //         builder->WithValueBackReference(IEmail::RAW_CONTACT_ID, 0);
    //         builder->WithValue(IContactsContractDataColumns::MIMETYPE, IEmail::CONTENT_ITEM_TYPE);
    //         builder->WithValue(IEmail::TYPE, IEmail::TYPE_MOBILE);
    //         builder->WithValue(IEmail::DATA, emailAddress);
    //         AutoPtr<IContentProviderOperation> operation4;
    //         builder->Build((IContentProviderOperation**)&operation4);
    //         operationList->Add(TO_IINTERFACE(operation4));
    //     }
    // }

    // if (myGroupsId != NULL) {
    //     builder = NULL;
    //     helper->NewInsert(uri2, (IContentProviderOperationBuilder**)&builder);
    //     builder->WithValueBackReference(IGroupMembership::RAW_CONTACT_ID, 0);
    //     builder->WithValue(IContactsContractDataColumns::MIMETYPE, IGroupMembership::CONTENT_ITEM_TYPE);
    //     builder->WithValue(IGroupMembership::GROUP_SOURCE_ID, myGroupsId);
    //     AutoPtr<IContentProviderOperation> operation5;
    //     builder->Build((IContentProviderOperation**)&operation5);
    //     operationList->Add(TO_IINTERFACE(operation5));
    // }

    //try {
    AutoPtr<ArrayOf<IContentProviderResult*> > tmp;
    ECode ec = resolver->ApplyBatch(IContactsContract::AUTHORITY, operationList,
            (ArrayOf<IContentProviderResult*>**)&tmp);
    //} catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::E(TAG, "E_REMOTE_EXCEPTION %d", ec);
    }
    //} catch (OperationApplicationException e) {
    if (ec == (ECode)E_OPERATION_APPLICATION_EXCEPTION) {
        Logger::E(TAG, "E_REMOTE_EXCEPTION %d", ec);
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
        Logger::E(TAG, sb.ToString());
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
            CAccount::New(accountName, accountType, (IAccount**)&mAccount);
        }
    }

    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    RegisterForContextMenu(IView::Probe(listView));

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
    (*array2)[0] = Elastos::Droid::R::id::text1;

    AutoPtr<ISimpleCursorAdapter> adapter;
    CSimpleCursorAdapter::New(this, Elastos::Droid::TeleService::R::layout::sim_import_list_entry, mCursor,
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
        intent->GetInt32Extra(String("index"), 0, &tmp);
        mInitialSelection = tmp - 1;
    }
    return intent->GetData(uri);
}

ECode CSimContacts::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean res;
    ADNList::OnCreateOptionsMenu(menu, &res);
    AutoPtr<IMenuItem> tmp;
    menu->Add(0, MENU_IMPORT_ALL, 0, Elastos::Droid::TeleService::R::string::importAllSimEntries,
            (IMenuItem**)&tmp);
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
        case Elastos::Droid::R::id::home:
            OnBackPressed();
            *result = TRUE;
            return NOERROR;
        case MENU_IMPORT_ALL:
        {
            String title;
            GetString(Elastos::Droid::TeleService::R::string::importAllSimEntries, &title);
            String message;
            GetString(Elastos::Droid::TeleService::R::string::importingSimContacts, &message);

            AutoPtr<IThread> thread = new ImportAllSimContactsThread(this);

            // TODO: need to show some error dialog.
            if (mCursor == NULL) {
                Logger::E(TAG, "cursor is null. Ignore silently.");
                break;
            }
            CProgressDialog::New(this, (IProgressDialog**)&mProgressDialog);
            AutoPtr<ICharSequence> obj = CoreUtils::Convert(title);
            IDialog::Probe(mProgressDialog)->SetTitle(obj);
            AutoPtr<ICharSequence> obj2 = CoreUtils::Convert(message);
            IAlertDialog::Probe(mProgressDialog)->SetMessage(obj2);
            mProgressDialog->SetProgressStyle(IProgressDialog::STYLE_HORIZONTAL);
            String cancel;
            GetString(Elastos::Droid::TeleService::R::string::cancel, &cancel);
            AutoPtr<ICharSequence> obj3 = CoreUtils::Convert(cancel);
            IAlertDialog::Probe(mProgressDialog)->SetButton(IDialogInterface::BUTTON_NEGATIVE, obj3,
                    IDialogInterfaceOnClickListener::Probe(thread));
            mProgressDialog->SetProgress(0);
            Int32 count;
            mCursor->GetCount(&count);
            mProgressDialog->SetMax(count);
            IDialog::Probe(mProgressDialog)->Show();

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
            AutoPtr<IContextMenuInfo> menuInfo;
            item->GetMenuInfo((IContextMenuInfo**)&menuInfo);
            if (IAdapterContextMenuInfo::Probe(menuInfo) != NULL) {
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
    /* [in] */ IContextMenuInfo* menuInfo)
{
    if (IAdapterContextMenuInfo::Probe(menuInfo) != NULL) {
        AutoPtr<IAdapterContextMenuInfo> itemInfo = IAdapterContextMenuInfo::Probe(menuInfo);

        AutoPtr<IView> targetView;
        itemInfo->GetTargetView((IView**)&targetView);
        AutoPtr<IView> view;
        targetView->FindViewById(Elastos::Droid::R::id::text1, (IView**)&view);
        AutoPtr<ITextView> textView = ITextView::Probe(view);
        if (textView != NULL) {
            AutoPtr<ICharSequence> title;
            textView->GetText((ICharSequence**)&title);
            menu->SetHeaderTitle(title);
        }
        AutoPtr<IMenuItem> tmp;
        IMenu::Probe(menu)->Add(0, MENU_IMPORT_ONE, 0, Elastos::Droid::TeleService::R::string::importSimEntry,
                (IMenuItem**)&tmp);
    }
    return NOERROR;
}

ECode CSimContacts::OnListItemClick(
    /* [in] */ IListView* l,
    /* [in] */ IView* v,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    ImportOneSimContact(position);
    return NOERROR;
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
                AutoPtr<ICharSequence> obj = CoreUtils::Convert(phoneNumber);
                if (phoneNumber.IsNull() || !TextUtils::IsGraphic(obj)) {
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
} // namespace TeleService
} // namespace Droid
} // namespace Elastos