
#include "elastos/droid/settingsprovider/CProfilePickerActivity.h"
#include "elastos/droid/widget/ArrayAdapter.h"
#include "elastos/droid/widget/ListView.h"
#include <Elastos.Droid.View.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/Arrays.h>
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::App::IProfileManagerHelper;
using Elastos::Droid::App::CProfileManagerHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Internal::App::IAlertControllerAlertParams;
using Elastos::Droid::Internal::App::EIID_IAlertControllerAlertParamsOnPrepareListViewListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::ArrayAdapter;
using Elastos::Droid::Widget::ListView;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IUUIDHelper;
using Elastos::Utility::CUUIDHelper;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace SettingsProvider {

CAR_INTERFACE_IMPL(CProfilePickerActivity::MyDialogInterfaceOnClickListener, Object, IDialogInterfaceOnClickListener)

ECode CProfilePickerActivity::MyDialogInterfaceOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    // Save the position of most recently clicked item
    mHost->mClickedPos = which;
    return NOERROR;
}

const String CProfilePickerActivity::TAG("ProfilePickerActivity");

const String CProfilePickerActivity::SAVE_CLICKED_POS("clicked_pos");

CAR_OBJECT_IMPL(CProfilePickerActivity)

CAR_INTERFACE_IMPL_2(CProfilePickerActivity, AlertActivity, IDialogInterfaceOnClickListener, IAlertControllerAlertParamsOnPrepareListViewListener)

CProfilePickerActivity::CProfilePickerActivity()
    : mNonePos(-1)
    , mClickedPos(-1)
    , mHasNoneItem(FALSE)
{
    mProfileClickListener = new MyDialogInterfaceOnClickListener(this);
}

ECode CProfilePickerActivity::constructor()
{
    return AlertActivity::constructor();
}

ECode CProfilePickerActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    AlertActivity::OnCreate(savedInstanceState);

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);

    // Set custom theme
    Int32 themeExtra;
    intent->GetInt32Extra(IProfileManager::EXTRA_PROFILE_DIALOG_THEME, 0, &themeExtra);
    if (themeExtra != 0) {
        ECode ec = NOERROR;
        //try
        {
            AutoPtr<IPackageManager> pm;
            FAIL_GOTO(ec = GetPackageManager((IPackageManager**)&pm), ERROR)

            String package;
            FAIL_GOTO(ec = GetCallingPackage(&package), ERROR)

            AutoPtr<IResources> resources;
            FAIL_GOTO(ec = pm->GetResourcesForApplication(package, (IResources**)&resources), ERROR)

            AutoPtr<IResourcesTheme> theme;
            resources->NewTheme((IResourcesTheme**)&theme);
            theme->ApplyStyle(themeExtra, TRUE);

            AutoPtr<IResourcesTheme> tmp;
            GetTheme((IResourcesTheme**)&tmp);
            tmp->SetTo(theme);
        } //catch (NameNotFoundException e) {
    ERROR:
        if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
            // Resource not available. Fall-through default theme
        }
    }

    if (savedInstanceState != NULL) {
        savedInstanceState->GetInt32(SAVE_CLICKED_POS, -1, &mClickedPos);
    }

    // Get whether to show the 'None' item
    intent->GetBooleanExtra(IProfileManager::EXTRA_PROFILE_SHOW_NONE, TRUE, &mHasNoneItem);

    // Give the Activity so it can do managed queries
    AutoPtr<IInterface> obj;
    GetSystemService(IContext::PROFILE_SERVICE, (IInterface**)&obj);
    mProfileManager = IProfileManager::Probe(obj);

    AutoPtr<ArrayOf<IProfile*> > profiles;
    mProfileManager->GetProfiles((ArrayOf<IProfile*>**)&profiles);
    Arrays::AsList(profiles, (IList**)&mProfiles);

    // Get the UUID whose list item should have a checkmark
    String uuid;
    intent->GetStringExtra(IProfileManager::EXTRA_PROFILE_EXISTING_UUID, &uuid);
    if (uuid.IsNull()) {
        mExistingUUID = NULL;
    }
    else {
        ECode ec = NOERROR;
        //try
        {
            AutoPtr<IUUIDHelper> helper;
            CUUIDHelper::AcquireSingleton((IUUIDHelper**)&helper);
            ec = helper->FromString(uuid, (IUUID**)&mExistingUUID);
        }// catch (IllegalArgumentException e) {
        if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
            Logger::E(TAG, "Invalid existing UUID: %s", uuid.string());
            AutoPtr<IProfileManagerHelper> helper;
            CProfileManagerHelper::AcquireSingleton((IProfileManagerHelper**)&helper);
            AutoPtr<IUUID> noprofile;
            helper->GetNoProfile((IUUID**)&noprofile);
            mExistingUUID = noprofile;
        }
    }

    AutoPtr<IArrayList> profileNames;
    CArrayList::New((IArrayList**)&profileNames);
    Int32 size;
    mProfiles->GetSize(&size);
    Logger::E(TAG, "==============[snow]======================mProfiles::size = %d", size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mProfiles->Get(i, (IInterface**)&obj);
        AutoPtr<IProfile> profile = IProfile::Probe(obj);

        String name;
        profile->GetName(&name);
        profileNames->Add(CoreUtils::Convert(name));
    }

    AutoPtr<IAlertControllerAlertParams> p = mAlertParams;

    AutoPtr<ArrayAdapter> adapter = new ArrayAdapter();
    AutoPtr<ArrayOf<IInterface*> > array;
    profileNames->ToArray((ArrayOf<IInterface*>**)&array);
    adapter->constructor(this, Elastos::Droid::R::layout::select_dialog_singlechoice_holo, array);
    p->SetAdapter(IListAdapter::Probe(adapter));
    p->SetOnClickListener(mProfileClickListener);
    p->SetIsSingleChoice(TRUE);

    String str;
    GetString(Elastos::Droid::R::string::ok, &str);
    p->SetPositiveButtonText(CoreUtils::Convert(str));
    p->SetPositiveButtonListener((IDialogInterfaceOnClickListener*)this);

    GetString(Elastos::Droid::R::string::cancel, &str);
    p->SetNegativeButtonText(CoreUtils::Convert(str));
    p->SetPositiveButtonListener((IDialogInterfaceOnClickListener*)this);    // not SetNegativeButtonListener ??????

    p->SetOnPrepareListViewListener((IAlertControllerAlertParamsOnPrepareListViewListener*)this);

    AutoPtr<ICharSequence> tmp;
    intent->GetCharSequenceExtra(IProfileManager::EXTRA_PROFILE_TITLE, (ICharSequence**)&tmp);
    p->SetTitle(tmp);

    AutoPtr<ICharSequence> title;
    p->GetTitle((ICharSequence**)&title);
    if (title == NULL) {
        String str;
        GetString(Elastos::Droid::R::string::profile_picker_title, &str);
        p->SetTitle(CoreUtils::Convert(str));
    }

    SetupAlert();
    return NOERROR;
}

ECode CProfilePickerActivity::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    AlertActivity::OnSaveInstanceState(outState);
    return outState->PutInt32(SAVE_CLICKED_POS, mClickedPos);
}

ECode CProfilePickerActivity::OnPrepareListView(
    /* [in] */ IListView* listView)
{
    if (mHasNoneItem) {
        mNonePos = AddNoneItem(listView);

        // The 'None' item should use a NO_PROFILE UUID
        AutoPtr<IProfileManagerHelper> helper;
        CProfileManagerHelper::AcquireSingleton((IProfileManagerHelper**)&helper);
        AutoPtr<IUUID> noprofile;
        helper->GetNoProfile((IUUID**)&noprofile);
        Boolean res;
        if (mExistingUUID == NULL || (IObject::Probe(mExistingUUID)->Equals(noprofile, &res), res)) {
            mClickedPos = mNonePos;
        }
    }

    // Has a valid UUID
    if (mClickedPos == -1 && mExistingUUID != NULL) {
        mClickedPos = GetPositionForUUID(mExistingUUID);
    }
    if (mClickedPos == -1) {
        AutoPtr<IProfile> profile;
        mProfileManager->GetActiveProfile((IProfile**)&profile);
        AutoPtr<IUUID> uuid;
        profile->GetUuid((IUUID**)&uuid);
        mClickedPos = GetPositionForUUID(uuid);
    }

    // Put a checkmark next to an item.
    mAlertParams->SetCheckedItem(mClickedPos);
    return NOERROR;
}

Int32 CProfilePickerActivity::GetPositionForUUID(
    /* [in] */ IUUID* uuid)
{
    Int32 count;
    mProfiles->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        mProfiles->Get(i, (IInterface**)&obj);
        AutoPtr<IProfile> p = IProfile::Probe(obj);
        AutoPtr<IUUID> _uuid;
        p->GetUuid((IUUID**)&_uuid);

        Boolean res;
        if ((IObject::Probe(_uuid))->Equals(uuid, &res), res) {
            return mNonePos + i + 1;
        }
    }
    return -1;
}

Int32 CProfilePickerActivity::AddStaticItem(
    /* [in] */ IListView* listView,
    /* [in] */ Int32 textResId)
{
    AutoPtr<ILayoutInflater> inflater;
    GetLayoutInflater((ILayoutInflater**)&inflater);
    AutoPtr<IView> view;
    inflater->Inflate(Elastos::Droid::R::layout::select_dialog_singlechoice_holo, IViewGroup::Probe(listView), FALSE, (IView**)&view);
    AutoPtr<ITextView> textView = ITextView::Probe(view);

    textView->SetText(textResId);
    listView->AddHeaderView(IView::Probe(textView));
    return ((ListView*)listView)->GetHeaderViewsCount() - 1;
}

Int32 CProfilePickerActivity::AddNoneItem(
    /* [in] */ IListView* listView)
{
    return AddStaticItem(listView, Elastos::Droid::R::string::profile_none);
}

ECode CProfilePickerActivity::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    Boolean positiveResult = which == IDialogInterface::BUTTON_POSITIVE;

    if (positiveResult) {
        AutoPtr<IIntent> resultIntent;
        CIntent::New((IIntent**)&resultIntent);

        AutoPtr<IProfileManagerHelper> helper;
        CProfileManagerHelper::AcquireSingleton((IProfileManagerHelper**)&helper);
        AutoPtr<IUUID> uuid;
        helper->GetNoProfile((IUUID**)&uuid);

        if (mClickedPos != mNonePos) {
            Int32 pos = mHasNoneItem ? mClickedPos - 1 : mClickedPos;
            Int32 size;
            if (pos >= 0 && pos < (mProfiles->GetSize(&size), size)) {
                AutoPtr<IInterface> obj;
                mProfiles->Get(pos, (IInterface**)&obj);
                AutoPtr<IProfile> p = IProfile::Probe(obj);
                p->GetUuid((IUUID**)&uuid);
            }
        }

        String str;
        uuid->ToString(&str);
        resultIntent->PutExtra(IProfileManager::EXTRA_PROFILE_PICKED_UUID, str);
        SetResult(RESULT_OK, resultIntent);
    }
    else {
        SetResult(RESULT_CANCELED);
    }

    return Finish();
}

} // namespace SettingsProvider
} // namespace Droid
} // namespace Elastos
