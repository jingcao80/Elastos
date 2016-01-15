
#ifndef __ELASTOS_DROID_SERVER_CCOUNTRYDETECTORSERVICE_H__
#define __ELASTOS_DROID_SERVER_CCOUNTRYDETECTORSERVICE_H__

#include "_Elastos_Droid_Server_CCountryDetectorService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Runnable.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Location::ICountryListener;
using Elastos::Droid::Location::IICountryListener;
using Elastos::Droid::Location::ICountry;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CCountryDetectorService)
{
public:
    class CountryDetectedRunnable
        : public Runnable
    {
    public:
        CountryDetectedRunnable(
            /* [in] */ ICountry* country,
            /* [in] */ CCountryDetectorService* service)
            : mCountry(country)
            , mHost(service)
        {}

        CARAPI Run();

    private:
        AutoPtr<ICountry> mCountry;
        CCountryDetectorService* mHost;
    };

    class SetCountryListenerRunnable
        : public Runnable
    {
    public:
        SetCountryListenerRunnable(
            /* [in] */ ICountryListener* listener,
            /* [in] */ CCountryDetectorService* service)
            : mListener(listener)
            , mHost(service)
        {}

        CARAPI Run();

    private:
        AutoPtr<ICountryListener> mListener;
        CCountryDetectorService* mHost;
    };

    /**
     * The class represents the remote listener, it will also removes itself
     * from listener list when the remote process was died.
     */
    class Receiver
        : public ElRefBase
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL();

        Receiver(
            /* [in] */ IICountryListener* listener,
            /* [in] */ CCountryDetectorService* owner);

        CARAPI ProxyDied();

        CARAPI GetListener(
            /* [out] */ IICountryListener **listener);

    private:
        AutoPtr<IICountryListener> mListener;
        AutoPtr<IBinder> mKey;
        CCountryDetectorService* mHost;
    };

    class LocationBasedDetectorListener
        : public ElRefBase
        , public ICountryListener
    {
    public:
        CAR_INTERFACE_DECL();

        LocationBasedDetectorListener(
            /* [in] */ CCountryDetectorService* host);

        CARAPI OnCountryDetected(
            /* [in] */ ICountry* country);

    private:
        CCountryDetectorService* mHost;
    };

public:
    CARAPI constructor(
        /* [in] */ IContext *context);

    //@Override
    CARAPI DetectCountry(
        /* [out] */  ICountry** country);

    /**
     * Add the ICountryListener into the listener list.
     */
    //@Override
    CARAPI AddCountryListener(
        /* [in] */ IICountryListener* listener);

    /**
     * Remove the ICountryListener from the listener list.
     */
    //@Override
    CARAPI RemoveCountryListener(
        /* [in] */ IICountryListener *listener);

    CARAPI Run();

    CARAPI ToString(
        /* [out] */ String* str);

    virtual CARAPI NotifyReceivers(
        /* [in] */ ICountry *country);

    virtual CARAPI SystemReady();

    virtual CARAPI SetCountryListener(
        /* [in] */ ICountryListener *listener);

    // For testing
    virtual CARAPI IsSystemReady(
        /* [out] */ Boolean *isReady);

    //@SuppressWarnings("unused")
    //@Override
//    CARAPI Dump(
//        /* [in] */ IFileDescriptor *fd,
//        /* [in] */ IPrintWriter *fout,
//        /* [in] */ ArrayOf<String> *args);

private:
    CARAPI AddListener(
        /* [in] */ IICountryListener *listener);

    CARAPI RemoveListener(
        /* [in] */ IBinder *key);

    CARAPI Initialize();

private:
    const static String TAG;

    /** Whether to dump the state of the country detector service to bugreports */
    const static Boolean DEBUG;

    HashMap<AutoPtr<IBinder>, AutoPtr<Receiver> > mReceivers;
    Object mReceiverslock;

    AutoPtr<IContext> mContext;
    //AutoPtr<IComprehensiveCountryDetector> mCountryDetector;
    Boolean mSystemReady;
    AutoPtr<IHandler> mHandler;
    AutoPtr<ICountryListener> mLocationBasedDetectorListener;

};

}
}
}

#endif //__ELASTOS_DROID_SERVER_CCOUNTRYDETECTORSERVICE_H__
