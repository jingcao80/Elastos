
#include "elastos/droid/test/PackageInfoSources.h"
#include "elastos/droid/test/ClassPathPackageInfoSource.h"

namespace Elastos {
namespace Droid {
namespace Test {

AutoPtr<ClassPathPackageInfoSource> PackageInfoSources::sClassPathSource;

AutoPtr<ClassPathPackageInfoSource> PackageInfoSources::ForClassPath(
    /* [in] */ IClassLoader* classLoader)
{
    if (sClassPathSource == NULL) {
        sClassPathSource = new ClassPathPackageInfoSource();
        sClassPathSource->SetClassLoader(classLoader);
    }
    return sClassPathSource;
}

} // namespace Test
} // namespace Droid
} // namespace Elastos
