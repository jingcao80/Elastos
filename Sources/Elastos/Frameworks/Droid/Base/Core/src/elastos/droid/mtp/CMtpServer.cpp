
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/mtp/CMtpServer.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/Thread.h>
#include <elastos/core/AutoLock.h>

#include "MediaMtp/MtpServer.h"
#include "MediaMtp/MtpStorage.h"
#include <fcntl.h>

using namespace android;
using Elastos::Core::AutoLock;
using Elastos::Core::IThread;
using Elastos::Core::CThread;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Mtp {

static const String TAG("CMtpServer");

CMtpServer::CMtpServer()
    : mNativeContext(0)
{}

CAR_INTERFACE_IMPL_2(CMtpServer, Object, IRunnable, IMtpServer)

CAR_OBJECT_IMPL(CMtpServer)

ECode CMtpServer::constructor(
    /* [in] */ IMtpDatabase* database,
    /* [in] */ Boolean usePtp)
{
    NativeSetup(database, usePtp);
    database->SetServer(IMtpServer::Probe(this));
    return NOERROR;
}

ECode CMtpServer::Start()
{
    AutoPtr<IThread> thread;
    CThread::New(this, String("MtpServer"), (IThread**)&thread);
    thread->Start();
    return NOERROR;
}

ECode CMtpServer::Run()
{
    NativeRun();
    NativeCleanup();

    return NOERROR;
}

ECode CMtpServer::SendObjectAdded(
    /* [in] */ Int32 handle)
{
    NativeSendObjectAdded(handle);

    return NOERROR;
}

ECode CMtpServer::SendObjectRemoved(
    /* [in] */ Int32 handle)
{
    NativeSendObjectRemoved(handle);

    return NOERROR;
}

ECode CMtpServer::SendDevicePropertyChanged(
    /* [in] */ Int32 property)
{
    NativeSendDevicePropertyChanged(property);

    return NOERROR;
}

ECode CMtpServer::AddStorage(
    /* [in] */ IMtpStorage* storage)
{
    NativeAddStorage(storage);

    return NOERROR;
}

ECode CMtpServer::RemoveStorage(
    /* [in] */ IMtpStorage* storage)
{
    Int32 id;
    storage->GetStorageId(&id);
    NativeRemoveStorage(id);

    return NOERROR;
}

//JNI code
void CMtpServer::NativeSetup(
    /* [in] */ IMtpDatabase* database,
    /* [in] */ Boolean usePtp)
{
    Int32 fd = open("/dev/mtp_usb", O_RDWR);
    if (fd >= 0) {
        Int64 db;
        database->GetNativeContext(&db);
        MtpServer* server = new MtpServer(fd, (MtpDatabase*)db,
            usePtp, 1015/*AID_MEDIA_RW*/, 0664, 0775);
        mNativeContext = (Int32)server;
    }
    else {
        Logger::E(TAG, "could not open MTP driver, errno: %d", errno);
    }
}

void CMtpServer::NativeRun()
{
    MtpServer* server = (MtpServer*)mNativeContext;
    if (server)
        server->run();
    else {
        Logger::E(TAG, "server is null in run");
    }
}

void CMtpServer::NativeCleanup()
{
    Elastos::Core::AutoLock autoLock(mutex);

    MtpServer* server = (MtpServer*)mNativeContext;
    if (server) {
        delete server;
        mNativeContext = 0;
    }
    else {
        Logger::E(TAG, "server is null in cleanup");
    }
}

void CMtpServer::NativeSendObjectAdded(
    /* [in] */ Int32 handle)
{
    Elastos::Core::AutoLock autoLock(mutex);

    MtpServer* server = (MtpServer*)mNativeContext;
    if (server)
        server->sendObjectAdded(handle);
    else {
        Logger::E(TAG, "server is null in send_object_added");
    }
}

void CMtpServer::NativeSendObjectRemoved(
    /* [in] */ Int32 handle)
{
    Elastos::Core::AutoLock autoLock(mutex);

    MtpServer* server = (MtpServer*)mNativeContext;
    if (server)
        server->sendObjectRemoved(handle);
    else {
        Logger::E(TAG, "server is null in send_object_removed");
    }
}

void CMtpServer::NativeSendDevicePropertyChanged(
    /* [in] */ Int32 property)
{
    Elastos::Core::AutoLock autoLock(mutex);

    MtpServer* server = (MtpServer*)mNativeContext;
    if (server)
        server->sendDevicePropertyChanged(property);
    else {
        Logger::E(TAG, "server is null in send_object_removed");
    }
}

void CMtpServer::NativeAddStorage(
    /* [in] */ IMtpStorage* storage)
{
    Elastos::Core::AutoLock autoLock(mutex);

    MtpServer* server = (MtpServer*)mNativeContext;
    if (server) {
        Int32 storageId;
        storage->GetStorageId(&storageId);
        String path;
        storage->GetPath(&path);
        String description;
        storage->GetDescription(&description);
        Int64 reserveSpace;
        storage->GetReserveSpace(&reserveSpace);
        Boolean removable;
        storage->IsRemovable(&removable);
        Int64 maxFileSize;
        storage->GetMaxFileSize(&maxFileSize);

        const char *pathStr = path.string();
        if (pathStr != NULL) {
            const char *descriptionStr = description.string();
            if (descriptionStr != NULL) {
                MtpStorage* storage = new MtpStorage(storageId, pathStr, descriptionStr,
                        reserveSpace, removable, maxFileSize);
                server->addStorage(storage);
            }
        }
    }
    else {
        Logger::E(TAG, "server is null in add_storage");
    }
}

void CMtpServer::NativeRemoveStorage(
    /* [in] */ Int32 storageId)
{
    Elastos::Core::AutoLock autoLock(mutex);

    MtpServer* server = (MtpServer*)mNativeContext;
    if (server) {
        MtpStorage* storage = server->getStorage(storageId);
        if (storage) {
            server->removeStorage(storage);
            delete storage;
        }
    }
    else {
        Logger::E(TAG, "server is null in remove_storage");
    }
}


} // namespace Mtp
} // namepsace Droid
} // namespace Elastos
