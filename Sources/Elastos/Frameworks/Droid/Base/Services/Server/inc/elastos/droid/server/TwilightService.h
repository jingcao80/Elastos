
#ifndef __ELASTOS_DROID_SERVER_TWILIGHTSERVICE_H__
#define __ELASTOS_DROID_SERVER_TWILIGHTSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "TwilightCalculator.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/os/HandlerBase.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Location::ILocationListener;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Server::TwilightCalculator;


namespace Elastos {
namespace Droid {
namespace Server {

class TwilightService
    : public ElRefBase
{
public:
    /**
     * Describes whether it is day or night.
     * This object is immutable.
     */
    class TwilightState
        : public ElRefBase
    {
    public:
        TwilightState(
            /* [in] */ Boolean isNight,
            /* [in] */ Int64 yesterdaySunset,
            /* [in] */ Int64 todaySunrise,
            /* [in] */ Int64 todaySunset,
            /* [in] */ Int64 tomorrowSunrise);

        /**
         * Returns true if it is currently night time.
         */
        CARAPI_(Boolean) IsNight();

        /**
         * Returns the time of yesterday's sunset in the System.currentTimeMillis() timebase,
         * or -1 if the sun never sets.
         */
        CARAPI_(Int64) GetYesterdaySunset();

        /**
         * Returns the time of today's sunrise in the System.currentTimeMillis() timebase,
         * or -1 if the sun never rises.
         */
        CARAPI_(Int64) GetTodaySunrise();

        /**
         * Returns the time of today's sunset in the System.currentTimeMillis() timebase,
         * or -1 if the sun never sets.
         */
        CARAPI_(Int64) GetTodaySunset();

        /**
         * Returns the time of tomorrow's sunrise in the System.currentTimeMillis() timebase,
         * or -1 if the sun never rises.
         */
        CARAPI_(Int64) GetTomorrowSunrise();

        CARAPI_(Boolean) Equals(
            /* [in] */ TwilightState* other);

        CARAPI_(Int32) GetHashCode();

        CARAPI_(String) ToString();

    private:
        Boolean mIsNight;
        Int64 mYesterdaySunset;
        Int64 mTodaySunrise;
        Int64 mTodaySunset;
        Int64 mTomorrowSunrise;
    };

    /**
     * Listener for changes in twilight state.
     */
    interface ITwilightListener
        : public IInterface
    {
    public:
        virtual CARAPI OnTwilightStateChanged() = 0;
    };

private:
    class TwilightListenerRecord
        : public Runnable
    {
    public:
        TwilightListenerRecord(
            /* [in] */ ITwilightListener* listener,
            /* [in] */ IHandler* handler);

        CARAPI Post();

        //@Override
        CARAPI Run();
    private:
        AutoPtr<ITwilightListener> mListener;
        AutoPtr<IHandler> mHandler;
    };

    class LocationHandler
        : public HandlerBase
    {
    public:
        LocationHandler(
            /* [in] */ TwilightService* owner);

        CARAPI ProcessNewLocation(
            /* [in] */ ILocation* location);

        CARAPI EnableLocationUpdates();

        CARAPI RequestLocationUpdate();

        CARAPI RequestTwilightUpdate();

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CARAPI_(void) RetrieveLocation();

        CARAPI_(void) SetLocation(
            /* [in] */ ILocation* location);

        CARAPI_(void) UpdateTwilightState();

    private:
        static const Int32 MSG_ENABLE_LOCATION_UPDATES; // = 1;
        static const Int32 MSG_GET_NEW_LOCATION_UPDATE; // = 2;
        static const Int32 MSG_PROCESS_NEW_LOCATION; // = 3;
        static const Int32 MSG_DO_TWILIGHT_UPDATE; // = 4;

        static const Int64 LOCATION_UPDATE_MS; // = 24 * 1000 * 60 * 60 ; //DateUtils.HOUR_IN_MILLIS;
        static const Int64 MIN_LOCATION_UPDATE_MS; // = 30 * 1000 * 60 ; //DateUtils.MINUTE_IN_MILLIS;
        static const Float LOCATION_UPDATE_DISTANCE_METER; // = 1000 * 20;
        static const Int64 LOCATION_UPDATE_ENABLE_INTERVAL_MIN; // = 5000;
        static const Int64 LOCATION_UPDATE_ENABLE_INTERVAL_MAX; // = 15 * 1000 * 60 ; //DateUtils.MINUTE_IN_MILLIS;
        static const Double FACTOR_GMT_OFFSET_LONGITUDE; // = 1000.0 * 360.0 / 1000 * 60 * 60 * 24 ; //DateUtils.DAY_IN_MILLIS;

        Boolean mPassiveListenerEnabled;
        Boolean mNetworkListenerEnabled;
        Boolean mDidFirstInit;
        Int64 mLastNetworkRegisterTime;
        Int64 mLastUpdateInterval;
        AutoPtr<ILocation> mLocation;
        AutoPtr<TwilightCalculator> mTwilightCalculator;

        AutoPtr<IHandler> mHandler;
        AutoPtr<TwilightService> mOwner;
    };

    class UpdateLocationReceiver
        : public BroadcastReceiver
    {
    public:
        UpdateLocationReceiver(
            /* [in] */ TwilightService* owner);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("TwilightService::UpdateLocationReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        AutoPtr<TwilightService> mOwner;
    };

    // A LocationListener to initialize the network location provider. The location updates
    // are handled through the passive location provider.
    class EmptyLocationListener
        : public ElRefBase
        , public ILocationListener
    {
    public:
        CAR_INTERFACE_DECL();

        EmptyLocationListener();


        CARAPI OnLocationChanged(
            /* [in] */ ILocation* location);

        CARAPI OnProviderEnabled(
            /* [in] */ const String& provider);

        CARAPI OnProviderDisabled(
            /* [in] */ const String& provider);

        CARAPI OnStatusChanged(
            /* [in] */ const String& provider,
            /* [in] */ Int32 status,
            /* [in] */ IBundle* extras);
    };

    class _LocationListener
        : public ElRefBase
        , public ILocationListener
    {
    public:
        CAR_INTERFACE_DECL();

        _LocationListener(
            /* [in] */ TwilightService* owner);


        CARAPI OnLocationChanged(
            /* [in] */ ILocation* location);

        CARAPI OnProviderEnabled(
            /* [in] */ const String& provider);

        CARAPI OnProviderDisabled(
            /* [in] */ const String& provider);

        CARAPI OnStatusChanged(
            /* [in] */ const String& provider,
            /* [in] */ Int32 status,
            /* [in] */ IBundle* extras);
    private:
        AutoPtr<TwilightService> mOwner;
    };

public:
    TwilightService(
        /* [in] */ IContext* context);

    /**
     * Gets the current twilight state.
     *
     * @return The current twilight state, or null if no information is available.
     */
    CARAPI GetCurrentState(
        /* [out] */ TwilightState** result);

    /**
     * Listens for twilight time.
     *
     * @param listener The listener.
     * @param handler The handler on which to post calls into the listener.
     */
    CARAPI RegisterListener(
        /* [in] */ ITwilightListener* listener,
        /* [in] */ IHandler* handler);

    CARAPI_(void) SystemReady();

private:
    CARAPI_(void) SetTwilightState(
        /* [in] */ TwilightState* state);

    // The user has moved if the accuracy circles of the two locations don't overlap.
    static CARAPI_(Boolean) HasMoved(
        /* [in] */ ILocation* from,
        /* [in] */ ILocation* to);

private:
    static const String TAG;
    static const Boolean DEBUG;
    static const String ACTION_UPDATE_TWILIGHT_STATE;

    AutoPtr<IContext> mContext;
    AutoPtr<IAlarmManager> mAlarmManager;
    AutoPtr<ILocationManager> mLocationManager;
    AutoPtr<LocationHandler> mLocationHandler;

    Object mLock;

    List<AutoPtr<TwilightListenerRecord> > mListeners;
    Boolean mSystemReady;

    AutoPtr<TwilightState> mTwilightState;

    AutoPtr<UpdateLocationReceiver> mUpdateLocationReceiver;

    AutoPtr<EmptyLocationListener> mEmptyLocationListener;

    AutoPtr<_LocationListener> mLocationListener;
};



} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_TWILIGHTSERVICE_H__
