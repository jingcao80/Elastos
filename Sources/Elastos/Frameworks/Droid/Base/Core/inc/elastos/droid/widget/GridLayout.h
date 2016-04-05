
#ifndef __ELASTOS_DROID_WIDGET_GRIDLAYOUT_H__
#define __ELASTOS_DROID_WIDGET_GRIDLAYOUT_H__

#define HASH_FOR_WIDGET
#include "elastos/droid/ext/frameworkhash.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/ViewGroup.h"
#include <elastos/core/Math.h>
#include <elastos/droid/R.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/Pair.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::Pair;
using Elastos::Utility::Etl::HashMap;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewGroupMarginLayoutParams;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;

namespace Elastos {
namespace Droid {
namespace Widget {

class ECO_PUBLIC GridLayout
    : public ViewGroup
    , public IGridLayout
{
public:
    class Arc;
    class Interval;
    class MutableInt;
    class Alignment;
    class Spec;
    class Bounds;

public:
    class LayoutParams
        : public ViewGroup::MarginLayoutParams
        , public IGridLayoutLayoutParams
    {
    private:
        CARAPI constructor(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height,
            /* [in] */ Int32 left,
            /* [in] */ Int32 top,
            /* [in] */ Int32 right,
            /* [in] */ Int32 bottom,
            /* [in] */ IGridLayoutSpec* rowSpec,
            /* [in] */ IGridLayoutSpec* columnSpec);

        CARAPI_(void) ReInitSuper(
            /* [in] */ IContext* context,
            /* [in] */ IAttributeSet* attrs);

        CARAPI_(void) Init(
            /* [in] */ IContext* context,
            /* [in] */ IAttributeSet* attrs);

    public:
        CAR_INTERFACE_DECL()

        LayoutParams();

        CARAPI constructor(
            /* [in] */ IGridLayoutSpec* rowSpec,
            /* [in] */ IGridLayoutSpec* columnSpec);

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ IViewGroupLayoutParams* params);

        CARAPI constructor(
            /* [in] */ IViewGroupMarginLayoutParams* params);

        CARAPI constructor(
            /* [in] */ IGridLayoutLayoutParams* that);

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ IAttributeSet* attrs);

        CARAPI SetGravity(
            /* [in] */ Int32 gravity);

        CARAPI SetRowSpec(
            /* [in] */ IGridLayoutSpec* r);

        CARAPI SetColumnSpec(
            /* [in] */ IGridLayoutSpec* c);

        CARAPI GetRowSpec(
            /* [out] */ IGridLayoutSpec** r);

        CARAPI GetColumnSpec(
            /* [out] */ IGridLayoutSpec** c);

        CARAPI_(void) SetRowSpecSpan(
            /* [in] */ GridLayout::Interval* span);

        CARAPI_(void) SetColumnSpecSpan(
            /* [in] */ GridLayout::Interval* span);

        virtual CARAPI Equals(
            /* [in] */ IInterface* o,
            /* [out] */ Boolean* result);

        virtual CARAPI GetHashCode(
            /* [in] */ Int32* hash);

    protected:
        virtual CARAPI SetBaseAttributes(
            /* [in] */ ITypedArray* attributes,
            /* [in] */ Int32 widthAttr,
            /* [in] */ Int32 heightAttr);

    private:
        static const Int32 DEFAULT_WIDTH = IViewGroupLayoutParams::WRAP_CONTENT;
        static const Int32 DEFAULT_HEIGHT = IViewGroupLayoutParams::WRAP_CONTENT;
        static const Int32 DEFAULT_MARGIN;// = Elastos::Core::Math::INT32_MIN_VALUE;
        static const Int32 DEFAULT_ROW ;// Elastos::Core::Math::INT32_MIN_VALUE;
        static const Int32 DEFAULT_COLUMN;// = Elastos::Core::Math::INT32_MIN_VALUE;
        static const AutoPtr<GridLayout::Interval> DEFAULT_SPAN;
        static const Int32 DEFAULT_SPAN_SIZE;

        static const Int32 MARGIN = R::styleable::ViewGroup_MarginLayout_layout_margin;
        static const Int32 LEFT_MARGIN = R::styleable::ViewGroup_MarginLayout_layout_marginLeft;
        static const Int32 TOP_MARGIN = R::styleable::ViewGroup_MarginLayout_layout_marginTop;
        static const Int32 RIGHT_MARGIN = R::styleable::ViewGroup_MarginLayout_layout_marginRight;
        static const Int32 BOTTOM_MARGIN = R::styleable::ViewGroup_MarginLayout_layout_marginBottom;
        static const Int32 COLUMN = R::styleable::GridLayout_Layout_layout_column;
        static const Int32 COLUMN_SPAN = R::styleable::GridLayout_Layout_layout_columnSpan;
        static const Int32 COLUMN_WEIGHT = R::styleable::GridLayout_Layout_layout_columnWeight;
        static const Int32 ROW = R::styleable::GridLayout_Layout_layout_row;
        static const Int32 ROW_SPAN = R::styleable::GridLayout_Layout_layout_rowSpan;
        static const Int32 ROW_WEIGHT = R::styleable::GridLayout_Layout_layout_rowWeight;
        static const Int32 GRAVITY = R::styleable::GridLayout_Layout_layout_gravity;

    public:
        AutoPtr<IGridLayoutSpec> mRowSpec;
        AutoPtr<IGridLayoutSpec> mColumnSpec;
    };

    template<typename K, typename V>
    class PackedMap
        : public Object
    {
    public:
        AutoPtr< ArrayOf<Int32> > mIndex;
        AutoPtr< ArrayOf<K> > mKeys;
        AutoPtr< ArrayOf<V> > mValues;

    public:
        PackedMap(
            /* [in] */ ArrayOf<K>* keys,
            /* [in] */ ArrayOf<V>* values)
        {
            mIndex = CreateIndex(keys);
            mKeys = Compact(keys, mIndex);
            mValues = Compact(values, mIndex);
        }

        static AutoPtr< ArrayOf<Int32> > CreateIndex(
            /* [in] */ ArrayOf<K>* keys)
        {
            Int32 size = keys->GetLength();
            AutoPtr< ArrayOf<Int32> > result = ArrayOf<Int32>::Alloc(size);
            HashMap<K, Int32> keyToIndex;
            for(Int32 i = 0; i < size; i ++) {
                K key = (*keys)[i];
                Int32 index;
                if (keyToIndex.Find(key) != keyToIndex.End()) {
                    index = keyToIndex.Find(key)->mSecond;
                }
                else {
                    index = keyToIndex.GetSize();
                    keyToIndex[key] = index;
                }
                (*result)[i] = index;
            }
            return result;
        }

        template<typename T>
        static AutoPtr< ArrayOf<T> > Compact(
            /* [in] */ ArrayOf<T>* keys,
            /* [in] */ ArrayOf<Int32>* index)
        {
            Int32 size = keys->GetLength();
            Int32 len = Max2(index, -1) + 1;
            if (len < 0 || len > 255) return NULL;
            AutoPtr< ArrayOf<T> > result = ArrayOf<T>::Alloc(len);
            for (Int32 i = 0; i < size; i++) {
                result->Set((*index)[i], (*keys)[i]);
            }
            return result;
        }

        V GetValue(Int32 i) {
            return (*mValues)[(*mIndex)[i]];
        }

    };

    class Axis
        : public Object
    {
        friend class GridLayout;
        friend class TopoSort;
    private:
        Axis(
            /* [in] */ Boolean horizontal,
            /* [in] */ GridLayout* host);

        CARAPI_(Int32) CalculateMaxIndex();

        CARAPI_(Int32) GetMaxIndex();

        AutoPtr<PackedMap<AutoPtr<IGridLayoutSpec>, AutoPtr<IBounds> > > CreateGroupBounds();

        CARAPI_(void) ComputeGroupBounds();

        AutoPtr< PackedMap<AutoPtr<IInterval>, AutoPtr<MutableInt> > > CreateLinks(
            /* [in] */ Boolean min);

        CARAPI_(void) ComputeLinks(
            /* [in] */ PackedMap<AutoPtr<IInterval>, AutoPtr<MutableInt> >* links,
            /* [in] */ Boolean min);

        AutoPtr< PackedMap<AutoPtr<IInterval>, AutoPtr<MutableInt> > > GetForwardLinks();

        AutoPtr< PackedMap<AutoPtr<IInterval>, AutoPtr<MutableInt> > > GetBackwardLinks();

        CARAPI_(void) Include(
            /* [in] */ List< AutoPtr<Arc> >& arcs,
            /* [in] */ Interval* key,
            /* [in] */ MutableInt* size,
            /* [in] */ Boolean ignoreIfAlreadyPresent);

        CARAPI_(void) Include(
            /* [in] */ List< AutoPtr<Arc> >& arcs,
            /* [in] */ Interval* key,
            /* [in] */ MutableInt* size);

        CARAPI_(AutoPtr< ArrayOf<AutoPtr<ArrayOf<GridLayout::Arc*> > > > ) GroupArcsByFirstVertex(
            /* [in] */ AutoPtr< ArrayOf<Arc*> > arcs);

        CARAPI_(AutoPtr< ArrayOf<Arc*> >) TopologicalSort(
            /* [in] */ ArrayOf<Arc*>* arcs);

        CARAPI_(AutoPtr< ArrayOf<Arc*> >) TopologicalSort(
            /* [in] */ List< AutoPtr<Arc> >& arcs);

        CARAPI_(void) AddComponentSizes(
            /* [in] */ List< AutoPtr<Arc> >& result,
            /* [in] */ PackedMap<AutoPtr<IInterval>, AutoPtr<MutableInt> >* links);

        CARAPI_(AutoPtr< ArrayOf<Arc*> >) CreateArcs();

        CARAPI_(void) ComputeArcs();

        CARAPI_(Boolean) Relax(
            /* [in] */ ArrayOf<Int32>* locations,
            /* [in] */ Arc* entry);

        CARAPI_(void) Init(
            /* [in] */ ArrayOf<Int32>* locations);

        CARAPI_(String) ArcsToString(
            /* [in] */ List< AutoPtr<Arc> >& arcs);

        CARAPI_(void) LogError(
            /* [in] */ const String& axisName,
            /* [in] */ ArrayOf<Arc*>* arcs,
            /* [in] */ ArrayOf<Boolean>* culprits0);

        CARAPI_(void) Solve(
            /* [in] */ ArrayOf<Arc*>* arcs,
            /* [in] */ ArrayOf<Int32>* locations);

        CARAPI_(void) ComputeMargins(
            /* [in] */ Boolean leading);

        CARAPI_(void) Solve(
            /* [in] */ ArrayOf<Int32>* a);

        CARAPI_(Boolean) ComputeHasWeights();

        CARAPI_(Boolean) HasWeights();

        CARAPI_(void) RecordOriginalMeasurement(
            /* [in] */ Int32 i);

        CARAPI_(void) ShareOutDelta();

        CARAPI_(void) SolveAndDistributeSpace(
            /* [in] */ ArrayOf<Int32>* a);

        CARAPI_(void) ComputeLocations(
            /* [in] */ ArrayOf<Int32>* a);

        CARAPI_(Int32) Size(
            /* [in] */ ArrayOf<Int32>* locations);

        CARAPI_(void) SetParentConstraints(
            /* [in] */ Int32 min,
            /* [in] */ Int32 max);

        CARAPI_(Int32) GetMeasure(
            /* [in] */ Int32 min,
            /* [in] */ Int32 max);

    public:
        CARAPI_(Int32) GetCount();

        CARAPI_(void) SetCount(
            /* [in] */ Int32 count);

        CARAPI_(Boolean) IsOrderPreserved();

        CARAPI_(void) SetOrderPreserved(
            /* [in] */ Boolean orderPreserved);

        AutoPtr<PackedMap<AutoPtr<IGridLayoutSpec>, AutoPtr<IBounds> > > GetGroupBounds();

        CARAPI_(AutoPtr< ArrayOf<Arc*> >) GetArcs();

        CARAPI_(AutoPtr< ArrayOf<Int32> >) GetLeadingMargins();

        CARAPI_(AutoPtr< ArrayOf<Int32> >) GetTrailingMargins();

        CARAPI_(AutoPtr< ArrayOf<Int32> >) GetLocations();

        CARAPI_(Int32) GetMeasure(
            /* [in] */ Int32 measureSpec);

        CARAPI_(void) Layout(
            /* [in] */ Int32 size);

        CARAPI_(void) InvalidateStructure();

        CARAPI_(void) InvalidateValues();

        CARAPI_(AutoPtr<ArrayOf<Int32> >) GetOriginalMeasurements();

        CARAPI_(AutoPtr<ArrayOf<Int32> >) GetDeltas();

    private:
        static const Int32 NEW = 0;
        static const Int32 PENDING = 1;
        static const Int32 COMPLETE = 2;

        GridLayout* mHost;
        Int32 mMaxIndex; //= UNDEFINED;
        AutoPtr<MutableInt> mParentMin;//= new MutableInt(0);
        AutoPtr<MutableInt> mParentMax;//new MutableInt(-MAX_SIZE);

    public:
        Boolean mHorizontal;
        Int32 mDefinedCount; //= UNDEFINED
        Boolean mGroupBoundsValid; //=FALSE
        AutoPtr<PackedMap<AutoPtr<IGridLayoutSpec>, AutoPtr<IBounds> > > mGroupBounds;
        AutoPtr< PackedMap<AutoPtr<IInterval>, AutoPtr<MutableInt> > > mForwardLinks;
        Boolean mForwardLinksValid; // = FALSE;
        AutoPtr< PackedMap<AutoPtr<IInterval>, AutoPtr<MutableInt> > > mBackwardLinks;
        Boolean mBackwardLinksValid;// = false;
        AutoPtr< ArrayOf<Int32> > mLeadingMargins;
        Boolean mLeadingMarginsValid;// = false;
        AutoPtr< ArrayOf<Int32> > mTrailingMargins;
        Boolean mTrailingMarginsValid;// = false;
        AutoPtr< ArrayOf<Arc*> > mArcs;
        Boolean mArcsValid;// = false;
        AutoPtr< ArrayOf<Int32> > mLocations;
        Boolean mLocationsValid;// = false;
        Boolean mHasWeights;
        Boolean mHasWeightsValid;// = false;
        AutoPtr< ArrayOf<Int32> > mOriginalMeasurements;
        AutoPtr< ArrayOf<Int32> > mDeltas;
        Boolean mOrderPreserved;// = DEFAULT_ORDER_PRESERVED;

    }; //end of Axis

    class Arc
        : public Object
    {
    public:
        AutoPtr<Interval> mSpan;
        AutoPtr<MutableInt> mValue;
        Boolean mValid; // = false;

    public:
        Arc(
            /* [in] */ Interval* span,
            /* [in] */ MutableInt* value);

        CARAPI_(String) ToString();
    };

    class MutableInt
        : public ElRefBase
    {
    public:
        Int32 mValue;

    public:
        MutableInt();

        MutableInt(
            /* [in] */ Int32 value);

        CARAPI_(void) Reset();

        CARAPI_(String) ToString();
    };



    template<typename K, typename V>
    class Assoc
        : public List<Pair<K, V> >
    {

    private:
        Assoc() {
        }

    public:
        static Assoc<K, V>* Of(K keyType, V valueType) {
            return new Assoc<K, V>();
        }

        CARAPI Put(K key, V value) {
            List<Pair<K, V> >::PushBack(Pair<K, V>(key, value));
            return NOERROR;
        }

        AutoPtr< PackedMap<K, V> > Pack() {

            Int32 N = List<Pair <K, V> >::GetSize();
            AutoPtr< ArrayOf<K> > keys = ArrayOf<K>::Alloc(N);
            AutoPtr< ArrayOf<V> > values = ArrayOf<V>::Alloc(N);
            typename List<Pair<K, V> >::Iterator it = List<Pair <K, V> >::Begin();
            Int32 i = 0;
            for(; it != List<Pair <K, V> >::End(); it ++, i++) {
                keys->Set(i, it->mFirst);
                values->Set(i, it->mSecond);
            }
            AutoPtr< PackedMap<K, V> > pm = new PackedMap<K, V>(keys, values);
            return pm;
        }
    };

    class Bounds
        : public Object
        , public IBounds
    {
        friend class Alignment;
        friend class Axis;
    public:
        Int32 mBefore;
        Int32 mAfter;
        Int32 mFlexibility;

    protected:
        virtual CARAPI_(void) Include(
            /* [in] */ Int32 before,
            /* [in] */ Int32 after);

        CARAPI_(void) Include(
            /* [in] */ IGridLayout* gl,
            /* [in] */ IView* c,
            /* [in] */ Spec* spec,
            /* [in] */ Axis* axis,
            /* [in] */ Int32 size);

    public:
        CAR_INTERFACE_DECL()

        Bounds();

        CARAPI_(String) ToString();

        virtual CARAPI_(void) Reset();

        virtual CARAPI_(Int32) Size(
            /* [in] */ Boolean min);

        virtual CARAPI_(Int32) GetOffset(
            /* [in] */ IGridLayout* gl,
            /* [in] */ IView* c,
            /* [in] */ Alignment* a,
            /* [in] */ Int32 size,
            /* [in] */ Boolean horizontal);

        CARAPI GetBefore(
            /* [out] */ Int32* before);

        CARAPI GetAfter(
            /* [out] */ Int32* after);

        CARAPI GetFlexibility(
            /* [out] */ Int32* flexibility);

        CARAPI SetBefore(
            /* [in] */ Int32 before);

        CARAPI SetAfter(
            /* [in] */ Int32 after);

        CARAPI SetFlexibility(
            /* [in] */ Int32 flexibility);

    };

    class Interval
        : public Object
        , public IInterval
    {
    public:
        CAR_INTERFACE_DECL()

        Interval(
            /* [in] */ Int32 min,
            /* [in] */ Int32 max);
        /**
         * The minimum value.
         */
        CARAPI GetMin(
            /* [out] */ Int32* min);

        /**
         * The maximum value.
         */
        CARAPI GetMax(
            /* [out] */ Int32* max);

        CARAPI Size(
            /* [out] */ Int32* size);

        CARAPI Inverse(
            /* [out] */ IInterval** interval);

        CARAPI_(Int32) Size();

        CARAPI_(AutoPtr<Interval>) Inverse();

        CARAPI Equals(
            /* [in] */ IInterface* that,
            /* [out] */ Boolean* result);

        CARAPI GetHashCode(
            /* [out] */ Int32* hashCode);

        CARAPI ToString(
            /* [out] */ String* str);

    public:
        Int32 mMin;
        Int32 mMax;
    };

    class Spec
        : public Object
        , public IGridLayoutSpec
    {
        friend class GridLayout;

    public:
        CAR_INTERFACE_DECL()

        const static AutoPtr<IGridLayoutSpec> UNDEFINED;
        const static Float DEFAULT_WEIGHT = 0;
        Boolean mStartDefined;
        AutoPtr<Interval> mSpan;
        AutoPtr<Alignment> mAlignment;
        Float mWeight;

    private:
        Spec(
            /* [in] */ Boolean startDefined,
            /* [in] */ Interval* span,
            /* [in] */ Alignment* alignment,
            /* [in] */ Float weight);

        Spec(
            /* [in] */ Boolean startDefined,
            /* [in] */ Int32 start,
            /* [in] */ Int32 size,
            /* [in] */ Alignment* alignment,
            /* [in] */ Float weight);

    public:
        CARAPI_(AutoPtr<Spec>) CopyWriteSpan(
            /* [in] */ Interval* span);

        CARAPI_(AutoPtr<Spec>) CopyWriteAlignment(
            /* [in] */ Alignment* alignment);

        CARAPI_(Int32) GetFlexibility();

        CARAPI CopyWriteSpan(
            /* [in] */ IInterval* span,
            /* [out] */ IGridLayoutSpec** spec);

        CARAPI CopyWriteAlignment(
            /* [in] */ IGridLayoutAlignment* alignment,
            /* [out] */ IGridLayoutSpec** spec);

        CARAPI GetFlexibility(
            /* [out] */ Int32* rst);

        CARAPI Equals(
            /* [in] */ IInterface* that,
            /* [out] */ Boolean* res);

        CARAPI GetHashCode(
            /* [out] */ Int32* code);
    };

    class Alignment
        : public Object
        , public IGridLayoutAlignment
    {
    public:
        Alignment();

        CAR_INTERFACE_DECL()

        CARAPI GetSizeInCell(
            /* [in] */ IView* view,
            /* [in] */ Int32 viewSize,
            /* [in] */ Int32 cellSize,
            /* [out] */ Int32* size);

        virtual CARAPI_(AutoPtr<Bounds>) GetBounds();

        virtual CARAPI GetBounds(
            /* [out] */ IBounds** bounds);
    };

    class UndefinedAlignment
        : public Alignment
    {
    public:
        CARAPI GetGravityOffset(
            /* [in] */ IView* view,
            /* [in] */ Int32 cellDelta,
            /* [out] */ Int32* offset);

        CARAPI GetAlignmentValue(
            /* [in] */ IView* view,
            /* [in] */ Int32 viewSize,
            /* [in] */ Int32 mode,
            /* [out] */ Int32* value);
    };

    class LeadingAlignment
        : public Alignment
    {
    public:
        CARAPI GetGravityOffset(
            /* [in] */ IView* view,
            /* [in] */ Int32 cellDelta,
            /* [out] */ Int32* offset);

        CARAPI GetAlignmentValue(
            /* [in] */ IView* view,
            /* [in] */ Int32 viewSize,
            /* [in] */ Int32 mode,
            /* [out] */ Int32* value);
    };

    class TrailingAlignment
        : public Alignment
    {
    public:
        CARAPI GetGravityOffset(
            /* [in] */ IView* view,
            /* [in] */ Int32 cellDelta,
            /* [out] */ Int32* offset);

        CARAPI GetAlignmentValue(
            /* [in] */ IView* view,
            /* [in] */ Int32 viewSize,
            /* [in] */ Int32 mode,
            /* [out] */ Int32* value);
    };

    class SwitchingAlignment
        : public Alignment
    {
    public:
        SwitchingAlignment(
            /* [in] */ Alignment* ltr,
            /* [in] */ Alignment* rtl);

        CARAPI GetGravityOffset(
            /* [in] */ IView* view,
            /* [in] */ Int32 cellDelta,
            /* [out] */ Int32* offset);

        CARAPI GetAlignmentValue(
            /* [in] */ IView* view,
            /* [in] */ Int32 viewSize,
            /* [in] */ Int32 mode,
            /* [out] */ Int32* value);

    private:
        AutoPtr<Alignment> mLtr;
        AutoPtr<Alignment> mRtl;
    };

    class CenterAlignment
        : public Alignment
    {
    public:
        CARAPI GetGravityOffset(
            /* [in] */ IView* view,
            /* [in] */ Int32 cellDelta,
            /* [out] */ Int32* offset);

        CARAPI GetAlignmentValue(
            /* [in] */ IView* view,
            /* [in] */ Int32 viewSize,
            /* [in] */ Int32 mode,
            /* [out] */ Int32* value);
    };

    class BaseLineAlignment
        : public Alignment
    {
    public:
        CARAPI GetGravityOffset(
            /* [in] */ IView* view,
            /* [in] */ Int32 cellDelta,
            /* [out] */ Int32* offset);

        CARAPI GetAlignmentValue(
            /* [in] */ IView* view,
            /* [in] */ Int32 viewSize,
            /* [in] */ Int32 mode,
            /* [out] */ Int32* value);

        CARAPI_(AutoPtr<Bounds>) GetBounds();
    };

    class AlignmentBounds
        : public Bounds
    {
        friend class BaseLineAlignment;

    private:
        Int32 mSize;

    protected:
        CARAPI_(void) Reset();

        CARAPI_(void) Include(
            /* [in] */ Int32 before,
            /* [in] */ Int32 after);

    public:
        CARAPI_(Int32) Size(
            /* [in] */ Boolean min);

        CARAPI_(Int32) GetOffset(
            /* [in] */ IGridLayout* gl,
            /* [in] */ IView* c,
            /* [in] */ Alignment* a,
            /* [in] */ Int32 size,
            /* [in] */ Boolean horizontal);
    };

    class FillAlignment
        : public Alignment
    {
    public:
        CARAPI GetGravityOffset(
            /* [in] */ IView* view,
            /* [in] */ Int32 cellDelta,
            /* [out] */ Int32* offset);

        CARAPI GetAlignmentValue(
            /* [in] */ IView* view,
            /* [in] */ Int32 viewSize,
            /* [in] */ Int32 mode,
            /* [out] */ Int32* value);

        CARAPI GetSizeInCell(
            /* [in] */ IView* view,
            /* [in] */ Int32 viewSize,
            /* [in] */ Int32 cellSize,
            /* [out] */ Int32* Cell);
    };

    class TopoSort
        : public Object
    {
    private:
        AutoPtr< ArrayOf<Arc*> > mResult;
        Int32 mCursor;
        AutoPtr< ArrayOf<AutoPtr<ArrayOf<GridLayout::Arc*> > > > mArcsByVertex;
        AutoPtr< ArrayOf<Int32> > mvisited;
    public:
        TopoSort(
            /* [in] */ ArrayOf<Arc*>* result,
            /* [in] */ Int32 cursor,
            /* [in] */ ArrayOf< AutoPtr< ArrayOf< Arc*> > >* arcsByVertex,
            /* [in] */ ArrayOf<Int32>* visited);

        CARAPI_(void) Walk(
            /* [in] */ Int32 loc);

        CARAPI_(AutoPtr< ArrayOf<Arc*> >) Sort();
    };

public:
    GridLayout();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs = NULL,
        /* [in] */ Int32 defStyle = 0,
        /* [in] */ Int32 defStyleRes = 0);

    CARAPI_(Int32) GetOrientation();

    CARAPI GetOrientation(
        /* [out] */ Int32* result);

    CARAPI SetOrientation(
        /* [in] */ Int32 orientation);

    CARAPI_(Int32) GetRowCount();

    CARAPI GetRowCount(
        /* [out] */ Int32* result);

    CARAPI SetRowCount(
        /* [in] */ Int32 rowCount);

    CARAPI_(Int32) GetColumnCount();

    CARAPI GetColumnCount(
        /* [out] */ Int32* result);

    CARAPI SetColumnCount(
        /* [in] */ Int32 columnCount);

    CARAPI_(Boolean) GetUseDefaultMargins();

    CARAPI GetUseDefaultMargins(
        /* [out] */ Boolean* result);

    CARAPI SetUseDefaultMargins(
        /* [in] */ Boolean useDefaultMargins);

    CARAPI_(Int32) GetAlignmentMode();

    CARAPI GetAlignmentMode(
        /* [out] */ Int32* result);

    CARAPI SetAlignmentMode(
        /* [in] */ Int32 alignmentMode);

    CARAPI_(Boolean) IsRowOrderPreserved();

    CARAPI IsRowOrderPreserved(
        /* [out] */ Boolean* result);

    CARAPI SetRowOrderPreserved(
        /* [in] */ Boolean rowOrderPreserved);

    CARAPI_(Boolean) IsColumnOrderPreserved();

    CARAPI IsColumnOrderPreserved(
        /* [out] */ Boolean* result);

    CARAPI SetColumnOrderPreserved(
        /* [in] */ Boolean columnOrderPreserved);

    static CARAPI_(Int32) Max2(
        /* [in] */ ArrayOf<Int32>* a,
        /* [in] */ Int32 valueIfEmpty);

    template <typename T>
    static CARAPI_(AutoPtr< ArrayOf<T> >) Append(
        /* [in] */ ArrayOf<T>* a,
        /* [in] */ ArrayOf<T>* b)
    {
        AutoPtr< ArrayOf<T> > result = ArrayOf<T>::Alloc(a->GetLength() + b->GetLength());
        for(Int32 i = 0, index = 0; i < result->GetLength(); i++) {
            if (i < a->GetLength()) {
                result->Set(i, (*a)[i]);
            } else {
                result->Set(i, (*b)[index]);
                ++index;
            }
        }
        return result;
    }

    static CARAPI_(AutoPtr<Alignment>) GetAlignment(
        /* [in] */ Int32 gravity,
        /* [in] */ Boolean horizontal);

    CARAPI_(Int32) GetMargin1(
        /* [in] */ IView* view,
        /* [in] */ Boolean horizontal,
        /* [in] */ Boolean leading);

    using View::GetLayoutParams;

    CARAPI_(AutoPtr<IGridLayoutLayoutParams>) GetLayoutParams(
        /* [in] */ IView* c);

    CARAPI GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** params);

    CARAPI_(Int32) GetMeasurementIncludingMargin(
        /* [in] */ IView* c,
        /* [in] */ Boolean horizontal);


    CARAPI GetMeasurementIncludingMargin(
        /* [in] */ IView* c,
        /* [in] */ Boolean horizontal,
        /* [out] */ Int32* result);

    CARAPI RequestLayout();

    CARAPI_(AutoPtr<Alignment>) GetAlignment(
        /* [in] */ Alignment* alignment,
        /* [in] */ Boolean horizontal);

    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    static CARAPI_(AutoPtr<IGridLayoutSpec>) GetSpec(
        /* [in] */ Int32 start,
        /* [in] */ Int32 size,
        /* [in] */ Alignment* alignment);

    static CARAPI_(AutoPtr<IGridLayoutSpec>) GetSpec(
        /* [in] */ Int32 start,
        /* [in] */ Alignment* alignment);

    static CARAPI_(AutoPtr<IGridLayoutSpec>) GetSpec(
        /* [in] */ Int32 start,
        /* [in] */ Int32 size);

    static CARAPI_(AutoPtr<IGridLayoutSpec>) GetSpec(
        /* [in] */ Int32 start);

    static CARAPI_(AutoPtr<IGridLayoutSpec>) GetSpec(
        /* [in] */ Int32 start,
        /* [in] */ Int32 size,
        /* [in] */ Alignment* alignment,
        /* [in] */ Float weight);

    static CARAPI_(AutoPtr<IGridLayoutSpec>) GetSpec(
        /* [in] */ Int32 start,
        /* [in] */ Alignment* alignment,
        /* [in] */ Float weight);

    static CARAPI_(AutoPtr<IGridLayoutSpec>) GetSpec(
        /* [in] */ Int32 start,
        /* [in] */ Int32 size,
        /* [in] */ Float weight);

    static CARAPI_(AutoPtr<IGridLayoutSpec>) GetSpec(
        /* [in] */ Int32 start,
        /* [in] */ Float weight);

    static CARAPI_(AutoPtr<Alignment>) CreateSwitchingAlignment(
        /* [in] */ Alignment* ltr,
        /* [in] */ Alignment* rtl);

protected:
    CARAPI_(void) OnSetLayoutParams(
        /* [in] */ IView* child,
        /* [in] */ IViewGroupLayoutParams* layoutParams);

    CARAPI_(Boolean) CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    CARAPI_(AutoPtr<IViewGroupLayoutParams>) GenerateDefaultLayoutParams();

    CARAPI_(AutoPtr<IViewGroupLayoutParams>) GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    CARAPI_(void) OnDebugDrawMargins(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IPaint* paint);

    CARAPI_(void) OnDebugDraw(
        /* [in] */ ICanvas* canvas);

    CARAPI_(void) OnViewAdded(
        /* [in] */ IView* child);

    CARAPI_(void) OnViewRemoved(
        /* [in] */ IView* child);

    CARAPI_(void) OnChildVisibilityChanged(
        /* [in] */ IView* child,
        /* [in] */ Int32 oldVisibility,
        /* [in] */ Int32 newVisibility);

    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthSpec,
        /* [in] */ Int32 heightSpec);

    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    CARAPI_(Int32) Adjust(
        /* [in] */ Int32 measureSpec,
        /* [in] */ Int32 delta);

private:
    CARAPI_(Int32) GetDefaultMargin(
        /* [in] */ IView* c,
        /* [in] */ Boolean horizontal,
        /* [in] */ Boolean leading);

    CARAPI_(Int32) GetDefaultMargin(
        /* [in] */ IView* c,
        /* [in] */ Boolean isAtEdge,
        /* [in] */ Boolean horizontal,
        /* [in] */ Boolean leading);

    CARAPI_(Int32) GetDefaultMargin(
        /* [in] */ IView* c,
        /* [in] */ IViewGroupLayoutParams* p,
        /* [in] */ Boolean horizontal,
        /* [in] */ Boolean leading);

    CARAPI_(Int32) GetMargin(
        /* [in] */ IView* view,
        /* [in] */ Boolean horizontal,
        /* [in] */ Boolean leading);

    CARAPI_(Int32) GetTotalMargin(
        /* [in] */ IView* child,
        /* [in] */ Boolean horizontal);

    static CARAPI_(Boolean) Fits(
        /* [in] */ ArrayOf<Int32>* a,
        /* [in] */ Int32 value,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    static CARAPI_(void) ProcrusteanFill(
        /* [in] */ ArrayOf<Int32>* a,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 value);

    static CARAPI_(void) SetCellGroup(
        /* [in] */ IViewGroupLayoutParams* lp,
        /* [in] */ Int32 row,
        /* [in] */ Int32 rowSpan,
        /* [in] */ Int32 col,
        /* [in] */ Int32 colSpan);

    static CARAPI_(Int32) Clip(
        /* [in] */ Interval* minorRange,
        /* [in] */ Boolean minorWasDefined,
        /* [in] */ Int32 count);

    CARAPI_(void) ValidateLayoutParams();

    CARAPI_(void) InvalidateStructure();

    CARAPI_(void) InvalidateValues();

    static CARAPI HandleInvalidParams(
        /* [in] */ const String& msg);

    CARAPI_(void) CheckLayoutParams(
        /* [in] */ IGridLayoutLayoutParams* lp,
        /* [in] */ Boolean horizontal);

    CARAPI_(void) DrawLine(
        /* [in] */ ICanvas* graphics,
        /* [in] */ Int32 x1,
        /* [in] */ Int32 y1,
        /* [in] */ Int32 x2,
        /* [in] */ Int32 y2,
        /* [in] */ IPaint* paint);

    CARAPI_(Int32) ComputeLayoutParamsHashCode();

    CARAPI_(void) ConsistencyCheck();

    CARAPI_(void) MeasureChildWithMargins2(
        /* [in] */ IView* child,
        /* [in] */ Int32 parentWidthSpec,
        /* [in] */ Int32 parentHeightSpec,
        /* [in] */ Int32 childWidth,
        /* [in] */ Int32 childHeight);

    CARAPI_(void) MeasureChildrenWithMargins(
        /* [in] */ Int32 widthSpec,
        /* [in] */ Int32 heightSpec,
        /* [in] */ Boolean firstPass);

    CARAPI_(Int32) GetMeasurement(
        /* [in] */ IView* c,
        /* [in] */ Boolean horizontal);

    static CARAPI_(Boolean) CanStretch(
        /* [in] */ Int32 flexibility);

public:
    static const String TAG;

    static const Int32 HORIZONTAL = ILinearLayout::HORIZONTAL;
    static const Int32 VERTICAL = ILinearLayout::VERTICAL;
    static const Int32 UNDEFINED;// = Elastos::Core::Math::INT32_MIN_VALUE;
    static const Int32 ALIGN_BOUNDS = 0;
    static const Int32 ALIGN_MARGINS = 1;
    static const Int32 MAX_SIZE = 100000;
    static const Int32 DEFAULT_CONTAINER_MARGIN = 0;
    static const Int32 UNINITIALIZED_HASH = 0;
private:
    static const Int32 DEFAULT_ORIENTATION = HORIZONTAL;
    static const Int32 DEFAULT_COUNT; //= UNDEFINED;
    static const Boolean DEFAULT_USE_DEFAULT_MARGINS;// = FALSE;
    static const Boolean DEFAULT_ORDER_PRESERVED = TRUE;
    static const Int32 DEFAULT_ALIGNMENT_MODE = ALIGN_MARGINS;

    static const Int32 ORIENTATION = R::styleable::GridLayout_orientation;
    static const Int32 ROW_COUNT = R::styleable::GridLayout_rowCount;
    static const Int32 COLUMN_COUNT = R::styleable::GridLayout_columnCount;
    static const Int32 USE_DEFAULT_MARGINS = R::styleable::GridLayout_useDefaultMargins;
    static const Int32 ALIGNMENT_MODE = R::styleable::GridLayout_alignmentMode;
    static const Int32 ROW_ORDER_PRESERVED = R::styleable::GridLayout_rowOrderPreserved;
    static const Int32 COLUMN_ORDER_PRESERVED = R::styleable::GridLayout_columnOrderPreserved;

    AutoPtr<Axis> mHorizontalAxis;
    AutoPtr<Axis> mVerticalAxis;

    Int32 mOrientation;
    Boolean mUseDefaultMargins;
    Int32 mAlignmentMode;
    Int32 mDefaultGap;
    Int32 mLastLayoutParamsHashCode;

public:
    const static AutoPtr<Alignment> UNDEFINED_ALIGNMENT; // = UndefinedAlignment
    const static AutoPtr<Alignment> LEADING; // = LeadingAlignment
    const static AutoPtr<Alignment> TRAILING; // = TrailingAlignment

    const static AutoPtr<Alignment> TOP; //= LEADING
    const static AutoPtr<Alignment> BOTTOM; //= TRAILING
    const static AutoPtr<Alignment> START; //= LEADING
    const static AutoPtr<Alignment> END; //= TRAILING

    const static AutoPtr<Alignment> LEFT; //= createSwitchingAlignment(START, END);
    const static AutoPtr<Alignment> RIGHT; //= createSwitchingAlignment(END, START);

    const static AutoPtr<Alignment> CENTER; // = CenterAlignment
    const static AutoPtr<Alignment> BASELINE; // = BaseLineAlignment
    const static AutoPtr<Alignment> FILL; // = FillAlignment

private:
    static const Int32 INFLEXIBLE = 0;
    static const Int32 CAN_STRETCH = 2;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Widget::GridLayout::Arc, IInterface)

#endif //__ELASTOS_DROID_WIDGET_GRIDLAYOUT_H__
