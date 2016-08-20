#include "CNodeEditText.h"

#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace JSPkgName {

EXTERN NodeBridge* g_pNodeBridge;
EXTERN NodeBridge** g_ppNodeBridge;

EXTERN IHandler* myHandler;

CAR_INTERFACE_IMPL(JSActName, EditText, ICalculatorEditText)

JS_CAR_OBJECT_IMPL(JSActName)

CAR_INTERFACE_IMPL(JSActName::SuperObject, Object, ICalculatorEditTextSuperObject)

JSActName::CCalculatorEditText(){}
JSActName::~CCalculatorEditText(){}

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
    ALOGD("CCalculatorEditText::constructor========begin========");

    ECode ec = NOERROR;

    SuperObject* superObject = new SuperObject(this);
    mSuperObject = (ICalculatorEditTextSuperObject*)superObject->Probe(EIID_ICalculatorEditTextSuperObject);

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);
    JSEvtName::RegisterCalculatorEditText(context, _this, (ICalculatorEditTextListener**)&mListener);

    if (mListener) {
        ec = mListener->OnCreate(_this, context, attrs, defStyle);
    }
    else {
        ec = EditText::constructor(context, attrs, defStyle);
    }

    ALOGD("CCalculatorEditText::constructor========end========");
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

} // namespace JSPkgName
} // namespace Node
} // namespace DevSamples
} // namespace Elastos