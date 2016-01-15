
#ifndef __ELASTOS_DROID_MEDIA_CMEDIASCANNERCONNECTION_H__
#define __ELASTOS_DROID_MEDIA_CMEDIASCANNERCONNECTION_H__

#include "_Elastos_Droid_Media_CMediaScannerConnection.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Media::IIMediaScannerService;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBinder;


namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaScannerConnection)
    , public Object
    , public IMediaScannerConnection
    , public IServiceConnection
{
public:
    class MyMediaScannerListener
        : public Object
        , public IIMediaScannerListener
    {
    public:
        CAR_INTERFACE_DECL();

        MyMediaScannerListener(
            /* [in] */ IWeakReference* host);

        CARAPI ScanCompleted(
            /* [in] */ const String& path,
            /* [in] */ IUri* uri);

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

    /*static*/ class ClientProxy
        : public Object
        , public IMediaScannerConnectionClient
    {
    friend class CMediaScannerConnection;

    public:
        CAR_INTERFACE_DECL();

        ClientProxy(
            /* [in] */ ArrayOf<String>* paths,
            /* [in] */ ArrayOf<String>* mimeTypes,
            /* [in] */ IOnScanCompletedListener* client);

        CARAPI OnMediaScannerConnected();

        CARAPI OnScanCompleted(
            /* [in] */ const String& path,
            /* [in] */ IUri* uri);

        CARAPI_(void) ScanNextPath();

    private:
        AutoPtr< ArrayOf<String> > mPaths;
        AutoPtr< ArrayOf<String> > mMimeTypes;

        AutoPtr<IOnScanCompletedListener> mClient;
        AutoPtr<IMediaScannerConnection> mConnection;

        Int32 mNextPath;
    };

    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CMediaScannerConnection();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IMediaScannerConnectionClient* client);

    /**
     * Initiates a connection to the media scanner service.
     * {@link MediaScannerConnectionClient#onMediaScannerConnected()}
     * will be called when the connection is established.
     */
    CARAPI Connect();

    /**
     * Releases the connection to the media scanner service.
     */
    CARAPI Disconnect();

    /**
     * Returns whether we are connected to the media scanner service
     * @return true if we are connected, false otherwise
     */
    /*synchronized*/
    CARAPI IsConnected(
        /* [out] */ Boolean* result);

    /**
     * Requests the media scanner to scan a file.
     * Success or failure of the scanning operation cannot be determined until
     * {@link MediaScannerConnectionClient#onScanCompleted(String, Uri)} is called.
     *
     * @param path the path to the file to be scanned.
     * @param mimeType  an optional mimeType for the file.
     * If mimeType is NULL, then the mimeType will be inferred from the file extension.
     */
    CARAPI ScanFile(
        /* [in] */ const String& path,
        /* [in] */ const String& mimeType);

    /**
     * Part of the ServiceConnection interface.  Do not call.
     */
    CARAPI OnServiceConnected(
        /* [in] */ IComponentName* className,
        /* [in] */ IBinder* sevice);

    /**
     * Part of the ServiceConnection interface.  Do not call.
     */
    CARAPI OnServiceDisconnected(
        /* [in] */ IComponentName* className);

    /**
     * Convenience for constructing a {@link MediaScannerConnection}, calling
     * {@link #connect} on it, and calling {@link #scanFile} with the given
     * <var>path</var> and <var>mimeType</var> when the connection is
     * established.
     * @param context The caller's Context, required for establishing a connection to
     * the media scanner service.
     * Success or failure of the scanning operation cannot be determined until
     * {@link MediaScannerConnectionClient#onScanCompleted(String, Uri)} is called.
     * @param paths Array of paths to be scanned.
     * @param mimeTypes Optional array of MIME types for each path.
     * If mimeType is NULL, then the mimeType will be inferred from the file extension.
     * @param callback Optional callback through which you can receive the
     * scanned URI and MIME type; If NULL, the file will be scanned but
     * you will not get a result back.
     * @see scanFile(String, String)
     */
    static CARAPI ScanFile(
        /* [in] */ IContext* context,
        /* [in] */ ArrayOf<String>* paths,
        /* [in] */ ArrayOf<String>* mimeTypes,
        /* [in] */ IOnScanCompletedListener* cb);

private:
    static const String TAG; // = "MediaScannerConnection";

    AutoPtr<IContext> mContext;

    AutoPtr<IMediaScannerConnectionClient> mClient;

    AutoPtr<IIMediaScannerService> mService;

    Boolean mConnected; // true if connect() has been called since last disconnect()

    AutoPtr<IIMediaScannerListener> mListener; // = new IMediaScannerListener.Stub();

    Object mThisLock;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIASCANNERCONNECTION_H__
