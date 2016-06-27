
#include "elastos/droid/systemui/qs/CDataUsageGraph.h"
#include "R.h"
#include "Elastos.Droid.Content.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CRectF;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {

CAR_OBJECT_IMPL(CDataUsageGraph)
CAR_INTERFACE_IMPL(CDataUsageGraph, View, IDataUsageGraph)
CDataUsageGraph::CDataUsageGraph()
    : mTrackColor(0)
    , mUsageColor(0)
    , mOverlimitColor(0)
    , mWarningColor(0)
    , mMarkerWidth(0)
    , mLimitLevel(0)
    , mWarningLevel(0)
    , mUsageLevel(0)
    , mMaxLevel(0)
{
    CRectF::New((IRectF**)&mTmpRect);
    CPaint::New((IPaint**)&mTmpPaint);
}

ECode CDataUsageGraph::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    View::constructor(context, attrs);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetColor(R::color::data_usage_graph_track, &mTrackColor);
    res->GetColor(R::color::system_accent_color, &mUsageColor);
    res->GetColor(R::color::system_warning_color, &mOverlimitColor);
    res->GetColor(R::color::data_usage_graph_warning, &mWarningColor);
    res->GetDimensionPixelSize(R::dimen::data_usage_graph_marker_width, &mMarkerWidth);
    return NOERROR;
}

ECode CDataUsageGraph::SetLevels(
    /* [in] */ Int64 limitLevel,
    /* [in] */ Int64 warningLevel,
    /* [in] */ Int64 usageLevel)
{
    mLimitLevel = Elastos::Core::Math::Max((Int64)0, limitLevel);
    mWarningLevel = Elastos::Core::Math::Max((Int64)0, warningLevel);
    mUsageLevel = Elastos::Core::Math::Max((Int64)0, usageLevel);
    mMaxLevel = Elastos::Core::Math::Max(Elastos::Core::Math::Max(
            Elastos::Core::Math::Max(mLimitLevel, mWarningLevel), mUsageLevel), (Int64)1);
    PostInvalidate();
    return NOERROR;
}

void CDataUsageGraph::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    View::OnDraw(canvas);

    AutoPtr<IRectF> r = mTmpRect;
    AutoPtr<IPaint> p = mTmpPaint;
    Int32 w = 0;
    GetWidth(&w);
    Int32 h = 0;
    GetHeight(&h);

    const Boolean overLimit = mLimitLevel > 0 && mUsageLevel > mLimitLevel;
    Float usageRight = w * (mUsageLevel / (Float) mMaxLevel);
    if (overLimit) {
        // compute the gap
        usageRight = w * (mLimitLevel / (Float) mMaxLevel) - (mMarkerWidth / 2);
        usageRight = Elastos::Core::Math::Min(
                Elastos::Core::Math::Max(usageRight, (Float)mMarkerWidth), (Float)w - mMarkerWidth * 2);

        // draw overlimit
        r->Set(usageRight + mMarkerWidth, 0, w, h);
        p->SetColor(mOverlimitColor);
        canvas->DrawRect(r, p);
    }
    else {
        // draw track
        r->Set(0, 0, w, h);
        p->SetColor(mTrackColor);
        canvas->DrawRect(r, p);
    }

    // draw usage
    r->Set(0, 0, usageRight, h);
    p->SetColor(mUsageColor);
    canvas->DrawRect(r, p);

    // draw warning marker
    Float warningLeft = w * (mWarningLevel / (Float) mMaxLevel) - mMarkerWidth / 2;
    warningLeft = Elastos::Core::Math::Min(
            Elastos::Core::Math::Max(warningLeft, (Float)0), (Float)w - mMarkerWidth);
    r->Set(warningLeft, 0, warningLeft + mMarkerWidth, h);
    p->SetColor(mWarningColor);
    canvas->DrawRect(r, p);
}

} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
