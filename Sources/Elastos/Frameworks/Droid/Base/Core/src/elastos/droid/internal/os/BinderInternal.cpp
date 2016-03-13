#include "elastos/droid/internal/os/BinderInternal.h"
#include <Elastos.Droid.Os.h>
#include <Elastos.CoreLibrary.Utility.h>

#include <binder/IInterface.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>

using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

// native code from android_util_Binder.cpp
static volatile int32_t gNumRefsCreated = 0;

AutoPtr<Elastos::Droid::Os::IBinder> ElastosObjectForIBinder(const android::sp<android::IBinder>& val)
{
    assert(0 && "Should not happen in Elastos.");
    return NULL;
}

// AutoPtr<IArrayList> InitGcWatchers()
// {
//     AutoPtr<IArrayList> list;
//     CArrayList::New((IArrayList**)&list);
//     return list;
// }

// AutoPtr<IWeakReference> BinderInternal::sGcWatcher; //new WeakReference<GcWatcher>(new GcWatcher());

// AutoPtr<IArrayList> BinderInternal::sGcWatchers = InitGcWatchers();
// AutoPtr<ArrayOf<IInterface*> > BinderInternal::sTmpWatchers = ArrayOf<IInterface*> ::Alloc(1);
// Int64 BinderInternal::sLastGcTime = 0;

BinderInternal::GcWatcher::~GcWatcher()
{
    Finalize();
}

ECode BinderInternal::GcWatcher::Finalize()
{
    // handleGc();
    // sLastGcTime = SystemClock.uptimeMillis();
    // synchronized (sGcWatchers) {
    //     sTmpWatchers = sGcWatchers.toArray(sTmpWatchers);
    // }
    // for (int i=0; i<sTmpWatchers.length; i++) {
    //     if (sTmpWatchers[i] != null) {
    //         sTmpWatchers[i].run();
    //     }
    // }
    // sGcWatcher = new WeakReference<GcWatcher>(new GcWatcher());
    return NOERROR;
}

ECode BinderInternal::AddGcWatcher(
    /* [in] */ IRunnable* watcher)
{
    // synchronized (sGcWatchers) {
    //     sGcWatchers.add(watcher);
    // }
    return NOERROR;
}

ECode BinderInternal::JoinThreadPool()
{
    android::sp<android::IBinder> b = android::ProcessState::self()->getContextObject(NULL);
    android::IPCThreadState::self()->joinThreadPool();
    return NOERROR;
}

Int64 BinderInternal::GetLastGcTime()
{
    // return sLastGcTime;
    return 0;
}

AutoPtr<Elastos::Droid::Os::IBinder> BinderInternal::GetContextObject()
{
    android::sp<android::IBinder> b = android::ProcessState::self()->getContextObject(NULL);
    return ElastosObjectForIBinder(b);
}

ECode BinderInternal::DisableBackgroundScheduling(
    /* [in] */ Boolean disable)
{
    android::IPCThreadState::disableBackgroundScheduling(disable ? true : false);
    return NOERROR;
}

ECode BinderInternal::HandleGc()
{
    // ALOGV("Gc has executed, clearing binder ops");
    // android_atomic_and(0, &gNumRefsCreated);
    return NOERROR;
}

ECode BinderInternal::ForceGc(
    /* [in] */ const String& reason)
{
    // EventLog.writeEvent(2741, reason);
    // Runtime.getRuntime().gc();
    return NOERROR;
}

ECode BinderInternal::ForceBinderGc()
{
    return ForceGc(String("Binder"));
}


} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos
