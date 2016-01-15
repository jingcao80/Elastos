
#include "elastos/droid/app/ListActivity.h"
#include "elastos/droid/R.h"
#include "elastos/droid/os/CHandler.h"
#include <elastos/core/AutoLock.h>

using Elastos::Droid::R;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;

namespace Elastos {
namespace Droid {
namespace App {

//====================================================
// ListActivity::MyListener
//====================================================

CAR_INTERFACE_IMPL(ListActivity::MyListener, Object, IAdapterViewOnItemClickListener)

ListActivity::MyListener::MyListener(
    /* [in] */ ListActivity* host)
    : mHost(host)
{}

ECode ListActivity::MyListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* v,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    return mHost->OnListItemClick(IListView::Probe(parent), v, position, id);
}

//====================================================
// ListActivity::RequestRunnable
//====================================================

ListActivity::RequestRunnable::RequestRunnable(
    /* [in] */ ListActivity* host)
    : mHost(host)
{}

ECode ListActivity::RequestRunnable::Run()
{
    return (IViewParent::Probe(mHost->mList))->FocusableViewAvailable(IView::Probe(mHost->mList));
}


//====================================================
// ListActivity
//====================================================
CAR_INTERFACE_IMPL(ListActivity, Activity, IListActivity)

ListActivity::ListActivity()
    : mFinishedStart(FALSE)
{
    CHandler::New((IHandler**)&mHandler);
    mRequestFocus = new RequestRunnable(this);
    mOnClickListener = new MyListener(this);
}

ListActivity::~ListActivity()
{}

ECode ListActivity::constructor()
{
    return Activity::constructor();
}

ECode ListActivity::OnContentChanged()
{
    Activity::OnContentChanged();
    AutoPtr<IView> emptyView = Activity::FindViewById(R::id::empty);
    mList = IListView::Probe(Activity::FindViewById(R::id::list));
    if (mList == NULL) {
        // throw new RuntimeException(
        //         "Your content must have a ListView whose id attribute is " +
        //         "'android.R.id.list'");
        return E_RUNTIME_EXCEPTION;
    }
    if (emptyView != NULL) {
        IAdapterView::Probe(mList)->SetEmptyView(emptyView);
    }
    IAdapterView::Probe(mList)->SetOnItemClickListener(mOnClickListener);
    if (mFinishedStart) {
        SetListAdapter(mAdapter);
    }

    Boolean res = FALSE;
    mHandler->Post(mRequestFocus, &res);
    mFinishedStart = TRUE;
    return NOERROR;
}

ECode ListActivity::SetListAdapter(
    /* [in] */ IListAdapter* adapter)
{
    AutoLock lock(mLock);
    FAIL_RETURN(EnsureList())
    mAdapter = adapter;
    return IAdapterView::Probe(mList)->SetAdapter(IAdapter::Probe(adapter));
}

ECode ListActivity::SetSelection(
    /* [in] */ Int32 position)
{
    return IAdapterView::Probe(mList)->SetSelection(position);
}

ECode ListActivity::GetSelectedItemPosition(
    /* [our] */ Int32* pos)
{
    VALIDATE_NOT_NULL(pos)
    return IAdapterView::Probe(mList)->GetSelectedItemPosition(pos);
}

ECode ListActivity::GetSelectedItemId(
    /* [our] */ Int64* id)
{
    VALIDATE_NOT_NULL(id)
    return IAdapterView::Probe(mList)->GetSelectedItemId(id);
}

ECode ListActivity::GetListView(
    /* [out] */ IListView** listView)
{
    VALIDATE_NOT_NULL(listView)
    FAIL_RETURN(EnsureList())
    *listView = mList;
    REFCOUNT_ADD(*listView)
    return NOERROR;
}

ECode ListActivity::GetListAdapter(
    /* [out] */ IListAdapter** listAdapter)
{
    VALIDATE_NOT_NULL(listAdapter)
    *listAdapter = mAdapter;
    REFCOUNT_ADD(*listAdapter)
    return NOERROR;
}

ECode ListActivity::OnListItemClick(
    /* [in] */ IListView* l,
    /* [in] */ IView* v,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    return NOERROR;
}

ECode ListActivity::OnRestoreInstanceState(
    /* [in] */ IBundle* state)
{
    FAIL_RETURN(EnsureList())
    return Activity::OnRestoreInstanceState(state);
}

ECode ListActivity::OnDestroy()
{
    mHandler->RemoveCallbacks(mRequestFocus);
    return Activity::OnDestroy();
}

ECode ListActivity::EnsureList()
{
    if (mList != NULL) {
        return NOERROR;
    }
    return Activity::SetContentView(R::layout::list_content_simple);
}

} // namespace App
} // namespace Droid
} // namespace Elastos
