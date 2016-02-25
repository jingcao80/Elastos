#include "elastos/droid/systemui/statusbar/policy/CKeyButtonView.h"

using Elastos::Droid::View::EIID_View;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {


IVIEW_METHODS_IMPL(CKeyButtonView, KeyButtonView)
IDRAWABLECALLBACK_METHODS_IMPL(CKeyButtonView, KeyButtonView)
IKEYEVENTCALLBACK_METHODS_IMPL(CKeyButtonView, KeyButtonView)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CKeyButtonView, KeyButtonView)


PInterface CKeyButtonView::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    return _CKeyButtonView::Probe(riid);
}

ECode CKeyButtonView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return KeyButtonView::Init(context, attrs);
}

ECode CKeyButtonView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return KeyButtonView::Init(context, attrs, defStyle);
}

ECode CKeyButtonView::SetDrawingAlpha(
    /* [in] */ Float alpha)
{
    return KeyButtonView::SetDrawingAlpha(alpha);
}

ECode CKeyButtonView::GetDrawingAlpha(
    /* [out] */ Float* alpha)
{
    VALIDATE_NOT_NULL(alpha);
    *alpha = KeyButtonView::GetDrawingAlpha();
    return NOERROR;
}

ECode CKeyButtonView::SetGlowAlpha(
    /* [in] */ Float alpha)
{
    return KeyButtonView::SetGlowAlpha(alpha);
}

ECode CKeyButtonView::GetGlowAlpha(
    /* [out] */ Float* alpha)
{
    VALIDATE_NOT_NULL(alpha);
    *alpha = KeyButtonView::GetGlowAlpha();
    return NOERROR;
}

ECode CKeyButtonView::SetGlowScale(
    /* [in] */ Float scale)
{
    return KeyButtonView::SetGlowScale(scale);
}

ECode CKeyButtonView::GetGlowScale(
    /* [out] */ Float* scale)
{
    VALIDATE_NOT_NULL(scale);
    *scale = KeyButtonView::GetGlowScale();
    return NOERROR;
}

ECode CKeyButtonView::GetAdjustViewBounds(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = KeyButtonView::GetAdjustViewBounds();
    return NOERROR;
}

ECode CKeyButtonView::SetAdjustViewBounds(
    /* [in] */ Boolean adjustViewBounds)
{
    return KeyButtonView::SetAdjustViewBounds(adjustViewBounds);
}

ECode CKeyButtonView::GetMaxWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    *width = KeyButtonView::GetMaxWidth();
    return NOERROR;
}

ECode CKeyButtonView::SetMaxWidth(
    /* [in] */ Int32 maxWidth)
{
    return KeyButtonView::SetMaxWidth(maxWidth);
}

ECode CKeyButtonView::GetMaxHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = KeyButtonView::GetMaxHeight();
    return NOERROR;
}

ECode CKeyButtonView::SetMaxHeight(
    /* [in] */ Int32 maxHeight)
{
    return KeyButtonView::SetMaxHeight(maxHeight);
}

ECode CKeyButtonView::GetDrawable(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    AutoPtr<IDrawable> cf = KeyButtonView::GetDrawable();
    *drawable = cf;
    REFCOUNT_ADD(*drawable);
    return NOERROR;
}

ECode CKeyButtonView::SetImageResource(
    /* [in] */ Int32 resId)
{
    return KeyButtonView::SetImageResource(resId);
}

ECode CKeyButtonView::SetImageURI(
    /* [in] */ IUri* uri)
{
    return KeyButtonView::SetImageURI(uri);
}

ECode CKeyButtonView::SetImageDrawable(
    /* [in] */ IDrawable* drawable)
{
    return KeyButtonView::SetImageDrawable(drawable);
}

ECode CKeyButtonView::SetImageBitmap(
    /* [in] */ IBitmap* bm)
{
    return KeyButtonView::SetImageBitmap(bm);
}

ECode CKeyButtonView::SetImageState(
    /* [in] */ ArrayOf<Int32>* state,
    /* [in] */ Boolean mg)
{
    return KeyButtonView::SetImageState(state, mg);
}

ECode CKeyButtonView::SetImageLevel(
    /* [in] */ Int32 level)
{
    return KeyButtonView::SetImageLevel(level);
}

ECode CKeyButtonView::SetScaleType(
    /* [in] */ ImageViewScaleType scaleType)
{
    return KeyButtonView::SetScaleType(scaleType);
}

ECode CKeyButtonView::GetScaleType(
    /* [out] */ ImageViewScaleType* scaleType)
{
    VALIDATE_NOT_NULL(scaleType);
    *scaleType = KeyButtonView::GetScaleType();
    return NOERROR;
}

ECode CKeyButtonView::GetImageMatrix(
    /* [out] */ IMatrix** martix)
{
    VALIDATE_NOT_NULL(martix);
    AutoPtr<IMatrix> cf = KeyButtonView::GetImageMatrix();
    *martix = cf;
    REFCOUNT_ADD(*martix);
    return NOERROR;
}

ECode CKeyButtonView::SetImageMatrix(
    /* [in] */ IMatrix* matrix)
{
    return KeyButtonView::SetImageMatrix(matrix);
}

ECode CKeyButtonView::GetCropToPadding(
    /* [out] */ Boolean* padding)
{
    VALIDATE_NOT_NULL(padding);
    *padding = KeyButtonView::GetCropToPadding();
    return NOERROR;
}

ECode CKeyButtonView::SetCropToPadding(
    /* [in] */ Boolean cropToPadding)
{
    return KeyButtonView::SetCropToPadding(cropToPadding);
}

ECode CKeyButtonView::SetBaseline(
    /* [in] */ Int32 baseline)
{
    return KeyButtonView::SetBaseline(baseline);
}

ECode CKeyButtonView::SetBaselineAlignBottom(
    /* [in] */ Boolean aligned)
{
    return KeyButtonView::SetBaselineAlignBottom(aligned);
}

ECode CKeyButtonView::GetBaselineAlignBottom(
    /* [out] */ Boolean* aligned)
{
    VALIDATE_NOT_NULL(aligned);
    *aligned = KeyButtonView::GetBaselineAlignBottom();
    return NOERROR;
}

ECode CKeyButtonView::SetColorFilter(
    /* [in] */ Int32 color)
{
    return KeyButtonView::SetColorFilter(color);
}

ECode CKeyButtonView::SetColorFilter(
    /* [in] */ Int32 color,
    /* [in] */ PorterDuffMode mode)
{
    return KeyButtonView::SetColorFilter(color, mode);
}

ECode CKeyButtonView::ClearColorFilter()
{
    return KeyButtonView::ClearColorFilter();
}

ECode CKeyButtonView::GetColorFilter(
    /* [out] */ IColorFilter** filter)
{
    VALIDATE_NOT_NULL(filter);
    AutoPtr<IColorFilter> cf = KeyButtonView::GetColorFilter();
    *filter = cf;
    REFCOUNT_ADD(*filter);
    return NOERROR;
}

ECode CKeyButtonView::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    return KeyButtonView::SetColorFilter(cf);
}

ECode CKeyButtonView::GetImageAlpha(
    /* [out] */ Int32* alpha)
{
    VALIDATE_NOT_NULL(alpha);
    *alpha = KeyButtonView::GetImageAlpha();
    return NOERROR;
}

ECode CKeyButtonView::SetImageAlpha(
    /* [in] */ Int32 alpha)
{
    return KeyButtonView::SetImageAlpha(alpha);
}

ECode CKeyButtonView::SetAlpha(
    /* [in] */ Int32 alpha)
{
    return KeyButtonView::SetAlpha(alpha);
}

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
