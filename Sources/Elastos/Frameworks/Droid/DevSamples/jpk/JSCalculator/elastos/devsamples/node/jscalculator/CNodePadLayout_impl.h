#include "CNodePadLayout.h"

#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace JSPkgName {

EXTERN NodeBridge* g_pNodeBridge;
EXTERN NodeBridge** g_ppNodeBridge;

EXTERN IHandler* myHandler;

CAR_INTERFACE_IMPL(JSActName, ViewGroup, ICalculatorPadLayout)

JS_CAR_OBJECT_IMPL(JSActName)

CAR_INTERFACE_IMPL(JSActName::SuperObject, Object, ICalculatorPadLayoutSuperObject)

JSActName::CCalculatorPadLayout(){}
JSActName::~CCalculatorPadLayout(){}

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
    return ViewGroup::constructor(context, attrs, defStyle);
}
ECode JSActName::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    ALOGD("CCalculatorPadLayout::constructor========begin========");

    ECode ec = NOERROR;

    SuperObject* superObject = new SuperObject(this);
    mSuperObject = (ICalculatorPadLayoutSuperObject*)superObject->Probe(EIID_ICalculatorPadLayoutSuperObject);

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);
    JSEvtName::RegisterCalculatorPadLayout(context, _this, (ICalculatorPadLayoutListener**)&mListener);

    if (mListener) {
        ec = mListener->OnCreate(_this, context, attrs, defStyle);
    }
    else {
        ec = ViewGroup::constructor(context, attrs, defStyle);
    }

    ALOGD("CCalculatorPadLayout::constructor========end========");
    return NOERROR;
}

//----------------------------------------------

ECode JSActName::_ShouldDelayChildPressedState(
        /* [out] */ Boolean* result)
{
    return ViewGroup::ShouldDelayChildPressedState(result);
}
ECode JSActName::ShouldDelayChildPressedState(
        /* [out] */ Boolean* result)
{
    ALOGD("CCalculatorPadLayout::ShouldDelayChildPressedState========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->ShouldDelayChildPressedState(_this, result);
    }
    else {
        ec = ViewGroup::ShouldDelayChildPressedState(result);
    }

    return NOERROR;
}

ECode JSActName::_GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** result)
{
    return ViewGroup::GenerateLayoutParams(attrs, result);
}
ECode JSActName::GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** result)
{
    ALOGD("CCalculatorPadLayout::GenerateLayoutParams========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->GenerateLayoutParams(_this, attrs, result);
    }
    else {
        ec = ViewGroup::GenerateLayoutParams(attrs, result);
    }

    return NOERROR;
}

ECode JSActName::_GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** result)
{
    return ViewGroup::GenerateDefaultLayoutParams(result);
}
ECode JSActName::GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** result)
{
    ALOGD("CCalculatorPadLayout::GenerateDefaultLayoutParams========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->GenerateDefaultLayoutParams(_this, result);
    }
    else {
        ec = ViewGroup::GenerateDefaultLayoutParams(result);
    }

    return NOERROR;
}

ECode JSActName::_OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec)
{
    return ViewGroup::OnMeasure(widthMeasureSpec, heightMeasureSpec);
}
ECode JSActName::OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec)
{
    ALOGD("CCalculatorPadLayout::OnMeasure========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->OnMeasure(_this, widthMeasureSpec, heightMeasureSpec);
    }
    else {
        ec = ViewGroup::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    }

    return NOERROR;
}

ECode JSActName::_OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom)
{
    //return ViewGroup::OnLayout(changed, left, top, right, bottom);
    ALOGD("CCalculatorPadLayout::_OnLayout====error:ViewGroup::_OnLayout is pure virtual!====");
    //assert(0);
    return NOERROR;
}
ECode JSActName::OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom)
{
    ALOGD("CCalculatorPadLayout::OnLayout========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->OnLayout(_this, changed, left, top, right, bottom);
    }
    else {
        //ec = ViewGroup::OnLayout(changed, left, top, right, bottom);
        ALOGD("CCalculatorPadLayout::OnLayout====error:ViewGroup::OnLayout is pure virtual!====");
        //Add C++ Code for OnLayout here
        assert(0);
    }

    return NOERROR;
}

ECode JSActName::_GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p,
        /* [out] */ IViewGroupLayoutParams** result)
{
    *result =  ViewGroup::GenerateLayoutParams(p);
    return NOERROR;
}
// ECode JSActName::GenerateLayoutParams(
//         /* [in] */ IViewGroupLayoutParams* p,
//         /* [out] */ IViewGroupLayoutParams** result)
// {
//     ALOGD("CCalculatorPadLayout::GenerateLayoutParams========begin========");

//     ECode ec = NOERROR;

//     AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

//     if (mListener) {
//         ec = mListener->GenerateLayoutParams_0(_this, p, result);
//     }
//     else {
//         *result = ViewGroup::GenerateLayoutParams(p);
//     }

//     return ec;
// }
AutoPtr<IViewGroupLayoutParams> JSActName::GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p)
{
    ALOGD("CCalculatorPadLayout::GenerateLayoutParams====begin====group");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    AutoPtr<IViewGroupLayoutParams> params;
    if (mListener) {
        ec = mListener->GenerateLayoutParams(_this, p, (IViewGroupLayoutParams**)&params);
    }
    else {
        params = ViewGroup::GenerateLayoutParams(p);
    }

    return params;
}

ECode JSActName::_CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p,
        /* [out] */ Boolean* result)
{
    *result = ViewGroup::CheckLayoutParams(p);
    return NOERROR;
}
ECode  JSActName::CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p,
        /* [out] */ Boolean* result)
{
    ALOGD("CCalculatorPadLayout::CheckLayoutParams========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->CheckLayoutParams(_this, p, result);
    }
    else {
        *result = ViewGroup::CheckLayoutParams(p);
    }

    ALOGD("CCalculatorPadLayout::CheckLayoutParams========begin========");
    return NOERROR;
}

ECode JSActName::_SetChildrenDrawingOrderEnabled(
        /* [in] */ Boolean enabled)
{
    ViewGroup::SetChildrenDrawingOrderEnabled(enabled);
    return NOERROR;
}

ECode JSActName::_DrawChild(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IView* child,
        /* [in] */ Int64 drawingTime,
        /* [out] */ Boolean* result)
{
    *result = ViewGroup::DrawChild(canvas, child, drawingTime);
    return NOERROR;
}
Boolean JSActName::DrawChild(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IView* child,
        /* [in] */ Int64 drawingTime)
{
    ALOGD("CCalculatorPadLayout::DrawChild========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    Boolean result;
    if (mListener) {
        ec = mListener->DrawChild(_this, canvas, child, drawingTime, &result);
    }
    else {
        result = ViewGroup::DrawChild(canvas, child, drawingTime);
    }

    return result;
}

//----------------------------------------------

ECode JSActName::_Draw(
        /* [in] */ ICanvas* canvas)
{
    return ViewGroup::Draw(canvas);
}
ECode JSActName::Draw(
        /* [in] */ ICanvas* canvas)
{
    ALOGD("CCalculatorPadLayout::Draw========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->Draw(_this, canvas);
    }
    else {
        ec = ViewGroup::Draw(canvas);
    }

    ALOGD("CCalculatorPadLayout::Draw========end========");
    return NOERROR;
}


ECode JSActName::_DispatchDraw(
        /* [in] */ ICanvas* canvas)
{
    return ViewGroup::DispatchDraw(canvas);
}
ECode JSActName::DispatchDraw(
        /* [in] */ ICanvas* canvas)
{
    ALOGD("CCalculatorPadLayout::Draw========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->DispatchDraw(_this, canvas);
    }
    else {
        ec = ViewGroup::DispatchDraw(canvas);
    }

    ALOGD("CCalculatorPadLayout::Draw========end========");
    return NOERROR;
}

ECode JSActName::_OnDraw(
        /* [in] */ ICanvas* canvas)
{
    ViewGroup::OnDraw(canvas);
    return NOERROR;
}
//ECode JSActName::OnDraw(
void JSActName::OnDraw(
        /* [in] */ ICanvas* canvas)
{
    ALOGD("CCalculatorPadLayout::OnDraw========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->OnDraw(_this, canvas);
    }
    else {
        ViewGroup::OnDraw(canvas);
    }

    return;
}

ECode JSActName::GetSuperObject(
    /* [out] */ ICalculatorPadLayoutSuperObject ** ppSuperObject)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    assert(ppSuperObject != NULL);
    *ppSuperObject = mSuperObject;

    REFCOUNT_ADD(*ppSuperObject);
    return NOERROR;
}

ECode JSActName::SuperObject::Test()
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);

    //mHost->_SetTextSize(unit, size);

    return NOERROR;
}

} // namespace JSPkgName
} // namespace Node
} // namespace DevSamples
} // namespace Elastos