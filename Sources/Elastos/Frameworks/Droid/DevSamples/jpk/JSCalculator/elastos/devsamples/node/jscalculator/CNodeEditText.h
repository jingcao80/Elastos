// //#ifndef __CNodeActivity_H__
// //#define __CNodeActivity_H__

// #include "CNodeListener.h"

// #define JSActCarClassHead EE(JSPkgName,JSActName)
// #define JSActNameStr FF(JSActName)

// #include JSActCarClassHead

// #include <elastos/droid/app/Activity.h>
// #include <elastos/droid/os/Handler.h>

// using Elastos::Droid::App::Activity;
// using Elastos::Droid::View::IViewOnClickListener;

// using Elastos::Droid::Os::Handler;

// using Elastos::Droid::App::IDialog;
// using Elastos::Droid::Os::IHandler;
// using Elastos::Droid::Os::IMessage;

// namespace Elastos {
// namespace DevSamples {
// namespace Node {
// namespace JSPkgName {

// JSCarClass(JSActName)
//     , public EditText
//     , public IEditTextOne
// {
// public:
//     class MyHandler : public Handler
//     {
//     public:
//         JS_TO_STRING_IMPL(JSActName,MyHandler)

//         MyHandler(
//             /* [in] */ JSActName* host)
//             : mHost(host)
//         {};

//         virtual CARAPI HandleMessage(
//             /* [in] */ IMessage* msg);

//     private:
//         AutoPtr<JSActName> mHost;
//     };

// public:
//     CAR_INTERFACE_DECL()

//     CAR_OBJECT_DECL()

//     CARAPI constructor();

//     virtual CARAPI OnCreateContextMenu(
//         /* [in] */ IContextMenu* menu,
//         /* [in] */ IView* v,
//         /* [in] */ IContextMenuInfo* menuInfo);

//     CARAPI OnClickPopupWindow(
//         /* [in] */ IView* v);

// protected:
//     CARAPI OnCreate(
//         /* [in] */ IBundle* savedInstanceState);

//     CARAPI OnStart();

//     CARAPI OnResume();

//     CARAPI OnPause();

//     CARAPI OnStop();

//     CARAPI OnDestroy();

//     CARAPI_(AutoPtr<IDialog>) OnCreateDialog(
//     /* [in] */ Int32 id);

// private:
//     CARAPI OnActivityResult(
//         /* [in] */ Int32 requestCode,
//         /* [in] */ Int32 resultCode,
//         /* [in] */ IIntent *data);

// private:
//     static const String TAG;

//     String mPackageName;
//     String mActivityName;

//     AutoPtr<IHandler> mHandler;
//     AutoPtr<IActivityListener> mListener;
// };

// } // namespace JSPkgName
// } // namespace Node
// } // namespace DevSamples
// } // namespace Elastos

// //#endif // __H__


//--------------------------------------------------------------


//#ifndef _ELASTOS_DROID_CALCULATOR2_CALCULATOREDITTEXT_H__
//#define _ELASTOS_DROID_CALCULATOR2_CALCULATOREDITTEXT_H__

#include "CNodeListener.h"

#define JSActCarClassHead EE(JSPkgName,JSActName)
#define JSActNameStr FF(JSActName)

#include JSActCarClassHead

//#include "_Elastos.Droid.Calculator2.h"
#include <elastos/droid/widget/EditText.h>
//#include <elastos/core/Object.h>
//#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::EIID_IActionModeCallback;
using Elastos::Droid::View::IActionMode;
using Elastos::Droid::View::IActionModeCallback;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::EditText;

// namespace Elastos {
// namespace Droid {
// namespace Calculator2 {
namespace Elastos {
namespace DevSamples {
namespace Node {
namespace JSPkgName {

// class CalculatorEditText
//     : public EditText
//     , public ICalculatorEditText
JSCarClass(JSActName)
    , public EditText
    , public ICalculatorEditText
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

private:
    // class MyActionModeCallback
    //     : public Object
    //     , public IActionModeCallback
    // {
    // public:
    //     MyActionModeCallback();

    //     CAR_INTERFACE_DECL()

    //     CARAPI OnActionItemClicked(
    //         /* [in] */ IActionMode* mode,
    //         /* [in] */ IMenuItem* item,
    //         /* [out] */ Boolean* result);

    //     CARAPI OnCreateActionMode(
    //         /* [in] */ IActionMode* mode,
    //         /* [in] */ IMenu* menu,
    //         /* [out] */ Boolean* result);

    //     CARAPI OnDestroyActionMode(
    //         /* [in] */ IActionMode* mode);

    //     CARAPI OnPrepareActionMode(
    //         /* [in] */ IActionMode* mode,
    //         /* [in] */ IMenu* menu,
    //         /* [out] */ Boolean* result);
    // };

private:
    class SuperObject
        : public Object
        , public ICalculatorEditTextSuperObject
    {
    public:
        SuperObject(
            /* [in] */ JSActName* host)
            : mHost(host)
        {};

        CAR_INTERFACE_DECL()

        CARAPI SetTextSize(
            /* [in] */ Int32 unit,
            /* [in] */ Float size);

    private:
        AutoPtr<JSActName> mHost;
    };

public:
    CCalculatorEditText();

    virtual ~CCalculatorEditText();

    //CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI _constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI _OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI _OnSaveInstanceState(
        /* [out] */ IParcelable** result);
    CARAPI OnSaveInstanceState(
        /* [out] */ IParcelable** result);

    CARAPI _SetTextSize(
        /* [in] */ Int32 unit,
        /* [in] */ Float size);
    CARAPI SetTextSize(
        /* [in] */ Int32 unit,
        /* [in] */ Float size);

    CARAPI SetOnTextSizeChangeListener(
        /* [in] */  IOnTextSizeChangeListener* listener);

    CARAPI GetVariableTextSize(
        /* [in] */ const String& text,
        /* [out] */ Float* result);

    CARAPI _GetCompoundPaddingTop(
        /* [out] */ Int32* result);
    CARAPI GetCompoundPaddingTop(
        /* [out] */ Int32* result);

    CARAPI _GetCompoundPaddingBottom(
        /* [out] */ Int32* result);
    CARAPI GetCompoundPaddingBottom(
        /* [out] */ Int32* result);

protected:
    CARAPI _OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI _OnTextChanged(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 lengthBefore,
        /* [in] */ Int32 lengthAfter);
    CARAPI OnTextChanged(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 lengthBefore,
        /* [in] */ Int32 lengthAfter);

public:
    CARAPI GetSuperObject(
        /* [out] */ ICalculatorEditTextSuperObject** ppSuperObject);

private:
        AutoPtr<ICalculatorEditTextListener> mListener;

        AutoPtr<ICalculatorEditTextSuperObject> mSuperObject;

//     static CARAPI_(AutoPtr<MyActionModeCallback>) InitCallback();

// private:
//     static const AutoPtr<MyActionModeCallback> NO_SELECTION_ACTION_MODE_CALLBACK;
//     Float mMaximumTextSize;
//     Float mMinimumTextSize;
//     Float mStepTextSize;

//     // Temporary objects for use in layout methods.
//     AutoPtr<IPaint> mTempPaint;
//     AutoPtr<IRect> mTempRect;

//     Int32 mWidthConstraint;
//     AutoPtr<IOnTextSizeChangeListener> mOnTextSizeChangeListener;
};

// } // namespace Calculator2
// } // namespace Droid
// } // namespace Elastos
} // namespace JSPkgName
} // namespace Node
} // namespace DevSamples
} // namespace Elastos

//#endif // _ELASTOS_DROID_CALCULATOR2_CALCULATOREDITTEXT_H__



