
#ifndef __ELASTOS_UTILITY_JAR_ELASTOS_UTILITY_CPACK200_H__
#define __ELASTOS_UTILITY_JAR_ELASTOS_UTILITY_CPACK200_H__

#include "_Elastos_Utility_Jar_CPack200.h"
#include "Singleton.h"

using Elastos::Utility::Jar::IPacker;
using Elastos::Utility::Jar::IUnpacker;

namespace Elastos {
namespace Utility {
namespace Jar {

CarClass(CPack200)
    , public Singleton
    , public IPack200
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI NewPacker(
        /* [out] */ IPacker** packer);

    CARAPI NewUnpacker(
        /* [out] */ IUnpacker** unpacker);

private:
    // TODO: Add your private member variables here.
};

} // namespace Jar
} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_JAR_ELASTOS_UTILITY_CPACK200_H__
