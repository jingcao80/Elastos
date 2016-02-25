/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.settings.inputmethod;

using Elastos::Droid::Settings::IR;

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IDialogFragment;
using Elastos::Droid::App::LoaderManager::ILoaderCallbacks;
using Elastos::Droid::Content::IAsyncTaskLoader;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::ILoader;
using Elastos::Droid::Hardware::Input::IInputDeviceIdentifier;
using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Droid::Hardware::Input::IKeyboardLayout;
using Elastos::Droid::Hardware::Input::InputManager::IInputDeviceListener;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::ICheckedTextView;
using Elastos::Droid::Widget::IRadioButton;
using Elastos::Droid::Widget::ITextView;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;

public class KeyboardLayoutDialogFragment extends DialogFragment
        implements InputDeviceListener, LoaderCallbacks<KeyboardLayoutDialogFragment.Keyboards> {
    private static const String KEY_INPUT_DEVICE_IDENTIFIER = "inputDeviceIdentifier";

    private InputDeviceIdentifier mInputDeviceIdentifier;
    private Int32 mInputDeviceId = -1;
    private InputManager mIm;
    private KeyboardLayoutAdapter mAdapter;

    public KeyboardLayoutDialogFragment() {
    }

    public KeyboardLayoutDialogFragment(InputDeviceIdentifier inputDeviceIdentifier) {
        mInputDeviceIdentifier = inputDeviceIdentifier;
    }

    //@Override
    CARAPI OnAttach(Activity activity) {
        super->OnAttach(activity);

        Context context = activity->GetBaseContext();
        mIm = (InputManager)context->GetSystemService(Context.INPUT_SERVICE);
        mAdapter = new KeyboardLayoutAdapter(context);
    }

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        if (savedInstanceState != NULL) {
            mInputDeviceIdentifier = savedInstanceState->GetParcelable(KEY_INPUT_DEVICE_IDENTIFIER);
        }

        GetLoaderManager()->InitLoader(0, NULL, this);
    }

    //@Override
    CARAPI OnSaveInstanceState(Bundle outState) {
        super->OnSaveInstanceState(outState);
        outState->PutParcelable(KEY_INPUT_DEVICE_IDENTIFIER, mInputDeviceIdentifier);
    }

    //@Override
    public Dialog OnCreateDialog(Bundle savedInstanceState) {
        Context context = GetActivity();
        LayoutInflater inflater = LayoutInflater->From(context);
        AlertDialog.Builder builder = new AlertDialog->Builder(context)
            .SetTitle(R::string::keyboard_layout_dialog_title)
            .SetPositiveButton(R::string::keyboard_layout_dialog_setup_button,
                    new DialogInterface->OnClickListener() {
                        //@Override
                        CARAPI OnClick(DialogInterface dialog, Int32 which) {
                            OnSetupLayoutsButtonClicked();
                        }
                    })
            .SetSingleChoiceItems(mAdapter, -1,
                    new DialogInterface->OnClickListener() {
                        //@Override
                        CARAPI OnClick(DialogInterface dialog, Int32 which) {
                            OnKeyboardLayoutClicked(which);
                        }
                    })
            .SetView(inflater->Inflate(R.layout.keyboard_layout_dialog_switch_hint, NULL));
        UpdateSwitchHintVisibility();
        return builder->Create();
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();

        mIm->RegisterInputDeviceListener(this, NULL);

        InputDevice inputDevice =
                mIm->GetInputDeviceByDescriptor(mInputDeviceIdentifier->GetDescriptor());
        if (inputDevice == NULL) {
            Dismiss();
            return;
        }
        mInputDeviceId = inputDevice->GetId();
    }

    //@Override
    CARAPI OnPause() {
        mIm->UnregisterInputDeviceListener(this);
        mInputDeviceId = -1;

        super->OnPause();
    }

    //@Override
    CARAPI OnCancel(DialogInterface dialog) {
        super->OnCancel(dialog);
        Dismiss();
    }

    private void OnSetupLayoutsButtonClicked() {
        ((OnSetupKeyboardLayoutsListener)GetTargetFragment()).OnSetupKeyboardLayouts(
                mInputDeviceIdentifier);
    }

    //@Override
    CARAPI OnActivityResult(Int32 requestCode, Int32 resultCode, Intent data) {
        super->OnActivityResult(requestCode, resultCode, data);
        Show(GetActivity()->GetFragmentManager(), "layout");
    }

    private void OnKeyboardLayoutClicked(Int32 which) {
        if (which >= 0 && which < mAdapter->GetCount()) {
            KeyboardLayout keyboardLayout = mAdapter->GetItem(which);
            if (keyboardLayout != NULL) {
                mIm->SetCurrentKeyboardLayoutForInputDevice(mInputDeviceIdentifier,
                        keyboardLayout->GetDescriptor());
            }
            Dismiss();
        }
    }

    //@Override
    public Loader<Keyboards> OnCreateLoader(Int32 id, Bundle args) {
        return new KeyboardLayoutLoader(GetActivity()->GetBaseContext(), mInputDeviceIdentifier);
    }

    //@Override
    CARAPI OnLoadFinished(Loader<Keyboards> loader, Keyboards data) {
        mAdapter->Clear();
        mAdapter->AddAll(data.keyboardLayouts);
        mAdapter->SetCheckedItem(data.current);
        AlertDialog dialog = (AlertDialog)GetDialog();
        if (dialog != NULL) {
            dialog->GetListView()->SetItemChecked(data.current, TRUE);
        }
        UpdateSwitchHintVisibility();
    }

    //@Override
    CARAPI OnLoaderReset(Loader<Keyboards> loader) {
        mAdapter->Clear();
        UpdateSwitchHintVisibility();
    }

    //@Override
    CARAPI OnInputDeviceAdded(Int32 deviceId) {
    }

    //@Override
    CARAPI OnInputDeviceChanged(Int32 deviceId) {
        if (mInputDeviceId >= 0 && deviceId == mInputDeviceId) {
            GetLoaderManager()->RestartLoader(0, NULL, this);
        }
    }

    //@Override
    CARAPI OnInputDeviceRemoved(Int32 deviceId) {
        if (mInputDeviceId >= 0 && deviceId == mInputDeviceId) {
            Dismiss();
        }
    }

    private void UpdateSwitchHintVisibility() {
        AlertDialog dialog = (AlertDialog)GetDialog();
        if (dialog != NULL) {
            View customPanel = dialog->FindViewById(R.id.customPanel);
            customPanel->SetVisibility(mAdapter->GetCount() > 1 ? View.VISIBLE : View.GONE);
        }
    }

    private static const class KeyboardLayoutAdapter extends ArrayAdapter<KeyboardLayout> {
        private final LayoutInflater mInflater;
        private Int32 mCheckedItem = -1;

        public KeyboardLayoutAdapter(Context context) {
            Super(context, R.layout.simple_list_item_2_single_choice);
            mInflater = (LayoutInflater)context->GetSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        CARAPI SetCheckedItem(Int32 position) {
            mCheckedItem = position;
        }

        //@Override
        public View GetView(Int32 position, View convertView, ViewGroup parent) {
            KeyboardLayout item = GetItem(position);
            String label, collection;
            if (item != NULL) {
                label = item->GetLabel();
                collection = item->GetCollection();
            } else {
                label = GetContext()->GetString(R::string::keyboard_layout_default_label);
                collection = "";
            }

            Boolean checked = (position == mCheckedItem);
            if (collection->IsEmpty()) {
                return InflateOneLine(convertView, parent, label, checked);
            } else {
                return InflateTwoLine(convertView, parent, label, collection, checked);
            }
        }

        private View InflateOneLine(View convertView, ViewGroup parent,
                String label, Boolean checked) {
            View view = convertView;
            if (view == NULL || IsTwoLine(view)) {
                view = mInflater->Inflate(
                        R.layout.simple_list_item_single_choice,
                        parent, FALSE);
                SetTwoLine(view, FALSE);
            }
            CheckedTextView headline = (CheckedTextView) view->FindViewById(android.R.id.text1);
            headline->SetText(label);
            headline->SetChecked(checked);
            return view;
        }

        private View InflateTwoLine(View convertView, ViewGroup parent,
                String label, String collection, Boolean checked) {
            View view = convertView;
            if (view == NULL || !IsTwoLine(view)) {
                view = mInflater->Inflate(
                        R.layout.simple_list_item_2_single_choice,
                        parent, FALSE);
                SetTwoLine(view, TRUE);
            }
            TextView headline = (TextView) view->FindViewById(android.R.id.text1);
            TextView subText = (TextView) view->FindViewById(android.R.id.text2);
            RadioButton radioButton =
                    (RadioButton)view->FindViewById(R.id.radio);
            headline->SetText(label);
            subText->SetText(collection);
            radioButton->SetChecked(checked);
            return view;
        }

        private static Boolean IsTwoLine(View view) {
            return view->GetTag() == Boolean.TRUE;
        }

        private static void SetTwoLine(View view, Boolean twoLine) {
            view->SetTag(Boolean->ValueOf(twoLine));
        }
    }

    private static const class KeyboardLayoutLoader extends AsyncTaskLoader<Keyboards> {
        private final InputDeviceIdentifier mInputDeviceIdentifier;

        public KeyboardLayoutLoader(Context context, InputDeviceIdentifier inputDeviceIdentifier) {
            Super(context);
            mInputDeviceIdentifier = inputDeviceIdentifier;
        }

        //@Override
        public Keyboards LoadInBackground() {
            Keyboards keyboards = new Keyboards();
            InputManager im = (InputManager)GetContext()->GetSystemService(Context.INPUT_SERVICE);
            String[] keyboardLayoutDescriptors = im->GetKeyboardLayoutsForInputDevice(
                    mInputDeviceIdentifier);
            for (String keyboardLayoutDescriptor : keyboardLayoutDescriptors) {
                KeyboardLayout keyboardLayout = im->GetKeyboardLayout(keyboardLayoutDescriptor);
                if (keyboardLayout != NULL) {
                    keyboards.keyboardLayouts->Add(keyboardLayout);
                }
            }
            Collections->Sort(keyboards.keyboardLayouts);

            String currentKeyboardLayoutDescriptor =
                    im->GetCurrentKeyboardLayoutForInputDevice(mInputDeviceIdentifier);
            if (currentKeyboardLayoutDescriptor != NULL) {
                final Int32 numKeyboardLayouts = keyboards.keyboardLayouts->Size();
                for (Int32 i = 0; i < numKeyboardLayouts; i++) {
                    if (keyboards.keyboardLayouts->Get(i).GetDescriptor()->Equals(
                            currentKeyboardLayoutDescriptor)) {
                        keyboards.current = i;
                        break;
                    }
                }
            }

            if (keyboards.keyboardLayouts->IsEmpty()) {
                keyboards.keyboardLayouts->Add(NULL); // default layout
                keyboards.current = 0;
            }
            return keyboards;
        }

        //@Override
        protected void OnStartLoading() {
            super->OnStartLoading();
            ForceLoad();
        }

        //@Override
        protected void OnStopLoading() {
            super->OnStopLoading();
            CancelLoad();
        }
    }

    public class Keyboards {
        public final ArrayList<KeyboardLayout> keyboardLayouts = new ArrayList<KeyboardLayout>();
        public Int32 current = -1;
    }

    public interface OnSetupKeyboardLayoutsListener {
        CARAPI OnSetupKeyboardLayouts(InputDeviceIdentifier mInputDeviceIdentifier);
    }
}