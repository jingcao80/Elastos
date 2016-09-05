
#include "elastos/droid/preference/PreferenceScreen.h"
#include "elastos/droid/R.h"
#include "elastos/droid/preference/CPreferenceGroupAdapter.h"
#include "elastos/droid/preference/CPreferenceManager.h"
#include "elastos/droid/app/CDialog.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/widget/CListView.h"
#include "elastos/droid/preference/CPreferenceScreenSavedState.h"

using Elastos::Core::CString;
using Elastos::Droid::App::CDialog;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnDismissListener;
using Elastos::Droid::Preference::CPreferenceGroupAdapter;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Widget::CListView;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Droid::View::IWindow;

namespace Elastos {
namespace Droid {
namespace Preference {

CAR_INTERFACE_IMPL(PreferenceScreen::InnerListener, Object, IAdapterViewOnItemClickListener)

PreferenceScreen::InnerListener::InnerListener(
    /* [in] */ PreferenceScreen* host)
    : mHost(host)
{}

ECode PreferenceScreen::InnerListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    return mHost->OnItemClick(parent, view, position, id);
}

CAR_INTERFACE_IMPL_2(PreferenceScreen, PreferenceGroup, IPreferenceScreen, \
    IDialogInterfaceOnDismissListener)

PreferenceScreen::PreferenceScreen()
{
}

ECode PreferenceScreen::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return PreferenceGroup::constructor(context, attrs, R::attr::preferenceScreenStyle, 0);
}

ECode PreferenceScreen::GetRootAdapter(
    /* [out] */ IListAdapter** adapter)
{
    VALIDATE_NOT_NULL(adapter)
    if (mRootAdapter == NULL) {
        OnCreateRootAdapter((IListAdapter**)&mRootAdapter);
    }

    *adapter = mRootAdapter;
    REFCOUNT_ADD(*adapter)
    return NOERROR;
}

ECode PreferenceScreen::OnCreateRootAdapter(
    /* [out] */ IListAdapter** adapter)
{
    VALIDATE_NOT_NULL(adapter)
    AutoPtr<IPreferenceGroupAdapter> a;
    CPreferenceGroupAdapter::New(this, (IPreferenceGroupAdapter**)&a);
    *adapter = IListAdapter::Probe(a);
    REFCOUNT_ADD(*adapter)
    return NOERROR;
}

ECode PreferenceScreen::Bind(
    /* [in] */ IListView* listView)
{
    AutoPtr<InnerListener> listener = new InnerListener(this);
    AutoPtr<IAdapterView> av = IAdapterView::Probe(listView);
    av->SetOnItemClickListener(listener.Get());
    AutoPtr<IListAdapter> adapter;
    GetRootAdapter((IListAdapter**)&adapter);
    IAdapter* a = IAdapter::Probe(adapter);
    av->SetAdapter(a);

    OnAttachedToActivity();
    return NOERROR;
}

ECode PreferenceScreen::OnClick()
{
    AutoPtr<IIntent> intent;
    String fragment;
    Int32 count;
    if ((GetIntent((IIntent**)&intent), intent != NULL) || (GetFragment(&fragment), !fragment.IsNull())
            || (GetPreferenceCount(&count), count == 0)) {
        return NOERROR;
    }

    return ShowDialog(NULL);
}

ECode PreferenceScreen::ShowDialog(
    /* [in] */IBundle* state)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    if (mListView != NULL) {
        IAdapterView* av = IAdapterView::Probe(mListView);
        av->SetAdapter(NULL);
    }

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    ILayoutInflater* inflater = ILayoutInflater::Probe(obj);
    AutoPtr<IView> childPrefScreen;
    inflater->Inflate(R::layout::preference_list_fragment, NULL, (IView**)&childPrefScreen);
    AutoPtr<IView> listViewObj;
    childPrefScreen->FindViewById(R::id::list, (IView**)&listViewObj);
    mListView = IListView::Probe(listViewObj);
    Bind(mListView);

    // Set the title bar if title is available, else no title bar
    AutoPtr<ICharSequence> title;
    GetTitle((ICharSequence**)&title);
    Int32 resID;
    context->GetThemeResId(&resID);
    AutoPtr<IDialog> dialog;
    CDialog::New(context, resID, (IDialog**)&dialog);
    mDialog = dialog;
    if (TextUtils::IsEmpty(title)) {
        AutoPtr<IWindow> window;
        dialog->GetWindow((IWindow**)&window);
        Boolean result;
        window->RequestFeature(IWindow::FEATURE_NO_TITLE, &result);
    }
    else {
        dialog->SetTitle(title);
    }
    dialog->SetContentView(childPrefScreen);
    dialog->SetOnDismissListener(this);
    if (state != NULL) {
        dialog->OnRestoreInstanceState(state);
    }

    // Add the screen to the list of preferences screens opened as dialogs
    AutoPtr<IPreferenceManager> manager;
    GetPreferenceManager((IPreferenceManager**)&manager);
    manager->AddPreferencesScreen(IDialogInterface::Probe(dialog));
    return dialog->Show();
}

ECode PreferenceScreen::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    mDialog = NULL;
    AutoPtr<IPreferenceManager> manager;
    GetPreferenceManager((IPreferenceManager**)&manager);
    manager->RemovePreferencesScreen(dialog);
    return NOERROR;
}

ECode PreferenceScreen::GetDialog(
    /* [out] */ IDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog)
    *dialog = mDialog;
    REFCOUNT_ADD(*dialog)
    return NOERROR;
}

ECode PreferenceScreen::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    // If the list has headers, subtract them from the index.
    if (IListView::Probe(parent) != NULL) {
        position -= ((CListView*)IListView::Probe(parent))->GetHeaderViewsCount();
    }

    AutoPtr<IListAdapter> adapter;
    GetRootAdapter((IListAdapter**)&adapter);
    AutoPtr<IInterface> item;
    IAdapter::Probe(adapter)->GetItem(position, (IInterface**)&item);
    IPreference* preference = IPreference::Probe(item);
    if (preference == NULL) {
        return NOERROR;
    }
    preference->PerformClick(this);
    return NOERROR;
}

ECode PreferenceScreen::IsOnSameScreenAsChildren(
    /* [out] */ Boolean* isOnSameScreenAsChildren)
{
    VALIDATE_NOT_NULL(isOnSameScreenAsChildren)
    *isOnSameScreenAsChildren = FALSE;
    return NOERROR;
}

ECode PreferenceScreen::OnSaveInstanceState(
    /* [out] */ IParcelable** state)
{
    VALIDATE_NOT_NULL(state)
    AutoPtr<IParcelable> superState;
    PreferenceGroup::OnSaveInstanceState((IParcelable**)&superState);
    AutoPtr<IDialog> dialog = mDialog;
    Boolean isShow;
    if (dialog == NULL || (dialog->IsShowing(&isShow), !isShow)) {
        *state = superState;
        REFCOUNT_ADD(*state)
        return NOERROR;
    }

    AutoPtr<IPreferenceScreenSavedState> myState;
    CPreferenceScreenSavedState::New((IPreferenceScreenSavedState**)&myState);
    myState->SetDialogShowing(TRUE);
    AutoPtr<IBundle> b;
    dialog->OnSaveInstanceState((IBundle**)&b);
    myState->SetDialogBundle(b);

    *state = IParcelable::Probe(myState);
    REFCOUNT_ADD(*state)
    return NOERROR;
}

ECode PreferenceScreen::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    if (state == NULL || IPreferenceScreenSavedState::Probe(state) == NULL) {
        // Didn't save state for us in onSaveInstanceState
        PreferenceGroup::OnRestoreInstanceState(state);
        return NOERROR;
    }

    AutoPtr<IPreferenceScreenSavedState> myState = IPreferenceScreenSavedState::Probe(state);
    AutoPtr<IParcelable> superState;
    IPreferenceBaseSavedState::Probe(myState)->GetSuperState((IParcelable**)&superState);
    PreferenceGroup::OnRestoreInstanceState(superState);
    Boolean isDialogShowing;
    if (myState->IsDialogShowing(&isDialogShowing), isDialogShowing) {
        AutoPtr<IBundle> b;
        myState->GetDialogBundle((IBundle**)&b);
        ShowDialog(b);
    }
    return NOERROR;
}

}  // Preference
}  // Droid
}  // Elastos
