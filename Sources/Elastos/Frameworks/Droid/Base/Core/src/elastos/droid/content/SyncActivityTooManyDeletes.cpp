
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/content/SyncActivityTooManyDeletes.h"
#include "elastos/droid/content/ContentResolver.h"
#include "elastos/droid/os/CBundle.h"

using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;


namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL_2(SyncActivityTooManyDeletes, Activity, \
    ISyncActivityTooManyDeletes, IAdapterViewOnItemClickListener)

SyncActivityTooManyDeletes::SyncActivityTooManyDeletes()
    : mNumDeletes(0)
{}

SyncActivityTooManyDeletes::~SyncActivityTooManyDeletes()
{}

ECode SyncActivityTooManyDeletes::constructor()
{
    return Activity::constructor();
}

ECode SyncActivityTooManyDeletes::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    assert(0 && "TODO");
    //FAIL_RETURN(Activity::OnCreate(savedInstanceState))

    // AutoPtr<IIntent> intent;
    // AutoPtr<IBundle> extras;
    // FAIL_RETURN(GetIntent((IIntent**)&intent))
    // FAIL_RETURN(intent->GetExtras((IBundle**)&extras))

    // if (extras == NULL) {
    //     FAIL_RETURN(Finish())
    //     return NOERROR;
    // }

    // FAIL_RETURN(extras->GetInt64(String("numDeletes"), &mNumDeletes))
    // AutoPtr<IParcelable> parcelable;
    // FAIL_RETURN(extras->GetParcelable(String("account"), (IParcelable**)&parcelable))
    // mAccount = IAccount::Probe(parcelable);
    // FAIL_RETURN(extras->GetString(String("authority"), &mAuthority))
    // FAIL_RETURN(extras->GetString(String("provider"), &mProvider))
    // AutoPtr<IResources> res;
    // FAIL_RETURN(GetResources((IResources**)&res))

    // // the order of these must match up with the constants for position used in onItemClick
    // AutoPtr<ArrayOf<ICharSequence> > options = ArrayOf<ICharSequence>::Alloc(3);
    // res->GetText(R::string::sync_really_delete, (ICharSequence**)&(*options)[0]);
    // res->GetText(R::string::sync_undo_deletes, (ICharSequence**)&(*options)[1]);
    // res->GetText(R::string::sync_do_nothing, (ICharSequence**)&(*options)[2]);

    // AutoPtr<IListAdapter> adapter;
    // // CArrayAdapter::New(this,
    // //     R::layout::simple_list_item_1, R::id::text1, options, (IListAdapter**)&adapter);

    // AutoPtr<IListView> listView;
    // // CListView::New(this, (IListView**)&listView);
    // listView->SetAdapter(adapter);
    // listView->SetItemsCanFocus(true);
    // listView->SetOnItemClickListener(this);

    // AutoPtr<ITextView> textView;
    // // CTextView::New(this, (ITextView**)&textView);

    // AutoPtr<ICharSequence> tooManyDeletesDescFormat;
    // res->GetText(R::string::sync_too_many_deletes_desc, (ICharSequence**)&tooManyDeletesDescFormat);
    // String format = Object::ToString(tooManyDeletesDescFormat);
    // String name;
    // mAccount->GetName(&name);
    // String str;
    // str.AppendFormat(format.string(), mNumDeletes, mProvider.string(), name.string());
    // textView->SetText(str);

    AutoPtr<ILinearLayout> ll;
    // CLinearLayout::New(this, (ILinearLayout**)&ll);
    // ll->SetOrientation(ILinearLayout::VERTICAL);
    // AutoPtr<ILinearLayoutLayoutParams> lp;
    // CLinearLayoutLayoutParams::New(
    //    IViewGroupLayoutParams::MATCH_PARENT,
    //    IViewGroupLayoutParams::WRAP_CONTENT, 0, (ILinearLayoutLayoutParams**)&lp);
    // ll->AddView(textView, lp);
    // ll->AddView(listView, lp);

    // TODO: consider displaying the icon of the account type
//        AuthenticatorDescription[] descs = AccountManager.get(this).getAuthenticatorTypes();
//        for (AuthenticatorDescription desc : descs) {
//            if (desc.type.equals(mAccount.type)) {
//                try {
//                    final Context authContext = createPackageContext(desc.packageName, 0);
//                    ImageView imageView = new ImageView(this);
//                    imageView.setImageDrawable(authContext.getDrawable(desc.iconId));
//                    ll.addView(imageView, lp);
//                } catch (PackageManager.NameNotFoundException e) {
//                }
//                break;
//            }
//        }

    // FAIL_RETURN(SetContentView(ll))
    return NOERROR;
}

ECode SyncActivityTooManyDeletes::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    // the constants for position correspond to the items options array in onCreate()
    if (position == 0) {
        FAIL_RETURN(StartSyncReallyDelete())
    }
    else if (position == 1) {
        FAIL_RETURN(StartSyncUndoDeletes())
    }
    FAIL_RETURN(Finish())
    return NOERROR;
}

ECode SyncActivityTooManyDeletes::StartSyncReallyDelete()
{
    AutoPtr<IBundle> extras;
    FAIL_RETURN(CBundle::New((IBundle**)&extras))
    FAIL_RETURN(extras->PutBoolean(IContentResolver::SYNC_EXTRAS_OVERRIDE_TOO_MANY_DELETIONS, TRUE))
    FAIL_RETURN(extras->PutBoolean(IContentResolver::SYNC_EXTRAS_MANUAL, TRUE))
    FAIL_RETURN(extras->PutBoolean(IContentResolver::SYNC_EXTRAS_EXPEDITED, TRUE))
    FAIL_RETURN(extras->PutBoolean(IContentResolver::SYNC_EXTRAS_UPLOAD, TRUE))
    FAIL_RETURN(ContentResolver::RequestSync(mAccount, mAuthority, extras))
    return NOERROR;
}

ECode SyncActivityTooManyDeletes::StartSyncUndoDeletes()
{
    AutoPtr<IBundle> extras;
    FAIL_RETURN(CBundle::New((IBundle**)&extras))
    FAIL_RETURN(extras->PutBoolean(IContentResolver::SYNC_EXTRAS_DISCARD_LOCAL_DELETIONS, TRUE))
    FAIL_RETURN(extras->PutBoolean(IContentResolver::SYNC_EXTRAS_MANUAL, TRUE))
    FAIL_RETURN(extras->PutBoolean(IContentResolver::SYNC_EXTRAS_EXPEDITED, TRUE))
    FAIL_RETURN(extras->PutBoolean(IContentResolver::SYNC_EXTRAS_UPLOAD, TRUE))
    FAIL_RETURN(ContentResolver::RequestSync(mAccount, mAuthority, extras))
    return NOERROR;
}

}
}
}
