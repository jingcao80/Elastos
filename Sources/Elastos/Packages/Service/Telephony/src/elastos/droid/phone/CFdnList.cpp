
#include "elastos/droid/phone/CFdnList.h"

namespace Elastos {
namespace Droid {
namespace Phone {

const Int32 CFdnList::MENU_ADD = 1;
const Int32 CFdnList::MENU_EDIT = 2;
const Int32 CFdnList::MENU_DELETE = 3;

const String CFdnList::INTENT_EXTRA_NAME("name");
const String CFdnList::INTENT_EXTRA_NUMBER("number");

CAR_INTERFACE_IMPL(CFdnList, ADNList, IFdnList)

CAR_OBJECT_IMPL(CFdnList)

ECode CFdnList::constructor()
{
    return ADNList::constructor();
}

ECode CFdnList::OnCreate(
    /* [in] */ IBundle* icicle)
{
    ADNList::OnCreate(icicle);

    AutoPtr<IActionBar> actionBar;
    GetActionBar((IActionBar**)&actionBar);
    if (actionBar != NULL) {
        // android.R.id.home will be triggered in onOptionsItemSelected()
        actionBar->SetDisplayHomeAsUpEnabled(TRUE);
    }
    return NOERROR;
}

ECode CFdnList::ResolveIntent(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> _uri;
    helper->Parse(String("content://icc/fdn"), (IUri**)&_uri);
    intent->SetData(_uri);
    return intent->GetData(uri);
}

ECode CFdnList::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    ADNList::OnCreateOptionsMenu(menu);

    AutoPtr<IResources> r;
    GetResources((IResources**)&r);

    // Added the icons to the context menu
    String str;
    menu->Add(0, MENU_ADD, 0, (r->GetString(R.string.menu_add, &str), str))
    menu->SetIcon(android.R.drawable.ic_menu_add);

    menu->Add(0, MENU_EDIT, 0, (r->GetString(R.string.menu_edit, &str), str));
    menu->SetIcon(android.R.drawable.ic_menu_edit);

    menu->Add(0, MENU_DELETE, 0, (r->GetString(R.string.menu_delete, &str), str));
    menu->SetIcon(android.R.drawable.ic_menu_delete);
    *result = TRUE;
    return NOERROR;
}

ECode CFdnList::OnPrepareOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    ADNList::OnPrepareOptionsMenu(menu);
    Int32 position;
    GetSelectedItemPosition(&position);
    Boolean hasSelection = (position >= 0);

    menu->FindItem(MENU_ADD);
    menu->SetVisible(TRUE);

    menu->FindItem(MENU_EDIT);
    menu->SetVisible(hasSelection);

    menu->FindItem(MENU_DELETE);
    menu->SetVisible(hasSelection);

    *result = TRUE;
    return NOERROR;
}

ECode CFdnList::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 id;
    item->GetItemId(&id);
    switch (id) {
        case android.R.id.home:  // See ActionBar#setDisplayHomeAsUpEnabled()
        {
            AutoPtr<IIntent> intent;
            CIntent::New(this, FdnSetting.class, (IIntent**)&intent);
            intent->SetAction(IIntent::ACTION_MAIN);
            intent->AddFlags(IIntent::FLAG_ACTIVITY_CLEAR_TOP);
            StartActivity(intent);
            Finish();
            *result = TRUE;
            return NOERROR;
        }
        case MENU_ADD:
            AddContact();
            *result = TRUE;
            return NOERROR;

        case MENU_EDIT:
            EditSelected();
            *result = TRUE;
            return NOERROR;

        case MENU_DELETE:
            DeleteSelected();
            *result = TRUE;
            return NOERROR;
    }

    return ADNList::OnOptionsItemSelected(item, result);
}

ECode CFdnList::OnListItemClick(
    /* [in] */ IListView* l,
    /* [in] */ IView* v,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    // TODO: is this what we really want?
    return EditSelected(position);
}

void CFdnList::AddContact()
{
    // if we don't put extras "name" when starting this activity, then
    // EditFdnContactScreen treats it like add contact.
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetClass(this, EditFdnContactScreen.class);
    StartActivity(intent);
}

void CFdnList::EditSelected()
{
    Int32 position;
    GetSelectedItemPosition(&position);
    EditSelected(position);
}

void CFdnList::EditSelected(
    /* [in] */ Int32 position)
{
    Boolean res;
    if (mCursor->MoveToPosition(position, &res), res) {
        String name;
        mCursor->GetString(NAME_COLUMN, &name);
        String number;
        mCursor->GetString(NUMBER_COLUMN, &number);

        AutoPtr<IIntent> intent;
        CIntent::New((IIntent**)&intent);
        intent->SetClass(this, EditFdnContactScreen.class);
        intent->PutExtra(INTENT_EXTRA_NAME, name);
        intent->PutExtra(INTENT_EXTRA_NUMBER, number);
        StartActivity(intent);
    }
}

void CFdnList::DeleteSelected()
{
    Int32 position;
    GetSelectedItemPosition(&position);
    Boolean res;
    if (mCursor->MoveToPosition(position, &res), res) {
        String name;
        mCursor->GetString(NAME_COLUMN, &name);
        String number;
        mCursor->GetString(NUMBER_COLUMN, &number);

        AutoPtr<IIntent> intent;
        CIntent::New((IIntent**)&intent);
        intent->SetClass(this, DeleteFdnContactScreen.class);
        intent->PutExtra(INTENT_EXTRA_NAME, name);
        intent->PutExtra(INTENT_EXTRA_NUMBER, number);
        StartActivity(intent);
    }
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos