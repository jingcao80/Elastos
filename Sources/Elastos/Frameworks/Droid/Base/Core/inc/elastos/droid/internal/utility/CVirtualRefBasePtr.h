
#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CVIRTUALREFBASEPTR_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CVIRTUALREFBASEPTR_H__

#include "_Elastos_Droid_Internal_Utility_CVirtualRefBasePtr.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CarClass(CVirtualRefBasePtr)
    , public Object
    , public IVirtualRefBasePtr
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    ~CVirtualRefBasePtr();

    CARAPI constructor(
        /* [in] */ Int64 ptr);

    CARAPI Get(
        /* [in] */ Int64* ptr);

    CARAPI ReleasePtr();

private:
    Int64 mNativePtr;
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos


#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CVIRTUALREFBASEPTR_H__
