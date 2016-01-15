#ifndef __ELASTOS_DROID_OS_FACTORY_TEST_H__
#define __ELASTOS_DROID_OS_FACTORY_TEST_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * Provides support for in-place factory test functions.
 *
 * This class provides a few properties that alter the normal operation of the system
 * during factory testing.
 *
 * {@hide}
 */
class ECO_PUBLIC FactoryTest
{
public:
    static const Int32 FACTORY_TEST_OFF;// = 0;
    static const Int32 FACTORY_TEST_LOW_LEVEL;// = 1;
    static const Int32 FACTORY_TEST_HIGH_LEVEL;// = 2;

    /**
     * Gets the current factory test mode.
     *
     * @return One of: {@link #FACTORY_TEST_OFF}, {@link #FACTORY_TEST_LOW_LEVEL},
     * or {@link #FACTORY_TEST_HIGH_LEVEL}.
     */
    static Int32 GetMode();

    /**
     * When true, long-press on power should immediately cause the device to
     * shut down, without prompting the user.
     */
    static Boolean IsLongPressOnPowerOffEnabled();

private:
    ECO_LOCAL FactoryTest();
    ECO_LOCAL FactoryTest(const FactoryTest&);
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_FACTORY_TEST_H__
