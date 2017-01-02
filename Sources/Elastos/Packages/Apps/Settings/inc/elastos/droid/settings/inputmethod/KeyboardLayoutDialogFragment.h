//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_SETTINGS_INPUTMETHOD_KEYBOARDLAYOUTDIALOGFRAGMENT_H__
#define __ELASTOS_DROID_SETTINGS_INPUTMETHOD_KEYBOARDLAYOUTDIALOGFRAGMENT_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/app/DialogFragment.h"
#include "elastos/droid/content/AsyncTaskLoader.h"
#include "elastos/droid/widget/ArrayAdapter.h"

using Elastos::Droid::App::DialogFragment;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::ILoaderManagerLoaderCallbacks;
using Elastos::Droid::Content::AsyncTaskLoader;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::ILoader;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Hardware::Input::IInputDeviceIdentifier;
using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Droid::Hardware::Input::IInputDeviceListener;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::Widget::ArrayAdapter;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

class KeyboardLayoutDialogFragment
    : public DialogFragment
{
    //LoaderCallbacks<KeyboardLayoutDialogFragment.Keyboards>
public:
    class InnerListener
        : public Object
        , public IInputDeviceListener
        , public ILoaderManagerLoaderCallbacks
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("KeyboardLayoutDialogFragment::InnerListener")

        InnerListener(
            /* [in] */ KeyboardLayoutDialogFragment* host);

        //@Override
        CARAPI OnInputDeviceAdded(
            /* [in] */ Int32 deviceId);

        //@Override
        CARAPI OnInputDeviceChanged(
            /* [in] */ Int32 deviceId);

        //@Override
        CARAPI OnInputDeviceRemoved(
            /* [in] */ Int32 deviceId);

        //@Override
        CARAPI OnCreateLoader(
            /* [in] */ Int32 id,
            /* [in] */ IBundle* args,
            /* [out] */ ILoader** loader);

        //@Override
        CARAPI OnLoadFinished(
            /* [in] */ ILoader* loader,
            /* [in] */ IInterface* data);

        //@Override
        CARAPI OnLoaderReset(
            /* [in] */ ILoader* loader);

    private:
        KeyboardLayoutDialogFragment* mHost;
    };

    class Keyboards
        : public Object
    {
    public:
        Keyboards();

        ~Keyboards();

    public:
        // ArrayList<KeyboardLayout> keyboardLayouts = new ArrayList<KeyboardLayout>();
        AutoPtr<IArrayList> mKeyboardLayouts;
        Int32 mCurrent;
    };

private:
    class KeyboardLayoutAdapter
        : public ArrayAdapter
    {
    public:
        TO_STRING_IMPL("KeyboardLayoutDialogFragment::KeyboardLayoutAdapter")

        KeyboardLayoutAdapter(
            /* [in] */ IContext* context);

        ~KeyboardLayoutAdapter();

        CARAPI SetCheckedItem(
            /* [in] */ Int32 position);

        //@Override
        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

    private:
        CARAPI_(AutoPtr<IView>) InflateOneLine(
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [in] */ const String& label,
            /* [in] */ Boolean checked);

        CARAPI_(AutoPtr<IView>) InflateTwoLine(
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [in] */ const String& label,
            /* [in] */ const String& collection,
            /* [in] */ Boolean checked);

        static CARAPI_(Boolean) IsTwoLine(
            /* [in] */ IView* view);

        static CARAPI_(void) SetTwoLine(
            /* [in] */ IView* view,
            /* [in] */ Boolean twoLine);

    private:
        AutoPtr<ILayoutInflater> mInflater;
        Int32 mCheckedItem;
    };

    class KeyboardLayoutLoader
        : public AsyncTaskLoader
    {
    public:
        TO_STRING_IMPL("KeyboardLayoutDialogFragment::KeyboardLayoutLoader")

        KeyboardLayoutLoader(
            /* [in] */ IContext* context,
            /* [in] */ IInputDeviceIdentifier* inputDeviceIdentifier);

        ~KeyboardLayoutLoader();

        //@Override
        CARAPI LoadInBackground(
            /* [out] */ IInterface** result);//Keyboards

    protected:
        //@Override
        CARAPI OnStartLoading();

        //@Override
        CARAPI OnStopLoading();

    private:
        AutoPtr<IInputDeviceIdentifier> mInputDeviceIdentifier;
    };

    class DialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        TO_STRING_IMPL("KeyboardLayoutDialogFragment::DialogInterfaceOnClickListener")

        DialogInterfaceOnClickListener(
            /* [in] */ KeyboardLayoutDialogFragment* host,
            /* [in] */ Int32 id);

        ~DialogInterfaceOnClickListener();

        //@Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        KeyboardLayoutDialogFragment* mHost;
        Int32 mId;
    };

public:
    TO_STRING_IMPL("KeyboardLayoutDialogFragment")

    KeyboardLayoutDialogFragment();

    ~KeyboardLayoutDialogFragment();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IInputDeviceIdentifier* inputDeviceIdentifier);

    //@Override
    CARAPI OnAttach(
        /* [in] */ IActivity* activity);

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    //@Override
    CARAPI OnCreateDialog(
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IDialog** dialog);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnCancel(
        /* [in] */ IDialogInterface* dialog);

    //@Override
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

    //@Override
    CARAPI OnCreateLoader(
        /* [in] */ Int32 id,
        /* [in] */ IBundle* args,
        /* [out] */ ILoader** loader);

    //@Override
    CARAPI OnLoadFinished(
        /* [in] */ ILoader* loader,
        /* [in] */ IInterface* data);

    //@Override
    CARAPI OnLoaderReset(
        /* [in] */ ILoader* loader);

    //@Override
    CARAPI OnInputDeviceAdded(
        /* [in] */ Int32 deviceId);

    //@Override
    CARAPI OnInputDeviceChanged(
        /* [in] */ Int32 deviceId);

    //@Override
    CARAPI OnInputDeviceRemoved(
        /* [in] */ Int32 deviceId);

private:
    CARAPI_(void) OnSetupLayoutsButtonClicked();

    CARAPI_(void) OnKeyboardLayoutClicked(
        /* [in] */ Int32 which);

    CARAPI_(void) UpdateSwitchHintVisibility();

private:
    static const String KEY_INPUT_DEVICE_IDENTIFIER;

    AutoPtr<IInputDeviceIdentifier> mInputDeviceIdentifier;
    Int32 mInputDeviceId;
    AutoPtr<IInputManager> mIm;
    AutoPtr<KeyboardLayoutAdapter> mAdapter;

    AutoPtr<InnerListener> mListener;
};

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_INPUTMETHOD_KEYBOARDLAYOUTDIALOGFRAGMENT_H__
