#ifndef __ELASTOS_APPS_CONTACTS_COMMON_UTIL_STOPWATCH_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_UTIL_STOPWATCH_H__

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Util {

/**
 * A {@link StopWatch} records start, laps and stop, and print them to logcat.
 */
class StopWatch
    : public Object
    , public IStopWatch
{
private:
    class NullStopWatch
        : public Object
        , public IStopWatch
    {
    public:
        NullStopWatch();

        CARAPI Lap(
            /* [in] */ const String& lapLabel);

        CARAPI StopAndLog(
            /* [in] */ const String& TAG,
            /* [in] */ Int32 timeThresholdToLog);

    public:
        static const AutoPtr<NullStopWatch> INSTANCE;
    };

public:
    /**
     * Create a new instance and start it.
     */
    static CARAPI_(AutoPtr<IStopWatch>) Start(
        /* [in] */ const String& label);

    /**
     * Record a lap.
     */
    CARAPI Lap(
        /* [in] */ const String& lapLabel);

    /**
     * Stop it and log the result, if the total time >= {@code timeThresholdToLog}.
     */
    CARAPI StopAndLog(
        /* [in] */ const String& TAG,
        /* [in] */ Int32 timeThresholdToLog);

    /**
     * Return a dummy instance that does no operations.
     */
    static CARAPI_(AutoPtr<IStopWatch>) GetNullStopWatch();

private:
    StopWatch(
        /* [in] */ const String& label);

private:
    String mLabel;

    AutoPtr<IArrayList> mTimes;
    AutoPtr<IArrayList> mLapLabels;
};

} // Util
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_UTIL_STOPWATCH_H__
