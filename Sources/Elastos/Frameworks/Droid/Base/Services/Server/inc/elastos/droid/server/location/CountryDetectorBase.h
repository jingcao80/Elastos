
#ifndef __ELASTOS_DROID_SERVER_LOCATION_COUNTRYDETECTORBASE_H__
#define __ELASTOS_DROID_SERVER_LOCATION_COUNTRYDETECTORBASE_H__

#ifdef DROID_CORE
#include "Elastos.Droid.Core_server.h"
#elif defined(DROID_SERVER)
#include "Elastos.Droid.Core.h"
#endif

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Location::ICountry;
using Elastos::Droid::Location::ICountryListener;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::CHandler;
namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

/**
 * This class defines the methods need to be implemented by the country
 * detector.
 * <p>
 * Calling {@link #detectCountry} to start detecting the country. The country
 * could be returned immediately if it is available.
 *
 * @hide
 */
class CountryDetectorBase : public ElRefBase
{
public:
    CountryDetectorBase(
        /* [in] */ IContext* ctx)
    {
        mContext = ctx;
        CHandler::New((IHandler**)&mHandler);
    }

    /**
     * Start detecting the country that the user is in.
     *
     * @return the country if it is available immediately, otherwise null should
     *         be returned.
     */
    virtual CARAPI DetectCountry(
                /* [out] */ ICountry** country) = 0;

    /**
     * Register a listener to receive the notification when the country is detected or changed.
     * <p>
     * The previous listener will be replaced if it exists.
     */
    virtual CARAPI_(void) SetCountryListener(
        /* [in] */ ICountryListener* listener)
    {
        mListener = listener;
    }

    /**
     * Stop detecting the country. The detector should release all system services and be ready to
     * be freed
     */
    virtual CARAPI_(void) Stop() = 0;

protected:

    virtual CARAPI_(void) NotifyListener(
        /* [in] */ ICountry* country)
    {
        if (mListener != NULL) {
            mListener->OnCountryDetected(country);
        }
    }

protected:
    AutoPtr<IHandler> mHandler;
    AutoPtr<IContext> mContext;
    AutoPtr<ICountryListener> mListener;
    AutoPtr<ICountry> mDetectedCountry;
};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_COUNTRYDETECTORBASE_H__
