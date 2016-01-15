#ifndef __ELASTOS_CORE_CPATHCLASSLOADER_H__
#define __ELASTOS_CORE_CPATHCLASSLOADER_H__

#include "_Elastos_Core_CPathClassLoader.h"
#include "Object.h"

using Elastos::Core::IClassLoader;

namespace Elastos {
namespace Core {

CarClass(CPathClassLoader)
    , public Object
    , public IClassLoader
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ const String& path);

    CARAPI LoadClass(
        /* [in] */ const String& className,
        /* [out] */ IClassInfo** klass);

private:
    String mPath;
};

} // namespace Core
} // namespace Elastos

#endif //__ELASTOS_CORE_CPATHCLASSLOADER_H__
