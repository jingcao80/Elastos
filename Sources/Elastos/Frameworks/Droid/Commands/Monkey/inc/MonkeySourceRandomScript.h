#ifndef __MONKEYSOURCERANDOMSCRIPT_H__
#define __MONKEYSOURCERANDOMSCRIPT_H__

#include <ext/frameworkext.h>
#include <elastos/utility/etl/List.h>
#include "Elastos.Droid.Commands.Monkey_server.h"

using Elastos::Utility::Etl::List;
using Elastos::Core::IRandom;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

class MonkeySourceRandomScript
{
public:
    /**
     * Creates a MonkeySourceRandomScript instance with an additional setup script.
     *
     * @param setupFileName The name of the setup script file on the device.
     * @param scriptFileNames An ArrayList of the names of the script files to be run randomly.
     * @param throttle The amount of time to sleep in ms between events.
     * @param randomizeThrottle Whether to randomize throttle.
     * @param random The random number generator.
     */
    MonkeySourceRandomScript();

    ~MonkeySourceRandomScript();

    CARAPI Init(
        /* [in] */ const String& setupFileName,
        /* [in] */ ArrayOf<String> *scriptFileNames,
        /* [in] */ Int64 throttle,
        /* [in] */ Boolean randomizeThrottle,
        /* [in] */ IRandom *random,
        /* [in] */ Int64 profileWaitTime,
        /* [in] */ Int64 deviceSleepTime,
        /* [in] */ Boolean randomizeScript);

    CARAPI Init(
        /* [in] */ ArrayOf<String> *scriptFileNames,
        /* [in] */ Int64 throttle,
        /* [in] */ Boolean randomizeThrottle,
        /* [in] */ IRandom *random,
        /* [in] */ Int64 profileWaitTime,
        /* [in] */ Int64 deviceSleepTime,
        /* [in] */ Boolean randomizeScript);

    /**
     * Gets the next event from the current event source.  If the event source is null, a new
     * script event source is chosen randomly from the list of script sources and the next event is
     * chosen from that.
     *
     * @return The first event in the event queue or null if the end of the file
     *         is reached or if an error is encountered reading the file.
     */
    CARAPI GetNextEvent(
            /* [out] */ IMonkeyEvent **event);

    /**
     * Sets the verbosity for the source as well as all sub event sources.
     *
     * @param verbose The verbose level.
     */
    CARAPI SetVerbose(
            /* [in] */ Int32 verbose);

    /**
     * Validates that all the underlying event sources are valid
     *
     * @return True if all the script files are valid.
     *
     * @see MonkeySourceScript#validate()
     */
    CARAPI Validate(
            /* [out] */ Boolean *result);

private:
    /** The verbose level of the source (currently not used) */
    Int32 mVerbose;

    /** The source for the setup script if it exists */
    AutoPtr<IMonkeySourceScript> mSetupSource;

    /** The list of MonkeySourceScript instances to be played in random order */
    AutoPtr<List<AutoPtr<IMonkeySourceScript> > > mScriptSources;

    /** The current source, set to the setup source and then a random script */
    AutoPtr<IMonkeySourceScript> mCurrentSource;

    /** The random number generator */
    AutoPtr<IRandom> mRandom;

    Boolean mRandomizeScript;

    Int32 mScriptCount;
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos


#endif // __MONKEYSOURCERANDOMSCRIPT_H__
