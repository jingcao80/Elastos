
#ifndef __ELASTOS_UTILITY_JAR_ELASTOS_UTILITY_CJAROUTPUTSTREAM_H__
#define __ELASTOS_UTILITY_JAR_ELASTOS_UTILITY_CJAROUTPUTSTREAM_H__

#include "_Elastos_Utility_Jar_CJarOutputStream.h"
#include "ZipOutputStream.h"

using Elastos::Utility::Zip::IZipEntry;
using Elastos::Utility::Zip::ZipOutputStream;
using Elastos::Utility::Jar::IManifest;
using Elastos::IO::IOutputStream;

namespace Elastos {
namespace Utility {
namespace Jar {

CarClass(CJarOutputStream)
    , public ZipOutputStream
    , public IJarOutputStream
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IOutputStream* os,
        /* [in] */ IManifest* manifest);

    CARAPI constructor(
        /* [in] */ IOutputStream* os);

private:
    AutoPtr<IManifest> mManifest;
};

} // namespace Jar
} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_JAR_ELASTOS_UTILITY_CJAROUTPUTSTREAM_H__
