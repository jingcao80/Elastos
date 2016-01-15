
#include "elastos/droid/media/CMediaScannerConnection.h"
#include "elastos/droid/content/CComponentName.h"
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/content/CIntent.h"
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CComponentName;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Media {

const String CMediaScannerConnection::TAG("MediaScannerConnection");

//---------------------------------------------------
//    CMediaScannerConnection::MyMediaScannerListener
//---------------------------------------------------

CAR_INTERFACE_IMPL(CMediaScannerConnection::MyMediaScannerListener, Object, IIMediaScannerListener);

CMediaScannerConnection::MyMediaScannerListener::MyMediaScannerListener(
    /* [in] */ IWeakReference* host)
    : mWeakHost(host)
{}

ECode CMediaScannerConnection::MyMediaScannerListener::ScanCompleted(
    /* [in] */ const String& path,
    /* [in] */ IUri* uri)
{
    AutoPtr<IMediaScannerConnection> connection;
    mWeakHost->Resolve(EIID_IMediaScannerConnection, (IInterface**)&connection);
    if (connection) {
        CMediaScannerConnection* msc = (CMediaScannerConnection*)connection.Get();
        AutoPtr<IMediaScannerConnectionClient> client = msc->mClient;
        if (client != NULL) {
            IOnScanCompletedListener::Probe(client)->OnScanCompleted(path, uri);
        }
    }
    return NOERROR;
}

//----------------------------------------
//    CMediaScannerConnection::ClientProxy
//----------------------------------------

CAR_INTERFACE_IMPL(CMediaScannerConnection::ClientProxy, Object, IMediaScannerConnectionClient);

CMediaScannerConnection::ClientProxy::ClientProxy(
    /* [in] */ ArrayOf<String>* paths,
    /* [in] */ ArrayOf<String>* mimeTypes,
    /* [in] */ IOnScanCompletedListener* client)
    : mPaths(paths)
    , mMimeTypes(mimeTypes)
    , mClient(client)
    , mNextPath(0)
{}

ECode CMediaScannerConnection::ClientProxy::OnMediaScannerConnected()
{
    ScanNextPath();
    return NOERROR;
}

ECode CMediaScannerConnection::ClientProxy::OnScanCompleted(
    /* [in] */ const String& path,
    /* [in] */ IUri* uri)
{
    if (mClient != NULL) {
        mClient->OnScanCompleted(path, uri);
    }
    ScanNextPath();
    return NOERROR;
}

void CMediaScannerConnection::ClientProxy::ScanNextPath()
{
    if (mNextPath >= mPaths->GetLength()) {
        mConnection->Disconnect();
        return;
    }

    String mimeType;
    if (mMimeTypes != NULL && mNextPath < mMimeTypes->GetLength()) {
        mimeType = (*mMimeTypes)[mNextPath];
    }

    mConnection->ScanFile((*mPaths)[mNextPath], mimeType);
    mNextPath++;
}

//----------------------------------------
//    CMediaScannerConnection
//----------------------------------------
CAR_OBJECT_IMPL(CMediaScannerConnection)

CAR_INTERFACE_IMPL(CMediaScannerConnection, Object, IMediaScannerConnection)

CMediaScannerConnection::CMediaScannerConnection()
    : mConnected(FALSE)
{
}

ECode CMediaScannerConnection::constructor(
    /* [in] */ IContext * context,
    /* [in] */ IMediaScannerConnectionClient* client)
{
    mContext = context;
    mClient = client;
    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    mListener = new MyMediaScannerListener(wr);
    return NOERROR;
}

ECode CMediaScannerConnection::Connect()
{
   AutoLock lock(mThisLock);

   if (!mConnected) {
       AutoPtr<IIntent> intent;
       // CIntent::New(MediaScannerService.class.getName(), (IIntent**)&intent);
       CIntent::New(String("IIMediaScannerService"), (IIntent**)&intent);
       AutoPtr<IComponentName> component;
       CComponentName::New(String("com.android.providers.media"),
            String("com.android.providers.media.MediaScannerService"), (IComponentName**)&component);
       intent->SetComponent(component.Get());
       Boolean tempState;
       mContext->BindService(intent.Get(), IServiceConnection::Probe(this), IContext::BIND_AUTO_CREATE, &tempState);
       mConnected = TRUE;
   }

    return NOERROR;
}

ECode CMediaScannerConnection::Disconnect()
{
    AutoLock lock(mThisLock);

    ECode ec = NOERROR;
    if (mConnected) {
        if (FALSE) {
            Slogger::V(TAG, "Disconnecting from Media Scanner");
        }

//      try {
        ec = mContext->UnbindService(IServiceConnection::Probe(this));
    //      } catch (IllegalArgumentException ex) {
        if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
            if (FALSE) {
                Slogger::V(TAG, "disconnect failed: " /*+ ex*/);
            }
            ec = NOERROR;
        }
    //      }
        mConnected = FALSE;
    }

    return ec;
}

/*synchronized*/
ECode CMediaScannerConnection::IsConnected(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock lock(mThisLock);

    *result = mService != NULL && mConnected;
    return NOERROR;
}

ECode CMediaScannerConnection::ScanFile(
    /* [in] */ const String& path,
    /* [in] */ const String& mimeType)
{
    AutoLock lock(mThisLock);

    if (mService == NULL || !mConnected) {
        Slogger::E(TAG, "not connected to MediaScannerService");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

//        try {
    if (FALSE) {
        Slogger::V(TAG, "Scanning file %s", path.string());
    }
    ECode ec = mService->RequestScanFile(path, mimeType, mListener);
//        } catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        if (FALSE) {
            Slogger::D(TAG, "Failed to scan file %s", path.string());
        }
        ec = NOERROR;
    }
//        }

    return ec;
}

ECode CMediaScannerConnection::OnServiceConnected(
    /* [in] */ IComponentName* className,
    /* [in] */ IBinder* service)
{
    if (FALSE) {
        Slogger::V(TAG, "Connected to Media Scanner");
    }

    synchronized(this) {
        mService = IIMediaScannerService::Probe((IObject*)service);
        if (mService != NULL && mClient != NULL) {
            mClient->OnMediaScannerConnected();
        }
    }

    return NOERROR;
}

ECode CMediaScannerConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* className)
{
    if (FALSE) {
        Slogger::V(TAG, "Disconnected from Media Scanner");
    }

    AutoLock lock(mThisLock);

    mService = NULL;

    return NOERROR;
}

/*static */
ECode CMediaScannerConnection::ScanFile(
    /* [in] */ IContext* context,
    /* [in] */ ArrayOf<String>* paths,
    /* [in] */ ArrayOf<String>* mimeTypes,
    /* [in] */ IOnScanCompletedListener* cb)
{
    AutoPtr<ClientProxy> client = new ClientProxy(paths, mimeTypes, cb);
    AutoPtr<CMediaScannerConnection> connection;
    CMediaScannerConnection::NewByFriend(context, client, (CMediaScannerConnection**)&connection);
    client->mConnection = (IMediaScannerConnection*)connection.Get();
    connection->Connect();
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
