
#include "CActivityOne.h"
#include "R.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::CStringWrapper;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::App::CDownloadManagerRequest;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Provider::IDownloadsImpl;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IToastHelper;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace DownloadDemo {

CActivityOne::MyListener::MyListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

PInterface CActivityOne::MyListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IViewOnClickListener*)this;
    }
    else if (riid == EIID_IViewOnClickListener) {
        return (IViewOnClickListener*)this;
    }

    return NULL;
}

UInt32 CActivityOne::MyListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CActivityOne::MyListener::Release()
{
    return ElRefBase::Release();
}

ECode CActivityOne::MyListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)(IViewOnClickListener*)this) {
        *pIID = EIID_IViewOnClickListener;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CActivityOne::MyListener::OnClick(
    /* [in] */ IView* v)
{
Slogger::E("CActivityOne", __FUNCTION__);

    // AutoPtr<ICharSequence> text;
    // mHost->mText->GetText((ICharSequence**)&text);
    String textStr("http://down10.zol.com.cn/zoldown/wrar501@89734@.exe");
    // String textStr("http://f.hiphotos.baidu.com/zhidao/wh%3D600%2C800/sign=eb86871cb21c8701d6e3bae0174fb217/d53f8794a4c27d1e17ee023c1ad5ad6edcc43879.jpg");
    // String textStr;
    // text->ToString(&textStr);
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> uri;
    helper->Parse(textStr, (IUri**)&uri);
    AutoPtr<IDownloadManagerRequest> request;
    CDownloadManagerRequest::New(uri, (IDownloadManagerRequest**)&request);

    request->SetAllowedNetworkTypes(IDownloadManagerRequest::NETWORK_MOBILE | IDownloadManagerRequest::NETWORK_WIFI);
    AutoPtr<ICharSequence> cs;
    CStringWrapper::New(String("Updating"), (ICharSequence**)&cs);
    request->SetTitle(cs);
    cs = NULL;
    CStringWrapper::New(String("Downloading file"), (ICharSequence**)&cs);
    request->SetDescription(cs);
    request->SetDestinationInExternalPublicDir(IEnvironment::DIRECTORY_DOWNLOADS, String("rar.exe"));

    Int32 id;
    v->GetId(&id);
    switch (id) {
        case R::id::download: {
            Slogger::E("CActivityOne", "click download");
            mHost->mDownloadManager->Enqueue(request, &mHost->mDownloadId);
            Boolean result;
            mHost->mHandler->PostDelayed(mHost->mRunnable, 1000, &result);
            break;
        }
        case R::id::rm: {
            Slogger::E("CActivityOne", "click remove");
            AutoPtr<ArrayOf<Int64> > idArray = ArrayOf<Int64>::Alloc(1);
            idArray->Set(0, mHost->mDownloadId);
            Int32 result;
            mHost->mDownloadManager->Remove(idArray, &result);
            break;
        }
    }
    return NOERROR;

}

CAR_INTERFACE_IMPL(CActivityOne::MyRunnable, IRunnable);

ECode CActivityOne::MyRunnable::Run()
{
    Int32 status = 0;
    AutoPtr<IDownloadManagerQuery> query;
    CDownloadManagerQuery::New((IDownloadManagerQuery**)&query);
    AutoPtr<ArrayOf<Int64> > idArray = ArrayOf<Int64>::Alloc(1);
    idArray->Set(0, mHost->mDownloadId);
    query->SetFilterById(idArray);
    AutoPtr<ICursor> cursor;
    mHost->mDownloadManager->Query(query, (ICursor**)&cursor);
    Slogger::E("CActivityOne", "the cursor: %p", cursor.Get());
    Int32 count;
    cursor->GetCount(&count);
    if (count > 0) {
        Boolean tofirst;
        cursor->MoveToFirst(&tofirst);
        Boolean afterLast;
        cursor->IsAfterLast(&afterLast);
        while (!afterLast) {
            Int32 index;
            cursor->GetColumnIndex(IDownloadManager::COLUMN_STATUS, &index);
            cursor->GetInt32(index, &status);
            Slogger::E("CActivityOne", "the status: %d", status);
            if (status == IDownloadManager::STATUS_RUNNING) {
                Slogger::E("CActivityOne", "the download is running");
                Boolean result;
                mHost->mHandler->PostDelayed(mHost->mRunnable, 1000, &result);
            } else if (status == IDownloadManager::STATUS_PAUSED) {
                Slogger::E("CActivityOne", "the download is paused");
            } else if (status == IDownloadManager::STATUS_SUCCESSFUL) {
                Slogger::E("CActivityOne", "the download is successful");
            } else if (status == IDownloadManager::STATUS_FAILED) {
                Slogger::E("CActivityOne", "the download is failed");
            } else if (status == IDownloadManager::STATUS_PENDING) {
                Slogger::E("CActivityOne", "the downlod is pending");
            }
            Boolean hasNext;
            cursor->MoveToNext(&hasNext);
            cursor->IsAfterLast(&afterLast);
        }
    }
    if (cursor != NULL) cursor->Close();
    return NOERROR;
}

ECode CActivityOne::MyReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
Slogger::E("CActivityOne", __FUNCTION__);
    String action;
    intent->GetAction(&action);
    if (IDownloadManager::ACTION_DOWNLOAD_COMPLETE.Equals(action)) {
        Slogger::E("CActivityOne", "recevice the action ACTION_DOWNLOAD_COMPLETE");
        Int64 downloadId;
        intent->GetInt64Extra(IDownloadManager::EXTRA_DOWNLOAD_ID, 0, &downloadId);
        AutoPtr<IDownloadManagerQuery> query;
        CDownloadManagerQuery::New((IDownloadManagerQuery**)&query);
        AutoPtr<ArrayOf<Int64> > idArray = ArrayOf<Int64>::Alloc(1);
        idArray->Set(0, downloadId);
        query->SetFilterById(idArray);
        AutoPtr<ICursor> c;
        mHost->mDownloadManager->Query(query, (ICursor**)&c);
        Boolean first;
        c->MoveToFirst(&first);
        if (first) {
            Int32 columnIndex;
            c->GetColumnIndex(IDownloadManager::COLUMN_STATUS, &columnIndex);
            Int32 status;
            c->GetInt32(columnIndex, &status);
            if (IDownloadManager::STATUS_SUCCESSFUL == status) {
                c->GetColumnIndex(IDownloadManager::COLUMN_LOCAL_URI, &columnIndex);
                String uriString;
                c->GetString(columnIndex, &uriString);
                Slogger::E("CActivityOne", "the local uri: %s", uriString.string());

                AutoPtr<IToastHelper> thelper;
                CToastHelper::AcquireSingleton((IToastHelper**)&thelper);
                AutoPtr<ICharSequence> cs;
                CStringWrapper::New(String("get file complete: ") + uriString, (ICharSequence**)&cs);
                AutoPtr<IToast> toast;
                thelper->MakeText(IContext::Probe(this),
                        cs, 0, (IToast**)&toast);
                toast->Show();
            }
        }
        if (c != NULL) c->Close();
    } else if (IDownloadsImpl::ACTION_DOWNLOAD_COMPLETED.Equals(action)) {
        Slogger::E("CActivityOne", "recevice the action ACTION_DOWNLOAD_COMPLETED");
    }
    return NOERROR;
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);

    CHandler::New((IHandler**)&mHandler);
    mRunnable = new MyRunnable(this);

    AutoPtr<IView> view = FindViewById(R::id::download);
    mDBt = IButton::Probe(view);

    view = NULL;
    view = FindViewById(R::id::rm);
    mRBt = IButton::Probe(view);

    // get download service and set request
    AutoPtr<IInterface> service;
    GetSystemService(IContext::DOWNLOAD_SERVICE, (IInterface**)&service);
    mDownloadManager = IDownloadManager::Probe(service);

    // set listener
    AutoPtr<MyListener> l = new MyListener(this);
    mDBt->SetOnClickListener(l);
    mRBt->SetOnClickListener(l);


    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    return Activity::OnStart();
}

ECode CActivityOne::OnResume()
{
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IDownloadManager::ACTION_DOWNLOAD_COMPLETE, (IIntentFilter**)&filter);
    filter->AddAction(IDownloadsImpl::ACTION_DOWNLOAD_COMPLETED);
    mReceiver = new MyReceiver(this);
    AutoPtr<IIntent> intent;
    RegisterReceiver(mReceiver, filter, (IIntent**)&intent);
    return Activity::OnResume();
}

ECode CActivityOne::OnPause()
{
    UnregisterReceiver(mReceiver);
    return Activity::OnPause();
}

ECode CActivityOne::OnStop()
{
    return Activity::OnStop();
}

ECode CActivityOne::OnDestroy()
{
    return Activity::OnDestroy();
}

} // namespace DownloadDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
