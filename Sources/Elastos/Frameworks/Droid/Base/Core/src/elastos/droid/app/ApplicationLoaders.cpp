#include "elastos/droid/app/ApplicationLoaders.h"
#include <elastos/core/ClassLoader.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::ClassLoader;
using Elastos::Core::CPathClassLoader;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {

static const String TAG("ApplicationLoaders");

AutoPtr<ApplicationLoaders> ApplicationLoaders::gApplicationLoaders;

AutoPtr<ApplicationLoaders> ApplicationLoaders::GetDefault()
{
    if (gApplicationLoaders == NULL) {
        gApplicationLoaders = new ApplicationLoaders();
    }
    return gApplicationLoaders;
}

AutoPtr<IClassLoader> ApplicationLoaders::GetClassLoader(
    /* [in] */ const String& zip,
    /* [in] */ const String& libPath,
    /* [in] */ IClassLoader* inParent)
{
    // Logger::I(TAG, "GetClassLoader: zip=%s, libPath=%s, parent=%s",
    //     zip.string(), libPath.string(), TO_CSTR(inParent));

    AutoPtr<IClassLoader> parent = inParent;
    /*
     * This is the parent we use if they pass "null" in.  In theory
     * this should be the "system" class loader; in practice we
     * don't use that and can happily (and more efficiently) use the
     * bootstrap class loader.
     */
    AutoPtr<IClassLoader> baseParent = ClassLoader::GetSystemClassLoader();//.getParent();

    AutoLock lock(mLoadersLock);
    if (parent == NULL) {
        parent = baseParent;
    }

    // /*
    //  * If we're one step up from the base class loader, find
    //  * something in our cache.  Otherwise, we create a whole
    //  * new ClassLoader for the zip archive.
    //  */
    // if (parent == baseParent) {
    //     AutoPtr<IClassLoader> loader = mLoaders[zip];
    //     if (loader != NULL) {
    //         return loader;
    //     }

    //     AutoPtr<IClassLoader> pathClassloader;
    //     CPathClassLoader::New(zip, parent, (IClassLoader**)&pathClassloader);
    //     mLoaders[zip] = pathClassloader;
    //     return pathClassloader;
    // }

    AutoPtr<IClassLoader> pathClassloader;
    CPathClassLoader::New(zip, parent, (IClassLoader**)&pathClassloader);
    mLoaders[zip] = pathClassloader;
    return pathClassloader;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
