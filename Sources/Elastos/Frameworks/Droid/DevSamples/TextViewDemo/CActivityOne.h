
#ifndef __CACTIVITYONE_H__
#define __CACTIVITYONE_H__

#include "elastos/droid/app/Activity.h"
#include "_CActivityOne.h"

using Elastos::ElRefBase;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::View::IViewOnClickListener;
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
using Elastos::Droid::Widget::IRadioGroup;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace TextViewDemo {


class CActivityOne : public Activity
{
public:
    class MyListener
    : public IViewOnTouchListener
    , public IViewOnKeyListener
    , public IAdapterViewOnItemClickListener
    , public IAdapterViewOnItemLongClickListener
    , public IViewOnClickListener
    , public IDialogInterfaceOnClickListener
    , public IRadioGroupOnCheckedChangeListener
    , public ElRefBase
    {
    public:
        MyListener(
            /* [in] */ CActivityOne* host);

        ~MyListener();

        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

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
            /* [in] */  IRadioGroup* group,
            /* [in] */  Int32 checkedId);

    private:
        AutoPtr<CActivityOne> mHost;
    };

public:
    CActivityOne();
    ~CActivityOne();

    virtual CARAPI OnCreateContextMenu(
        /* [in] */ IContextMenu* menu,
        /* [in] */ IView* v,
        /* [in] */ IContextMenuInfo* menuInfo);

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

    CARAPI OnCreatePopupWindow();

    CARAPI OnTestPowerManager();

    CARAPI OnTestConnectivityManager();

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

    AutoPtr<IEditText> mEditText;
    AutoPtr<ITextView> mTextView;
    AutoPtr<IListView> mListView;
};

} // namespace TextViewDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CACTIVITYONE_H__
