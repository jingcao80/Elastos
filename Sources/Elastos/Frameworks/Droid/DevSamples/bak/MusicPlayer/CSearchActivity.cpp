#include "CSearchActivity.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Utility::Logging::Logger;
// using Elastos::Utility::IObjectStringMap;
// using Elastos::Utility::CObjectStringMap;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
// using Elastos::Core::CInteger32;
// using Elastos::Core::IInteger32;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Utility::CParcelableObjectContainer;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::IImageView;
// using Elastos::Droid::Widget::ISimpleAdapter;
// using Elastos::Droid::Widget::CSimpleAdapter;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::EIID_IAdapter;
using Elastos::Droid::Widget::EIID_IListAdapter;
using Elastos::Droid::Widget::EIID_IBaseAdapter;

namespace MusicPlayer {

//=======================================================================
//              CSearchActivity::MyListener
//=======================================================================
CAR_INTERFACE_IMPL(CSearchActivity::MyListener, IViewOnClickListener)

CSearchActivity::MyListener::MyListener(
    /* [in] */ CSearchActivity* host)
    : mActivity(host)
{
}

ECode CSearchActivity::MyListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 viewId;
    v->GetId(&viewId);

    AutoPtr<IIntent> intent;

    switch (viewId)
    {
    case R::search::imgview_title_back:
        mActivity->Finish();
        break;

    case R::search::title_btn_search:
        break;

    case R::search::title_btn_edit:
        CIntent::New((IIntent**)&intent);
        intent->SetClassName(mActivity,
            String("MusicPlayer.CEditActivity"));
        mActivity->StartActivity(intent);
        break;

    case R::main::foot_btn_play:
        mActivity->mBtnPlay->SetVisibility(IView::INVISIBLE);
        mActivity->mBtnPause->SetVisibility(IView::VISIBLE);
        break;

    case R::main::foot_btn_pause:
        mActivity->mBtnPause->SetVisibility(IView::INVISIBLE);
        mActivity->mBtnPlay->SetVisibility(IView::VISIBLE);
        break;
    }

    return NOERROR;
}

//=======================================================================
//              CSearchActivity::ContentAdapter
//=======================================================================
Int32 CSearchActivity::ContentAdapter::mImage[] = {
    R::drawable::music_logo,
    R::drawable::btn_search
};

Int32 CSearchActivity::ContentAdapter::mImageLength = ArraySize(CSearchActivity::ContentAdapter::mImage);

CSearchActivity::ContentAdapter::ContentAdapter(
    /* [in] */ CSearchActivity* activity)
    :  mActivity(activity)
{
}

PInterface CSearchActivity::ContentAdapter::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface) (IBaseAdapter*) this;
    }
    else if (riid == EIID_IBaseAdapter) {
        return (PInterface) (IBaseAdapter*) this;
    }
    else if (riid == EIID_IListAdapter) {
        return (PInterface) (IListAdapter*) this;
    }
    else if (riid == EIID_IAdapter) {
        return (PInterface) (IBaseAdapter*) this;
    }

    return NULL;
}

UInt32 CSearchActivity::ContentAdapter::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CSearchActivity::ContentAdapter::Release() {
    return ElRefBase::Release();
}

ECode CSearchActivity::ContentAdapter::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    VALIDATE_NOT_NULL(pIID);

    if (pObject == (IInterface *)(IBaseAdapter *)this) {
        *pIID = EIID_IBaseAdapter;
        return NOERROR;
    }
    else if (pObject == (IInterface *)(IListAdapter *)this) {
        *pIID = EIID_IListAdapter;
        return NOERROR;
    }
    else if (pObject == (IInterface *)(IAdapter *)this) {
        *pIID = EIID_IAdapter;
        return NOERROR;
    }

    assert(0);
    return E_INVALID_ARGUMENT;
}

ECode CSearchActivity::ContentAdapter::RegisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    return E_NOT_IMPLEMENTED;
}

ECode CSearchActivity::ContentAdapter::UnregisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    return E_NOT_IMPLEMENTED;
}

ECode CSearchActivity::ContentAdapter::GetCount(
    /* [in] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = mImageLength;
    return NOERROR;
}

ECode CSearchActivity::ContentAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    return E_NOT_IMPLEMENTED;
}

ECode CSearchActivity::ContentAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* itemId)
{
    VALIDATE_NOT_NULL(itemId);
    *itemId = position;
    return NOERROR;
}

ECode CSearchActivity::ContentAdapter::HasStableIds(
    /* [out] */ Boolean* hasStableIds)
{
    VALIDATE_NOT_NULL(hasStableIds);
    *hasStableIds = FALSE;

    return NOERROR;
}

ECode CSearchActivity::ContentAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<ILayoutInflater> inflater;
    mActivity->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&inflater);

    AutoPtr<IView> conertView = convertView;
    if (conertView == NULL) {
        inflater->Inflate(R::layout::view_main_item, NULL, (IView**)&conertView);
        assert(conertView != NULL);

        AutoPtr<IView> temp;
        conertView->FindViewById(R::item::main_imgview_left, (IView**)&temp);
        assert(temp != NULL);
        AutoPtr<IImageView> imgview = IImageView::Probe(temp);
        assert(imgview != NULL);
        imgview->SetImageResource(mImage[position]);
    }

    *view = conertView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CSearchActivity::ContentAdapter::GetItemViewType(
    /* [in] */ Int32 position,
    /* [out] */ Int32* viewType)
{
    VALIDATE_NOT_NULL(viewType);
//            *pViewType = 0;
    return NOERROR;
}

ECode CSearchActivity::ContentAdapter::GetViewTypeCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = 1;
    return NOERROR;
}

ECode CSearchActivity::ContentAdapter::IsEmpty(
    /* [out] */ Boolean* isEmpty)
{
    VALIDATE_NOT_NULL(isEmpty);
    *isEmpty = mImageLength == 0;
    return NOERROR;
}

ECode CSearchActivity::ContentAdapter::AreAllItemsEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = TRUE;
    return NOERROR;
}

ECode CSearchActivity::ContentAdapter::IsEnabled(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = TRUE;
    return NOERROR;
}

ECode CSearchActivity::ContentAdapter::NotifyDataSetChanged()
{
    return E_NOT_IMPLEMENTED;
}

ECode CSearchActivity::ContentAdapter::NotifyDataSetInvalidated()
{
    return E_NOT_IMPLEMENTED;
}

ECode CSearchActivity::ContentAdapter::GetDropDownView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    return E_NOT_IMPLEMENTED;
}

//=======================================================================
//              CSearchActivity::MediaScannerReceiver
//=======================================================================

ECode CSearchActivity::MediaScannerReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Logger::D("OnReceive", "\n+++++++++++++ OnReceive() \n");
    return NOERROR;
}

//=======================================================================
//              CSearchActivity
//=======================================================================

ECode CSearchActivity::InitView()
{
    AutoPtr<IView> view = FindViewById(R::search::title_btn_search);
    assert(view != NULL);
    mBtnSearch = IButton::Probe(view);
    assert(mBtnSearch != NULL);

    mBtnBack = FindViewById(R::search::imgview_title_back);
    assert(mBtnBack != NULL);

    view = FindViewById(R::search::title_btn_edit);
    assert(view != NULL);
    mBtnEdit = IButton::Probe(view);
    assert(mBtnEdit != NULL);

    view = FindViewById(R::main::foot_btn_play);
    assert(view != NULL);
    mBtnPlay = IButton::Probe(view);
    assert(mBtnPlay != NULL);

    view = FindViewById(R::main::foot_btn_pause);
    assert(view != NULL);
    mBtnPause = IButton::Probe(view);
    assert(mBtnPause != NULL);

    view = FindViewById(R::main::content_listview);
    assert(view != NULL);
    mContentListView = IListView::Probe(view);
    assert(mContentListView != NULL);

#if 0
    AutoPtr<ContentAdapter> mAdapter = new ContentAdapter(this);
    mContentListView->SetAdapter(mAdapter);
#endif


#if 0
    AutoPtr<ArrayOf<String> > from = ArrayOf<String>::Alloc(2);
    (*from)[0] = "title";
    (*from)[1] = "icon";
    AutoPtr<ArrayOf<Int32> > to = ArrayOf<Int32>::Alloc(2);
    (*to)[0] = R::item::main_tv_singer;
    (*to)[1] = R::item::main_imgview_left;

    AutoPtr<IObjectContainer> mDataList;
    CParcelableObjectContainer::New((IObjectContainer**)&mDataList);

    AutoPtr<ICharSequence> cs;
    for(Int32 i=0; i<10; ++i)
        {
            AutoPtr<IObjectStringMap> map;
            CObjectStringMap::New((IObjectStringMap**)&map);
            assert(NULL != map);

            StringBuilder sb("name");
            sb += i;
            cs = sb.ToCharSequence();
            map->Put(String("title"), cs->Probe(EIID_IInterface));
            AutoPtr<IInteger32> drawableId;
            CInteger32::New(R::drawable::music_logo, (IInteger32**)&drawableId);
            map->Put(String("icon"), drawableId->Probe(EIID_IInterface));
            mDataList->Add(map);
        }

    AutoPtr<ISimpleAdapter> mAdapter;
    CSimpleAdapter::New(this, mDataList, R::layout::view_main_item, from.Get(), to.Get(), (ISimpleAdapter**)&mAdapter);
    mContentListView->SetAdapter(mAdapter);
#endif

    AutoPtr<IObjectContainer> dataList;
    CParcelableObjectContainer::New((IObjectContainer**)&dataList);

    for (Int32 i = 1; i<6; ++i) {
        StringBuilder sb("音乐");
        sb += i;
        AutoPtr<ICharSequence> seq = sb.ToCharSequence();
        dataList->Add(seq->Probe(EIID_IInterface));
    }

    AutoPtr<IArrayAdapter> adapter;
    CArrayAdapter::New(this, R::layout::view_textview, dataList, (IArrayAdapter**)&adapter);
    mContentListView->SetAdapter(adapter);

    return NOERROR;
}

ECode CSearchActivity::SetupListener()
{
    mBtnSearch->SetOnClickListener(mActionListener);
    mBtnEdit->SetOnClickListener(mActionListener);
    mBtnPlay->SetOnClickListener(mActionListener);
    mBtnPause->SetOnClickListener(mActionListener);
    mBtnBack->SetOnClickListener(mActionListener);

    return NOERROR;
}

ECode CSearchActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    Logger::V("TAG", "CSearchActivity::OnCreate\n");
    SetContentView(R::layout::activity_search);

    mActionListener = new MyListener(this);

    InitView();

    SetupListener();

    return NOERROR;
}

} //end namespace MusicPlayer
