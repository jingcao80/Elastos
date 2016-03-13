
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Text.h"
#include "elastos/providers/downloadprovider/downloads/CDownloadNotifier.h"
#include "elastos/providers/downloadprovider/downloads/CDownloadInfo.h"
#include "elastos/providers/downloadprovider/downloads/Constants.h"
#include "R.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::INotificationBuilder;
using Elastos::Droid::App::CNotificationBuilder;
using Elastos::Droid::App::IDownloadManager;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::INotificationInboxStyle;
using Elastos::Droid::App::CNotificationInboxStyle;
using Elastos::Droid::App::INotificationStyle;
using Elastos::Droid::App::IDownloadManagerRequest;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Provider::IDownloadsImpl;
using Elastos::Droid::Provider::CDownloadsImpl;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Text::CTextUtils;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Text::Format::CDateUtils;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Net::CUriBuilder;
using Elastos::Core::CString;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::CInteger64;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringUtils;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;
using Elastos::Providers::DownloadProvider::Downloads::CDownloadInfo;

namespace Elastos {
namespace Providers {
namespace DownloadProvider {
namespace Downloads {

//===============================================================
// CDownloadNotifier::
//===============================================================
Int32 CDownloadNotifier::TYPE_ACTIVE = 1;
Int32 CDownloadNotifier::TYPE_WAITING = 2;
Int32 CDownloadNotifier::TYPE_COMPLETE = 3;

CAR_OBJECT_IMPL(CDownloadNotifier)

CAR_INTERFACE_IMPL(CDownloadNotifier, Object, IDownloadNotifier)

CDownloadNotifier::CDownloadNotifier()
{
    CHashMap::New((IHashMap**)&mActiveNotifs);

    assert(0 && "TODO");
    // mDownloadSpeed = new LongSparseLongArray();

    // mDownloadTouch = new LongSparseLongArray();
}

ECode CDownloadNotifier::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    AutoPtr<IInterface> p;
    context->GetSystemService(
            IContext::NOTIFICATION_SERVICE, (IInterface**)&p);
    mNotifManager = INotificationManager::Probe(p);
    return NOERROR;
}

ECode CDownloadNotifier::CancelAll()
{
    mNotifManager->CancelAll();
    return NOERROR;
}

ECode CDownloadNotifier::NotifyDownloadSpeed(
    /* [in] */ Int64 id,
    /* [in] */ Int64 bytesPerSecond)
{
    synchronized (mDownloadSpeedLock) {
        if (bytesPerSecond != 0) {
            mDownloadSpeed->Put(id, bytesPerSecond);
            mDownloadTouch->Put(id, SystemClock::GetElapsedRealtime());
        }
        else {
            mDownloadSpeed->Delete(id);
            mDownloadTouch->Delete(id);
        }
    }
    return NOERROR;
}

ECode CDownloadNotifier::UpdateWith(
    /* [in] */ ICollection* downloads)
{
    synchronized (mActiveNotifsLock) {
        UpdateWithLocked(downloads);
    }
    return NOERROR;
}

AutoPtr<IClassInfo> CDownloadNotifier::TransformClassInfo(
    /* [in] */ ClassID objId)
{
    AutoPtr<IModuleInfo> moduleInfo;
    String path(objId.mUunm);
    ASSERT_SUCCEEDED(_CReflector_AcquireModuleInfo(
           path, (IModuleInfo**)&moduleInfo));
    Int32 clsCount;
    moduleInfo->GetClassCount(&clsCount);
    AutoPtr< ArrayOf<IClassInfo*> > buf = ArrayOf<IClassInfo*>::Alloc(clsCount);
    moduleInfo->GetAllClassInfos(buf);
    AutoPtr<IClassInfo> info;
    ClassID id;
    id.mUunm = (char*)malloc(80);
    for (Int32 i = 0; i < clsCount; i++) {
        (*buf)[i]->GetId(&id);
        if (id == objId) {
           info = (*buf)[i];
           break;
        }
    }

    free(id.mUunm);
    return info;
}

void CDownloadNotifier::UpdateWithLocked(
    /* [in] */ ICollection* downloads)
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);

    // Cluster downloads together
    assert(0 && "TODO");
    // AutoPtr<IMultimap> clustered = ArrayListMultimap::Create();
    AutoPtr<IIterator> it;
    downloads->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IDownloadInfo> info = IDownloadInfo::Probe(p);
        String tag = BuildNotificationTag(info);
        if (tag != NULL) {
            // clustered->Put(tag, info);
        }
    }

    // Build notification for each cluster
    AutoPtr<ISet> st;
    // clustered->GetKeySet((ISet**)&st);
    AutoPtr<IIterator> stIt;
    st->GetIterator((IIterator**)&stIt);
    while ((stIt->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        stIt->GetNext((IInterface**)&p);
        String tag;
        ICharSequence::Probe(p)->ToString(&tag);
        Int32 type = GetNotificationTagType(tag);
        AutoPtr<ICollection> cluster;// = clustered->Get(tag);

        AutoPtr<INotificationBuilder> builder;
        CNotificationBuilder::New(mContext, (INotificationBuilder**)&builder);
        Int32 clr = 0;
        res->GetColor(0x01060058, // R::color::system_notification_accent_color
                &clr);
        builder->SetColor(clr);

        // Use time when cluster was first shown to avoid shuffling
        Int64 firstShown = 0;
        AutoPtr<ICharSequence> pTag;
        CString::New(tag, (ICharSequence**)&pTag);
        Boolean bConKey = FALSE;
        if ((mActiveNotifs->ContainsKey(pTag, &bConKey), bConKey)) {
            AutoPtr<IInterface> p;
            mActiveNotifs->Get(pTag, (IInterface**)&p);
            AutoPtr<IInteger64> pFS = IInteger64::Probe(p);
            pFS->GetValue(&firstShown);
        }
        else {
            AutoPtr<ISystem> sys;
            CSystem::AcquireSingleton((ISystem**)&sys);
            sys->GetCurrentTimeMillis(&firstShown);
            AutoPtr<IInteger64> pFS;
            CInteger64::New(firstShown, (IInteger64**)&pFS);
            mActiveNotifs->Put(pTag, pFS);
        }
        builder->SetWhen(firstShown);

        // Show relevant icon
        if (type == TYPE_ACTIVE) {
            builder->SetSmallIcon(0x01080081); // R::drawable::stat_sys_download
        }
        else if (type == TYPE_WAITING) {
            builder->SetSmallIcon(0x0108008a); // R::drawable::stat_sys_warning
        }
        else if (type == TYPE_COMPLETE) {
            builder->SetSmallIcon(0x01080082); // R::drawable::stat_sys_download_done
        }

        // Build action intents
        if (type == TYPE_ACTIVE || type == TYPE_WAITING) {
            // build a synthetic uri for intent identification purposes
            AutoPtr<IUriBuilder> uribuild;
            CUriBuilder::New((IUriBuilder**)&uribuild);
            uribuild->Scheme(String("active-dl"));
            uribuild->AppendPath(tag);
            AutoPtr<IUri> uri;
            uribuild->Build((IUri**)&uri);
            AutoPtr<IIntent> intent;
            CIntent::New(Constants::ACTION_LIST,
                    uri, mContext, TransformClassInfo(ECLSID_CDownloadReceiver), (IIntent**)&intent);
            intent->PutExtra(IDownloadManager::EXTRA_NOTIFICATION_CLICK_DOWNLOAD_IDS,
                    GetDownloadIds(cluster));
            AutoPtr<IPendingIntentHelper> hlp;
            CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&hlp);
            AutoPtr<IPendingIntent> pi;
            hlp->GetBroadcast(mContext,
                    0, intent, IPendingIntent::FLAG_UPDATE_CURRENT, (IPendingIntent**)&pi);
            builder->SetContentIntent(pi);
            builder->SetOngoing(TRUE);

        }
        else if (type == TYPE_COMPLETE) {
            AutoPtr<IIterator> it;
            cluster->GetIterator((IIterator**)&it);
            AutoPtr<IInterface> info;
            it->GetNext((IInterface**)&info);
            AutoPtr<CDownloadInfo> _info = (CDownloadInfo*)(IDownloadInfo::Probe(info));
            AutoPtr<IContentUris> cu;
            CContentUris::AcquireSingleton((IContentUris**)&cu);
            AutoPtr<IDownloadsImpl> impl;
            CDownloadsImpl::AcquireSingleton((IDownloadsImpl**)&impl);
            AutoPtr<IUri> uriDL;
            impl->GetALL_DOWNLOADS_CONTENT_URI((IUri**)&uriDL);
            AutoPtr<IUri> uri;
            cu->WithAppendedId(uriDL, _info->mId, (IUri**)&uri);
            builder->SetAutoCancel(TRUE);

            String action;
            Boolean bSE = FALSE;
            if ((impl->IsStatusError(_info->mStatus, &bSE), bSE)) {
                action = Constants::ACTION_LIST;
            }
            else {
                if (_info->mDestination != IDownloadsImpl::DESTINATION_SYSTEMCACHE_PARTITION) {
                    action = Constants::ACTION_OPEN;
                }
                else {
                    action = Constants::ACTION_LIST;
                }
            }

            AutoPtr<IIntent> intent;
            CIntent::New(action, uri, mContext, TransformClassInfo(ECLSID_CDownloadReceiver), (IIntent**)&intent);
            intent->PutExtra(IDownloadManager::EXTRA_NOTIFICATION_CLICK_DOWNLOAD_IDS,
                    GetDownloadIds(cluster));
            AutoPtr<IPendingIntentHelper> hlp;
            CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&hlp);
            AutoPtr<IPendingIntent> ci;
            hlp->GetBroadcast(mContext,
                    0, intent, IPendingIntent::FLAG_UPDATE_CURRENT, (IPendingIntent**)&ci);
            builder->SetContentIntent(ci);

            AutoPtr<IIntent> hideIntent;
            CIntent::New(Constants::ACTION_HIDE,
                    uri, mContext, TransformClassInfo(ECLSID_CDownloadReceiver), (IIntent**)&intent);
            AutoPtr<IPendingIntent> di;
            hlp->GetBroadcast(mContext, 0, hideIntent, 0, (IPendingIntent**)&di);
            builder->SetDeleteIntent(di);
        }

        // Calculate and show progress
        String remainingText(NULL);
        String percentText(NULL);
        if (type == TYPE_ACTIVE) {
            Int64 current = 0;
            Int64 total = 0;
            Int64 speed = 0;
            synchronized (mDownloadSpeedLock) {
                AutoPtr<IIterator> itClu;
                cluster->GetIterator((IIterator**)&itClu);
                Boolean bHasNxt = FALSE;
                while ((itClu->HasNext(&bHasNxt), bHasNxt)) {
                    AutoPtr<IInterface> p;
                    itClu->GetNext((IInterface**)&p);
                    AutoPtr<CDownloadInfo> info = (CDownloadInfo*)(IDownloadInfo::Probe(p));
                    if (info->mTotalBytes != -1) {
                        current += info->mCurrentBytes;
                        total += info->mTotalBytes;
                        Int64 curSpeed = 0;
                        mDownloadSpeed->Get(info->mId, &curSpeed);
                        speed += curSpeed;
                    }
                }
            }

            if (total > 0) {
                Int32 percent = (Int32) ((current * 100) / total);
                AutoPtr<IInteger64> pPercent;
                CInteger64::New(percent, (IInteger64**)&pPercent);
                AutoPtr<ArrayOf<IInteger64*> > arrPercent = ArrayOf<IInteger64*>::Alloc(1);
                (*arrPercent)[0] = pPercent;
                res->GetString(R::string::download_percent, (ArrayOf<IInterface*>*)arrPercent.Get(), &percentText);

                if (speed > 0) {
                    Int64 remainingMillis = ((total - current) * 1000) / speed;
                    AutoPtr<IDateUtils> du;
                    CDateUtils::AcquireSingleton((IDateUtils**)&du);
                    AutoPtr<ICharSequence> pcs;
                    du->FormatDuration(remainingMillis, (ICharSequence**)&pcs);
                    AutoPtr<ArrayOf<IInterface*> > arrCS = ArrayOf<IInterface*>::Alloc(1);
                    (*arrCS)[0] = pcs;
                    res->GetString(R::string::download_remaining,
                            arrCS, &remainingText);
                }

                builder->SetProgress(100, percent, FALSE);
            }
            else {
                builder->SetProgress(100, 0, TRUE);
            }
        }

        // Build titles and description
        Int32 s = 0;
        cluster->GetSize(&s);
        AutoPtr<INotification> notif;
        if (s == 1) {
            AutoPtr<IIterator> it;
            cluster->GetIterator((IIterator**)&it);
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<CDownloadInfo> info = (CDownloadInfo*)(IDownloadInfo::Probe(p));

            builder->SetContentTitle(GetDownloadTitle(res, info));

            if (type == TYPE_ACTIVE) {
                AutoPtr<ITextUtils> tu;
                CTextUtils::AcquireSingleton((ITextUtils**)&tu);
                Boolean bEmp = FALSE;
                if (!(tu->IsEmpty(info->mDescription, &bEmp), bEmp)) {
                    AutoPtr<ICharSequence> pDes;
                    CString::New(info->mDescription, (ICharSequence**)&pDes);
                    builder->SetContentText(pDes);
                }
                else {
                    AutoPtr<ICharSequence> pRT;
                    CString::New(remainingText, (ICharSequence**)&pRT);
                    builder->SetContentText(pRT);
                }
                AutoPtr<ICharSequence> pPT;
                CString::New(percentText, (ICharSequence**)&pPT);
                builder->SetContentInfo(pPT);

            }
            else if (type == TYPE_WAITING) {
                String str;
                res->GetString(R::string::notification_need_wifi_for_size, &str);
                AutoPtr<ICharSequence> pStr;
                CString::New(str, (ICharSequence**)&pStr);
                builder->SetContentText(pStr);

            }
            else if (type == TYPE_COMPLETE) {
                AutoPtr<IDownloadsImpl> impl;
                CDownloadsImpl::AcquireSingleton((IDownloadsImpl**)&impl);
                Boolean bStatErr = FALSE, bStatSucc = FALSE;
                if ((impl->IsStatusError(info->mStatus, &bStatErr), bStatErr)) {
                    AutoPtr<ICharSequence> cs;
                    res->GetText(R::string::notification_download_failed, (ICharSequence**)&cs);
                    builder->SetContentText(cs);
                }
                else if ((impl->IsStatusSuccess(info->mStatus, &bStatSucc), bStatSucc)) {
                    AutoPtr<ICharSequence> cs;
                    res->GetText(R::string::notification_download_complete, (ICharSequence**)&cs);
                    builder->SetContentText(cs);
                }
            }

            builder->Build((INotification**)&notif);

        }
        else {
            AutoPtr<INotificationInboxStyle> inboxStyle;
            CNotificationInboxStyle::New(builder, (INotificationInboxStyle**)&inboxStyle);

            AutoPtr<IIterator> itClu;
            cluster->GetIterator((IIterator**)&itClu);
            Boolean bHasNxt = FALSE;
            while ((itClu->HasNext(&bHasNxt), bHasNxt)) {
                AutoPtr<IInterface> p;
                itClu->GetNext((IInterface**)&p);
                AutoPtr<IDownloadInfo> info = IDownloadInfo::Probe(p);
                inboxStyle->AddLine(GetDownloadTitle(res, info));
            }

            Int32 size = 0;
            cluster->GetSize(&size);
            AutoPtr<IInteger32> pSize;
            CInteger32::New(size, (IInteger32**)&pSize);
            AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
            (*args)[0] = pSize;
            if (type == TYPE_ACTIVE) {
                String str;
                res->GetQuantityString(
                        R::plurals::notif_summary_active, size, args, &str);
                AutoPtr<ICharSequence> pStr;
                CString::New(str, (ICharSequence**)&pStr);
                builder->SetContentTitle(pStr);

                AutoPtr<ICharSequence> pRemainingText;
                CString::New(remainingText, (ICharSequence**)&pRemainingText);
                builder->SetContentText(pRemainingText);

                AutoPtr<ICharSequence> pPercentText;
                CString::New(percentText, (ICharSequence**)&pPercentText);
                builder->SetContentInfo(pPercentText);

                inboxStyle->SetSummaryText(pRemainingText);

            }
            else if (type == TYPE_WAITING) {
                String str;
                res->GetQuantityString(
                        R::plurals::notif_summary_waiting, size, args, &str);
                AutoPtr<ICharSequence> pStr;
                CString::New(str, (ICharSequence**)&pStr);
                builder->SetContentTitle(pStr);
                String conText;
                res->GetString(R::string::notification_need_wifi_for_size, &conText);
                AutoPtr<ICharSequence> pConText;
                CString::New(conText, (ICharSequence**)&pConText);
                builder->SetContentText(pConText);
                String sumText;
                res->GetString(R::string::notification_need_wifi_for_size, &sumText);
                AutoPtr<ICharSequence> pSumText;
                CString::New(sumText, (ICharSequence**)&pSumText);
                inboxStyle->SetSummaryText(pSumText);
            }

            INotificationStyle::Probe(inboxStyle)->Build((INotification**)&notif);
        }

        mNotifManager->Notify(tag, 0, notif);
    }

    // Remove stale tags that weren't renewed
    AutoPtr<ISet> ks;
    mActiveNotifs->GetKeySet((ISet**)&ks);
    AutoPtr<IIterator> itKs;
    ks->GetIterator((IIterator**)&itKs);
    Boolean bNxt = FALSE;
    while ((itKs->HasNext(&bNxt), bNxt)) {
        AutoPtr<IInterface> tag;
        itKs->GetNext((IInterface**)&tag);
        assert(0 && "TODO");
        // if (!clustered->ContainsKey(tag)) {
        //     mNotifManager->Cancel(tag, 0);
        //     itKs->Remove();
        // }
    }
}

AutoPtr<ICharSequence> CDownloadNotifier::GetDownloadTitle(
    /* [in] */ IResources* res,
    /* [in] */ IDownloadInfo* info)
{
    AutoPtr<CDownloadInfo> _info = (CDownloadInfo*)info;
    AutoPtr<ITextUtils> tu;
    CTextUtils::AcquireSingleton((ITextUtils**)&tu);
    Boolean bEmp = FALSE;
    if (!(tu->IsEmpty(_info->mTitle, &bEmp), bEmp)) {
        AutoPtr<ICharSequence> pRes;
        CString::New(_info->mTitle, (ICharSequence**)&pRes);
        return pRes;
    }
    else {
        String cs;
        res->GetString(R::string::download_unknown_title, &cs);
        AutoPtr<ICharSequence> pRes;
        CString::New(cs, (ICharSequence**)&pRes);
        return pRes;
    }
}

AutoPtr<ArrayOf<Int64> > CDownloadNotifier::GetDownloadIds(
    /* [in] */ ICollection* infos)
{
    Int32 size = 0;
    infos->GetSize(&size);
    AutoPtr<ArrayOf<Int64> > ids = ArrayOf<Int64>::Alloc(size);
    AutoPtr<IIterator> it;
    infos->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    Int32 i = 0;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<CDownloadInfo> info = (CDownloadInfo*)(IDownloadInfo::Probe(p));
        (*ids)[i++] = info->mId;
    }
    return ids;
}

ECode CDownloadNotifier::DumpSpeeds()
{
    synchronized (mDownloadSpeedLock) {
        Int32 size = 0;
        mDownloadSpeed->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            Int32 id = 0;
            mDownloadSpeed->KeyAt(i, &id);
            Int64 dt = 0;
            mDownloadTouch->Get(id, &dt);
            Int64 delta = SystemClock::GetElapsedRealtime() - dt;
            Int64 va = 0;
            mDownloadSpeed->ValueAt(i, &va);
            Logger::D("CDownloadNotifier", "Download %d speed %pbps, %dms ago",
                id, va, delta);
        }
    }
    return NOERROR;
}

String CDownloadNotifier::BuildNotificationTag(
    /* [in] */ IDownloadInfo* info)
{
    AutoPtr<CDownloadInfo> _info = (CDownloadInfo*)info;
    if (_info->mStatus == IDownloadsImpl::STATUS_QUEUED_FOR_WIFI) {
        String res;
        res += TYPE_WAITING;
        res += String(":");
        res += _info->mPackage;
        return res;
    }
    else if (IsActiveAndVisible(info)) {
        String res;
        res += TYPE_ACTIVE;
        res += String(":");
        res += _info->mPackage;
        return res;
    }
    else if (IsCompleteAndVisible(info)) {
        // Complete downloads always have unique notifs
        String res;
        res += TYPE_COMPLETE;
        res += String(":");
        res += _info->mId;
        return res;
    }
    else {
        return String(NULL);
    }
}

Int32 CDownloadNotifier::GetNotificationTagType(
    /* [in] */ String tag)
{
    return StringUtils::ParseInt32(tag.Substring(0, tag.IndexOf(':')));
}

Boolean CDownloadNotifier::IsActiveAndVisible(
    /* [in] */ IDownloadInfo* download)
{
    AutoPtr<CDownloadInfo> _download = (CDownloadInfo*)download;
    return _download->mStatus == IDownloadsImpl::STATUS_RUNNING &&
            (_download->mVisibility == IDownloadManagerRequest::VISIBILITY_VISIBLE
            || _download->mVisibility == IDownloadManagerRequest::VISIBILITY_VISIBLE_NOTIFY_COMPLETED);
}

Boolean CDownloadNotifier::IsCompleteAndVisible(
    /* [in] */ IDownloadInfo* download)
{
    AutoPtr<CDownloadInfo> _download = (CDownloadInfo*)download;
    AutoPtr<IDownloadsImpl> impl;
    CDownloadsImpl::AcquireSingleton((IDownloadsImpl**)&impl);
    Boolean bComp = FALSE;
    return (impl->IsStatusCompleted(_download->mStatus, &bComp), bComp) &&
            (_download->mVisibility == IDownloadManagerRequest::VISIBILITY_VISIBLE_NOTIFY_COMPLETED
            || _download->mVisibility == IDownloadManagerRequest::VISIBILITY_VISIBLE_NOTIFY_ONLY_COMPLETION);
}

} // namespace Downloads
} // namespace DownloadProvider
} // namespace Providers
} // namespace Elastos
