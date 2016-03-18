#ifndef __ELASTOS_DROID_MEDIA_MEDIASCANNERCONNECTION_H__
#define __ELASTOS_DROID_MEDIA_MEDIASCANNERCONNECTION_H__

namespace Elastos {
namespace Droid {
namespace Media {

class MediaScannerConnection
    : public Object
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


public:
    MediaScannerConnection();

    virtual ~MediaScannerConnection();

    CAR_INTERFACE_DECL()

};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif __ELASTOS_DROID_MEDIA_MEDIASCANNERCONNECTION_H__