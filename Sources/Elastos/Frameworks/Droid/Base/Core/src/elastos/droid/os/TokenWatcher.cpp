
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/TokenWatcher.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/etl/Set.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Etl::Set;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Os {

ECode TokenWatcher::NotificationTaskRunnable::Run()
{
    mHost->NotificationTask();
    return NOERROR;
}

TokenWatcher::TokenWatcher(
    /* [in] */ IHandler* h,
    /* [in] */ const String& tag)
    : mHandler(h)
    , mTag( (tag != NULL) ? tag : String("TokenWatcher") )
    , mNotificationQueue(-1)
    , mAcquired(FALSE)
{
    mNotificationTask = new NotificationTaskRunnable(this);
}

ECode TokenWatcher::AcquireToken(
    /* [in] */ IBinder* token,
    /* [in] */ const String& tag)
{
    AutoLock lock(mLock);
    // explicitly checked to avoid bogus sendNotification calls because
    // of the WeakHashMap and the GC
    Int32 oldSize = mTokens.GetSize();

    AutoPtr<Death> d = new Death(token, tag, this);
    //try {
    AutoPtr<IProxy> proxy = (IProxy*)token->Probe(EIID_IProxy);
    if (proxy != NULL) {
        proxy->LinkToDeath(d, 0);
    }
    //} catch (RemoteException e) {
    //    return;
    //}
    mTokens[token] = d;

    if (oldSize == 0 && !mAcquired) {
        SendNotificationLocked(TRUE);
        mAcquired = TRUE;
    }
    return NOERROR;
}

ECode TokenWatcher::Cleanup(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean unlink)
{
    AutoLock lock(mLock);
    AutoPtr<Death> d = mTokens[token];
    mTokens.Erase(token);
    if (unlink && d != NULL) {
        AutoPtr<IProxy> proxy = (IProxy*)(d->mToken)->Probe(EIID_IProxy);
        if (proxy != NULL) {
            Boolean result;
            proxy->UnlinkToDeath(d, 0, &result);
        }
        d->mToken = NULL;
    }

    if (mTokens.IsEmpty() && mAcquired) {
        SendNotificationLocked(FALSE);
        mAcquired = FALSE;
    }
    return NOERROR;
}

ECode TokenWatcher::ReleaseToken(
    /* [in] */ IBinder* token)
{
    return Cleanup(token, TRUE);
}

ECode TokenWatcher::IsAcquired(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    {
        AutoLock lock(mLock);
        *result = mAcquired;
    }
    return NOERROR;
}

ECode TokenWatcher::Dump()
{
    AutoPtr< List<String> > a = DumpInternal();
    List<String>::Iterator iter = a->Begin();
    String s;
    for (; iter != a->End(); ++iter) {
        s = *iter;
        Logger::I(mTag, s.string());
    }
    delete a;
    return NOERROR;
}

ECode TokenWatcher::Dump(
    /* [in] */ IPrintWriter* pw)
{
    AutoPtr< List<String> > a = DumpInternal();
    List<String>::Iterator iter = a->Begin();
    String s;
    for (; iter != a->End(); ++iter) {
        pw->Println(s);
    }
    delete a;
    return NOERROR;
}

AutoPtr< List<String> > TokenWatcher::DumpInternal()
{
    AutoPtr< List<String> > a = new List<String>;
    {
        AutoLock lock(mLock);
        Set<AutoPtr<IBinder> > keys;
        HashMap<AutoPtr<IBinder>, AutoPtr<Death> >::Iterator iter = mTokens.Begin();
        for (; iter != mTokens.End(); ++iter) {
            keys.Insert(iter->mFirst);
        }
        StringBuilder sb("Token count: ");
        sb += mTokens.GetSize();
        a->PushBack(sb.ToString());
        Int32 i = 0;
        Set<AutoPtr<IBinder> >::Iterator iter2 = keys.Begin();
        AutoPtr<IBinder> b;
        for (; iter2 != keys.End(); ++iter) {
            StringBuilder sb2("[");
            sb2 += i;
            sb2 += "] ";
            sb2 += mTokens[b]->mTag;
            sb2 += " - ";
            String bstr;
            b->ToString(&bstr);
            sb2 += bstr;
            a->PushBack(sb2.ToString());
            i++;
        }
    }
    return a;
}

ECode TokenWatcher::NotificationTask()
{
    Int32 value;
    {
        AutoLock lock(mLock);
        value = mNotificationQueue;
        mNotificationQueue = -1;
    }
    if (value == 1) {
        Acquired();
    }
    else if (value == 0) {
        Released();
    }
    return NOERROR;
}

void TokenWatcher::SendNotificationLocked(
    /* [in] */ Boolean on)
{
    Int32 value = on ? 1 : 0;
    if (mNotificationQueue == -1) {
        // empty
        mNotificationQueue = value;
        Boolean bval;
        mHandler->Post(mNotificationTask, &bval);
    }
    else if (mNotificationQueue != value) {
        // it's a pair, so cancel it
        mNotificationQueue = -1;
        mHandler->RemoveCallbacks(mNotificationTask);
    }
    // else, same so do nothing -- maybe we should warn?
}

TokenWatcher::Death::Death(
    /* [in] */ IBinder* token,
    /* [in] */ const String& tag,
    /* [in] */ TokenWatcher* host)
        : mToken(token)
        , mTag(tag)
        , mHost(host)
{
}

CAR_INTERFACE_IMPL(TokenWatcher::Death, Object, IProxyDeathRecipient);

ECode TokenWatcher::Death::ProxyDied()
{
    return mHost->Cleanup(mToken, FALSE);
}

TokenWatcher::Death::~Death()
{
    // try {
    if (mToken != NULL) {
        Logger::W(mHost->mTag, "cleaning up leaked reference: ", mTag.string());
        mHost->ReleaseToken(mToken);
    }
    // }
    // finally {
    //     super.finalize();
    // }
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
