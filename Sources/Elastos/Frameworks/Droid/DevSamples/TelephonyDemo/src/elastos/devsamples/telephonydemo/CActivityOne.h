
#ifndef __Elastos_DevSamples_TelephonyDemo_CActivityOne_H__
#define __Elastos_DevSamples_TelephonyDemo_CActivityOne_H__

#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.Widget.h>
#include <elastos/droid/app/Activity.h>
#include "_Elastos_DevSamples_TelephonyDemo_CActivityOne.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::Animation::IAnimation;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IAdapterViewOnItemLongClickListener;
using Elastos::Droid::Widget::IRadioGroupOnCheckedChangeListener;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IPopupWindow;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IRadioButton;
using Elastos::Droid::Widget::IRadioGroup;
using Elastos::Droid::Widget::IToggleButton;

namespace Elastos {
namespace DevSamples {
namespace TelephonyDemo {

CarClass(CActivityOne)
    , public Activity
    , public IActivityOne
{
public:
    class MyListener
        : public Object
        , public IViewOnTouchListener
        , public IViewOnKeyListener
        , public IViewOnClickListener
        , public IAdapterViewOnItemClickListener
        , public IAdapterViewOnItemLongClickListener
        , public IDialogInterfaceOnClickListener
        , public IRadioGroupOnCheckedChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyListener(
            /* [in] */ CActivityOne* host);

        ~MyListener();

        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

        CARAPI OnKey(
            /* [in] */ IView * v,
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

        CARAPI OnClick(
            /* [in] */ IView* v);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

        CARAPI OnItemLongClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id,
            /* [out] */ Boolean* result);

        CARAPI OnCheckedChanged(
            /* [in] */ IRadioGroup* group,
            /* [in] */ Int32 checkedId);

    private:
        CActivityOne* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    virtual CARAPI OnCreateContextMenu(
        /* [in] */ IContextMenu* menu,
        /* [in] */ IView* v,
        /* [in] */ IContextMenuInfo* menuInfo);

    CARAPI OnClickPopupWindow(
        /* [in] */ IView* v);

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

    CARAPI_(AutoPtr<IDialog>) OnCreateDialog(
    /* [in] */ Int32 id);

    CARAPI ShowAlert(
        /* [in] */ const String& info);

    CARAPI OnTestPowerManager();

    CARAPI OnTestConnectivityManager();

    CARAPI OnTestTelephony();

private:
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

    CARAPI CreateNavigationBar();

private:
    AutoPtr<IDrawable> mDrawable1;
    AutoPtr<IDrawable> mDrawable2;

    AutoPtr<IView> mButton1;
    AutoPtr<IView> mButton2;
    AutoPtr<IView> mButton3;

    AutoPtr<IAnimation> mAlphaAnimation;
    AutoPtr<IAnimation> mScaleAnimation;
    AutoPtr<IAnimation> mRotateAnimation;
    AutoPtr<IAnimation> mTranslateAnimation;

    AutoPtr<IView> mAnimationButton;
    AutoPtr<IView> mDialogButton;
    AutoPtr<IView> mPopupWindowButton;
    AutoPtr<IView> mPowerManagerButton;
    AutoPtr<IView> mConnectivityManagerButton;
    AutoPtr<IView> mContent;

    AutoPtr<IView> mBack;
    AutoPtr<IView> mHome;
    AutoPtr<IView> mMenu;

    AutoPtr<IPopupWindow> mPopupWindow;

    AutoPtr<ICheckBox> mAndroidCheckBox;
    AutoPtr<ICheckBox> mIosCheckBox;
    AutoPtr<ICheckBox> mUbuntuCheckBox;

    AutoPtr<IEditText> mEditText;
    AutoPtr<ITextView> mTextView;
    AutoPtr<IListView> mListView;

    AutoPtr<IRadioGroup> mRadioGroup;
    AutoPtr<IRadioButton> mRadioButtonMale;
    AutoPtr<IRadioButton> mRadioButtonFemale;
    AutoPtr<IRadioButton> mRadioButtonSecret;

    AutoPtr<IToggleButton> mToggleButton1;
    AutoPtr<IToggleButton> mToggleButton2;


    AutoPtr<IView> mTelephonyTestButton;
};

} // namespace TelephonyDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_TextViewDemo_CActivityOne_H__
