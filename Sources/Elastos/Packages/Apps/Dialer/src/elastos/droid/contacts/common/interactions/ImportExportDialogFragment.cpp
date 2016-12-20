
#include "elastos/droid/contacts/common/interactions/ImportExportDialogFragment.h"
#include "elastos/droid/contacts/common/model/AccountTypeManager.h"
#include "elastos/droid/contacts/common/vcard/VCardCommonArguments.h"
#include "R.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Contacts::Common::Editor::EIID_ISelectAccountDialogFragmentListener;
using Elastos::Droid::Contacts::Common::Model::AccountTypeManager;
using Elastos::Droid::Contacts::Common::Model::IAccountTypeManager;
using Elastos::Droid::Contacts::Common::VCard::VCardCommonArguments;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Provider::IContactsContractContactsColumns;
using Elastos::Droid::Provider::IContactsContractContacts;
using Elastos::Droid::Provider::CContactsContractContacts;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Core::IInteger32;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::IO::ICloseable;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Interactions {

//=================================================================
// ImportExportDialogFragment::DialogItemAdapter
//=================================================================
ECode ImportExportDialogFragment::DialogItemAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 textViewResourceId)
{
    return ArrayAdapter::constructor(context, textViewResourceId);
}

ECode ImportExportDialogFragment::DialogItemAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)
    *view = NULL;

    AutoPtr<ITextView> result;
    if (convertView != NULL) {
        result = ITextView::Probe(convertView);
    }
    else {
        AutoPtr<IView> item;
        mDialogInflater->Inflate(Elastos::Droid::Dialer::R::layout::select_dialog_item, parent, FALSE, (IView**)&item);
        result = ITextView::Probe(item);
    }

    AutoPtr<IInterface> item;
    GetItem(position, (IInterface**)&item);
    Int32 resId;
    IInteger32::Probe(item)->GetValue(&resId);
    AutoPtr<ICharSequence> cs;
    CString::New(StringUtils::ToString(resId), (ICharSequence**)&cs);
    result->SetText(cs);
    *view = IView::Probe(result);
    REFCOUNT_ADD(*view);
    return NOERROR;
}


//=================================================================
// ImportExportDialogFragment::DialogOnClickListener
//=================================================================
CAR_INTERFACE_IMPL(ImportExportDialogFragment::DialogOnClickListener, Object, IDialogInterfaceOnClickListener)

ECode ImportExportDialogFragment::DialogOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    Boolean dismissDialog;
    Int32 resId;
    AutoPtr<IInterface> item;
    mAdapter->GetItem(which, (IInterface**)&item);
    IInteger32::Probe(item)->GetValue(&resId);
    switch (resId) {
        case Elastos::Droid::Dialer::R::string::import_from_sim:
        case Elastos::Droid::Dialer::R::string::import_from_sdcard: {
            dismissDialog = mHost->HandleImportRequest(resId);
            break;
        }
        case Elastos::Droid::Dialer::R::string::export_to_sdcard: {
            dismissDialog = TRUE;
            AutoPtr<IIntent> exportIntent;
            assert(0);
            // CIntent::New(mContext, ECLSID_CExportVCardActivity, (IIntent**)&exportIntent);
            exportIntent->PutExtra(VCardCommonArguments::ARG_CALLING_ACTIVITY,
                    mCallingActivity);
            mContext->StartActivity(exportIntent);
            break;
        }
        case Elastos::Droid::Dialer::R::string::share_visible_contacts: {
            dismissDialog = TRUE;
            mHost->DoShareVisibleContacts();
            break;
        }
        default: {
            dismissDialog = TRUE;
            AutoPtr<IResources> res;
            mContext->GetResources((IResources**)&res);
            String name;
            res->GetResourceEntryName(resId, &name);
            Logger::E(TAG, "Unexpected resource: %s", name.string());
        }
    }
    if (dismissDialog) {
        dialog->Dismiss();
    }
    return NOERROR;
}


//=================================================================
// ImportExportDialogFragment
//=================================================================
const String ImportExportDialogFragment::TAG("ImportExportDialogFragment");
const String ImportExportDialogFragment::KEY_RES_ID("resourceId");
const String ImportExportDialogFragment::ARG_CONTACTS_ARE_AVAILABLE("CONTACTS_ARE_AVAILABLE");

static AutoPtr<ArrayOf<String> > InitLOOKUP_PROJECTION()
{
    AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(1);
    (*projection)[0] = IContactsContractContactsColumns::LOOKUP_KEY;
    return projection;
}
const AutoPtr<ArrayOf<String> > ImportExportDialogFragment::LOOKUP_PROJECTION = InitLOOKUP_PROJECTION();

CAR_INTERFACE_IMPL(ImportExportDialogFragment, AnalyticsDialogFragment, ISelectAccountDialogFragmentListener);

ECode ImportExportDialogFragment::constructor()
{
    return AnalyticsDialogFragment::constructor();
}

void ImportExportDialogFragment::Show(
    /* [in] */ IFragmentManager* fragmentManager,
    /* [in] */ Boolean contactsAreAvailable,
    /* [in] */ IObject* callingActivity)
{
    AutoPtr<ImportExportDialogFragment> fragment = new ImportExportDialogFragment();
    fragment->constructor();
    AutoPtr<IBundle> args;
    CBundle::New((IBundle**)&args);
    args->PutBoolean(ARG_CONTACTS_ARE_AVAILABLE, contactsAreAvailable);
    EGuid clsId;
    callingActivity->GetClassID(&clsId);
    args->PutString(VCardCommonArguments::ARG_CALLING_ACTIVITY, String(clsId.mUunm));
    fragment->SetArguments(args);
    fragment->Show(fragmentManager, TAG);
}

ECode ImportExportDialogFragment::OnAttach(
    /* [in] */ IActivity* activity)
{
    AnalyticsDialogFragment::OnAttach(activity);
    SendScreenView();
    return NOERROR;
}

ECode ImportExportDialogFragment::OnCreateDialog(
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog)
    *dialog = NULL;

    // Wrap our context to inflate list items using the correct theme
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IContext> context = IContext::Probe(activity);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&service);
    AutoPtr<ILayoutInflater> dialogInflater = ILayoutInflater::Probe(service);
    AutoPtr<IBundle> args;
    GetArguments((IBundle**)&args);
    Boolean contactsAreAvailable;
    args->GetBoolean(ARG_CONTACTS_ARE_AVAILABLE, &contactsAreAvailable);
    String callingActivity;
    args->GetString(
            VCardCommonArguments::ARG_CALLING_ACTIVITY, &callingActivity);

    // Adapter that shows a list of string resources
    AutoPtr<DialogItemAdapter> adapter = new DialogItemAdapter(dialogInflater);
    adapter->constructor(context, Elastos::Droid::Dialer::R::layout::select_dialog_item);

    service = NULL;
    context->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&service);
    AutoPtr<ITelephonyManager> manager = ITelephonyManager::Probe(service);

    Boolean hasIccCard;
    Boolean value;
    if (manager != NULL && (manager->HasIccCard(&hasIccCard), hasIccCard)
            && (res->GetBoolean(Elastos::Droid::Dialer::R::bool_::config_allow_sim_import, &value), value)) {
        AutoPtr<ICharSequence> cs;
        CString::New(StringUtils::ToString(Elastos::Droid::Dialer::R::string::import_from_sim), (ICharSequence**)&cs);
        adapter->Add(cs);
    }
    if (res->GetBoolean(Elastos::Droid::Dialer::R::bool_::config_allow_import_from_sdcard, &value), value) {
        AutoPtr<ICharSequence> cs;
        CString::New(StringUtils::ToString(Elastos::Droid::Dialer::R::string::import_from_sdcard), (ICharSequence**)&cs);
        adapter->Add(cs);
    }
    if (res->GetBoolean(Elastos::Droid::Dialer::R::bool_::config_allow_export_to_sdcard, &value), value) {
        if (contactsAreAvailable) {
            AutoPtr<ICharSequence> cs;
            CString::New(StringUtils::ToString(Elastos::Droid::Dialer::R::string::export_to_sdcard), (ICharSequence**)&cs);
            adapter->Add(cs);
        }
    }
    if (res->GetBoolean(Elastos::Droid::Dialer::R::bool_::config_allow_share_visible_contacts, &value), value) {
        if (contactsAreAvailable) {
            AutoPtr<ICharSequence> cs;
            CString::New(StringUtils::ToString(Elastos::Droid::Dialer::R::string::share_visible_contacts), (ICharSequence**)&cs);
            adapter->Add(cs);
        }
    }

    AutoPtr<DialogOnClickListener> clickListener = new DialogOnClickListener(adapter, context, callingActivity, this);
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(context, (IAlertDialogBuilder**)&builder);
    builder->SetTitle(contactsAreAvailable
                    ? Elastos::Droid::Dialer::R::string::dialog_import_export
                    : Elastos::Droid::Dialer::R::string::dialog_import);
    builder->SetSingleChoiceItems(IListAdapter::Probe(adapter), -1, (IDialogInterfaceOnClickListener*)clickListener);
    AutoPtr<IAlertDialog> d;
    builder->Create((IAlertDialog**)&d);
    *dialog = IDialog::Probe(d);
    REFCOUNT_ADD(*dialog);
    return NOERROR;
}

void ImportExportDialogFragment::DoShareVisibleContacts()
{
    // TODO move the query into a loader and do this in a background thread
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IContext> context = IContext::Probe(activity);
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<IContactsContractContacts> contacts;
    CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&contacts);
    AutoPtr<IUri> contentUri;
    contacts->GetCONTENT_URI((IUri**)&contentUri);
    AutoPtr<ICursor> cursor;
    cr->Query(contentUri,
            LOOKUP_PROJECTION, IContactsContractContactsColumns::IN_VISIBLE_GROUP + "!=0",
            NULL, String(NULL), (ICursor**)&cursor);
    if (cursor != NULL) {
        // try {
        Boolean moveToFirst;
        if (cursor->MoveToFirst(&moveToFirst), !moveToFirst) {
            AutoPtr<IToastHelper> helper;
            CToastHelper::AcquireSingleton((IToastHelper**)&helper);
            AutoPtr<IToast> toast;
            helper->MakeText(context, Elastos::Droid::Dialer::R::string::share_error, IToast::LENGTH_SHORT, (IToast**)&toast);
            toast->Show();
            return;
        }

        StringBuilder uriListBuilder;
        Int32 index = 0;
        Boolean moveToNext;
        do {
            if (index != 0)
                uriListBuilder.AppendChar(':');
            String str;
            cursor->GetString(0, &str);
            uriListBuilder.Append(str);
            index++;
        } while (cursor->MoveToNext(&moveToNext), moveToNext);

        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);

        AutoPtr<IUri> vcardUir;
        contacts->GetCONTENT_MULTI_VCARD_URI((IUri**)&vcardUir);
        String encodeStr;
        helper->Encode(uriListBuilder.ToString(), &encodeStr);
        AutoPtr<IUri> uri;
        helper->WithAppendedPath(vcardUir, encodeStr, (IUri**)&uri);

        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_SEND, (IIntent**)&intent);
        intent->SetType(IContactsContractContacts::CONTENT_VCARD_TYPE);
        intent->PutExtra(IIntent::EXTRA_STREAM, IParcelable::Probe(uri));
        context->StartActivity(intent);
        // } finally {
        //     cursor.close();
        // }
        ICloseable::Probe(cursor)->Close();
    }
}

Boolean ImportExportDialogFragment::HandleImportRequest(
    /* [in] */ Int32 resId)
{
    // There are three possibilities:
    // - more than one accounts -> ask the user
    // - just one account -> use the account without asking the user
    // - no account -> use phone-local storage without asking the user
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IAccountTypeManager> accountTypes = AccountTypeManager::GetInstance(IContext::Probe(activity));
    AutoPtr<IList> accountList;
    accountTypes->GetAccounts(TRUE, (IList**)&accountList);
    Int32 size;
    accountList->GetSize(&size);
    if (size > 1) {
        // Send over to the account selector
        AutoPtr<IBundle> args;
        CBundle::New((IBundle**)&args);
        args->PutInt32(KEY_RES_ID, resId);
        assert(0);
        // SelectAccountDialogFragment::Show(
        //         getFragmentManager(), this,
        //         R.string.dialog_new_contact_account,
        //         AccountListFilter.ACCOUNTS_CONTACT_WRITABLE, args);

        // In this case, because this DialogFragment is used as a target fragment to
        // SelectAccountDialogFragment, we can't close it yet.  We close the dialog when
        // we get a callback from it.
        return FALSE;
    }

    assert(0);
    // AccountSelectionUtil::DoImport(getActivity(), resId,
    //         (size == 1 ? accountList.get(0) : null));
    return TRUE; // Close the dialog.
}

ECode ImportExportDialogFragment::OnAccountChosen(
    /* [in] */ IAccountWithDataSet* account,
    /* [in] */ IBundle* extraArgs)
{
    assert(0);
    // AccountSelectionUtil::DoImport(getActivity(), extraArgs.getInt(KEY_RES_ID), account);

    // At this point the dialog is still showing (which is why we can use getActivity() above)
    // So close it.
    Dismiss();
}

ECode ImportExportDialogFragment::OnAccountSelectorCancelled()
{
    // See onAccountChosen() -- at this point the dialog is still showing.  Close it.
    Dismiss();
    return NOERROR;
}

} // namespace Interactions
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

