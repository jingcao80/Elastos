
#ifndef __CARUTILITY_H__
#define __CARUTILITY_H__

#include "ElNode.h"
#include "config.h"

#if ENABLE(CAR_BRIDGE)

#include "ext/frameworkdef.h"
#include <elastos.h>

namespace JSC {
namespace Bindings {

CarDataType CarDataTypeFromClassName(const char* name);
const char* ClassNameFromCarDataType(CarDataType type);

const char* SignatureFromCarDataType(CarDataType type);
CarDataType CarDataTypeFromSignature(const char* name);

} // namespace Bindings
} // namespace JSC

#else

gggg

#endif // ENABLE(CAR_BRIDGE)

#endif //__COBJECTWRAPPER_H__
