
#include "R.h"
#include "elastos/droid/server/telecom/AccountSelection.h"
#include "elastos/droid/server/telecom/CallsManager.h"
#include <Elastos.Droid.Telecomm.h>
#include <Elastos.Droid.Media.h>
#include <elastos/core/StringUtils.h>
#include <elastos/droid/R.h>
#include <elastos/droid/app/Activity.h>
#include <elastos/droid/net/Uri.h>
#include <elastos/utility/logging/Logger.h>
#include <Elastos.Droid.Graphics.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Telecomm::Telecom::IPhoneAccount;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

//=============================================================================
// AccountSelection::AccountListAdapter
//=============================================================================
AccountSelection::AccountListAdapter::AccountListAdapter(
    /* [in] */ AccountSelection* host)
    : mHost(host)
{}

ECode AccountSelection::AccountListAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resource,
    /* [in] */ IList* accountHandles)
{
    ArrayAdapter::constructor(context, resource, accountHandles);
    mContext = context;
    mResId = resource;
    return NOERROR;
}

ECode AccountSelection::AccountListAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(obj);

    AutoPtr<IView> rowView;
    inflater->Inflate(mResId, NULL, (IView**)&rowView);
    AutoPtr<IView> view;
    rowView->FindViewById(R::id::text, (IView**)&view);
    AutoPtr<ITextView> textView = ITextView::Probe(view);
    view = NULL;
    rowView->FindViewById(R::id::icon, (IView**)&view);
    AutoPtr<IImageView> imageView = IImageView::Probe(view);
    obj = NULL;
    GetItem(position, (IInterface**)&obj);
    AutoPtr<IPhoneAccountHandle> accountHandle = IPhoneAccountHandle::Probe(obj);
    AutoPtr<IPhoneAccount> account;
    mHost->mTelecomManager->GetPhoneAccount(accountHandle, (IPhoneAccount**)&account);
    AutoPtr<ICharSequence> cs;
    account->GetLabel((ICharSequence**)&cs);
    textView->SetText(cs);
    AutoPtr<IDrawable> icon;
    account->GetIcon(mContext, (IDrawable**)&icon);
    imageView->SetImageDrawable(icon);
    *result = rowView;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//=============================================================================
// AccountSelection::SubDialogInterfaceOnClickListener
//=============================================================================
CAR_INTERFACE_IMPL(AccountSelection::SubDialogInterfaceOnClickListener, Object, IDialogInterfaceOnClickListener)

AccountSelection::SubDialogInterfaceOnClickListener::SubDialogInterfaceOnClickListener(
    /* [in] */ AccountSelection* host)
    : mHost(host)
{}

ECode AccountSelection::SubDialogInterfaceOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    mHost->mIsSelected = TRUE;
    AutoPtr<IInterface> obj;
    mHost->mAccountHandles->Get(which, (IInterface**)&obj);
    CallsManager::GetInstance()->PhoneAccountSelectedForMMI(mHost->mHandle, IPhoneAccountHandle::Probe(obj));
    mHost->Finish();
    return NOERROR;
}

//=============================================================================
// AccountSelection::SubDialogInterfaceOnCancelListener
//=============================================================================
CAR_INTERFACE_IMPL(AccountSelection::SubDialogInterfaceOnCancelListener, Object, IDialogInterfaceOnCancelListener)

AccountSelection::SubDialogInterfaceOnCancelListener::SubDialogInterfaceOnCancelListener(
    /* [in] */ AccountSelection* host)
    : mHost(host)
{}

// @Override
ECode AccountSelection::SubDialogInterfaceOnCancelListener::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    mHost->mIsSelected = FALSE;
    mHost->Finish();
    return NOERROR;
}

//=============================================================================
// AccountSelection
//=============================================================================
CAR_INTERFACE_IMPL(AccountSelection, Activity, IAccountSelection)

AccountSelection::AccountSelection()
    : mIsSelected(FALSE)
{}

ECode AccountSelection::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Logger::D("AccountSelection", "onCreate()...  this = %s", TO_CSTR(this));
    Activity::OnCreate(icicle);
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    AutoPtr<IBundle> extras;
    intent->GetExtras((IBundle**)&extras);
    String s;
    extras->GetString(String("Handle"), &s);
    Uri::Parse(s, (IUri**)&mHandle);
    AutoPtr<IArrayList> al;
    extras->GetParcelableArrayList(Elastos::Droid::Telecomm::Telecom::ICall::AVAILABLE_PHONE_ACCOUNTS,
        (IArrayList**)&al);
    mAccountHandles = IList::Probe(al);
    if (mAccountHandles == NULL ) {
        Finish();
        return NOERROR;
    }
    AutoPtr<IInterface> obj;
    GetSystemService(IContext::TELECOM_SERVICE, (IInterface**)&obj);
    mTelecomManager = ITelecomManager::Probe(obj);
    AutoPtr<IDialogInterfaceOnClickListener> selectionListener = new SubDialogInterfaceOnClickListener(this);
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&builder);
    AutoPtr<IContext> context;
    builder->GetContext((IContext**)&context);
    AutoPtr<IListAdapter> AccountListAdapter = new AccountSelection::AccountListAdapter(this);
    ((AccountSelection::AccountListAdapter*) AccountListAdapter.Get())->constructor(context,
            R::layout::account_list_item,
            mAccountHandles);
    builder->SetTitle(R::string::select_account_dialog_title);
    builder->SetAdapter(AccountListAdapter, selectionListener);
    builder->SetOnCancelListener(new SubDialogInterfaceOnCancelListener(this));
    AutoPtr<IAlertDialog> alertDialog;
    builder->Create((IAlertDialog**)&alertDialog);
    alertDialog = NULL;
    builder->Show((IAlertDialog**)&alertDialog);
    return NOERROR;
}

ECode AccountSelection::OnDestroy()
{
    Logger::D("AccountSelection", "onDestroy()...  this = %s", StringUtils::ToString(mIsSelected).string());
    if(!mIsSelected) {
        CallsManager::GetInstance()->PhoneAccountSelectedForMMI(mHandle, NULL);
    }
    Activity::OnDestroy();
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

