
#ifndef __ELASTOS_DROID_LOCATION_COUNTRYDETECTOR_H__
#define __ELASTOS_DROID_LOCATION_COUNTRYDETECTOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkhash.h"
#include <elastos/core/Runnable.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Location::ICountryListener;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Core::Runnable;
using Elastos::Core::Object;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Location {

/**
 * This class provides access to the system country detector service. This
 * service allows applications to obtain the country that the user is in.
 * <p>
 * The country will be detected in order of reliability, like
 * <ul>
 * <li>Mobile network</li>
 * <li>Location</li>
 * <li>SIM's country</li>
 * <li>Phone's locale</li>
 * </ul>
 * <p>
 * Call the {@link #detectCountry()} to get the available country immediately.
 * <p>
 * To be notified of the future country change, use the
 * {@link #addCountryListener}
 * <p>
 * <p>
 * You do not instantiate this class directly; instead, retrieve it through
 * {@link android.content.Context#getSystemService
 * Context.getSystemService(Context.COUNTRY_DETECTOR)}.
 * <p>
 * Both ACCESS_FINE_LOCATION and ACCESS_COARSE_LOCATION permissions are needed.
 *
 * @hide
 */
class CountryDetector
    : public Object
    , public ICountryDetector
{
private:
    /**
     * The class to wrap the ICountryListener.Stub and CountryListener objects
     * together. The CountryListener will be notified through the specific
     * looper once the country changed and detected.
     */
    class CountryDetectorListenerTransport
        : public Object
        , public ICountryDetectorListenerTransport
        , public IICountryListener
    {
    public:
        CAR_INTERFACE_DECL()

        CountryDetectorListenerTransport(
            /* [in] */ ICountryListener* listener,
            /* [in] */ ILooper* looper);

        CARAPI OnCountryDetected(
            /* [in] */ ICountry* country);

        class CountryDetectorListenerTransportRunnnable
            : public Runnable
        {
        public:
            CountryDetectorListenerTransportRunnnable(
                /* [in] */ ICountry* country,
                /* [in] */ CountryDetectorListenerTransport* host);

            CARAPI Run();

        private:
            AutoPtr<ICountry> mCountry;
            CountryDetectorListenerTransport* mHost;
        };

    private:
        AutoPtr<ICountryListener> mListener;
        AutoPtr<IHandler> mHandler;
    };

public:
    CAR_INTERFACE_DECL()

    CountryDetector();
    /**
     * @hide - hide this constructor because it has a parameter of type
     *       ICountryDetector, which is a system private class. The right way to
     *       create an instance of this class is using the factory
     *       Context.getSystemService.
     */
    CARAPI constructor(
        /* [in] */ IICountryDetector* service);

    /**
     * Start detecting the country that the user is in.
     *
     * @return the country if it is available immediately, otherwise null will
     *         be returned.
     */
    CARAPI DetectCountry(
        /* [out] */ ICountry** country);

    /**
     * Add a listener to receive the notification when the country is detected
     * or changed.
     *
     * @param listener will be called when the country is detected or changed.
     * @param looper a Looper object whose message queue will be used to
     *        implement the callback mechanism. If looper is null then the
     *        callbacks will be called on the main thread.
     */
   CARAPI AddCountryListener(
       /* [in] */ ICountryListener* listener,
       /* [in] */ ILooper* looper);

    /**
     * Remove the listener
     */
    CARAPI RemoveCountryListener(
        /* [in] */ ICountryListener* listener);

private:
    static const String TAG;
    AutoPtr<IICountryDetector> mService;
    AutoPtr<IHashMap> mListeners;
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_LOCATION_COUNTRYDETECTOR_H__
