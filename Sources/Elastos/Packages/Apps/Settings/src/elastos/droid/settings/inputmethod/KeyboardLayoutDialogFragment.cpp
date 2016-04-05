
#include "elastos/droid/settings/inputmethod/KeyboardLayoutDialogFragment.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "_Settings.h"
#include "../R.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::ILoaderManager;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::EIID_ILoaderManagerLoaderCallbacks;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IContextWrapper;
using Elastos::Droid::Hardware::Input::IKeyboardLayout;
using Elastos::Droid::Hardware::Input::EIID_IInputDeviceListener;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IAbsListView;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::ICheckedTextView;
using Elastos::Droid::Widget::IRadioButton;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

const String KeyboardLayoutDialogFragment::KEY_INPUT_DEVICE_IDENTIFIER("inputDeviceIdentifier");

//===============================================================================
//                  KeyboardLayoutDialogFragment::Keyboards
//===============================================================================

KeyboardLayoutDialogFragment::Keyboards::Keyboards()
    : mCurrent(-1)
{
    CArrayList::New((IArrayList**)&mKeyboardLayouts);
}

KeyboardLayoutDialogFragment::Keyboards::~Keyboards()
{}

//===============================================================================
//                  KeyboardLayoutDialogFragment::KeyboardLayoutAdapter
//===============================================================================

KeyboardLayoutDialogFragment::KeyboardLayoutAdapter::KeyboardLayoutAdapter(
    /* [in] */ IContext* context)
    : mCheckedItem(-1)
{
    ArrayAdapter::constructor(context,
            Elastos::Droid::R::layout::simple_list_item_2_single_choice);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    mInflater = ILayoutInflater::Probe(obj);
}

KeyboardLayoutDialogFragment::KeyboardLayoutAdapter::~KeyboardLayoutAdapter()
{}

ECode KeyboardLayoutDialogFragment::KeyboardLayoutAdapter::SetCheckedItem(
    /* [in] */ Int32 position)
{
    mCheckedItem = position;
    return NOERROR;
}

ECode KeyboardLayoutDialogFragment::KeyboardLayoutAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    AutoPtr<IInterface> obj;
    GetItem(position, (IInterface**)&obj);
    AutoPtr<IKeyboardLayout> item = IKeyboardLayout::Probe(obj);
    String label, collection;
    if (item != NULL) {
        item->GetLabel(&label);
        item->GetCollection(&collection);
    }
    else {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        context->GetString(R::string::keyboard_layout_default_label, &label);
        collection = "";
    }

    Boolean checked = (position == mCheckedItem);
    if (collection.IsEmpty()) {
        AutoPtr<IView> _view = InflateOneLine(convertView, parent, label, checked);
        *view = _view;
        REFCOUNT_ADD(*view);
        return NOERROR;
    }
    else {
        AutoPtr<IView> _view = InflateTwoLine(convertView, parent, label, collection, checked);
        *view = _view;
        REFCOUNT_ADD(*view);
        return NOERROR;
    }
}

AutoPtr<IView> KeyboardLayoutDialogFragment::KeyboardLayoutAdapter::InflateOneLine(
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [in] */ const String& label,
    /* [in] */ Boolean checked)
{
    AutoPtr<IView> view = convertView;
    if (view == NULL || IsTwoLine(view)) {
        view = NULL;
        mInflater->Inflate(
                Elastos::Droid::R::layout::simple_list_item_single_choice,
                parent, FALSE, (IView**)&view);
        SetTwoLine(view, FALSE);
    }
    AutoPtr<IView> _view;
    view->FindViewById(Elastos::Droid::R::id::text1, (IView**)&_view);
    AutoPtr<ICheckedTextView> headline = ICheckedTextView::Probe(_view);
    ITextView::Probe(headline)->SetText(CoreUtils::Convert(label));
    ICheckable::Probe(headline)->SetChecked(checked);
    return view;
}

AutoPtr<IView> KeyboardLayoutDialogFragment::KeyboardLayoutAdapter::InflateTwoLine(
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [in] */ const String& label,
    /* [in] */ const String& collection,
    /* [in] */ Boolean checked)
{
    AutoPtr<IView> view = convertView;
    if (view == NULL || !IsTwoLine(view)) {
        view = NULL;
        mInflater->Inflate(
                Elastos::Droid::R::layout::simple_list_item_2_single_choice,
                parent, FALSE, (IView**)&view);
        SetTwoLine(view, TRUE);
    }
    AutoPtr<IView> _view;
    view->FindViewById(Elastos::Droid::R::id::text1, (IView**)&_view);
    AutoPtr<ITextView> headline = ITextView::Probe(_view);
    _view = NULL;
    view->FindViewById(Elastos::Droid::R::id::text2, (IView**)&_view);
    AutoPtr<ITextView> subText = ITextView::Probe(_view);
    _view = NULL;
    view->FindViewById(Elastos::Droid::R::id::radio, (IView**)&_view);
    AutoPtr<IRadioButton> radioButton = IRadioButton::Probe(_view);
    headline->SetText(CoreUtils::Convert(label));
    subText->SetText(CoreUtils::Convert(collection));
    ICheckable::Probe(radioButton)->SetChecked(checked);
    return view;
}

Boolean KeyboardLayoutDialogFragment::KeyboardLayoutAdapter::IsTwoLine(
    /* [in] */ IView* view)
{
    AutoPtr<IInterface> obj;
    view->GetTag((IInterface**)&obj);
    return IBoolean::Probe(obj) == CoreUtils::Convert(TRUE);
}

void KeyboardLayoutDialogFragment::KeyboardLayoutAdapter::SetTwoLine(
    /* [in] */ IView* view,
    /* [in] */ Boolean twoLine)
{
    view->SetTag(CoreUtils::Convert(twoLine));
}

//===============================================================================
//                  KeyboardLayoutDialogFragment::KeyboardLayoutLoader
//===============================================================================

KeyboardLayoutDialogFragment::KeyboardLayoutLoader::KeyboardLayoutLoader(
    /* [in] */ IContext* context,
    /* [in] */ IInputDeviceIdentifier* inputDeviceIdentifier)
{
    AsyncTaskLoader::constructor(context);
    mInputDeviceIdentifier = inputDeviceIdentifier;
}

KeyboardLayoutDialogFragment::KeyboardLayoutLoader::~KeyboardLayoutLoader()
{}

ECode KeyboardLayoutDialogFragment::KeyboardLayoutLoader::LoadInBackground(
    /* [out] */ IInterface** result)//Keyboards
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<Keyboards> keyboards = new Keyboards();
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::INPUT_SERVICE, (IInterface**)&obj);
    AutoPtr<IInputManager> im = IInputManager::Probe(obj);
    AutoPtr< ArrayOf<String> > keyboardLayoutDescriptors;
    im->GetKeyboardLayoutsForInputDevice(mInputDeviceIdentifier,
            (ArrayOf<String>**)&keyboardLayoutDescriptors);
    for (Int32 i = 0; i < keyboardLayoutDescriptors->GetLength(); i++) {
        String keyboardLayoutDescriptor = (*keyboardLayoutDescriptors)[i];
        AutoPtr<IKeyboardLayout> keyboardLayout;
        im->GetKeyboardLayout(keyboardLayoutDescriptor,
                (IKeyboardLayout**)&keyboardLayout);
        if (keyboardLayout != NULL) {
            keyboards->mKeyboardLayouts->Add(keyboardLayout);
        }
    }

    AutoPtr<ICollections> coll;
    CCollections::AcquireSingleton((ICollections**)&coll);
    coll->Sort(IList::Probe(keyboards->mKeyboardLayouts));

    String currentKeyboardLayoutDescriptor;
    im->GetCurrentKeyboardLayoutForInputDevice(mInputDeviceIdentifier,
            &currentKeyboardLayoutDescriptor);
    if (!currentKeyboardLayoutDescriptor.IsNull()) {
        Int32 numKeyboardLayouts;
        keyboards->mKeyboardLayouts->GetSize(&numKeyboardLayouts);
        for (Int32 i = 0; i < numKeyboardLayouts; i++) {
            obj = NULL;
            keyboards->mKeyboardLayouts->Get(i, (IInterface**)&obj);
            AutoPtr<IKeyboardLayout> kbl = IKeyboardLayout::Probe(obj);
            String descriptor;
            kbl->GetDescriptor(&descriptor);
            if (descriptor.Equals(
                    currentKeyboardLayoutDescriptor)) {
                keyboards->mCurrent = i;
                break;
            }
        }
    }

    Boolean res;
    if (keyboards->mKeyboardLayouts->IsEmpty(&res), res) {
        keyboards->mKeyboardLayouts->Add(NULL); // default layout
        keyboards->mCurrent = 0;
    }
    *result = (IObject*)keyboards;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode KeyboardLayoutDialogFragment::KeyboardLayoutLoader::OnStartLoading()
{
    AsyncTaskLoader::OnStartLoading();
    return ForceLoad();
}

ECode KeyboardLayoutDialogFragment::KeyboardLayoutLoader::OnStopLoading()
{
    AsyncTaskLoader::OnStopLoading();
    Boolean res;
    return CancelLoad(&res);
}

//===============================================================================
//                  KeyboardLayoutDialogFragment::DialogInterfaceOnClickListener
//===============================================================================

CAR_INTERFACE_IMPL(KeyboardLayoutDialogFragment::DialogInterfaceOnClickListener,
        Object, IDialogInterfaceOnClickListener)

KeyboardLayoutDialogFragment::DialogInterfaceOnClickListener::DialogInterfaceOnClickListener(
    /* [in] */ KeyboardLayoutDialogFragment* host,
    /* [in] */ Int32 id)
    : mHost(host)
    , mId(id)
{}

KeyboardLayoutDialogFragment::DialogInterfaceOnClickListener::~DialogInterfaceOnClickListener()
{}

ECode KeyboardLayoutDialogFragment::DialogInterfaceOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    switch (mId) {
        case 0:
            mHost->OnSetupLayoutsButtonClicked();
            break;
        case 1:
            mHost->OnKeyboardLayoutClicked(which);
            break;
    }
    return NOERROR;
}

//===============================================================================
//                  KeyboardLayoutDialogFragment
//===============================================================================

CAR_INTERFACE_IMPL_2(KeyboardLayoutDialogFragment, DialogFragment,
        IInputDeviceListener, ILoaderManagerLoaderCallbacks)

KeyboardLayoutDialogFragment::KeyboardLayoutDialogFragment()
    : mInputDeviceId(-1)
{}

KeyboardLayoutDialogFragment::KeyboardLayoutDialogFragment(
    /* [in] */ IInputDeviceIdentifier* inputDeviceIdentifier)
    : mInputDeviceId(-1)
{
    mInputDeviceIdentifier = inputDeviceIdentifier;
}

KeyboardLayoutDialogFragment::~KeyboardLayoutDialogFragment()
{}

ECode KeyboardLayoutDialogFragment::OnAttach(
    /* [in] */ IActivity* activity)
{
    DialogFragment::OnAttach(activity);

    AutoPtr<IContext> context;
    IContextWrapper::Probe(activity)->GetBaseContext((IContext**)&context);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::INPUT_SERVICE, (IInterface**)&obj);
    mIm = IInputManager::Probe(obj);
    mAdapter = new KeyboardLayoutAdapter(context);
    return NOERROR;
}

ECode KeyboardLayoutDialogFragment::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    DialogFragment::OnCreate(savedInstanceState);

    if (savedInstanceState != NULL) {
        AutoPtr<IParcelable> parcelable;
        savedInstanceState->GetParcelable(KEY_INPUT_DEVICE_IDENTIFIER,
                (IParcelable**)&parcelable);
        mInputDeviceIdentifier = IInputDeviceIdentifier::Probe(parcelable);
    }

    AutoPtr<ILoaderManager> lm;
    GetLoaderManager((ILoaderManager**)&lm);
    AutoPtr<ILoader> loader;
    return lm->InitLoader(0, NULL, this, (ILoader**)&loader);
}

ECode KeyboardLayoutDialogFragment::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    DialogFragment::OnSaveInstanceState(outState);
    return outState->PutParcelable(KEY_INPUT_DEVICE_IDENTIFIER,
            IParcelable::Probe(mInputDeviceIdentifier));
}

ECode KeyboardLayoutDialogFragment::OnCreateDialog(
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IContext> context = IContext::Probe(activity);
    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(context, (ILayoutInflater**)&inflater);
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(context, (IAlertDialogBuilder**)&builder);
    builder->SetTitle(R::string::keyboard_layout_dialog_title);
    AutoPtr<DialogInterfaceOnClickListener> listener =
            new DialogInterfaceOnClickListener(this, 0);
    builder->SetPositiveButton(
            R::string::keyboard_layout_dialog_setup_button, listener);
    AutoPtr<DialogInterfaceOnClickListener> listener1 =
            new DialogInterfaceOnClickListener(this, 1);
    builder->SetSingleChoiceItems(mAdapter, -1, listener1);
    AutoPtr<IView> view;
    inflater->Inflate(R::layout::keyboard_layout_dialog_switch_hint,
            NULL, (IView**)&view);
    builder->SetView(view);
    UpdateSwitchHintVisibility();
    AutoPtr<IAlertDialog> ad;
    builder->Create((IAlertDialog**)&ad);
    *dialog = IDialog::Probe(ad);
    REFCOUNT_ADD(*dialog);
    return NOERROR;
}

ECode KeyboardLayoutDialogFragment::OnResume()
{
    DialogFragment::OnResume();

    mIm->RegisterInputDeviceListener(this, NULL);

    String dsp;
    mInputDeviceIdentifier->GetDescriptor(&dsp);
    AutoPtr<IInputDevice> inputDevice;
    mIm->GetInputDeviceByDescriptor(dsp, (IInputDevice**)&inputDevice);
    if (inputDevice == NULL) {
        return Dismiss();
    }
    inputDevice->GetId(&mInputDeviceId);
    return NOERROR;
}

ECode KeyboardLayoutDialogFragment::OnPause()
{
    mIm->UnregisterInputDeviceListener(this);
    mInputDeviceId = -1;

    return DialogFragment::OnPause();
}

ECode KeyboardLayoutDialogFragment::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    DialogFragment::OnCancel(dialog);
    return Dismiss();
}

void KeyboardLayoutDialogFragment::OnSetupLayoutsButtonClicked()
{
    AutoPtr<IFragment> frag;
    GetTargetFragment((IFragment**)&frag);
    AutoPtr<IOnSetupKeyboardLayoutsListener> skl =
            IOnSetupKeyboardLayoutsListener::Probe(frag);
    skl->OnSetupKeyboardLayouts(mInputDeviceIdentifier);
}

ECode KeyboardLayoutDialogFragment::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    DialogFragment::OnActivityResult(requestCode, resultCode, data);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IFragmentManager> fragManager;
    activity->GetFragmentManager((IFragmentManager**)&fragManager);
    return Show(fragManager, String("layout"));
}

void KeyboardLayoutDialogFragment::OnKeyboardLayoutClicked(
    /* [in] */ Int32 which)
{
    Int32 count;
    mAdapter->GetCount(&count);
    if (which >= 0 && which < count) {
        AutoPtr<IInterface> obj;
        mAdapter->GetItem(which, (IInterface**)&obj);
        AutoPtr<IKeyboardLayout> keyboardLayout = IKeyboardLayout::Probe(obj);
        if (keyboardLayout != NULL) {
            String dsp;
            keyboardLayout->GetDescriptor(&dsp);
            mIm->SetCurrentKeyboardLayoutForInputDevice(mInputDeviceIdentifier, dsp);
        }
        Dismiss();
    }
}

ECode KeyboardLayoutDialogFragment::OnCreateLoader(
    /* [in] */ Int32 id,
    /* [in] */ IBundle* args,
    /* [out] */ ILoader** loader)
{
    VALIDATE_NOT_NULL(loader)

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IContext> context;
    IContextWrapper::Probe(activity)->GetBaseContext((IContext**)&context);

    AutoPtr<ILoader> ld = new KeyboardLayoutLoader(context, mInputDeviceIdentifier);

    *loader = ld;
    REFCOUNT_ADD(*loader);
    return NOERROR;
}

ECode KeyboardLayoutDialogFragment::OnLoadFinished(
    /* [in] */ ILoader* loader,
    /* [in] */ IInterface* data)
{
    mAdapter->Clear();

    AutoPtr<Keyboards> kbData = (Keyboards*)IObject::Probe(data);
    mAdapter->AddAll(ICollection::Probe(kbData->mKeyboardLayouts));
    mAdapter->SetCheckedItem(kbData->mCurrent);

    AutoPtr<IDialog> _dialog;
    GetDialog((IDialog**)&_dialog);
    AutoPtr<IAlertDialog> dialog = IAlertDialog::Probe(_dialog);
    if (dialog != NULL) {
        AutoPtr<IListView> listView;
        dialog->GetListView((IListView**)&listView);
        IAbsListView::Probe(listView)->SetItemChecked(kbData->mCurrent, TRUE);
    }
    UpdateSwitchHintVisibility();
    return NOERROR;
}

ECode KeyboardLayoutDialogFragment::OnLoaderReset(
    /* [in] */ ILoader* loader)
{
    mAdapter->Clear();
    UpdateSwitchHintVisibility();
    return NOERROR;
}

ECode KeyboardLayoutDialogFragment::OnInputDeviceAdded(
    /* [in] */ Int32 deviceId)
{
    return NOERROR;
}

ECode KeyboardLayoutDialogFragment::OnInputDeviceChanged(
    /* [in] */ Int32 deviceId)
{
    if (mInputDeviceId >= 0 && deviceId == mInputDeviceId) {
        AutoPtr<ILoaderManager> lm;
        GetLoaderManager((ILoaderManager**)&lm);
        AutoPtr<ILoader> loader;
        lm->RestartLoader(0, NULL, this, (ILoader**)&loader);
    }
    return NOERROR;
}

ECode KeyboardLayoutDialogFragment::OnInputDeviceRemoved(
    /* [in] */ Int32 deviceId)
{
    if (mInputDeviceId >= 0 && deviceId == mInputDeviceId) {
        Dismiss();
    }
    return NOERROR;
}

void KeyboardLayoutDialogFragment::UpdateSwitchHintVisibility()
{
    AutoPtr<IDialog> _dialog;
    GetDialog((IDialog**)&_dialog);
    AutoPtr<IAlertDialog> dialog = IAlertDialog::Probe(_dialog);
    if (dialog != NULL) {
        AutoPtr<IView> customPanel;
        _dialog->FindViewById(Elastos::Droid::R::id::customPanel,
                (IView**)&customPanel);
        Int32 count;
        mAdapter->GetCount(&count);
        customPanel->SetVisibility(count > 1 ? IView::VISIBLE : IView::GONE);
    }
}

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos
