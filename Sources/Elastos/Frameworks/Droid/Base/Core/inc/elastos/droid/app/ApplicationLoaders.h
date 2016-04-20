
#ifndef __ELASTOS_DROID_APP_APPLICATION_LOADERS_H__
#define __ELASTOS_DROID_APP_APPLICATION_LOADERS_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Core::IClassLoader;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace App {

class ApplicationLoaders
    : public Object
{
public:
    static AutoPtr<ApplicationLoaders> GetDefault();

    AutoPtr<IClassLoader> GetClassLoader(
        /* [in] */ const String& zip,
        /* [in] */ const String& libPath,
        /* [in] */ IClassLoader* parent);

private:
    static AutoPtr<ApplicationLoaders> gApplicationLoaders;

    HashMap<String, AutoPtr<IClassLoader> > mLoaders;
    Object mLoadersLock;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_APP_APPLICATION_LOADERS_H__
