
#include <Elastos.CoreLibrary.Apache.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/webkit/native/media/MediaDrmBridge.h"
#include "elastos/droid/webkit/native/media/api/MediaDrmBridge_dec.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/CHandler.h"
//TODO #include "elastos/droid/media/CMediaCrypto.h"
//TODO #include "elastos/droid/meida/CMediaDrmHelper.h"
//TODO #include "elastos/droid/meida/CMediaDrm.h"
//TODO #include "elastos/droid/media/CMediaCryptoHelper.h"

//TODO #include <elastos/io/CByteBufferHelper.h>
//TODO #include <elastos/utility/CArrayDeque.h>
//TODO #include <elastos/utility/CHashMap.h>
//TODO #include <elastos/utility/CUUID.h>
#include <elastos/utility/logging/Logger.h>

//TODO #include <org/apache/http/util/EntityUtils.h>
//TODO #include <org/apache/http/impl/client/DefaultHttpClient.h>
//TODO #include <org/apache/http/client/methods/CHttpPost.h>

//TODO using Elastos::Droid::Media::CMediaDrm;
using Elastos::Droid::Media::IMediaDrmHelper;
//TODO using Elastos::Droid::Media::CMediaDrmHelper;
//TODO using Elastos::Droid::Media::CMediaCrypto;
using Elastos::Droid::Media::IMediaCryptoHelper;
//TODO using Elastos::Droid::Media::CMediaCryptoHelper;
using Elastos::Droid::Media::IMediaDrmKeyRequest;
using Elastos::Droid::Media::IMediaDrmProvisionRequest;

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::IO::ByteOrder_BIG_ENDIAN;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::CByteBufferHelper;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::CHandler;
using Elastos::Utility::IArrayDeque;
using Elastos::Utility::IDeque;
using Elastos::Utility::CArrayDeque;
using Elastos::Utility::IHashMap;
//TODO using Elastos::Utility::CHashMap;
using Elastos::Utility::IUUID;
//TODO using Elastos::Utility::CUUID;
using Elastos::Utility::Logging::Logger;

using Org::Apache::Http::IHttpResponse;
using Org::Apache::Http::IHttpRequest;
using Org::Apache::Http::Client::Methods::IHttpUriRequest;
using Org::Apache::Http::IRequestLine;
using Org::Apache::Http::IStatusLine;
using Org::Apache::Http::Client::IHttpClient;
using Org::Apache::Http::Client::Methods::IHttpPost;
using Org::Apache::Http::IHttpMessage;
//TODO using Org::Apache::Http::Impl::Client::DefaultHttpClient;
//TODO using Org::Apache::Http::Utility::EntityUtils;
using Org::Apache::Http::IHttpEntity;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Media {

//===============================================================
//          MediaDrmBridge::PendingCreateSessionData
//===============================================================

MediaDrmBridge::PendingCreateSessionData::PendingCreateSessionData(
        int sessionId, ArrayOf<Byte>* initData, const String& mimeType)
    :mSessionId(sessionId),
     mInitData(initData),
     mMimeType(mimeType)
{
}

Int32 MediaDrmBridge::PendingCreateSessionData::SessionId()
{
    return mSessionId;
}

AutoPtr<ArrayOf<Byte> > MediaDrmBridge::PendingCreateSessionData::InitData()
{
    return mInitData;
}

String MediaDrmBridge::PendingCreateSessionData::MimeType()
{
    return mMimeType;
}

//===============================================================
//               MediaDrmBridge::MediaDrmListener
//===============================================================
CAR_INTERFACE_IMPL(MediaDrmBridge::MediaDrmListener, Object, IMediaDrmOnEventListener);

MediaDrmBridge::MediaDrmListener::MediaDrmListener(
    /* [in] */ MediaDrmBridge* owner)
    : mOwner(owner)
{
}

//@Override
ECode MediaDrmBridge::MediaDrmListener::OnEvent(
    /* [in] */ IMediaDrm* mediaDrm,
    /* [in] */ ArrayOf<Byte>* session_array,
    /* [in] */ Int32 event,
    /* [in] */ Int32 extra,
    /* [in] */ ArrayOf<Byte>* data)
{
    if (session_array == NULL) {
        Logger::E(TAG, "MediaDrmListener: Null session.");
        return NOERROR;
    }

    AutoPtr<IByteBufferHelper> bbHelper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&bbHelper);
    AutoPtr<IByteBuffer> session;
    bbHelper->Wrap(session_array, (IByteBuffer**)&session);
    if (!mOwner->SessionExists(session)) {
        Logger::E(TAG, "MediaDrmListener: Invalid session.");
        return NOERROR;
    }

    Int32 sessionId = INVALID_SESSION_ID;
    HashMap<AutoPtr<IByteBuffer>, Int32>::Iterator hmIter =
        mOwner->mSessionIds.Find(session);
    if (hmIter != mOwner->mSessionIds.End())
    {
        //Int32 sessionId = mSessionIds.get(session);
        sessionId = hmIter->mSecond;
        if (sessionId == INVALID_SESSION_ID) {
            Logger::E(TAG, "MediaDrmListener: Invalid session ID.");
            return NOERROR;
        }
    }

    switch(event) {
        case IMediaDrm::EVENT_PROVISION_REQUIRED:
            Logger::D(TAG, "MediaDrm.EVENT_PROVISION_REQUIRED");
            break;
        case IMediaDrm::EVENT_KEY_REQUIRED:
        {
            Logger::D(TAG, "MediaDrm.EVENT_KEY_REQUIRED");
            if (mOwner->mProvisioningPending) {
                return NOERROR;
            }
            HashMap<AutoPtr<IByteBuffer>, String>::Iterator hmsIter =
                mOwner->mSessionMimeTypes.Find(session);
            String mime;
            if (hmsIter != mOwner->mSessionMimeTypes.End())
            {
                mime = hmsIter->mSecond;
            }
            AutoPtr<IMediaDrmKeyRequest> request;
            // try {
            ECode ecode = mOwner->GetKeyRequest(session, data, mime, (IMediaDrmKeyRequest**)&request);
            if (FAILED(ecode)) {
                //TODO here should check the error code, if NotProvisionedException, should run below code
                // media/Media_Utils.cpp->ThrowExceptionAsNecessary
                // base/media/jni/android_media_MediaDrm.cpp->throwExceptionAsNecessary
                Logger::E(TAG, "TODO MediaDrmBridge.cpp->MediaDrmListener::OnEvent", ecode);
                /*
                   if (ecode == NotProvisionedException)
                   {
                   Logger::E(TAG, "Device not provisioned, e:0x%x", ecode);
                   StartProvisioning();
                   return;
                   }
                 */
            }
            // } catch (android.media.NotProvisionedException e) {
            //     Log.e(TAG, "Device not provisioned", e);
            //     startProvisioning();
            //     return;
            // }

            if (request != NULL) {
                mOwner->OnSessionMessage(sessionId, request);
            }
            else {
                mOwner->OnSessionError(sessionId);
            }
            break;
        }
        case IMediaDrm::EVENT_KEY_EXPIRED:
            Logger::D(TAG, "MediaDrm.EVENT_KEY_EXPIRED");
            mOwner->OnSessionError(sessionId);
            break;
        case IMediaDrm::EVENT_VENDOR_DEFINED:
            Logger::D(TAG, "MediaDrm.EVENT_VENDOR_DEFINED");
            assert(FALSE);  // Should never happen.
            break;
        default:
            Logger::E(TAG, "Invalid DRM event ,event:%d", event);
            return NOERROR;
    }

    return NOERROR;
}

//===============================================================
//              MediaDrmBridge::PostRequestTask
//===============================================================

const String MediaDrmBridge::PostRequestTask::MDB_PTAG("PostRequestTask");

MediaDrmBridge::PostRequestTask::PostRequestTask(
    /* [in] */ MediaDrmBridge* owner,
    /* [in] */ ArrayOf<Byte>* drmRequest)
    : mOwner(owner)
    , mDrmRequest(drmRequest)
{
}

//@Override
ECode MediaDrmBridge::PostRequestTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    String url;
    AutoPtr<ICharSequence> csq = ICharSequence::Probe((*params)[0]);
    csq->ToString(&url);

    mResponseBody = PostRequest(url, mDrmRequest);
    if (mResponseBody != NULL) {
        Logger::D(MDB_PTAG, "response length=%d", mResponseBody->GetLength());
    }
    return NOERROR;
}

AutoPtr<ArrayOf<Byte> > MediaDrmBridge::PostRequestTask::PostRequest(
    /* [in] */ const String& url,
    /* [in] */ ArrayOf<Byte>* drmRequest)
{
    AutoPtr<IHttpClient> httpClient;//TODO  = new DefaultHttpClient();
    String str(url);
    str += "&signedRequest=";
    //is this OK??
    String drmRequestStr((const char*)(drmRequest->GetPayload()), drmRequest->GetLength());
    //drmRequest->ToString(&drmRequestStr);
    str += drmRequestStr;
    AutoPtr<IHttpPost> httpPost;
    //TODO CHttpPost::New(str, (IHttpPost**)&httpPost);

    String requestLineStr;
    AutoPtr<IHttpRequest> httpRequest = IHttpRequest::Probe(httpPost);
    AutoPtr<IRequestLine> requestLine;
    httpRequest->GetRequestLine((IRequestLine**)&requestLine);
    //TODO is this convertion ok?
    AutoPtr<IObject> rlObject = IObject::Probe(requestLine);
    rlObject->ToString(&requestLineStr);
    Logger::D(MDB_PTAG, "PostRequest:%s", requestLineStr.string());
    // try {
        // Add data
        AutoPtr<IHttpMessage> httpMessage = IHttpMessage::Probe(httpPost);
        httpMessage->SetHeader(String("Accept"), String("*/*"));
        httpMessage->SetHeader(String("User-Agent"), String("Widevine CDM v1.0"));
        httpMessage->SetHeader(String("Content-Type"), String("application/json"));

        // Execute HTTP Post Request
        AutoPtr<IHttpResponse> response;
        AutoPtr<IHttpUriRequest> httpUriRequest = IHttpUriRequest::Probe(httpPost);
        httpClient->Execute(httpUriRequest, (IHttpResponse**)&response);

        AutoPtr<ArrayOf<Byte> > responseBody;
        AutoPtr<IStatusLine> line;
        response->GetStatusLine((IStatusLine**)&line);
        Int32 responseCode;
        line->GetStatusCode(&responseCode);
        if (responseCode == 200) {
            AutoPtr<IHttpEntity> httpEntity;
            response->GetEntity((IHttpEntity**)&httpEntity);
            //TODO EntityUtils::ToByteArray(httpEntity, (ArrayOf<Byte>**)&responseBody);
        }
        else {
            Logger::E(MDB_PTAG, "Server returned HTTP error code,%d ", responseCode);
            return NULL;
        }

        return responseBody;
    // } catch (ClientProtocolException e) {
    //     e.printStackTrace();
    // } catch (IOException e) {
    //     e.printStackTrace();
    // }

    //return NULL;
}

//@Override
ECode MediaDrmBridge::PostRequestTask::OnPostExecute(
    /* [in] */ IInterface* result)
{
    mOwner->OnProvisionResponse(mResponseBody);
    return NOERROR;
}

//===============================================================
//       MediaDrmBridge::ResumePendingOperationsRunnable
//===============================================================
CAR_INTERFACE_IMPL(MediaDrmBridge::ResumePendingOperationsRunnable, Object, IRunnable);

MediaDrmBridge::ResumePendingOperationsRunnable::ResumePendingOperationsRunnable(
    /* [in] */ MediaDrmBridge* owner)
    : mOwner(owner)
{
}

ECode MediaDrmBridge::ResumePendingOperationsRunnable::Run()
{
    mOwner->ProcessPendingCreateSessionData();
    return NOERROR;
}

//===============================================================
//           MediaDrmBridge::OnSessionCreatedRunnable
//===============================================================
CAR_INTERFACE_IMPL(MediaDrmBridge::OnSessionCreatedRunnable, Object, IRunnable);

MediaDrmBridge::OnSessionCreatedRunnable::OnSessionCreatedRunnable(
    /* [in] */ MediaDrmBridge* owner,
    /* [in] */ Int32 sessionId,
    /* [in] */ const String& webSessionId)
    : mOwner(owner)
    , mSessionId(sessionId)
    , mWebSessionId(webSessionId)
{
}

ECode MediaDrmBridge::OnSessionCreatedRunnable::Run()
{
    mOwner->NativeOnSessionCreated(mOwner->mNativeMediaDrmBridge, mSessionId, mWebSessionId);
    return NOERROR;
}

//===============================================================
//          MediaDrmBridge::OnSessionMessageRunnable
//===============================================================
CAR_INTERFACE_IMPL(MediaDrmBridge::OnSessionMessageRunnable, Object, IRunnable);

MediaDrmBridge::OnSessionMessageRunnable::OnSessionMessageRunnable(
    /* [in] */ MediaDrmBridge* owner,
    /* [in] */ Int32 sessionId,
    /* [in] */ IMediaDrmKeyRequest* request)
    : mOwner(owner)
    , mSessionId(sessionId)
    , mRequest(request)
{
}

ECode MediaDrmBridge::OnSessionMessageRunnable::Run()
{
    AutoPtr<ArrayOf<Byte> > data;
    mRequest->GetData((ArrayOf<Byte>**)&data);
    String url;
    mRequest->GetDefaultUrl(&url);
    mOwner->NativeOnSessionMessage(mOwner->mNativeMediaDrmBridge, mSessionId,
                    data, url);
    return NOERROR;
}

//===============================================================
//           MediaDrmBridge::OnSessionReadyRunnable
//===============================================================
CAR_INTERFACE_IMPL(MediaDrmBridge::OnSessionReadyRunnable, Object, IRunnable);

MediaDrmBridge::OnSessionReadyRunnable::OnSessionReadyRunnable(
    /* [in] */ MediaDrmBridge* owner,
    /* [in] */ Int32 sessionId)
    : mOwner(owner)
    , mSessionId(sessionId)
{
}

ECode MediaDrmBridge::OnSessionReadyRunnable::Run()
{
    mOwner->NativeOnSessionReady(mOwner->mNativeMediaDrmBridge, mSessionId);
    return NOERROR;
}

//===============================================================
//            MediaDrmBridge::OnSessionClosedRunnable
//===============================================================
CAR_INTERFACE_IMPL(MediaDrmBridge::OnSessionClosedRunnable, Object, IRunnable);

MediaDrmBridge::OnSessionClosedRunnable::OnSessionClosedRunnable(
    /* [in] */ MediaDrmBridge* owner,
    /* [in] */ Int32 sessionId)
    : mOwner(owner)
    , mSessionId(sessionId)
{
}

ECode MediaDrmBridge::OnSessionClosedRunnable::Run()
{
    mOwner->NativeOnSessionClosed(mOwner->mNativeMediaDrmBridge, mSessionId);
    return NOERROR;
}

//===============================================================
//          MediaDrmBridge::OnSessionErrorRunnable
//===============================================================
CAR_INTERFACE_IMPL(MediaDrmBridge::OnSessionErrorRunnable, Object, IRunnable);

MediaDrmBridge::OnSessionErrorRunnable::OnSessionErrorRunnable(
    /* [in] */ MediaDrmBridge* owner,
    /* [in] */ Int32 sessionId)
    : mOwner(owner)
    , mSessionId(sessionId)
{
}

ECode MediaDrmBridge::OnSessionErrorRunnable::Run()
{
    mOwner->NativeOnSessionError(mOwner->mNativeMediaDrmBridge, mSessionId);
    return NOERROR;
}

//===============================================================
//                      MediaDrmBridge
//===============================================================

const String MediaDrmBridge::TAG("MediaDrmBridge");
const String MediaDrmBridge::SECURITY_LEVEL("securityLevel");
const String MediaDrmBridge::PRIVACY_MODE("privacyMode");
const String MediaDrmBridge::SESSION_SHARING("sessionSharing");
const String MediaDrmBridge::ENABLE("enable");
const Int32 MediaDrmBridge::INVALID_SESSION_ID;

MediaDrmBridge::MediaDrmBridge(
    /* [in] */ IUUID* schemeUUID,
    /* [in] */ Int64 nativeMediaDrmBridge,
    /* [in] */ Boolean singleSessionMode)
{
    mSchemeUUID = schemeUUID;
    //TODO CMediaDrm::New(schemeUUID, (IMediaDrm**)&mMediaDrm);
    mNativeMediaDrmBridge = nativeMediaDrmBridge;
    CHandler::New((IHandler**)&mHandler);
    mSingleSessionMode = singleSessionMode;
    //mSessionIds = new HashMap<AutoPtr<IByteBuffer>, Int32>();
    //mSessionMimeTypes = new HashMap<AutoPtr<IByteBuffer>, String>();
    //mPendingCreateSessionDataQueue = new ArrayDeque<PendingCreateSessionData>();
    CArrayDeque::New((IArrayDeque**)&mPendingCreateSessionDataQueue);
    mResetDeviceCredentialsPending = FALSE;
    mProvisioningPending = FALSE;

    AutoPtr<MediaDrmListener> lisener = new MediaDrmListener(this);
    mMediaDrm->SetOnEventListener(lisener);
    mMediaDrm->SetPropertyString(PRIVACY_MODE, ENABLE);
    if (!mSingleSessionMode) {
        mMediaDrm->SetPropertyString(SESSION_SHARING, ENABLE);
    }

    // We could open a MediaCrypto session here to support faster start of
    // clear lead (no need to wait for createSession()). But on
    // Android, memory and battery resources are precious and we should
    // only create a session when we are sure we'll use it.
    // TODO(xhwang): Investigate other options to support fast start.
}

AutoPtr<IUUID> MediaDrmBridge::GetUUIDFromBytes(
    /* [in] */ ArrayOf<Byte>* data)
{
    if (data->GetLength() != 16) {
        return NULL;
    }

    Int64 mostSigBits = 0;
    Int64 leastSigBits = 0;
    for (Int32 i = 0; i < 8; i++) {
        mostSigBits = (mostSigBits << 8) | ((*data)[i] & 0xff);
    }

    for (Int32 i = 8; i < 16; i++) {
        leastSigBits = (leastSigBits << 8) | ((*data)[i] & 0xff);
    }

    AutoPtr<IUUID> uuid;
    //TODO CUUID::New(mostSigBits, leastSigBits, (IUUID**)&uuid);

    return uuid;
}

/**
 *  Gets session associated with the sessionId.
 *
 *  @return session if sessionId maps a valid opened session. Returns null
 *  otherwise.
 */
AutoPtr<IByteBuffer> MediaDrmBridge::GetSession(
    /* [in] */ Int32 sessionId)
{
    HashMap<AutoPtr<IByteBuffer>, Int32>::Iterator iter =
        mSessionIds.Begin();
    //for (ByteBuffer session : mSessionIds.keySet())
    for(;iter != mSessionIds.End(); ++iter)
    {
        if (iter->mSecond == sessionId) {
            return iter->mFirst;
        }
    }
    return NULL;
}

/**
 * Create a MediaCrypto object.
 *
 * @return whether a MediaCrypto object is successfully created.
 */
Boolean MediaDrmBridge::CreateMediaCrypto()
{
    if (mMediaDrm == NULL) {
        return FALSE;
    }
    assert(!mProvisioningPending);
    assert(mMediaCryptoSession == NULL);
    assert(mMediaCrypto == NULL);

    // Open media crypto session.
    mMediaCryptoSession = OpenSession();
    if (mMediaCryptoSession == NULL) {
        Logger::E(TAG, "Cannot create MediaCrypto Session.");
        return FALSE;
    }
    Logger::D(TAG, "MediaCrypto Session created: 0x%p", mMediaCryptoSession.Get());

    // Create MediaCrypto object.
    // try {
        AutoPtr<IMediaCryptoHelper> helper;
        //TODO CMediaCryptoHelper::AcquireSingleton((IMediaCryptoHelper**)&helper);
        Boolean isCryptoSchemeSupported;
        helper->IsCryptoSchemeSupported(mSchemeUUID, &isCryptoSchemeSupported);
        if (isCryptoSchemeSupported) {
            //final byte[] mediaCryptoSession = mMediaCryptoSession.array();
            AutoPtr<ArrayOf<Byte> > mediaCryptoSession;
            mMediaCryptoSession->GetArray((ArrayOf<Byte>**)&mediaCryptoSession);

            //mMediaCrypto = new MediaCrypto(mSchemeUUID, mediaCryptoSession);
            //TODO CMediaCrypto::New(mSchemeUUID, mediaCryptoSession, (IMediaCrypto**)mMediaCrypto);
            assert(mMediaCrypto != NULL);
            Logger::D(TAG, "MediaCrypto successfully created!");
            //mSessionIds.put(mMediaCryptoSession, INVALID_SESSION_ID);
            mSessionIds[mMediaCryptoSession]  = INVALID_SESSION_ID;
            // Notify the native code that MediaCrypto is ready.
            NativeOnMediaCryptoReady(mNativeMediaDrmBridge);
            return TRUE;
        }
        else {
            Logger::E(TAG, "Cannot create MediaCrypto for unsupported scheme.");
        }
    // } catch (android.media.MediaCryptoException e) {
    //     Log.e(TAG, "Cannot create MediaCrypto", e);
    // }

    ReleaseResources();

    return FALSE;
}

/**
 * Open a new session..
 *
 * @return the session opened. Returns null if unexpected error happened.
 */
AutoPtr<IByteBuffer> MediaDrmBridge::OpenSession()
{
    assert(mMediaDrm != NULL);
    // try {
        AutoPtr<ArrayOf<Byte> > session;
        mMediaDrm->OpenSession((ArrayOf<Byte>**)&session);
        // ByteBuffer.wrap() is backed by the byte[]. Make a clone here in
        // case the underlying byte[] is modified.
        AutoPtr<IByteBufferHelper> bbHelper;
        CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&bbHelper);
        AutoPtr<IByteBuffer> bb;
        bbHelper->Wrap(session->Clone(), (IByteBuffer**)&bb);
        return bb;
    // } catch (java.lang.RuntimeException e) {  // TODO(xhwang): Drop this?
    //     Log.e(TAG, "Cannot open a new session", e);
    //     release();
    //     return null;
    // } catch (android.media.NotProvisionedException e) {
    //     // Throw NotProvisionedException so that we can startProvisioning().
    //     throw e;
    // } catch (android.media.MediaDrmException e) {
    //     // Other MediaDrmExceptions (e.g. ResourceBusyException) are not
    //     // recoverable.
    //     Log.e(TAG, "Cannot open a new session", e);
    //     release();
    //     return null;
    // }
}

/**
 * Close a session.
 *
 * @param session to be closed.
 */
void MediaDrmBridge::CloseSession(
    /* [in] */ IByteBuffer* session)
{
    assert(mMediaDrm != NULL);
    AutoPtr<ArrayOf<Byte> > array;
    session->GetArray((ArrayOf<Byte>**)&array);
    mMediaDrm->CloseSession(array);
}

/**
 * Check whether the crypto scheme is supported for the given container.
 * If |containerMimeType| is an empty string, we just return whether
 * the crypto scheme is supported.
 *
 * @return true if the container and the crypto scheme is supported, or
 * false otherwise.
 */
//@CalledByNative
Boolean MediaDrmBridge::IsCryptoSchemeSupported(
    /* [in] */ ArrayOf<Byte>* schemeUUID,
    /* [in] */ const String& containerMimeType)
{
    AutoPtr<IUUID> cryptoScheme = GetUUIDFromBytes(schemeUUID);

    AutoPtr<IMediaCryptoHelper> helper;
    //TODO CMediaCryptoHelper::AcquireSingleton((IMediaCryptoHelper**)&helper);
    Boolean result;

    if (containerMimeType.IsEmpty()) {
        helper->IsCryptoSchemeSupported(cryptoScheme, &result);
        return result;
    }

    AutoPtr<IMediaDrmHelper> mdHelper;
    //TODO CMediaDrmHelper::AcquireSingleton((IMediaDrmHelper**)&mdHelper);
    mdHelper->IsCryptoSchemeSupported(cryptoScheme, containerMimeType, &result);
    return result;
}

/**
 * Create a new MediaDrmBridge from the crypto scheme UUID.
 *
 * @param schemeUUID Crypto scheme UUID.
 * @param securityLevel Security level to be used.
 * @param nativeMediaDrmBridge Native object of this class.
 */
//@CalledByNative return MediaDrmBridge
AutoPtr<IInterface> MediaDrmBridge::Create(
    /* [in] */ ArrayOf<Byte>* schemeUUID,
    /* [in] */ Int64 nativeMediaDrmBridge)
{
    AutoPtr<IUUID> cryptoScheme = GetUUIDFromBytes(schemeUUID);
    AutoPtr<IMediaDrmHelper> mdHelper;
    //TODO CMediaDrmHelper::AcquireSingleton((IMediaDrmHelper**)&mdHelper);
    Boolean isSupported;
    mdHelper->IsCryptoSchemeSupported(cryptoScheme, &isSupported);
    if (cryptoScheme == NULL || !isSupported) {
        return NULL;
    }

    Boolean singleSessionMode = FALSE;
    if (Build::VERSION::RELEASE.Equals("4.4")) {
        singleSessionMode = TRUE;
    }

    Logger::D(TAG, "MediaDrmBridge uses %s -session mode.",
            (singleSessionMode ? "single" : "multiple"));

    AutoPtr<MediaDrmBridge> mediaDrmBridge;
    // try {
        mediaDrmBridge = new MediaDrmBridge(
            cryptoScheme, nativeMediaDrmBridge, singleSessionMode);
        Logger::D(TAG, "MediaDrmBridge successfully created.");
    // } catch (android.media.UnsupportedSchemeException e) {
    //     Log.e(TAG, "Unsupported DRM scheme", e);
    // } catch (java.lang.IllegalArgumentException e) {
    //     Log.e(TAG, "Failed to create MediaDrmBridge", e);
    // } catch (java.lang.IllegalStateException e) {
    //     Log.e(TAG, "Failed to create MediaDrmBridge", e);
    // }

    AutoPtr<IInterface> result = TO_IINTERFACE(mediaDrmBridge);
    return result;
}

/**
 * Set the security level that the MediaDrm object uses.
 * This function should be called right after we construct MediaDrmBridge
 * and before we make any other calls.
 */
//@CalledByNative
Boolean MediaDrmBridge::SetSecurityLevel(
    /* [in] */ const String& securityLevel)
{
    if (mMediaDrm == NULL || mMediaCrypto != NULL) {
        return FALSE;
    }

    String currentSecurityLevel;
    mMediaDrm->GetPropertyString(SECURITY_LEVEL, &currentSecurityLevel);
    Logger::D(TAG, "Security level: current %s, new %s", currentSecurityLevel.string(), securityLevel.string());
    if (securityLevel.Equals(currentSecurityLevel)) {
        // No need to set the same security level again. This is not just
        // a shortcut! Setting the same security level actually causes an
        // exception in MediaDrm!
        return TRUE;
    }

    // try {
        mMediaDrm->SetPropertyString(SECURITY_LEVEL, securityLevel);
        return TRUE;
    // } catch (java.lang.IllegalArgumentException e) {
    //     Log.e(TAG, "Failed to set security level " + securityLevel, e);
    // } catch (java.lang.IllegalStateException e) {
    //     Log.e(TAG, "Failed to set security level " + securityLevel, e);
    // }

//    Log.e(TAG, "Security level " + securityLevel + " not supported!");

//    return FALSE;
}

/**
 * Return the MediaCrypto object if available.
 */
//@CalledByNative
AutoPtr<IMediaCrypto> MediaDrmBridge::GetMediaCrypto()
{
    return mMediaCrypto;
}

/**
 * Reset the device DRM credentials.
 */
//@CalledByNative
void MediaDrmBridge::ResetDeviceCredentials()
{
    mResetDeviceCredentialsPending = TRUE;
    AutoPtr<IMediaDrmProvisionRequest> request;
    mMediaDrm->GetProvisionRequest((IMediaDrmProvisionRequest**)&request);
    AutoPtr<ArrayOf<Byte> > data;
    request->GetData((ArrayOf<Byte>**)&data);
    AutoPtr<PostRequestTask> postTask = new PostRequestTask(this, data);
    String url;
    request->GetDefaultUrl(&url);
    AutoPtr<ICharSequence> iUrl;
    CString::New(url, (ICharSequence**)&iUrl);
    AutoPtr<ArrayOf<IInterface*> > params = ArrayOf<IInterface*>::Alloc(1);
    params->Set(0, iUrl);
    postTask->Execute(params);
}

/**
 * Release the MediaDrmBridge object.
 */
//@CalledByNative
void MediaDrmBridge::ReleaseResources()
{
    // Do not reset mHandler and mNativeMediaDrmBridge so that we can still
    // post KeyError back to native code.

    mPendingCreateSessionDataQueue->Clear();
    mPendingCreateSessionDataQueue = NULL;

    HashMap<AutoPtr<IByteBuffer>, Int32>::Iterator iter =
        mSessionIds.Begin();
    //for (ByteBuffer session : mSessionIds.keySet())
    for(;iter != mSessionIds.End(); ++iter)
    {
        CloseSession(iter->mFirst);
    }

    mSessionIds.Clear();
    //mSessionIds = NULL;
    mSessionMimeTypes.Clear();
    //mSessionMimeTypes = NULL;

    // This session was closed in the "for" loop above.
    mMediaCryptoSession = NULL;

    if (mMediaCrypto != NULL) {
        mMediaCrypto->ReleaseResources();
        mMediaCrypto = NULL;
    }

    if (mMediaDrm != NULL) {
        mMediaDrm->ReleaseResources();
        mMediaDrm = NULL;
    }
}

/**
 * Get a key request.
 *
 * @param session Session on which we need to get the key request.
 * @param data Data needed to get the key request.
 * @param mime Mime type to get the key request.
 *
 * @return the key request.
 */
ECode MediaDrmBridge::GetKeyRequest(
    /* [in] */ IByteBuffer* session,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ const String& mime,
    /* [out] */ IMediaDrmKeyRequest** result)
{
    assert(mMediaDrm != NULL);
    assert(mMediaCrypto != NULL);
    assert(!mProvisioningPending);
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    //HashMap<String, String> optionalParameters = new HashMap<String, String>();
    AutoPtr<IHashMap> optionalParameters;
    //TODO CHashMap::New((IHashMap**)&optionalParameters);
    AutoPtr<IMediaDrmKeyRequest> request;
    AutoPtr<ArrayOf<Byte> > sessionArray;
    session->GetArray((ArrayOf<Byte>**)&sessionArray);
    ECode ecode = mMediaDrm->GetKeyRequest(
            sessionArray, data, mime, IMediaDrm::KEY_TYPE_STREAMING,
            optionalParameters, (IMediaDrmKeyRequest**)&request);
    String re = (request != NULL) ? String("successed") : String("failed");
    Logger::D(TAG, "getKeyRequest %s!", re.string());
    *result = request.Get();
    REFCOUNT_ADD(*result);
    return ecode;
}

/**
 * Save data to |mPendingCreateSessionDataQueue| so that we can resume the
 * createSession() call later.
 */
void MediaDrmBridge::SavePendingCreateSessionData(
    /* [in] */ Int32 sessionId,
    /* [in] */ ArrayOf<Byte>* initData,
    /* [in] */ const String& mime)
{
    Logger::D(TAG, "savePendingCreateSessionData()");
    AutoPtr<PendingCreateSessionData> data = new PendingCreateSessionData(sessionId, initData, mime);
    AutoPtr<IInterface> sessionData = TO_IINTERFACE(data);
    AutoPtr<IDeque> deque = IDeque::Probe(mPendingCreateSessionDataQueue);
    Boolean result;
    deque->Offer(sessionData, &result);
}

/**
 * Process all pending createSession() calls synchronously.
 */
void MediaDrmBridge::ProcessPendingCreateSessionData()
{
    Logger::D(TAG, "processPendingCreateSessionData()");
    assert(mMediaDrm != NULL);

    // Check mMediaDrm != null because error may happen in createSession().
    // Check !mProvisioningPending because NotProvisionedException may be
    // thrown in createSession().
    Boolean isEmpty;
    mPendingCreateSessionDataQueue->IsEmpty(&isEmpty);
    while (mMediaDrm != NULL && !mProvisioningPending && !isEmpty) {
        //PendingCreateSessionData pendingData = mPendingCreateSessionDataQueue.poll();
        AutoPtr<IDeque> deque = IDeque::Probe(mPendingCreateSessionDataQueue);
        AutoPtr<IInterface> sessionData;
        deque->Poll((IInterface**)&sessionData);
        //TODO pay attention here for the force-cast
        AutoPtr<PendingCreateSessionData> pendingData = (PendingCreateSessionData*)(IObject*)(IInterface*)sessionData;
        int sessionId = pendingData->SessionId();
        //byte[] initData = pendingData.initData();
        AutoPtr<ArrayOf<Byte> > initData = pendingData->InitData();
        String mime = pendingData->MimeType();
        CreateSession(sessionId, initData, mime);
    }
}

/**
 * Process pending operations asynchrnously.
 */
void MediaDrmBridge::ResumePendingOperations()
{
    AutoPtr<IRunnable> runnable = new ResumePendingOperationsRunnable(this);
    Boolean result;
    mHandler->Post(runnable, &result);
}

/**
 * Create a session with |sessionId|, |initData| and |mime|.
 * In multiple session mode, a new session will be open. In single session
 * mode, the mMediaCryptoSession will be used.
 *
 * @param sessionId ID for the session to be created.
 * @param initData Data needed to generate the key request.
 * @param mime Mime type.
 */
//@CalledByNative
void MediaDrmBridge::CreateSession(
    /* [in] */ Int32 sessionId,
    /* [in] */ ArrayOf<Byte>* initData,
    /* [in] */ const String& mime)
{
    Logger::D(TAG, "createSession()");
    if (mMediaDrm == NULL) {
        Logger::E(TAG, "createSession() called when MediaDrm is null.");
        return;
    }

    if (mProvisioningPending) {
        assert(mMediaCrypto == NULL);
        SavePendingCreateSessionData(sessionId, initData, mime);
        return;
    }

    Boolean newSessionOpened = FALSE;
    AutoPtr<IByteBuffer> session;
    // try {
        // Create MediaCrypto if necessary.
        if (mMediaCrypto == NULL && !CreateMediaCrypto()) {
            OnSessionError(sessionId);
            return;
        }

        HashMap<AutoPtr<IByteBuffer>, Int32>::Iterator hmIter =
            mSessionIds.Find(mMediaCryptoSession);
        Boolean UNUSED(found) = (hmIter != mSessionIds.End());
        assert(mMediaCrypto != NULL);
        assert(found);

        if (mSingleSessionMode) {
            session = mMediaCryptoSession;
            HashMap<AutoPtr<IByteBuffer>, String>::Iterator hmsIter =
                mSessionMimeTypes.Find(session);
            if (hmsIter != mSessionMimeTypes.End())
            {
                if(!((hmsIter->mSecond).Equals(mime)))
                {
                    Logger::E(TAG, "Only one mime type is supported in single session mode.");
                    OnSessionError(sessionId);
                    return;
                }
            }
            else
            {
                Logger::E(TAG, "Only one mime type is supported in single session mode.");
                OnSessionError(sessionId);
                return;
            }
        }
        else {
            session = OpenSession();
            if (session == NULL) {
                Logger::E(TAG, "Cannot open session in createSession().");
                OnSessionError(sessionId);
                return;
            }

            newSessionOpened = TRUE;
            //assert(!mSessionIds->ContainsKey(session));
            HashMap<AutoPtr<IByteBuffer>, Int32>::Iterator hmIter =
                mSessionIds.Find(session);
            Boolean UNUSED(found) = (hmIter != mSessionIds.End());
            assert(!found);
        }

        AutoPtr<IMediaDrmKeyRequest> request;
        ECode ecode  = GetKeyRequest(session, initData, mime, (IMediaDrmKeyRequest**)&request);
        if (FAILED(ecode))
        {
            Logger::E(TAG, "Device not provisioned,ecode:%d", ecode);
            //TODO here should check the error code, if NotProvisionedException, should run below code
            // media/Media_Utils.cpp->ThrowExceptionAsNecessary
            // base/media/jni/android_media_MediaDrm.cpp->throwExceptionAsNecessary
            Logger::E(TAG, "TODO MediaDrmBridge.cpp->CreateSession", ecode);
            /*if (ecode == NotProvisionedException)
            {
                if (newSessionOpened) {
                    closeSession(session);
                }
                SavePendingCreateSessionData(sessionId, initData, mime);
                StartProvisioning();
            }
            */
            return;
        }
        if (request == NULL) {
            if (newSessionOpened) {
                CloseSession(session);
            }
            OnSessionError(sessionId);
            return;
        }

        OnSessionCreated(sessionId, GetWebSessionId(session));
        OnSessionMessage(sessionId, request);
        if (newSessionOpened) {
            Logger::D(TAG, "createSession(): Session %s(%d)",
                    GetWebSessionId(session).string(), sessionId);
        }

        //mSessionIds->Put(session, sessionId);
        mSessionIds[session] = sessionId;
        //mSessionMimeTypes->Put(session, mime);
        mSessionMimeTypes[session] = mime;
    // } catch (android.media.NotProvisionedException e) {
    //     Log.e(TAG, "Device not provisioned", e);
    //     if (newSessionOpened) {
    //         closeSession(session);
    //     }
    //     savePendingCreateSessionData(sessionId, initData, mime);
    //     startProvisioning();
    // }
}

/**
 * Returns whether |sessionId| is a valid key session, excluding the media
 * crypto session in multi-session mode.
 *
 * @param sessionId Crypto session Id.
 */
Boolean MediaDrmBridge::SessionExists(
    /* [in] */ IByteBuffer* session)
{
    if (mMediaCryptoSession == NULL) {
        assert(mSessionIds.IsEmpty());
        Logger::E(TAG, "Session doesn't exist because media crypto session is not created.");
        return FALSE;
    }

    HashMap<AutoPtr<IByteBuffer>, Int32>::Iterator hmIter =
        mSessionIds.Find(mMediaCryptoSession);
    Boolean found = (hmIter != mSessionIds.End());
    //assert(mSessionIds->ContainsKey(mMediaCryptoSession));
    assert(found);

    if (mSingleSessionMode) {
        return mMediaCryptoSession.Get() == session;
    }

    //return !session.Equals(mMediaCryptoSession) && mSessionIds.containsKey(session);
    return !(mMediaCryptoSession.Get() == session) && found;
}

/**
 * Cancel a key request for a session Id.
 *
 * @param sessionId Reference ID of session to be released.
 */
//@CalledByNative
void MediaDrmBridge::ReleaseSession(
    /* [in] */ Int32 sessionId)
{
    Logger::E(TAG, "releaseSession(): %d", sessionId);
    if (mMediaDrm == NULL) {
        Logger::D(TAG, "releaseSession() called when MediaDrm is null.");
        return;
    }

    AutoPtr<IByteBuffer> session = GetSession(sessionId);
    if (session == NULL) {
        Logger::D(TAG, "Invalid sessionId in releaseSession.");
        OnSessionError(sessionId);
        return;
    }

    AutoPtr<ArrayOf<Byte> > sessionArray;
    session->GetArray((ArrayOf<Byte>**)&sessionArray);
    mMediaDrm->RemoveKeys(sessionArray);

    // We don't close the media crypto session in single session mode.
    if (!mSingleSessionMode) {
        Logger::D(TAG, "Session %d closed.", sessionId);
        CloseSession(session);
        mSessionIds.Erase(session);
        OnSessionClosed(sessionId);
    }
}

/**
 * Add a key for a session Id.
 *
 * @param sessionId Reference ID of session to be updated.
 * @param key Response data from the server.
 */
//@CalledByNative
void MediaDrmBridge::UpdateSession(
    /* [in] */ Int32 sessionId,
    /* [in] */ ArrayOf<Byte>* key)
{
    Logger::D(TAG, "updateSession(): %d", sessionId);
    if (mMediaDrm == NULL) {
        Logger::E(TAG, "updateSession() called when MediaDrm is null.");
        return;
    }

    // TODO(xhwang): We should be able to DCHECK this when WD EME is implemented.
    AutoPtr<IByteBuffer> session = GetSession(sessionId);
    if (!SessionExists(session)) {
        Logger::E(TAG, "Invalid session in updateSession.");
        OnSessionError(sessionId);
        return;
    }

    // try {
    //     try {
            AutoPtr<ArrayOf<Byte> > sessionArray;
            session->GetArray((ArrayOf<Byte>**)&sessionArray);
            AutoPtr<ArrayOf<Byte> > result;
            ECode ecode = mMediaDrm->ProvideKeyResponse(sessionArray, key, (ArrayOf<Byte>**)&result);
        // } catch (java.lang.IllegalStateException e) {
        //     // This is not really an exception. Some error code are incorrectly
        //     // reported as an exception.
        //     // TODO(qinmin): remove this exception catch when b/10495563 is fixed.
        //     Log.e(TAG, "Exception intentionally caught when calling provideKeyResponse()", e);
        // }
        if (FAILED(ecode))
        {
            Logger::E(TAG, "Exception intentionally caught when calling provideKeyResponse(), ecode:0x%x", ecode);
            OnSessionError(sessionId);
            ReleaseResources();
            return;
        }
        OnSessionReady(sessionId);
        Logger::D(TAG, "Key successfully added for session %d", sessionId);
        return;
    // } catch (android.media.NotProvisionedException e) {
    //     // TODO(xhwang): Should we handle this?
    //     Log.e(TAG, "failed to provide key response", e);
    // } catch (android.media.DeniedByServerException e) {
    //     Log.e(TAG, "failed to provide key response", e);
    // }

    //OnSessionError(sessionId);
    //ReleaseResources();
}

/**
 * Return the security level of this DRM object.
 */
//@CalledByNative
String MediaDrmBridge::GetSecurityLevel()
{
    if (mMediaDrm == NULL) {
        Logger::D(TAG, "getSecurityLevel() called when MediaDrm is null.");
        return String("");
    }

    String property;
    mMediaDrm->GetPropertyString(String("securityLevel"), &property);

    return property;
}

void MediaDrmBridge::StartProvisioning()
{
    Logger::D(TAG, "startProvisioning");
    assert(mMediaDrm != NULL);
    assert(!mProvisioningPending);
    mProvisioningPending = TRUE;
    AutoPtr<IMediaDrmProvisionRequest> request;
    mMediaDrm->GetProvisionRequest((IMediaDrmProvisionRequest**)&request);
    AutoPtr<ArrayOf<Byte> > data;
    request->GetData((ArrayOf<Byte>**)&data);
    AutoPtr<PostRequestTask> postTask = new PostRequestTask(this, data);
    String url;
    request->GetDefaultUrl(&url);
    AutoPtr<ICharSequence> iUrl;
    CString::New(url, (ICharSequence**)&iUrl);
    AutoPtr<ArrayOf<IInterface*> > params = ArrayOf<IInterface*>::Alloc(1);
    params->Set(0, iUrl);
    postTask->Execute(params);
}

/**
 * Called when the provision response is received.
 *
 * @param response Response data from the provision server.
 */
void MediaDrmBridge::OnProvisionResponse(
    /* [in] */ ArrayOf<Byte>* response)
{
//    Log.d(TAG, "onProvisionResponse()");
    assert(mProvisioningPending);
    mProvisioningPending = FALSE;

    // If |mMediaDrm| is released, there is no need to callback native.
    if (mMediaDrm == NULL) {
        return;
    }

    Boolean success = ProvideProvisionResponse(response);

    if (mResetDeviceCredentialsPending) {
        NativeOnResetDeviceCredentialsCompleted(mNativeMediaDrmBridge, success);
        mResetDeviceCredentialsPending = FALSE;
    }

    if (success) {
        ResumePendingOperations();
    }
}

/**
 * Provide the provisioning response to MediaDrm.
 * @returns false if the response is invalid or on error, true otherwise.
 */
Boolean MediaDrmBridge::ProvideProvisionResponse(
    /* [in] */ ArrayOf<Byte>* response)
{
    if (response == NULL || response->GetLength() == 0) {
        Logger::E(TAG, "Invalid provision response.");
        return FALSE;
    }

    // try {
        ECode ecode = mMediaDrm->ProvideProvisionResponse(response);
        if (FAILED(ecode))
        {
            Logger::E(TAG, "failed to provide provision response, ecode:0x%x", ecode);
            return FALSE;
        }
    // } catch (android.media.DeniedByServerException e) {
    //     Log.e(TAG, "failed to provide provision response", e);
    // } catch (java.lang.IllegalStateException e) {
    //     Log.e(TAG, "failed to provide provision response", e);
    // }

    return TRUE;
}

void MediaDrmBridge::OnSessionCreated(
    /* [in] */ Int32 sessionId,
    /* [in] */ const String& webSessionId)
{
    AutoPtr<IRunnable> runnable = new OnSessionCreatedRunnable(this, sessionId, webSessionId);
    Boolean result;
    mHandler->Post(runnable, &result);
}

void MediaDrmBridge::OnSessionMessage(
    /* [in] */ Int32 sessionId,
    /* [in] */ IMediaDrmKeyRequest* request)
{
    AutoPtr<IRunnable> runnable = new OnSessionMessageRunnable(this, sessionId, request);
    Boolean result;
    mHandler->Post(runnable, &result);
}

void MediaDrmBridge::OnSessionReady(
    /* [in] */ Int32 sessionId)
{
    AutoPtr<IRunnable> runnable = new OnSessionReadyRunnable(this, sessionId);
    Boolean result;
    mHandler->Post(runnable, &result);
}

void MediaDrmBridge::OnSessionClosed(
    /* [in] */ Int32 sessionId)
{
    AutoPtr<IRunnable> runnable = new OnSessionClosedRunnable(this, sessionId);
    Boolean result;
    mHandler->Post(runnable, &result);
}

void MediaDrmBridge::OnSessionError(
    /* [in] */ Int32 sessionId)
{
    // TODO(qinmin): pass the error code to native.
    AutoPtr<IRunnable> runnable = new OnSessionErrorRunnable(this, sessionId);
    Boolean result;
    mHandler->Post(runnable, &result);
}

String MediaDrmBridge::GetWebSessionId(
    /* [in] */ IByteBuffer* session)
{
    String webSessionId;
    // try {
        AutoPtr<ArrayOf<Byte> > data;
        mMediaCryptoSession->GetArray((ArrayOf<Byte>**)&data);
        //TODO Is this right?
        //String webSessionId = new String(session.array(), "UTF-8");
        webSessionId = String((const char*)(data->GetPayload()), data->GetLength());
    // } catch (java.io.UnsupportedEncodingException e) {
    //     Log.e(TAG, "getWebSessionId failed", e);
    // } catch (java.lang.NullPointerException e) {
    //     Log.e(TAG, "getWebSessionId failed", e);
    // }

    return webSessionId;
}

void MediaDrmBridge::AddKeySystemUuidMapping(
    /* [in] */ const String& keySystem,
    /* [in] */ IUUID* uuid)
{
    AutoPtr<IByteBuffer> uuidBuffer;
    AutoPtr<IByteBufferHelper> bbHelper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&bbHelper);
    bbHelper->AllocateDirect(16, (IByteBuffer**)&uuidBuffer);
    // MSB (byte) should be positioned at the first element.
    uuidBuffer->SetOrder(ByteOrder_BIG_ENDIAN);
    Int64 mostBits;
    uuid->GetMostSignificantBits(&mostBits);
    uuidBuffer->PutInt64(mostBits);
    Int64 leastBits;
    uuid->GetLeastSignificantBits(&leastBits);
    uuidBuffer->PutInt64(leastBits);
    NativeAddKeySystemUuidMapping(keySystem, uuidBuffer);
}

void MediaDrmBridge::NativeOnMediaCryptoReady(
    /* [in] */ Int64 nativeMediaDrmBridge)
{
    // Elastos_MediaDrmBridge_nativeOnMediaCryptoReady(THIS_PROBE(IInterface), (Handle32)nativeMediaDrmBridge);
}

void MediaDrmBridge::NativeOnSessionCreated(
    /* [in] */ Int64 nativeMediaDrmBridge,
    /* [in] */ Int32 sessionId,
    /* [in] */ const String& webSessionId)
{
    // Elastos_MediaDrmBridge_nativeOnSessionCreated(THIS_PROBE(IInterface), (Handle32)nativeMediaDrmBridge, sessionId, webSessionId);
}

void MediaDrmBridge::NativeOnSessionMessage(
    /* [in] */ Int64 nativeMediaDrmBridge,
    /* [in] */ Int32 sessionId,
    /* [in] */ ArrayOf<Byte>* message,
    /* [in] */ const String& destinationUrl)
{
    // Elastos_MediaDrmBridge_nativeOnSessionMessage(THIS_PROBE(IInterface), (Handle32)nativeMediaDrmBridge, sessionId, message, destinationUrl);
}

void MediaDrmBridge::NativeOnSessionReady(
    /* [in] */ Int64 nativeMediaDrmBridge,
    /* [in] */ Int32 sessionId)
{
    // Elastos_MediaDrmBridge_nativeOnSessionReady(THIS_PROBE(IInterface), (Handle32)nativeMediaDrmBridge, sessionId);
}

void MediaDrmBridge::NativeOnSessionClosed(
    /* [in] */ Int64 nativeMediaDrmBridge,
    /* [in] */ Int32 sessionId)
{
    // Elastos_MediaDrmBridge_nativeOnSessionClosed(THIS_PROBE(IInterface), (Handle32)nativeMediaDrmBridge, sessionId);
}

void MediaDrmBridge::NativeOnSessionError(
    /* [in] */ Int64 nativeMediaDrmBridge,
    /* [in] */ Int32 sessionId)
{
    // Elastos_MediaDrmBridge_nativeOnSessionError(THIS_PROBE(IInterface), (Handle32)nativeMediaDrmBridge, sessionId);
}

void MediaDrmBridge::NativeOnResetDeviceCredentialsCompleted(
    /* [in] */ Int64 nativeMediaDrmBridge,
    /* [in] */ Boolean success)
{
    // Elastos_MediaDrmBridge_nativeOnResetDeviceCredentialsCompleted(THIS_PROBE(IInterface), (Handle32)nativeMediaDrmBridge, success);
}

void MediaDrmBridge::NativeAddKeySystemUuidMapping(
    /* [in] */ const String& keySystem,
    /* [in] */ IByteBuffer* uuid)
{
    Elastos_MediaDrmBridge_nativeAddKeySystemUuidMapping(keySystem, TO_IINTERFACE(uuid));
}

//callback function definition
Boolean MediaDrmBridge::SetSecurityLevel(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& securityLevel)
{
    AutoPtr<MediaDrmBridge> mObj = (MediaDrmBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaDrmBridge::SetSecurityLevel, mObj is NULL");
        return FALSE;
    }
    return mObj->SetSecurityLevel(securityLevel);
}

AutoPtr<IInterface> MediaDrmBridge::GetMediaCrypto(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaDrmBridge> mObj = (MediaDrmBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaDrmBridge::GetMediaCrypto, mObj is NULL");
        return NULL;
    }
    return mObj->GetMediaCrypto();
}

void MediaDrmBridge::ResetDeviceCredentials(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaDrmBridge> mObj = (MediaDrmBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaDrmBridge::ResetDeviceCredentials, mObj is NULL");
        return;
    }
    mObj->ResetDeviceCredentials();
}

void MediaDrmBridge::ReleaseResources(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaDrmBridge> mObj = (MediaDrmBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaDrmBridge::ReleaseResources, mObj is NULL");
        return;
    }
    mObj->ReleaseResources();
}

void MediaDrmBridge::CreateSession(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 sessionId,
    /* [in] */ ArrayOf<Byte>* initData,
    /* [in] */ const String& mime)
{
    AutoPtr<MediaDrmBridge> mObj = (MediaDrmBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaDrmBridge::CreateSession, mObj is NULL");
        return;
    }
    mObj->CreateSession(sessionId, initData, mime);
}

void MediaDrmBridge::ReleaseSession(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 sessionId)
{
    AutoPtr<MediaDrmBridge> mObj = (MediaDrmBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaDrmBridge::ReleaseSession, mObj is NULL");
        return;
    }
    mObj->ReleaseSession(sessionId);
}

void MediaDrmBridge::UpdateSession(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 sessionId,
    /* [in] */ ArrayOf<Byte>* key)
{
    AutoPtr<MediaDrmBridge> mObj = (MediaDrmBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaDrmBridge::UpdateSession, mObj is NULL");
        return;
    }
    mObj->UpdateSession(sessionId, key);
}

String MediaDrmBridge::GetSecurityLevel(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaDrmBridge> mObj = (MediaDrmBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaDrmBridge::GetSecurityLevel, mObj is NULL");
        return String(NULL);
    }
    return mObj->GetSecurityLevel();
}


} // namespace Media
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
