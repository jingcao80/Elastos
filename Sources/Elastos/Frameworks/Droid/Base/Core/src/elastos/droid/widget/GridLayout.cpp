
#include "Elastos.Droid.Content.h"
#include "elastos/droid/widget/GridLayout.h"
#include "elastos/droid/graphics/Insets.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/Color.h"
#include "elastos/droid/widget/CGridLayoutLayoutParams.h"

#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IInsets;
using Elastos::Droid::Graphics::Insets;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::Color;
using Elastos::Droid::Widget::IGridLayoutLayoutParams;
using Elastos::Droid::Widget::CGridLayoutLayoutParams;
using Elastos::Droid::Widget::GridLayout;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Widget {

static AutoPtr<GridLayout::Interval> InitDSpan() {
    AutoPtr<GridLayout::Interval> val = new GridLayout::Interval(GridLayout::UNDEFINED, GridLayout::UNDEFINED + 1);
    return val;
}

const Int32 GridLayout::LayoutParams::DEFAULT_MARGIN = Elastos::Core::Math::INT32_MIN_VALUE;
const Int32 GridLayout::LayoutParams::DEFAULT_ROW = Elastos::Core::Math::INT32_MIN_VALUE;
const Int32 GridLayout::LayoutParams::DEFAULT_COLUMN = Elastos::Core::Math::INT32_MIN_VALUE;
const AutoPtr<GridLayout::Interval> GridLayout::LayoutParams::DEFAULT_SPAN = InitDSpan();
const Int32 GridLayout::LayoutParams::DEFAULT_SPAN_SIZE = DEFAULT_SPAN->Size();

CAR_INTERFACE_IMPL(GridLayout::LayoutParams, ViewGroup::MarginLayoutParams, IGridLayoutLayoutParams)

void GridLayout::LayoutParams::ReInitSuper(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::ViewGroup_MarginLayout),
            ArraySize(R::styleable::ViewGroup_MarginLayout));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);
    //try {
        Int32 margin = 0;
        a->GetDimensionPixelSize(MARGIN, DEFAULT_MARGIN, &margin);

        a->GetDimensionPixelSize(LEFT_MARGIN, margin, &mLeftMargin);
        a->GetDimensionPixelSize(TOP_MARGIN, margin, &mTopMargin);
        a->GetDimensionPixelSize(RIGHT_MARGIN, margin, &mRightMargin);
        a->GetDimensionPixelSize(BOTTOM_MARGIN, margin, &mBottomMargin);
    //} finally {
        a->Recycle();
    //}
}

void GridLayout::LayoutParams::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::GridLayout_Layout),
            ArraySize(R::styleable::GridLayout_Layout));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);

//    try {
        Int32 gravity = 0;
        a->GetInt32(GRAVITY, IGravity::NO_GRAVITY, &gravity);

        Int32 column = 0;
        a->GetInt32(COLUMN, DEFAULT_COLUMN, &column);
        Int32 colSpan = 0;
        a->GetInt32(COLUMN_SPAN, DEFAULT_SPAN_SIZE, &colSpan);
        Float colWeight;
        a->GetFloat(COLUMN_WEIGHT, GridLayout::Spec::DEFAULT_WEIGHT, &colWeight);
        mColumnSpec = GridLayout::GetSpec(column, colSpan, GridLayout::GetAlignment(gravity, TRUE), colWeight);

        Int32 row = 0;
        a->GetInt32(ROW, DEFAULT_ROW, &row);
        Int32 rowSpan = 0;
        a->GetInt32(ROW_SPAN, DEFAULT_SPAN_SIZE, &rowSpan);
        mRowSpec = GridLayout::GetSpec(row, rowSpan, GridLayout::GetAlignment(gravity, FALSE), colWeight);
//    } finally {
        a->Recycle();
//    }
}


GridLayout::LayoutParams::LayoutParams()
{}

ECode GridLayout::LayoutParams::SetGravity(
    /* [in] */ Int32 gravity)
{
    AutoPtr<GridLayout::Spec> row = (GridLayout::Spec*)mRowSpec.Get();
    AutoPtr<GridLayout::Spec> col = (GridLayout::Spec*)mColumnSpec.Get();
    mRowSpec = IGridLayoutSpec::Probe(row->CopyWriteAlignment(GridLayout::GetAlignment(gravity, FALSE)));
    mColumnSpec = IGridLayoutSpec::Probe(col->CopyWriteAlignment(GridLayout::GetAlignment(gravity, TRUE)));
    return NOERROR;
}

ECode GridLayout::LayoutParams::SetRowSpec(
    /* [in] */ IGridLayoutSpec* r)
{
    mRowSpec = r;
    return NOERROR;
}

ECode GridLayout::LayoutParams::SetColumnSpec(
    /* [in] */ IGridLayoutSpec* c)
{
    mColumnSpec = c;
    return NOERROR;
}

ECode GridLayout::LayoutParams::GetRowSpec(
    /* [out] */ IGridLayoutSpec** r)
{
    VALIDATE_NOT_NULL(r)

    *r = mRowSpec;
    REFCOUNT_ADD(*r)
    return NOERROR;
}

ECode GridLayout::LayoutParams::GetColumnSpec(
    /* [out] */ IGridLayoutSpec** c)
{
    VALIDATE_NOT_NULL(c)

    *c = mColumnSpec;
    REFCOUNT_ADD(*c)
    return NOERROR;
}

void GridLayout::LayoutParams::SetRowSpecSpan(
    /* [in] */ GridLayout::Interval* span)
{
    AutoPtr<GridLayout::Spec> row = (GridLayout::Spec*)mRowSpec.Get();
    mRowSpec = row->CopyWriteSpan(span);
}

void GridLayout::LayoutParams::SetColumnSpecSpan(
    /* [in] */ GridLayout::Interval* span)
{
    AutoPtr<GridLayout::Spec> col = (GridLayout::Spec*)mColumnSpec.Get();
    mColumnSpec = col->CopyWriteSpan(span);
}

ECode GridLayout::LayoutParams::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(obj);

    IGridLayoutLayoutParams * glp = IGridLayoutLayoutParams::Probe(obj);
    if (glp == NULL) return NOERROR;

    LayoutParams* that = (LayoutParams*) glp;

    Boolean equals = FALSE;
    IObject::Probe(mColumnSpec)->Equals(that->mColumnSpec, &equals);
    if (!equals) return NOERROR;
    IObject::Probe(mRowSpec)->Equals(that->mRowSpec, &equals);
    if (!equals) return NOERROR;

    *result = TRUE;
    return NOERROR;
}

ECode GridLayout::LayoutParams::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash)

    Int32 result = 0;
    IObject::Probe(mRowSpec)->GetHashCode(&result);
    Int32 col = 0;
    IObject::Probe(mColumnSpec)->GetHashCode(&col);
    result = 31 * result + col;
    *hash = result;
    return NOERROR;
}

ECode GridLayout::LayoutParams::SetBaseAttributes(
    /* [in] */ ITypedArray* attributes,
    /* [in] */ Int32 widthAttr,
    /* [in] */ Int32 heightAttr)
{
    FAIL_RETURN(attributes->GetLayoutDimension(widthAttr, DEFAULT_WIDTH, &mWidth));
    return attributes->GetLayoutDimension(heightAttr, DEFAULT_HEIGHT, &mHeight);
}

ECode GridLayout::LayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ IGridLayoutSpec* rowSpec,
    /* [in] */ IGridLayoutSpec* columnSpec)
{
    FAIL_RETURN(MarginLayoutParams::constructor(width, height))
    SetMargins(left, top, right, bottom);
    mRowSpec = rowSpec;
    mColumnSpec = columnSpec;
    return NOERROR;
}

ECode GridLayout::LayoutParams::constructor()
{
    return constructor(GridLayout::Spec::UNDEFINED, GridLayout::Spec::UNDEFINED);
}

ECode GridLayout::LayoutParams::constructor(
    /* [in] */ IGridLayoutSpec* rowSpec,
    /* [in] */ IGridLayoutSpec* columnSpec)
{
    return constructor(DEFAULT_WIDTH, DEFAULT_HEIGHT,
        DEFAULT_MARGIN, DEFAULT_MARGIN, DEFAULT_MARGIN, DEFAULT_MARGIN,
        rowSpec, columnSpec);
}

ECode GridLayout::LayoutParams::constructor(
    /* [in] */ IViewGroupLayoutParams* params)
{
    return MarginLayoutParams::constructor(params);
}

ECode GridLayout::LayoutParams::constructor(
    /* [in] */ IViewGroupMarginLayoutParams* params)
{
    return MarginLayoutParams::constructor(params);
}

ECode GridLayout::LayoutParams::constructor(
    /* [in] */ IGridLayoutLayoutParams* that)
{
    FAIL_RETURN(MarginLayoutParams::constructor(IViewGroupMarginLayoutParams::Probe(that)))
    AutoPtr<LayoutParams> cgl = (LayoutParams*)that;
    mRowSpec = cgl->mRowSpec;
    mColumnSpec = cgl->mColumnSpec;
    return NOERROR;
}

ECode GridLayout::LayoutParams::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(MarginLayoutParams::constructor(context, attrs))
    ReInitSuper(context, attrs);
    Init(context, attrs);
    return NOERROR;
}

static AutoPtr<GridLayout::Alignment> InitUndefine()
{
    AutoPtr<GridLayout::Alignment> ment = new GridLayout::UndefinedAlignment();
    return ment;
}

static AutoPtr<GridLayout::Alignment> InitLeading()
{
    AutoPtr<GridLayout::Alignment> ment = new GridLayout::LeadingAlignment();
    return ment;
}

static AutoPtr<GridLayout::Alignment> InitTrailing()
{
    AutoPtr<GridLayout::Alignment> ment = new GridLayout::TrailingAlignment();
    return ment;
}

static AutoPtr<GridLayout::Alignment> InitCenter()
{
    AutoPtr<GridLayout::Alignment> ment = new GridLayout::CenterAlignment();
    return ment;
}

static AutoPtr<GridLayout::Alignment> InitBaseLine()
{
    AutoPtr<GridLayout::Alignment> ment = new GridLayout::BaseLineAlignment();
    return ment;
}

static AutoPtr<GridLayout::Alignment> InitFill()
{
    AutoPtr<GridLayout::Alignment> ment = new GridLayout::FillAlignment();
    return ment;
}

static AutoPtr<GridLayout::Alignment> InitLeft()
{
    AutoPtr<GridLayout::Alignment> ment = GridLayout::CreateSwitchingAlignment(GridLayout::START, GridLayout::END);
    return ment;
}

static AutoPtr<GridLayout::Alignment> InitRight()
{
    AutoPtr<GridLayout::Alignment> ment = GridLayout::CreateSwitchingAlignment(GridLayout::END, GridLayout::START);
    return ment;
}

static AutoPtr<GridLayout::Alignment> InitTop()
{
    if (GridLayout::LEADING) {
        return GridLayout::LEADING;
    }
    return NULL;
}

static AutoPtr<GridLayout::Alignment> InitBottom()
{
    if (GridLayout::TRAILING) {
        return GridLayout::TRAILING;
    }
    return NULL;
}

const String GridLayout::TAG("GridLayout");
const Int32 GridLayout::HORIZONTAL;
const Int32 GridLayout::VERTICAL;
const Int32 GridLayout::UNDEFINED = Elastos::Core::Math::INT32_MIN_VALUE;
const Int32 GridLayout::ALIGN_BOUNDS;
const Int32 GridLayout::ALIGN_MARGINS;
const Int32 GridLayout::MAX_SIZE;
const Int32 GridLayout::DEFAULT_CONTAINER_MARGIN;
const Int32 GridLayout::UNINITIALIZED_HASH;
const Int32 GridLayout::DEFAULT_ORIENTATION;
const Int32 GridLayout::DEFAULT_COUNT = UNDEFINED;
const Boolean GridLayout::DEFAULT_USE_DEFAULT_MARGINS = FALSE;
const Boolean GridLayout::DEFAULT_ORDER_PRESERVED;
const Int32 GridLayout::DEFAULT_ALIGNMENT_MODE;
const Int32 GridLayout::ORIENTATION;
const Int32 GridLayout::ROW_COUNT;
const Int32 GridLayout::COLUMN_COUNT;
const Int32 GridLayout::USE_DEFAULT_MARGINS;
const Int32 GridLayout::ALIGNMENT_MODE;
const Int32 GridLayout::ROW_ORDER_PRESERVED;
const Int32 GridLayout::COLUMN_ORDER_PRESERVED;
const AutoPtr<GridLayout::Alignment> GridLayout::UNDEFINED_ALIGNMENT = InitUndefine(); // = UndefinedAlignment
const AutoPtr<GridLayout::Alignment> GridLayout::LEADING = InitLeading(); // = LeadingAlignment
const AutoPtr<GridLayout::Alignment> GridLayout::TRAILING = InitTrailing(); // = TrailingAlignment
const AutoPtr<GridLayout::Alignment> GridLayout::TOP = InitTop(); //= LEADING
const AutoPtr<GridLayout::Alignment> GridLayout::BOTTOM = InitBottom(); //= TRAILING
const AutoPtr<GridLayout::Alignment> GridLayout::START = InitTop(); //= LEADING
const AutoPtr<GridLayout::Alignment> GridLayout::END = InitBottom(); //= TRAILING
const AutoPtr<GridLayout::Alignment> GridLayout::LEFT = InitLeft(); //= createSwitchingAlignment(START, END);
const AutoPtr<GridLayout::Alignment> GridLayout::RIGHT = InitRight(); //= createSwitchingAlignment(END, START);
const AutoPtr<GridLayout::Alignment> GridLayout::CENTER = InitCenter(); // = CenterAlignment
const AutoPtr<GridLayout::Alignment> GridLayout::BASELINE = InitBaseLine(); // = BaseLineAlignment
const AutoPtr<GridLayout::Alignment> GridLayout::FILL = InitFill(); // = FillAlignment*/
const Int32 GridLayout::INFLEXIBLE;
const Int32 GridLayout::CAN_STRETCH;

CAR_INTERFACE_IMPL(GridLayout, Object, IGridLayout)

GridLayout::GridLayout()
    : mOrientation(DEFAULT_ORIENTATION)
    , mUseDefaultMargins(DEFAULT_USE_DEFAULT_MARGINS)
    , mAlignmentMode(DEFAULT_ALIGNMENT_MODE)
    , mDefaultGap(0)
    , mLastLayoutParamsHashCode(UNINITIALIZED_HASH)
{
    mHorizontalAxis = new Axis(TRUE, this);
    mVerticalAxis = new Axis(FALSE, this);
}

ECode GridLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    ViewGroup::constructor(context, attrs, defStyleAttr, defStyleRes);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetDimensionPixelOffset(R::dimen::default_gap, &mDefaultGap);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::GridLayout),
            ArraySize(R::styleable::GridLayout));
    AutoPtr<ITypedArray> a;
    FAIL_RETURN(context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a));

//    try {
    Int32 rowCount = 0;
    a->GetInt32(ROW_COUNT, DEFAULT_COUNT, &rowCount);
    SetRowCount(rowCount);
    Int32 columnCount = 0;
    a->GetInt32(COLUMN_COUNT, DEFAULT_COUNT, &columnCount);
    SetColumnCount(columnCount);
    Int32 orientation = 0;
    a->GetInt32(ORIENTATION, DEFAULT_ORIENTATION, &orientation);
    SetOrientation(orientation);
    Boolean margins = FALSE;
    a->GetBoolean(USE_DEFAULT_MARGINS, DEFAULT_USE_DEFAULT_MARGINS, &margins);
    SetUseDefaultMargins(margins);
    Int32 alignmentMode = 0;
    a->GetInt32(ALIGNMENT_MODE, DEFAULT_ALIGNMENT_MODE, &alignmentMode);
    SetAlignmentMode(alignmentMode);
    Boolean rowOrderPreserved = FALSE;
    a->GetBoolean(ROW_ORDER_PRESERVED, DEFAULT_ORDER_PRESERVED, &rowOrderPreserved);
    SetRowOrderPreserved(rowOrderPreserved);
    Boolean columnOrderPreserved = FALSE;
    a->GetBoolean(COLUMN_ORDER_PRESERVED, DEFAULT_ORDER_PRESERVED, &columnOrderPreserved);
    SetColumnOrderPreserved(columnOrderPreserved);
//        } finally {
    a->Recycle();
//        }
    return NOERROR;
}

Int32 GridLayout::GetOrientation()
{
   return mOrientation;
}

ECode GridLayout::GetOrientation(
    /* [out] */ Int32* orientation)
{
   *orientation = mOrientation;
   return NOERROR;
}


ECode GridLayout::GetRowCount(
    /* [out] */ Int32* result)
{
    *result = GetRowCount();
    return NOERROR;
}

ECode GridLayout::GetColumnCount(
    /* [out] */ Int32* result)
{
    *result = GetColumnCount();
    return NOERROR;
}

ECode GridLayout::GetUseDefaultMargins(
    /* [out] */ Boolean* result)
{
    *result = GetUseDefaultMargins();
    return NOERROR;
}

ECode GridLayout::GetAlignmentMode(
    /* [out] */ Int32* result)
{
    *result = GetAlignmentMode();
    return NOERROR;
}
ECode GridLayout::IsRowOrderPreserved(
    /* [out] */ Boolean* result)
{
    *result = IsRowOrderPreserved();
    return NOERROR;
}

ECode GridLayout::IsColumnOrderPreserved(
    /* [out] */ Boolean* result)
{
    *result = IsColumnOrderPreserved();
    return NOERROR;
}

ECode GridLayout::SetOrientation(
    /* [in] */ Int32 orientation)
{
    if (mOrientation != orientation) {
        mOrientation = orientation;
        InvalidateStructure();
        RequestLayout();
    }
    return NOERROR;
}

Int32 GridLayout::GetRowCount()
{
    return mVerticalAxis->GetCount();
}

ECode GridLayout::SetRowCount(
    /* [in] */ Int32 rowCount)
{
    mVerticalAxis->SetCount(rowCount);
    InvalidateStructure();
    RequestLayout();
    return NOERROR;
}

Int32 GridLayout::GetColumnCount()
{
    return mHorizontalAxis->GetCount();
}

ECode GridLayout::SetColumnCount(
    /* [in] */ Int32 columnCount)
{
    mHorizontalAxis->SetCount(columnCount);
    InvalidateStructure();
    RequestLayout();
    return NOERROR;
}

Boolean GridLayout::GetUseDefaultMargins()
{
    return mUseDefaultMargins;
}

ECode GridLayout::SetUseDefaultMargins(
    /* [in] */ Boolean useDefaultMargins)
{
    mUseDefaultMargins = useDefaultMargins;
    RequestLayout();
    return NOERROR;
}

Int32 GridLayout::GetAlignmentMode()
{
    return mAlignmentMode;
}

ECode GridLayout::SetAlignmentMode(
    /* [in] */ Int32 alignmentMode)
{
    mAlignmentMode = alignmentMode;
    RequestLayout();
    return NOERROR;
}

Boolean GridLayout::IsRowOrderPreserved()
{
    return mVerticalAxis->IsOrderPreserved();;
}

ECode GridLayout::SetRowOrderPreserved(
    /* [in] */ Boolean rowOrderPreserved)
{
    mVerticalAxis->SetOrderPreserved(rowOrderPreserved);
    InvalidateStructure();
    RequestLayout();
    return NOERROR;
}

Boolean GridLayout::IsColumnOrderPreserved()
{
    return mHorizontalAxis->IsOrderPreserved();
}

ECode GridLayout::SetColumnOrderPreserved(
    /* [in] */ Boolean columnOrderPreserved)
{
    mHorizontalAxis->SetOrderPreserved(columnOrderPreserved);
    InvalidateStructure();
    RequestLayout();
    return NOERROR;
}

Int32 GridLayout::Max2(
    /* [in] */ ArrayOf<Int32>* a,
    /* [in] */ Int32 valueIfEmpty)
{
    Int32 result = valueIfEmpty;
    for (Int32 i = 0, N = a->GetLength(); i < N; i++) {
        result = Elastos::Core::Math::Max(result, (*a)[i]);
    }
    return result;
}

AutoPtr<GridLayout::Alignment> GridLayout::GetAlignment(
    /* [in] */ Int32 gravity,
    /* [in] */ Boolean horizontal)
{
    Int32 mask = horizontal ? IGravity::HORIZONTAL_GRAVITY_MASK : IGravity::VERTICAL_GRAVITY_MASK;
    Int32 shift = horizontal ? IGravity::AXIS_X_SHIFT : IGravity::AXIS_Y_SHIFT;
    Int32 flags = (gravity & mask) >> shift;
    switch (flags) {
        case (IGravity::AXIS_SPECIFIED | IGravity::AXIS_PULL_BEFORE):
            return horizontal ? LEFT : TOP;
        case (IGravity::AXIS_SPECIFIED | IGravity::AXIS_PULL_AFTER):
            return horizontal ? RIGHT : BOTTOM;
        case (IGravity::AXIS_SPECIFIED | IGravity::AXIS_PULL_BEFORE | IGravity::AXIS_PULL_AFTER):
            return FILL;
        case IGravity::AXIS_SPECIFIED:
            return CENTER;
        case (IGravity::AXIS_SPECIFIED | IGravity::AXIS_PULL_BEFORE | IGravity::RELATIVE_LAYOUT_DIRECTION):
            return START;
        case (IGravity::AXIS_SPECIFIED | IGravity::AXIS_PULL_AFTER | IGravity::RELATIVE_LAYOUT_DIRECTION):
            return END;
        default:
            return UNDEFINED_ALIGNMENT;
    }
}

Int32 GridLayout::GetMargin1(
    /* [in] */ IView* view,
    /* [in] */ Boolean horizontal,
    /* [in] */ Boolean leading)
{
    AutoPtr<IGridLayoutLayoutParams> lp = GetLayoutParams(view);
    Int32 lMargin, rMargin, tMargin, bMargin;
    AutoPtr<IViewGroupMarginLayoutParams> mlp = IViewGroupMarginLayoutParams::Probe(lp);
    mlp->GetLeftMargin(&lMargin);
    mlp->GetRightMargin(&rMargin);
    mlp->GetTopMargin(&tMargin);
    mlp->GetBottomMargin(&bMargin);
    Int32 margin =  horizontal ?
            (leading ? lMargin : rMargin) :
            (leading ? tMargin : bMargin);
    return margin == UNDEFINED ? GetDefaultMargin(view, IViewGroupLayoutParams::Probe(lp), horizontal, leading) : margin;
}

AutoPtr<IGridLayoutLayoutParams> GridLayout::GetLayoutParams(
    /* [in] */ IView* c)
{
    AutoPtr<IViewGroupLayoutParams> lp;
    c->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    AutoPtr<IGridLayoutLayoutParams> glp = IGridLayoutLayoutParams::Probe(lp);
    return glp;
}

ECode GridLayout::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IViewGroupLayoutParams** params)
{
    VALIDATE_NOT_NULL(params);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IGridLayoutLayoutParams> lp;
    FAIL_RETURN(CGridLayoutLayoutParams::New(context, attrs, (IGridLayoutLayoutParams**)&lp));
    *params = IViewGroupLayoutParams::Probe(lp);
    REFCOUNT_ADD(*params);
    return NOERROR;
}

Int32 GridLayout::GetMeasurementIncludingMargin(
    /* [in] */ IView* c,
    /* [in] */ Boolean horizontal)
{
    Int32 visibility = 0;
    c->GetVisibility(&visibility);
    if (visibility == IView::GONE) {
        return 0;
    }
    return GetMeasurement(c, horizontal) + GetTotalMargin(c, horizontal);
}

ECode GridLayout::GetMeasurementIncludingMargin(
    /* [in] */ IView* c,
    /* [in] */ Boolean horizontal,
    /* [out] */ Int32* result)
{
    *result = GetMeasurementIncludingMargin(c, horizontal);
    return NOERROR;
}

ECode GridLayout::RequestLayout()
{
    ViewGroup::RequestLayout();
    InvalidateValues();
    return NOERROR;
}

AutoPtr<GridLayout::Alignment> GridLayout::GetAlignment(
    /* [in] */ Alignment* alignment,
    /* [in] */ Boolean horizontal)
{
    if (alignment != UNDEFINED_ALIGNMENT) {
        return alignment;
    } else {
        if (horizontal) {
            return START;
        } else {
            return BASELINE;
        }
    }
}

ECode GridLayout::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    ViewGroup::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> seq = CoreUtils::Convert(String("GridLayout"));
    IAccessibilityRecord::Probe(event)->SetClassName(seq);
    return NOERROR;
}

ECode GridLayout::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    ViewGroup::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> seq = CoreUtils::Convert(String("GridLayout"));
    info->SetClassName(seq);
    return NOERROR;
}

void GridLayout::OnSetLayoutParams(
    /* [in] */ IView* child,
    /* [in] */ IViewGroupLayoutParams* layoutParams)
{
    ViewGroup::OnSetLayoutParams(child, layoutParams);

    if (!CheckLayoutParams(layoutParams)) {
        String wt = String("supplied LayoutParams are of the wrong type");
        HandleInvalidParams(wt);
    }
    InvalidateStructure();
}

Boolean GridLayout::CheckLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    if (!IGridLayoutLayoutParams::Probe(p)) {
        return FALSE;
    }
    AutoPtr<IGridLayoutLayoutParams> lp = (IGridLayoutLayoutParams*)p;

    CheckLayoutParams(lp, TRUE);
    CheckLayoutParams(lp, FALSE);
    return TRUE;
}

AutoPtr<IViewGroupLayoutParams> GridLayout::GenerateDefaultLayoutParams()
{
    AutoPtr<IViewGroupLayoutParams> lp;
    CGridLayoutLayoutParams::New((IViewGroupLayoutParams**)&lp);
    return lp;
}

AutoPtr<IViewGroupLayoutParams> GridLayout::GenerateLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    AutoPtr<IViewGroupLayoutParams> lp;
    CGridLayoutLayoutParams::New(p, (IViewGroupLayoutParams**)&lp);
    return lp;
}

void GridLayout::OnDebugDrawMargins(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IPaint* paint)
{
    AutoPtr<IGridLayoutLayoutParams> lp;
    CGridLayoutLayoutParams::New((IGridLayoutLayoutParams**)&lp);
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> c;
        GetChildAt(i, (IView**)&c);
        IViewGroupMarginLayoutParams::Probe(lp)->SetMargins(
                GetMargin1(c, TRUE, TRUE),
                GetMargin1(c, FALSE, TRUE),
                GetMargin1(c, TRUE, FALSE),
                GetMargin1(c, FALSE, FALSE));
        IViewGroupLayoutParams::Probe(lp)->OnDebugDraw(c, canvas, paint);
    }
}

void GridLayout::OnDebugDraw(
    /* [in] */ ICanvas* canvas)
{
    AutoPtr<IPaint> paint;
    CPaint::New((IPaint**)&paint);
    paint->SetStyle(Elastos::Droid::Graphics::PaintStyle_STROKE);
    Int32 color = Color::Argb(50, 255, 255, 255);
    paint->SetColor(color);

    AutoPtr<IInsets> insets;
    GetOpticalInsets((IInsets**)&insets);

    Int32 w, h;
    GetWidth(&w);
    GetHeight(&h);
    Int32 it, il, ir, ib;
    insets->GetTop(&it);
    insets->GetLeft(&il);
    insets->GetRight(&ir);
    insets->GetBottom(&ib);

    Int32 top    =     mPaddingTop    + it;
    Int32 left   =     mPaddingLeft   + il;
    Int32 right  = w - mPaddingRight  - ir;
    Int32 bottom = h - mPaddingBottom - ib;

    AutoPtr< ArrayOf<Int32> > xs = mHorizontalAxis->mLocations;
    if (xs) {
        for (Int32 i = 0, length = xs->GetLength(); i < length; i++) {
            Int32 x = left + (*xs)[i];
            DrawLine(canvas, x, top, x, bottom, paint);
        }
    }

    AutoPtr< ArrayOf<Int32> > ys = mVerticalAxis->mLocations;
    if (ys) {
        for (Int32 i = 0, length = ys->GetLength(); i < length; i++) {
            Int32 y = top + (*ys)[i];
            DrawLine(canvas, left, y, right, y, paint);
        }
    }

    ViewGroup::OnDebugDraw(canvas);
}

void GridLayout::OnViewAdded(
    /* [in] */ IView* child)
{
    ViewGroup::OnViewAdded(child);
    InvalidateStructure();
}

void GridLayout::OnViewRemoved(
    /* [in] */ IView* child)
{
    ViewGroup::OnViewRemoved(child);
    InvalidateStructure();
}

void GridLayout::OnChildVisibilityChanged(
    /* [in] */ IView* child,
    /* [in] */ Int32 oldVisibility,
    /* [in] */ Int32 newVisibility)
{
    ViewGroup::OnChildVisibilityChanged(child, oldVisibility, newVisibility);
    if (oldVisibility == IView::GONE || newVisibility == IView::GONE) {
        InvalidateStructure();
    }
}

void GridLayout::OnMeasure(
    /* [in] */ Int32 widthSpec,
    /* [in] */ Int32 heightSpec)
{
    ConsistencyCheck();

    InvalidateValues();

    Int32 hPadding = mPaddingLeft + mPaddingRight;
    Int32 vPadding = mPaddingTop  + mPaddingBottom;

    Int32 widthSpecSansPadding =  Adjust(widthSpec, -hPadding);
    Int32 heightSpecSansPadding = Adjust(heightSpec, -vPadding);

    MeasureChildrenWithMargins(widthSpec, heightSpec, TRUE);

    Int32 widthSansPadding = 0, heightSansPadding = 0;

    if (mOrientation == ILinearLayout::HORIZONTAL) {
        widthSansPadding = mHorizontalAxis->GetMeasure(widthSpec);
        MeasureChildrenWithMargins(widthSpecSansPadding, heightSpecSansPadding, FALSE);
        heightSansPadding = mVerticalAxis->GetMeasure(heightSpecSansPadding);
    } else {
        heightSansPadding = mVerticalAxis->GetMeasure(heightSpecSansPadding);
        MeasureChildrenWithMargins(widthSpecSansPadding, heightSpecSansPadding, FALSE);
        widthSansPadding = mHorizontalAxis->GetMeasure(widthSpecSansPadding);
    }

    Int32 measuredWidth = Elastos::Core::Math::Max(hPadding + widthSansPadding, GetSuggestedMinimumWidth());
    Int32 measuredHeight = Elastos::Core::Math::Max(vPadding + heightSansPadding, GetSuggestedMinimumHeight());

    SetMeasuredDimension(
            ResolveSizeAndState(measuredWidth, widthSpec, 0),
            ResolveSizeAndState(measuredHeight, heightSpec, 0));
}

ECode GridLayout::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    ConsistencyCheck();

    Int32 targetWidth = mRight - mLeft;
    Int32 targetHeight = mBottom - mTop;

    mHorizontalAxis->Layout(targetWidth - mPaddingLeft - mPaddingRight);
    mVerticalAxis->Layout(targetHeight - mPaddingTop - mPaddingBottom);

    AutoPtr< ArrayOf<Int32> > hLocations = mHorizontalAxis->GetLocations();
    AutoPtr< ArrayOf<Int32> > vLocations = mVerticalAxis->GetLocations();

    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++)
    {
        AutoPtr<IView> c;
        GetChildAt(i, (IView**)&c);
        Int32 visibility = 0;
        c->GetVisibility(&visibility);
        if (visibility == IView::GONE) continue;
        AutoPtr<IGridLayoutLayoutParams> lp = GetLayoutParams(c);
        AutoPtr<IGridLayoutSpec> columnSpec, rowSpec;
        lp->GetColumnSpec((IGridLayoutSpec**)&columnSpec);
        lp->GetRowSpec((IGridLayoutSpec**)&rowSpec);

        AutoPtr<Spec> columnSpecImpl = (Spec*)columnSpec.Get();
        AutoPtr<Spec> rowSpecImpl = (Spec*)rowSpec.Get();

        AutoPtr<Interval> colSpan = columnSpecImpl->mSpan;
        AutoPtr<Interval> rowSpan = rowSpecImpl->mSpan;

        Int32 x1 = (*hLocations)[colSpan->mMin];
        Int32 y1 = (*vLocations)[rowSpan->mMin];

        Int32 x2 = (*hLocations)[colSpan->mMax];
        Int32 y2 = (*vLocations)[rowSpan->mMax];

        Int32 cellWidth = x2 - x1;
        Int32 cellHeight = y2 - y1;

        Int32 pWidth = GetMeasurement(c, TRUE);
        Int32 pHeight = GetMeasurement(c, FALSE);

        AutoPtr<Alignment> hAlign = GetAlignment(columnSpecImpl->mAlignment, TRUE);
        AutoPtr<Alignment> vAlign = GetAlignment(rowSpecImpl->mAlignment, FALSE);

        AutoPtr<Bounds> boundsX = (Bounds*)(mHorizontalAxis->GetGroupBounds()->GetValue(i).Get());
        AutoPtr<Bounds> boundsY = (Bounds*)(mVerticalAxis->GetGroupBounds()->GetValue(i).Get());

        Int32 gravityOffsetX = 0;
        hAlign->GetGravityOffset(c, cellWidth - boundsX->Size(TRUE), &gravityOffsetX);
        Int32 gravityOffsetY = 0;
        vAlign->GetGravityOffset(c, cellHeight - boundsY->Size(TRUE), &gravityOffsetY);

        Int32 leftMargin = GetMargin(c, TRUE, TRUE);
        Int32 topMargin = GetMargin(c, FALSE, TRUE);
        Int32 rightMargin = GetMargin(c, TRUE, FALSE);
        Int32 bottomMargin = GetMargin(c, FALSE, FALSE);

        Int32 sumMarginsX = leftMargin + rightMargin;
        Int32 sumMarginsY = topMargin + bottomMargin;

        Int32 alignmentOffsetX = boundsX->GetOffset(this, c, hAlign, pWidth + sumMarginsX, TRUE);
        Int32 alignmentOffsetY = boundsY->GetOffset(this, c, vAlign, pHeight + sumMarginsY, FALSE);

        Int32 width = 0;
        hAlign->GetSizeInCell(c, pWidth, cellWidth - sumMarginsX, &width);

        Int32 height = 0;
        vAlign->GetSizeInCell(c, pHeight, cellHeight - sumMarginsY, &height);


        Int32 dx = x1 + gravityOffsetX + alignmentOffsetX;

        Boolean isLayoutRtl;
        IsLayoutRtl(&isLayoutRtl);
        Int32 cx = !isLayoutRtl ? mPaddingLeft + leftMargin + dx :
                targetWidth - width - mPaddingRight - rightMargin - dx;

        Int32 cy = mPaddingTop + y1 + gravityOffsetY + alignmentOffsetY + topMargin;

        Int32 mw = 0, mh = 0;
        c->GetMeasuredWidth(&mw);
        c->GetMeasuredHeight(&mh);
        if (width != mw || height != mh) {
            c->Measure(View::MeasureSpec::MakeMeasureSpec(width, View::MeasureSpec::EXACTLY),
                View::MeasureSpec::MakeMeasureSpec(height, View::MeasureSpec::EXACTLY));
        }
        c->Layout(cx, cy, cx + width, cy + height);
    }
    return NOERROR;
}

Int32 GridLayout::Adjust(
    /* [in] */ Int32 measureSpec,
    /* [in] */ Int32 delta)
{
    return View::MeasureSpec::MakeMeasureSpec(
            MeasureSpec::GetSize(measureSpec + delta),  MeasureSpec::GetMode(measureSpec));
}

Int32 GridLayout::GetDefaultMargin(
    /* [in] */ IView* c,
    /* [in] */ Boolean horizontal,
    /* [in] */ Boolean leading)
{
    if (ISpace::Probe(c) != NULL) {
        return 0;
    }
    return mDefaultGap / 2;
}

Int32 GridLayout::GetDefaultMargin(
    /* [in] */ IView* c,
    /* [in] */ Boolean isAtEdge,
    /* [in] */ Boolean horizontal,
    /* [in] */ Boolean leading)
{
    return isAtEdge ? DEFAULT_CONTAINER_MARGIN : GetDefaultMargin(c, horizontal, leading);
}

Int32 GridLayout::GetDefaultMargin(
    /* [in] */ IView* c,
    /* [in] */ IViewGroupLayoutParams* p,
    /* [in] */ Boolean horizontal,
    /* [in] */ Boolean leading)
{
    if (!mUseDefaultMargins) {
        return 0;
    }
    AutoPtr<IGridLayoutLayoutParams> glp = IGridLayoutLayoutParams::Probe(p);
    AutoPtr<IGridLayoutSpec> columnSpec, rowSpec;
    glp->GetColumnSpec((IGridLayoutSpec**)&columnSpec);
    glp->GetRowSpec((IGridLayoutSpec**)&rowSpec);

    AutoPtr<Spec> spec = horizontal ? (Spec*)columnSpec.Get() : (Spec*)rowSpec.Get();
    AutoPtr<Axis> axis = horizontal ? mHorizontalAxis : mVerticalAxis;
    AutoPtr<Interval> span = spec->mSpan;

    Boolean isLayoutRtl;
    IsLayoutRtl(&isLayoutRtl);
    Boolean leading1 = (horizontal && isLayoutRtl) ? !leading : leading;
    Boolean isAtEdge = leading1 ? (span->mMin == 0) : (span->mMax == axis->GetCount());

    return GetDefaultMargin(c, isAtEdge, horizontal, leading);
}

Int32 GridLayout::GetMargin(
    /* [in] */ IView* view,
    /* [in] */ Boolean horizontal,
    /* [in] */ Boolean leading)
{
    if (mAlignmentMode == ALIGN_MARGINS) {
        return GetMargin1(view, horizontal, leading);
    } else {
        AutoPtr<Axis> axis = horizontal ? mHorizontalAxis : mVerticalAxis;
        AutoPtr< ArrayOf<Int32> > margins = leading ? axis->GetLeadingMargins() : axis->GetTrailingMargins();

        AutoPtr<IGridLayoutLayoutParams> lp = GetLayoutParams(view);
        AutoPtr<IGridLayoutSpec> columnSpec, rowSpec;
        lp->GetColumnSpec((IGridLayoutSpec**)&columnSpec);
        lp->GetRowSpec((IGridLayoutSpec**)&rowSpec);
        AutoPtr<Spec> spec = horizontal ? (Spec*)columnSpec.Get() : (Spec*)rowSpec.Get();
        Int32 index = leading ? spec->mSpan->mMin : spec->mSpan->mMax;
        return (*margins)[index];
    }
}

Int32 GridLayout::GetTotalMargin(
    /* [in] */ IView* child,
    /* [in] */ Boolean horizontal)
{
    return GetMargin(child, horizontal, TRUE) + GetMargin(child, horizontal, FALSE);
}

Boolean GridLayout::Fits(
    /* [in] */ ArrayOf<Int32>* a,
    /* [in] */ Int32 value,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    if (end > a->GetLength()) {
        return FALSE;
    }
    for (Int32 i = start; i < end; i++) {
        if ((*a)[i] > value) {
            return FALSE;
        }
    }
    return TRUE;
}

void GridLayout::ProcrusteanFill(
    /* [in] */ ArrayOf<Int32>* a,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 value)
{
    Int32 length = a->GetLength();
    if(start < 0 || end > length) {
        /*throw new ArrayIndexOutOfBoundsException("start < 0 || end > len."
                    + " start=" + start + ", end=" + end + ", len=" + len);*/
    }
    if (start > end) {
        //throw new IllegalArgumentException("start > end: " + start + " > " + end);
    }

    Int32 newStart = Elastos::Core::Math::Min(start, length);
    Int32 newEnd = Elastos::Core::Math::Min(end, length);

    for(Int32 i = newStart; i < newEnd; i++) {
        (*a)[i] = value;
    }
}

void GridLayout::SetCellGroup(
    /* [in] */ IViewGroupLayoutParams* lp,
    /* [in] */ Int32 row,
    /* [in] */ Int32 rowSpan,
    /* [in] */ Int32 col,
    /* [in] */ Int32 colSpan)
{
    AutoPtr<Interval> rowItv = new Interval(row, row + rowSpan);
    AutoPtr<Interval> colItv = new Interval(col, col + colSpan);
    AutoPtr<CGridLayoutLayoutParams> glp = (CGridLayoutLayoutParams*)lp;
    glp->SetRowSpecSpan(rowItv);
    glp->SetColumnSpecSpan(colItv);
}

Int32 GridLayout::Clip(
    /* [in] */ Interval* minorRange,
    /* [in] */ Boolean minorWasDefined,
    /* [in] */ Int32 count)
{
    Int32 size = minorRange->Size();
    if (count == 0) {
        return size;
    }
    Int32 min = minorWasDefined ? Elastos::Core::Math::Min(minorRange->mMin, count) : 0;
    return Elastos::Core::Math::Min(size, count - min);
}

void GridLayout::ValidateLayoutParams()
{
    Boolean horizontal = (mOrientation == HORIZONTAL);
    AutoPtr<Axis> axis = horizontal ? mHorizontalAxis : mVerticalAxis;
    Int32 count = (axis->mDefinedCount != UNDEFINED) ? axis->mDefinedCount : 0;

    Int32 major = 0;
    Int32 minor = 0;
    AutoPtr< ArrayOf<Int32> > maxSizes = ArrayOf<Int32>::Alloc(count);
    Int32 childCount;
    GetChildCount(&childCount);

    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> v;
        GetChildAt(i, (IView**)&v);
        AutoPtr<IViewGroupLayoutParams> lp;
        v->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        AutoPtr<IGridLayoutLayoutParams> glp = IGridLayoutLayoutParams::Probe(lp);

        AutoPtr<IGridLayoutSpec> columnSpec, rowSpec;
        glp->GetColumnSpec((IGridLayoutSpec**)&columnSpec);
        glp->GetRowSpec((IGridLayoutSpec**)&rowSpec);
        AutoPtr<Spec> majorSpec = horizontal ? (Spec*)rowSpec.Get() : (Spec*)columnSpec.Get();

        AutoPtr<Interval> majorRange = majorSpec->mSpan;
        Boolean majorWasDefined = majorSpec->mStartDefined;
        Int32 majorSpan = majorRange->Size();
        if (majorWasDefined) {
            major = majorRange->mMin;
        }

        AutoPtr<Spec> minorSpec = horizontal ? (Spec*)columnSpec.Get() : (Spec*)rowSpec.Get();
        AutoPtr<Interval> minorRange = minorSpec->mSpan;
        Boolean minorWasDefined = minorSpec->mStartDefined;
        Int32 minorSpan = Clip(minorRange, minorWasDefined, count);
        if (minorWasDefined) {
            minor = minorRange->mMin;
        }

        if (count != 0) {
            if (!majorWasDefined || !minorWasDefined) {
                while (!Fits(maxSizes, major, minor, minor + minorSpan)) {
                    if (minorWasDefined) {
                        major++;
                    } else {
                        if (minor + minorSpan <= count) {
                            minor++;
                        } else {
                            minor = 0;
                            major++;
                        }
                    }
                }
            }
            ProcrusteanFill(maxSizes, minor, minor + minorSpan, major + majorSpan);
        }

        if (horizontal) {
            SetCellGroup(lp, major, majorSpan, minor, minorSpan);
        } else {
            SetCellGroup(lp, minor, minorSpan, major, majorSpan);
        }

        minor = minor + minorSpan;
    }
}

void GridLayout::InvalidateStructure()
{
    mLastLayoutParamsHashCode = UNINITIALIZED_HASH;
    mHorizontalAxis->InvalidateStructure();
    mVerticalAxis->InvalidateStructure();
    InvalidateValues();
}

void GridLayout::InvalidateValues()
{
    if (mHorizontalAxis && mVerticalAxis) {
        mHorizontalAxis->InvalidateValues();
        mVerticalAxis->InvalidateValues();
    }
}

ECode GridLayout::HandleInvalidParams(
    /* [in] */ const String& msg)
{
    //////////////////////////////// is right
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

void GridLayout::CheckLayoutParams(
    /* [in] */ IGridLayoutLayoutParams* lp,
    /* [in] */ Boolean horizontal)
{
    String groupName = horizontal ? String("column") : String("row");
    AutoPtr<IGridLayoutSpec> columnSpec, rowSpec;
    lp->GetColumnSpec((IGridLayoutSpec**)&columnSpec);
    lp->GetRowSpec((IGridLayoutSpec**)&rowSpec);
    AutoPtr<Spec> spec = horizontal ? (Spec*)columnSpec.Get() : (Spec*)rowSpec.Get();
    AutoPtr<Interval> span = spec->mSpan;
    if (span->mMin != UNDEFINED && span->mMin < 0) {
        String wt = groupName + String(" indices must be positive");
        HandleInvalidParams(wt);
    }
    AutoPtr<Axis> axis = horizontal ? mHorizontalAxis : mVerticalAxis;
    Int32 count = axis->mDefinedCount;
    if (count != UNDEFINED) {
        if (span->mMax > count) {
            String wt = groupName + String(" indices (start + span) mustn't exceed the ") + groupName + String(" count");
            HandleInvalidParams(wt);
        }
        if (span->Size() > count) {
            String wt = groupName + String(" span mustn't exceed the ") + groupName + String(" count");
            HandleInvalidParams(wt);
        }
    }
}

void GridLayout::DrawLine(
    /* [in] */ ICanvas* graphics,
    /* [in] */ Int32 x1,
    /* [in] */ Int32 y1,
    /* [in] */ Int32 x2,
    /* [in] */ Int32 y2,
    /* [in] */ IPaint* paint)
{
    Boolean isLayoutRtl;
    IsLayoutRtl(&isLayoutRtl);
    if (isLayoutRtl) {
        Int32 width;
        GetWidth(&width);
        graphics->DrawLine(width - x1, y1, width - x2, y2, paint);
    } else {
        graphics->DrawLine(x1, y1, x2, y2, paint);
    }
}

Int32 GridLayout::ComputeLayoutParamsHashCode()
{
    Int32 result = 1;
    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> c;
        GetChildAt(i, (IView**)&c);
        Int32 visibility = 0;
        c->GetVisibility(&visibility);
        if (visibility == IView::GONE) continue;
        AutoPtr<IViewGroupLayoutParams> lp;
        c->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        IGridLayoutLayoutParams * glp = IGridLayoutLayoutParams::Probe(lp.Get());
        Int32 hash;
        IObject::Probe(glp)->GetHashCode(&hash);
        result = 31 * result + hash;
    }
    return result;
}

void GridLayout::ConsistencyCheck()
{
    if (mLastLayoutParamsHashCode == UNINITIALIZED_HASH) {
        ValidateLayoutParams();
        mLastLayoutParamsHashCode = ComputeLayoutParamsHashCode();
    } else if (mLastLayoutParamsHashCode != ComputeLayoutParamsHashCode()) {
        SLOGGERW(TAG, String("The fields of some layout parameters were modified in between layout") +
            " operations. Check the javadoc for GridLayout.LayoutParams#rowSpec.");
        InvalidateStructure();
        ConsistencyCheck();
    }
}

void GridLayout::MeasureChildWithMargins2(
    /* [in] */ IView* child,
    /* [in] */ Int32 parentWidthSpec,
    /* [in] */ Int32 parentHeightSpec,
    /* [in] */ Int32 childWidth,
    /* [in] */ Int32 childHeight)
{
    Int32 childWidthSpec = GetChildMeasureSpec(parentWidthSpec,
            GetTotalMargin(child, TRUE), childWidth);
    Int32 childHeightSpec = GetChildMeasureSpec(parentHeightSpec,
            GetTotalMargin(child, FALSE), childHeight);

    child->Measure(childWidthSpec, childHeightSpec);
}

void GridLayout::MeasureChildrenWithMargins(
    /* [in] */ Int32 widthSpec,
    /* [in] */ Int32 heightSpec,
    /* [in] */ Boolean firstPass)
{
    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> c;
        GetChildAt(i, (IView**)&c);
        Int32 visibility = 0;
        c->GetVisibility(&visibility);
        if (visibility == IView::GONE) continue;
        AutoPtr<IGridLayoutLayoutParams> glp = GetLayoutParams(c);
        AutoPtr<IViewGroupLayoutParams> lp = IViewGroupLayoutParams::Probe(glp);
        Int32 width, height;
        lp->GetWidth(&width);
        lp->GetHeight(&height);
        if (firstPass) {
            MeasureChildWithMargins2(c, widthSpec, heightSpec, width, height);
            mHorizontalAxis->RecordOriginalMeasurement(i);
            mVerticalAxis->RecordOriginalMeasurement(i);
        } else {
            Boolean horizontal = (mOrientation == HORIZONTAL);
            AutoPtr<IGridLayoutSpec> columnSpec, rowSpec;
            glp->GetColumnSpec((IGridLayoutSpec**)&columnSpec);
            glp->GetRowSpec((IGridLayoutSpec**)&rowSpec);
            AutoPtr<Spec> spec = horizontal ? (Spec*)columnSpec.Get() : (Spec*)rowSpec.Get();
            if (spec->mAlignment == FILL) {
                AutoPtr<Interval> span = spec->mSpan;
                AutoPtr<Axis> axis = horizontal ? mHorizontalAxis : mVerticalAxis;
                AutoPtr< ArrayOf<Int32> > locations = axis->GetLocations();
                Int32 cellSize = (*locations)[span->mMax] - (*locations)[span->mMin];
                Int32 viewSize = cellSize - GetTotalMargin(c, horizontal);
                if (horizontal) {
                    MeasureChildWithMargins2(c, widthSpec, heightSpec, viewSize, height);
                } else {
                    MeasureChildWithMargins2(c, widthSpec, heightSpec, width, viewSize);
                }
            }
        }
    }
}

Int32 GridLayout::GetMeasurement(
    /* [in] */ IView* c,
    /* [in] */ Boolean horizontal)
{
    Int32 result = 0;
    if (horizontal) {
        c->GetMeasuredWidth(&result);
    } else {
        c->GetMeasuredHeight(&result);
    }
    return result;
}

AutoPtr<GridLayout::Alignment> GridLayout::CreateSwitchingAlignment(
    /* [in] */ Alignment* ltr,
    /* [in] */ Alignment* rtl)
{
    AutoPtr<Alignment> al = new SwitchingAlignment(ltr, rtl);
    return al;
}

Boolean GridLayout::CanStretch(
    /* [in] */ Int32 flexibility)
{
    return (flexibility & CAN_STRETCH) != 0;
}

AutoPtr<IGridLayoutSpec> GridLayout::GetSpec(
    /* [in] */ Int32 start,
    /* [in] */ Int32 size,
    /* [in] */ Alignment* alignment)
{
    return GetSpec(start, size, alignment, Spec::DEFAULT_WEIGHT);
}

AutoPtr<IGridLayoutSpec> GridLayout::GetSpec(
    /* [in] */ Int32 start,
    /* [in] */ Alignment* alignment)
{
    return GetSpec(start, 1, alignment);
}

AutoPtr<IGridLayoutSpec> GridLayout::GetSpec(
    /* [in] */ Int32 start,
    /* [in] */ Int32 size)
{
    return GetSpec(start, size, UNDEFINED_ALIGNMENT);
}

AutoPtr<IGridLayoutSpec> GridLayout::GetSpec(
    /* [in] */ Int32 start)
{
    return GetSpec(start, 1);
}

AutoPtr<IGridLayoutSpec> GridLayout::GetSpec(
    /* [in] */ Int32 start,
    /* [in] */ Int32 size,
    /* [in] */ Alignment* alignment,
    /* [in] */ Float weight)
{
    AutoPtr<IGridLayoutSpec> result = new Spec(start != UNDEFINED, start, size, alignment, weight);
    return result;
}

AutoPtr<IGridLayoutSpec> GridLayout::GetSpec(
    /* [in] */ Int32 start,
    /* [in] */ Alignment* alignment,
    /* [in] */ Float weight)
{
    return GetSpec(start, 1, alignment, weight);
}

AutoPtr<IGridLayoutSpec> GridLayout::GetSpec(
    /* [in] */ Int32 start,
    /* [in] */ Int32 size,
    /* [in] */ Float weight)
{
    return GetSpec(start, size, UNDEFINED_ALIGNMENT, weight);
}

AutoPtr<IGridLayoutSpec> GridLayout::GetSpec(
    /* [in] */ Int32 start,
    /* [in] */ Float weight)
{
    return GetSpec(start, 1, weight);
}

//==================================================================
//                      GridLayout::Axis
//==================================================================
const Int32 GridLayout::Axis::NEW;
const Int32 GridLayout::Axis::PENDING;
const Int32 GridLayout::Axis::COMPLETE;

GridLayout::Axis::Axis(
    /* [in] */ Boolean horizontal,
    /* [in] */ GridLayout* host)
    : mHost(host)
    , mMaxIndex(GridLayout::UNDEFINED)
    , mHorizontal(horizontal)
    , mDefinedCount(GridLayout::UNDEFINED)
    , mGroupBoundsValid(FALSE)
    , mForwardLinksValid(FALSE)
    , mBackwardLinksValid(FALSE)
    , mLeadingMarginsValid(FALSE)
    , mTrailingMarginsValid(FALSE)
    , mArcsValid(FALSE)
    , mLocationsValid(FALSE)
    , mHasWeights(FALSE)
    , mHasWeightsValid(FALSE)
    , mOrderPreserved(GridLayout::DEFAULT_ORDER_PRESERVED)
{
    mParentMin = new MutableInt(0);
    mParentMax = new MutableInt(-GridLayout::MAX_SIZE);
}

Int32 GridLayout::Axis::CalculateMaxIndex()
{
    Int32 result = -1;
    Int32 childCount;
    mHost->GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> c;
        mHost->GetChildAt(i, (IView**)&c);
        AutoPtr<IGridLayoutLayoutParams> glp = mHost->GetLayoutParams(c);
        AutoPtr<IGridLayoutSpec> columnSpec, rowSpec;
        glp->GetColumnSpec((IGridLayoutSpec**)&columnSpec);
        glp->GetRowSpec((IGridLayoutSpec**)&rowSpec);
        AutoPtr<Spec> spec = mHorizontal ? (Spec*)columnSpec.Get() : (Spec*)rowSpec.Get();
        AutoPtr<Interval> span = spec->mSpan;
        result = Elastos::Core::Math::Max(result, span->mMin);
        result = Elastos::Core::Math::Max(result, span->mMax);
        result = Elastos::Core::Math::Max(result, span->Size());
    }
    return result == -1 ? GridLayout::UNDEFINED : result;
}

Int32 GridLayout::Axis::GetMaxIndex()
{
    if (mMaxIndex == GridLayout::UNDEFINED) {
        mMaxIndex = Elastos::Core::Math::Max(0, CalculateMaxIndex());
    }
    return mMaxIndex;
}

AutoPtr<GridLayout::PackedMap<AutoPtr<IGridLayoutSpec>, AutoPtr<IBounds> > > GridLayout::Axis::CreateGroupBounds()
{
    Assoc<AutoPtr<IGridLayoutSpec>, AutoPtr<IBounds> >* assoc = Assoc<AutoPtr<IGridLayoutSpec>, AutoPtr<IBounds> >::Of(NULL, NULL);
    Int32 childCount;
    mHost->GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> c;
        mHost->GetChildAt(i, (IView**)&c);
        // we must include views that are GONE here, see introductory javadoc
        AutoPtr<IGridLayoutLayoutParams> glp = mHost->GetLayoutParams(c);
        AutoPtr<IGridLayoutSpec> columnSpec, rowSpec;
        glp->GetColumnSpec((IGridLayoutSpec**)&columnSpec);
        glp->GetRowSpec((IGridLayoutSpec**)&rowSpec);

        AutoPtr<Spec> spec = mHorizontal ? (Spec*)columnSpec.Get() : (Spec*)rowSpec.Get();
        AutoPtr<Bounds> bounds = mHost->GetAlignment(spec->mAlignment, mHorizontal)->GetBounds();
        AutoPtr<IGridLayoutSpec> specParam = spec;
        assoc->Put(specParam, bounds);
    }
    AutoPtr<PackedMap<AutoPtr<IGridLayoutSpec>, AutoPtr<IBounds> > > map = assoc->Pack();
    delete assoc;
    return map;
}

void GridLayout::Axis::ComputeGroupBounds()
{
    AutoPtr< ArrayOf<AutoPtr<IBounds> > > values = mGroupBounds->mValues;
    for (Int32 i = 0; i < values->GetLength(); i++) {
        Bounds* bounds = (Bounds*)((*values)[i].Get());
        bounds->Reset();
    }
    Int32 childCount;
    mHost->GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> c;
        mHost->GetChildAt(i, (IView**)&c);
        // we must include views that are GONE here, see introductory javadoc
        AutoPtr<IGridLayoutLayoutParams> glp = mHost->GetLayoutParams(c);
        AutoPtr<IGridLayoutSpec> columnSpec, rowSpec;
        glp->GetColumnSpec((IGridLayoutSpec**)&columnSpec);
        glp->GetRowSpec((IGridLayoutSpec**)&rowSpec);
        AutoPtr<Spec> spec = mHorizontal ? (Spec*)columnSpec.Get() : (Spec*)rowSpec.Get();
        AutoPtr<ArrayOf<Int32> > deltas = GetDeltas();
        Int32 margin = mHost->GetMeasurementIncludingMargin(c, mHorizontal);
        AutoPtr<ArrayOf<Int32> > measurements = GetOriginalMeasurements();
        int size = (spec->mWeight == 0) ?
                margin : (*measurements)[i] + (*deltas)[i];
        AutoPtr<IBounds> bounds = mGroupBounds->GetValue(i);
        Bounds* boundsImp = (Bounds*)bounds.Get();
        boundsImp->Include(mHost, c, spec, this, size);
    }
}

AutoPtr< GridLayout::PackedMap<AutoPtr<IInterval>, AutoPtr<GridLayout::MutableInt> > > GridLayout::Axis::CreateLinks(
    /* [in] */ Boolean min)
{
    Assoc< AutoPtr<IInterval>, AutoPtr<MutableInt> >* result = Assoc< AutoPtr<IInterval>, AutoPtr<MutableInt> >::Of(NULL, NULL);
    AutoPtr< ArrayOf<AutoPtr<IGridLayoutSpec> > > keys = GetGroupBounds()->mKeys;
    for (Int32 i = 0, N = keys->GetLength(); i < N; i++) {
        AutoPtr<Spec> sp = (Spec*)((*keys)[i].Get());
        AutoPtr<Interval> span = min ? sp->mSpan : sp->mSpan->Inverse();
        AutoPtr<MutableInt> mut = new MutableInt();
        result->Put(span.Get(), mut);
    }
    AutoPtr<PackedMap<AutoPtr<IInterval>, AutoPtr<MutableInt> > > map = result->Pack();
    delete result;
    return map;
}

void GridLayout::Axis::ComputeLinks(
    /* [in] */ PackedMap<AutoPtr<IInterval>, AutoPtr<MutableInt> >* links,
    /* [in] */ Boolean min)
{
    AutoPtr< ArrayOf<AutoPtr<MutableInt> > > spans = links->mValues;
    for (Int32 i = 0; i < spans->GetLength(); i++) {
        (*spans)[i]->Reset();
    }

    AutoPtr< ArrayOf<AutoPtr<IBounds> > > bounds = GetGroupBounds()->mValues;
    for (Int32 i = 0; i < bounds->GetLength(); i++) {
        Bounds* boundsImpl = (Bounds*)((*bounds)[i].Get());
        Int32 size = boundsImpl->Size(min);
        AutoPtr<MutableInt> valueHolder = links->GetValue(i);
        valueHolder->mValue = Elastos::Core::Math::Max(valueHolder->mValue, min ? size : -size);
    }
}

AutoPtr< GridLayout::PackedMap<AutoPtr<IInterval>, AutoPtr<GridLayout::MutableInt> > > GridLayout::Axis::GetForwardLinks()
{
    if (!mForwardLinks) {
        mForwardLinks = CreateLinks(TRUE);
    }
    if (!mForwardLinksValid) {
        ComputeLinks(mForwardLinks, TRUE);
        mForwardLinksValid = TRUE;
    }
    return mForwardLinks;
}

AutoPtr< GridLayout::PackedMap<AutoPtr<IInterval>, AutoPtr<GridLayout::MutableInt> > > GridLayout::Axis::GetBackwardLinks()
{
    if (!mBackwardLinks) {
        mBackwardLinks = CreateLinks(FALSE);
    }
    if (!mBackwardLinksValid) {
        ComputeLinks(mBackwardLinks, FALSE);
        mBackwardLinksValid = TRUE;
    }
    return mBackwardLinks;
}

void GridLayout::Axis::Include(
    /* [in] */ List< AutoPtr<Arc> >& arcs,
    /* [in] */ Interval* key,
    /* [in] */ MutableInt* size,
    /* [in] */ Boolean ignoreIfAlreadyPresent)
{
    if (key->Size() == 0) {
        return;
    }
    if (ignoreIfAlreadyPresent) {
        List< AutoPtr<Arc> >::Iterator it = arcs.Begin();
        for (; it != arcs.End(); it++)
        {
            AutoPtr<Interval> span = (*it)->mSpan;
            Boolean equal;
            AutoPtr<IObject> objKey = key;
            if (span->Equals(objKey, &equal), equal) {
                return;
            }
        }
    }
    AutoPtr<Arc> obj = new Arc(key, size);
    arcs.PushBack(obj);
}

void GridLayout::Axis::Include(
    /* [in] */ List< AutoPtr<Arc> >& arcs,
    /* [in] */ Interval* key,
    /* [in] */ MutableInt* size)
{
    Include(arcs, key, size, TRUE);
}

AutoPtr< ArrayOf<AutoPtr<ArrayOf<GridLayout::Arc*> > > > GridLayout::Axis::GroupArcsByFirstVertex(
    /* [in] */ AutoPtr< ArrayOf<Arc*> > arcs)
{
    Int32 N = GetCount() + 1;

    AutoPtr< ArrayOf<AutoPtr<ArrayOf<GridLayout::Arc*> > > > result = ArrayOf<AutoPtr<ArrayOf<GridLayout::Arc*> > >::Alloc(N);

    Int32 sizes[N];
    for (UInt32 i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++) {
        sizes[i] = 0;
    }
    for (Int32 i = 0; i < arcs->GetLength(); i++) {
        AutoPtr<Arc> firstArc = (*arcs)[i];
        sizes[firstArc->mSpan->mMin]++;
    }
    for (UInt32 i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++) {
        AutoPtr<ArrayOf<Arc*> > Arcn = ArrayOf<Arc*>::Alloc(sizes[i]);
        result->Set(i, Arcn);
    }
    for (UInt32 i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++) {
        sizes[i] = 0;
    }
    for (Int32 i = 0; i < arcs->GetLength(); i++) {
        AutoPtr<Arc> secondArc = (*arcs)[i];
        Int32 index = secondArc->mSpan->mMin;
        (*result)[index]->Set(sizes[index]++, secondArc);
    }
    return result;
}

AutoPtr< ArrayOf<GridLayout::Arc*> > GridLayout::Axis::TopologicalSort(
    /* [in] */ ArrayOf<Arc*>* arcs)
{
    Int32 len = arcs->GetLength();
    AutoPtr< ArrayOf<Arc*> > result = ArrayOf<Arc*>::Alloc(len);
    Int32 cursor = len - 1;
    AutoPtr< ArrayOf<AutoPtr<ArrayOf<GridLayout::Arc*> > > > arcsByVertex = GroupArcsByFirstVertex(arcs);
    AutoPtr< ArrayOf<Int32> > visited = ArrayOf<Int32>::Alloc(GetCount() + 1);
    AutoPtr<TopoSort> sort = new TopoSort(result, cursor, arcsByVertex, visited);
    return sort->Sort();
    /*return new Object() {
        Arc[] result = new Arc[arcs.length];
        int cursor = result.length - 1;
        Arc[][] arcsByVertex = groupArcsByFirstVertex(arcs);
        int[] visited = new int[getCount() + 1];

        void walk(int loc) {
            switch (visited[loc]) {
                case NEW: {
                    visited[loc] = PENDING;
                    for (Arc arc : arcsByVertex[loc]) {
                        walk(arc.span.max);
                        result[cursor--] = arc;
                    }
                    visited[loc] = COMPLETE;
                    break;
                }
                case PENDING: {
                    // le singe est dans l'arbre
                    assert false;
                    break;
                }
                case COMPLETE: {
                    break;
                }
            }
        }

        Arc[] sort() {
            for (int loc = 0, N = arcsByVertex.length; loc < N; loc++) {
                walk(loc);
            }
            assert cursor == -1;
            return result;
        }
    }.sort();*/
}

AutoPtr< ArrayOf<GridLayout::Arc*> > GridLayout::Axis::TopologicalSort(
    /* [in] */ List< AutoPtr<Arc> >& arcs)
{
    AutoPtr< ArrayOf<Arc*> > result = ArrayOf<Arc*>::Alloc(arcs.GetSize());
    List< AutoPtr<Arc> >::Iterator it = arcs.Begin();
    for(Int32 i = 0; it != arcs.End(); it++, i++) {
        result->Set(i, *it);
    }
    return TopologicalSort(result);
}

void GridLayout::Axis::AddComponentSizes(
    /* [in] */ List< AutoPtr<Arc> >& result,
    /* [in] */ PackedMap<AutoPtr<IInterval>, AutoPtr<MutableInt> >* links)
{
    for (Int32 i = 0; i < links->mKeys->GetLength(); i++) {
        AutoPtr<IInterval> tmp = (*links->mKeys)[i];
        AutoPtr<Interval> key = (Interval*)tmp.Get();
        Include(result, key, (*links->mValues)[i], FALSE);
    }
}

AutoPtr< ArrayOf<GridLayout::Arc*> > GridLayout::Axis::CreateArcs()
{
    List< AutoPtr<Arc> > mins, maxs;

    AddComponentSizes(mins, GetForwardLinks());
    AddComponentSizes(maxs, GetBackwardLinks());

    if (mOrderPreserved) {
        for (Int32 i = 0; i < GetCount(); i++) {
            AutoPtr<Interval> val = new Interval(i, i + 1);
            AutoPtr<MutableInt> mut = new MutableInt(0);
            Include(mins, val, mut);
        }
    }

    Int32 N = GetCount();
    AutoPtr<Interval> firVal = new Interval(0, N);
    AutoPtr<Interval> secVal = new Interval(N, 0);
    Include(mins, firVal, mParentMin, FALSE);
    Include(maxs, secVal, mParentMax, FALSE);

    AutoPtr< ArrayOf<Arc*> > sMins = TopologicalSort(mins);
    AutoPtr< ArrayOf<Arc*> > sMaxs = TopologicalSort(maxs);
    /*AutoPtr< ArrayOf<Arc*> > test = mHost->Append(sMins.Get(), sMaxs.Get());
    return test;*/
    return mHost->Append(sMins.Get(), sMaxs.Get());
}

void GridLayout::Axis::ComputeArcs()
{
    GetForwardLinks();
    GetBackwardLinks();
}

Boolean GridLayout::Axis::Relax(
    /* [in] */ ArrayOf<Int32>* locations,
    /* [in] */ Arc* entry)
{
    if (!entry->mValid) {
        return FALSE;
    }
    AutoPtr<Interval> span = entry->mSpan;
    Int32 u = span->mMin;
    Int32 v = span->mMax;
    Int32 value = entry->mValue->mValue;
    Int32 candidate = (*locations)[u] + value;
    if (candidate > (*locations)[v]) {
        (*locations)[v] = candidate;
        return TRUE;
    }
    return FALSE;
}

void GridLayout::Axis::Init(
    /* [in] */ ArrayOf<Int32>* locations)
{
    for (Int32 i = 0; i < locations->GetLength(); i++) {
        (*locations)[i] = 0;
    }
}

String GridLayout::Axis::ArcsToString(
    /* [in] */ List< AutoPtr<Arc> >& arcs)
{
    String var = mHorizontal ? String("x") : String("y");
    StringBuilder result;
    Boolean first = TRUE;
    List< AutoPtr<Arc> >::Iterator it = arcs.Begin();
    for (; it != arcs.End(); it++) {
        if (first) {
            first = FALSE;
        } else {
            result += String(", ");
        }
        Int32 src = (*it)->mSpan->mMin;
        Int32 dst = (*it)->mSpan->mMax;
        Int32 value = (*it)->mValue->mValue;
        if (src < dst) {
            result += var;
            result += dst;
            result += String("-");
            result += var;
            result += src;
            result += String(">=");
            result += value;
        } else {
            result += var;
            result += src;
            result += String("-");
            result += var;
            result += dst;
            result += String("<=");
            result += -value;
        }
    }
    String str;
    result.ToString(&str);
    return str;
}

void GridLayout::Axis::LogError(
    /* [in] */ const String& axisName,
    /* [in] */ ArrayOf<Arc*>* arcs,
    /* [in] */ ArrayOf<Boolean>* culprits0)
{
    List< AutoPtr<Arc> > culprits;
    List< AutoPtr<Arc> > removed;
    for (Int32 c = 0; c < arcs->GetLength(); c++) {
        AutoPtr<Arc> arc = (*arcs)[c];
        if ((*culprits0)[c]) {
            culprits.PushBack(arc);
        }
        if (!arc->mValid) {
            removed.PushBack(arc);
        }
    }
    /*Log.d(TAG, axisName + " constraints: " + arcsToString(culprits) + " are inconsistent; "
            + "permanently removing: " + arcsToString(removed) + ". ");*/
}

void GridLayout::Axis::Solve(
    /* [in] */ ArrayOf<Arc*>* arcs,
    /* [in] */ ArrayOf<Int32>* locations)
{

    String axisName = mHorizontal ? String("horizontal") : String("vertical");
    Int32 N = GetCount() + 1;
    AutoPtr< ArrayOf<Boolean> > originalCulprits = NULL;
    for (Int32 p = 0; p < arcs->GetLength(); p++) {
        Init(locations);

        for (Int32 i = 0; i < N; i++) {
            Boolean changed = FALSE;
            for (Int32 j = 0, length = arcs->GetLength(); j < length; j++) {
                changed |= Relax(locations, (*arcs)[j]);
            }
            if (!changed) {
                if (originalCulprits) {
                    LogError(axisName, arcs, originalCulprits);
                }
                return;
            }
        }

        AutoPtr< ArrayOf<Boolean> > culprits = ArrayOf<Boolean>::Alloc(arcs->GetLength());
        for (Int32 i = 0; i < N; i++) {
            for (Int32 j = 0, length = arcs->GetLength(); j < length; j++) {
                (*culprits)[j] |= Relax(locations, (*arcs)[j]);
            }
        }

        if (p == 0) {
            originalCulprits = culprits;
        }

        for (Int32 i = 0; i < arcs->GetLength(); i++) {
            if ((*culprits)[i]) {
                AutoPtr<Arc> arc = (*arcs)[i];
                if (arc->mSpan->mMin < arc->mSpan->mMax) {
                    continue;
                }
                arc->mValid = FALSE;
                break;
            }
        }
    }
}

void GridLayout::Axis::ComputeMargins(
    /* [in] */ Boolean leading)
{
    AutoPtr< ArrayOf<Int32> > margins = leading ? mLeadingMargins : mTrailingMargins;
    Int32 childCount;
    mHost->GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> c;
        mHost->GetChildAt(i, (IView**)&c);
        Int32 visibility = 0;
        c->GetVisibility(&visibility);
        if (visibility == IView::GONE) continue;
        AutoPtr<IGridLayoutLayoutParams> glp = mHost->GetLayoutParams(c);
        AutoPtr<IGridLayoutSpec> columnSpec, rowSpec;
        glp->GetColumnSpec((IGridLayoutSpec**)&columnSpec);
        glp->GetRowSpec((IGridLayoutSpec**)&rowSpec);

        AutoPtr<Spec> spec = mHorizontal ? (Spec*)columnSpec.Get() : (Spec*)rowSpec.Get();
        AutoPtr<Interval> span = spec->mSpan;
        Int32 index = leading ? span->mMin : span->mMax;
        (*margins)[index] = Elastos::Core::Math::Max((*margins)[index], mHost->GetMargin1(c, mHorizontal, leading));
    }
}

void GridLayout::Axis::Solve(
    /* [in] */ ArrayOf<Int32>* a)
{
    Solve(GetArcs(), a);
}

Boolean GridLayout::Axis::ComputeHasWeights()
{
    Int32 childCount;
    mHost->GetChildCount(&childCount);
    for (int i = 0; i < childCount; i++) {
        AutoPtr<IView> v;
        mHost->GetChildAt(i, (IView**)&v);
        AutoPtr<IGridLayoutLayoutParams> lp = mHost->GetLayoutParams(v);
        AutoPtr<IGridLayoutSpec> columnSpec, rowSpec;
        lp->GetColumnSpec((IGridLayoutSpec**)&columnSpec);
        lp->GetRowSpec((IGridLayoutSpec**)&rowSpec);

        Spec* spec = mHorizontal ? (Spec*)columnSpec.Get() : (Spec*)rowSpec.Get();
        if (spec->mWeight != 0) {
            return TRUE;
        }
    }
    return FALSE;
}

Boolean GridLayout::Axis::HasWeights()
{
    if (!mHasWeightsValid) {
        mHasWeights = ComputeHasWeights();
        mHasWeightsValid = TRUE;
    }
    return mHasWeights;
}

void GridLayout::Axis::RecordOriginalMeasurement(
    /* [in] */ Int32 i)
{
    if (HasWeights()) {
        AutoPtr<ArrayOf<Int32> > arr = GetOriginalMeasurements();
        AutoPtr<IView> v;
        mHost->GetChildAt(i, (IView**)&v);
        (*arr)[i] = mHost->GetMeasurementIncludingMargin(v, mHorizontal);
    }
}

void GridLayout::Axis::ShareOutDelta()
{
    Int32 totalDelta = 0;
    Float totalWeight = 0;
    Int32 childCount;
    mHost->GetChildCount(&childCount);
    for (int i = 0; i < childCount; i++) {
        AutoPtr<IView> c;
        mHost->GetChildAt(i, (IView**)&c);
        AutoPtr<IGridLayoutSpec> columnSpec, rowSpec;
        AutoPtr<IGridLayoutLayoutParams> lp = mHost->GetLayoutParams(c);
        lp->GetColumnSpec((IGridLayoutSpec**)&columnSpec);
        lp->GetRowSpec((IGridLayoutSpec**)&rowSpec);
        Spec* spec = mHorizontal ? (Spec*)columnSpec.Get() : (Spec*)rowSpec.Get();
        Float weight = spec->mWeight;

        if (weight != 0) {
            AutoPtr<ArrayOf<Int32> > measurements = GetOriginalMeasurements();
            Int32 delta = mHost->GetMeasurement(c, mHorizontal) - (*measurements)[i];
            totalDelta += delta;
            totalWeight += weight;
        }
    }

    mHost->GetChildCount(&childCount);
    for (int i = 0; i < childCount; i++) {
        AutoPtr<IView> c;
        mHost->GetChildAt(i, (IView**)&c);
        AutoPtr<IGridLayoutSpec> columnSpec, rowSpec;
        AutoPtr<IGridLayoutLayoutParams> lp = mHost->GetLayoutParams(c);
        lp->GetColumnSpec((IGridLayoutSpec**)&columnSpec);
        lp->GetRowSpec((IGridLayoutSpec**)&rowSpec);
        Spec* spec = mHorizontal ? (Spec*)columnSpec.Get() : (Spec*)rowSpec.Get();
        Float weight = spec->mWeight;

        if (weight != 0) {
            Int32 delta = Elastos::Core::Math::Round((weight * totalDelta / totalWeight));
            (*mDeltas)[i] = delta;
            // the two adjustments below are to counter the above rounding and avoid off-by-ones at the end
            totalDelta -= delta;
            totalWeight -= weight;
        }
    }
}

void GridLayout::Axis::SolveAndDistributeSpace(
    /* [in] */ ArrayOf<Int32>* a)
{
    AutoPtr<ArrayOf<Int32> > deltas = GetDeltas();
    for(Int32 i = 0; i < deltas->GetLength(); i++) {
        (*deltas)[i] = 0;
    }
    Solve(a);
    ShareOutDelta();
    mArcsValid = FALSE;
    mForwardLinksValid = FALSE;
    mBackwardLinksValid = FALSE;
    mGroupBoundsValid = FALSE;
    Solve(a);
}

void GridLayout::Axis::ComputeLocations(
    /* [in] */ ArrayOf<Int32>* a)
{
    if (!HasWeights()) {
        Solve(a);
    } else {
        SolveAndDistributeSpace(a);
    }
    if (!mOrderPreserved) {
        Int32 a0 = (*a)[0];
        for (Int32 i = 0, N = a->GetLength(); i < N; i++) {
            (*a)[i] = (*a)[i] - a0;
        }
    }
}

Int32 GridLayout::Axis::Size(
    /* [in] */ ArrayOf<Int32>* locations)
{
    return (*locations)[GetCount()];
}

void GridLayout::Axis::SetParentConstraints(
    /* [in] */ Int32 min,
    /* [in] */ Int32 max)
{
    mParentMin->mValue = min;
    mParentMax->mValue = -max;
    mLocationsValid = FALSE;
}

Int32 GridLayout::Axis::GetMeasure(
    /* [in] */ Int32 min,
    /* [in] */ Int32 max)
{
    SetParentConstraints(min, max);
    return Size(GetLocations());
}

Int32 GridLayout::Axis::GetCount()
{
    return Elastos::Core::Math::Max(mDefinedCount, GetMaxIndex());
}

void GridLayout::Axis::SetCount(
    /* [in] */ Int32 count)
{
    if (count != GridLayout::UNDEFINED && count < GetMaxIndex()) {
        if (mHorizontal) {
            String wt = String("column") + String("Count must be greater than or equal to the maximum of all grid indices ")
                + String("(and spans) defined in the LayoutParams of each child");
            mHost->HandleInvalidParams(wt);
        } else {
            String wt = String("row") + String("Count must be greater than or equal to the maximum of all grid indices ")
                + String("(and spans) defined in the LayoutParams of each child");
            mHost->HandleInvalidParams(wt);
        }
    }
    mDefinedCount = count;
}

Boolean GridLayout::Axis::IsOrderPreserved()
{
    return mOrderPreserved;
}

void GridLayout::Axis::SetOrderPreserved(
    /* [in] */ Boolean orderPreserved)
{
    mOrderPreserved = orderPreserved;
    InvalidateStructure();
}

AutoPtr<GridLayout::PackedMap<AutoPtr<IGridLayoutSpec>, AutoPtr<IBounds> > > GridLayout::Axis::GetGroupBounds()
{
    if (!mGroupBounds) {
        mGroupBounds = CreateGroupBounds();
    }
    if (!mGroupBoundsValid) {
        ComputeGroupBounds();
        mGroupBoundsValid = true;
    }
    return mGroupBounds;
}

AutoPtr< ArrayOf<GridLayout::Arc*> > GridLayout::Axis::GetArcs()
{
    if (!mArcs) {
        mArcs = CreateArcs();
    }
    if (!mArcsValid) {
        ComputeArcs();
        mArcsValid = TRUE;
    }
    return mArcs;
}

AutoPtr< ArrayOf<Int32> > GridLayout::Axis::GetLeadingMargins()
{
    if (!mLeadingMargins) {
        mLeadingMargins = ArrayOf<Int32>::Alloc(GetCount() + 1);
    }
    if (!mLeadingMarginsValid) {
        ComputeMargins(TRUE);
        mLeadingMarginsValid = TRUE;
    }
    return mLeadingMargins;
}

AutoPtr< ArrayOf<Int32> > GridLayout::Axis::GetTrailingMargins()
{
    if (!mTrailingMargins) {
        mTrailingMargins = ArrayOf<Int32>::Alloc(GetCount() + 1);
    }
    if (!mTrailingMarginsValid) {
        ComputeMargins(FALSE);
        mTrailingMarginsValid = TRUE;
    }
    return mTrailingMargins;
}

AutoPtr< ArrayOf<Int32> > GridLayout::Axis::GetLocations()
{
    if (!mLocations) {
        Int32 N = GetCount() + 1;
        mLocations = ArrayOf<Int32>::Alloc(N);
    }
    if (!mLocationsValid) {
        ComputeLocations(mLocations);
        mLocationsValid = TRUE;
    }
    return mLocations;
}

Int32 GridLayout::Axis::GetMeasure(
    /* [in] */ Int32 measureSpec)
{
    Int32 mode = MeasureSpec::GetMode(measureSpec);
    Int32 size = MeasureSpec::GetSize(measureSpec);
    switch (mode) {
        case MeasureSpec::UNSPECIFIED: {
            return GetMeasure(0, MAX_SIZE);
        }
        case MeasureSpec::EXACTLY: {
            return GetMeasure(size, size);
        }
        case MeasureSpec::AT_MOST: {
            return GetMeasure(0, size);
        }
        default: {
            //assert false;
            return 0;
        }
    }
}

void GridLayout::Axis::Layout(
    /* [in] */ Int32 size)
{
    SetParentConstraints(size, size);
    GetLocations();
}

void GridLayout::Axis::InvalidateStructure()
{
    mMaxIndex = UNDEFINED;
    mGroupBounds = NULL;
    mForwardLinks = NULL;
    mBackwardLinks = NULL;
    mLeadingMargins = NULL;
    mTrailingMargins = NULL;
    mArcs = NULL;
    mLocations = NULL;
    mOriginalMeasurements = NULL;
    mDeltas = NULL;
    mHasWeightsValid = FALSE;
    InvalidateValues();
}

void GridLayout::Axis::InvalidateValues()
{
    mGroupBoundsValid = FALSE;
    mForwardLinksValid = FALSE;
    mBackwardLinksValid = FALSE;
    mLeadingMarginsValid = FALSE;
    mTrailingMarginsValid = FALSE;
    mArcsValid = FALSE;
    mLocationsValid = FALSE;
}

AutoPtr<ArrayOf<Int32> > GridLayout::Axis::GetDeltas()
{
    if (mDeltas == NULL) {
        Int32 childCount;
        mHost->GetChildCount(&childCount);
        mDeltas = ArrayOf<Int32>::Alloc(childCount);
    }
    return mDeltas;
}

AutoPtr<ArrayOf<Int32> > GridLayout::Axis::GetOriginalMeasurements()
{
    if (mOriginalMeasurements == NULL) {
        Int32 childCount;
        mHost->GetChildCount(&childCount);
        mOriginalMeasurements = ArrayOf<Int32>::Alloc(childCount);
    }
    return mOriginalMeasurements;
}

//==================================================================
//                      GridLayout::Arc
//==================================================================
GridLayout::Arc::Arc(
    /* [in] */ Interval* span,
    /* [in] */ MutableInt* value)
    : mValid(TRUE)
{
    mSpan = span;
    mValue = value;
}

String GridLayout::Arc::ToString()
{
    String str;
    mSpan->ToString(&str);
    str += String(" ");
    String mut = mValue->ToString();
    if (!mValid) {
        str += String("+>");
    } else {
        str += String("->");
    }
    str += String(" ");
    str += mut;
    return str;
}

//==================================================================
//                      GridLayout::MutableInt
//==================================================================
GridLayout::MutableInt::MutableInt()
{
    Reset();
}

GridLayout::MutableInt::MutableInt(
    /* [in] */ Int32 value)
{
    mValue = value;
}

void GridLayout::MutableInt::Reset()
{
    mValue = Elastos::Core::Math::INT32_MIN_VALUE;
}

String GridLayout::MutableInt::ToString()
{
    return StringUtils::ToString(mValue);
}

//==================================================================
//                      GridLayout::Bounds
//==================================================================
CAR_INTERFACE_IMPL(GridLayout::Bounds, Object, IBounds)

GridLayout::Bounds::Bounds()
{
    Reset();
}

ECode GridLayout::Bounds::GetBefore(
    /* [out] */ Int32* before)
{
    VALIDATE_NOT_NULL(before)
    *before = mBefore;
    return NOERROR;
}

ECode GridLayout::Bounds::GetAfter(
    /* [out] */ Int32* after)
{
    VALIDATE_NOT_NULL(after)
    *after = mAfter;
    return NOERROR;
}

ECode GridLayout::Bounds::GetFlexibility(
    /* [out] */ Int32* flexibility)
{
    VALIDATE_NOT_NULL(flexibility)
    *flexibility = mFlexibility;
    return NOERROR;
}

ECode GridLayout::Bounds::SetBefore(
    /* [in] */ Int32 before)
{
    mBefore = before;
    return NOERROR;
}

ECode GridLayout::Bounds::SetAfter(
    /* [in] */ Int32 after)
{
    mAfter = after;
    return NOERROR;
}

ECode GridLayout::Bounds::SetFlexibility(
    /* [in] */ Int32 flexibility)
{
    mFlexibility = flexibility;
    return NOERROR;
}

void GridLayout::Bounds::Reset()
{
    mBefore = Elastos::Core::Math::INT32_MIN_VALUE;
    mAfter = Elastos::Core::Math::INT32_MIN_VALUE;
    mFlexibility = GridLayout::CAN_STRETCH;
}

void GridLayout::Bounds::Include(
    /* [in] */ Int32 before,
    /* [in] */ Int32 after)
{
    mBefore = Elastos::Core::Math::Max(mBefore, before);
    mAfter = Elastos::Core::Math::Max(mAfter, after);
}

Int32 GridLayout::Bounds::Size(
    /* [in] */ Boolean min)
{
    if (!min) {
        if (CanStretch(mFlexibility)) {
            return GridLayout::MAX_SIZE;
        }
    }
    return mBefore + mAfter;
}

Int32 GridLayout::Bounds::GetOffset(
    /* [in] */ IGridLayout* gl,
    /* [in] */ IView* c,
    /* [in] */ Alignment* a,
    /* [in] */ Int32 size,
    /* [in] */ Boolean horizontal)
{
    Int32 mode = 0;
    IViewGroup::Probe(gl)->GetLayoutMode(&mode);
    Int32 value = 0;
    a->GetAlignmentValue(c, size, mode, &value);
    return mBefore -value;
}

void GridLayout::Bounds::Include(
    /* [in] */ IGridLayout* gl,
    /* [in] */ IView* c,
    /* [in] */ Spec* spec,
    /* [in] */ Axis* axis,
    /* [in] */ Int32 size)
{
    mFlexibility &= spec->GetFlexibility();
    Boolean horizontal = axis->mHorizontal;
    GridLayout* layout = (GridLayout*)(gl);
    AutoPtr<Alignment> alignment = layout->GetAlignment(spec->mAlignment, horizontal);
    Int32 mode = 0;
    IViewGroup::Probe(gl)->GetLayoutMode(&mode);
    Int32 before = 0;
    alignment->GetAlignmentValue(c, size, mode, &before);
    Include(before, size - before);
}

String GridLayout::Bounds::ToString()
{
    String before = StringUtils::ToString(mBefore);
    String after = StringUtils::ToString(mAfter);
    return String("Bounds{") + String("before=") + before + String(", after=") + after + String("}");
}

//==================================================================
//                      GridLayout::Interval
//==================================================================
CAR_INTERFACE_IMPL(GridLayout::Interval, Object, IInterval)

GridLayout::Interval::Interval(
    /* [in] */ Int32 min,
    /* [in] */ Int32 max)
    : mMin(min)
    , mMax(max)
{}

Int32 GridLayout::Interval::Size()
{
    return mMax - mMin;
}

ECode GridLayout::Interval::Size(
    /* [out] */ Int32* size)
{
    *size = mMax - mMin;
    return NOERROR;
}

AutoPtr<GridLayout::Interval> GridLayout::Interval::Inverse()
{
    AutoPtr<Interval> val = new Interval(mMax, mMin);
    return val;
}

ECode GridLayout::Interval::Inverse(
    /* [out] */ IInterval** interval)
{
    AutoPtr<Interval> tmp = Inverse();
    *interval = tmp.Get();
    REFCOUNT_ADD(*interval)

    return NOERROR;
}

ECode GridLayout::Interval::GetMin(
    /* [out] */ Int32* min)
{
    *min = mMin;
    return NOERROR;
}

/**
 * The maximum value.
 */
ECode GridLayout::Interval::GetMax(
    /* [out] */ Int32* max)
{
    *max = mMax;
    return NOERROR;
}

ECode GridLayout::Interval::Equals(
    /* [in] */ IInterface* that,
    /* [out] */ Boolean* result)
{
    IObject* obj = IObject::Probe(that);
    Interval* other = (Interval*)obj;
    if (this == other) {
        *result = TRUE;
        return NOERROR;
    }
    if (other == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    if (mMax != other->mMax) {
        *result = FALSE;
        return NOERROR;
    }
    if (mMin != other->mMin) {
        *result = FALSE;
        return NOERROR;
    }

    *result = TRUE;
    return NOERROR;
}

ECode GridLayout::Interval::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    Int32 result = mMin;
    result = 31 * result + mMax;
    *hashCode = result;
    return NOERROR;
}

ECode GridLayout::Interval::ToString(
    /* [out] */ String* str)
{
    String min = StringUtils::ToString(mMin);
    String max = StringUtils::ToString(mMax);
    *str = String("[") + min + String(", ") + max + String("]");
    return NOERROR;
}

//==================================================================
//                      GridLayout::Spec
//==================================================================
CAR_INTERFACE_IMPL(GridLayout::Spec, Object, IGridLayoutSpec)

static AutoPtr<IGridLayoutSpec> InitStatic() {
    AutoPtr<IGridLayoutSpec> sp = GridLayout::GetSpec(GridLayout::UNDEFINED);
    return sp;
}

const AutoPtr<IGridLayoutSpec> GridLayout::Spec::UNDEFINED = InitStatic();

GridLayout::Spec::Spec(
    /* [in] */ Boolean startDefined,
    /* [in] */ Interval* span,
    /* [in] */ Alignment* alignment,
    /* [in] */ Float weight)
    : mStartDefined(startDefined)
    , mSpan(span)
    , mAlignment(alignment)
    , mWeight(weight)
{}

GridLayout::Spec::Spec(
    /* [in] */ Boolean startDefined,
    /* [in] */ Int32 start,
    /* [in] */ Int32 size,
    /* [in] */ Alignment* alignment,
    /* [in] */ Float weight)
    : mStartDefined(startDefined)
    , mAlignment(alignment)
    , mWeight(weight)
{
    AutoPtr<Interval> val = new Interval(start, start + size);
    mSpan = (Interval*)val;
}


AutoPtr<GridLayout::Spec> GridLayout::Spec::CopyWriteSpan(
    /* [in] */ Interval* span)
{
    AutoPtr<Spec> sp = new Spec(mStartDefined, span, mAlignment, mWeight);
    return sp;
}

ECode GridLayout::Spec::CopyWriteSpan(
    /* [in] */ IInterval* span,
    /* [out] */ IGridLayoutSpec** spec)
{
    VALIDATE_NOT_NULL(spec)

    Interval* impl = (Interval*)span;
    AutoPtr<Spec> specTmp = CopyWriteSpan(impl);
    *spec = specTmp.Get();
    REFCOUNT_ADD(*spec)
    return NOERROR;
}


ECode GridLayout::Spec::CopyWriteAlignment(
    /* [in] */ IGridLayoutAlignment* alignment,
    /* [out] */ IGridLayoutSpec** spec)
{
    VALIDATE_NOT_NULL(spec)

    Alignment* impl = (Alignment*)alignment;
    AutoPtr<Spec> specTmp = CopyWriteAlignment(impl);
    *spec = specTmp.Get();
    REFCOUNT_ADD(*spec)
    return NOERROR;
}

ECode GridLayout::Spec::GetFlexibility(
    /* [out] */ Int32* rst)
{
    VALIDATE_NOT_NULL(rst)

    *rst = GetFlexibility();
    return NOERROR;
}

AutoPtr<GridLayout::Spec> GridLayout::Spec::CopyWriteAlignment(
    /* [in] */ Alignment* alignment)
{
    AutoPtr<Spec> sp = new Spec(mStartDefined, mSpan, alignment, mWeight);
    return sp;
}

Int32 GridLayout::Spec::GetFlexibility()
{
    return (mAlignment == GridLayout::UNDEFINED_ALIGNMENT && mWeight == 0) ? GridLayout::INFLEXIBLE : GridLayout::CAN_STRETCH;
}

ECode GridLayout::Spec::Equals(
    /* [in] */ IInterface* that,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;

    AutoPtr<IGridLayoutSpec> sp = IGridLayoutSpec::Probe(that);
    if (sp == NULL) {
        return NOERROR;
    }
    Spec* spec = (Spec*)sp.Get();
    if (mAlignment != spec->mAlignment) {
        return NOERROR;
    }
    Boolean equal;
    AutoPtr<IObject> obj = spec->mSpan;
    if (mSpan->Equals(obj, &equal), equal) {
        return NOERROR;
    }
    *res = TRUE;
    return NOERROR;
}

ECode GridLayout::Spec::GetHashCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code);
    Int32 result;
    mSpan->GetHashCode(&result);
    result = 31 * result /*+ mAlignment->GetHashCode()*/;
    *code = result;
    return NOERROR;
}

//==================================================================
//                      GridLayout::Alignment
//==================================================================
CAR_INTERFACE_IMPL(GridLayout::Alignment, Object, IGridLayoutAlignment)

GridLayout::Alignment::Alignment()
{}

AutoPtr<GridLayout::Bounds> GridLayout::Alignment::GetBounds()
{
    AutoPtr<Bounds> b = new Bounds();
    return b;
}

ECode GridLayout::Alignment::GetBounds(
    /* [out] */ IBounds** bounds)
{
    VALIDATE_NOT_NULL(bounds)

    AutoPtr<IBounds> tmp = GetBounds();
    *bounds = tmp;
    REFCOUNT_ADD(*bounds)

    return NOERROR;
}

ECode GridLayout::Alignment::GetSizeInCell(
    /* [in] */ IView* view,
    /* [in] */ Int32 viewSize,
    /* [in] */ Int32 cellSize,
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    *size = viewSize;
    return NOERROR;
}

//==================================================================
//                      GridLayout::UndefinedAlignment
//==================================================================
ECode GridLayout::UndefinedAlignment::GetGravityOffset(
    /* [in] */ IView* view,
    /* [in] */ Int32 cellDelta,
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset);
    *offset = UNDEFINED;
    return NOERROR;
}

ECode GridLayout::UndefinedAlignment::GetAlignmentValue(
    /* [in] */ IView* view,
    /* [in] */ Int32 viewSize,
    /* [in] */ Int32 mode,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = UNDEFINED;
    return NOERROR;
}

//==================================================================
//                      GridLayout::LeadingAlignment
//==================================================================
ECode GridLayout::LeadingAlignment::GetGravityOffset(
    /* [in] */ IView* view,
    /* [in] */ Int32 cellDelta,
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset);
    *offset = 0;
    return NOERROR;
}

ECode GridLayout::LeadingAlignment::GetAlignmentValue(
    /* [in] */ IView* view,
    /* [in] */ Int32 viewSize,
    /* [in] */ Int32 mode,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;
    return NOERROR;
}

//==================================================================
//                      GridLayout::TrailingAlignment
//==================================================================
ECode GridLayout::TrailingAlignment::GetGravityOffset(
    /* [in] */ IView* view,
    /* [in] */ Int32 cellDelta,
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset);
    *offset = cellDelta;
    return NOERROR;
}

ECode GridLayout::TrailingAlignment::GetAlignmentValue(
    /* [in] */ IView* view,
    /* [in] */ Int32 viewSize,
    /* [in] */ Int32 mode,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = viewSize;
    return NOERROR;
}

//==================================================================
//                      GridLayout::SwitchingAlignment
//==================================================================
GridLayout::SwitchingAlignment::SwitchingAlignment(
    /* [in] */ Alignment* ltr,
    /* [in] */ Alignment* rtl)
    : mLtr(ltr)
    , mRtl(rtl)
{}

ECode GridLayout::SwitchingAlignment::GetGravityOffset(
    /* [in] */ IView* view,
    /* [in] */ Int32 cellDelta,
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset);
    Boolean res = FALSE;
    view->IsLayoutRtl(&res);
    (!res ? mLtr : mRtl)->GetGravityOffset(view, cellDelta, offset);
    return NOERROR;
}

ECode GridLayout::SwitchingAlignment::GetAlignmentValue(
    /* [in] */ IView* view,
    /* [in] */ Int32 viewSize,
    /* [in] */ Int32 mode,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    Boolean res = FALSE;
    view->IsLayoutRtl(&res);
    (!res ? mLtr : mRtl)->GetAlignmentValue(view, viewSize, mode, value);
    return NOERROR;
}

//==================================================================
//                      GridLayout::CenterAlignment
//==================================================================
ECode GridLayout::CenterAlignment::GetGravityOffset(
    /* [in] */ IView* view,
    /* [in] */ Int32 cellDelta,
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset);
    *offset = cellDelta >> 1;
    return NOERROR;
}

ECode GridLayout::CenterAlignment::GetAlignmentValue(
    /* [in] */ IView* view,
    /* [in] */ Int32 viewSize,
    /* [in] */ Int32 mode,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = viewSize >> 1;
    return NOERROR;
}

//==================================================================
//                      GridLayout::BaseLineAlignment
//==================================================================
ECode GridLayout::BaseLineAlignment::GetGravityOffset(
    /* [in] */ IView* view,
    /* [in] */ Int32 cellDelta,
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset);
    *offset = 0;
    return NOERROR;
}

ECode GridLayout::BaseLineAlignment::GetAlignmentValue(
    /* [in] */ IView* view,
    /* [in] */ Int32 viewSize,
    /* [in] */ Int32 mode,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    Int32 visibility;
    view->GetVisibility(&visibility);
    if (visibility == GONE) {
        *value = 0;
        return NOERROR;
    }
    Int32 baseline;
    view->GetBaseline(&baseline);
    *value = baseline == -1 ? UNDEFINED : baseline;
    return NOERROR;
}

AutoPtr<GridLayout::Bounds> GridLayout::BaseLineAlignment::GetBounds()
{
    AutoPtr<AlignmentBounds> b = new AlignmentBounds();
    return b;
}

//==================================================================
//                      GridLayout::AlignmentBounds
//==================================================================

void GridLayout::AlignmentBounds::Reset()
{
    Bounds::Reset();
    mSize = Elastos::Core::Math::INT32_MIN_VALUE;
}

void GridLayout::AlignmentBounds::Include(
    /* [in] */ Int32 before,
    /* [in] */ Int32 after)
{
    Bounds::Include(before, after);
    mSize = Elastos::Core::Math::Max(mSize, before + after);
}

Int32 GridLayout::AlignmentBounds::Size(
    /* [in] */ Boolean min)
{
    return Elastos::Core::Math::Max(Bounds::Size(min), mSize);
}

Int32 GridLayout::AlignmentBounds::GetOffset(
    /* [in] */ IGridLayout* gl,
    /* [in] */ IView* c,
    /* [in] */ Alignment* a,
    /* [in] */ Int32 size,
    /* [in] */ Boolean horizontal)
{
    return Elastos::Core::Math::Max(0, Bounds::GetOffset(gl, c, a, size, horizontal));
}

//==================================================================
//                      GridLayout::FillAlignment
//==================================================================
ECode GridLayout::FillAlignment::GetGravityOffset(
    /* [in] */ IView* view,
    /* [in] */ Int32 cellDelta,
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset);
    *offset = 0;
    return NOERROR;
}

ECode GridLayout::FillAlignment::GetAlignmentValue(
    /* [in] */ IView* view,
    /* [in] */ Int32 viewSize,
    /* [in] */ Int32 mode,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = UNDEFINED;
    return NOERROR;
}

ECode GridLayout::FillAlignment::GetSizeInCell(
    /* [in] */ IView* view,
    /* [in] */ Int32 viewSize,
    /* [in] */ Int32 cellSize,
    /* [in] */ Int32* cell)
{
    VALIDATE_NOT_NULL(cell);
    *cell = cellSize;
    return NOERROR;
}

//==================================================================
//                      GridLayout::TopoSort
//==================================================================
GridLayout::TopoSort::TopoSort(
    /* [in] */ ArrayOf<Arc*>* result,
    /* [in] */ Int32 cursor,
    /* [in] */ ArrayOf< AutoPtr< ArrayOf< Arc*> > >* arcsByVertex,
    /* [in] */ ArrayOf<Int32>* visited)
    : mResult(result)
    , mCursor(cursor)
    , mArcsByVertex(arcsByVertex)
    , mvisited(visited)
{}

void GridLayout::TopoSort::Walk(
    /* [in] */ Int32 loc)
{
    switch((*mvisited)[loc]) {
        case Axis::NEW:
        {
            (*mvisited)[loc] = Axis::PENDING;
            AutoPtr<ArrayOf<GridLayout::Arc*> > a = (*mArcsByVertex)[loc];
            for (Int32 i = 0; i < a->GetLength(); i++) {
                AutoPtr<Arc> arc = (*a)[i];
                Walk(arc->mSpan->mMax);
                mResult->Set(mCursor--, arc);
            }
            (*mvisited)[loc] = Axis::COMPLETE;
            break;
        }
        case Axis::PENDING:
        //assert false;
            break;

        case Axis::COMPLETE:
            break;
    }
}

AutoPtr< ArrayOf<GridLayout::Arc*> > GridLayout::TopoSort::Sort()
{
    for(Int32 loc = 0, N = mArcsByVertex->GetLength(); loc < N; loc++) {
        Walk(loc);
    }
    assert(mCursor == -1);
    return mResult;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
