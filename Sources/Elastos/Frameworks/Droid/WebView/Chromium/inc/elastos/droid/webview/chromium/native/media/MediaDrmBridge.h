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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_MEDIA_MEDIADRMBRIDGE_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_MEDIA_MEDIADRMBRIDGE_H__

#include "Elastos.Droid.Media.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/AsyncTask.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Media::IMediaCrypto;
using Elastos::Droid::Media::IMediaDrmOnEventListener;
using Elastos::Droid::Media::EIID_IMediaDrmOnEventListener;
using Elastos::Droid::Media::IMediaDrm;
using Elastos::Droid::Media::IMediaDrmKeyRequest;

using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::IRunnable;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::IUUID;
using Elastos::IO::IByteBuffer;

using Elastos::Utility::Etl::HashMap;

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::IO::IByteBuffer);

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Media {

/**
 * A wrapper of the android MediaDrm class. Each MediaDrmBridge manages multiple
 * sessions for a single MediaSourcePlayer.
 */
//@JNINamespace("media")
class MediaDrmBridge
    : public Object
{
private:
    /**
     *  This class contains data needed to call createSession().
     */
    class PendingCreateSessionData
        : public Object
    {
    public:
        PendingCreateSessionData(
            /* [in] */ Int32 sessionId,
            /* [in] */ ArrayOf<Byte>* initData,
            /* [in] */ const String& mimeType);

        CARAPI_(Int32) SessionId();

        CARAPI_(AutoPtr<ArrayOf<Byte> >) InitData();

        CARAPI_(String) MimeType();

    private:
        const Int32 mSessionId;
        const AutoPtr<ArrayOf<Byte> > mInitData;
        const String mMimeType;
    };

    class MediaDrmListener
        : public Object
        , public IMediaDrmOnEventListener
    {
    public:
        CAR_INTERFACE_DECL()
        MediaDrmListener(
            /* [in] */ MediaDrmBridge* owner);

        //@Override
        CARAPI OnEvent(
            /* [in] */ IMediaDrm* mediaDrm,
            /* [in] */ ArrayOf<Byte>* session_array,
            /* [in] */ Int32 event,
            /* [in] */ Int32 extra,
            /* [in] */ ArrayOf<Byte>* data);

    private:
        MediaDrmBridge* mOwner;
    };

    class PostRequestTask
        : public AsyncTask
    {
    public:
        PostRequestTask(
            /* [in] */ MediaDrmBridge* owner,
            /* [in] */ ArrayOf<Byte>* drmRequest);

    protected:
        //@Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

        //@Override
        CARAPI OnPostExecute(
            /* [in] */ IInterface* result);

    private:
        CARAPI_(AutoPtr<ArrayOf<Byte> >) PostRequest(
            /* [in] */ const String& url,
            /* [in] */ ArrayOf<Byte>* drmRequest);

    private:
        static const String MDB_PTAG;
        MediaDrmBridge* mOwner;

        AutoPtr<ArrayOf<Byte> > mDrmRequest;
        AutoPtr<ArrayOf<Byte> > mResponseBody;
    };

    class ResumePendingOperationsRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()
        ResumePendingOperationsRunnable(
            /* [in] */ MediaDrmBridge* owner);

        CARAPI Run();

    private:
        MediaDrmBridge* mOwner;
    };

    class OnSessionCreatedRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()
        OnSessionCreatedRunnable(
            /* [in] */ MediaDrmBridge* owner,
            /* [in] */ Int32 sessionId,
            /* [in] */ const String& webSessionId);

        CARAPI Run();

    private:
        MediaDrmBridge* mOwner;
        Int32 mSessionId;
        const String mWebSessionId;
    };

    class OnSessionMessageRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()
        OnSessionMessageRunnable(
            /* [in] */ MediaDrmBridge* owner,
            /* [in] */ Int32 sessionId,
            /* [in] */ IMediaDrmKeyRequest* request);

        CARAPI Run();

    private:
        MediaDrmBridge* mOwner;
        Int32 mSessionId;
        AutoPtr<IMediaDrmKeyRequest> mRequest;
    };

    class OnSessionReadyRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()
        OnSessionReadyRunnable(
            /* [in] */ MediaDrmBridge* owner,
            /* [in] */ Int32 sessionId);

        CARAPI Run();

    private:
        MediaDrmBridge* mOwner;
        Int32 mSessionId;
    };

    class OnSessionClosedRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()
        OnSessionClosedRunnable(
            /* [in] */ MediaDrmBridge* owner,
            /* [in] */ Int32 sessionId);

        CARAPI Run();

    private:
        MediaDrmBridge* mOwner;
        Int32 mSessionId;
    };

    class OnSessionErrorRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        OnSessionErrorRunnable(
            /* [in] */ MediaDrmBridge* owner,
            /* [in] */ Int32 sessionId);

        CARAPI Run();

    private:
        MediaDrmBridge* mOwner;
        Int32 mSessionId;
    };

public:
    /**
     * Provide the provisioning response to MediaDrm.
     * @returns false if the response is invalid or on error, true otherwise.
     */
    CARAPI_(Boolean) ProvideProvisionResponse(
        /* [in] */ ArrayOf<Byte>* response);

    static CARAPI_(void) AddKeySystemUuidMapping(
        /* [in] */ const String& keySystem,
        /* [in] */ IUUID* uuid);

private:
    MediaDrmBridge(
        /* [in] */ IUUID* schemeUUID,
        /* [in] */ HANDLE nativeMediaDrmBridge,
        /* [in] */ Boolean singleSessionMode);

    static CARAPI_(AutoPtr<IUUID>) GetUUIDFromBytes(
        /* [in] */ ArrayOf<Byte>* data);

    /**
     *  Gets session associated with the sessionId.
     *
     *  @return session if sessionId maps a valid opened session. Returns null
     *  otherwise.
     */
    CARAPI_(AutoPtr<IByteBuffer>) GetSession(
        /* [in] */ Int32 sessionId);

    /**
     * Create a MediaCrypto object.
     *
     * @return whether a MediaCrypto object is successfully created.
     */
    CARAPI_(Boolean) CreateMediaCrypto();

    /**
     * Open a new session..
     *
     * @return the session opened. Returns null if unexpected error happened.
     */
    CARAPI_(AutoPtr<IByteBuffer>) OpenSession();

    /**
     * Close a session.
     *
     * @param session to be closed.
     */
    CARAPI_(void) CloseSession(
        /* [in] */ IByteBuffer* session);

    /**
     * Check whether the crypto scheme is supported for the given container.
     * If |containerMimeType| is an empty string, we just return whether
     * the crypto scheme is supported.
     *
     * @return true if the container and the crypto scheme is supported, or
     * false otherwise.
     */
    //@CalledByNative
    static CARAPI_(Boolean) IsCryptoSchemeSupported(
        /* [in] */ ArrayOf<Byte>* schemeUUID,
        /* [in] */ const String& containerMimeType);

    /**
     * Create a new MediaDrmBridge from the crypto scheme UUID.
     *
     * @param schemeUUID Crypto scheme UUID.
     * @param securityLevel Security level to be used.
     * @param nativeMediaDrmBridge Native object of this class.
     */
    //@CalledByNative return MediaDrmBridge
    static CARAPI_(AutoPtr<IInterface>) Create(
        /* [in] */ ArrayOf<Byte>* schemeUUID,
        /* [in] */ HANDLE nativeMediaDrmBridge);

    /**
     * Set the security level that the MediaDrm object uses.
     * This function should be called right after we construct MediaDrmBridge
     * and before we make any other calls.
     */
    //@CalledByNative
    CARAPI_(Boolean) SetSecurityLevel(
        /* [in] */ const String& securityLevel);

    /**
     * Return the MediaCrypto object if available.
     */
    //@CalledByNative
    CARAPI_(AutoPtr<IMediaCrypto>) GetMediaCrypto();

    /**
     * Reset the device DRM credentials.
     */
    //@CalledByNative
    CARAPI_(void) ResetDeviceCredentials();

    /**
     * Release the MediaDrmBridge object.
     */
    //@CalledByNative
    CARAPI_(void) ReleaseResources();

    /**
     * Get a key request.
     *
     * @param session Session on which we need to get the key request.
     AutoPtr<key request.
     * @param mime Mime type to get the key request.
     *
     * @return the key request.
     */
    CARAPI GetKeyRequest(
        /* [in] */ IByteBuffer* session,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ const String& mime,
        /* [out] */ IMediaDrmKeyRequest** result);

    /**
     * Save data to |mPendingCreateSessionDataQueue| so that we can resume the
     * createSession() call later.
     */
    CARAPI_(void) SavePendingCreateSessionData(
        /* [in] */ Int32 sessionId,
        /* [in] */ ArrayOf<Byte>* initData,
        /* [in] */ const String& mime);

    /**
     * Process all pending createSession() calls synchronously.
     */
    CARAPI_(void) ProcessPendingCreateSessionData();

    /**
     * Process pending operations asynchrnously.
     */
    CARAPI_(void) ResumePendingOperations();

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
    CARAPI_(void) CreateSession(
        /* [in] */ Int32 sessionId,
        /* [in] */ ArrayOf<Byte>* initData,
        /* [in] */ const String& mime);

    /**
     * Returns whether |sessionId| is a valid key session, excluding the media
     * crypto session in multi-session mode.
     *
     * @param sessionId Crypto session Id.
     */
    CARAPI_(Boolean) SessionExists(
        /* [in] */ IByteBuffer* session);

    /**
     * Cancel a key request for a session Id.
     *
     * @param sessionId Reference ID of session to be released.
     */
    //@CalledByNative
    CARAPI_(void) ReleaseSession(
        /* [in] */ Int32 sessionId);

    /**
     * Add a key for a session Id.
     *
     * @param sessionId Reference ID of session to be updated.
     * @param key Response data from the server.
     */
    //@CalledByNative
    CARAPI_(void) UpdateSession(
        /* [in] */ Int32 sessionId,
        /* [in] */ ArrayOf<Byte>* key);

    /**
     * Return the security level of this DRM object.
     */
    //@CalledByNative
    CARAPI_(String) GetSecurityLevel();

    CARAPI_(void) StartProvisioning();

    /**
     * Called when the provision response is received.
     *
     * @param response Response data from the provision server.
     */
    CARAPI_(void) OnProvisionResponse(
        /* [in] */ ArrayOf<Byte>* response);

    CARAPI_(void) OnSessionCreated(
        /* [in] */ Int32 sessionId,
        /* [in] */ const String& webSessionId);

    CARAPI_(void) OnSessionMessage(
        /* [in] */ Int32 sessionId,
        /* [in] */ IMediaDrmKeyRequest* request);

    CARAPI_(void) OnSessionReady(
        /* [in] */ Int32 sessionId);

    CARAPI_(void) OnSessionClosed(
        /* [in] */ Int32 sessionId);

    CARAPI_(void) OnSessionError(
        /* [in] */ Int32 sessionId);

    CARAPI_(String) GetWebSessionId(
        /* [in] */ IByteBuffer* session);

    CARAPI_(void) NativeOnMediaCryptoReady(
        /* [in] */ HANDLE nativeMediaDrmBridge);

    CARAPI_(void) NativeOnSessionCreated(
        /* [in] */ HANDLE nativeMediaDrmBridge,
        /* [in] */ Int32 sessionId,
        /* [in] */ const String& webSessionId);

    CARAPI_(void) NativeOnSessionMessage(
        /* [in] */ HANDLE nativeMediaDrmBridge,
        /* [in] */ Int32 sessionId,
        /* [in] */ ArrayOf<Byte>* message,
        /* [in] */ const String& destinationUrl);

    CARAPI_(void) NativeOnSessionReady(
        /* [in] */ HANDLE nativeMediaDrmBridge,
        /* [in] */ Int32 sessionId);

    CARAPI_(void) NativeOnSessionClosed(
        /* [in] */ HANDLE nativeMediaDrmBridge,
        /* [in] */ Int32 sessionId);

    CARAPI_(void) NativeOnSessionError(
        /* [in] */ HANDLE nativeMediaDrmBridge,
        /* [in] */ Int32 sessionId);

    CARAPI_(void) NativeOnResetDeviceCredentialsCompleted(
        /* [in] */ HANDLE nativeMediaDrmBridge,
        /* [in] */ Boolean success);

    static CARAPI_(void) NativeAddKeySystemUuidMapping(
        /* [in] */ const String& keySystem,
        /* [in] */ IByteBuffer* uuid);

//callback function declaration
public:
    static CARAPI_(void*) ElaMediaDrmBridgeCallback_Init();

private:
    static CARAPI_(Boolean) SetSecurityLevel(
        /* [in] */ IInterface* obj,
        /* [in] */ const String& securityLevel);

    static CARAPI_(AutoPtr<IInterface>) GetMediaCrypto(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) ResetDeviceCredentials(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) ReleaseResources(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) CreateSession(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 sessionId,
        /* [in] */ ArrayOf<Byte>* initData,
        /* [in] */ const String& mime);

    static CARAPI_(void) ReleaseSession(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 sessionId);

    static CARAPI_(void) UpdateSession(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 sessionId,
        /* [in] */ ArrayOf<Byte>* key);

    static CARAPI_(String) GetSecurityLevel(
        /* [in] */ IInterface* obj);

private:
    // Implementation Notes:
    // - A media crypto session (mMediaCryptoSession) is opened after MediaDrm
    //   is created. This session will be added to mSessionIds.
    //   a) In multiple session mode, this session will only be used to create
    //      the MediaCrypto object. It's associated mime type is always null and
    //      it's session ID is always INVALID_SESSION_ID.
    //   b) In single session mode, this session will be used to create the
    //      MediaCrypto object and will be used to call getKeyRequest() and
    //      manage all keys.  The session ID will always be the lastest session
    //      ID passed by the caller.
    // - Each createSession() call creates a new session. All sessions are
    //   managed in mSessionIds.
    // - Whenever NotProvisionedException is thrown, we will clean up the
    //   current state and start the provisioning process.
    // - When provisioning is finished, we will try to resume suspended
    //   operations:
    //   a) Create the media crypto session if it's not created.
    //   b) Finish createSession() if previous createSession() was interrupted
    //      by a NotProvisionedException.
    // - Whenever an unexpected error occurred, we'll call release() to release
    //   all resources and clear all states. In that case all calls to this
    //   object will be no-op. All public APIs and callbacks should check
    //   mMediaBridge to make sure release() hasn't been called. Also, we call
    //   release() immediately after the error happens (e.g. after mMediaDrm)
    //   calls. Indirect calls should not call release() again to avoid
    //   duplication (even though it doesn't hurt to call release() twice).

    static const String TAG;
    static const String SECURITY_LEVEL;
    static const String PRIVACY_MODE;
    static const String SESSION_SHARING;
    static const String ENABLE;
    static const Int32 INVALID_SESSION_ID = 0;

    AutoPtr<IMediaDrm> mMediaDrm;
    HANDLE mNativeMediaDrmBridge;
    AutoPtr<IUUID> mSchemeUUID;
    AutoPtr<IHandler> mHandler;

    // In this mode, we only open one session, i.e. mMediaCryptoSession.
    Boolean mSingleSessionMode;

    // A session only for the purpose of creating a MediaCrypto object.
    // This session is opened when createSession() is called for the first
    // time.
    // - In multiple session mode, all following createSession() calls
    // should create a new session and use it to call getKeyRequest(). No
    // getKeyRequest() should ever be called on this media crypto session.
    // - In single session mode, all createSession() calls use the same
    // media crypto session. When createSession() is called with a new
    // initData, previously added keys may not be available anymore.
    AutoPtr<IByteBuffer> mMediaCryptoSession;
    AutoPtr<IMediaCrypto> mMediaCrypto;

    // The map of all opened sessions to their session reference IDs.
    HashMap<AutoPtr<IByteBuffer>, Int32> mSessionIds;
    // The map of all opened sessions to their mime types.
    HashMap<AutoPtr<IByteBuffer>, String> mSessionMimeTypes;

    // The queue of all pending createSession() data.
    //ArrayDeque<PendingCreateSessionData> mPendingCreateSessionDataQueue;
    AutoPtr<IArrayDeque> mPendingCreateSessionDataQueue;

    Boolean mResetDeviceCredentialsPending;

    // MediaDrmBridge is waiting for provisioning response from the server.
    //
    // Notes about NotProvisionedException: This exception can be thrown in a
    // lot of cases. To streamline implementation, we do not catch it in private
    // non-native methods and only catch it in public APIs.
    Boolean mProvisioningPending;
};

} // namespace Media
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_MEDIA_MEDIADRMBRIDGE_H__
