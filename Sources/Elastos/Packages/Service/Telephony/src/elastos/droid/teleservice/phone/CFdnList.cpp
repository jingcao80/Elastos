
#include "elastos/droid/teleservice/phone/CFdnList.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include "R.h"

using Elastos::Droid::App::IActionBar;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Widget::IListView;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

const String CFdnList::INTENT_EXTRA_NAME("name");
const String CFdnList::INTENT_EXTRA_NUMBER("number");

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
    AutoPtr<IUriHelper> helper;
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

    Boolean res;
    ADNList::OnCreateOptionsMenu(menu, &res);

    AutoPtr<IResources> r;
    GetResources((IResources**)&r);

    // Added the icons to the context menu
    String str;
    r->GetString(Elastos::Droid::TeleService::R::string::menu_add, &str);
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
    AutoPtr<IMenuItem> item1;
    menu->Add(0, MENU_ADD, 0, cchar, (IMenuItem**)&item1);
    item1->SetIcon(Elastos::Droid::R::drawable::ic_menu_add);

    r->GetString(Elastos::Droid::TeleService::R::string::menu_edit, &str);
    AutoPtr<IMenuItem> item2;
    AutoPtr<ICharSequence> cchar2 = CoreUtils::Convert(str);
    menu->Add(0, MENU_EDIT, 0, cchar2, (IMenuItem**)&item2);
    item2->SetIcon(Elastos::Droid::R::drawable::ic_menu_edit);

    r->GetString(Elastos::Droid::TeleService::R::string::menu_delete, &str);
    AutoPtr<IMenuItem> item3;
    AutoPtr<ICharSequence> cchar3 = CoreUtils::Convert(str);
    menu->Add(0, MENU_DELETE, 0, cchar3, (IMenuItem**)&item3);
    item3->SetIcon(Elastos::Droid::R::drawable::ic_menu_delete);
    *result = TRUE;
    return NOERROR;
}

ECode CFdnList::OnPrepareOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean res;
    ADNList::OnPrepareOptionsMenu(menu, &res);
    Int32 position;
    GetSelectedItemPosition(&position);
    Boolean hasSelection = (position >= 0);

    AutoPtr<IMenuItem> item;
    menu->FindItem(MENU_ADD, (IMenuItem**)&item);
    item->SetVisible(TRUE);

    AutoPtr<IMenuItem> item2;
    menu->FindItem(MENU_EDIT, (IMenuItem**)&item2);
    item2->SetVisible(hasSelection);

    AutoPtr<IMenuItem> item3;
    menu->FindItem(MENU_DELETE, (IMenuItem**)&item3);
    item3->SetVisible(hasSelection);

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
        case Elastos::Droid::R::id::home:  // See ActionBar#setDisplayHomeAsUpEnabled()
        {
            AutoPtr<IIntent> intent;
            assert(0);
            //CIntent::New(this, FdnSetting.class, (IIntent**)&intent);
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
    EditSelected(position);
    return NOERROR;
}

void CFdnList::AddContact()
{
    // if we don't put extras "name" when starting this activity, then
    // EditFdnContactScreen treats it like add contact.
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    assert(0);
    //intent->SetClass(this, EditFdnContactScreen.class);
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
        assert(0);
        //intent->SetClass(this, EditFdnContactScreen.class);
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
        assert(0);
        //intent->SetClass(this, DeleteFdnContactScreen.class);
        intent->PutExtra(INTENT_EXTRA_NAME, name);
        intent->PutExtra(INTENT_EXTRA_NUMBER, number);
        StartActivity(intent);
    }
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos