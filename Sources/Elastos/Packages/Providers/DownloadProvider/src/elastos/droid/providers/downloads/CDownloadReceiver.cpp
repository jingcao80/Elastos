//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/providers/downloads/CDownloadReceiver.h"
#include "elastos/droid/providers/downloads/Constants.h"
#include "elastos/droid/providers/downloads/RealSystemFacade.h"
#include "elastos/droid/providers/downloads/COpenHelper.h"
#include "R.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IDownloadManager;
using Elastos::Droid::App::IDownloadManagerRequest;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Text::CTextUtils;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Provider::IDownloadsImpl;
using Elastos::Droid::Provider::CDownloadsImpl;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Core::IThread;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::StringBuilder;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Downloads {

static AutoPtr<IHandler> GetHandler()
{
    AutoPtr<IHandlerThread> thread;
    CHandlerThread::New(String("DownloadReceiver"), (IHandlerThread**)&thread);
    IThread::Probe(thread)->Start();
    AutoPtr<IHandler> h;
    AutoPtr<ILooper> loop;
    thread->GetLooper((ILooper**)&loop);
    CHandler::New(loop, (IHandler**)&h);
    return h;
}

AutoPtr<IHandler> CDownloadReceiver::sAsyncHandler = GetHandler();

//===============================================================
// CDownloadReceiver::Runnable_1::
//===============================================================
CAR_INTERFACE_IMPL(CDownloadReceiver::Runnable_1, Object, IRunnable)

CDownloadReceiver::Runnable_1::Runnable_1(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent,
    /* [in] */ IPendingResult* result,
    /* [in] */ CDownloadReceiver* host)
{
    mContext = context;
    mIntent = intent;
    mResult = result;
    mHost = host;
}

ECode CDownloadReceiver::Runnable_1::Run()
{
    mHost->HandleUidRemoved(mContext, mIntent);
    mResult->Finish();
    return NOERROR;
}

//===============================================================
// CDownloadReceiver::Runnable_2::
//===============================================================
CAR_INTERFACE_IMPL(CDownloadReceiver::Runnable_2, Object, IRunnable)

CDownloadReceiver::Runnable_2::Runnable_2(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent,
    /* [in] */ IPendingResult* result,
    /* [in] */ CDownloadReceiver* host)
{
    mContext = context;
    mIntent = intent;
    mResult = result;
    mHost = host;
}

ECode CDownloadReceiver::Runnable_2::Run()
{
    mHost->HandleNotificationBroadcast(mContext, mIntent);
    mResult->Finish();
    return NOERROR;
}

//===============================================================
// CDownloadReceiver::
//===============================================================
CAR_OBJECT_IMPL(CDownloadReceiver)

CAR_INTERFACE_IMPL(CDownloadReceiver, BroadcastReceiver, IDownloadReceiver)

ECode CDownloadReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    if (mSystemFacade == NULL) {
        mSystemFacade = new RealSystemFacade(context);
    }

    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_BOOT_COMPLETED.Equals(action)) {
        StartService(context);

    }
    else if (IIntent::ACTION_MEDIA_MOUNTED.Equals(action)) {
        StartService(context);

    }
    else if (IConnectivityManager::CONNECTIVITY_ACTION.Equals(action)) {
        AutoPtr<IInterface> serv;
        context->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&serv);
        AutoPtr<IConnectivityManager> connManager = IConnectivityManager::Probe(serv);
        AutoPtr<INetworkInfo> info;
        connManager->GetActiveNetworkInfo((INetworkInfo**)&info);
        Boolean bCon = FALSE;
        if (info != NULL && (info->IsConnected(&bCon), bCon)) {
            StartService(context);
        }

    }
    else if (IIntent::ACTION_UID_REMOVED.Equals(action)) {
        AutoPtr<IPendingResult> result;
        GoAsync((IPendingResult**)&result);
        AutoPtr<Runnable_1> p = new Runnable_1(context, intent, result, this);
        Boolean b = FALSE;
        sAsyncHandler->Post(p, &b);

    }
    else if (Constants::ACTION_RETRY.Equals(action)) {
        StartService(context);

    }
    else if (Constants::ACTION_OPEN.Equals(action)
            || Constants::ACTION_LIST.Equals(action)
            || Constants::ACTION_HIDE.Equals(action)) {

        AutoPtr<IPendingResult> result;
        GoAsync((IPendingResult**)&result);
        if (result == NULL) {
            // TODO: remove this once test is refactored
            HandleNotificationBroadcast(context, intent);
        }
        else {
            AutoPtr<Runnable_2> p = new Runnable_2(context, intent, result, this);
            Boolean b = FALSE;
            sAsyncHandler->Post(p, &b);
        }
    }
    return NOERROR;
}

void CDownloadReceiver::HandleUidRemoved(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);

    Int32 uid = 0;
    intent->GetInt32Extra(IIntent::EXTRA_UID, -1, &uid);
    Int32 count = 0;
    AutoPtr<IDownloadsImpl> imp;
    CDownloadsImpl::AcquireSingleton((IDownloadsImpl**)&imp);
    AutoPtr<IUri> uri;
    imp->GetALL_DOWNLOADS_CONTENT_URI((IUri**)&uri);
    StringBuilder sb(Constants::UID); sb += "="; sb += uid;
    resolver->Delete(
            uri, sb.ToString(), NULL, &count);

    if (count > 0) {
        Slogger::D(Constants::TAG, "Deleted %d downloads owned by UID %d", count, uid);
    }
}

void CDownloadReceiver::HandleNotificationBroadcast(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (Constants::ACTION_LIST.Equals(action)) {
        AutoPtr<ArrayOf<Int64> > ids;
        intent->GetInt64ArrayExtra(
                IDownloadManager::EXTRA_NOTIFICATION_CLICK_DOWNLOAD_IDS, (ArrayOf<Int64>**)&ids);
        SendNotificationClickedIntent(context, ids);

    }
    else if (Constants::ACTION_OPEN.Equals(action)) {
        AutoPtr<IUri> uri;
        intent->GetData((IUri**)&uri);
        AutoPtr<IContentUris> cu;
        CContentUris::AcquireSingleton((IContentUris**)&cu);
        Int64 id = 0;
        cu->ParseId(uri, &id);
        OpenDownload(context, id);
        HideNotification(context, id);

    }
    else if (Constants::ACTION_HIDE.Equals(action)) {
        AutoPtr<IUri> uri;
        intent->GetData((IUri**)&uri);
        AutoPtr<IContentUris> cu;
        CContentUris::AcquireSingleton((IContentUris**)&cu);
        Int64 id = 0;
        cu->ParseId(uri, &id);
        HideNotification(context, id);
    }
}

void CDownloadReceiver::HideNotification(
    /* [in] */ IContext* context,
    /* [in] */ Int64 id)
{
    Int32 status = 0;
    Int32 visibility = 0;

    AutoPtr<IContentUris> cu;
    CContentUris::AcquireSingleton((IContentUris**)&cu);
    AutoPtr<IDownloadsImpl> imp;
    CDownloadsImpl::AcquireSingleton((IDownloadsImpl**)&imp);
    AutoPtr<IUri> all_uri;
    imp->GetALL_DOWNLOADS_CONTENT_URI((IUri**)&all_uri);
    AutoPtr<IUri> uri;
    cu->WithAppendedId(all_uri, id, (IUri**)&uri);
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ICursor> cursor;
    cr->Query(uri, NULL, String(NULL), NULL, String(NULL), (ICursor**)&cursor);
    Boolean bMF = FALSE;
    if ((cursor->MoveToFirst(&bMF), bMF)) {
        status = GetInt32(cursor, IDownloadsImpl::COLUMN_STATUS);
        visibility = GetInt32(cursor, IDownloadsImpl::COLUMN_VISIBILITY);
    }
    else {
        Logger::W(Constants::TAG, "Missing details for download %d", id);
        return;
    }
    ICloseable::Probe(cursor)->Close();

    Boolean bCom = FALSE;
    if ((imp->IsStatusCompleted(status, &bCom), bCom) &&
            (visibility == IDownloadManagerRequest::VISIBILITY_VISIBLE_NOTIFY_COMPLETED
            || visibility == IDownloadManagerRequest::VISIBILITY_VISIBLE_NOTIFY_ONLY_COMPLETION)) {
        AutoPtr<IContentValues> values;
        CContentValues::New((IContentValues**)&values);
        values->Put(IDownloadsImpl::COLUMN_VISIBILITY,
                IDownloadsImpl::VISIBILITY_VISIBLE);
        Int32 ct = 0;
        cr->Update(uri, values, String(NULL), NULL, &ct);
    }
}

void CDownloadReceiver::OpenDownload(
    /* [in] */ IContext* context,
    /* [in] */ Int64 id)
{
    AutoPtr<IOpenHelper> ohlp;
    COpenHelper::AcquireSingleton((IOpenHelper**)&ohlp);
    Boolean bStart = FALSE;
    ohlp->StartViewIntent(context, id, IIntent::FLAG_ACTIVITY_NEW_TASK, &bStart);
    if (!bStart) {
        AutoPtr<IToastHelper> th;
        CToastHelper::AcquireSingleton((IToastHelper**)&th);
        AutoPtr<IToast> t;
        th->MakeText(context, R::string::download_no_application_title, IToast::LENGTH_SHORT, (IToast**)&t);
        t->Show();
    }
}

void CDownloadReceiver::SendNotificationClickedIntent(
    /* [in] */ IContext* context,
    /* [in] */ ArrayOf<Int64>* ids)
{
    String packageName;
    String clazz;
    Boolean isPublicApi = FALSE;

    AutoPtr<IContentUris> cu;
    CContentUris::AcquireSingleton((IContentUris**)&cu);
    AutoPtr<IDownloadsImpl> imp;
    CDownloadsImpl::AcquireSingleton((IDownloadsImpl**)&imp);
    AutoPtr<IUri> all_uri;
    imp->GetALL_DOWNLOADS_CONTENT_URI((IUri**)&all_uri);
    AutoPtr<IUri> uri;
    cu->WithAppendedId(
            all_uri, (*ids)[0], (IUri**)&uri);
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ICursor> cursor;
    cr->Query(uri, NULL, String(NULL), NULL, String(NULL), (ICursor**)&cursor);
    Boolean bMF = FALSE;
    if ((cursor->MoveToFirst(&bMF), bMF)) {
        packageName = GetString(cursor, IDownloadsImpl::COLUMN_NOTIFICATION_PACKAGE);
        clazz = GetString(cursor, IDownloadsImpl::COLUMN_NOTIFICATION_CLASS);
        isPublicApi = GetInt32(cursor, IDownloadsImpl::COLUMN_IS_PUBLIC_API) != 0;
    }
    else {
        Logger::W(Constants::TAG, "Missing details for download %d", (*ids)[0]);
        return;
    }
    ICloseable::Probe(cursor)->Close();

    AutoPtr<ITextUtils> tu;
    CTextUtils::AcquireSingleton((ITextUtils**)&tu);
    Boolean bEmp = FALSE;
    if ((tu->IsEmpty(packageName, &bEmp), bEmp)) {
        Logger::W(Constants::TAG, "Missing package; skipping broadcast");
        return;
    }

    AutoPtr<IIntent> appIntent;
    if (isPublicApi) {
        CIntent::New(IDownloadManager::ACTION_NOTIFICATION_CLICKED, (IIntent**)&appIntent);
        appIntent->SetPackage(packageName);
        appIntent->PutExtra(IDownloadManager::EXTRA_NOTIFICATION_CLICK_DOWNLOAD_IDS, ids);

    }
    else { // legacy behavior
        if ((tu->IsEmpty(clazz, &bEmp), bEmp)) {
            Logger::W(Constants::TAG, "Missing class; skipping broadcast");
            return;
        }

        CIntent::New(IDownloadManager::ACTION_NOTIFICATION_CLICKED, (IIntent**)&appIntent);
        appIntent->SetClassName(packageName, clazz);
        appIntent->PutExtra(IDownloadManager::EXTRA_NOTIFICATION_CLICK_DOWNLOAD_IDS, ids);

        if (ids->GetLength() == 1) {
            appIntent->SetData(uri);
        }
        else {
            AutoPtr<IDownloadsImpl> imp;
            CDownloadsImpl::AcquireSingleton((IDownloadsImpl**)&imp);
            AutoPtr<IUri> uri;
            imp->GetCONTENT_URI((IUri**)&uri);
            appIntent->SetData(uri);
        }
    }

    mSystemFacade->SendBroadcast(appIntent);
}

String CDownloadReceiver::GetString(
    /* [in] */ ICursor* cursor,
    /* [in] */ String col)
{
    Int32 index = 0;
    cursor->GetColumnIndexOrThrow(col, &index);
    String result;
    cursor->GetString(index, &result);
    return result;
}

Int32 CDownloadReceiver::GetInt32(
    /* [in] */ ICursor* cursor,
    /* [in] */ String col)
{
    Int32 index = 0;
    cursor->GetColumnIndexOrThrow(col, &index);
    Int32 result = 0;
    cursor->GetInt32(index, &result);
    return result;
}

void CDownloadReceiver::StartService(
    /* [in] */ IContext* context)
{
    AutoPtr<IIntent> p;
    CIntent::New(context, ECLSID_CDownloadService, (IIntent**)&p);
    AutoPtr<IComponentName> cn;
    context->StartService(p, (IComponentName**)&cn);
}

} // namespace Downloads
} // namespace Providers
} // namespace Droid
} // namespace Elastos
