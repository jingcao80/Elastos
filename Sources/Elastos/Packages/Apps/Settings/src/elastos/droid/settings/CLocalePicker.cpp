
#include "elastos/droid/settings/CLocalePicker.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/settings/CSettingsDialogFragment.h"
#include "elastos/droid/R.h"
#include "R.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::App::EIID_ILocaleSelectionListener;
using Elastos::Droid::Widget::IListView;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::StringUtils;
using Elastos::Utility::CLocale;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {

const String CLocalePicker::TAG("CLocalePicker");
const Int32 CLocalePicker::DLG_SHOW_GLOBAL_WARNING = 1;
const String CLocalePicker::SAVE_TARGET_LOCALE("locale");

//===============================================================================
//                  CLocalePicker::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL(CLocalePicker::InnerListener, Object, ILocaleSelectionListener)

CLocalePicker::InnerListener::InnerListener(
    /* [in] */ CLocalePicker* host)
    : mHost(host)
{}

ECode CLocalePicker::InnerListener::OnLocaleSelected(
    /* [in] */ ILocale* locale)
{
    return mHost->OnLocaleSelected(locale);
}

//===============================================================================
//                  CLocalePicker::OnCreateDialogRunnable
//===============================================================================

CAR_INTERFACE_IMPL(CLocalePicker::OnCreateDialogRunnable, Object, IRunnable)

CLocalePicker::OnCreateDialogRunnable::OnCreateDialogRunnable(
    /* [in] */ CLocalePicker* host,
    /* [in] */ IActivity* activity,
    /* [in] */ Int32 dialogId)
    : mHost(host)
    , mActivity(activity)
    , mDialogId(dialogId)
{}

CLocalePicker::OnCreateDialogRunnable::~OnCreateDialogRunnable()
{}

ECode CLocalePicker::OnCreateDialogRunnable::Run()
{
    mHost->RemoveDialog(mDialogId);
    mActivity->OnBackPressed();
    mHost->LocalePicker::UpdateLocale(mHost->mTargetLocale);
    return NOERROR;
}

//===============================================================================
//                  CLocalePicker
//===============================================================================

CAR_INTERFACE_IMPL(CLocalePicker, LocalePicker, IDialogCreatable)

CAR_OBJECT_IMPL(CLocalePicker)

CLocalePicker::CLocalePicker()
{}

CLocalePicker::~CLocalePicker()
{}

ECode CLocalePicker::constructor()
{
    LocalePicker::constructor();
    AutoPtr<InnerListener> listener = new InnerListener(this);
    return SetLocaleSelectionListener(listener);
}

ECode CLocalePicker::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    LocalePicker::OnCreate(savedInstanceState);
    Boolean res;
    if (savedInstanceState != NULL && (savedInstanceState->ContainsKey(SAVE_TARGET_LOCALE, &res), res)) {
        String str;
        savedInstanceState->GetString(SAVE_TARGET_LOCALE, &str);
        CLocale::New(str, (ILocale**)&mTargetLocale);
    }
    return NOERROR;
}

ECode CLocalePicker::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IView> view;
    LocalePicker::OnCreateView(inflater, container, savedInstanceState, (IView**)&view);
    AutoPtr<IView> tmp;
    view->FindViewById(Elastos::Droid::R::id::list, (IView**)&tmp);
    IListView* list = IListView::Probe(tmp);
    Utils::ForcePrepareCustomPreferencesList(container, view, list, FALSE);
    *result = view;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CLocalePicker::OnLocaleSelected(
    /* [in] */ ILocale* locale)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    if (Utils::HasMultipleUsers(IContext::Probe(activity))) {
        mTargetLocale = locale;
        ShowDialog(DLG_SHOW_GLOBAL_WARNING);
    }
    else {
        activity->OnBackPressed();
        LocalePicker::UpdateLocale(locale);
    }
    return NOERROR;
}

ECode CLocalePicker::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    LocalePicker::OnSaveInstanceState(outState);

    if (mTargetLocale != NULL) {
        String str;
        mTargetLocale->ToString(&str);
        outState->PutString(SAVE_TARGET_LOCALE, str);
    }
    return NOERROR;
}

void CLocalePicker::ShowDialog(
    /* [in] */ Int32 dialogId)
{
    if (mDialogFragment != NULL) {
        Logger::E(TAG, "Old dialog fragment not NULL!");
    }
    CSettingsDialogFragment::New(this, dialogId,
            (ISettingsDialogFragment**)&mDialogFragment);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IFragmentManager> fm;
    activity->GetFragmentManager((IFragmentManager**)&fm);
    IDialogFragment::Probe(mDialogFragment)->Show(fm, StringUtils::ToString(dialogId));
}

ECode CLocalePicker::OnCreateDialog(
    /* [in] */ Int32 dialogId,
    /* [out] */ IDialog** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<OnCreateDialogRunnable> runnable = new OnCreateDialogRunnable(this, activity, dialogId);
    AutoPtr<IDialog> dialog = Utils::BuildGlobalChangeWarningDialog(IContext::Probe(activity),
            R::string::global_locale_change_title, runnable);
    *result = dialog;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

void CLocalePicker::RemoveDialog(
    /* [in] */ Int32 dialogId)
{
    // mDialogFragment may not be visible yet in parent fragment's OnResume().
    // To be able to dismiss dialog at that time, don't check
    // mDialogFragment->IsVisible().
    if (mDialogFragment != NULL && ((CSettingsDialogFragment*)mDialogFragment.Get())->GetDialogId() == dialogId) {
        IDialogFragment::Probe(mDialogFragment)->Dismiss();
    }
    mDialogFragment = NULL;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos