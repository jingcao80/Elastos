
#include "CMainActivity.h"

#include "CMainActivity.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::App::EIID_IActivity;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::EIID_IServiceConnection;
// using Elastos::Droid::Os::CHandler;
// using Elastos::Droid::Os::CLooperHelper;
// using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::EIID_ISeekBarOnSeekBarChangeListener;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;

using Elastos::Droid::Os::IDebug;
using Elastos::Droid::Os::CDebug;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::CFileDescriptor;
using Libcore::IO::IOsConstants;
using Libcore::IO::COsConstants;
using Libcore::IO::ILibcore;
using Libcore::IO::CLibcore;
using Libcore::IO::IOs;
using Libcore::IO::IIoBridge;
using Libcore::IO::CIoBridge;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Core::CStringWrapper;
using Elastos::Droid::Provider::IMediaStoreVideoVideoColumns;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace VideoPlayerDemo {

//=======================================================================
//              CMainActivity::ItemOnClickListener
//=======================================================================

CAR_INTERFACE_IMPL(CMainActivity::ItemOnClickListener, IAdapterViewOnItemClickListener)

CMainActivity::ItemOnClickListener::ItemOnClickListener(
    /* [in] */ CMainActivity* host)
    : mActivity(host)
{
}

ECode CMainActivity::ItemOnClickListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->PutExtra(String("position"), position);
    intent->SetClassName(mActivity, String("VideoPlayerDemo.CPlayActivity"));
    mActivity->StartActivity(intent);

    return NOERROR;
}

//=======================================================================
//              CMainActivity
//=======================================================================

CMainActivity::CMainActivity()
{
}

CMainActivity::~CMainActivity()
{
    Logger::V("CMainActivity", "~CMainActivity()");
}

PInterface CMainActivity::Probe(
    /* [in] */REIID riid)
{
    if (riid == EIID_IInterface) {
        return (IInterface*)(IActivity *)this;
    }
    else if (riid == EIID_IActivity) {
        return (IActivity *)this;
    }

    return Activity::Probe(riid);
}

UInt32 CMainActivity::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CMainActivity::Release()
{
    return ElRefBase::Release();
}

ECode CMainActivity::GetInterfaceID(
    /* [in] */ IInterface* pObject,
    /* [out] */ InterfaceID* pIID)
{
    VALIDATE_NOT_NULL(pIID);

    return Activity::GetInterfaceID(pObject, pIID);
}

ECode CMainActivity::GetClassID(
    /* [out] */ ClassID* clsid)
{
    if (NULL == clsid) {
        return E_INVALID_ARGUMENT;
    }

    *clsid = ECLSID_CMainActivity;
    return NOERROR;
}

ECode CMainActivity::OnCreate(
/* [in] */IBundle* savedInstanceState)
{
    Logger::V("CMainActivity", "OnCreate");

    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::activity_main);

    mItemListener = new ItemOnClickListener(this);

    AutoPtr<IView> view;
    view = FindViewById(R::main::content_listview);
    assert(view != NULL);
    mContentListView = IListView::Probe(view);
    assert(mContentListView != NULL);

    mContentListView->SetOnItemClickListener(mItemListener);

    CArrayAdapter::New(this, R::layout::view_textview, (IArrayAdapter**)&mAdapter);

    mDataList = NULL;
    GetVideoList(String("type"), (ArrayOf<IInterface*>**)&mDataList);
    mContentListView->SetAdapter(mAdapter);

    if (NULL != mDataList) {
        Logger::D("CMainActivity", "InitStatus setup mAdapter");
        mAdapter->AddAll(mDataList);
        mAdapter->NotifyDataSetChanged();
    }
    else {
        Logger::D("CMainActivity", "Get mDataList = NULL");
    }

    return NOERROR;
}

ECode CMainActivity::OnResume()
{
    Logger::D("CMainActivity", "OnResume()");
    Activity::OnResume();
    return NOERROR;
}

ECode CMainActivity::OnStop()
{
    Logger::D("CMainActivity", "OnStop()");
    Activity::OnStop();
    return NOERROR;
}

ECode CMainActivity::OnDestroy()
{
    Logger::D("CMainActivity", "OnDestroy()");
    Activity::OnDestroy();
    return NOERROR;
}

ECode CMainActivity::GetVideoList(
    /* [in] */ const String& type,
    /* [out, callee] */ ArrayOf<IInterface*>** result)
{
    VALIDATE_NOT_NULL(result);

    Logger::D("CMainActivity", "GetVideoList()");

    AutoPtr<IContentResolver> cr;
    GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ArrayOf<String> > projections = ArrayOf<String>::Alloc(2);
    projections->Set(0, IMediaStoreVideoVideoColumns::ID);
    projections->Set(1, IMediaStoreVideoVideoColumns::DISPLAY_NAME);
    AutoPtr<ICursor> c;
    AutoPtr<IUri> uri;
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    helper->Parse(String("content://media/external/video/media"), (IUri**)&uri);

    cr->Query(uri, projections, String(NULL), NULL, String(NULL), (ICursor**)&c);
    Boolean b;
    AutoPtr<ArrayOf<IInterface*> > array;
    if (c != NULL) {
        Int32 count;
        c->GetCount(&count);
        array = ArrayOf<IInterface*>::Alloc(count);
        for(Int32 i = 0; i < count; i++) {
            c->MoveToNext(&b);
            if(!b) break;
            Int32 id;
            String name;
            c->GetInt32(0, &id);
            c->GetString(1, &name);
            AutoPtr<ICharSequence> seq;
            CStringWrapper::New(name, (ICharSequence**)&seq);
            array->Set(i, seq);
            Logger::D("CMediaService", " Media id = %d, name = %s",id,name.string());
        }
    }
    else {
        *result = NULL;
        return NOERROR;
    }
    if (c != NULL) {
        c->Close();
    }

    *result = array;
    REFCOUNT_ADD(*result);
    return NOERROR;
}


} // namespace VideoPlayerDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
