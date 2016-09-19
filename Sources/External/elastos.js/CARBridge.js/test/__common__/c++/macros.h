#ifndef __CAR_BRIDGE_TEST_MACROS_H
# define __CAR_BRIDGE_TEST_MACROS_H

# define CAR_BRIDGE_TEST_NAMESPACE_BEGIN \
namespace Elastos { \
namespace CARBridge { \
namespace Test {

# define CAR_BRIDGE_TEST_NAMESPACE_END \
} \
} \
}

# define CAR_BRIDGE_TEST ::Elastos::CARBridge::Test

# define CAR_BRIDGE_TEST_NAMESPACE_USING \
using namespace ::Elastos::CARBridge::Test;

#endif
