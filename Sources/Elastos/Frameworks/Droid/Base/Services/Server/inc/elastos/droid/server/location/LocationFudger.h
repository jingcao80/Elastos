
#ifndef __ELASTOS_DROID_SERVER_LOCATION_LOCATIONFUDGER_H__
#define __ELASTOS_DROID_SERVER_LOCATION_LOCATIONFUDGER_H__

#ifdef DROID_CORE
#include "Elastos.Droid.Core_server.h"
#elif defined(DROID_SERVER)
#include "Elastos.Droid.Core.h"
#endif
#include "elastos/droid/database/ContentObserver.h"

using Elastos::IO::IPrintWriter;
using Elastos::IO::IFileDescriptor;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Location::ILocation;
using Elastos::Security::ISecureRandom;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

/**
 * Contains the logic to obfuscate (fudge) locations for coarse applications.
 *
 * <p>The goal is just to prevent applications with only
 * the coarse location permission from receiving a fine location.
 */
class LocationFudger : public ElRefBase
{
public:
    /**
     * This is the fastest interval that applications can receive coarse
     * locations.
     */
    static const Int64 FASTEST_INTERVAL_MS;// = 10 * 60 * 1000;  // 10 minutes

private:

    class ConstructorContentObserver : public ContentObserver
    {
    public:
        ConstructorContentObserver(
            /* [in] */ IHandler* handler,
            /* [in] */ LocationFudger* host);

        virtual CARAPI OnChange(
            /* [in] */ Boolean selfChange);
    private:

        LocationFudger* mHost;
    };
public:

    LocationFudger(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler);

    /**
     * Get the cached coarse location, or generate a new one and cache it.
     */
    CARAPI_(AutoPtr<ILocation>) GetOrCreate(
        /* [in] */ ILocation* location);

    CARAPI_(void) Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:

    CARAPI_(AutoPtr<ILocation>) AddCoarseLocationExtraLocked(
        /* [in] */ ILocation* location);

    CARAPI_(Double) NextOffsetLocked();
    /**
     * Create a coarse location.
     *
     * <p>Two techniques are used: random offsets and snap-to-grid.
     *
     * <p>First we add a random offset. This mitigates against detecting
     * grid transitions. Without a random offset it is possible to detect
     * a users position very accurately when they cross a grid boundary.
     * The random offset changes very slowly over time, to mitigate against
     * taking many location samples and averaging them out.
     *
     * <p>Second we snap-to-grid (quantize). This has the nice property of
     * producing stable results, and mitigating against taking many samples
     * to average out a random offset.
     */
    CARAPI_(AutoPtr<ILocation>) CreateCoarseLocked(
        /* [in] */ ILocation* fine);

    /**
     * Update the random offset over time.
     *
     * <p>If the random offset was new for every location
     * fix then an application can more easily average location results
     * over time,
     * especially when the location is near a grid boundary. On the
     * other hand if the random offset is constant then if an application
     * found a way to reverse engineer the offset they would be able
     * to detect location at grid boundaries very accurately. So
     * we choose a random offset and then very slowly move it, to
     * make both approaches very hard.
     *
     * <p>The random offset does not need to be large, because snap-to-grid
     * is the primary obfuscation mechanism. It just needs to be large
     * enough to stop information leakage as we cross grid boundaries.
     */
    CARAPI_(void) UpdateRandomOffsetLocked();

    static CARAPI_(Double) WrapLatitude(
        /* [in] */ Double lat);

    static CARAPI_(Double) WrapLongitude(
        /* [in] */ Double lon);

    static CARAPI_(Double) MetersToDegreesLatitude(
        /* [in] */ Double distance);

    /**
     * Requires latitude since longitudinal distances change with distance from equator.
     */
    static CARAPI_(Double) MetersToDegreesLongitude(
        /* [in] */ Double distance,
        /* [in] */ Double lat);

     /**
     * This is the main control: call this to set the best location accuracy
     * allowed for coarse applications and all derived values.
     */
    CARAPI_(void) SetAccuracyInMetersLocked(
        /* [in] */ Float accuracyInMeters);

    /**
     * Same as setAccuracyInMetersLocked without the pre-lock requirement.
     */
    CARAPI_(void) SetAccuracyInMeters(
        /* [in] */ Float accuracyInMeters);

    /**
     * Loads the coarse accuracy value from secure settings.
     */
    CARAPI_(Float) LoadCoarseAccuracy();

private:

    static const Boolean D;// = FALSE;
    static const String TAG;// = "LocationFudge";

    /**
     * Default coarse accuracy in meters.
     */
    static const Float DEFAULT_ACCURACY_IN_METERS;// = 2000.0f;

    /**
     * Minimum coarse accuracy in meters.
     */
    static const Float MINIMUM_ACCURACY_IN_METERS;// = 200.0f;

    /**
     * Secure settings key for coarse accuracy.
     */
    static const String COARSE_ACCURACY_CONFIG_NAME;// = "locationCoarseAccuracy";

    /**
     * The duration until we change the random offset.
     */
    static const Int64 CHANGE_INTERVAL_MS;// = 60 * 60 * 1000;  // 1 hour

    /**
     * The percentage that we change the random offset at every interval.
     *
     * <p>0.0 indicates the random offset doesn't change. 1.0
     * indicates the random offset is completely replaced every interval.
     */
    static const Double CHANGE_PER_INTERVAL;// = 0.03;  // 3% change

    // Pre-calculated weights used to move the random offset.
    //
    // The goal is to iterate on the previous offset, but keep
    // the resulting standard deviation the same. The variance of
    // two gaussian distributions summed together is equal to the
    // sum of the variance of each distribution. So some quick
    // algebra results in the following sqrt calculation to
    // weigh in a new offset while keeping the final standard
    // deviation unchanged.
    static const Double NEW_WEIGHT;// = CHANGE_PER_INTERVAL;
    static const Double PREVIOUS_WEIGHT;// = Math.sqrt(1 - NEW_WEIGHT * NEW_WEIGHT);

    /**
     * This number actually varies because the earth is not round, but
     * 111,000 meters is considered generally acceptable.
     */
    static const Int32 APPROXIMATE_METERS_PER_DEGREE_AT_EQUATOR;// = 111000;

    /**
     * Maximum latitude.
     *
     * <p>We pick a value 1 meter away from 90.0 degrees in order
     * to keep cosine(MAX_LATITUDE) to a non-zero value, so that we avoid
     * divide by zero fails.
     */
    static const Double MAX_LATITUDE;/* = 90.0 -
            (1.0 / APPROXIMATE_METERS_PER_DEGREE_AT_EQUATOR);*/

    static Boolean sHavaNextNextGaussian;

    static Double sNextNextGaussian;

    Object mLock;

    /**
     * Used to monitor coarse accuracy secure setting for changes.
     */
    AutoPtr<IContentObserver> mSettingsObserver;

    /**
     * Used to resolve coarse accuracy setting.
     */
    AutoPtr<IContext> mContext;

    // all fields below protected by mLock
    Double mOffsetLatitudeMeters;
    Double mOffsetLongitudeMeters;
    Int64 mNextInterval;

    /**
     * Best location accuracy allowed for coarse applications.
     * This value should only be set by {@link #setAccuracyInMetersLocked(float)}.
     */
    Float mAccuracyInMeters;

    /**
     * The distance between grids for snap-to-grid. See {@link #createCoarse}.
     * This value should only be set by {@link #setAccuracyInMetersLocked(float)}.
     */
    Double mGridSizeInMeters;

    /**
     * Standard deviation of the (normally distributed) random offset applied
     * to coarse locations. It does not need to be as large as
     * {@link #COARSE_ACCURACY_METERS} because snap-to-grid is the primary obfuscation
     * method. See further details in the implementation.
     * This value should only be set by {@link #setAccuracyInMetersLocked(float)}.
     */
    Double mStandardDeviationInMeters;


};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_LOCATIONFUDGER_H__
