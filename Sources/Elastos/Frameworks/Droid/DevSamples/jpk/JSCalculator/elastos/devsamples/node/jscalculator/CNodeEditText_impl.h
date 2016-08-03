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
    EditText::constructor(context, attrs, defStyle);
    return NOERROR;
}

ECode JSActName::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    ALOGD("CCalculatorEditText::constructor========begin====0====");

    ECode ec = NOERROR;

    EditText::constructor(context, attrs, defStyle);
    ALOGD("CCalculatorEditText::constructor========begin====1====");

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);
    ALOGD("CCalculatorEditText::constructor========begin====2====");

    JSEvtName::RegisterCustomControl(context, _this, (ICalculatorEditTextListener**)&mListener);
    ALOGD("CCalculatorEditText::constructor========begin====3====");

    if (mListener) {
        Int32 cnt = 0;
        ec = attrs->GetAttributeCount(&cnt);
        ALOGD("CCalculatorEditText::constructor========begin====0====AttributeCount:%d",cnt);
        IObject* temp_obj = IObject::Probe(attrs);
        String temp_name;
        ec = temp_obj->ToString(&temp_name);
        ALOGD("CCalculatorEditText::constructor========begin====0====ToString:%s",temp_name.string());

        ALOGD("CCalculatorEditText::constructor========begin====4.1====");
        ec = mListener->OnXXXCreateXXX(_this, context, attrs, defStyle);
        ALOGD("CCalculatorEditText::constructor========begin====5====");
    }
    else {
        ALOGD("CCalculatorEditText::constructor========begin====6====");
    }

    //return ec;
    return NOERROR;

    // EditText::constructor(context, attrs, defStyle);

    // AutoPtr<IAttributeSet> attbs = attrs;
    // AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
    //     const_cast<Int32 *>(R::styleable::CalculatorEditText),
    //     ArraySize(R::styleable::CalculatorEditText));

    // AutoPtr<ITypedArray> a;
    // context->ObtainStyledAttributes(attbs.Get(), attrIds.Get(), defStyle, 0, (ITypedArray**)&a);

    // Float textsize;
    // GetTextSize(&textsize);
    // a->GetDimension(
    //         R::styleable::CalculatorEditText_maxTextSize, textsize, &mMaximumTextSize);
    // a->GetDimension(
    //         R::styleable::CalculatorEditText_minTextSize, textsize, &mMinimumTextSize);
    // a->GetDimension(R::styleable::CalculatorEditText_stepTextSize,
    //         (mMaximumTextSize - mMinimumTextSize) / 3, &mStepTextSize);

    // a->Recycle();

    // SetCustomSelectionActionModeCallback(
    //     IActionModeCallback::Probe(NO_SELECTION_ACTION_MODE_CALLBACK));
    // Boolean flag = FALSE;
    // IsFocusable(&flag);
    // if (flag) {
    //     AutoPtr<IScrollingMovementMethodHelper> smm;
    //     CScrollingMovementMethodHelper::AcquireSingleton((IScrollingMovementMethodHelper**)&smm);
    //     AutoPtr<IMovementMethod> mmd;
    //     smm->GetInstance((IMovementMethod**)&mmd);
    //     SetMovementMethod(mmd.Get());
    // }
    // SetTextSize(ITypedValue::COMPLEX_UNIT_PX, mMaximumTextSize);
    // Int32 cpaddingBottom;
    // GetCompoundPaddingBottom(&cpaddingBottom);
    // Int32 cpaddingTop;
    // GetCompoundPaddingTop(&cpaddingTop);
    // Int32 lineHeight;
    // GetLineHeight(&lineHeight);

    // return SetMinHeight(lineHeight + cpaddingBottom + cpaddingTop);
}

ECode JSActName::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    ALOGD("CCalculatorEditText::OnTouchEvent========begin========");
    return NOERROR;

    // VALIDATE_NOT_NULL(result);

    // Int32 actionMasked;
    // event->GetActionMasked(&actionMasked);
    // if (actionMasked == IMotionEvent::ACTION_UP) {
    //     // Hack to prevent keyboard and insertion handle from showing.
    //     CancelLongPress();
    // }

    // return EditText::OnTouchEvent(event, result);
}

ECode JSActName::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    ALOGD("CCalculatorEditText::OnMeasure========begin========");
    return NOERROR;

    // EditText::OnMeasure(widthMeasureSpec, heightMeasureSpec);

    // Int32 paddingLeft;
    // GetPaddingLeft(&paddingLeft);
    // Int32 paddingRight;
    // GetPaddingRight(&paddingRight);
    // mWidthConstraint =
    //         View::MeasureSpec::GetSize(widthMeasureSpec) - paddingLeft - paddingRight;
    // AutoPtr<ICharSequence> cs;
    // GetText((ICharSequence**)&cs);
    // String str;
    // cs->ToString(&str);
    // Float vol;
    // GetVariableTextSize(str, &vol);
    // SetTextSize(ITypedValue::COMPLEX_UNIT_PX, vol);
    // return NOERROR;
}

ECode JSActName::OnSaveInstanceState(
    /* [out] */ IParcelable* result)
{
    ALOGD("CCalculatorEditText::OnMeasureOnSaveInstanceState========begin========");
    return NOERROR;

    // VALIDATE_NOT_NULL(result);
    // AutoPtr<IParcelable> pl = EditText::OnSaveInstanceState();

    // // EditText will freeze any text with a selection regardless of getFreezesText() ->
    // // return null to prevent any state from being preserved at the instance level.
    // result = NULL;
    // return NOERROR;
}

ECode JSActName::OnTextChanged(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 lengthBefore,
    /* [in] */ Int32 lengthAfter)
{
    String temp;
    text->ToString(&temp);
    ALOGD("CCalculatorEditText::OnTextChanged========begin========text:%s", temp.string());
    return NOERROR;

    // EditText::OnTextChanged(text, start, lengthBefore, lengthAfter);

    // Int32 textLength;
    // text->GetLength(&textLength);
    // Int32 istart;
    // GetSelectionStart(&istart);
    // Int32 end;
    // GetSelectionEnd(&end);
    // if (istart != textLength || end != textLength) {
    //     // Pin the selection to the end of the current text.
    //     SetSelection(textLength);
    // }
    // String str;
    // text->ToString(&str);
    // Float vol;
    // GetVariableTextSize(str, &vol);
    // return SetTextSize(ITypedValue::COMPLEX_UNIT_PX, vol);
}

ECode JSActName::SetTextSize(
    /* [in] */ Int32 unit,
    /* [in] */ Float size)
{
    ALOGD("CCalculatorEditText::SetTextSize========begin========");
    return NOERROR;

    // Float oldTextSize;
    // GetTextSize(&oldTextSize);
    // EditText::SetTextSize(unit, size);

    // Float fvol;
    // if (mOnTextSizeChangeListener != NULL && (GetTextSize(&fvol), fvol != oldTextSize)) {
    //     mOnTextSizeChangeListener->OnTextSizeChanged(this, oldTextSize);
    // }
    // return NOERROR;
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

ECode JSActName::GetCompoundPaddingTop(
    /* [out] */ Int32* result)
{
    ALOGD("CCalculatorEditText::GetCompoundPaddingTop========begin========");
    return NOERROR;

    // VALIDATE_NOT_NULL(result);

    // // Measure the top padding from the capital letter height of the text instead of the top,
    // // but don't remove more than the available top padding otherwise clipping may occur.
    // AutoPtr<ITextPaint> tp;
    // GetPaint((ITextPaint**)&tp);
    // IPaint::Probe(tp)->GetTextBounds(String("H"), 0, 1, mTempRect);
    // AutoPtr<IPaintFontMetricsInt> fontMetrics;
    // IPaint::Probe(tp)->GetFontMetricsInt((IPaintFontMetricsInt**)&fontMetrics);
    // Int32 height;
    // mTempRect->GetHeight(&height);
    // Int32 ascent;
    // fontMetrics->GetAscent(&ascent);
    // Int32 paddingOffset = -(ascent + height);
    // Int32 top;
    // EditText::GetCompoundPaddingTop(&top);
    // Int32 paddingTop;
    // GetPaddingTop(&paddingTop);
    // *result = top - Elastos::Core::Math::Min(paddingTop, paddingOffset);
    // return NOERROR;
}

ECode JSActName::GetCompoundPaddingBottom(
    /* [out] */ Int32* result)
{
    ALOGD("CCalculatorEditText::GetCompoundPaddingBottom========begin========");
    return NOERROR;

    // VALIDATE_NOT_NULL(result);

    // // Measure the bottom padding from the baseline of the text instead of the bottom, but don't
    // // remove more than the available bottom padding otherwise clipping may occur.
    // AutoPtr<ITextPaint> tp;
    // GetPaint((ITextPaint**)&tp);
    // AutoPtr<IPaintFontMetricsInt> fontMetrics;
    // IPaint::Probe(tp)->GetFontMetricsInt((IPaintFontMetricsInt**)&fontMetrics);
    // Int32 bottom;
    // GetPaddingBottom(&bottom);
    // Int32 descent;
    // fontMetrics->GetDescent(&descent);
    // Int32 vbottom;
    // EditText::GetCompoundPaddingBottom(&vbottom);
    // *result = vbottom - Elastos::Core::Math::Min(bottom, descent);
    // return NOERROR;
}

// } // namespace Calculator2
// } // namespace Droid
// } // namespace Elastos
} // namespace JSPkgName
} // namespace Node
} // namespace DevSamples
} // namespace Elastos