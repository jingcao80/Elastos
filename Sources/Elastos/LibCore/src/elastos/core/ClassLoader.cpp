
#include "ClassLoader.h"
#include "CPathClassLoader.h"
#include "Globals.h"
#include "StringUtils.h"
#include "StringBuilder.h"
#include "io/CFile.h"
#include <cutils/properties.h>
#include <cutils/log.h>

using Elastos::IO::CFile;
using Elastos::IO::IFile;

ELAPI RegisterModuleInfo(
    /* [in] */ const String& moduleName);

namespace Elastos {
namespace Core {

//--------------------------------------------
//  ClassLoader::SystemClassLoader
//--------------------------------------------
AutoPtr<IClassLoader> ClassLoader::SystemClassLoader::sLoader;
String ClassLoader::SystemClassLoader::sSystemClasspath;

//--------------------------------------------
//  ClassLoader
//--------------------------------------------
CAR_INTERFACE_IMPL(ClassLoader, Object, IClassLoader)

ECode ClassLoader::constructor(
    /* [in] */ const String& classPath,
    /* [in] */ IClassLoader* parent)
{
    if (classPath.IsNullOrEmpty()) {
        ALOGE("ClassLoader::constructor: module path is null or empty");
        return E_INVALID_ARGUMENT;
    }

    StringUtils::Split(classPath, ":", (ArrayOf<String>**)&mClassPaths);
    assert(mClassPaths != NULL);
    mParent = parent;
    return NOERROR;
}

ECode ClassLoader::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb;
    if (mParent != NULL) {
        sb += "ClassLoader{0x";
        sb += StringUtils::ToHexString((Int32)this);
        sb += " ";
        for (Int32 i = 0; i < mClassPaths->GetLength(); i++) {
            if (i != 0) {
                sb += ":";
            }
            sb += (*mClassPaths)[i];
        }
        sb += " parent:";
        sb += Object::ToString(mParent);
        sb += "}";
    }
    else {
        sb += "ClassLoader{0x";
        sb += StringUtils::ToHexString((Int32)this);
        sb += " ";
        for (Int32 i = 0; i < mClassPaths->GetLength(); i++) {
            if (i != 0) {
                sb += ":";
            }
            sb += (*mClassPaths)[i];
        }
        sb += "}";
    }
    *str = sb.ToString();
    return NOERROR;
}

static String GetSystemProperties(
    /* [in] */ const String& key,
    /* [in] */ const String& def)
{
    static const  Int32 SYSTEM_PROPERTY_VALUE_MAX = 91;
    int len;
    char buf[SYSTEM_PROPERTY_VALUE_MAX + 1];

    len = property_get(key.string(), buf, "");
    if ((len <= 0) && (!def.IsNull())) {
        return def;
    }
    else if (len >= 0) {
        return String(buf);
    }

    return String("");
}

String ClassLoader::GetSystemClasspath()
{
    if (SystemClassLoader::sSystemClasspath.IsNull()) {
        SystemClassLoader::sSystemClasspath = GetSystemProperties(String("elastos.class.path"), String(NULL));
        if (SystemClassLoader::sSystemClasspath.IsNullOrEmpty()) {
            SystemClassLoader::sSystemClasspath = "/system/lib/Elastos.Droid.Core.eco";
        }
    }
    return SystemClassLoader::sSystemClasspath;
}

AutoPtr<IClassLoader> ClassLoader::CreateSystemClassLoader()
{
    AutoPtr<IClassLoader> passClassLoader;
    CPathClassLoader::New(GetSystemClasspath(), NULL, (IClassLoader**)&passClassLoader);
    return passClassLoader;
}

AutoPtr<IClassLoader> ClassLoader::GetSystemClassLoader()
{
    if (SystemClassLoader::sLoader == NULL) {
        SystemClassLoader::sLoader = CreateSystemClassLoader();
    }
    return SystemClassLoader::sLoader;
}

AutoPtr<IClassLoader> ClassLoader::GetClassLoader(
    /* [in] */ IClassInfo* clsInfo)
{
    if (clsInfo == NULL) {
        ALOGE("ClassLoader::GetClassLoader: invalid argument. class info is NULL.");
        return NULL;
    }

    AutoPtr<IInterface> obj;
    clsInfo->GetClassLoader((IInterface**)&obj);
    AutoPtr<IClassLoader> loader = IClassLoader::Probe(obj);
    if (loader == NULL) {
        loader = GetSystemClassLoader();
    }
    return loader;
}

AutoPtr<IClassInfo> ClassLoader::FindLoadedClass(
    /* [in] */ const String& className)
{
    AutoPtr<IClassInfo> klass;
    HashMap<String, AutoPtr<IClassInfo> >::Iterator it = mClassTable.Find(className);
    if (it != mClassTable.End()) {
        klass = it->mSecond;
    }
    return klass;
}

ECode ClassLoader::LoadClass(
    /* [in] */ const String& className,
    /* [out] */ IClassInfo** klass)
{
    VALIDATE_NOT_NULL(klass);
    *klass = NULL;

    AutoPtr<IClassInfo> clazz = FindLoadedClass(className);
    if (clazz == NULL) {
        if (mParent != NULL) {
            mParent->LoadClass(className, (IClassInfo**)&clazz);
        }

        if (clazz == NULL) {
            FAIL_RETURN(FindClass(className, (IClassInfo**)&clazz));
            if (clazz != NULL) clazz->SetClassLoader((IClassLoader*)this);
        }
    }

    *klass = clazz;
    REFCOUNT_ADD(*klass);
    return NOERROR;
}

ECode ClassLoader::FindClass(
    /* [in] */ const String& className,
    /* [out] */ IClassInfo** klass)
{
    VALIDATE_NOT_NULL(klass);
    *klass = NULL;

    if (className.IsNullOrEmpty()) {
        ALOGE("ClassLoader::FindClass: className is null or empty");
        return E_INVALID_ARGUMENT;
    }

    HashMap<String, AutoPtr<IModuleInfo> >::Iterator it;
    ECode ec = NOERROR;
    for (Int32 i = 0; i < mClassPaths->GetLength(); i++) {
        AutoPtr<IModuleInfo> moduleInfo;
        String path = (*mClassPaths)[i];
        it = mModuleTable.Find(path);
        if (it == mModuleTable.End()) {
            ec = CReflector::AcquireModuleInfo(path, (IModuleInfo**)&moduleInfo);
            if (FAILED(ec) || moduleInfo == NULL) {
                ALOGE("ClassLoader::FindClass %s, failed to AcquireModuleInfo at path %d/%d: %s",
                    className.string(), i +1, mClassPaths->GetLength(), path.string());
                continue;
            }
            mModuleTable[path] = moduleInfo;
        }
        else {
            moduleInfo = it->mSecond;
        }

        ec = moduleInfo->GetClassInfo(className, klass);
        if (SUCCEEDED(ec)) {
            (*klass)->SetClassLoader((IClassLoader*)this);
            return NOERROR;
        }
    }

    return E_CLASS_NOT_FOUND_EXCEPTION;
}

ECode ClassLoader::LoadInterface(
    /* [in] */ const String& name,
    /* [out] */ IInterfaceInfo** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<IInterfaceInfo> itfc;
    // not cache now
    if (mParent != NULL) {
        mParent->LoadInterface(name, (IInterfaceInfo**)&itfc);
    }

    if (itfc == NULL) {
        FAIL_RETURN(FindInterface(name, (IInterfaceInfo**)&itfc));
        if (itfc != NULL) itfc->SetClassLoader((IClassLoader*)this);
    }

    *result = itfc;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ClassLoader::FindInterface(
    /* [in] */ const String& name,
    /* [out] */ IInterfaceInfo** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (name.IsNullOrEmpty()) {
        ALOGE("ClassLoader::FindClass: interface name is null or empty");
        return E_INVALID_ARGUMENT;
    }

    ECode ec = NOERROR;
    HashMap<String, AutoPtr<IModuleInfo> >::Iterator it;
    for (Int32 i = 0; i < mClassPaths->GetLength(); i++) {
        String path = (*mClassPaths)[i];
        AutoPtr<IModuleInfo> moduleInfo;
        it = mModuleTable.Find(path);
        if (it == mModuleTable.End()) {
            ec = CReflector::AcquireModuleInfo(path, (IModuleInfo**)&moduleInfo);
            if (FAILED(ec) || moduleInfo == NULL) {
                ALOGE("ClassLoader::FindInterface %s, failed to AcquireModuleInfo at path %d/%d: %s",
                    name.string(), i +1, mClassPaths->GetLength(), path.string());
                continue;
            }
            mModuleTable[path] = moduleInfo;
        }
        else {
            moduleInfo = it->mSecond;
        }

        ec = moduleInfo->GetInterfaceInfo(name, result);
        if (SUCCEEDED(ec)) {
            return NOERROR;
        }
    }

    return E_CLASS_NOT_FOUND_EXCEPTION;
}

void ClassLoader::RegisterModuleInfos()
{
    if (mClassPaths != NULL) {
        for (Int32 i = 0; i < mClassPaths->GetLength(); i++) {
            RegisterModuleInfo((*mClassPaths)[i]);
        }
    }
}

//-------------------------------------------------------
//  BootClassLoader
//-------------------------------------------------------
// AutoPtr<IClassLoader> BootClassLoader::sInstance;

// AutoPtr<IClassLoader> BootClassLoader::GetInstance()
// {
//     if (sInstance == NULL) {
//         sInstance = (IClassLoader*)new BootClassLoader();
//     }

//     return sInstance;
// }

// ECode BootClassLoader::LoadClass(
//     /* [in] */ const String& className,
//     /* [out] */ IClassInfo** klass)
// {
//     VALIDATE_NOT_NULL(klass)
//     *klass = NULL;
//     AutoPtr<IClassInfo> clazz = FindLoadedClass(className);

//     if (clazz == NULL) {
//         FAIL_RETURN(FindClass(className, (IClassInfo**)&clazz));
//     }

//     *klass = clazz;
//     REFCOUNT_ADD(*klass);
//     return NOERROR;
// }

// ECode BootClassLoader::FindClass(
//     /* [in] */ const String& className,
//     /* [out] */ IClassInfo** klass)
// {
//     VALIDATE_NOT_NULL(klass)
//     *klass = NULL;

//     String bootClassPath("/system/lib/Elastos.Core.eco");//assert(0 && "TODO") gCore.mBootClassPathStr

//     AutoPtr< ArrayOf<String> > paths;
//     if (bootClassPath.Equals(".")) {
//         AutoPtr<IFile> dir;
//         CFile::New(bootClassPath, (IFile**)&dir);
//         dir->List((ArrayOf<String>**)&paths);
//     }
//     else {
//         StringUtils::Split(bootClassPath, ":", (ArrayOf<String>**)&paths);
//     }

//     for (Int32 i = 0; i < paths->GetLength(); i++) {
//         String path = (*paths)[i];
//         AutoPtr<IModuleInfo> module;
//         ECode ec = CReflector::AcquireModuleInfo(path, (IModuleInfo**)&module);
//         if (FAILED(ec) || module == NULL) continue;
//         ec = module->GetClassInfo(className, klass);
//         if (SUCCEEDED(ec)) {
//             (*klass)->SetClassLoader((IClassLoader*)this);
//             return NOERROR;
//         }
//     }

//     *klass = NULL;
//     return E_CLASS_NOT_FOUND_EXCEPTION;
// }

}
}
