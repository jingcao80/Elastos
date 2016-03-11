
#include "Elastos.Droid.Content.h"
#include "elastos/droid/widget/SimpleAdapter.h"
#include "elastos/droid/net/Uri.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Uri;
using Elastos::Core::StringUtils;
using Elastos::Core::IBoolean;
using Elastos::Core::IInteger32;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IMap;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Widget {

//==============================================================================
//                      SimpleAdapter::SimpleFilter
//==============================================================================
SimpleAdapter::SimpleFilter::SimpleFilter(
    /* [in] */ SimpleAdapter* host)
    : mHost(host)
{}

ECode SimpleAdapter::SimpleFilter::PerformFiltering(
    /* [in] */ ICharSequence* prefix,
    /* [out] */ IFilterResults** filterResults)
{
    VALIDATE_NOT_NULL(filterResults);
    AutoPtr<FilterResults> results = new FilterResults();

    if (mHost->mUnfilteredData == NULL) {
        CArrayList::New(ICollection::Probe(mHost->mData), (IArrayList**)&mHost->mUnfilteredData);
    }

    Int32 length = 0;
    if (prefix == NULL || (prefix->GetLength(&length), length) == 0) {
        results->mValues = mHost->mUnfilteredData;
        mHost->mUnfilteredData->GetSize(&results->mCount);
    }
    else {
        String prefixString;
        prefix->ToString(&prefixString);
        prefixString = prefixString.ToLowerCase();

        AutoPtr<IArrayList> unfilteredValues = mHost->mUnfilteredData;
        Int32 count = 0;
        unfilteredValues->GetSize(&count);

        AutoPtr<IArrayList> newValues;
        CArrayList::New(count, (IArrayList**)&newValues);

        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IMap> h;
            unfilteredValues->Get(i, (IInterface**)&h);
            if (h != NULL) {
                Int32 len = mHost->mTo->GetLength();

                for (Int32 j = 0; j < len; j++) {
                    AutoPtr<ICharSequence> cs;
                    AutoPtr<ICharSequence> key;
                    CString::New((*mHost->mFrom)[j], (ICharSequence**)&key);
                    h->Get(key, (IInterface**)&cs);
                    String str;
                    cs->ToString(&str);

                    AutoPtr<ArrayOf<String> > words;
                    StringUtils::Split(str, " ", (ArrayOf<String>**)&words);
                    Int32 wordCount = words->GetLength();

                    for (Int32 k = 0; k < wordCount; k++) {
                        String word = (*words)[k];

                        if (word.ToLowerCase().StartWith(prefixString)) {
                            newValues->Add(h);
                            break;
                        }
                    }
                }
            }
        }

        results->mValues = newValues;
        newValues->GetSize(&results->mCount);
    }

    *filterResults = (IFilterResults*)results.Get();
    REFCOUNT_ADD(*filterResults);

    return NOERROR;
}

ECode SimpleAdapter::SimpleFilter::PublishResults(
    /* [in] */ ICharSequence* constraint,
    /* [in] */ IFilterResults* results)
{
    //noinspection unchecked
    mHost->mData = IList::Probe(((FilterResults*)results)->mValues);
    if (((FilterResults*)results)->mCount > 0) {
        mHost->NotifyDataSetChanged();
    } else {
        mHost->NotifyDataSetInvalidated();
    }

    return NOERROR;
}

//==============================================================================
//                      SimpleAdapter::SimpleAdapter
//==============================================================================
CAR_INTERFACE_IMPL_2(SimpleAdapter, BaseAdapter, ISimpleAdapter, IFilterable);
SimpleAdapter::SimpleAdapter()
    : mResource(0)
    , mDropDownResource(0)
{}

ECode SimpleAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IList* data,
    /* [in] */ Int32 resource,
    /* [in] */ ArrayOf<String>* from,
    /* [in] */ ArrayOf<Int32>* to)
{
    mData = data;
    mResource = mDropDownResource = resource;
    mFrom = from;
    mTo = to;
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    mInflater = ILayoutInflater::Probe(obj);
    return NOERROR;
}

ECode SimpleAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    return mData->GetSize(count);
}

ECode SimpleAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item);
    return mData->Get(position, item);
}

ECode SimpleAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    *id = position;
    return NOERROR;
}

ECode SimpleAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IView> v = CreateViewFromResource(position, convertView, parent, mResource);
    *view = v;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

AutoPtr<IView> SimpleAdapter::CreateViewFromResource(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [in] */ Int32 resource)
{
    AutoPtr<IView> view;

    if (convertView == NULL) {
        mInflater->Inflate(resource, parent, FALSE, (IView**)&view);
    }
    else {
        view = convertView;
    }

    BindView(position, view);
    return view;
}

ECode SimpleAdapter::SetDropDownViewResource(
    /* [in] */ Int32 resource)
{
    mDropDownResource = resource;
    return NOERROR;
}

ECode SimpleAdapter::GetDropDownView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IView> v = CreateViewFromResource(
        position, convertView, parent, mDropDownResource);
    *view = v;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode SimpleAdapter::BindView(
    /* [in] */ Int32 position,
    /* [in] */ IView* view)
{
    AutoPtr<IMap> dataSet;
    mData->Get(position, (IInterface**)&dataSet);
    if (dataSet == NULL) {
        return NOERROR;
    }

    AutoPtr<ISimpleAdapterViewBinder> binder = mViewBinder;
    Int32 count = mTo->GetLength();

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> v;
        view->FindViewById((*mTo)[i], (IView**)&v);
        if (v != NULL) {
            AutoPtr<IInterface> data;
            AutoPtr<ICharSequence> key;
            CString::New((*mFrom)[i], (ICharSequence**)&key);
            dataSet->Get(key, (IInterface**)&data);

            String text;
            if (data != NULL) {
                if (ICharSequence::Probe(data) != NULL) {
                    ICharSequence::Probe(data)->ToString(&text);
                }
                else if (IInteger32::Probe(data) == NULL && IBoolean::Probe(data) == NULL){
                    Slogger::I("SimpleAdapter", "item %d, i: %d, %s, view %08x' data problem may exist.",
                        position, i, (*mFrom)[i].string(), (*mTo)[i]);
                }
            }

            if (text.IsNull()) {
                text = "";
            }

            Boolean bound = FALSE;
            if (binder != NULL) {
                binder->SetViewValue(v, data, text, &bound);
            }

            if (!bound) {
                if (ICheckable::Probe(v)) {
                    if (IBoolean::Probe(data)) {
                        IBoolean* ib = IBoolean::Probe(data);
                        Boolean value;
                        ib->GetValue(&value);
                        ICheckable::Probe(v)->SetChecked((Boolean)value);
                    }
                    else if (ITextView::Probe(v)) {
                        // Note: keep the instanceof TextView check at the bottom of these
                        // ifs since a lot of views are TextViews (e.g. CheckBoxes).
                        SetViewText(ITextView::Probe(v), text);
                    }
                    else {
                        Slogger::E("SimpleAdapter", "item %d, view %08x' data should be bound to a IBoolean",
                            position, (*mTo)[i]);
                        //throw new IllegalStateException(v.getClass().getName() +
                        //        " should be bound to a Boolean, not a " +
                        //        (data == NULL ? "<unknown type>" : data.getClass()));
                        return E_ILLEGAL_STATE_EXCEPTION;
                    }
                }
                else if (ITextView::Probe(v)) {
                    // Note: keep the instanceof TextView check at the bottom of these
                    // ifs since a lot of views are TextViews (e.g. CheckBoxes).
                    SetViewText(ITextView::Probe(v), text);
                }
                else if (IImageView::Probe(v)) {
                    if (IInteger32::Probe(data)) {
                        IInteger32* ii = IInteger32::Probe(data);
                        Int32 value;
                        ii->GetValue(&value);
                        SetViewImage(IImageView::Probe(v), value);
                    }
                    else {
                        SetViewImage(IImageView::Probe(v), text);
                    }
                }
                else {
                    Slogger::E("SimpleAdapter", "item %d, view %08x is not a view that can be bounds by this SimpleAdapter.",
                        position, (*mTo)[i]);
                    return E_ILLEGAL_STATE_EXCEPTION;
                }
            }
        }
    }

    return NOERROR;
}

ECode SimpleAdapter::GetViewBinder(
    /* [out] */ ISimpleAdapterViewBinder** viewBinder)
{
    VALIDATE_NOT_NULL(viewBinder);
    *viewBinder = mViewBinder;
    REFCOUNT_ADD(*viewBinder);
    return NOERROR;
}

ECode SimpleAdapter::SetViewBinder(
    /* [in] */ ISimpleAdapterViewBinder* viewBinder)
{
    mViewBinder = viewBinder;
    return NOERROR;
}

ECode SimpleAdapter::SetViewImage(
    /* [in] */ IImageView* v,
    /* [in] */ Int32 value)
{
    return v->SetImageResource(value);
}

ECode SimpleAdapter::SetViewImage(
    /* [in] */ IImageView* v,
    /* [in] */ const String& value)
{
    Int32 intValue = StringUtils::ParseInt32(value);
    if (!value.Equals("0") || intValue != 0) {
        v->SetImageResource(intValue);
    }
    else {
        AutoPtr<IUri> uri;
        Uri::Parse(value, (IUri**)&uri);
        v->SetImageURI(uri);
    }

    return NOERROR;
}

ECode SimpleAdapter::SetViewText(
    /* [in] */ ITextView* v,
    /* [in] */ const String& text)
{
    AutoPtr<ICharSequence> cs;
    CString::New(text, (ICharSequence**)&cs);

    return v->SetText(cs);
}

ECode SimpleAdapter::GetFilter(
    /* [out] */ IFilter** filter)
{
    VALIDATE_NOT_NULL(filter);
    if (mFilter == NULL) {
        mFilter = new SimpleFilter(this);
    }

    *filter = mFilter;
    REFCOUNT_ADD(*filter);
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
