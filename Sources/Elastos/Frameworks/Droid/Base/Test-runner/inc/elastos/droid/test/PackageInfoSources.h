
#ifndef __ELASTOS_DROID_TEST_PACKAGEINFOSOURCES_H__
#define __ELASTOS_DROID_TEST_PACKAGEINFOSOURCES_H__

#include "elastos/droid/test/ClassPathPackageInfoSource.h"

using Elastos::Core::IClassLoader;

namespace Elastos {
namespace Droid {
namespace Test {

/**
 * {@hide} Not needed for SDK.
 */
class PackageInfoSources
{
public:
    static CARAPI_(AutoPtr<ClassPathPackageInfoSource>) ForClassPath(
        /* [in] */ IClassLoader* classLoader);

private:
    PackageInfoSources() {}

private:
    static AutoPtr<ClassPathPackageInfoSource> sClassPathSource;
};

} // namespace Test
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TEST_PACKAGEINFOSOURCES_H__
