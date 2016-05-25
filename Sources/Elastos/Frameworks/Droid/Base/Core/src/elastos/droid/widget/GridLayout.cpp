
#include "Elastos.Droid.Content.h"
#include "elastos/droid/widget/GridLayout.h"
#include "elastos/droid/graphics/Insets.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/Color.h"
#include "elastos/droid/widget/CGridLayoutLayoutParams.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

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
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Widget {

//==================================================================
//                      GridLayout::Axis
//==================================================================
const Int32 GridLayout::Axis::NEW = 0;
const Int32 GridLayout::Axis::PENDING = 1;
const Int32 GridLayout::Axis::COMPLETE = 2;

GridLayout::Axis::Axis(
    /* [in] */ Boolean horizontal,
    /* [in] */ GridLayout* host)
    : mHorizontal(horizontal)
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
    , mMaxIndex(GridLayout::UNDEFINED)
    , mHost(host)
{
    mParentMin = new MutableInt(0);
    mParentMax = new MutableInt(-MAX_SIZE);
}

Int32 GridLayout::Axis::CalculateMaxIndex()
{
    Int32 result = -1;
    Int32 N;
    mHost->GetChildCount(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IView> c;
        mHost->GetChildAt(i, (IView**)&c);
        AutoPtr<IGridLayoutLayoutParams> params = mHost->GetLayoutParams(c);
        AutoPtr<Spec> spec;
        if (mHorizontal) {
            AutoPtr<IGridLayoutSpec> columnSpec;
            params->GetColumnSpec((IGridLayoutSpec**)&columnSpec);
            spec = (Spec*)columnSpec.Get();
        }
        else {
            AutoPtr<IGridLayoutSpec> rowSpec;
            params->GetRowSpec((IGridLayoutSpec**)&rowSpec);
            spec = (Spec*)rowSpec.Get();
        }
        AutoPtr<Interval> span = spec->mSpan;
        Int32 size;
        span->Size(&size);
        result = Elastos::Core::Math::Max(result, span->mMin);
        result = Elastos::Core::Math::Max(result, span->mMax);
        result = Elastos::Core::Math::Max(result, size);
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

Int32 GridLayout::Axis::GetCount()
{
    return Elastos::Core::Math::Max(mDefinedCount, GetMaxIndex());
}

void GridLayout::Axis::SetCount(
    /* [in] */ Int32 count)
{
    if (count != GridLayout::UNDEFINED && count < GetMaxIndex()) {
        if (mHorizontal) {
            String wt = String("column Count must be greater than or equal to the"
                    "maximum of all grid indices (and spans) defined in the LayoutParams of each child");
            mHost->HandleInvalidParams(wt);
        }
        else {
            String wt = String("row Count must be greater than or equal to the"
                    "maximum of all grid indices (and spans) defined in the LayoutParams of each child");
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

AutoPtr<GridLayout::PackedMap<AutoPtr<IGridLayoutSpec>, AutoPtr<IBounds> > > GridLayout::Axis::CreateGroupBounds()
{
    AutoPtr< Assoc<AutoPtr<IGridLayoutSpec>, AutoPtr<IBounds> > > assoc =
            Assoc<AutoPtr<IGridLayoutSpec>, AutoPtr<IBounds> >::Of(NULL, NULL);
    Int32 N;
    mHost->GetChildCount(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IView> c;
        mHost->GetChildAt(i, (IView**)&c);
        // we must include views that are GONE here, see introductory javadoc
        AutoPtr<IGridLayoutLayoutParams> params = mHost->GetLayoutParams(c);
        AutoPtr<Spec> spec;
        if (mHorizontal) {
            AutoPtr<IGridLayoutSpec> columnSpec;
            params->GetColumnSpec((IGridLayoutSpec**)&columnSpec);
            spec = (Spec*)columnSpec.Get();
        }
        else {
            AutoPtr<IGridLayoutSpec> rowSpec;
            params->GetRowSpec((IGridLayoutSpec**)&rowSpec);
            spec = (Spec*)rowSpec.Get();
        }
        AutoPtr<IBounds> bounds;
        mHost->GetAlignment(spec->mAlignment, mHorizontal)->GetBounds((IBounds**)&bounds);
        assoc->Put((IGridLayoutSpec*)spec, bounds);
    }
    return assoc->Pack();
}

void GridLayout::Axis::ComputeGroupBounds()
{
    AutoPtr< ArrayOf<IBounds*> > values = mGroupBounds->mValues;
    for (Int32 i = 0; i < values->GetLength(); i++) {
        Bounds* bounds = (Bounds*)(*values)[i];
        bounds->Reset();
    }
    Int32 N;
    mHost->GetChildCount(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IView> c;
        mHost->GetChildAt(i, (IView**)&c);
        // we must include views that are GONE here, see introductory javadoc
        AutoPtr<IGridLayoutLayoutParams> params = mHost->GetLayoutParams(c);
        AutoPtr<Spec> spec;
        if (mHorizontal) {
            AutoPtr<IGridLayoutSpec> columnSpec;
            params->GetColumnSpec((IGridLayoutSpec**)&columnSpec);
            spec = (Spec*)columnSpec.Get();
        }
        else {
            AutoPtr<IGridLayoutSpec> rowSpec;
            params->GetRowSpec((IGridLayoutSpec**)&rowSpec);
            spec = (Spec*)rowSpec.Get();
        }
        Int32 size;
        if (spec->mWeight == 0) {
            mHost->GetMeasurementIncludingMargin(c, mHorizontal, &size);
        }
        else {
            size = (*GetOriginalMeasurements())[i] + (*GetDeltas())[i];
        }
        AutoPtr<IBounds> bounds = mGroupBounds->GetValue(i);
        Bounds* boundsObj = (Bounds*)bounds.Get();
        boundsObj->Include(mHost, c, spec, this, size);
    }
}

AutoPtr<GridLayout::PackedMap<AutoPtr<IGridLayoutSpec>, AutoPtr<IBounds> > >
GridLayout::Axis::GetGroupBounds()
{
    if (mGroupBounds == NULL) {
        mGroupBounds = CreateGroupBounds();
    }
    if (!mGroupBoundsValid) {
        ComputeGroupBounds();
        mGroupBoundsValid = TRUE;
    }
    return mGroupBounds;
}

AutoPtr< GridLayout::PackedMap<AutoPtr<IInterval>, AutoPtr<GridLayout::MutableInt> > >
GridLayout::Axis::CreateLinks(
    /* [in] */ Boolean min)
{
    AutoPtr< Assoc< AutoPtr<IInterval>, AutoPtr<MutableInt> > > result =
            Assoc< AutoPtr<IInterval>, AutoPtr<MutableInt> >::Of(NULL, NULL);
    AutoPtr< ArrayOf<AutoPtr<IGridLayoutSpec> > > keys = GetGroupBounds()->mKeys;
    for (Int32 i = 0, N = keys->GetLength(); i < N; i++) {
        Spec* sp = (Spec*)(*keys)[i].Get();
        AutoPtr<Interval> span;
        if (min) {
            span = sp->mSpan;
        }
        else {
            AutoPtr<IInterval> tmp;
            sp->mSpan->Inverse((IInterval**)&tmp);
            span = (Interval*)tmp.Get();
        }
        AutoPtr<MutableInt> mut = new MutableInt();
        result->Put((IInterval*)span, mut);
    }
    return result->Pack();
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
        Bounds* boundsImpl = (Bounds*)(*bounds)[i].Get();
        Int32 size = boundsImpl->Size(min);
        AutoPtr<MutableInt> valueHolder = links->GetValue(i);
        valueHolder->mValue = Elastos::Core::Math::Max(valueHolder->mValue, min ? size : -size);
    }
}

AutoPtr< GridLayout::PackedMap<AutoPtr<IInterval>, AutoPtr<GridLayout::MutableInt> > >
GridLayout::Axis::GetForwardLinks()
{
    if (mForwardLinks == NULL) {
        mForwardLinks = CreateLinks(TRUE);
    }
    if (!mForwardLinksValid) {
        ComputeLinks(mForwardLinks, TRUE);
        mForwardLinksValid = TRUE;
    }
    return mForwardLinks;
}

AutoPtr< GridLayout::PackedMap<AutoPtr<IInterval>, AutoPtr<GridLayout::MutableInt> > >
GridLayout::Axis::GetBackwardLinks()
{
    if (mBackwardLinks = NULL) {
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
    /*
    Remove self referential links.
    These appear:
        . as parental constraints when GridLayout has no children
        . when components have been marked as GONE
    */
    Int32 count;
    if (key->Size(&count), count == 0) {
        return;
    }
    // this bit below should really be computed outside here -
            // its just to stop default (row/col > 0) constraints obliterating valid entries
    if (ignoreIfAlreadyPresent) {
        List< AutoPtr<Arc> >::Iterator it;
        for (it = arcs.Begin(); it != arcs.End(); ++it) {
            AutoPtr<Interval> span = (*it)->mSpan;
            Boolean equal;
            if (span->Equals((IInterval*)key, &equal), equal) {
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

AutoPtr< ArrayOf<AutoPtr<ArrayOf<GridLayout::Arc*> > > >
GridLayout::Axis::GroupArcsByFirstVertex(
    /* [in] */ AutoPtr< ArrayOf<Arc*> > arcs)
{
    Int32 N = GetCount() + 1; // the number of vertices
    AutoPtr< ArrayOf<AutoPtr<ArrayOf<GridLayout::Arc*> > > > result =
            ArrayOf<AutoPtr<ArrayOf<GridLayout::Arc*> > >::Alloc(N);
    AutoPtr< ArrayOf<Int32> > sizes = ArrayOf<Int32>::Alloc(N);
    for (Int32 i = 0; i < arcs->GetLength(); i++) {
        AutoPtr<Arc> arc = (*arcs)[i];
        (*sizes)[arc->mSpan->mMin]++;
    }
    for (Int32 i = 0; i < sizes->GetLength(); i++) {
        AutoPtr<ArrayOf<Arc*> > arcArray = ArrayOf<Arc*>::Alloc((*sizes)[i]);
        result->Set(i, arcArray);
    }
    // reuse the sizes array to hold the current last elements as we insert each arc
    Arrays::Fill(sizes, 0);
    for (Int32 i = 0; i < arcs->GetLength(); i++) {
        AutoPtr<Arc> arc = (*arcs)[i];
        Int32 index = arc->mSpan->mMin;
        (*result)[index]->Set((*sizes)[index]++, arc);
    }
    return result;
}

AutoPtr< ArrayOf<GridLayout::Arc*> > GridLayout::Axis::TopologicalSort(
    /* [in] */ ArrayOf<Arc*>* arcs)
{
    AutoPtr<TopoSort> sort = new TopoSort(arcs, this);
    return sort->Sort();
}

AutoPtr< ArrayOf<GridLayout::Arc*> > GridLayout::Axis::TopologicalSort(
    /* [in] */ List< AutoPtr<Arc> >& arcs)
{
    AutoPtr< ArrayOf<Arc*> > result = ArrayOf<Arc*>::Alloc(arcs.GetSize());
    List< AutoPtr<Arc> >::Iterator it = arcs.Begin();
    for(Int32 i = 0; it != arcs.End(); ++it, ++i) {
        result->Set(i, *it);
    }
    return TopologicalSort(result);
}

void GridLayout::Axis::AddComponentSizes(
    /* [in] */ List< AutoPtr<Arc> >& result,
    /* [in] */ PackedMap<AutoPtr<IInterval>, AutoPtr<MutableInt> >* links)
{
    for (Int32 i = 0; i < links->mKeys->GetLength(); i++) {
        AutoPtr<Interval> key = (Interval*)(*links->mKeys)[i].Get();
        Include(result, key, (*links->mValues)[i], FALSE);
    }
}

AutoPtr< ArrayOf<GridLayout::Arc*> > GridLayout::Axis::CreateArcs()
{
    List< AutoPtr<Arc> > mins, maxs;

    // Add the minimum values from the components.
    AddComponentSizes(mins, GetForwardLinks());
    // Add the maximum values from the components.
    AddComponentSizes(maxs, GetBackwardLinks());

    // Add ordering constraints to prevent row/col sizes from going negative
    if (mOrderPreserved) {
        // Add a constraint for every row/col
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

    return mHost->Append(sMins.Get(), sMaxs.Get());
}

void GridLayout::Axis::ComputeArcs()
{
    GetForwardLinks();
    GetBackwardLinks();
}

AutoPtr< ArrayOf<GridLayout::Arc*> > GridLayout::Axis::GetArcs()
{
    if (mArcs = NULL) {
        mArcs = CreateArcs();
    }
    if (!mArcsValid) {
        ComputeArcs();
        mArcsValid = TRUE;
    }
    return mArcs;
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
    Arrays::Fill(locations, 0);
}

String GridLayout::Axis::ArcsToString(
    /* [in] */ List< AutoPtr<Arc> >& arcs)
{
    String var = mHorizontal ? String("x") : String("y");
    StringBuilder result;
    Boolean first = TRUE;
    List< AutoPtr<Arc> >::Iterator it;
    for (it = arcs.Begin(); it != arcs.End(); ++it) {
        if (first) {
            first = FALSE;
        }
        else {
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
        }
        else {
            result += var;
            result += src;
            result += String("-");
            result += var;
            result += dst;
            result += String("<=");
            result += -value;
        }
    }
    return result.ToString();
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
    // TODO:
    /*Log.d(TAG, axisName + " constraints: " + arcsToString(culprits) + " are inconsistent; "
            + "permanently removing: " + arcsToString(removed) + ". ");*/
}

void GridLayout::Axis::Solve(
    /* [in] */ ArrayOf<Arc*>* arcs,
    /* [in] */ ArrayOf<Int32>* locations)
{
    String axisName = mHorizontal ? String("horizontal") : String("vertical");
    Int32 N = GetCount() + 1; // The number of vertices is the number of columns/rows + 1.
    AutoPtr< ArrayOf<Boolean> > originalCulprits;

    for (Int32 p = 0; p < arcs->GetLength(); p++) {
        Init(locations);

        // We take one extra pass over traditional Bellman-Ford (and omit their final step)
        for (Int32 i = 0; i < N; i++) {
            Boolean changed = FALSE;
            for (Int32 j = 0, length = arcs->GetLength(); j < length; j++) {
                changed |= Relax(locations, (*arcs)[j]);
            }
            if (!changed) {
                if (originalCulprits != NULL) {
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
                // Only remove max values, min values alone cannot be inconsistent
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
    Int32 N;
    mHost->GetChildCount(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IView> c;
        mHost->GetChildAt(i, (IView**)&c);
        Int32 visibility = 0;
        c->GetVisibility(&visibility);
        if (visibility == IView::GONE) continue;
        AutoPtr<IGridLayoutLayoutParams> lp = mHost->GetLayoutParams(c);
        AutoPtr<Spec> spec;
        if (mHorizontal) {
            AutoPtr<IGridLayoutSpec> columnSpec;
            lp->GetColumnSpec((IGridLayoutSpec**)&columnSpec);
            spec = (Spec*)columnSpec.Get();
        }
        else {
            AutoPtr<IGridLayoutSpec> rowSpec;
            lp->GetRowSpec((IGridLayoutSpec**)&rowSpec);
            spec = (Spec*)rowSpec.Get();
        }
        AutoPtr<Interval> span = spec->mSpan;
        Int32 index = leading ? span->mMin : span->mMax;
        (*margins)[index] = Elastos::Core::Math::Max((*margins)[index], mHost->GetMargin1(c, mHorizontal, leading));
    }
}

AutoPtr< ArrayOf<Int32> > GridLayout::Axis::GetLeadingMargins()
{
    if (mLeadingMargins = NULL) {
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
    if (mTrailingMargins = NULL) {
        mTrailingMargins = ArrayOf<Int32>::Alloc(GetCount() + 1);
    }
    if (!mTrailingMarginsValid) {
        ComputeMargins(FALSE);
        mTrailingMarginsValid = TRUE;
    }
    return mTrailingMargins;
}

void GridLayout::Axis::Solve(
    /* [in] */ ArrayOf<Int32>* a)
{
    Solve(GetArcs(), a);
}

Boolean GridLayout::Axis::ComputeHasWeights()
{
    Int32 N;
    mHost->GetChildCount(&N);
    for (int i = 0; i < N; i++) {
        AutoPtr<IView> v;
        mHost->GetChildAt(i, (IView**)&v);
        AutoPtr<IGridLayoutLayoutParams> lp = mHost->GetLayoutParams(v);
        AutoPtr<Spec> spec;
        if (mHorizontal) {
            AutoPtr<IGridLayoutSpec> columnSpec;
            lp->GetColumnSpec((IGridLayoutSpec**)&columnSpec);
            spec = (Spec*)columnSpec.Get();
        }
        else {
            AutoPtr<IGridLayoutSpec> rowSpec;
            lp->GetRowSpec((IGridLayoutSpec**)&rowSpec);
            spec = (Spec*)rowSpec.Get();
        }
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

AutoPtr<ArrayOf<Int32> > GridLayout::Axis::GetOriginalMeasurements()
{
    if (mOriginalMeasurements == NULL) {
        Int32 N;
        mHost->GetChildCount(&N);
        mOriginalMeasurements = ArrayOf<Int32>::Alloc(N);
    }
    return mOriginalMeasurements;
}

void GridLayout::Axis::RecordOriginalMeasurement(
    /* [in] */ Int32 i)
{
    if (HasWeights()) {
        AutoPtr<ArrayOf<Int32> > arr = GetOriginalMeasurements();
        AutoPtr<IView> v;
        mHost->GetChildAt(i, (IView**)&v);
        Int32 result;
        mHost->GetMeasurementIncludingMargin(v, mHorizontal, &result);
        (*arr)[i] = result;
    }
}

AutoPtr<ArrayOf<Int32> > GridLayout::Axis::GetDeltas()
{
    if (mDeltas == NULL) {
        Int32 N;
        mHost->GetChildCount(&N);
        mDeltas = ArrayOf<Int32>::Alloc(N);
    }
    return mDeltas;
}

void GridLayout::Axis::ShareOutDelta()
{
    Int32 totalDelta = 0;
    Float totalWeight = 0;
    Int32 N;
    mHost->GetChildCount(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IView> c;
        mHost->GetChildAt(i, (IView**)&c);
        AutoPtr<IGridLayoutLayoutParams> lp = mHost->GetLayoutParams(c);
        AutoPtr<Spec> spec;
        if (mHorizontal) {
            AutoPtr<IGridLayoutSpec> columnSpec;
            lp->GetColumnSpec((IGridLayoutSpec**)&columnSpec);
            spec = (Spec*)columnSpec.Get();
        }
        else {
            AutoPtr<IGridLayoutSpec> rowSpec;
            lp->GetRowSpec((IGridLayoutSpec**)&rowSpec);
            spec = (Spec*)rowSpec.Get();
        }
        Float weight = spec->mWeight;
        if (weight != 0) {
            AutoPtr<ArrayOf<Int32> > measurements = GetOriginalMeasurements();
            Int32 delta = mHost->GetMeasurement(c, mHorizontal) - (*measurements)[i];
            totalDelta += delta;
            totalWeight += weight;
        }
    }
    mHost->GetChildCount(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IView> c;
        mHost->GetChildAt(i, (IView**)&c);
        AutoPtr<IGridLayoutLayoutParams> lp = mHost->GetLayoutParams(c);
        AutoPtr<Spec> spec;
        if (mHorizontal) {
            AutoPtr<IGridLayoutSpec> columnSpec;
            lp->GetColumnSpec((IGridLayoutSpec**)&columnSpec);
            spec = (Spec*)columnSpec.Get();
        }
        else {
            AutoPtr<IGridLayoutSpec> rowSpec;
            lp->GetRowSpec((IGridLayoutSpec**)&rowSpec);
            spec = (Spec*)rowSpec.Get();
        }
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
    Arrays::Fill(deltas, 0);
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
    }
    else {
        SolveAndDistributeSpace(a);
    }
    if (!mOrderPreserved) {
        // Solve returns the smallest solution to the constraint system for which all
        // values are positive. One value is therefore zero - though if the row/col
        // order is not preserved this may not be the first vertex. For consistency,
        // translate all the values so that they measure the distance from a[0]; the
        // leading edge of the parent. After this transformation some values may be
        // negative.
        Int32 a0 = (*a)[0];
        for (Int32 i = 0, N = a->GetLength(); i < N; i++) {
            (*a)[i] = (*a)[i] - a0;
        }
    }
}

AutoPtr< ArrayOf<Int32> > GridLayout::Axis::GetLocations()
{
    if (mLocations == NULL) {
        Int32 N = GetCount() + 1;
        mLocations = ArrayOf<Int32>::Alloc(N);
    }
    if (!mLocationsValid) {
        ComputeLocations(mLocations);
        mLocationsValid = TRUE;
    }
    return mLocations;
}

Int32 GridLayout::Axis::Size(
    /* [in] */ ArrayOf<Int32>* locations)
{
    // The parental edges are attached to vertices 0 and N - even when order is not
    // being preserved and other vertices fall outside this range. Measure the distance
    // between vertices 0 and N, assuming that locations[0] = 0.
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


//=================================================================
// GridLayout::GridLayoutLayoutParams
//=================================================================
Int32 GridLayout::GridLayoutLayoutParams::DEFAULT_WIDTH;
Int32 GridLayout::GridLayoutLayoutParams::DEFAULT_HEIGHT;
Int32 GridLayout::GridLayoutLayoutParams::DEFAULT_MARGIN;
Int32 GridLayout::GridLayoutLayoutParams::DEFAULT_ROW;
Int32 GridLayout::GridLayoutLayoutParams::DEFAULT_COLUMN;
AutoPtr<GridLayout::Interval> GridLayout::GridLayoutLayoutParams::DEFAULT_SPAN;
Int32 GridLayout::GridLayoutLayoutParams::DEFAULT_SPAN_SIZE;

Int32 GridLayout::GridLayoutLayoutParams::MARGIN;
Int32 GridLayout::GridLayoutLayoutParams::LEFT_MARGIN;
Int32 GridLayout::GridLayoutLayoutParams::TOP_MARGIN;
Int32 GridLayout::GridLayoutLayoutParams::RIGHT_MARGIN;
Int32 GridLayout::GridLayoutLayoutParams::BOTTOM_MARGIN;

Int32 GridLayout::GridLayoutLayoutParams::COLUMN;
Int32 GridLayout::GridLayoutLayoutParams::COLUMN_SPAN;
Int32 GridLayout::GridLayoutLayoutParams::COLUMN_WEIGHT;

Int32 GridLayout::GridLayoutLayoutParams::ROW;
Int32 GridLayout::GridLayoutLayoutParams::ROW_SPAN;
Int32 GridLayout::GridLayoutLayoutParams::ROW_WEIGHT;

Int32 GridLayout::GridLayoutLayoutParams::GRAVITY;

CAR_INTERFACE_IMPL(GridLayout::GridLayoutLayoutParams, ViewGroup::MarginLayoutParams, IGridLayoutLayoutParams)

GridLayout::GridLayoutLayoutParams::GridLayoutLayoutParams()
    : mRowSpec(Spec::UNDEFINED)
    , mColumnSpec(Spec::UNDEFINED)
{}

ECode GridLayout::GridLayoutLayoutParams::constructor(
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

ECode GridLayout::GridLayoutLayoutParams::constructor(
    /* [in] */ IGridLayoutSpec* rowSpec,
    /* [in] */ IGridLayoutSpec* columnSpec)
{
    return constructor(DEFAULT_WIDTH, DEFAULT_HEIGHT,
            DEFAULT_MARGIN, DEFAULT_MARGIN, DEFAULT_MARGIN, DEFAULT_MARGIN,
            rowSpec, columnSpec);
}

ECode GridLayout::GridLayoutLayoutParams::constructor()
{
    return constructor(Spec::UNDEFINED, Spec::UNDEFINED);
}

ECode GridLayout::GridLayoutLayoutParams::constructor(
    /* [in] */ IViewGroupLayoutParams* params)
{
    return MarginLayoutParams::constructor(params);
}

ECode GridLayout::GridLayoutLayoutParams::constructor(
    /* [in] */ IViewGroupMarginLayoutParams* params)
{
    return MarginLayoutParams::constructor(params);
}

ECode GridLayout::GridLayoutLayoutParams::constructor(
    /* [in] */ IGridLayoutLayoutParams* that)
{
    FAIL_RETURN(MarginLayoutParams::constructor(
            IViewGroupMarginLayoutParams::Probe(that)))

    AutoPtr<GridLayoutLayoutParams> cgl = (GridLayoutLayoutParams*)that;
    mRowSpec = cgl->mRowSpec;
    mColumnSpec = cgl->mColumnSpec;
    return NOERROR;
}

ECode GridLayout::GridLayoutLayoutParams::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(MarginLayoutParams::constructor(context, attrs))
    ReInitSuper(context, attrs);
    Init(context, attrs);
    return NOERROR;
}

void GridLayout::GridLayoutLayoutParams::ReInitSuper(
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

void GridLayout::GridLayoutLayoutParams::Init(
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
    mColumnSpec = GetSpec(column, colSpan, GetAlignment(gravity, TRUE), colWeight);

    Int32 row = 0;
    a->GetInt32(ROW, DEFAULT_ROW, &row);
    Int32 rowSpan = 0;
    a->GetInt32(ROW_SPAN, DEFAULT_SPAN_SIZE, &rowSpan);
    Float rowWeight;
    a->GetFloat(ROW_WEIGHT, Spec::DEFAULT_WEIGHT, &rowWeight);
    mRowSpec = GetSpec(row, rowSpan, GetAlignment(gravity, FALSE), rowWeight);
//    } finally {
    a->Recycle();
//    }
}

ECode GridLayout::GridLayoutLayoutParams::SetGravity(
    /* [in] */ Int32 gravity)
{
    AutoPtr<IGridLayoutSpec> spec;
    mRowSpec->CopyWriteAlignment(GetAlignment(gravity, FALSE), (IGridLayoutSpec**)&spec);
    mRowSpec = spec;
    spec = NULL;
    mColumnSpec->CopyWriteAlignment(GetAlignment(gravity, TRUE), (IGridLayoutSpec**)&spec);
    mColumnSpec = spec;
    return NOERROR;
}

ECode GridLayout::GridLayoutLayoutParams::SetBaseAttributes(
    /* [in] */ ITypedArray* attributes,
    /* [in] */ Int32 widthAttr,
    /* [in] */ Int32 heightAttr)
{
    attributes->GetLayoutDimension(widthAttr, DEFAULT_WIDTH, &mWidth);
    attributes->GetLayoutDimension(heightAttr, DEFAULT_HEIGHT, &mHeight);
    return NOERROR;
}

void GridLayout::GridLayoutLayoutParams::SetRowSpecSpan(
    /* [in] */ GridLayout::Interval* span)
{
    AutoPtr<IGridLayoutSpec> spec;
    mRowSpec->CopyWriteSpan(span, (IGridLayoutSpec**)&spec);
    mRowSpec = spec;
}

void GridLayout::GridLayoutLayoutParams::SetColumnSpecSpan(
    /* [in] */ GridLayout::Interval* span)
{
    AutoPtr<IGridLayoutSpec> spec;
    mColumnSpec->CopyWriteSpan(span, (IGridLayoutSpec**)&spec);
    mColumnSpec = spec;
}

ECode GridLayout::GridLayoutLayoutParams::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    IGridLayoutLayoutParams* glp = IGridLayoutLayoutParams::Probe(obj);
    if (glp == NULL) return NOERROR;

    GridLayoutLayoutParams* that = (GridLayoutLayoutParams*)glp;

    if (this == that) {
        *result = TRUE;
        return NOERROR;
    }

    if (!Object::Equals(mColumnSpec, that->mColumnSpec)) return NOERROR;
    if (!Object::Equals(mRowSpec, that->mRowSpec)) return NOERROR;

    *result = TRUE;
    return NOERROR;
}

ECode GridLayout::GridLayoutLayoutParams::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash)

    Int32 result = Object::GetHashCode(mRowSpec);
    result = 31 * result + Object::GetHashCode(mColumnSpec);
    *hash = result;
    return NOERROR;
}

ECode GridLayout::GridLayoutLayoutParams::SetRowSpec(
    /* [in] */ IGridLayoutSpec* r)
{
    mRowSpec = r;
    return NOERROR;
}

ECode GridLayout::GridLayoutLayoutParams::SetColumnSpec(
    /* [in] */ IGridLayoutSpec* c)
{
    mColumnSpec = c;
    return NOERROR;
}

ECode GridLayout::GridLayoutLayoutParams::GetRowSpec(
    /* [out] */ IGridLayoutSpec** r)
{
    VALIDATE_NOT_NULL(r)
    *r = mRowSpec;
    REFCOUNT_ADD(*r)
    return NOERROR;
}

ECode GridLayout::GridLayoutLayoutParams::GetColumnSpec(
    /* [out] */ IGridLayoutSpec** c)
{
    VALIDATE_NOT_NULL(c)
    *c = mColumnSpec;
    REFCOUNT_ADD(*c)
    return NOERROR;
}


//==================================================================
//                      GridLayout::Arc
//==================================================================
GridLayout::Arc::Arc(
    /* [in] */ Interval* span,
    /* [in] */ MutableInt* value)
    : mSpan(span)
    , mValue(value)
    , mValid(TRUE)
{}

ECode GridLayout::Arc::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);

    String str;
    mSpan->ToString(&str);
    str += String(" ");
    String mut;
    mValue->ToString(&mut);
    if (!mValid) {
        str += String("+>");
    }
    else {
        str += String("->");
    }
    str += String(" ");
    str += mut;
    *info = str;
    return NOERROR;
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

ECode GridLayout::MutableInt::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);

    *info = StringUtils::ToString(mValue);
    return NOERROR;
}


//==================================================================
//                      GridLayout::Bounds
//==================================================================
CAR_INTERFACE_IMPL(GridLayout::Bounds, Object, IBounds)

GridLayout::Bounds::Bounds()
{
    Reset();
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
    Int32 flexibility;
    spec->GetFlexibility(&flexibility);
    mFlexibility &= flexibility;
    Boolean horizontal = axis->mHorizontal;
    AutoPtr<Alignment> alignment = ((GridLayout*)(gl))->GetAlignment(spec->mAlignment, horizontal);
    // todo test this works correctly when the returned value is UNDEFINED
    Int32 mode = 0;
    IViewGroup::Probe(gl)->GetLayoutMode(&mode);
    Int32 before = 0;
    alignment->GetAlignmentValue(c, size, mode, &before);
    Include(before, size - before);
}

ECode GridLayout::Bounds::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    String before = StringUtils::ToString(mBefore);
    String after = StringUtils::ToString(mAfter);
    *info = String("Bounds{") + String("before=") +
            before + String(", after=") + after + String("}");
    return NOERROR;
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

ECode GridLayout::Interval::Size(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    *size = mMax - mMin;
    return NOERROR;
}

ECode GridLayout::Interval::Inverse(
    /* [out] */ IInterval** interval)
{
    VALIDATE_NOT_NULL(interval);
    *interval = new Interval(mMax, mMin);
    REFCOUNT_ADD(*interval)
    return NOERROR;
}

ECode GridLayout::Interval::Equals(
    /* [in] */ IInterface* that,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    IInterval* itv = IInterval::Probe(that);
    if (itv == NULL) return NOERROR;

    Interval* interval = (Interval*)itv;

    if (mMax != interval->mMax){
        return NOERROR;
    }
    //noinspection RedundantIfStatement
    if (mMin != interval->mMin) {
        return NOERROR;
    }

    *result = TRUE;
    return NOERROR;
}

ECode GridLayout::Interval::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    Int32 result = mMin;
    result = 31 * result + mMax;
    *hashCode = result;
    return NOERROR;
}

ECode GridLayout::Interval::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    String min = StringUtils::ToString(mMin);
    String max = StringUtils::ToString(mMax);
    *str = String("[") + min + String(", ") + max + String("]");
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


//==================================================================
//                      GridLayout::Spec
//==================================================================
AutoPtr<IGridLayoutSpec> GridLayout::Spec::UNDEFINED;
Float GridLayout::Spec::DEFAULT_WEIGHT;

CAR_INTERFACE_IMPL(GridLayout::Spec, Object, IGridLayoutSpec)

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
    mSpan = new Interval(start, start + size);
}

ECode GridLayout::Spec::CopyWriteSpan(
    /* [in] */ IInterval* span,
    /* [out] */ IGridLayoutSpec** spec)
{
    VALIDATE_NOT_NULL(spec)
    *spec = new Spec(mStartDefined, (Interval*)span, mAlignment, mWeight);
    REFCOUNT_ADD(*spec)
    return NOERROR;
}

ECode GridLayout::Spec::CopyWriteAlignment(
    /* [in] */ IGridLayoutAlignment* alignment,
    /* [out] */ IGridLayoutSpec** spec)
{
    VALIDATE_NOT_NULL(spec)
    *spec = new Spec(mStartDefined, mSpan, (Alignment*)alignment, mWeight);
    REFCOUNT_ADD(*spec)
    return NOERROR;
}

ECode GridLayout::Spec::GetFlexibility(
    /* [out] */ Int32* rst)
{
    VALIDATE_NOT_NULL(rst)
    *rst = (mAlignment == UNDEFINED_ALIGNMENT && mWeight == 0) ? INFLEXIBLE : CAN_STRETCH;
    return NOERROR;
}

ECode GridLayout::Spec::Equals(
    /* [in] */ IInterface* that,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;

    IGridLayoutSpec* sp = IGridLayoutSpec::Probe(that);
    if (sp == NULL) {
        return NOERROR;
    }


    Spec* spec = (Spec*)sp;

    if (this == spec) {
        *res = TRUE;
        return NOERROR;
    }

    if (!Object::Equals(mAlignment, spec->mAlignment)) {
        return NOERROR;
    }
    //noinspection RedundantIfStatement
    if (!Object::Equals(mSpan, spec->mSpan)) {
        return NOERROR;
    }

    *res = TRUE;
    return NOERROR;
}

ECode GridLayout::Spec::GetHashCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code);
    Int32 result = Object::GetHashCode(mSpan);
    result = 31 * result + Object::GetHashCode(mAlignment);
    *code = result;
    return NOERROR;
}


//==================================================================
//                      GridLayout::Alignment
//==================================================================
CAR_INTERFACE_IMPL(GridLayout::Alignment, Object, IGridLayoutAlignment)

GridLayout::Alignment::Alignment()
{}

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

ECode GridLayout::Alignment::GetBounds(
    /* [out] */ IBounds** bounds)
{
    VALIDATE_NOT_NULL(bounds)
    *bounds = new Bounds();
    REFCOUNT_ADD(*bounds)
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
    *offset = 0; // baseline gravity is top
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

ECode GridLayout::BaseLineAlignment::GetBounds(
    /* [out] */ IBounds** bounds)
{
    VALIDATE_NOT_NULL(bounds);
    *bounds = new AlignmentBounds();
    REFCOUNT_ADD(*bounds);
    return NOERROR;
}


//==================================================================
//                      GridLayout::AlignmentBounds
//==================================================================
GridLayout::AlignmentBounds::AlignmentBounds()
    : mSize(0)
{}

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
    /* [in] */ ArrayOf<Arc*>* arcs,
    /* [in] */ Axis* axis)
{
    mResult = ArrayOf<Arc*>::Alloc(arcs->GetLength());
    mCursor = mResult->GetLength() - 1;
    mArcsByVertex = axis->GroupArcsByFirstVertex(arcs);
    mVisited = ArrayOf<Int32>::Alloc(axis->GetCount() + 1);
}

void GridLayout::TopoSort::Walk(
    /* [in] */ Int32 loc)
{
    switch((*mVisited)[loc]) {
        case Axis::NEW:
        {
            (*mVisited)[loc] = Axis::PENDING;
            AutoPtr<ArrayOf<GridLayout::Arc*> > a = (*mArcsByVertex)[loc];
            for (Int32 i = 0; i < a->GetLength(); i++) {
                AutoPtr<Arc> arc = (*a)[i];
                Walk(arc->mSpan->mMax);
                mResult->Set(mCursor--, arc);
            }
            (*mVisited)[loc] = Axis::COMPLETE;
            break;
        }
        case Axis::PENDING:
            // le singe est dans l'arbre
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


//=====================================================================
// GridLayout
//=====================================================================
Int32 GridLayout::HORIZONTAL;
Int32 GridLayout::VERTICAL;
Int32 GridLayout::UNDEFINED;
Int32 GridLayout::ALIGN_BOUNDS = 0;
Int32 GridLayout::ALIGN_MARGINS = 1;
Int32 GridLayout::MAX_SIZE = 100000;
Int32 GridLayout::DEFAULT_CONTAINER_MARGIN = 0;
Int32 GridLayout::UNINITIALIZED_HASH = 0;
AutoPtr<GridLayout::Alignment> GridLayout::UNDEFINED_ALIGNMENT;
AutoPtr<GridLayout::Alignment> GridLayout::LEADING;
AutoPtr<GridLayout::Alignment> GridLayout::TRAILING;
AutoPtr<GridLayout::Alignment> GridLayout::TOP;
AutoPtr<GridLayout::Alignment> GridLayout::BOTTOM;
AutoPtr<GridLayout::Alignment> GridLayout::START;
AutoPtr<GridLayout::Alignment> GridLayout::END;
AutoPtr<GridLayout::Alignment> GridLayout::LEFT;
AutoPtr<GridLayout::Alignment> GridLayout::RIGHT;
AutoPtr<GridLayout::Alignment> GridLayout::CENTER;
AutoPtr<GridLayout::Alignment> GridLayout::BASELINE;
AutoPtr<GridLayout::Alignment> GridLayout::FILL;
String GridLayout::TAG("GridLayout");
Int32 GridLayout::DEFAULT_ORIENTATION;
Int32 GridLayout::DEFAULT_COUNT;
Boolean GridLayout::DEFAULT_USE_DEFAULT_MARGINS = FALSE;
Boolean GridLayout::DEFAULT_ORDER_PRESERVED = TRUE;
Int32 GridLayout::DEFAULT_ALIGNMENT_MODE;
Int32 GridLayout::ORIENTATION;
Int32 GridLayout::ROW_COUNT;
Int32 GridLayout::COLUMN_COUNT;
Int32 GridLayout::USE_DEFAULT_MARGINS;
Int32 GridLayout::ALIGNMENT_MODE;
Int32 GridLayout::ROW_ORDER_PRESERVED;
Int32 GridLayout::COLUMN_ORDER_PRESERVED;
Int32 GridLayout::INFLEXIBLE = 0;
Int32 GridLayout::CAN_STRETCH = 2;
INIT_PROI_4 const GridLayout::StaticInitializer GridLayout::sInitializer;

CAR_INTERFACE_IMPL(GridLayout, ViewGroup, IGridLayout)

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
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode GridLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode GridLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return constructor(context, attrs, defStyle, 0);
}

ECode GridLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(ViewGroup::constructor(context, attrs, defStyleAttr, defStyleRes));

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

ECode GridLayout::GetOrientation(
    /* [out] */ Int32* orientation)
{
    VALIDATE_NOT_NULL(orientation)
    *orientation = mOrientation;
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

ECode GridLayout::GetRowCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mVerticalAxis->GetCount();
    return NOERROR;
}

ECode GridLayout::SetRowCount(
    /* [in] */ Int32 rowCount)
{
    mVerticalAxis->SetCount(rowCount);
    InvalidateStructure();
    RequestLayout();
    return NOERROR;
}

ECode GridLayout::GetColumnCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mHorizontalAxis->GetCount();
    return NOERROR;
}

ECode GridLayout::SetColumnCount(
    /* [in] */ Int32 columnCount)
{
    mHorizontalAxis->SetCount(columnCount);
    InvalidateStructure();
    RequestLayout();
    return NOERROR;
}

ECode GridLayout::GetUseDefaultMargins(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mUseDefaultMargins;
    return NOERROR;
}

ECode GridLayout::SetUseDefaultMargins(
    /* [in] */ Boolean useDefaultMargins)
{
    mUseDefaultMargins = useDefaultMargins;
    RequestLayout();
    return NOERROR;
}

ECode GridLayout::GetAlignmentMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAlignmentMode;
    return NOERROR;
}

ECode GridLayout::SetAlignmentMode(
    /* [in] */ Int32 alignmentMode)
{
    mAlignmentMode = alignmentMode;
    RequestLayout();
    return NOERROR;
}

ECode GridLayout::IsRowOrderPreserved(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mVerticalAxis->IsOrderPreserved();
    return NOERROR;
}

ECode GridLayout::SetRowOrderPreserved(
    /* [in] */ Boolean rowOrderPreserved)
{
    mVerticalAxis->SetOrderPreserved(rowOrderPreserved);
    InvalidateStructure();
    RequestLayout();
    return NOERROR;
}

ECode GridLayout::IsColumnOrderPreserved(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mHorizontalAxis->IsOrderPreserved();
    return NOERROR;
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
    return /* isAtEdge ? DEFAULT_CONTAINER_MARGIN : */GetDefaultMargin(c, horizontal, leading);
}

Int32 GridLayout::GetDefaultMargin(
    /* [in] */ IView* c,
    /* [in] */ IGridLayoutLayoutParams* glp,
    /* [in] */ Boolean horizontal,
    /* [in] */ Boolean leading)
{
    if (!mUseDefaultMargins) {
        return 0;
    }
    AutoPtr<Spec> spec;
    AutoPtr<Axis> axis;
    if (horizontal) {
        AutoPtr<IGridLayoutSpec> columnSpec;
        glp->GetColumnSpec((IGridLayoutSpec**)&columnSpec);
        spec = (Spec*)columnSpec.Get();
        axis = mHorizontalAxis;
    }
    else {
        AutoPtr<IGridLayoutSpec> rowSpec;
        glp->GetRowSpec((IGridLayoutSpec**)&rowSpec);
        spec = (Spec*)rowSpec.Get();
        axis = mVerticalAxis;
    }

    AutoPtr<Interval> span = spec->mSpan;
    Boolean isLayoutRtl;
    IsLayoutRtl(&isLayoutRtl);
    Boolean leading1 = (horizontal && isLayoutRtl) ? !leading : leading;
    Boolean isAtEdge = leading1 ? (span->mMin == 0) : (span->mMax == axis->GetCount());

    return GetDefaultMargin(c, isAtEdge, horizontal, leading);
}

Int32 GridLayout::GetMargin1(
    /* [in] */ IView* view,
    /* [in] */ Boolean horizontal,
    /* [in] */ Boolean leading)
{
    AutoPtr<IGridLayoutLayoutParams> lp = GetLayoutParams(view);
    Int32 margin;
    if (horizontal) {
        if (leading) {
            IViewGroupMarginLayoutParams::Probe(lp)->GetLeftMargin(&margin);
        }
        else {
            IViewGroupMarginLayoutParams::Probe(lp)->GetRightMargin(&margin);
        }
    }
    else {
        if (leading) {
            IViewGroupMarginLayoutParams::Probe(lp)->GetTopMargin(&margin);
        }
        else {
            IViewGroupMarginLayoutParams::Probe(lp)->GetBottomMargin(&margin);
        }
    }
    return margin == UNDEFINED ?
            GetDefaultMargin(view, lp, horizontal, leading) : margin;
}

Int32 GridLayout::GetMargin(
    /* [in] */ IView* view,
    /* [in] */ Boolean horizontal,
    /* [in] */ Boolean leading)
{
    if (mAlignmentMode == ALIGN_MARGINS) {
        return GetMargin1(view, horizontal, leading);
    }
    else {
        AutoPtr<Axis> axis = horizontal ? mHorizontalAxis : mVerticalAxis;
        AutoPtr< ArrayOf<Int32> > margins = leading ? axis->GetLeadingMargins() : axis->GetTrailingMargins();
        AutoPtr<IGridLayoutLayoutParams> lp = GetLayoutParams(view);
        AutoPtr<Spec> spec;
        if (horizontal) {
            AutoPtr<IGridLayoutSpec> columnSpec;
            lp->GetColumnSpec((IGridLayoutSpec**)&columnSpec);
            spec = (Spec*)columnSpec.Get();
        }
        else {
            AutoPtr<IGridLayoutSpec> rowSpec;
            lp->GetRowSpec((IGridLayoutSpec**)&rowSpec);
            spec = (Spec*)rowSpec.Get();
        }
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
    Arrays::Fill(a, Elastos::Core::Math::Min(start, length),
            Elastos::Core::Math::Min(end, length), value);
}

void GridLayout::SetCellGroup(
    /* [in] */ IGridLayoutLayoutParams* lp,
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
    Int32 size;
    minorRange->Size(&size);
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

    Int32 N;
    GetChildCount(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IView> v;
        GetChildAt(i, (IView**)&v);
        AutoPtr<IViewGroupLayoutParams> lp;
        v->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        AutoPtr<IGridLayoutLayoutParams> glp = IGridLayoutLayoutParams::Probe(lp);

        AutoPtr<Spec> majorSpec;
        if (horizontal) {
            AutoPtr<IGridLayoutSpec> rowSpec;
            glp->GetRowSpec((IGridLayoutSpec**)&rowSpec);
            majorSpec = (Spec*)rowSpec.Get();
        }
        else {
            AutoPtr<IGridLayoutSpec> columnSpec;
            glp->GetColumnSpec((IGridLayoutSpec**)&columnSpec);
            majorSpec = (Spec*)columnSpec.Get();
        }
        AutoPtr<Interval> majorRange = majorSpec->mSpan;
        Boolean majorWasDefined = majorSpec->mStartDefined;
        Int32 majorSpan;
        majorRange->Size(&majorSpan);
        if (majorWasDefined) {
            major = majorRange->mMin;
        }

        AutoPtr<Spec> minorSpec;
        if (horizontal) {
            AutoPtr<IGridLayoutSpec> columnSpec;
            glp->GetColumnSpec((IGridLayoutSpec**)&columnSpec);
            minorSpec = (Spec*)columnSpec.Get();
        }
        else {
            AutoPtr<IGridLayoutSpec> rowSpec;
            glp->GetRowSpec((IGridLayoutSpec**)&rowSpec);
            minorSpec = (Spec*)rowSpec.Get();
        }
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
                    }
                    else {
                        if (minor + minorSpan <= count) {
                            minor++;
                        }
                        else {
                            minor = 0;
                            major++;
                        }
                    }
                }
            }
            ProcrusteanFill(maxSizes, minor, minor + minorSpan, major + majorSpan);
        }

        if (horizontal) {
            SetCellGroup(glp, major, majorSpan, minor, minorSpan);
        }
        else {
            SetCellGroup(glp, minor, minorSpan, major, majorSpan);
        }

        minor = minor + minorSpan;
    }
}

void GridLayout::InvalidateStructure()
{
    mLastLayoutParamsHashCode = UNINITIALIZED_HASH;
    mHorizontalAxis->InvalidateStructure();
    mVerticalAxis->InvalidateStructure();
    // This can end up being done twice. Better twice than not at all.
    InvalidateValues();
}

void GridLayout::InvalidateValues()
{
    // Need null check because requestLayout() is called in View's initializer,
    // before we are set up.
    if (mHorizontalAxis != NULL && mVerticalAxis != NULL) {
        mHorizontalAxis->InvalidateValues();
        mVerticalAxis->InvalidateValues();
    }
}

void GridLayout::OnSetLayoutParams(
    /* [in] */ IView* child,
    /* [in] */ IViewGroupLayoutParams* layoutParams)
{
    ViewGroup::OnSetLayoutParams(child, layoutParams);

    if (!CheckLayoutParams(layoutParams)) {
        String wt("supplied LayoutParams are of the wrong type");
        HandleInvalidParams(wt);
    }

    InvalidateStructure();
}

AutoPtr<IGridLayoutLayoutParams> GridLayout::GetLayoutParams(
    /* [in] */ IView* c)
{
    AutoPtr<IViewGroupLayoutParams> lp;
    c->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    return IGridLayoutLayoutParams::Probe(lp);
}

ECode GridLayout::HandleInvalidParams(
    /* [in] */ const String& msg)
{
    Logger::E(TAG, "%s. ", msg.string());
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode GridLayout::CheckLayoutParams(
    /* [in] */ IGridLayoutLayoutParams* lp,
    /* [in] */ Boolean horizontal)
{
    String groupName = horizontal ? String("column") : String("row");
    AutoPtr<Spec> spec;
    if (horizontal) {
        AutoPtr<IGridLayoutSpec> columnSpec;
        lp->GetColumnSpec((IGridLayoutSpec**)&columnSpec);
        spec = (Spec*)columnSpec.Get();
    }
    else {
        AutoPtr<IGridLayoutSpec> rowSpec;
        lp->GetRowSpec((IGridLayoutSpec**)&rowSpec);
        spec = (Spec*)rowSpec.Get();
    }
    AutoPtr<Interval> span = spec->mSpan;
    if (span->mMin != UNDEFINED && span->mMin < 0) {
        String wt = groupName + String(" indices must be positive");
        FAIL_RETURN(HandleInvalidParams(wt))
    }
    AutoPtr<Axis> axis = horizontal ? mHorizontalAxis : mVerticalAxis;
    Int32 count = axis->mDefinedCount;
    if (count != UNDEFINED) {
        if (span->mMax > count) {
            String wt = groupName + String(" indices (start + span) mustn't exceed the ") + groupName + String(" count");
            return HandleInvalidParams(wt);
        }
        Int32 size;
        if (span->Size(&size), size > count) {
            String wt = groupName + String(" span mustn't exceed the ") + groupName + String(" count");
            return HandleInvalidParams(wt);
        }
    }
    return NOERROR;
}

Boolean GridLayout::CheckLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    IGridLayoutLayoutParams* gllp = IGridLayoutLayoutParams::Probe(p);
    if (!gllp) {
        return FALSE;
    }

    if (FAILED(CheckLayoutParams(gllp, TRUE))) return FALSE;
    if (FAILED(CheckLayoutParams(gllp, FALSE))) return FALSE;

    return TRUE;
}

ECode GridLayout::GenerateDefaultLayoutParams(
    /* [out] */ IViewGroupLayoutParams** params)
{
    VALIDATE_NOT_NULL(params);
    return CGridLayoutLayoutParams::New(params);
}

ECode GridLayout::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IViewGroupLayoutParams** params)
{
    VALIDATE_NOT_NULL(params);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    return CGridLayoutLayoutParams::New(context, attrs, params);
}

AutoPtr<IViewGroupLayoutParams> GridLayout::GenerateLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    AutoPtr<IViewGroupLayoutParams> lp;
    CGridLayoutLayoutParams::New(p, (IViewGroupLayoutParams**)&lp);
    return lp;
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
    }
    else {
        graphics->DrawLine(x1, y1, x2, y2, paint);
    }
}

void GridLayout::OnDebugDrawMargins(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IPaint* paint)
{
    AutoPtr<IGridLayoutLayoutParams> lp;
    CGridLayoutLayoutParams::New((IGridLayoutLayoutParams**)&lp);
    Int32 N;
    GetChildCount(&N);
    for (Int32 i = 0; i < N; i++) {
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
    paint->SetColor(Color::Argb(50, 255, 255, 255));

    AutoPtr<IInsets> insets;
    GetOpticalInsets((IInsets**)&insets);

    Int32 paddingTop, paddingLeft, paddingRight, paddingBottom;
    GetPaddingLeft(&paddingLeft);
    GetPaddingTop(&paddingTop);
    GetPaddingRight(&paddingRight);
    GetPaddingBottom(&paddingBottom);
    Int32 w, h;
    GetWidth(&w);
    GetHeight(&h);
    Int32 it, il, ir, ib;
    insets->GetTop(&it);
    insets->GetLeft(&il);
    insets->GetRight(&ir);
    insets->GetBottom(&ib);

    Int32 top    =     paddingTop    + it;
    Int32 left   =     paddingLeft   + il;
    Int32 right  = w - paddingRight  - ir;
    Int32 bottom = h - paddingBottom - ib;

    AutoPtr< ArrayOf<Int32> > xs = mHorizontalAxis->mLocations;
    if (xs != NULL) {
        for (Int32 i = 0, length = xs->GetLength(); i < length; i++) {
            Int32 x = left + (*xs)[i];
            DrawLine(canvas, x, top, x, bottom, paint);
        }
    }

    AutoPtr< ArrayOf<Int32> > ys = mVerticalAxis->mLocations;
    if (ys != NULL) {
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

Int32 GridLayout::ComputeLayoutParamsHashCode()
{
    Int32 result = 1;
    Int32 N;
    GetChildCount(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IView> c;
        GetChildAt(i, (IView**)&c);
        Int32 visibility = 0;
        c->GetVisibility(&visibility);
        if (visibility == IView::GONE) continue;
        AutoPtr<IViewGroupLayoutParams> lp;
        c->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        result = 31 * result + Object::GetHashCode(lp);
    }
    return result;
}

void GridLayout::ConsistencyCheck()
{
    if (mLastLayoutParamsHashCode == UNINITIALIZED_HASH) {
        ValidateLayoutParams();
        mLastLayoutParamsHashCode = ComputeLayoutParamsHashCode();
    }
    else if (mLastLayoutParamsHashCode != ComputeLayoutParamsHashCode()) {
        Logger::D(TAG, "The fields of some layout parameters were modified in between layout"
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
    Int32 N;
    GetChildCount(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IView> c;
        GetChildAt(i, (IView**)&c);
        Int32 visibility = 0;
        c->GetVisibility(&visibility);
        if (visibility == IView::GONE) continue;
        AutoPtr<IGridLayoutLayoutParams> glp = GetLayoutParams(c);
        if (firstPass) {
            AutoPtr<IViewGroupLayoutParams> lp = IViewGroupLayoutParams::Probe(glp);
            Int32 width, height;
            lp->GetWidth(&width);
            lp->GetHeight(&height);
            MeasureChildWithMargins2(c, widthSpec, heightSpec, width, height);
            mHorizontalAxis->RecordOriginalMeasurement(i);
            mVerticalAxis->RecordOriginalMeasurement(i);
        }
        else {
            Boolean horizontal = (mOrientation == HORIZONTAL);
            AutoPtr<Spec> spec;
            if (horizontal) {
                AutoPtr<IGridLayoutSpec> columnSpec;
                glp->GetColumnSpec((IGridLayoutSpec**)&columnSpec);
                spec = (Spec*)columnSpec.Get();
            }
            else {
                AutoPtr<IGridLayoutSpec> rowSpec;
                glp->GetRowSpec((IGridLayoutSpec**)&rowSpec);
                spec = (Spec*)rowSpec.Get();
            }
            if (spec->mAlignment == FILL) {
                AutoPtr<Interval> span = spec->mSpan;
                AutoPtr<Axis> axis = horizontal ? mHorizontalAxis : mVerticalAxis;
                AutoPtr< ArrayOf<Int32> > locations = axis->GetLocations();
                Int32 cellSize = (*locations)[span->mMax] - (*locations)[span->mMin];
                Int32 viewSize = cellSize - GetTotalMargin(c, horizontal);
                if (horizontal) {
                    Int32 height;
                    IViewGroupLayoutParams::Probe(glp)->GetHeight(&height);
                    MeasureChildWithMargins2(c, widthSpec, heightSpec, viewSize, height);
                }
                else {
                    Int32 width;
                    IViewGroupLayoutParams::Probe(glp)->GetWidth(&width);
                    MeasureChildWithMargins2(c, widthSpec, heightSpec, width, viewSize);
                }
            }
        }
    }
}

Int32 GridLayout::Adjust(
    /* [in] */ Int32 measureSpec,
    /* [in] */ Int32 delta)
{
    return View::MeasureSpec::MakeMeasureSpec(
            MeasureSpec::GetSize(measureSpec + delta),  MeasureSpec::GetMode(measureSpec));
}

void GridLayout::OnMeasure(
    /* [in] */ Int32 widthSpec,
    /* [in] */ Int32 heightSpec)
{
    ConsistencyCheck();

    InvalidateValues();

    Int32 paddingLeft, paddingRight, paddingTop, paddingBottom;
    GetPaddingLeft(&paddingLeft);
    GetPaddingTop(&paddingTop);
    GetPaddingRight(&paddingRight);
    GetPaddingBottom(&paddingBottom);

    Int32 hPadding = paddingLeft + paddingRight;
    Int32 vPadding = paddingTop  + paddingBottom;

    Int32 widthSpecSansPadding =  Adjust(widthSpec, -hPadding);
    Int32 heightSpecSansPadding = Adjust(heightSpec, -vPadding);

    MeasureChildrenWithMargins(widthSpecSansPadding, heightSpecSansPadding, TRUE);

    Int32 widthSansPadding = 0, heightSansPadding = 0;

    if (mOrientation == HORIZONTAL) {
        widthSansPadding = mHorizontalAxis->GetMeasure(widthSpecSansPadding);
        MeasureChildrenWithMargins(widthSpecSansPadding, heightSpecSansPadding, FALSE);
        heightSansPadding = mVerticalAxis->GetMeasure(heightSpecSansPadding);
    }
    else {
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

Int32 GridLayout::GetMeasurement(
    /* [in] */ IView* c,
    /* [in] */ Boolean horizontal)
{
    Int32 result = 0;
    if (horizontal) {
        c->GetMeasuredWidth(&result);
    }
    else {
        c->GetMeasuredHeight(&result);
    }
    return result;
}

ECode GridLayout::GetMeasurementIncludingMargin(
    /* [in] */ IView* c,
    /* [in] */ Boolean horizontal,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 visibility = 0;
    c->GetVisibility(&visibility);
    if (visibility == IView::GONE) {
        *result = 0;
        return NOERROR;
    }
    *result = GetMeasurement(c, horizontal) + GetTotalMargin(c, horizontal);
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
    }
    else {
        if (horizontal) {
            return START;
        }
        else {
            return BASELINE;
        }
    }
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

    Int32 paddingLeft, paddingRight, paddingTop, paddingBottom;
    GetPaddingLeft(&paddingLeft);
    GetPaddingTop(&paddingTop);
    GetPaddingRight(&paddingRight);
    GetPaddingBottom(&paddingBottom);

    mHorizontalAxis->Layout(targetWidth - paddingLeft - paddingRight);
    mVerticalAxis->Layout(targetHeight - paddingTop - paddingBottom);

    AutoPtr< ArrayOf<Int32> > hLocations = mHorizontalAxis->GetLocations();
    AutoPtr< ArrayOf<Int32> > vLocations = mVerticalAxis->GetLocations();

    Int32 N;
    GetChildCount(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IView> c;
        GetChildAt(i, (IView**)&c);
        Int32 visibility = 0;
        c->GetVisibility(&visibility);
        if (visibility == IView::GONE) continue;
        AutoPtr<IGridLayoutLayoutParams> lp = GetLayoutParams(c);
        AutoPtr<IGridLayoutSpec> columnSpec, rowSpec;
        lp->GetColumnSpec((IGridLayoutSpec**)&columnSpec);
        lp->GetRowSpec((IGridLayoutSpec**)&rowSpec);
        Spec* columnSpecImpl = (Spec*)columnSpec.Get();
        Spec* rowSpecImpl = (Spec*)rowSpec.Get();

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

        AutoPtr<Bounds> boundsX = (Bounds*)mHorizontalAxis->GetGroupBounds()->GetValue(i).Get();
        AutoPtr<Bounds> boundsY = (Bounds*)mVerticalAxis->GetGroupBounds()->GetValue(i).Get();

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
        Int32 cx = !isLayoutRtl ? paddingLeft + leftMargin + dx :
                targetWidth - width - paddingRight - rightMargin - dx;
        Int32 cy = paddingTop + y1 + gravityOffsetY + alignmentOffsetY + topMargin;

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

ECode GridLayout::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    ViewGroup::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> seq = CoreUtils::Convert(String("CGridLayout"));
    IAccessibilityRecord::Probe(event)->SetClassName(seq);
    return NOERROR;
}

ECode GridLayout::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    ViewGroup::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> seq = CoreUtils::Convert(String("CGridLayout"));
    info->SetClassName(seq);
    return NOERROR;
}

AutoPtr<IGridLayoutSpec> GridLayout::GetSpec(
    /* [in] */ Int32 start,
    /* [in] */ Int32 size,
    /* [in] */ Alignment* alignment,
    /* [in] */ Float weight)
{
    return new Spec(start != UNDEFINED, start, size, alignment, weight);
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

AutoPtr<GridLayout::Alignment> GridLayout::CreateSwitchingAlignment(
    /* [in] */ Alignment* ltr,
    /* [in] */ Alignment* rtl)
{
    return new SwitchingAlignment(ltr, rtl);
}

Boolean GridLayout::CanStretch(
    /* [in] */ Int32 flexibility)
{
    return (flexibility & CAN_STRETCH) != 0;
}


//=====================================================================
// GridLayout::StaticInitializer
//=====================================================================
GridLayout::StaticInitializer::StaticInitializer()
{
    //=====================================================================
    // GridLayout
    //=====================================================================
    GridLayout::HORIZONTAL = ILinearLayout::HORIZONTAL;
    GridLayout::VERTICAL = ILinearLayout::VERTICAL;
    GridLayout::UNDEFINED = Elastos::Core::Math::INT32_MIN_VALUE;
    GridLayout::UNDEFINED_ALIGNMENT = new GridLayout::UndefinedAlignment();
    GridLayout::LEADING = new GridLayout::LeadingAlignment();
    GridLayout::TRAILING = new GridLayout::TrailingAlignment();
    GridLayout::TOP = GridLayout::LEADING;
    GridLayout::BOTTOM = GridLayout::TRAILING;
    GridLayout::START = GridLayout::LEADING;
    GridLayout::END = GridLayout::TRAILING;
    GridLayout::LEFT = GridLayout::CreateSwitchingAlignment(GridLayout::START, GridLayout::END);
    GridLayout::RIGHT = GridLayout::CreateSwitchingAlignment(GridLayout::END, GridLayout::START);
    GridLayout::CENTER = new GridLayout::CenterAlignment();
    GridLayout::BASELINE = new GridLayout::BaseLineAlignment();
    GridLayout::FILL = new GridLayout::FillAlignment();
    GridLayout::DEFAULT_ORIENTATION = HORIZONTAL;
    GridLayout::DEFAULT_COUNT = UNDEFINED;
    GridLayout::DEFAULT_ALIGNMENT_MODE = ALIGN_MARGINS;
    GridLayout::ORIENTATION = R::styleable::GridLayout_orientation;
    GridLayout::ROW_COUNT = R::styleable::GridLayout_rowCount;
    GridLayout::COLUMN_COUNT = R::styleable::GridLayout_columnCount;
    GridLayout::USE_DEFAULT_MARGINS = R::styleable::GridLayout_useDefaultMargins;
    GridLayout::ALIGNMENT_MODE = R::styleable::GridLayout_alignmentMode;
    GridLayout::ROW_ORDER_PRESERVED = R::styleable::GridLayout_rowOrderPreserved;
    GridLayout::COLUMN_ORDER_PRESERVED = R::styleable::GridLayout_columnOrderPreserved;

    //=================================================================
    // GridLayout::GridLayoutLayoutParams
    //=================================================================
    GridLayout::GridLayoutLayoutParams::DEFAULT_WIDTH = IViewGroupLayoutParams::WRAP_CONTENT;
    GridLayout::GridLayoutLayoutParams::DEFAULT_HEIGHT = IViewGroupLayoutParams::WRAP_CONTENT;
    GridLayout::GridLayoutLayoutParams::DEFAULT_MARGIN = Elastos::Core::Math::INT32_MIN_VALUE;
    GridLayout::GridLayoutLayoutParams::DEFAULT_ROW = Elastos::Core::Math::INT32_MIN_VALUE;
    GridLayout::GridLayoutLayoutParams::DEFAULT_COLUMN = Elastos::Core::Math::INT32_MIN_VALUE;
    GridLayout::GridLayoutLayoutParams::DEFAULT_SPAN = new GridLayout::Interval(GridLayout::UNDEFINED, GridLayout::UNDEFINED + 1);
    Int32 size;
    GridLayout::GridLayoutLayoutParams::DEFAULT_SPAN->Size(&size);
    GridLayout::GridLayoutLayoutParams::DEFAULT_SPAN_SIZE = size;

    GridLayout::GridLayoutLayoutParams::MARGIN = R::styleable::ViewGroup_MarginLayout_layout_margin;
    GridLayout::GridLayoutLayoutParams::LEFT_MARGIN = R::styleable::ViewGroup_MarginLayout_layout_marginLeft;
    GridLayout::GridLayoutLayoutParams::TOP_MARGIN = R::styleable::ViewGroup_MarginLayout_layout_marginTop;
    GridLayout::GridLayoutLayoutParams::RIGHT_MARGIN = R::styleable::ViewGroup_MarginLayout_layout_marginRight;
    GridLayout::GridLayoutLayoutParams::BOTTOM_MARGIN = R::styleable::ViewGroup_MarginLayout_layout_marginBottom;

    GridLayout::GridLayoutLayoutParams::COLUMN = R::styleable::GridLayout_Layout_layout_column;
    GridLayout::GridLayoutLayoutParams::COLUMN_SPAN = R::styleable::GridLayout_Layout_layout_columnSpan;
    GridLayout::GridLayoutLayoutParams::COLUMN_WEIGHT = R::styleable::GridLayout_Layout_layout_columnWeight;

    GridLayout::GridLayoutLayoutParams::ROW = R::styleable::GridLayout_Layout_layout_row;
    GridLayout::GridLayoutLayoutParams::ROW_SPAN = R::styleable::GridLayout_Layout_layout_rowSpan;
    GridLayout::GridLayoutLayoutParams::ROW_WEIGHT = R::styleable::GridLayout_Layout_layout_rowWeight;

    GridLayout::GridLayoutLayoutParams::GRAVITY = R::styleable::GridLayout_Layout_layout_gravity;

    //==================================================================
    //                      GridLayout::Spec
    //==================================================================
    GridLayout::Spec::UNDEFINED = GridLayout::GetSpec(GridLayout::UNDEFINED);
    GridLayout::Spec::DEFAULT_WEIGHT = 0;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
