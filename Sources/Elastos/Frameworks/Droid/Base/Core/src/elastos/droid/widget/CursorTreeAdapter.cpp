
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Content.h"
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/widget/CursorTreeAdapter.h"
#include "elastos/droid/widget/CCursorFilter.h"
#include "elastos/droid/os/CHandler.h"

using Elastos::Droid::Database::EIID_IDataSetObserver;
using Elastos::Droid::Os::CHandler;
using Elastos::Core::CString;
using Elastos::IO::ICloseable;

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_INTERFACE_IMPL_3(CursorTreeAdapter, BaseExpandableListAdapter, ICursorTreeAdapter, IFilterable, ICursorFilterClient);
CursorTreeAdapter::CursorTreeAdapter()
    : mAutoRequery(FALSE)
{}

ECode CursorTreeAdapter::constructor(
    /* [in] */ ICursor* cursor,
    /* [in] */ IContext* context)
{
    return Init(cursor, context);
}

ECode CursorTreeAdapter::constructor(
    /* [in] */ ICursor* cursor,
    /* [in] */ IContext* context,
    /* [in] */ Boolean autoRequery)
{
    return Init(cursor, context, autoRequery);
}

ECode CursorTreeAdapter::Init(
    /* [in] */ ICursor* cursor,
    /* [in] */ IContext* context,
    /* [in] */ Boolean autoRequery)
{
    mContext = context;
    CHandler::New((IHandler**)&mHandler);
    mAutoRequery = autoRequery;

    mGroupCursorHelper = new MyCursorHelper(cursor, this);
    return NOERROR;
}

AutoPtr<CursorTreeAdapter::MyCursorHelper> CursorTreeAdapter::GetChildrenCursorHelper(
    /* [in] */ Int32 groupPosition,
    /* [in] */ Boolean requestCursor)
{
    Iterator it = mChildrenCursorHelpers.Find(groupPosition);
    AutoPtr<MyCursorHelper> cursorHelper;
    if (it != mChildrenCursorHelpers.End()) {
        cursorHelper = it->mSecond;
    }

    if (cursorHelper == NULL) {
        if (mGroupCursorHelper->MoveTo(groupPosition) == NULL) return NULL;

        AutoPtr<ICursor> cursor = GetChildrenCursor(mGroupCursorHelper->GetCursor());
        cursorHelper = new MyCursorHelper(cursor, this);
        mChildrenCursorHelpers[groupPosition] = cursorHelper;
    }

    return cursorHelper;
}

ECode CursorTreeAdapter::SetGroupCursor(
    /* [in] */ ICursor* cursor)
{
    mGroupCursorHelper->ChangeCursor(cursor, FALSE);
    return NOERROR;
}

ECode CursorTreeAdapter::SetChildrenCursor(
    /* [in] */ Int32 groupPosition,
    /* [in] */ ICursor* childrenCursor)
{
    /*
     * Don't request a cursor from the subclass, instead we will be setting
     * the cursor ourselves.
     */
    AutoPtr<MyCursorHelper> childrenCursorHelper = GetChildrenCursorHelper(groupPosition, FALSE);

    /*
     * Don't release any cursor since we know exactly what data is changing
     * (this cursor, which is still valid).
     */
    childrenCursorHelper->ChangeCursor(childrenCursor, FALSE);

    return NOERROR;
}

ECode CursorTreeAdapter::GetChild(
    /* [in] */ Int32 groupPosition,
    /* [in] */ Int32 childPosition,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor);
    // Return this group's children Cursor pointing to the particular child
    AutoPtr<ICursor> c = GetChildrenCursorHelper(groupPosition, TRUE)->MoveTo(childPosition);
    *cursor = c;
    REFCOUNT_ADD(*cursor);
    return NOERROR;
}

ECode CursorTreeAdapter::GetChildId(
    /* [in] */ Int32 groupPosition,
    /* [in] */ Int32 childPosition,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    *id = GetChildrenCursorHelper(groupPosition, TRUE)->GetId(childPosition);
    return NOERROR;
}

ECode CursorTreeAdapter::GetChildrenCount(
    /* [in] */ Int32 groupPosition,
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    AutoPtr<MyCursorHelper> helper = GetChildrenCursorHelper(groupPosition, TRUE);
    *count = (mGroupCursorHelper->IsValid() && helper != NULL) ? helper->GetCount() : 0;
    return NOERROR;
}

ECode CursorTreeAdapter::GetGroup(
    /* [in] */ Int32 groupPosition,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor);
    // Return the group Cursor pointing to the given group
    AutoPtr<ICursor> c = mGroupCursorHelper->MoveTo(groupPosition);
    *cursor = c;
    REFCOUNT_ADD(*cursor);
    return NOERROR;
}

ECode CursorTreeAdapter::GetGroupCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = mGroupCursorHelper->GetCount();
    return NOERROR;
}

ECode CursorTreeAdapter::GetGroupId(
    /* [in] */ Int32 groupPosition,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mGroupCursorHelper->GetId(groupPosition);
    return NOERROR;
}

ECode CursorTreeAdapter::GetGroupView(
    /* [in] */ Int32 groupPosition,
    /* [in] */ Boolean isExpanded,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<ICursor> cursor = mGroupCursorHelper->MoveTo(groupPosition);
    assert(cursor != NULL);

    AutoPtr<IView> v;
    if (convertView == NULL) {
        v = NewGroupView(mContext, cursor, isExpanded, parent);
    }
    else {
        v = convertView;
    }
    BindGroupView(v, mContext, cursor, isExpanded);
    *view = v;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CursorTreeAdapter::GetChildView(
    /* [in] */ Int32 groupPosition,
    /* [in] */ Int32 childPosition,
    /* [in] */ Boolean isLastChild,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<MyCursorHelper> cursorHelper = GetChildrenCursorHelper(groupPosition, TRUE);

    AutoPtr<ICursor> cursor = cursorHelper->MoveTo(childPosition);
    assert(cursor != NULL);

    AutoPtr<IView> v;
    if (convertView == NULL) {
        v = NewChildView(mContext, cursor, isLastChild, parent);
    }
    else {
        v = convertView;
    }
    BindChildView(v, mContext, cursor, isLastChild);
    *view = v;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CursorTreeAdapter::IsChildSelectable(
    /* [in] */ Int32 groupPosition,
    /* [in] */ Int32 childPosition,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode CursorTreeAdapter::HasStableIds(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

void CursorTreeAdapter::ReleaseCursorHelpers()
{
    Iterator it = mChildrenCursorHelpers.Begin();
    for (; it != mChildrenCursorHelpers.End(); it++) {
        AutoPtr<MyCursorHelper> helper = it->mSecond;
        helper->Deactivate();
    }

    mChildrenCursorHelpers.Clear();
}

ECode CursorTreeAdapter::NotifyDataSetChanged()
{
    NotifyDataSetChanged(TRUE);

    return NOERROR;
}

ECode CursorTreeAdapter::NotifyDataSetChanged(
    /* [in] */ Boolean releaseCursors)
{
    if (releaseCursors) {
        ReleaseCursorHelpers();
    }

    return BaseExpandableListAdapter::NotifyDataSetChanged();
}

ECode CursorTreeAdapter::NotifyDataSetInvalidated()
{
    ReleaseCursorHelpers();
    return BaseExpandableListAdapter::NotifyDataSetInvalidated();
}

ECode CursorTreeAdapter::OnGroupCollapsed(
    /* [in] */ Int32 groupPosition)
{
    DeactivateChildrenCursorHelper(groupPosition);

    return NOERROR;
}

ECode CursorTreeAdapter::DeactivateChildrenCursorHelper(
    /* [in] */ Int32 groupPosition)
{
    AutoPtr<MyCursorHelper> cursorHelper = GetChildrenCursorHelper(groupPosition, TRUE);
    mChildrenCursorHelpers.Erase(groupPosition);
    cursorHelper->Deactivate();

    return NOERROR;
}

ECode CursorTreeAdapter::ConvertToString(
    /* [in] */ ICursor* cursor,
    /* [out] */ ICharSequence** str)
{
    VALIDATE_NOT_NULL(str);
    String value("");
    if (cursor != NULL) {
        IObject::Probe(cursor)->ToString(&value);
    }
    return CString::New(value, str);
}

ECode CursorTreeAdapter::RunQueryOnBackgroundThread(
    /* [in] */ ICharSequence* constraint,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor);
    if (mFilterQueryProvider != NULL) {
        return mFilterQueryProvider->RunQuery(constraint, cursor);
    }

    AutoPtr<ICursor> c = mGroupCursorHelper->GetCursor();
    *cursor = c;
    REFCOUNT_ADD(*cursor);
    return NOERROR;
}

ECode CursorTreeAdapter::GetFilter(
    /* [out] */ IFilter** filter)
{
    VALIDATE_NOT_NULL(filter);
    if (mCursorFilter == NULL) {
        CCursorFilter::New(this, (ICursorFilter**)&mCursorFilter);
    }
    *filter = IFilter::Probe(mCursorFilter);
    REFCOUNT_ADD(*filter);
    return NOERROR;
}

ECode CursorTreeAdapter::GetFilterQueryProvider(
    /* [out] */ IFilterQueryProvider** provider)
{
    VALIDATE_NOT_NULL(provider);
    *provider = mFilterQueryProvider;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

ECode CursorTreeAdapter::SetFilterQueryProvider(
    /* [in] */ IFilterQueryProvider* filterQueryProvider)
{
    mFilterQueryProvider = filterQueryProvider;

    return NOERROR;
}

ECode CursorTreeAdapter::ChangeCursor(
    /* [in] */ ICursor* cursor)
{
    mGroupCursorHelper->ChangeCursor(cursor, TRUE);
    return NOERROR;
}

ECode CursorTreeAdapter::GetCursor(
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor);
    AutoPtr<ICursor> c = mGroupCursorHelper->GetCursor();
    *cursor = c;
    REFCOUNT_ADD(*cursor);
    return NOERROR;
}

CursorTreeAdapter::MyCursorHelper::MyCursorHelper(
    /* [in] */ ICursor* cursor,
    /* [in] */ CursorTreeAdapter* owner)
    : mOwner(owner)
{
    Boolean cursorPresent = cursor != NULL;
    mCursor = cursor;
    mDataValid = cursorPresent;
    mRowIDColumn = -1;
    if(cursorPresent)
        cursor->GetColumnIndex(String("_id"), &mRowIDColumn);

    mContentObserver = new MyContentObserver(this);
    mDataSetObserver = new MyDataSetObserver(this);
    if (cursorPresent) {
        cursor->RegisterContentObserver(mContentObserver);
        cursor->RegisterDataSetObserver(mDataSetObserver);
    }
}

AutoPtr<ICursor> CursorTreeAdapter::MyCursorHelper::GetCursor()
{
    return mCursor;
}

Int32 CursorTreeAdapter::MyCursorHelper::GetCount()
{
    if (mDataValid && mCursor != NULL) {
        Int32 count;
        mCursor->GetCount(&count);
        return count;
    }
    else {
        return 0;
    }
}

Int64 CursorTreeAdapter::MyCursorHelper::GetId(
    /* [in] */ Int32 position)
{
    if (mDataValid && mCursor != NULL) {
        Boolean res;
        mCursor->MoveToPosition(position, &res);
        if (res) {
            Int64 columnValue;
            mCursor->GetInt64(mRowIDColumn, &columnValue);
            return columnValue;
        }
        else {
            return 0;
        }
    }
    else {
        return 0;
    }
}

AutoPtr<ICursor> CursorTreeAdapter::MyCursorHelper::MoveTo(
    /* [in] */ Int32 position)
{
    Boolean res;
    if (mDataValid && mCursor != NULL && (mCursor->MoveToPosition(position, &res), res)) {
        return mCursor;
    }
    else {
        return NULL;
    }
}

void CursorTreeAdapter::MyCursorHelper::ChangeCursor(
    /* [in] */ ICursor* cursor,
    /* [in] */ Boolean releaseCursors)
{
    if (cursor == mCursor) return;

    Deactivate();
    mCursor = cursor;
    if (cursor != NULL) {
        cursor->RegisterContentObserver(mContentObserver);
        cursor->RegisterDataSetObserver(mDataSetObserver);
        cursor->GetColumnIndex(String("_id"), &mRowIDColumn);
        mDataValid = TRUE;
        // notify the observers about the new cursor
        mOwner->NotifyDataSetChanged(releaseCursors);
    }
    else {
        mRowIDColumn = -1;
        mDataValid = FALSE;
        // notify the observers about the lack of a data set
        mOwner->NotifyDataSetInvalidated();
    }
}

void CursorTreeAdapter::MyCursorHelper::Deactivate()
{
    if (mCursor == NULL) {
        return;
    }

    mCursor->UnregisterContentObserver(mContentObserver);
    mCursor->UnregisterDataSetObserver(mDataSetObserver);
    ICloseable::Probe(mCursor)->Close();
    mCursor = NULL;
}

Boolean CursorTreeAdapter::MyCursorHelper::IsValid()
{
    return mDataValid && mCursor != NULL;
}

CursorTreeAdapter::MyCursorHelper::MyContentObserver::MyContentObserver(
    /* [in] */ MyCursorHelper* owner)
{
    ContentObserver::constructor(mHandler);
    mOwner = owner;
}

ECode CursorTreeAdapter::MyCursorHelper::MyContentObserver::DeliverSelfNotifications(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode CursorTreeAdapter::MyCursorHelper::MyContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    if (mOwner->mOwner->mAutoRequery && mOwner->mCursor != NULL) {
        /*if (Config.LOGV) Log.v("Cursor", "Auto requerying " + mCursor +
                " due to update");*/
        mOwner->mCursor->Requery(&mOwner->mDataValid);
    }

    return NOERROR;
}

CAR_INTERFACE_IMPL(CursorTreeAdapter::MyCursorHelper::MyDataSetObserver, Object, IDataSetObserver)
ECode CursorTreeAdapter::MyCursorHelper::MyDataSetObserver::OnChanged()
{
    mOwner->mDataValid = TRUE;
    return mOwner->mOwner->NotifyDataSetChanged();
}

ECode CursorTreeAdapter::MyCursorHelper::MyDataSetObserver::OnInvalidated()
{
    mOwner->mDataValid = FALSE;
    return mOwner->mOwner->NotifyDataSetInvalidated();
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
