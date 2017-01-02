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

#ifndef __ELASTOS_DROID_WIDGET_ARRAYADAPTER_H__
#define __ELASTOS_DROID_WIDGET_ARRAYADAPTER_H__

#include "elastos/droid/widget/BaseAdapter.h"
#include "elastos/droid/widget/Filter.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::Widget::BaseAdapter;
using Elastos::Droid::Widget::Filter;
using Elastos::Core::IComparator;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Widget {

class ECO_PUBLIC ArrayAdapter
    : public BaseAdapter
    , public IArrayAdapter
    , public IFilterable
{
private:
    /**
     * <p>An array filter constrains the content of the array adapter with
     * a prefix. Each item that does not start with the supplied prefix
     * is removed from the list.</p>
     */
    class ECO_LOCAL ArrayFilter
        : public Filter
    {
    public:
        ArrayFilter(
            /* [in] */ ArrayAdapter* host);

    protected:
        //@Override
        CARAPI PerformFiltering(
            /* [in] */ ICharSequence* prefix,
            /* [out] */ IFilterResults** filterResults);

        //@Override
        CARAPI PublishResults(
            /* [in] */ ICharSequence* constraint,
            /* [in] */ IFilterResults* results);

    private:
        ArrayAdapter* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    ArrayAdapter();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 textViewResourceId);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resource,
        /* [in] */ Int32 textViewResourceId);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 textViewResourceId,
        /* [in] */ ArrayOf<IInterface*>* objects);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resource,
        /* [in] */ Int32 textViewResourceId,
        /* [in] */ ArrayOf<IInterface*>* objects);

    /**
     * Constructor
     *
     * @param context The current context.
     * @param resource The resource ID for a layout file containing a TextView to use when
     *                 instantiating views.
     * @param objects The objects to represent in the ListView.
     */
    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ Int32 resource,
        /* [in] */ IList* objects);

    /**
     * Constructor
     *
     * @param context The current context.
     * @param resource The resource ID for a layout file containing a layout to use when
     *                 instantiating views.
     * @param textViewResourceId The id of the TextView within the layout resource to be populated
     * @param objects The objects to represent in the ListView.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resource,
        /* [in] */ Int32 textViewResourceId,
        /* [in] */ IList* objects);

    virtual ~ArrayAdapter();

    virtual CARAPI Add(
        /* [in] */ IInterface* object);

    virtual CARAPI AddAll(
        /* [in] */ ICollection* collection);

    virtual CARAPI AddAll(
        /* [in] */ ArrayOf<IInterface* >* items) ;

    virtual CARAPI Insert(
        /* [in] */ IInterface* object,
        /* [in] */ Int32 index);

    virtual CARAPI Remove(
        /* [in] */ IInterface* object);

    virtual CARAPI Clear();

    virtual CARAPI Sort(
        /* [in] */ IComparator* comparator);

    virtual CARAPI NotifyDataSetChanged();

    virtual CARAPI SetNotifyOnChange(
        /* [in] */ Boolean notifyOnChange);

    virtual CARAPI GetContext(
        /* [out] */ IContext** context);

    virtual CARAPI GetCount(
        /* [out] */ Int32* count);

    virtual CARAPI GetItem(
        /* [in] */ Int32 position,
        /* [out] */ IInterface** item);

    virtual CARAPI GetPosition(
        /* [in] */ IInterface* item,
        /* [out] */ Int32* position);

    virtual CARAPI GetItemId(
        /* [in] */ Int32 position,
        /* [out] */ Int64* itemId);

    virtual CARAPI GetView(
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    virtual CARAPI SetDropDownViewResource(
        /* [in] */ Int32 resource);

    //@Override
    virtual CARAPI GetDropDownView(
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    /**
     * Creates a new ArrayAdapter from external resources. The content of the array is
     * obtained through {@link android.content.res.Resources#getTextArray(int)}.
     *
     * @param context The application's environment.
     * @param textArrayResId The identifier of the array to use as the data source.
     * @param textViewResId The identifier of the layout used to create views.
     *
     * @return An ArrayAdapter<CharSequence>.
     */
    static CARAPI_(AutoPtr<IArrayAdapter>) CreateFromResource(
        /* [in] */ IContext* context,
        /* [in] */ Int32 textArrayResId,
        /* [in] */ Int32 textViewResId);

    virtual CARAPI GetFilter(
        /* [out] */ IFilter** filter);

protected:
    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resource,
        /* [in] */ Int32 textViewResourceId,
        /* [in] */ IList* objects = NULL);

private:
    ECO_LOCAL CARAPI_(AutoPtr<IView>) CreateViewFromResource(
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [in] */ Int32 resource);

private:
    /**
     * Contains the list of objects that represent the data of this ArrayAdapter.
     * The content of this list is referred to as "the array" in the documentation.
     */
    AutoPtr<IList> mObjects;

    /**
     * Lock used to modify the content of {@link #mObjects}. Any write operation
     * performed on the array should be synchronized on this lock. This lock is also
     * used by the filter (see {@link #getFilter()} to make a synchronized copy of
     * the original array of data.
     */
    Object mLock;

    /**
     * The resource indicating what views to inflate to display the content of this
     * array adapter.
     */
    Int32 mResource;

    /**
     * The resource indicating what views to inflate to display the content of this
     * array adapter in a drop down widget.
     */
    Int32 mDropDownResource;

    /**
     * If the inflated resource is not a TextView, {@link #mFieldId} is used to find
     * a TextView inside the inflated views hierarchy. This field must contain the
     * identifier that matches the one defined in the resource file.
     */
    Int32 mFieldId;

    /**
     * Indicates whether or not {@link #notifyDataSetChanged()} must be called whenever
     * {@link #mObjects} is modified.
     */
    Boolean mNotifyOnChange;

    // context usually holds adapter, we use weak-reference here.
    AutoPtr<IWeakReference> mWeakContext;

    // A copy of the original mObjects array, initialized from and then used instead as soon as
    // the mFilter ArrayFilter is used. mObjects will then only contain the filtered values.
    AutoPtr<IArrayList> mOriginalValues;

    AutoPtr<ArrayFilter> mFilter;

    AutoPtr<ILayoutInflater> mInflater;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos


#endif //__ELASTOS_DROID_WIDGET_ARRAYADAPTER_H__
