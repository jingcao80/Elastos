// #include "CNodeActivity.h"

// #include <elastos/utility/logging/Logger.h>

// using Elastos::Droid::App::EIID_IActivity;
// using Elastos::Droid::App::IAlertDialog;
// using Elastos::Utility::Logging::Logger;

// namespace Elastos {
// namespace DevSamples {
// namespace Node {
// namespace JSPkgName {

// EXTERN NodeBridge* g_pNodeBridge;
// EXTERN NodeBridge** g_ppNodeBridge;

// EXTERN IHandler* myHandler;

// const String JSActName::TAG(JSActNameStr);

// static const String DBG_TAG(JSActNameStr);

// ECode JSActName::MyHandler::HandleMessage(
//     /* [in] */ IMessage* msg)
// {
//     AutoPtr<IMessageListener> listener = IMessageListener::Probe(mHost->mListener);
//     return listener->OnHandleMessage(mHost, msg);
// }

// CAR_INTERFACE_IMPL(JSActName, Activity, IActivityOne)

// JS_CAR_OBJECT_IMPL(JSActName)

// ECode JSActName::constructor()
// {
//     Logger::I(TAG, " >> constructor()");

//     return Activity::constructor();
// }

// ECode JSActName::OnCreate(
//     /* [in] */ IBundle* savedInstanceState)
// {
//     Logger::D(TAG, "OnCreate()-----");
//     Activity::OnCreate(savedInstanceState);

//     mHandler = new MyHandler(this);

//     myHandler = mHandler;

//     String _pkgPath = String("/data/temp/node/bin/");
//     String _pkgName = String(JSPkgNameStr);
//     String _nspName = String("Elastos.DevSamples.Node.");

//     mPackageName = _nspName + _pkgName;
//     mActivityName = String(JSActNameStr);

//     String _helperEcoName = _pkgPath + mPackageName + String(".Helper.eco");
//     String _helperClsName = mPackageName + String(".") + mActivityName + String("Helper");

//     AutoPtr<IInterface> helper;
//     ECode ec = JSEvtName::Require(_helperEcoName, _helperClsName, (IInterface**)&helper);

//     if (FAILED(ec)) {
//         ALOGD("OnCreate========create Helper failed!======nodejs module will be used");
//         AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);
//         JSEvtName::RegisterActivity(mPackageName, mActivityName, _this, (IActivityListener**)&mListener, mHandler.Get());
//     }
//     else {
//         ALOGD("OnCreate========create Helper success!======C++ epk will be used");
//         mListener = IActivityListener::Probe(helper);
//     }

//     return mListener->OnCreate(this, savedInstanceState);
// }

// ECode JSActName::OnStart()
// {
//     Logger::I(DBG_TAG, " >> OnStart()");
//     ECode ec = Activity::OnStart();
//     mListener->OnStart(this);
//     return ec;
// }

// ECode JSActName::OnResume()
// {
//     Logger::I(DBG_TAG, " >> OnResume()");
//     ECode ec = Activity::OnResume();
//     mListener->OnResume(this);
//     return ec;
// }

// ECode JSActName::OnPause()
// {
//     Logger::I(DBG_TAG, " >> OnPause()");
//     ECode ec = Activity::OnPause();
//     mListener->OnPause(this);
//     return ec;
// }

// ECode JSActName::OnStop()
// {
//     Logger::I(DBG_TAG, " >> OnStop()");
//     ECode ec = Activity::OnStop();
//     mListener->OnStop(this);
//     return ec;
// }

// ECode JSActName::OnDestroy()
// {
//     Logger::I(DBG_TAG, " >> OnDestroy()");
//     ECode ec = Activity::OnDestroy();
//     mListener->OnDestroy(this);
//     return ec;
// }

// ECode JSActName::OnActivityResult(
//     /* [in] */ Int32 requestCode,
//     /* [in] */ Int32 resultCode,
//     /* [in] */ IIntent *data)
// {
//     return mListener->OnActivityResult(this, requestCode, resultCode, data);
// }

// AutoPtr<IDialog> JSActName::OnCreateDialog(
//    /* [in] */ Int32 id)
// {
//     AutoPtr<IDialog> dlg;

//     AutoPtr<IInterface> alertDialog;
//     mListener->OnCreateDialog(this, id, (IInterface**)&alertDialog);
//     dlg = IDialog::Probe(alertDialog.Get());

//     return dlg;
// }

// ECode JSActName::OnCreateContextMenu(
//     /* [in] */ IContextMenu* menu,
//     /* [in] */ IView* v,
//     /* [in] */ IContextMenuInfo* menuInfo)
// {
//     return mListener->OnCreateContextMenu(this, menu, v, menuInfo);
// }

// ECode JSActName::OnClickPopupWindow(
//     /* [in] */ IView* view)
// {
//     return NOERROR;
// }

// } // namespace JSPkgName
// } // namespace Node
// } // namespace DevSamples
// } // namespace Elastos


//-----------------------------------------------


#include "CNodeEditText.h"

#include <elastos/utility/logging/Logger.h>

//#include "elastos/droid/calculator2/CalculatorEditText.h"
#include "R.h"
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Text.h>
#include <elastos/core/Math.h>

using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IPaintFontMetricsInt;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Text::CTextPaint;
using Elastos::Droid::Text::Method::CScrollingMovementMethodHelper;
using Elastos::Droid::Text::Method::IScrollingMovementMethodHelper;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Core::Math;

using Elastos::Utility::Logging::Logger;

// namespace Elastos {
// namespace Droid {
// namespace Calculator2 {
namespace Elastos {
namespace DevSamples {
namespace Node {
namespace JSPkgName {

EXTERN NodeBridge* g_pNodeBridge;
EXTERN NodeBridge** g_ppNodeBridge;

EXTERN IHandler* myHandler;

//const String JSActName::TAG(JSActNameStr);

//static const String DBG_TAG(JSActNameStr);

CAR_INTERFACE_IMPL(JSActName, EditText, ICalculatorEditText)

JS_CAR_OBJECT_IMPL(JSActName)

CAR_INTERFACE_IMPL(JSActName::SuperObject, Object, ICalculatorEditTextSuperObject)

//----------------------------------------------------------------
//           JSActName::MyActionModeCallback
//----------------------------------------------------------------
// JSActName::MyActionModeCallback::MyActionModeCallback()
// {}

// CAR_INTERFACE_IMPL(JSActName::MyActionModeCallback, Object, IActionModeCallback)

// ECode JSActName::MyActionModeCallback::OnActionItemClicked(
//     /* [in] */ IActionMode* mode,
//     /* [in] */ IMenuItem* item,
//     /* [out] */ Boolean* result)
// {
//     VALIDATE_NOT_NULL(result);
//     *result = FALSE;
//     return NOERROR;
// }

// ECode JSActName::MyActionModeCallback::OnCreateActionMode(
//     /* [in] */ IActionMode* mode,
//     /* [in] */ IMenu* menu,
//     /* [out] */ Boolean* result)
// {
//     VALIDATE_NOT_NULL(result);
//     *result = FALSE;
//     return NOERROR;
// }

// ECode JSActName::MyActionModeCallback::OnDestroyActionMode(
//     /* [in] */ IActionMode* mode)
// {
//     return NOERROR;
// }

// ECode JSActName::MyActionModeCallback::OnPrepareActionMode(
//     /* [in] */ IActionMode* mode,
//     /* [in] */ IMenu* menu,
//     /* [out] */ Boolean* result)
// {
//     VALIDATE_NOT_NULL(result);
//     *result = FALSE;
//     return NOERROR;
// }

//----------------------------------------------------------------
//           CalculatorEditText
//----------------------------------------------------------------

// AutoPtr<JSActName::MyActionModeCallback> JSActName::InitCallback()
// {
//     return new MyActionModeCallback();
// }
// const AutoPtr<JSActName::MyActionModeCallback> JSActName::NO_SELECTION_ACTION_MODE_CALLBACK = InitCallback();

JSActName::CCalculatorEditText()
    // : mWidthConstraint(-1)
{
    ALOGD("CCalculatorEditText::CCalculatorEditText========begin========");

    // CTextPaint::New((IPaint**)&mTempPaint);
    // CRect::New((IRect**)&mTempRect);
}

JSActName::~CCalculatorEditText()
{}

//CAR_INTERFACE_IMPL(CalculatorEditText, EditText, ICalculatorEditText)

ECode JSActName::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode JSActName::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode JSActName::_constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return EditText::constructor(context, attrs, defStyle);
}
ECode JSActName::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    ECode ec = NOERROR;

    SuperObject* superObject = new SuperObject(this);
    mSuperObject = (ICalculatorEditTextSuperObject*)superObject->Probe(EIID_ICalculatorEditTextSuperObject);

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);
    JSEvtName::RegisterCustomControl(context, _this, (ICalculatorEditTextListener**)&mListener);

    if (mListener) {
        ec = mListener->OnCreate(_this, context, attrs, defStyle);
    }
    else {
        ec = EditText::constructor(context, attrs, defStyle);
    }

    return ec;
}

ECode JSActName::_OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    return EditText::OnTouchEvent(event, result);
}
ECode JSActName::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    ALOGD("CCalculatorEditText::OnTouchEvent========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->OnTouchEvent(_this, event, result);
    }
    else {
        ec = EditText::OnTouchEvent(event, result);
    }

    return ec;
}

ECode JSActName::_OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    return EditText::OnMeasure(widthMeasureSpec, heightMeasureSpec);
}
ECode JSActName::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    ALOGD("CCalculatorEditText::OnMeasure========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->OnMeasure(_this, widthMeasureSpec, heightMeasureSpec);
    }
    else {
        ec = EditText::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    }

    return ec;
}

ECode JSActName::_OnSaveInstanceState(
    /* [out] */ IParcelable** result)
{
    *result = EditText::OnSaveInstanceState();
    return NOERROR;
}
ECode JSActName::OnSaveInstanceState(
    /* [out] */ IParcelable** result)
{
    ALOGD("CCalculatorEditText::OnMeasureOnSaveInstanceState========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->OnSaveInstanceState(_this, result);
    }
    else {
        *result = EditText::OnSaveInstanceState();
    }

    return ec;
}

ECode JSActName::_OnTextChanged(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 lengthBefore,
    /* [in] */ Int32 lengthAfter)
{
    return EditText::OnTextChanged(text, start, lengthBefore, lengthAfter);
}
ECode JSActName::OnTextChanged(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 lengthBefore,
    /* [in] */ Int32 lengthAfter)
{
    ALOGD("CCalculatorEditText::OnTextChanged========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->OnTextChanged(_this, text, start, lengthBefore, lengthAfter);
    }
    else {
        ec = EditText::OnTextChanged(text, start, lengthBefore, lengthAfter);
    }

    return ec;
}

ECode JSActName::_SetTextSize(
    /* [in] */ Int32 unit,
    /* [in] */ Float size)
{
    return EditText::SetTextSize(unit, size);
}
ECode JSActName::SetTextSize(
    /* [in] */ Int32 unit,
    /* [in] */ Float size)
{
    ALOGD("CCalculatorEditText::SetTextSize========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->SetTextSize(_this, unit, size);
    }
    else {
        ec = EditText::SetTextSize(unit, size);
    }

    return ec;
}

//to be deleted
ECode JSActName::SetOnTextSizeChangeListener(
    /* [in] */ IOnTextSizeChangeListener* listener)
{
    ALOGD("CCalculatorEditText::SetOnTextSizeChangeListener========begin========");
    return NOERROR;

    // mOnTextSizeChangeListener = listener;
    // return NOERROR;
}

//to be deleted
ECode JSActName::GetVariableTextSize(
    /* [in] */ const String& text,
    /* [out] */ Float* result)
{
    ALOGD("CCalculatorEditText::SetOnTextSizeChangeListener========begin========");
    return NOERROR;

    // VALIDATE_NOT_NULL(result);

    // if (mWidthConstraint < 0 || mMaximumTextSize <= mMinimumTextSize) {
    //     // Not measured, bail early.
    //     Float size;
    //     GetTextSize(&size);
    //     *result = size;
    //     return NOERROR;
    // }

    // // Capture current paint state.
    // AutoPtr<ITextPaint> paint;
    // GetPaint((ITextPaint**)&paint);
    // mTempPaint->Set(IPaint::Probe(paint));

    // // Step through increasing text sizes until the text would no longer fit.
    // Float lastFitTextSize = mMinimumTextSize;
    // Float width;
    // while (lastFitTextSize < mMaximumTextSize) {
    //     Float nextSize = Elastos::Core::Math::Min(lastFitTextSize + mStepTextSize, mMaximumTextSize);
    //     mTempPaint->SetTextSize(nextSize);

    //     mTempPaint->MeasureText(text, &width);
    //     if (width > mWidthConstraint) {
    //         break;
    //     }
    //     else {
    //         lastFitTextSize = nextSize;
    //     }
    // }

    // *result = lastFitTextSize;
    // return NOERROR;
}

ECode JSActName::_GetCompoundPaddingTop(
    /* [out] */ Int32* result)
{
    return EditText::GetCompoundPaddingTop(result);
}
ECode JSActName::GetCompoundPaddingTop(
    /* [out] */ Int32* result)
{
    ALOGD("CCalculatorEditText::GetCompoundPaddingTop========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->GetCompoundPaddingTop(_this, result);
    }
    else {
        ec = EditText::GetCompoundPaddingTop(result);
    }

    return ec;
}

ECode JSActName::_GetCompoundPaddingBottom(
    /* [out] */ Int32* result)
{
    return EditText::GetCompoundPaddingBottom(result);
}
ECode JSActName::GetCompoundPaddingBottom(
    /* [out] */ Int32* result)
{
    ALOGD("CCalculatorEditText::GetCompoundPaddingBottom========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->GetCompoundPaddingBottom(_this, result);
    }
    else {
        ec = EditText::GetCompoundPaddingBottom(result);
    }

    return ec;
}

ECode JSActName::GetSuperObject(
    /* [out] */ ICalculatorEditTextSuperObject ** ppSuperObject)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    assert(ppSuperObject != NULL);
    *ppSuperObject = mSuperObject;

    REFCOUNT_ADD(*ppSuperObject);
    return NOERROR;
}

ECode JSActName::SuperObject::SetTextSize(
    /* [in] */ Int32 unit,
    /* [in] */ Float size)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);

    mHost->_SetTextSize(unit, size);

    return NOERROR;
}

// } // namespace Calculator2
// } // namespace Droid
// } // namespace Elastos
} // namespace JSPkgName
} // namespace Node
} // namespace DevSamples
} // namespace Elastos