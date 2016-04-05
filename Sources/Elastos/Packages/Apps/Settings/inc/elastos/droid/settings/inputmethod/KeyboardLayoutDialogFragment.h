
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
    , public IInputDeviceListener
    , public ILoaderManagerLoaderCallbacks
{
    //LoaderCallbacks<KeyboardLayoutDialogFragment.Keyboards>
public:
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
    CAR_INTERFACE_DECL();

    KeyboardLayoutDialogFragment();

    KeyboardLayoutDialogFragment(
        /* [in] */ IInputDeviceIdentifier* inputDeviceIdentifier);

    ~KeyboardLayoutDialogFragment();

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
};

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_INPUTMETHOD_KEYBOARDLAYOUTDIALOGFRAGMENT_H__
