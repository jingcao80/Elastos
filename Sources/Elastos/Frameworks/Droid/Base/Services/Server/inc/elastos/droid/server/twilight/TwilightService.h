
#ifndef __ELASTOS_DROID_SERVER_TWILIGHT_TWILIGHTSERVICE_H__
#define __ELASTOS_DROID_SERVER_TWILIGHT_TWILIGHTSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include <Elastos.Droid.App.h>
#include "elastos/droid/server/SystemService.h"
#include "elastos/droid/server/TwilightCalculator.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/utility/etl/List.h>
#include <Elastos.Droid.Location.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Os.h>

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Location::ILocationListener;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Server::SystemService;
using Elastos::Droid::Server::TwilightCalculator;
using Elastos::Utility::Etl::List;


namespace Elastos {
namespace Droid {
namespace Server {
namespace Twilight {

class TwilightService
    : public SystemService
{
private:
    class TwilightListenerRecord
        : public Runnable
    {
    public:
        TO_STRING_IMPL("TwilightService::TwilightListenerRecord")

        TwilightListenerRecord(
            /* [in] */ ITwilightListener* listener,
            /* [in] */ IHandler* handler);

        CARAPI PostUpdate();

        //@Override
        CARAPI Run();
    private:
        AutoPtr<ITwilightListener> mListener;
        AutoPtr<IHandler> mHandler;
    };

    class MyTwilightManager
        : public Object
        , public ITwilightManager
    {
    public:
        TO_STRING_IMPL("TwilightService::MyTwilightManager")

        CAR_INTERFACE_DECL()

        MyTwilightManager(
            /* [in] */ TwilightService* host)
            : mHost(host)
        {}

        /**
         * Gets the current twilight state.
         *
         * @return The current twilight state, or null if no information is available.
         */
        //@Override
        CARAPI GetCurrentState(
            /* [out] */ ITwilightState** state);

        /**
         * Listens for twilight time.
         *
         * @param listener The listener.
         */
        //@Override
        CARAPI RegisterListener(
            /* [in] */ ITwilightListener* listener,
            /* [in] */ IHandler* handler);

        private:
            TwilightService* mHost;
    };

    class LocationHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("TwilightService::LocationHandler")

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
        TwilightService* mHost;
    };

    class UpdateLocationReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("TwilightService::UpdateLocationReceiver: ")

        UpdateLocationReceiver(
            /* [in] */ TwilightService* owner);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        AutoPtr<TwilightService> mHost;
    };

    // A LocationListener to initialize the network location provider. The location updates
    // are handled through the passive location provider.
    class EmptyLocationListener
        : public Object
        , public ILocationListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("TwilightService::EmptyLocationListener")

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

    class MyLocationListener
        : public Object
        , public ILocationListener
    {
    public:
        TO_STRING_IMPL("TwilightService::MyLocationListener")

        CAR_INTERFACE_DECL()

        MyLocationListener(
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
        TwilightService* mHost;
    };

public:
    TwilightService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI OnStart();

private:
    CARAPI_(void) SetTwilightState(
        /* [in] */ ITwilightState* state);

    // The user has moved if the accuracy circles of the two locations don't overlap.
    static CARAPI_(Boolean) HasMoved(
        /* [in] */ ILocation* from,
        /* [in] */ ILocation* to);

private:
    static const String TAG;
    static const Boolean DEBUG;
    static const String ACTION_UPDATE_TWILIGHT_STATE;

    Object mLock;

    AutoPtr<IAlarmManager> mAlarmManager;
    AutoPtr<ILocationManager> mLocationManager;
    AutoPtr<LocationHandler> mLocationHandler;

    List<AutoPtr<TwilightListenerRecord> > mListeners;

    AutoPtr<ITwilightState> mTwilightState;

    AutoPtr<UpdateLocationReceiver> mUpdateLocationReceiver;

    AutoPtr<EmptyLocationListener> mEmptyLocationListener;

    AutoPtr<MyLocationListener> mLocationListener;

    AutoPtr<ITwilightManager> mService;
};


} // namespace Twilight
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_TWILIGHT_TWILIGHTSERVICE_H__
