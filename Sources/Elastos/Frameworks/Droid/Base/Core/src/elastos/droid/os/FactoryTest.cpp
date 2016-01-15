
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/FactoryTest.h"
#include "elastos/droid/os/SystemProperties.h"

namespace Elastos {
namespace Droid {
namespace Os {

const Int32 FactoryTest::FACTORY_TEST_OFF = 0;
const Int32 FactoryTest::FACTORY_TEST_LOW_LEVEL = 1;
const Int32 FactoryTest::FACTORY_TEST_HIGH_LEVEL = 2;

Int32 FactoryTest::GetMode()
{
    Int32 value;
    SystemProperties::GetInt32(String("ro.factorytest"), FACTORY_TEST_OFF, &value);
    return value;
}

/**
 * When true, long-press on power should immediately cause the device to
 * shut down, without prompting the user.
 */
Boolean FactoryTest::IsLongPressOnPowerOffEnabled()
{
    Int32 value;
    SystemProperties::GetInt32(String("factory.long_press_power_off"), 0, &value);
    return value != 0;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
