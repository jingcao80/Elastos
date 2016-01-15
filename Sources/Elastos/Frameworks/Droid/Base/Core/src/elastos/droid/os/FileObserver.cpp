
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/FileObserver.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

#include <sys/inotify.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Core::CThread;
using Elastos::Core::EIID_IRunnable;

namespace Elastos {
namespace Droid {
namespace Os {

FileObserver::ObserverThread::ObserverThread()
{
    Thread::constructor(String("FileObserver"));
    mFd = NativeInit();
}

ECode FileObserver::ObserverThread::Run()
{
    NativeObserve(mFd);
    return NOERROR;
}

Int32 FileObserver::ObserverThread::StartWatching(
    /* [in] */ const String& path,
    /* [in] */ Int32 mask,
    /* [in] */ FileObserver* observer)
{
    Int32 wfd = NativeStartWatching(mFd, path, mask);
    if (wfd >= 0) {
        AutoLock Lock(mObserverslock);
        AutoPtr<IWeakReference> wr;
        observer->GetWeakReference((IWeakReference**)&wr);
        mObservers[wfd] = wr;
    }
    return wfd;
}

void FileObserver::ObserverThread::StopWatching(
    /* [in] */ Int32 descriptor)
{
    NativeStopWatching(mFd, descriptor);
}

ECode FileObserver::ObserverThread::OnEvent(
    /* [in] */ Int32 wfd,
    /* [in] */ Int32 mask,
    /* [in] */ const String& path)
{
    // look up our observer, fixing up the map if necessary...
    AutoPtr<IFileObserver> observer;
    {
        AutoLock Lock(mObserverslock);
        AutoPtr<IWeakReference> weak;
        HashMap<Int32, AutoPtr<IWeakReference> >::Iterator it = mObservers.Find(wfd);
        if (it != mObservers.End()) {
            weak = it->mSecond;
        }
        if (weak != NULL) {  // can happen with lots of events from a dead wfd
            weak->Resolve(EIID_IFileObserver, (IInterface**)&observer);
            if (observer == NULL) {
                mObservers.Erase(it);
            }
        }
    }

    // ...then call out to the observer without the sync lock held
    if (observer != NULL) {
        //try {
        return observer->OnEvent(mask, path);
        //} catch (Throwable throwable) {
        //    Log.wtf(LOG_TAG, "Unhandled exception in FileObserver " + observer, throwable);
        //}
    }
    return NOERROR;
}

Int32 FileObserver::ObserverThread::NativeInit()
{
    return inotify_init();
}

void FileObserver::ObserverThread::NativeObserve(
    /* [in] */ Int32 fd)
{
    char event_buf[512];
    struct inotify_event* event;

    while (1)
    {
        int event_pos = 0;
        int num_bytes = read(fd, event_buf, sizeof(event_buf));

        if (num_bytes < (int)sizeof(*event))
        {
            if (errno == EINTR)
                continue;

            Logger::E("ObserverThread","***** ERROR! android_os_fileobserver_observe() got a short event!");
            return;
        }

        while (num_bytes >= (int)sizeof(*event))
        {
            int event_size;
            event = (struct inotify_event *)(event_buf + event_pos);

            String path;

            if (event->len > 0)
            {
                path = String(event->name);
            }

            //env->CallVoidMethod(object, method_onEvent, event->wd, event->mask, path);
            OnEvent(event->wd, event->mask, path);
            // if (env->ExceptionCheck()) {
            //     env->ExceptionDescribe();
            //     env->ExceptionClear();
            // }
            if (!path.IsNull())
            {
                // env->DeleteLocalRef(path);
            }

            event_size = sizeof(*event) + event->len;
            num_bytes -= event_size;
            event_pos += event_size;
        }
    }
}

Int32 FileObserver::ObserverThread::NativeStartWatching(
    /* [in] */ Int32 fd,
    /* [in] */ const String& pathString,
    /* [in] */ Int32 mask)
{
    int res = -1;
    if (fd >= 0)
    {
        const char* path = pathString.string();

        res = inotify_add_watch(fd, path, mask);
    }
    return res;
}

void FileObserver::ObserverThread::NativeStopWatching(
    /* [in] */ Int32 fd,
    /* [in] */ Int32 wfd)
{
    inotify_rm_watch((int)fd, (uint32_t)wfd);
}

//========================================================
// FileObserver
//========================================================
CAR_INTERFACE_IMPL(FileObserver, Object, IFileObserver)

Boolean FileObserver::InitObserverThread()
{
    sObserverThread = new ObserverThread();
    sObserverThread->Start();
    return TRUE;
}

AutoPtr<FileObserver::ObserverThread> FileObserver::sObserverThread;
const Boolean FileObserver::sObserverThreadInited = FileObserver::InitObserverThread();

FileObserver::FileObserver()
    : mDescriptor(0)
    , mMask(0)
{}

FileObserver::FileObserver(
    /* [in] */ const String& path,
    /* [in] */ Int32 mask)
    : mPath(path)
    , mDescriptor(-1)
    , mMask(mask)
{}

FileObserver::~FileObserver()
{
    StopWatching();
}

ECode FileObserver::constructor(
    /* [in] */ const String& path)
{
    return constructor(path, IFileObserver::ALL_EVENTS);
}

ECode FileObserver::constructor(
    /* [in] */ const String& path,
    /* [in] */ Int32 mask)
{
    mPath = path;
    mMask = mask;
    mDescriptor = -1;
    return NOERROR;
}

ECode FileObserver::StartWatching()
{
    if (mDescriptor < 0) {
        mDescriptor = sObserverThread->StartWatching(mPath, mMask, this);
    }
    return NOERROR;
}

ECode FileObserver::StopWatching()
{
    if (mDescriptor >= 0) {
        sObserverThread->StopWatching(mDescriptor);
        mDescriptor = -1;
    }
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos

