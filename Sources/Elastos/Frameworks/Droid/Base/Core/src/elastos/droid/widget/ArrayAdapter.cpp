
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/widget/ArrayAdapter.h"
#include "elastos/droid/widget/CArrayAdapter.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Core::AutoLock;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Arrays;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Widget {

ArrayAdapter::ArrayFilter::ArrayFilter(
    /* [in] */ ArrayAdapter* host)
    : mHost(host)
{}

ECode ArrayAdapter::ArrayFilter::PerformFiltering(
    /* [in] */ ICharSequence* prefix,
    /* [out] */ IFilterResults** filterResults)
{
    VALIDATE_NOT_NULL(filterResults);

    AutoPtr<IFilterResults> results = new FilterResults();

    if (!mHost->mOriginalValues) {
        AutoLock lock(mHost->mLock);
        CArrayList::New(ICollection::Probe(mHost->mObjects), (IArrayList**)&mHost->mOriginalValues);
    }

    Int32 length = -1;
    if (prefix) {
        prefix->GetLength(&length);
    }

    if (prefix == NULL || length == 0) {
        AutoPtr<IArrayList> list;
        {
            AutoLock lock(mHost->mLock);
            CArrayList::New(ICollection::Probe(mHost->mOriginalValues), (IArrayList**)&list);
        }

        Int32 size = 0;
        mHost->mOriginalValues->GetSize(&size);
        results->SetValues(list);
        results->SetCount(size);
    }
    else {
        String prefixString;
        prefix->ToString(&prefixString);

        AutoPtr<IArrayList> values;
        {
            AutoLock lock(mHost->mLock);
            CArrayList::New(ICollection::Probe(mHost->mOriginalValues), (IArrayList**)&values);
        }

        Int32 count = 0;
        values->GetSize(&count);
        AutoPtr<IArrayList> newValues;
        CArrayList::New((IArrayList**)&newValues);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IInterface> value;
            values->Get(i, (IInterface**)&value);
            String valueText;
            if (ICharSequence::Probe(value)) {
                ICharSequence::Probe(value)->ToString(&valueText);
            }
            valueText = valueText.ToLowerCase();

            // First match against the whole, non-splitted value
            if (valueText.StartWith(prefixString)) {
                newValues->Add(value);
            } else {
                AutoPtr<ArrayOf<String> > words;
                StringUtils::Split(valueText, String(" "), (ArrayOf<String>**)&words);
                const Int32 wordCount = words->GetLength();

                // Start at index 0, in case valueText starts with space(s)
                for (Int32 k = 0; k < wordCount; k++) {
                    if ((*words)[k].StartWith(prefixString)) {
                        newValues->Add(value);
                        break;
                    }
                }
            }
        }

        results->SetValues(newValues);
        newValues->GetSize(&count);
        results->SetCount(count);
    }

    *filterResults = results;
    REFCOUNT_ADD(*filterResults);
    return NOERROR;
}

ECode ArrayAdapter::ArrayFilter::PublishResults(
    /* [in] */ ICharSequence* constraint,
    /* [in] */ IFilterResults* results)
{
    AutoPtr<IInterface> values;
    results->GetValues((IInterface**)&values);
    //noinspection unchecked
    mHost->mObjects = IList::Probe(values);
    Int32 count = 0;
    results->GetCount(&count);
    if (count > 0) {
        mHost->NotifyDataSetChanged();
    } else {
        mHost->NotifyDataSetInvalidated();
    }

    return NOERROR;
}

CAR_INTERFACE_IMPL_2(ArrayAdapter, BaseAdapter, IArrayAdapter, IFilterable);
ArrayAdapter::ArrayAdapter()
    : mResource(0)
    , mDropDownResource(0)
    , mFieldId(0)
    , mNotifyOnChange(TRUE)
    , mOriginalValues(NULL)
{}

ECode ArrayAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resource)
{
    AutoPtr<IArrayList> list;
    CArrayList::New((IArrayList**)&list);
    return Init(context, resource, 0, IList::Probe(list));
}

ECode ArrayAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resource,
    /* [in] */ Int32 textViewResourceId)
{
    AutoPtr<IArrayList> list;
    CArrayList::New((IArrayList**)&list);
    return Init(context, resource, textViewResourceId, IList::Probe(list));
}

ECode ArrayAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resource,
    /* [in] */ ArrayOf<IInterface*>* objects)
{
    AutoPtr<IList> list;
    Arrays::AsList(objects, (IList**)&list);
    return Init(context, resource, 0, list);
}

ECode ArrayAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resource,
    /* [in] */ Int32 textViewResourceId,
    /* [in] */ ArrayOf<IInterface*>* objects)
{
    AutoPtr<IList> list;
    Arrays::AsList(objects, (IList**)&list);
    return Init(context, resource, textViewResourceId, list);
}

ECode ArrayAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resource,
    /* [in] */ IList* objects)
{
    return Init(context, resource, 0, objects);
}

ECode ArrayAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resource,
    /* [in] */ Int32 textViewResourceId,
    /* [in] */ IList* objects)
{
    return Init(context, resource, textViewResourceId, objects);
}

ArrayAdapter::~ArrayAdapter()
{
    mOriginalValues = NULL;
}

ECode ArrayAdapter::Add(
    /* [in] */ IInterface* object)
{
    synchronized(mLock) {
        if (mOriginalValues != NULL) {
            mOriginalValues->Add(object);
        } else {
            mObjects->Add(object);
        }
    }

    if (mNotifyOnChange) NotifyDataSetChanged();

    return NOERROR;
}

ECode ArrayAdapter::AddAll(
    /* [in] */ ICollection* collection)
{
    synchronized(mLock) {
        if (mOriginalValues != NULL) {
            mOriginalValues->AddAll(collection);
        } else {
            mObjects->AddAll(collection);
        }
    }
    if (mNotifyOnChange) NotifyDataSetChanged();

    return NOERROR;
}

ECode ArrayAdapter::AddAll(
    /* [in] */ ArrayOf<IInterface* >* items)
{
    synchronized(mLock) {
        AutoPtr<ICollections> cs;
        CCollections::AcquireSingleton((ICollections**)&cs);
        if (mOriginalValues != NULL) {
            cs->AddAll(ICollection::Probe(mOriginalValues), items);
        } else {
            cs->AddAll(ICollection::Probe(mObjects), items);
        }
    }

    if (mNotifyOnChange) NotifyDataSetChanged();

    return NOERROR;
}

ECode ArrayAdapter::Insert(
    /* [in] */ IInterface* object,
    /* [in] */ Int32 index)
{
    synchronized(mLock) {
        if (mOriginalValues != NULL) {
            mOriginalValues->Add(index, object);
        } else {
            mObjects->Add(index, object);
        }
    }
    if (mNotifyOnChange) NotifyDataSetChanged();

    return NOERROR;
}

ECode ArrayAdapter::Remove(
    /* [in] */ IInterface* object)
{
    synchronized(mLock) {
        if (mOriginalValues != NULL) {
            mOriginalValues->Remove(object);
        } else {
            mObjects->Remove(object);
        }
    }
    if (mNotifyOnChange) NotifyDataSetChanged();

    return NOERROR;
}

ECode ArrayAdapter::Clear()
{
    synchronized(mLock) {
        if (mOriginalValues != NULL) {
            mOriginalValues->Clear();
        } else {
            mObjects->Clear();
        }
    }
    if (mNotifyOnChange) NotifyDataSetChanged();

    return NOERROR;
}

ECode ArrayAdapter::Sort(
    /* [in] */ IComparator* comparator)
{
    synchronized(mLock) {
        AutoPtr<ICollections> cs;
        CCollections::AcquireSingleton((ICollections**)&cs);
        if (mOriginalValues != NULL) {
            cs->Sort(IList::Probe(mOriginalValues), comparator);
        } else {
            cs->Sort(mObjects, comparator);
        }
    }
    if (mNotifyOnChange) NotifyDataSetChanged();

    return NOERROR;
}

ECode ArrayAdapter::NotifyDataSetChanged()
{
    BaseAdapter::NotifyDataSetChanged();
    mNotifyOnChange = TRUE;

    return NOERROR;
}

ECode ArrayAdapter::SetNotifyOnChange(
    /* [in] */ Boolean notifyOnChange)
{
    mNotifyOnChange = notifyOnChange;
    return NOERROR;
}

ECode ArrayAdapter::Init(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resource,
    /* [in] */ Int32 textViewResourceId,
    /* [in] */ IList* objects)
{
    mContext = context;
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    mInflater = ILayoutInflater::Probe(obj);
    mResource = mDropDownResource = resource;
    mObjects = objects;
    mFieldId = textViewResourceId;
    return NOERROR;
}

ECode ArrayAdapter::GetContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context);
    *context = mContext;
    REFCOUNT_ADD(*context);
    return NOERROR;
}

ECode ArrayAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    return mObjects->GetSize(count);
}

ECode ArrayAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item);
    return mObjects->Get(position, item);
}

ECode ArrayAdapter::GetPosition(
    /* [in] */ IInterface* item,
    /* [out] */ Int32* position)
{
    VALIDATE_NOT_NULL(position);
    return mObjects->IndexOf(item, position);
}

ECode ArrayAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* itemId)
{
    VALIDATE_NOT_NULL(itemId);
    *itemId = position;
    return NOERROR;
}

ECode ArrayAdapter::GetView(
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

AutoPtr<IView> ArrayAdapter::CreateViewFromResource(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [in] */ Int32 resource)
{
    AutoPtr<IView> view;
    AutoPtr<ITextView> text;

    if (convertView == NULL) {
        ECode ec = mInflater->Inflate(resource, parent, FALSE, (IView**)&view);
        if (FAILED(ec) || view == NULL) {
            Logger::E("ArrayAdapter", "Error: failed to inflate view with : "
                "position=%d, convertView=%p, parent=%p, resource=%08x, ec=%08x",
                position, convertView, parent, resource, ec);
        }
    }
    else {
        view = convertView;
    }

    //try {
    if (mFieldId == 0) {
        //  If no custom field is assigned, assume the whole resource is a TextView
        text = ITextView::Probe(view);
    }
    else {
        //  Otherwise, find the TextView field within the layout
        AutoPtr<IView> temp;
        view->FindViewById(mFieldId, (IView**)&temp);
        text = ITextView::Probe(temp);
    }

    if (text == NULL) {
        Logger::E("ArrayAdapter", "Failed to create view from resource: position=%d, resource=%08x, fieldId=%08x",
            position, resource, mFieldId);
        return NULL;
    }
    //} catch (ClassCastException e) {
    //    Log.e("ArrayAdapter", "You must supply a resource ID for a TextView");
    //    throw new IllegalStateException(
    //            "ArrayAdapter requires the resource ID to be a TextView", e);
    //}

    AutoPtr<IInterface> item;
    GetItem(position, (IInterface**)&item);
    if (ICharSequence::Probe(item)) {
        text->SetText(ICharSequence::Probe(item));
    }
    else {
        IObject* obj = IObject::Probe(item);
        if (obj != NULL) {
            String value;
            obj->ToString(&value);
            AutoPtr<ICharSequence> cs;
            CString::New(value, (ICharSequence**)&cs);
            text->SetText(cs);
        }
    }

    return view;
}

ECode ArrayAdapter::SetDropDownViewResource(
    /* [in] */ Int32 resource)
{
    mDropDownResource = resource;
    return NOERROR;
}

ECode ArrayAdapter::GetDropDownView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IView> v = CreateViewFromResource(position, convertView, parent, mDropDownResource);
    *view = v;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

AutoPtr<IArrayAdapter> ArrayAdapter::CreateFromResource(
    /* [in] */ IContext* context,
    /* [in] */ Int32 textArrayResId,
    /* [in] */ Int32 textViewResId)
{
    AutoPtr<ArrayOf<IInterface*> > strings;
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetTextArray(textArrayResId, (ArrayOf<ICharSequence*>**)&strings);
    AutoPtr<IArrayAdapter> adapter;
    CArrayAdapter::New(context, textViewResId, (ArrayOf<IInterface*>*)strings, (IArrayAdapter**)&adapter);
    return adapter;
}

ECode ArrayAdapter::GetFilter(
    /* [out] */ IFilter** filter)
{
    VALIDATE_NOT_NULL(filter);
    if (mFilter == NULL) {
        mFilter = new ArrayFilter(this);
    }

    *filter = mFilter;
    REFCOUNT_ADD(*filter);
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
