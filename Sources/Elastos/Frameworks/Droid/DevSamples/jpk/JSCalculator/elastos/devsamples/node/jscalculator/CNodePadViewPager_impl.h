#include "CNodePadViewPager.h"

#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace JSPkgName {

EXTERN NodeBridge* g_pNodeBridge;
EXTERN NodeBridge** g_ppNodeBridge;

EXTERN IHandler* myHandler;

CAR_INTERFACE_IMPL_2(JSActName, ViewGroup, IViewPager, ICalculatorPadViewPager)

JS_CAR_OBJECT_IMPL(JSActName)

CAR_INTERFACE_IMPL(JSActName::SuperObject, Object, ICalculatorPadViewPagerSuperObject)

JSActName::CCalculatorPadViewPager(){}
JSActName::~CCalculatorPadViewPager(){}

ECode JSActName::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode JSActName::_constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return ViewGroup::constructor(context, attrs);
}
ECode JSActName::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    ALOGD("CCalculatorPadViewPager::constructor========begin========");

    ECode ec = NOERROR;

    SuperObject* superObject = new SuperObject(this);
    mSuperObject = (ICalculatorPadViewPagerSuperObject*)superObject->Probe(EIID_ICalculatorPadViewPagerSuperObject);

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);
    JSEvtName::RegisterCalculatorPadViewPager(context, _this, (ICalculatorPadViewPagerListener**)&mListener);

    if (mListener) {
        ec = mListener->OnCreate(_this, context, attrs);
    }
    else {
        ec = ViewGroup::constructor(context, attrs);
    }

    ALOGD("CCalculatorPadViewPager::constructor========end========");
    return ec;
}

//------------------------------------------
//protected:
ECode JSActName::_OnFinishInflate()
{
    return ViewGroup::OnFinishInflate();
}
ECode JSActName::OnFinishInflate()
{
    ALOGD("CCalculatorPadViewPager::OnFinishInflate========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->OnFinishInflate(_this);
    }
    else {
        ec = ViewGroup::OnFinishInflate();
    }

    return ec;
}

//protected:
ECode JSActName::_OnDetachedFromWindow()
{
    return ViewGroup::OnDetachedFromWindow();
}
ECode JSActName::OnDetachedFromWindow()
{
    ALOGD("CCalculatorPadViewPager::OnDetachedFromWindow========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->OnDetachedFromWindow(_this);
    }
    else {
        ec = ViewGroup::OnDetachedFromWindow();
    }

    return ec;
}

ECode JSActName::_GetChildDrawingOrder(
        /* [in] */ Int32 childCount,
        /* [in] */ Int32 i,
        /* [out] */ Int32* result)
{
    *result = ViewGroup::GetChildDrawingOrder(childCount, i);
    return NOERROR;
}
Int32 JSActName::GetChildDrawingOrder(
        /* [in] */ Int32 childCount,
        /* [in] */ Int32 i)
{
    ALOGD("CCalculatorPadViewPager::GetChildDrawingOrder========begin========");

    Int32 result;
    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->GetChildDrawingOrder(_this, childCount, i, &result);
    }
    else {
        result = ViewGroup::GetChildDrawingOrder(childCount, i);
    }

    ALOGD("CCalculatorPadViewPager::GetChildDrawingOrder========end========");
    return result;
}

ECode JSActName::_VerifyDrawable(
        /* [in] */ IDrawable* who,
        /* [out] */Boolean* result)
{
    *result = ViewGroup::VerifyDrawable(who);
    return NOERROR;
}
//ECode JSActName::VerifyDrawable(
Boolean JSActName::VerifyDrawable(
        /* [in] */ IDrawable* who)
{
    ALOGD("CCalculatorPadViewPager::VerifyDrawable========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    Boolean result = FALSE;
    if (mListener) {
        ec = mListener->VerifyDrawable(_this, who, &result);
    }
    else {
        result = ViewGroup::VerifyDrawable(who);
    }

    return result;
}

ECode JSActName::_DrawableStateChanged()
{
    return ViewGroup::DrawableStateChanged();
}
ECode JSActName::DrawableStateChanged()
{
    ALOGD("CCalculatorPadViewPager::DrawableStateChanged========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->DrawableStateChanged(_this);
    }
    else {
        ec = ViewGroup::DrawableStateChanged();
    }

    return ec;
}

//public:
ECode JSActName::_OnSaveInstanceState(
        /* [out] */ IParcelable** result)
{
    *result = ViewGroup::OnSaveInstanceState();
    return NOERROR;
}
//ECode JSActName::OnSaveInstanceState(
//        /* [out] */ IParcelable** result)
AutoPtr<IParcelable> JSActName::OnSaveInstanceState()
{
    ALOGD("CCalculatorPadViewPager::OnSaveInstanceState========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    AutoPtr<IParcelable> result;
    if (mListener) {
        ec = mListener->OnSaveInstanceState(_this, (IParcelable**)&result);
    }
    else {
        result = ViewGroup::OnSaveInstanceState();
    }

    return result;
}

ECode JSActName::_OnRestoreInstanceState(
        /* [in] */ IParcelable* state)
{
    ViewGroup::OnRestoreInstanceState(state);
    return NOERROR;
}

//ECode JSActName::OnRestoreInstanceState(
void JSActName::OnRestoreInstanceState(
        /* [in] */ IParcelable* state)
{
    ALOGD("CCalculatorPadViewPager::OnRestoreInstanceState========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->OnRestoreInstanceState(_this, state);
    }
    else {
        ViewGroup::OnRestoreInstanceState(state);
    }

    return;
}

ECode JSActName::_AddView(
        /* [in] */ IView* child,
        /* [in] */ Int32 index,
        /* [in] */ IViewGroupLayoutParams* params)
{
    return ViewGroup::AddView(child, index, params);
}
ECode JSActName::AddView(
        /* [in] */ IView* child,
        /* [in] */ Int32 index,
        /* [in] */ IViewGroupLayoutParams* params)
{
    ALOGD("CCalculatorPadViewPager::AddView========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->AddView(_this, child, index, params);
    }
    else {
        ec = ViewGroup::AddView(child, index, params);
    }

    return ec;
}

ECode JSActName::_RemoveView(
        /* [in] */ IView* view)
{
    return ViewGroup::RemoveView(view);
}
ECode JSActName::RemoveView(
        /* [in] */ IView* view)
{
    ALOGD("CCalculatorPadViewPager::RemoveView========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->RemoveView(_this, view);
    }
    else {
        ec = ViewGroup::RemoveView(view);
    }

    return ec;
}

//protected:
ECode JSActName::_OnAttachedToWindow()
{
    return ViewGroup::OnAttachedToWindow();
}
ECode JSActName::OnAttachedToWindow()
{
    ALOGD("CCalculatorPadViewPager::OnAttachedToWindow========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->OnAttachedToWindow(_this);
    }
    else {
        ec = ViewGroup::OnAttachedToWindow();
    }

    return ec;
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
    ALOGD("CCalculatorPadViewPager::OnMeasure========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->OnMeasure(_this, widthMeasureSpec, heightMeasureSpec);
    }
    else {
        ec = ViewGroup::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    }

    return ec;
}

ECode JSActName::_OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh)
{
    ViewGroup::OnSizeChanged(w, h, oldw, oldh);
    return NOERROR;
}
//ECode JSActName::OnSizeChanged(
void JSActName::OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh)
{
    ALOGD("CCalculatorPadViewPager::OnSizeChanged========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->OnSizeChanged(_this, w, h, oldw, oldh);
    }
    else {
        ViewGroup::OnSizeChanged(w, h, oldw, oldh);
    }

    return;
}

ECode JSActName::_OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom)
{
    //return ViewGroup::OnLayout(changed, left, top, right, bottom);
    return NOERROR;
}
ECode JSActName::OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom)
{
    ALOGD("CCalculatorPadViewPager::OnLayout========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->OnLayout(_this, changed, left, top, right, bottom);
    }
    else {
        //ec = ViewGroup::OnLayout(changed, left, top, right, bottom);
    }

    return ec;
}

//public:
ECode JSActName::_ComputeScroll()
{
    return ViewGroup::ComputeScroll();
}
ECode JSActName::ComputeScroll()
{
    ALOGD("CCalculatorPadViewPager::ComputeScroll========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->ComputeScroll(_this);
    }
    else {
        ec = ViewGroup::ComputeScroll();
    }

    return ec;
}

ECode JSActName::_OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* res)
{
    return ViewGroup::OnInterceptTouchEvent(ev, res);
}
ECode JSActName::OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* res)
{
    ALOGD("CCalculatorPadViewPager::OnInterceptTouchEvent========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->OnInterceptTouchEvent(_this, ev, res);
    }
    else {
        ec = ViewGroup::OnInterceptTouchEvent(ev, res);
    }

    return ec;
}

ECode JSActName::_OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res)
{
    return ViewGroup::OnTouchEvent(event, res);
}
ECode JSActName::OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res)
{
    ALOGD("CCalculatorPadViewPager::OnTouchEvent========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->OnTouchEvent(_this, event, res);
    }
    else {
        ec = ViewGroup::OnTouchEvent(event, res);
    }

    return ec;
}

ECode JSActName::_Draw(
        /* [in] */ ICanvas* canvas)
{
    return ViewGroup::Draw(canvas);
}
ECode JSActName::Draw(
        /* [in] */ ICanvas* canvas)
{
    ALOGD("CCalculatorPadViewPager::Draw========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->Draw(_this, canvas);
    }
    else {
        ec = ViewGroup::Draw(canvas);
    }

    return ec;
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
    ALOGD("CCalculatorPadViewPager::OnDraw========begin========");

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

ECode JSActName::_DispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res)
{
    return ViewGroup::DispatchKeyEvent(event, res);
}
ECode JSActName::DispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res)
{
    ALOGD("CCalculatorPadViewPager::DispatchKeyEvent========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->DispatchKeyEvent(_this, event, res);
    }
    else {
        ec = ViewGroup::DispatchKeyEvent(event, res);
    }

    return ec;
}

ECode JSActName::_AddFocusables(
        /* [in] */ IArrayList* views,
        /* [in] */ Int32 direction,
        /* [in] */ Int32 focusableMode)
{
    return ViewGroup::AddFocusables(views, direction, focusableMode);
}
ECode JSActName::AddFocusables(
        /* [in] */ IArrayList* views,
        /* [in] */ Int32 direction,
        /* [in] */ Int32 focusableMode)
{
    ALOGD("CCalculatorPadViewPager::AddFocusables========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->AddFocusables(_this, views, direction, focusableMode);
    }
    else {
        ec = ViewGroup::AddFocusables(views, direction, focusableMode);
    }

    return ec;
}

ECode JSActName::_AddTouchables(
        /* [in] */ IArrayList* views)
{
    return ViewGroup::AddTouchables(views);
}
ECode JSActName::AddTouchables(
        /* [in] */ IArrayList* views)
{
    ALOGD("CCalculatorPadViewPager::AddTouchables========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->AddTouchables(_this, views);
    }
    else {
        ec = ViewGroup::AddTouchables(views);
    }

    return ec;
}

//protected:
ECode JSActName::_OnRequestFocusInDescendants(
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect,
        /* [out] */ Boolean* result)
{
    *result = ViewGroup::OnRequestFocusInDescendants(direction, previouslyFocusedRect);
    return NOERROR;
}
Boolean JSActName::OnRequestFocusInDescendants(
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect)
{
    ALOGD("CCalculatorPadViewPager::OnRequestFocusInDescendants========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    Boolean result;
    if (mListener) {
        ec = mListener->OnRequestFocusInDescendants(_this, direction, previouslyFocusedRect, &result);
    }
    else {
        result = ViewGroup::OnRequestFocusInDescendants(direction, previouslyFocusedRect);
    }

    return result;
}

//public:
ECode JSActName::_DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* res)
{
    return ViewGroup::DispatchPopulateAccessibilityEvent(event, res);
}
ECode JSActName::DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* res)
{
    ALOGD("CCalculatorPadViewPager::DispatchPopulateAccessibilityEvent========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->DispatchPopulateAccessibilityEvent(_this, event, res);
    }
    else {
        ec = ViewGroup::DispatchPopulateAccessibilityEvent(event, res);
    }

    return ec;
}

//protected:
ECode JSActName::_GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** params)
{
    return ViewGroup::GenerateDefaultLayoutParams(params);
}
ECode JSActName::GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** params)
{
    ALOGD("CCalculatorPadViewPager::GenerateDefaultLayoutParams========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->GenerateDefaultLayoutParams(_this, params);
    }
    else {
        ec = ViewGroup::GenerateDefaultLayoutParams(params);
    }

    return ec;
}

ECode JSActName::_GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p,
        /* [out] */ IViewGroupLayoutParams** params)
{
    *params = ViewGroup::GenerateLayoutParams(p);

    return NOERROR;
}
//ECode JSActName::GenerateLayoutParams(
AutoPtr<IViewGroupLayoutParams> JSActName::GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p)
{
    ALOGD("CCalculatorPadViewPager::GenerateLayoutParams========begin========");

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
    ALOGD("CCalculatorPadViewPager::_CheckLayoutParams========begin========");

    *result = ViewGroup::CheckLayoutParams(p);
    ALOGD("CCalculatorPadViewPager::_CheckLayoutParams========end========");

    return NOERROR;
}
Boolean JSActName::CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p)
{
    ALOGD("CCalculatorPadViewPager::CheckLayoutParams========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    Boolean result;
    if (mListener) {
        ec = mListener->CheckLayoutParams(_this, p, &result);
    }
    else {
        result = ViewGroup::CheckLayoutParams(p);
    }

    ALOGD("CCalculatorPadViewPager::CheckLayoutParams========end========");
    return result;
}

//public:
ECode JSActName::_GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** params)
{
    return ViewGroup::GenerateLayoutParams(attrs, params);
}
ECode JSActName::GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** params)
{
    ALOGD("CCalculatorPadViewPager::GenerateLayoutParams========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->GenerateLayoutParams(_this, attrs, params);
    }
    else {
        ec = ViewGroup::GenerateLayoutParams(attrs, params);
    }

    return ec;
}

//------------------------------------------

ECode JSActName::GetSuperObject(
    /* [out] */ ICalculatorPadViewPagerSuperObject ** ppSuperObject)
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