#ifndef __ELASTOS_CORE_CPATHCLASSLOADER_H__
#define __ELASTOS_CORE_CPATHCLASSLOADER_H__

#include "_Elastos_Core_CPathClassLoader.h"
#include "ClassLoader.h"

using Elastos::Core::IClassLoader;

namespace Elastos {
namespace Core {

CarClass(CPathClassLoader)
    , public ClassLoader
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Core
} // namespace Elastos

#endif //__ELASTOS_CORE_CPATHCLASSLOADER_H__
