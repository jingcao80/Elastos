
#ifndef __Elastos_DevSamples_SecurityDemo_CActivityOne_H__
#define __Elastos_DevSamples_SecurityDemo_CActivityOne_H__

#include <elastos/droid/app/Activity.h>
#include "_Elastos_DevSamples_SecurityDemo_CActivityOne.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;

namespace Elastos {
namespace DevSamples {
namespace SecurityDemo {

CarClass(CActivityOne)
    , public Activity
{
public:
    class MyListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CActivityOne* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

protected:

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

private:
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

    CARAPI Button2Function();// MAC test
    CARAPI Button3Function();// MessageDigest
    CARAPI Button4Function();// KeyPairGenerator
    CARAPI Button5Function();// KeyFactory
    CARAPI Button6Function();// KeyAgreement test
    CARAPI Button7Function();// CertificateFactory

    CARAPI ButtonCSecureRandom();
    CARAPI ButtonElastosKeyPairGenerator();
    CARAPI ButtonElastosKeyStore();

    CARAPI SignatureTest();

    CARAPI CipherTest();

private:
    AutoPtr<ArrayOf<Byte> > mRSAPublicEncoded;
    AutoPtr<ArrayOf<Byte> > mRSAPrivateEncoded;

    AutoPtr<ArrayOf<Byte> > mDHPublicEncoded;
    AutoPtr<ArrayOf<Byte> > mDHPrivateEncoded;
};

} // namespace SecurityDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_SecurityDemo_CActivityOne_H__
