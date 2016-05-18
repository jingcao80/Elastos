
#ifndef __ELASTOS_DROID_WIDGET_SIMPLEADAPTER_H__
#define __ELASTOS_DROID_WIDGET_SIMPLEADAPTER_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/BaseAdapter.h"
#include "elastos/droid/widget/Filter.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Widget {

class SimpleAdapter
    : public BaseAdapter
    , public ISimpleAdapter
    , public IFilterable
{
private:
    /**
     * <p>An array filters constrains the content of the array adapter with
     * a prefix. Each item that does not start with the supplied prefix
     * is removed from the list.</p>
     */
    class SimpleFilter
        : public Filter
    {
    public:
        SimpleFilter(
            /* [in] */ SimpleAdapter* host);

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
        SimpleAdapter* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    SimpleAdapter();

    /**
     * Constructor
     *
     * @param context The context where the View associated with this SimpleAdapter is running
     * @param data A List of Maps. Each entry in the List corresponds to one row in the list. The
     *        Maps contain the data for each row, and should include all the entries specified in
     *        "from"
     * @param resource Resource identifier of a view layout that defines the views for this list
     *        item. The layout file should include at least those named views defined in "to"
     * @param from A list of column names that will be added to the Map associated with each
     *        item.
     * @param to The views that should display column in the "from" parameter. These should all be
     *        TextViews. The first N views in this list are given the values of the first N columns
     *        in the from parameter.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IList* data,
        /* [in] */ Int32 resource,
        /* [in] */ ArrayOf<String>* from,
        /* [in] */ ArrayOf<Int32>* to);

    CARAPI GetCount(
        /* [out] */ Int32* count);

    CARAPI GetItem(
        /* [in] */ Int32 position,
        /* [out] */ IInterface** item);

    CARAPI GetItemId(
        /* [in] */ Int32 position,
        /* [out] */ Int64* id);

    virtual CARAPI GetView(
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    virtual CARAPI SetDropDownViewResource(
        /* [in] */ Int32 resource);

    CARAPI GetDropDownView(
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    virtual CARAPI GetViewBinder(
        /* [out] */ ISimpleAdapterViewBinder** viewBinder);

    virtual CARAPI SetViewBinder(
        /* [in] */ ISimpleAdapterViewBinder* viewBinder);

    virtual CARAPI SetViewImage(
        /* [in] */ IImageView* v,
        /* [in] */ Int32 value);

    virtual CARAPI SetViewImage(
        /* [in] */ IImageView* v,
        /* [in] */ const String& value);

    virtual CARAPI SetViewText(
        /* [in] */ ITextView* v,
        /* [in] */ const String& text);

    virtual CARAPI GetFilter(
        /* [out] */ IFilter** filter);

private:
    CARAPI_(AutoPtr<IView>) CreateViewFromResource(
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [in] */ Int32 resource);

    CARAPI BindView(
        /* [in] */ Int32 position,
        /* [in] */ IView* view);

private:
    AutoPtr<ArrayOf<Int32> > mTo;
    AutoPtr<ArrayOf<String> > mFrom;
    AutoPtr<ISimpleAdapterViewBinder> mViewBinder;

    AutoPtr<IList> mData; //List<? extends Map<String, ?>>

    Int32 mResource;
    Int32 mDropDownResource;
    AutoPtr<ILayoutInflater> mInflater;

    AutoPtr<SimpleFilter> mFilter;
    AutoPtr<IArrayList> mUnfilteredData; //ArrayList<Map<String, ?>>
    friend class SimpleFilter;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_SIMPLEADAPTER_H__
