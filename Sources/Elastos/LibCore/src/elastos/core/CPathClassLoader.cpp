
#include "CPathClassLoader.h"

namespace Elastos {
namespace Core {

CAR_INTERFACE_IMPL(CPathClassLoader, Object, IClassLoader)

CAR_OBJECT_IMPL(CPathClassLoader)

ECode CPathClassLoader::constructor(
    /* [in] */ const String& path)
{
    if (path.IsNullOrEmpty()) {
        ALOGE("CPathClassLoader::constructor: module path is null or empty");
        return E_INVALID_ARGUMENT;
    }

    mPath = path;
    return NOERROR;
}

ECode CPathClassLoader::LoadClass(
    /* [in] */ const String& className,
    /* [out] */ IClassInfo** klass)
{
    VALIDATE_NOT_NULL(klass);
    *klass = NULL;

    if (className.IsNullOrEmpty()) {
        ALOGE("CPathClassLoader::LoadClass: className is null or empty");
        return E_INVALID_ARGUMENT;
    }

    // ALOGI("CPathClassLoader::LoadClass: GetClassInfo %s in %s",
    //     className.string(), mPath.string());

    AutoPtr<IModuleInfo> moduleInfo;
    ECode ec = CReflector::AcquireModuleInfo(mPath, (IModuleInfo**)&moduleInfo);
    if (FAILED(ec)) {
        ALOGE("CPathClassLoader::LoadClass %s, failed to AcquireModuleInfo %s",
            className.string(), mPath.string());
        return E_CLASS_NOT_FOUND_EXCEPTION;
    }

    ec = moduleInfo->GetClassInfo(className, klass);
    if (FAILED(ec)) {
        ALOGE("CPathClassLoader::LoadClass: failed to GetClassInfo %s in %s",
            className.string(), mPath.string());
        return E_CLASS_NOT_FOUND_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Core
} // namespace Elastos
