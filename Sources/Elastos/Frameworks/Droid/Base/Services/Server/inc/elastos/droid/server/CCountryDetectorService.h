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

#ifndef __ELASTOS_DROID_SERVER_CCOUNTRYDETECTORSERVICE_H__
#define __ELASTOS_DROID_SERVER_CCOUNTRYDETECTORSERVICE_H__

#include "_Elastos_Droid_Server_CCountryDetectorService.h"
#include "elastos/droid/server/location/ComprehensiveCountryDetector.h"
#include "elastos/droid/ext/frameworkext.h"
#define HASH_FOR_OS
#include "elastos/droid/ext/frameworkhash.h"
#include <elastos/core/Runnable.h>
#include <elastos/utility/etl/HashMap.h>
#include <Elastos.Droid.Content.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::Runnable;
using Elastos::Droid::Location::IICountryDetector;
using Elastos::Droid::Location::ICountryListener;
using Elastos::Droid::Location::IICountryListener;
using Elastos::Droid::Location::ICountry;
using Elastos::Droid::Server::Location::ComprehensiveCountryDetector;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CCountryDetectorService)
    , public Object
    , public IICountryDetector
    , public IBinder
{
private:
    class InnerRunnable
        : public Runnable
    {
    public:
        InnerRunnable(
            /* [in] */ CCountryDetectorService* host);

        CARAPI Run();

    private:
        CCountryDetectorService* mHost;
    };

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
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

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
        : public Object
        , public ICountryListener
    {
    public:
        CAR_INTERFACE_DECL()

        LocationBasedDetectorListener(
            /* [in] */ CCountryDetectorService* host);

        CARAPI OnCountryDetected(
            /* [in] */ ICountry* country);

    private:
        CCountryDetectorService* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

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

    virtual CARAPI SystemRunning();

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
    AutoPtr<ComprehensiveCountryDetector> mCountryDetector;
    Boolean mSystemReady;
    AutoPtr<IHandler> mHandler;
    AutoPtr<ICountryListener> mLocationBasedDetectorListener;
};

}
}
}

#endif //__ELASTOS_DROID_SERVER_CCOUNTRYDETECTORSERVICE_H__
