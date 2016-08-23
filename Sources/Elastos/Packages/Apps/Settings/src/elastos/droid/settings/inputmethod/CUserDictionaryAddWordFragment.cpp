
#include "elastos/droid/settings/inputmethod/CUserDictionaryAddWordFragment.h"
#include "elastos/droid/settings/inputmethod/CUserDictionaryLocalePicker.h"
#include "elastos/droid/settings/inputmethod/UserDictionarySettingsUtils.h"
#include "elastos/droid/settings/CSettingsActivity.h"
#include "R.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Internal::App::EIID_ILocaleSelectionListener;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemSelectedListener;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

const Int32 CUserDictionaryAddWordFragment::OPTIONS_MENU_DELETE = IMenu::FIRST;

CAR_INTERFACE_IMPL_3(CUserDictionaryAddWordFragment, Fragment, IUserDictionaryAddWordFragment, IAdapterViewOnItemSelectedListener, ILocaleSelectionListener);

CAR_OBJECT_IMPL(CUserDictionaryAddWordFragment)

CUserDictionaryAddWordFragment::CUserDictionaryAddWordFragment()
    : mIsDeleting(FALSE)
{}

CUserDictionaryAddWordFragment::~CUserDictionaryAddWordFragment()
{}

ECode CUserDictionaryAddWordFragment::constructor()
{
    return Fragment::constructor();
}

ECode CUserDictionaryAddWordFragment::OnActivityCreated(
    /* [in] */ IBundle* savedInstanceState)
{
    Fragment::OnActivityCreated(savedInstanceState);
    SetHasOptionsMenu(TRUE);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IActionBar> actionBar;
    activity->GetActionBar((IActionBar**)&actionBar);
    actionBar->SetTitle(R::string::user_dict_settings_title);
    // Keep the instance so that we remember mContents when configuration changes (eg rotation)
    SetRetainInstance(TRUE);
    return NOERROR;
}

ECode CUserDictionaryAddWordFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedState,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    inflater->Inflate(R::layout::user_dictionary_add_word_fullscreen, NULL, (IView**)&mRootView);
    mIsDeleting = FALSE;
    // If we have a non-NULL mContents object, it's the old value before a configuration
    // change (eg rotation) so we need to use its values. Otherwise, read from the arguments.
    if (NULL == mContents) {
        AutoPtr<IBundle> arguments;
        GetArguments((IBundle**)&arguments);
        mContents = new UserDictionaryAddWordContents(mRootView, arguments);
    }
    else {
        // We create a new mContents object to account for the new situation : a word has
        // been added to the user dictionary when we started rotating, and we are now editing
        // it. That means in particular if the word undergoes any change, the old version should
        // be updated, so the mContents object needs to switch to EDIT mode if it was in
        // INSERT mode.
        AutoPtr<UserDictionaryAddWordContents> tmp = new UserDictionaryAddWordContents(mRootView,
                mContents /* oldInstanceToBeEdited */);
        mContents = tmp;
    }
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IActionBar> actionBar;
    activity->GetActionBar((IActionBar**)&actionBar);
    actionBar->SetSubtitle(CoreUtils::Convert(UserDictionarySettingsUtils::GetLocaleDisplayName(
            IContext::Probe(activity), mContents->GetCurrentUserDictionaryLocale())));
    *result = mRootView;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CUserDictionaryAddWordFragment::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [in] */ IMenuInflater* inflater)
{
    AutoPtr<IMenuItem> actionItem;
    menu->Add(0, OPTIONS_MENU_DELETE, 0, R::string::delete_, (IMenuItem**)&actionItem);
    actionItem->SetIcon(Elastos::Droid::R::drawable::ic_menu_delete);
    actionItem->SetShowAsAction(IMenuItem::SHOW_AS_ACTION_IF_ROOM |
            IMenuItem::SHOW_AS_ACTION_WITH_TEXT);
    return NOERROR;
}

ECode CUserDictionaryAddWordFragment::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Int32 id;
    item->GetItemId(&id);
    if (id == OPTIONS_MENU_DELETE) {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        mContents->Delete(IContext::Probe(activity));
        mIsDeleting = TRUE;
        activity->OnBackPressed();
        *result = TRUE;
        return NOERROR;
    }
    return NOERROR;
}

ECode CUserDictionaryAddWordFragment::OnResume()
{
    Fragment::OnResume();
    // We are being shown: display the word
    UpdateSpinner();
    return NOERROR;
}

void CUserDictionaryAddWordFragment::UpdateSpinner()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IArrayList> localesList = mContents->GetLocalesList(activity);

    AutoPtr<IArrayAdapter> adapter;//ArrayAdapter<LocaleRenderer>
    CArrayAdapter::New(IContext::Probe(activity),
            Elastos::Droid::R::layout::simple_spinner_item, IList::Probe(localesList), (IArrayAdapter**)&adapter);
    adapter->SetDropDownViewResource(Elastos::Droid::R::layout::simple_spinner_dropdown_item);
}

ECode CUserDictionaryAddWordFragment::OnPause()
{
    Fragment::OnPause();
    // We are being hidden: commit changes to the user dictionary, unless we were deleting it

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    if (!mIsDeleting) {
        mContents->Apply(IContext::Probe(activity), NULL);
    }
    return NOERROR;
}

ECode CUserDictionaryAddWordFragment::OnItemSelected(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 pos,
    /* [in] */ Int64 id)
{
    AutoPtr<IInterface> obj;
    parent->GetItemAtPosition(pos, (IInterface**)&obj);
    UserDictionaryAddWordContents::LocaleRenderer* locale = (UserDictionaryAddWordContents::LocaleRenderer*)IObject::Probe(obj);
    if (locale->IsMoreLanguages()) {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        CSettingsActivity* sa = (CSettingsActivity*)ISettingsActivity::Probe(activity);
        AutoPtr<IFragment> frag;
        CUserDictionaryLocalePicker::New((IUserDictionaryAddWordFragment*)this, (IFragment**)&frag);
        sa->StartPreferenceFragment(frag, TRUE);
    }
    else {
        mContents->UpdateLocale(locale->GetLocaleString());
    }
    return NOERROR;
}

ECode CUserDictionaryAddWordFragment::OnNothingSelected(
    /* [in] */ IAdapterView* parent)
{
    // I'm not sure we can come here, but if we do, that's the right thing to do.
    AutoPtr<IBundle> args;
    GetArguments((IBundle**)&args);
    String str;
    args->GetString(UserDictionaryAddWordContents::EXTRA_LOCALE, &str);
    mContents->UpdateLocale(str);
    return NOERROR;
}

ECode CUserDictionaryAddWordFragment::OnLocaleSelected(
    /* [in] */ ILocale* locale)
{
    String str;
    locale->ToString(&str);
    mContents->UpdateLocale(str);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    return activity->OnBackPressed();
}

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos