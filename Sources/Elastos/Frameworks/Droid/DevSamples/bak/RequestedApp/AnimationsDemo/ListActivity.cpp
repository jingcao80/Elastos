
#include "ListActivity.h"
#include <R.h>

using Elastos::Droid::Os::CHandler;
using Elastos::Core::EIID_IRunnable;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AnimationsDemo {

ListActivity::MyListener::MyListener(
    /* [in] */ ListActivity* host)
    : mHost(host)
{
}

PInterface ListActivity::MyListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IAdapterViewOnItemClickListener*)this;
    }
    else if (riid == EIID_IAdapterViewOnItemClickListener) {
        return (IAdapterViewOnItemClickListener*)this;
    }

    return NULL;
}

UInt32 ListActivity::MyListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 ListActivity::MyListener::Release()
{
    return ElRefBase::Release();
}

ECode ListActivity::MyListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    assert(0);
    return NOERROR;
}

ECode ListActivity::MyListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* v,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    return mHost->OnListItemClick(IListView::Probe(parent), v, position, id);;
}

ListActivity::RequestRunnable::RequestRunnable(
    /* [in] */ ListActivity* host)
    : mHost(host)
{
}

PInterface ListActivity::RequestRunnable::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IRunnable*)this;
    }
    else if (riid == EIID_IRunnable) {
        return (IRunnable*)this;
    }

    return NULL;
}

UInt32 ListActivity::RequestRunnable::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 ListActivity::RequestRunnable::Release()
{
    return ElRefBase::Release();
}

ECode ListActivity::RequestRunnable::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    assert(0);
    return NOERROR;
}

ECode ListActivity::RequestRunnable::Run()
{
    assert(IViewParent::Probe(mHost->mList) != NULL);
    return IViewParent::Probe(mHost->mList)->FocusableViewAvailable(IView::Probe(mHost->mList));
}

ListActivity::ListActivity()
    : mFinishedStart(FALSE)
{
    CHandler::New((IHandler**)&mHandler);
    mRequestFocus = new RequestRunnable(this);
    AutoPtr<MyListener> l = new MyListener(this);
    mOnClickListener = (IAdapterViewOnItemClickListener*)(l->Probe(EIID_IAdapterViewOnItemClickListener));
}

ECode ListActivity::OnContentChanged()
{
    Activity::OnContentChanged();
    AutoPtr<IView> emptyView = FindViewById(R::id::empty);
    mList = IListView::Probe(FindViewById(R::id::list));
    if (mList == NULL) {
        // throw new RuntimeException(
        //         "Your content must have a ListView whose id attribute is " +
        //         "'android.R.id.list'");
        return E_RUNTIME_EXCEPTION;
    }
    if (emptyView != NULL) {
        mList->SetEmptyView(emptyView);
    }
    mList->SetOnItemClickListener(mOnClickListener);
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
    EnsureList();
    mAdapter = adapter;
    return mList->SetAdapter(adapter);
}

ECode ListActivity::SetSelection(
    /* [in] */ Int32 position)
{
    return mList->SetSelection(position);
}

Int32 ListActivity::GetSelectedItemPosition()
{
    Int32 pos = 0;
    mList->GetSelectedItemPosition(&pos);
    return pos;
}

Int64 ListActivity::GetSelectedItemId()
{
    Int64 id = 0;
    mList->GetSelectedItemId(&id);
    return id;
}

AutoPtr<IListView> ListActivity::GetListView()
{
    EnsureList();
    return mList;
}

AutoPtr<IListAdapter> ListActivity::GetListAdapter()
{
    return mAdapter;
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
    EnsureList();
    return Activity::OnRestoreInstanceState(state);
}

ECode ListActivity::OnDestroy()
{
    mHandler->RemoveCallbacks(mRequestFocus);
    return Activity::OnDestroy();
}

void ListActivity::EnsureList()
{
    if (mList != NULL) {
        return;
    }
    SetContentView(R::layout::list_content_simple);
}

} // namespace AnimationsDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
