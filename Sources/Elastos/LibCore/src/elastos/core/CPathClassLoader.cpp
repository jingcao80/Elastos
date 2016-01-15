
#include "CPathClassLoader.h"

namespace Elastos {
namespace Core {

CAR_INTERFACE_IMPL(CPathClassLoader, Object, IClassLoader)

CAR_OBJECT_IMPL(CPathClassLoader)

ECode CPathClassLoader::constructor(
    /* [in] */ const String& path)
{
    if (path.IsNullOrEmpty()) {
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
        return E_INVALID_ARGUMENT;
    }

    String sname("C");
    sname += className.Substring(className.LastIndexOf('.') + 1);
    // ALOGD("CPathClassLoader::LoadClass: %s, className: %s mPath : %s\n",
    //     sname.string(), className.string(), mPath.string());
    ECode ec = NOERROR;
    AutoPtr<IModuleInfo> moduleInfo;
    if (sname.Equals("CFragmentOne")) {
        ec = CReflector::AcquireModuleInfo(String("FragmentDemo.eco"), (IModuleInfo**)&moduleInfo);
        if (FAILED(ec)) {
            return E_CLASS_NOT_FOUND_EXCEPTION;
        }
    }
    else {
        ec = CReflector::AcquireModuleInfo(mPath, (IModuleInfo**)&moduleInfo);
        if (FAILED(ec)) {
            return E_CLASS_NOT_FOUND_EXCEPTION;
        }
    }

    ec = moduleInfo->GetClassInfo(sname, klass);
    return FAILED(ec) ? E_CLASS_NOT_FOUND_EXCEPTION : NOERROR;
}

} // namespace Core
} // namespace Elastos
