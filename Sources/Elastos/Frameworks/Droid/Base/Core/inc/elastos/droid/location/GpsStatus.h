
#ifndef __ELASTOS_DROID_LOCATION_GPSSTATUS_H__
#define __ELASTOS_DROID_LOCATION_GPSSTATUS_H__

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Location.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IIterator;
using Elastos::Utility::IIterable;

namespace Elastos {
namespace Droid {
namespace Location {
/**
 * This class represents the current state of the GPS engine.
 * This class is used in conjunction with the {@link Listener} interface.
 */
class GpsStatus
    : public Object
    , public IGpsStatus
{
private:
    class SatelliteIterator
        : public Object
        , public IIterator
    {
    public:
        CAR_INTERFACE_DECL()

        SatelliteIterator(
            /* [in] */ ArrayOf<IGpsSatellite* >* satellites);

        CARAPI HasNext(
            /* [out] */ Boolean* result);

        //@Override
        CARAPI GetNext(
            /* [out] */ IInterface** object);

        CARAPI Remove();

    private:
        AutoPtr<ArrayOf<IGpsSatellite* > > mSatellites;
        Int32 mIndex;
    };

    class StatelliteList
        : public Object
        , public IIterable
    {
    public:
        friend class GpsStatus;

        CAR_INTERFACE_DECL()

        StatelliteList(
            /* [in] */ GpsStatus* host);

        //@Override
        CARAPI GetIterator(
            /* [out] */ IIterator** it);

    private:
        GpsStatus* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    GpsStatus();

    CARAPI constructor();

    /**
     * Used internally within {@link LocationManager} to copy GPS status
     * data from the Location Manager Service to its cached GpsStatus instance.
     * Is synchronized to ensure that GPS status updates are atomic.
     */
    CARAPI SetStatus(
        /* [in] */ Int32 svCount,
        /* [in] */ ArrayOf<Int32>* prns,
        /* [in] */ ArrayOf<Float>* snrs,
        /* [in] */ ArrayOf<Float>* elevations,
        /* [in] */ ArrayOf<Float>* azimuths,
        /* [in] */ Int32 ephemerisMask,
        /* [in] */ Int32 almanacMask,
        /* [in] */ Int32 usedInFixMask);

    /**
     * Used by {@link LocationManager#getGpsStatus} to copy LocationManager's
     * cached GpsStatus instance to the client's copy.
     * Since this method is only used within {@link LocationManager#getGpsStatus},
     * it does not need to be synchronized.
     */
    CARAPI SetStatus(
        /* [in] */ IGpsStatus* status);

    /**
     * Returns the time required to receive the first fix since the most recent
     * restart of the GPS engine.
     *
     * @return time to first fix in milliseconds
     */
    CARAPI SetTimeToFirstFix(
        /* [in] */ Int32 ttff);

    CARAPI GetTimeToFirstFix(
        /* [out] */ Int32* time);

    /**
     * Returns an array of {@link GpsSatellite} objects, which represent the
     * current state of the GPS engine.
     *
     * @return the list of satellites
     */
    CARAPI GetSatellites(
        /* [out] */ IIterable** satellites);

    /**
     * Returns the maximum number of satellites that can be in the satellite
     * list that can be returned by {@link #getSatellites()}.
     *
     * @return the maximum number of satellites
     */
    CARAPI GetMaxSatellites(
        /* [out] */ Int32* maxNumber);

private:
    static const Int32 NUM_SATELLITES = 255;

    /* These package private values are modified by the LocationManager class */
    Int32 mTimeToFirstFix;
    AutoPtr<ArrayOf<IGpsSatellite*> > mSatellites;
    AutoPtr<IIterable> mSatelliteList;

};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_GPSSTATUS_H__
