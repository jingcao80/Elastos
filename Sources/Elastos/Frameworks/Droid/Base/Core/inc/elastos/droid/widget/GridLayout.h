//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_WIDGET_GRIDLAYOUT_H__
#define __ELASTOS_DROID_WIDGET_GRIDLAYOUT_H__

#define HASH_FOR_WIDGET
#include "elastos/droid/ext/frameworkhash.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/ViewGroup.h"
#include <elastos/droid/R.h>
#include <elastos/core/Math.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/Pair.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewGroupMarginLayoutParams;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::Pair;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Logging::Slogger;

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
    class StaticInitializer;

public:
    /*
    This data structure is used in place of a Map where we have an index that refers to the order
    in which each key/value pairs were added to the map. In this case we store keys and values
    in arrays of a length that is equal to the number of unique keys. We also maintain an
    array of indexes from insertion order to the compacted arrays of keys and values.

    Note that behavior differs from that of a LinkedHashMap in that repeated entries
    *do* get added multiples times. So the length of index is equals to the number of
    items added.

    This is useful in the GridLayout class where we can rely on the order of children not
    changing during layout - to use integer-based lookup for our internal structures
    rather than using (and storing) an implementation of Map<Key, ?>.
     */
    template<typename K, typename V>
    class PackedMap
        : public Object
    {
    public:
        PackedMap(
            /* [in] */ ArrayOf<K>* keys,
            /* [in] */ ArrayOf<V>* values)
        {
            mIndex = CreateIndex(keys);
            mKeys = Compact(keys, mIndex);
            mValues = Compact(values, mIndex);
        }

        CARAPI_(V) GetValue(Int32 i)
        {
            return (*mValues)[(*mIndex)[i]];
        }

    private:
        static AutoPtr< ArrayOf<Int32> > CreateIndex(
            /* [in] */ ArrayOf<K>* keys)
        {
            Int32 size = keys->GetLength();
            AutoPtr< ArrayOf<Int32> > result = ArrayOf<Int32>::Alloc(size);

            HashMap<K, Int32> keyToIndex;
            for(Int32 i = 0; i < size; i ++) {
                K key = (*keys)[i];
                Int32 index;
                typename HashMap<K, Int32>::Iterator it = keyToIndex.Find(key);
                if (it != keyToIndex.End()) {
                    index = it->mSecond;
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

    public:
        AutoPtr< ArrayOf<Int32> > mIndex;
        AutoPtr< ArrayOf<K> > mKeys;
        AutoPtr< ArrayOf<V> > mValues;
    };

    /*
     This internal class houses the algorithm for computing the locations of grid lines;
     along either the horizontal or vertical axis. A GridLayout uses two instances of this class -
     distinguished by the "horizontal" flag which is true for the horizontal axis and false
     for the vertical one.
     */
    class Axis
        : public Object
    {
        friend class GridLayout;
        friend class TopoSort;

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

        CARAPI_(AutoPtr<ArrayOf<Int32> >) GetOriginalMeasurements();

        CARAPI_(AutoPtr<ArrayOf<Int32> >) GetDeltas();

        CARAPI_(AutoPtr< ArrayOf<Int32> >) GetLocations();

        CARAPI_(Int32) GetMeasure(
            /* [in] */ Int32 measureSpec);

        CARAPI_(void) Layout(
            /* [in] */ Int32 size);

        CARAPI_(void) InvalidateStructure();

        CARAPI_(void) InvalidateValues();

    private:
        Axis(
            /* [in] */ Boolean horizontal,
            /* [in] */ GridLayout* host);

        CARAPI_(Int32) CalculateMaxIndex();

        CARAPI_(Int32) GetMaxIndex();

        AutoPtr<PackedMap<AutoPtr<IGridLayoutSpec>, AutoPtr<IBounds> > > CreateGroupBounds();

        CARAPI_(void) ComputeGroupBounds();

        AutoPtr< PackedMap<AutoPtr<IInterval>, AutoPtr<IMutableInt> > > CreateLinks(
            /* [in] */ Boolean min);

        CARAPI_(void) ComputeLinks(
            /* [in] */ PackedMap<AutoPtr<IInterval>, AutoPtr<IMutableInt> >* links,
            /* [in] */ Boolean min);

        AutoPtr< PackedMap<AutoPtr<IInterval>, AutoPtr<IMutableInt> > > GetForwardLinks();

        AutoPtr< PackedMap<AutoPtr<IInterval>, AutoPtr<IMutableInt> > > GetBackwardLinks();

        CARAPI_(void) Include(
            /* [in] */ List< AutoPtr<Arc> >& arcs,
            /* [in] */ Interval* key,
            /* [in] */ MutableInt* size,
            /* [in] */ Boolean ignoreIfAlreadyPresent);

        CARAPI_(void) Include(
            /* [in] */ List< AutoPtr<Arc> >& arcs,
            /* [in] */ Interval* key,
            /* [in] */ MutableInt* size);

        AutoPtr< ArrayOf<AutoPtr<ArrayOf<GridLayout::Arc*> > > > GroupArcsByFirstVertex(
            /* [in] */ AutoPtr< ArrayOf<Arc*> > arcs);

        CARAPI_(AutoPtr< ArrayOf<Arc*> >) TopologicalSort(
            /* [in] */ ArrayOf<Arc*>* arcs);

        CARAPI_(AutoPtr< ArrayOf<Arc*> >) TopologicalSort(
            /* [in] */ List< AutoPtr<Arc> >& arcs);

        CARAPI_(void) AddComponentSizes(
            /* [in] */ List< AutoPtr<Arc> >& result,
            /* [in] */ PackedMap<AutoPtr<IInterval>, AutoPtr<IMutableInt> >* links);

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
        Boolean mHorizontal;

        Int32 mDefinedCount;

        AutoPtr<PackedMap<AutoPtr<IGridLayoutSpec>, AutoPtr<IBounds> > > mGroupBounds;
        Boolean mGroupBoundsValid;

        AutoPtr< PackedMap<AutoPtr<IInterval>, AutoPtr<IMutableInt> > > mForwardLinks;
        Boolean mForwardLinksValid;

        AutoPtr< PackedMap<AutoPtr<IInterval>, AutoPtr<IMutableInt> > > mBackwardLinks;
        Boolean mBackwardLinksValid;

        AutoPtr< ArrayOf<Int32> > mLeadingMargins;
        Boolean mLeadingMarginsValid;

        AutoPtr< ArrayOf<Int32> > mTrailingMargins;
        Boolean mTrailingMarginsValid;

        AutoPtr< ArrayOf<Arc*> > mArcs;
        Boolean mArcsValid;

        AutoPtr< ArrayOf<Int32> > mLocations;
        Boolean mLocationsValid;

        Boolean mHasWeights;
        Boolean mHasWeightsValid;
        AutoPtr< ArrayOf<Int32> > mOriginalMeasurements;
        AutoPtr< ArrayOf<Int32> > mDeltas;

        Boolean mOrderPreserved;

    private:
        static const Int32 NEW;
        static const Int32 PENDING;
        static const Int32 COMPLETE;

        Int32 mMaxIndex;
        AutoPtr<MutableInt> mParentMin;
        AutoPtr<MutableInt> mParentMax;
        GridLayout* mHost;
    }; //end of Axis

    /**
     * Layout information associated with each of the children of a GridLayout.
     * <p>
     * GridLayout supports both row and column spanning and arbitrary forms of alignment within
     * each cell group. The fundamental parameters associated with each cell group are
     * gathered into their vertical and horizontal components and stored
     * in the {@link #rowSpec} and {@link #columnSpec} layout parameters.
     * {@link GridLayout.Spec Specs} are immutable structures
     * and may be shared between the layout parameters of different children.
     * <p>
     * The row and column specs contain the leading and trailing indices along each axis
     * and together specify the four grid indices that delimit the cells of this cell group.
     * <p>
     * The  alignment properties of the row and column specs together specify
     * both aspects of alignment within the cell group. It is also possible to specify a child's
     * alignment within its cell group by using the {@link GridLayout.LayoutParams#setGravity(int)}
     * method.
     * <p>
     * The weight property is also included in Spec and specifies the proportion of any
     * excess space that is due to the associated view.
     *
     * <h4>WRAP_CONTENT and MATCH_PARENT</h4>
     *
     * Because the default values of the {@link #width} and {@link #height}
     * properties are both {@link #WRAP_CONTENT}, this value never needs to be explicitly
     * declared in the layout parameters of GridLayout's children. In addition,
     * GridLayout does not distinguish the special size value {@link #MATCH_PARENT} from
     * {@link #WRAP_CONTENT}. A component's ability to expand to the size of the parent is
     * instead controlled by the principle of <em>flexibility</em>,
     * as discussed in {@link GridLayout}.
     *
     * <h4>Summary</h4>
     *
     * You should not need to use either of the special size values:
     * {@code WRAP_CONTENT} or {@code MATCH_PARENT} when configuring the children of
     * a GridLayout.
     *
     * <h4>Default values</h4>
     *
     * <ul>
     *     <li>{@link #width} = {@link #WRAP_CONTENT}</li>
     *     <li>{@link #height} = {@link #WRAP_CONTENT}</li>
     *     <li>{@link #topMargin} = 0 when
     *          {@link GridLayout#setUseDefaultMargins(boolean) useDefaultMargins} is
     *          {@code false}; otherwise {@link #UNDEFINED}, to
     *          indicate that a default value should be computed on demand. </li>
     *     <li>{@link #leftMargin} = 0 when
     *          {@link GridLayout#setUseDefaultMargins(boolean) useDefaultMargins} is
     *          {@code false}; otherwise {@link #UNDEFINED}, to
     *          indicate that a default value should be computed on demand. </li>
     *     <li>{@link #bottomMargin} = 0 when
     *          {@link GridLayout#setUseDefaultMargins(boolean) useDefaultMargins} is
     *          {@code false}; otherwise {@link #UNDEFINED}, to
     *          indicate that a default value should be computed on demand. </li>
     *     <li>{@link #rightMargin} = 0 when
     *          {@link GridLayout#setUseDefaultMargins(boolean) useDefaultMargins} is
     *          {@code false}; otherwise {@link #UNDEFINED}, to
     *          indicate that a default value should be computed on demand. </li>
     *     <li>{@link #rowSpec}<code>.row</code> = {@link #UNDEFINED} </li>
     *     <li>{@link #rowSpec}<code>.rowSpan</code> = 1 </li>
     *     <li>{@link #rowSpec}<code>.alignment</code> = {@link #BASELINE} </li>
     *     <li>{@link #rowSpec}<code>.weight</code> = 0 </li>
     *     <li>{@link #columnSpec}<code>.column</code> = {@link #UNDEFINED} </li>
     *     <li>{@link #columnSpec}<code>.columnSpan</code> = 1 </li>
     *     <li>{@link #columnSpec}<code>.alignment</code> = {@link #START} </li>
     *     <li>{@link #columnSpec}<code>.weight</code> = 0 </li>
     * </ul>
     *
     * See {@link GridLayout} for a more complete description of the conventions
     * used by GridLayout in the interpretation of the properties of this class.
     *
     * @attr ref android.R.styleable#GridLayout_Layout_layout_row
     * @attr ref android.R.styleable#GridLayout_Layout_layout_rowSpan
     * @attr ref android.R.styleable#GridLayout_Layout_layout_rowWeight
     * @attr ref android.R.styleable#GridLayout_Layout_layout_column
     * @attr ref android.R.styleable#GridLayout_Layout_layout_columnSpan
     * @attr ref android.R.styleable#GridLayout_Layout_layout_columnWeight
     * @attr ref android.R.styleable#GridLayout_Layout_layout_gravity
     */
    class GridLayoutLayoutParams
        : public ViewGroup::MarginLayoutParams
        , public IGridLayoutLayoutParams
    {
    public:
        CAR_INTERFACE_DECL()

        GridLayoutLayoutParams();

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

        CARAPI_(void) SetRowSpecSpan(
            /* [in] */ Interval* span);

        CARAPI_(void) SetColumnSpecSpan(
            /* [in] */ Interval* span);

        CARAPI Equals(
            /* [in] */ IInterface* o,
            /* [out] */ Boolean* result);

        CARAPI GetHashCode(
            /* [in] */ Int32* hash);

        CARAPI SetRowSpec(
            /* [in] */ IGridLayoutSpec* r);

        CARAPI SetColumnSpec(
            /* [in] */ IGridLayoutSpec* c);

        CARAPI GetRowSpec(
            /* [out] */ IGridLayoutSpec** r);

        CARAPI GetColumnSpec(
            /* [out] */ IGridLayoutSpec** c);

    protected:
        /* @Override  */
        CARAPI SetBaseAttributes(
            /* [in] */ ITypedArray* attributes,
            /* [in] */ Int32 widthAttr,
            /* [in] */ Int32 heightAttr);

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

        static CARAPI_(Int32) Init_DEFAULT_SPAN_SIZE(
            /* [in] */ Interval* span);

    public:
        AutoPtr<IGridLayoutSpec> mRowSpec;
        AutoPtr<IGridLayoutSpec> mColumnSpec;

    private:
        // Default values

        static const Int32 DEFAULT_WIDTH;
        static const Int32 DEFAULT_HEIGHT;
        static const Int32 DEFAULT_MARGIN;
        static const Int32 DEFAULT_ROW ;
        static const Int32 DEFAULT_COLUMN;
        static const AutoPtr<GridLayout::Interval> DEFAULT_SPAN;
        static const Int32 DEFAULT_SPAN_SIZE;

        // TypedArray indices

        static const Int32 MARGIN;
        static const Int32 LEFT_MARGIN;
        static const Int32 TOP_MARGIN;
        static const Int32 RIGHT_MARGIN;
        static const Int32 BOTTOM_MARGIN;

        static const Int32 COLUMN;
        static const Int32 COLUMN_SPAN;
        static const Int32 COLUMN_WEIGHT;

        static const Int32 ROW;
        static const Int32 ROW_SPAN;
        static const Int32 ROW_WEIGHT;

        static const Int32 GRAVITY;
    };

    /*
    In place of a HashMap from span to Int, use an array of key/value pairs - stored in Arcs.
    Add the mutables completesCycle flag to avoid creating another hash table for detecting cycles.
     */
    class Arc
        : public Object
    {
    public:
        Arc(
            /* [in] */ Interval* span,
            /* [in] */ MutableInt* value);

        CARAPI ToString(
            /* [out] */ String* info);

    public:
        AutoPtr<Interval> mSpan;
        AutoPtr<MutableInt> mValue;
        Boolean mValid;
    };

    // A mutable Integer - used to avoid heap allocation during the layout operation

    class MutableInt
        : public Object
        , public IMutableInt
    {
    public:
        MutableInt();

        MutableInt(
            /* [in] */ Int32 value);

        CAR_INTERFACE_DECL();

        CARAPI Reset();

        CARAPI ToString(
            /* [out] */ String* info);

    public:
        Int32 mValue;
    };

    template<typename K, typename V>
    class Assoc
        : public List<Pair<K, V> >
    {
    public:
        static AutoPtr< Assoc<K, V> > Of(K keyType, V valueType)
        {
            return new Assoc<K, V>();
        }

        CARAPI_(void) Put(K key, V value)
        {
            List<Pair<K, V> >::PushBack(Pair<K, V>(key, value));
        }

        AutoPtr< PackedMap<K, V> > Pack()
        {
            Int32 N = List<Pair<K, V> >::GetSize();
            AutoPtr< ArrayOf<K> > keys = ArrayOf<K>::Alloc(N);
            AutoPtr< ArrayOf<V> > values = ArrayOf<V>::Alloc(N);
            typename List<Pair<K, V> >::Iterator it;
            Int32 i = 0;
            for(it = List<Pair<K, V> >::Begin(); it != List<Pair<K, V> >::End(); ++it, ++i) {
                keys->Set(i, it->mFirst);
                values->Set(i, it->mSecond);
            }
            AutoPtr< PackedMap<K, V> > pm = new PackedMap<K, V>(keys, values);
            return pm;
        }

    private:
        Assoc() {}
    };

    /*
    For each group (with a given alignment) we need to store the amount of space required
    before the alignment point and the amount of space required after it. One side of this
    calculation is always 0 for START and END alignments but we don't make use of this.
    For CENTER and BASELINE alignments both sides are needed and in the BASELINE case no
    simple optimisations are possible.

    The general algorithm therefore is to create a Map (actually a PackedMap) from
    group to Bounds and to loop through all Views in the group taking the maximum
    of the values for each View.
    */
    class Bounds
        : public Object
        , public IBounds
    {
        friend class Axis;
        friend class Alignment;
        friend class GridLayout;

    public:
        CAR_INTERFACE_DECL()

        CARAPI ToString(
            /* [out] */ String* info);

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

    protected:
        Bounds();

        virtual CARAPI_(void) Reset();

        virtual CARAPI_(void) Include(
            /* [in] */ Int32 before,
            /* [in] */ Int32 after);

        virtual CARAPI_(Int32) Size(
            /* [in] */ Boolean min);

        virtual CARAPI_(Int32) GetOffset(
            /* [in] */ IGridLayout* gl,
            /* [in] */ IView* c,
            /* [in] */ Alignment* a,
            /* [in] */ Int32 size,
            /* [in] */ Boolean horizontal);

        CARAPI_(void) Include(
            /* [in] */ IGridLayout* gl,
            /* [in] */ IView* c,
            /* [in] */ Spec* spec,
            /* [in] */ Axis* axis,
            /* [in] */ Int32 size);

    public:
        Int32 mBefore;
        Int32 mAfter;
        Int32 mFlexibility; // we're flexible iff all included specs are flexible
    };

    /**
     * An Interval represents a contiguous range of values that lie between
     * the interval's {@link #min} and {@link #max} values.
     * <p>
     * Intervals are immutable so may be passed as values and used as keys in hash tables.
     * It is not necessary to have multiple instances of Intervals which have the same
     * {@link #min} and {@link #max} values.
     * <p>
     * Intervals are often written as {@code [min, max]} and represent the set of values
     * {@code x} such that {@code min <= x < max}.
     */
    class Interval
        : public Object
        , public IInterval
    {
    public:
        CAR_INTERFACE_DECL()

        /**
         * Construct a new Interval, {@code interval}, where:
         * <ul>
         *     <li> {@code interval.min = min} </li>
         *     <li> {@code interval.max = max} </li>
         * </ul>
         *
         * @param min the minimum value.
         * @param max the maximum value.
         */
        Interval(
            /* [in] */ Int32 min,
            /* [in] */ Int32 max);

        CARAPI Size(
            /* [out] */ Int32* size);

        CARAPI Inverse(
            /* [out] */ IInterval** interval);

        CARAPI Equals(
            /* [in] */ IInterface* that,
            /* [out] */ Boolean* result);

        CARAPI GetHashCode(
            /* [out] */ Int32* hashCode);

        CARAPI ToString(
            /* [out] */ String* str);

        CARAPI GetMin(
            /* [out] */ Int32* min);

        CARAPI GetMax(
            /* [out] */ Int32* max);

    public:
        /**
         * The minimum value.
         */
        Int32 mMin;

        /**
         * The maximum value.
         */
        Int32 mMax;
    };

    /**
     * A Spec defines the horizontal or vertical characteristics of a group of
     * cells. Each spec. defines the <em>grid indices</em> and <em>alignment</em>
     * along the appropriate axis.
     * <p>
     * The <em>grid indices</em> are the leading and trailing edges of this cell group.
     * See {@link GridLayout} for a description of the conventions used by GridLayout
     * for grid indices.
     * <p>
     * The <em>alignment</em> property specifies how cells should be aligned in this group.
     * For row groups, this specifies the vertical alignment.
     * For column groups, this specifies the horizontal alignment.
     * <p>
     * Use the following static const methods to create specs:
     * <ul>
     *   <li>{@link #spec(int)}</li>
     *   <li>{@link #spec(int, int)}</li>
     *   <li>{@link #spec(int, Alignment)}</li>
     *   <li>{@link #spec(int, int, Alignment)}</li>
     *   <li>{@link #spec(int, float)}</li>
     *   <li>{@link #spec(int, int, float)}</li>
     *   <li>{@link #spec(int, Alignment, float)}</li>
     *   <li>{@link #spec(int, int, Alignment, float)}</li>
     * </ul>
     *
     */
    class Spec
        : public Object
        , public IGridLayoutSpec
    {
        friend class GridLayout;

    public:
        CAR_INTERFACE_DECL()

        CARAPI CopyWriteSpan(
            /* [in] */ IInterval* span,
            /* [out] */ IGridLayoutSpec** spec);

        CARAPI CopyWriteAlignment(
            /* [in] */ IGridLayoutAlignment* alignment,
            /* [out] */ IGridLayoutSpec** spec);

        CARAPI GetFlexibility(
            /* [out] */ Int32* rst);

        /**
         * Returns {@code true} if the {@code class}, {@code alignment} and {@code span}
         * properties of this Spec and the supplied parameter are pairwise equal,
         * {@code false} otherwise.
         *
         * @param that the object to compare this spec with
         *
         * @return {@code true} if the specified object is equal to this
         *         {@code Spec}; {@code false} otherwise
         */
        CARAPI Equals(
            /* [in] */ IInterface* that,
            /* [out] */ Boolean* res);

        CARAPI GetHashCode(
            /* [out] */ Int32* code);

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
        static const AutoPtr<IGridLayoutSpec> UNDEFINED;
        static const Float DEFAULT_WEIGHT;

        Boolean mStartDefined;
        AutoPtr<Interval> mSpan;
        AutoPtr<Alignment> mAlignment;
        Float mWeight;
    };

    /**
     * Alignments specify where a view should be placed within a cell group and
     * what size it should be.
     * <p>
     * The {@link LayoutParams} class contains a {@link LayoutParams#rowSpec rowSpec}
     * and a {@link LayoutParams#columnSpec columnSpec} each of which contains an
     * {@code alignment}. Overall placement of the view in the cell
     * group is specified by the two alignments which act along each axis independently.
     * <p>
     *  The GridLayout class defines the most common alignments used in general layout:
     * {@link #TOP}, {@link #LEFT}, {@link #BOTTOM}, {@link #RIGHT}, {@link #START},
     * {@link #END}, {@link #CENTER}, {@link #BASELINE} and {@link #FILL}.
     */
    /*
     * An Alignment implementation must define {@link #getAlignmentValue(View, int, int)},
     * to return the appropriate value for the type of alignment being defined.
     * The enclosing algorithms position the children
     * so that the locations defined by the alignment values
     * are the same for all of the views in a group.
     * <p>
     */
    class Alignment
        : public Object
        , public IGridLayoutAlignment
    {
    public:
        Alignment();

        CAR_INTERFACE_DECL()

        /**
         * Returns the size of the view specified by this alignment.
         * In the case of vertical alignments this method should return a height; for
         * horizontal alignments this method should return the width.
         * <p>
         * The default implementation returns {@code viewSize}.
         *
         * @param view              the view to which this alignment should be applied
         * @param viewSize          the measured size of the view
         * @param cellSize          the size of the cell into which this view will be placed
         * @return the aligned size
         */
        CARAPI GetSizeInCell(
            /* [in] */ IView* view,
            /* [in] */ Int32 viewSize,
            /* [in] */ Int32 cellSize,
            /* [out] */ Int32* size);

        CARAPI GetBounds(
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

    /**
     * Indicates that a view should be aligned with the <em>start</em>
     * edges of the other views in its cell group.
     */
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

    /**
     * Indicates that a view should be aligned with the <em>end</em>
     * edges of the other views in its cell group.
     */
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

    /**
     * Indicates that a view should be <em>centered</em> with the other views in its cell group.
     * This constant may be used in both {@link LayoutParams#rowSpec rowSpecs} and {@link
     * LayoutParams#columnSpec columnSpecs}.
     */
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

    /**
     * Indicates that a view should be aligned with the <em>baselines</em>
     * of the other views in its cell group.
     * This constant may only be used as an alignment in {@link LayoutParams#rowSpec rowSpecs}.
     *
     * @see View#getBaseline()
     */
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

        CARAPI GetBounds(
            /* [out] */ IBounds** bounds);
    };

    class AlignmentBounds
        : public Bounds
    {
        friend class BaseLineAlignment;

    public:
        AlignmentBounds();

        /* @Override */
        CARAPI_(Int32) Size(
            /* [in] */ Boolean min);

        /* @Override */
        CARAPI_(Int32) GetOffset(
            /* [in] */ IGridLayout* gl,
            /* [in] */ IView* c,
            /* [in] */ Alignment* a,
            /* [in] */ Int32 size,
            /* [in] */ Boolean horizontal);

    protected:
        /* @Override */
        CARAPI_(void) Reset();

        /* @Override */
        CARAPI_(void) Include(
            /* [in] */ Int32 before,
            /* [in] */ Int32 after);

    private:
        Int32 mSize;
    };

    /**
     * Indicates that a view should expanded to fit the boundaries of its cell group.
     * This constant may be used in both {@link LayoutParams#rowSpec rowSpecs} and
     * {@link LayoutParams#columnSpec columnSpecs}.
     */
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
    public:
        TopoSort(
            /* [in] */ ArrayOf<Arc*>* arcs,
            /* [in] */ Axis* axis);

        CARAPI_(void) Walk(
            /* [in] */ Int32 loc);

        CARAPI_(AutoPtr< ArrayOf<Arc*> >) Sort();

    private:
        AutoPtr< ArrayOf<Arc*> > mResult;
        Int32 mCursor;
        AutoPtr< ArrayOf<AutoPtr<ArrayOf<GridLayout::Arc*> > > > mArcsByVertex;
        AutoPtr< ArrayOf<Int32> > mVisited;
    };

public:
    GridLayout();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle,
        /* [in] */ Int32 defStyleRes);

    /**
     * Returns the current orientation.
     *
     * @return either {@link #HORIZONTAL} or {@link #VERTICAL}
     *
     * @see #setOrientation(int)
     *
     * @attr ref android.R.styleable#GridLayout_orientation
     */
    CARAPI GetOrientation(
        /* [out] */ Int32* orientation);

    /**
     *
     * GridLayout uses the orientation property for two purposes:
     * <ul>
     *  <li>
     *      To control the 'direction' in which default row/column indices are generated
     *      when they are not specified in a component's layout parameters.
     *  </li>
     *  <li>
     *      To control which axis should be processed first during the layout operation:
     *      when orientation is {@link #HORIZONTAL} the horizontal axis is laid out first.
     *  </li>
     * </ul>
     *
     * The order in which axes are laid out is important if, for example, the height of
     * one of GridLayout's children is dependent on its width - and its width is, in turn,
     * dependent on the widths of other components.
     * <p>
     * If your layout contains a {@link TextView} (or derivative:
     * {@code Button}, {@code EditText}, {@code CheckBox}, etc.) which is
     * in multi-line mode (the default) it is normally best to leave GridLayout's
     * orientation as {@code HORIZONTAL} - because {@code TextView} is capable of
     * deriving its height for a given width, but not the other way around.
     * <p>
     * Other than the effects above, orientation does not affect the actual layout operation of
     * GridLayout, so it's fine to leave GridLayout in {@code HORIZONTAL} mode even if
     * the height of the intended layout greatly exceeds its width.
     * <p>
     * The default value of this property is {@link #HORIZONTAL}.
     *
     * @param orientation either {@link #HORIZONTAL} or {@link #VERTICAL}
     *
     * @see #getOrientation()
     *
     * @attr ref android.R.styleable#GridLayout_orientation
     */
    CARAPI SetOrientation(
        /* [in] */ Int32 orientation);

    /**
     * Returns the current number of rows. This is either the last value that was set
     * with {@link #setRowCount(int)} or, if no such value was set, the maximum
     * value of each the upper bounds defined in {@link LayoutParams#rowSpec}.
     *
     * @return the current number of rows
     *
     * @see #setRowCount(int)
     * @see LayoutParams#rowSpec
     *
     * @attr ref android.R.styleable#GridLayout_rowCount
     */
    CARAPI GetRowCount(
        /* [out] */ Int32* rowCount);

    /**
     * RowCount is used only to generate default row/column indices when
     * they are not specified by a component's layout parameters.
     *
     * @param rowCount the number of rows
     *
     * @see #getRowCount()
     * @see LayoutParams#rowSpec
     *
     * @attr ref android.R.styleable#GridLayout_rowCount
     */
    CARAPI SetRowCount(
        /* [in] */ Int32 rowCount);

    /**
     * Returns the current number of columns. This is either the last value that was set
     * with {@link #setColumnCount(int)} or, if no such value was set, the maximum
     * value of each the upper bounds defined in {@link LayoutParams#columnSpec}.
     *
     * @return the current number of columns
     *
     * @see #setColumnCount(int)
     * @see LayoutParams#columnSpec
     *
     * @attr ref android.R.styleable#GridLayout_columnCount
     */
    CARAPI GetColumnCount(
        /* [out] */ Int32* columnCount);

    /**
     * ColumnCount is used only to generate default column/column indices when
     * they are not specified by a component's layout parameters.
     *
     * @param columnCount the number of columns.
     *
     * @see #getColumnCount()
     * @see LayoutParams#columnSpec
     *
     * @attr ref android.R.styleable#GridLayout_columnCount
     */
    CARAPI SetColumnCount(
        /* [in] */ Int32 columnCount);

    /**
     * Returns whether or not this GridLayout will allocate default margins when no
     * corresponding layout parameters are defined.
     *
     * @return {@code true} if default margins should be allocated
     *
     * @see #setUseDefaultMargins(boolean)
     *
     * @attr ref android.R.styleable#GridLayout_useDefaultMargins
     */
    CARAPI GetUseDefaultMargins(
        /* [out] */ Boolean* useDefaultMargins);

    /**
     * When {@code true}, GridLayout allocates default margins around children
     * based on the child's visual characteristics. Each of the
     * margins so defined may be independently overridden by an assignment
     * to the appropriate layout parameter.
     * <p>
     * When {@code false}, the default value of all margins is zero.
     * <p>
     * When setting to {@code true}, consider setting the value of the
     * {@link #setAlignmentMode(int) alignmentMode}
     * property to {@link #ALIGN_BOUNDS}.
     * <p>
     * The default value of this property is {@code false}.
     *
     * @param useDefaultMargins use {@code true} to make GridLayout allocate default margins
     *
     * @see #getUseDefaultMargins()
     * @see #setAlignmentMode(int)
     *
     * @see MarginLayoutParams#leftMargin
     * @see MarginLayoutParams#topMargin
     * @see MarginLayoutParams#rightMargin
     * @see MarginLayoutParams#bottomMargin
     *
     * @attr ref android.R.styleable#GridLayout_useDefaultMargins
     */
    CARAPI SetUseDefaultMargins(
        /* [in] */ Boolean useDefaultMargins);

    /**
     * Returns the alignment mode.
     *
     * @return the alignment mode; either {@link #ALIGN_BOUNDS} or {@link #ALIGN_MARGINS}
     *
     * @see #ALIGN_BOUNDS
     * @see #ALIGN_MARGINS
     *
     * @see #setAlignmentMode(int)
     *
     * @attr ref android.R.styleable#GridLayout_alignmentMode
     */
    CARAPI GetAlignmentMode(
        /* [out] */ Int32* alignmentMode);

    /**
     * Sets the alignment mode to be used for all of the alignments between the
     * children of this container.
     * <p>
     * The default value of this property is {@link #ALIGN_MARGINS}.
     *
     * @param alignmentMode either {@link #ALIGN_BOUNDS} or {@link #ALIGN_MARGINS}
     *
     * @see #ALIGN_BOUNDS
     * @see #ALIGN_MARGINS
     *
     * @see #getAlignmentMode()
     *
     * @attr ref android.R.styleable#GridLayout_alignmentMode
     */
    CARAPI SetAlignmentMode(
        /* [in] */ Int32 alignmentMode);

    /**
     * Returns whether or not row boundaries are ordered by their grid indices.
     *
     * @return {@code true} if row boundaries must appear in the order of their indices,
     *         {@code false} otherwise
     *
     * @see #setRowOrderPreserved(boolean)
     *
     * @attr ref android.R.styleable#GridLayout_rowOrderPreserved
     */
    CARAPI IsRowOrderPreserved(
        /* [out] */ Boolean* rowOrderPreserved);

    /**
     * When this property is {@code true}, GridLayout is forced to place the row boundaries
     * so that their associated grid indices are in ascending order in the view.
     * <p>
     * When this property is {@code false} GridLayout is at liberty to place the vertical row
     * boundaries in whatever order best fits the given constraints.
     * <p>
     * The default value of this property is {@code true}.

     * @param rowOrderPreserved {@code true} to force GridLayout to respect the order
     *        of row boundaries
     *
     * @see #isRowOrderPreserved()
     *
     * @attr ref android.R.styleable#GridLayout_rowOrderPreserved
     */
    CARAPI SetRowOrderPreserved(
        /* [in] */ Boolean rowOrderPreserved);

    /**
     * Returns whether or not column boundaries are ordered by their grid indices.
     *
     * @return {@code true} if column boundaries must appear in the order of their indices,
     *         {@code false} otherwise
     *
     * @see #setColumnOrderPreserved(boolean)
     *
     * @attr ref android.R.styleable#GridLayout_columnOrderPreserved
     */
    CARAPI IsColumnOrderPreserved(
        /* [out] */ Boolean* result);

    /**
     * When this property is {@code true}, GridLayout is forced to place the column boundaries
     * so that their associated grid indices are in ascending order in the view.
     * <p>
     * When this property is {@code false} GridLayout is at liberty to place the horizontal column
     * boundaries in whatever order best fits the given constraints.
     * <p>
     * The default value of this property is {@code true}.
     *
     * @param columnOrderPreserved use {@code true} to force GridLayout to respect the order
     *        of column boundaries.
     *
     * @see #isColumnOrderPreserved()
     *
     * @attr ref android.R.styleable#GridLayout_columnOrderPreserved
     */
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
        Int32 NA = a->GetLength();
        Int32 NB = b->GetLength();
        AutoPtr< ArrayOf<T> > result = ArrayOf<T>::Alloc(NA + NB);
        for (Int32 i = 0; i < NA; i++) {
            result->Set(i, (*a)[i]);
        }
        for (Int32 i = 0; i < NB; i++) {
            result->Set(NA + i, (*b)[i]);
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

    using ViewGroup::GetLayoutParams;

    CARAPI_(AutoPtr<IGridLayoutLayoutParams>) GetLayoutParams(
        /* [in] */ IView* c);

    CARAPI GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** params);

    static CARAPI_(Int32) Adjust(
        /* [in] */ Int32 measureSpec,
        /* [in] */ Int32 delta);

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

    static CARAPI_(Boolean) CanStretch(
        /* [in] */ Int32 flexibility);

protected:
    /* @Override */
    CARAPI_(void) OnSetLayoutParams(
        /* [in] */ IView* child,
        /* [in] */ IViewGroupLayoutParams* layoutParams);

    /* @Override */
    CARAPI_(Boolean) CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    /* @Override */
    CARAPI GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** params);

    /* @Override */
    CARAPI_(AutoPtr<IViewGroupLayoutParams>) GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    /* @Override */
    CARAPI_(void) OnDebugDrawMargins(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IPaint* paint);

    /* @Override */
    CARAPI_(void) OnDebugDraw(
        /* [in] */ ICanvas* canvas);

    /* @Override */
    CARAPI_(void) OnViewAdded(
        /* [in] */ IView* child);

    /* @Override */
    CARAPI_(void) OnViewRemoved(
        /* [in] */ IView* child);

    /* @Override */
    CARAPI_(void) OnChildVisibilityChanged(
        /* [in] */ IView* child,
        /* [in] */ Int32 oldVisibility,
        /* [in] */ Int32 newVisibility);

    /* @Override */
    CARAPI OnMeasure(
        /* [in] */ Int32 widthSpec,
        /* [in] */ Int32 heightSpec);

    /* @Override */
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

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
        /* [in] */ IGridLayoutLayoutParams* p,
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
        /* [in] */ IGridLayoutLayoutParams* lp,
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

    CARAPI CheckLayoutParams(
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

public:
    /**
     * The horizontal orientation.
     */
    static const Int32 HORIZONTAL;

    /**
     * The vertical orientation.
     */
    static const Int32 VERTICAL;

    /**
     * The constant used to indicate that a value is undefined.
     * Fields can use this value to indicate that their values
     * have not yet been set. Similarly, methods can return this value
     * to indicate that there is no suitable value that the implementation
     * can return.
     * The value used for the constant (currently {@link Integer#MIN_VALUE}) is
     * intended to avoid confusion between valid values whose sign may not be known.
     */
    static const Int32 UNDEFINED;

    /**
     * This constant is an {@link #setAlignmentMode(int) alignmentMode}.
     * When the {@code alignmentMode} is set to {@link #ALIGN_BOUNDS}, alignment
     * is made between the edges of each component's raw
     * view boundary: i.e. the area delimited by the component's:
     * {@link android.view.View#getTop() top},
     * {@link android.view.View#getLeft() left},
     * {@link android.view.View#getBottom() bottom} and
     * {@link android.view.View#getRight() right} properties.
     * <p>
     * For example, when {@code GridLayout} is in {@link #ALIGN_BOUNDS} mode,
     * children that belong to a row group that uses {@link #TOP} alignment will
     * all return the same value when their {@link android.view.View#getTop()}
     * method is called.
     *
     * @see #setAlignmentMode(int)
     */
    static const Int32 ALIGN_BOUNDS;

    /**
     * This constant is an {@link #setAlignmentMode(int) alignmentMode}.
     * When the {@code alignmentMode} is set to {@link #ALIGN_MARGINS},
     * the bounds of each view are extended outwards, according
     * to their margins, before the edges of the resulting rectangle are aligned.
     * <p>
     * For example, when {@code GridLayout} is in {@link #ALIGN_MARGINS} mode,
     * the quantity {@code top - layoutParams.topMargin} is the same for all children that
     * belong to a row group that uses {@link #TOP} alignment.
     *
     * @see #setAlignmentMode(int)
     */
    static const Int32 ALIGN_MARGINS;

    // Misc constants

    static const Int32 MAX_SIZE;
    static const Int32 DEFAULT_CONTAINER_MARGIN;
    static const Int32 UNINITIALIZED_HASH;

    static const AutoPtr<Alignment> UNDEFINED_ALIGNMENT;

    /**
     * Indicates that a view should be aligned with the <em>start</em>
     * edges of the other views in its cell group.
     */
    static const AutoPtr<Alignment> LEADING;

    /**
     * Indicates that a view should be aligned with the <em>end</em>
     * edges of the other views in its cell group.
     */
    static const AutoPtr<Alignment> TRAILING;

    /**
     * Indicates that a view should be aligned with the <em>top</em>
     * edges of the other views in its cell group.
     */
    static const AutoPtr<Alignment> TOP;

    /**
     * Indicates that a view should be aligned with the <em>bottom</em>
     * edges of the other views in its cell group.
     */
    static const AutoPtr<Alignment> BOTTOM;

    /**
     * Indicates that a view should be aligned with the <em>start</em>
     * edges of the other views in its cell group.
     */
    static const AutoPtr<Alignment> START;

    /**
     * Indicates that a view should be aligned with the <em>end</em>
     * edges of the other views in its cell group.
     */
    static const AutoPtr<Alignment> END;

    /**
     * Indicates that a view should be aligned with the <em>left</em>
     * edges of the other views in its cell group.
     */
    static const AutoPtr<Alignment> LEFT;

    /**
     * Indicates that a view should be aligned with the <em>right</em>
     * edges of the other views in its cell group.
     */
    static const AutoPtr<Alignment> RIGHT;

    /**
     * Indicates that a view should be <em>centered</em> with the other views in its cell group.
     * This constant may be used in both {@link LayoutParams#rowSpec rowSpecs} and {@link
     * LayoutParams#columnSpec columnSpecs}.
     */
    static const AutoPtr<Alignment> CENTER;

    /**
     * Indicates that a view should be aligned with the <em>baselines</em>
     * of the other views in its cell group.
     * This constant may only be used as an alignment in {@link LayoutParams#rowSpec rowSpecs}.
     *
     * @see View#getBaseline()
     */
    static const AutoPtr<Alignment> BASELINE;

    /**
     * Indicates that a view should expanded to fit the boundaries of its cell group.
     * This constant may be used in both {@link LayoutParams#rowSpec rowSpecs} and
     * {@link LayoutParams#columnSpec columnSpecs}.
     */
    static const AutoPtr<Alignment> FILL;

private:
    static const String TAG;

    // Defaults

    static const Int32 DEFAULT_ORIENTATION;
    static const Int32 DEFAULT_COUNT;
    static const Boolean DEFAULT_USE_DEFAULT_MARGINS;
    static const Boolean DEFAULT_ORDER_PRESERVED;
    static const Int32 DEFAULT_ALIGNMENT_MODE;

    // TypedArray indices

    static const Int32 ORIENTATION;
    static const Int32 ROW_COUNT;
    static const Int32 COLUMN_COUNT;
    static const Int32 USE_DEFAULT_MARGINS;
    static const Int32 ALIGNMENT_MODE;
    static const Int32 ROW_ORDER_PRESERVED;
    static const Int32 COLUMN_ORDER_PRESERVED;

    AutoPtr<Axis> mHorizontalAxis;
    AutoPtr<Axis> mVerticalAxis;
    Int32 mOrientation;
    Boolean mUseDefaultMargins;
    Int32 mAlignmentMode;
    Int32 mDefaultGap;
    Int32 mLastLayoutParamsHashCode;

    static const Int32 INFLEXIBLE;
    static const Int32 CAN_STRETCH;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Widget::GridLayout::Arc, IInterface)

#endif //__ELASTOS_DROID_WIDGET_GRIDLAYOUT_H__
