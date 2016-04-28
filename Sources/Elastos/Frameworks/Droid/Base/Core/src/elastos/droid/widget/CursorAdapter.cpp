
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/widget/CursorAdapter.h"
#include "elastos/droid/widget/CCursorFilter.h"
#include "elastos/droid/widget/CCursorAdapterChangeObserver.h"
#include "elastos/droid/widget/CCursorAdapterDataSetObserver.h"
#include "elastos/droid/os/CHandler.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Database::EIID_IDataSetObserver;
using Elastos::Droid::Os::CHandler;
using Elastos::Core::CString;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Widget {

static const String TAG("CursorAdapter");
static const Boolean DEBUG = FALSE;

//===============================================================================
// CursorAdapter
//===============================================================================
CAR_INTERFACE_IMPL_3(CursorAdapter, BaseAdapter, ICursorAdapter, IFilterable, ICursorFilterClient);

CursorAdapter::CursorAdapter()
    : mDataValid(FALSE)
    , mAutoRequery(FALSE)
    , mRowIDColumn(0)
{
}

ECode CursorAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* c)
{
    return Init(context, c, ICursorAdapter::FLAG_AUTO_REQUERY);
}

ECode CursorAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* c,
    /* [in] */ Boolean autoRequery)
{
    return Init(context, c, autoRequery
        ? ICursorAdapter::FLAG_AUTO_REQUERY : ICursorAdapter::FLAG_REGISTER_CONTENT_OBSERVER);
}

ECode CursorAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* c,
    /* [in] */ Int32 flags)
{
    return Init(context, c, flags);
}

ECode CursorAdapter::Init(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* c,
    /* [in] */ Boolean autoRequery)
{
    return Init(context, c, autoRequery
        ? ICursorAdapter::FLAG_AUTO_REQUERY : ICursorAdapter::FLAG_REGISTER_CONTENT_OBSERVER);
}

ECode CursorAdapter::Init(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* c,
    /* [in] */ Int32 flags)
{
    if ((flags & ICursorAdapter::FLAG_AUTO_REQUERY) == ICursorAdapter::FLAG_AUTO_REQUERY) {
        flags |= ICursorAdapter::FLAG_REGISTER_CONTENT_OBSERVER;
        mAutoRequery = TRUE;
    }
    else {
        mAutoRequery = FALSE;
    }
    Boolean cursorPresent = c != NULL;
    mCursor = c;
    mDataValid = cursorPresent;

    assert(context);
    AutoPtr<IWeakReferenceSource> wrs = IWeakReferenceSource::Probe(context);
    assert(wrs != NULL && "Error: Invalid context, IWeakReferenceSource not implemented!");
    wrs->GetWeakReference((IWeakReference**)&mWeakContext);

    Int32 index;
    c->GetColumnIndexOrThrow(String("_id"), &index);
    mRowIDColumn = cursorPresent ? index : -1;
    if ((flags & ICursorAdapter::FLAG_REGISTER_CONTENT_OBSERVER) == ICursorAdapter::FLAG_REGISTER_CONTENT_OBSERVER) {
        CCursorAdapterChangeObserver::New(this, (IContentObserver**)&mChangeObserver);
        CCursorAdapterDataSetObserver::New(this, (IDataSetObserver**)&mDataSetObserver);
    }

    if (cursorPresent) {
        if (mChangeObserver != NULL) c->RegisterContentObserver(mChangeObserver);
        if (mDataSetObserver != NULL) c->RegisterDataSetObserver(mDataSetObserver);
    }
    return NOERROR;
}

ECode CursorAdapter::GetCursor(
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor);
    *cursor = mCursor;
    REFCOUNT_ADD(*cursor);
    return NOERROR;
}

ECode CursorAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    if (mDataValid && mCursor != NULL) {
        return mCursor->GetCount(count);
    }

    *count = 0;
    return NOERROR;
}

ECode CursorAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item);
    if (mDataValid && mCursor != NULL) {
        Boolean res = FALSE;
        mCursor->MoveToPosition(position, &res);
        *item = mCursor;
        REFCOUNT_ADD(*item);
        return NOERROR;
    }

    *item = NULL;
    return NOERROR;
}

ECode CursorAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    *id = 0;
    if (mDataValid && mCursor != NULL) {
        Boolean r;
        if (mCursor->MoveToPosition(position, &r)) {
            return mCursor->GetInt64(mRowIDColumn, id);
        }
        else {
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CursorAdapter::HasStableIds(
    /* [out] */ Boolean* hasStableIds)
{
    VALIDATE_NOT_NULL(hasStableIds);
    *hasStableIds = TRUE;
    return NOERROR;
}

ECode CursorAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = NULL;
    if (!mDataValid) {
        Slogger::E("CursorAdapter::GetView", "this should only be called when the cursor is valid");
        return E_ILLEGAL_STATE_EXCEPTION;
        //throw new IllegalStateException("this should only be called when the cursor is valid");
    }
    Boolean r = FALSE;
    if (!(mCursor->MoveToPosition(position, &r), r)) {
        Slogger::E("CursorAdapter::GetView", "couldn't move cursor to position  %d", position);
        return E_ILLEGAL_STATE_EXCEPTION;
        //throw new IllegalStateException("couldn't move cursor to position " + position);
    }

    AutoPtr<IContext> context;
    mWeakContext->Resolve(EIID_IContext, (IInterface**)&context);
    if (context == NULL) {
        Slogger::E("CursorAdapter::GetView", "context has been destoried!");
        return NOERROR;
    }

    AutoPtr<IView> v;
    if (convertView == NULL) {
        NewView(context, mCursor, parent, (IView**)&v);
    }
    else {
        v = convertView;
    }
    BindView(v, context, mCursor);
    *view = v;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CursorAdapter::GetDropDownView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IView> v;
    if (mDataValid) {
        Boolean res = FALSE;
        mCursor->MoveToPosition(position, &res);

        AutoPtr<IContext> context;
        mWeakContext->Resolve(EIID_IContext, (IInterface**)&context);
        if (context == NULL) {
            Slogger::E("CursorAdapter::GetDropDownView", "context has been destoried!");
            return NOERROR;
        }

        if (convertView == NULL) {
            NewDropDownView(context, mCursor, parent, (IView**)&v);
        }
        else {
            v = convertView;
        }
        BindView(v, context, mCursor);
    }

    *view = v;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CursorAdapter::NewDropDownView(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    return NewView(context, cursor, parent, view);
}

ECode CursorAdapter::ChangeCursor(
    /* [in] */ ICursor* cursor)
{
    AutoPtr<ICursor> old;
    SwapCursor(cursor, (ICursor**)&old);
    if (old != NULL) {
        ICloseable::Probe(old)->Close();
    }
    return NOERROR;
}

ECode CursorAdapter::SwapCursor(
    /* [in] */ ICursor* newCursor,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor);
    if (newCursor == mCursor) {
        *cursor = NULL;
        return NOERROR;
    }
    AutoPtr<ICursor> oldCursor = mCursor;
    if (oldCursor != NULL) {
        if (mChangeObserver != NULL) oldCursor->UnregisterContentObserver(mChangeObserver);
        if (mDataSetObserver != NULL) oldCursor->UnregisterDataSetObserver(mDataSetObserver);
    }
    mCursor = newCursor;
    if (newCursor != NULL) {
        if (mChangeObserver != NULL) newCursor->RegisterContentObserver(mChangeObserver);
        if (mDataSetObserver != NULL) newCursor->RegisterDataSetObserver(mDataSetObserver);
        newCursor->GetColumnIndexOrThrow(String("_id"), &mRowIDColumn);
        mDataValid = TRUE;
        // notify the observers about the new cursor
        NotifyDataSetChanged();
    }
    else {
        mRowIDColumn = -1;
        mDataValid = FALSE;
        // notify the observers about the lack of a data set
        NotifyDataSetInvalidated();
    }
    *cursor = oldCursor;
    REFCOUNT_ADD(*cursor);
    return NOERROR;
}

ECode CursorAdapter::ConvertToString(
    /* [in] */ ICursor* cursor,
    /* [out] */ ICharSequence** charSequence)
{
    VALIDATE_NOT_NULL(charSequence);
    String str("");
    if (cursor != NULL) {
        IObject::Probe(cursor)->ToString(&str);
    }
    return CString::New(str, charSequence);
}

ECode CursorAdapter::RunQueryOnBackgroundThread(
    /* [in] */ ICharSequence* constraint,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor);
    if (mFilterQueryProvider != NULL) {
        return mFilterQueryProvider->RunQuery(constraint, cursor);
    }

    *cursor = mCursor;
    REFCOUNT_ADD(*cursor);
    return NOERROR;
}

ECode CursorAdapter::GetFilter(
    /* [out] */ IFilter** filter)
{
    VALIDATE_NOT_NULL(filter);
    if(mCursorFilter == NULL) {
        CCursorFilter::New(this, (ICursorFilter**)&mCursorFilter);
    }
    *filter = IFilter::Probe(mCursorFilter);
    REFCOUNT_ADD(*filter);
    return NOERROR;
}

ECode CursorAdapter::GetFilterQueryProvider(
    /* [out] */ IFilterQueryProvider** filterQueryProvider)
{
    VALIDATE_NOT_NULL(filterQueryProvider);
    *filterQueryProvider = mFilterQueryProvider;
    REFCOUNT_ADD(*filterQueryProvider);
    return NOERROR;
}

ECode CursorAdapter::SetFilterQueryProvider(
    /* [in] */ IFilterQueryProvider* filterQueryProvider)
{
    mFilterQueryProvider = filterQueryProvider;
    return NOERROR;
}

ECode CursorAdapter::OnContentChanged()
{
    Boolean closed;
    if (mAutoRequery && mCursor != NULL && !(mCursor->IsClosed(&closed), closed)) {
        if (DEBUG) Slogger::V(TAG, "Auto requerying %s due to update", TO_CSTR(mCursor));
        return mCursor->Requery(&mDataValid);
    }
    return NOERROR;
}

//===============================================================================
// CursorAdapter::ChangeObserver
//===============================================================================
CursorAdapter::ChangeObserver::ChangeObserver()
{
}

ECode CursorAdapter::ChangeObserver::constructor(
    /* [in] */ ICursorAdapter* host)
{
    IWeakReferenceSource* wrs = IWeakReferenceSource::Probe(host);
    wrs->GetWeakReference((IWeakReference**)&mWeakHost);

    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    return ContentObserver::constructor(handler);
}

ECode CursorAdapter::ChangeObserver::DeliverSelfNotifications(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

ECode CursorAdapter::ChangeObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    AutoPtr<ICursorAdapter> hostObj;
    mWeakHost->Resolve(EIID_ICursorAdapter, (IInterface**)&hostObj);
    if (hostObj == NULL) {
        // tell content service to remove this observer.
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    CursorAdapter* host = (CursorAdapter*)hostObj.Get();

    return host->OnContentChanged();
}

//===============================================================================
// CursorAdapter::ChangeObserver
//===============================================================================
CAR_INTERFACE_IMPL(CursorAdapter::DataSetObserver, Object, IDataSetObserver)

CursorAdapter::DataSetObserver::DataSetObserver()
{
}

ECode CursorAdapter::DataSetObserver::constructor(
    /* [in] */ ICursorAdapter* host)
{
    IWeakReferenceSource* wrs = IWeakReferenceSource::Probe(host);
    wrs->GetWeakReference((IWeakReference**)&mWeakHost);
    return NOERROR;
}

ECode CursorAdapter::DataSetObserver::OnChanged()
{
    AutoPtr<ICursorAdapter> hostObj;
    mWeakHost->Resolve(EIID_ICursorAdapter, (IInterface**)&hostObj);
    if (hostObj == NULL) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    CursorAdapter* host = (CursorAdapter*)hostObj.Get();
    host->mDataValid = TRUE;
    return host->NotifyDataSetChanged();
}

ECode CursorAdapter::DataSetObserver::OnInvalidated()
{
    AutoPtr<ICursorAdapter> hostObj;
    mWeakHost->Resolve(EIID_ICursorAdapter, (IInterface**)&hostObj);
    if (hostObj == NULL) {
        // tell content service to remove this observer.
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    CursorAdapter* host = (CursorAdapter*)hostObj.Get();
    host->mDataValid = FALSE;
    return host->NotifyDataSetInvalidated();
}

}// namespace Elastos
}// namespace Droid
}// namespace Widget
