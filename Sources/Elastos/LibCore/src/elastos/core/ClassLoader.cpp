
#include "ClassLoader.h"
#include "CPathClassLoader.h"
#include "CSystem.h"
#include "Globals.h"
#include "io/CFile.h"

using Elastos::IO::CFile;
using Elastos::IO::IFile;

namespace Elastos {
namespace Core {

//--------------------------------------------
//  ClassLoader::SystemClassLoader
//--------------------------------------------
AutoPtr<IClassLoader> ClassLoader::SystemClassLoader::sLoader;


//--------------------------------------------
//  ClassLoader
//--------------------------------------------
CAR_INTERFACE_IMPL(ClassLoader, Object, IClassLoader)

AutoPtr<IClassLoader> ClassLoader::CreateSystemClassLoader()
{
    AutoPtr<CSystem> system;
    CSystem::AcquireSingletonByFriend((CSystem**)&system);
    String classPath;
    system->GetProperty(String("elastos.class.path"), String("."), &classPath);

    // String[] paths = classPath.split(":");
    // URL[] urls = new URL[paths.length];
    // for (int i = 0; i < paths.length; i++) {
    // try {
    // urls[i] = new URL("file://" + paths[i]);
    // }
    // catch (Exception ex) {
    // ex.printStackTrace();
    // }
    // }
    //
    // return new java.net.URLClassLoader(urls, null);

    // TODO Make this a java.net.URLClassLoader once we have those?
    AutoPtr<IClassLoader> passClassLoader;
    CPathClassLoader::New(classPath, (IClassLoader**)&passClassLoader);
    return passClassLoader;
}

AutoPtr<IClassLoader> ClassLoader::GetSystemClassLoader()
{
    if (SystemClassLoader::sLoader == NULL) {
        SystemClassLoader::sLoader = CreateSystemClassLoader();
    }
    return SystemClassLoader::sLoader;
}

ECode ClassLoader::FindClass(
    /* [in] */ const String& className,
    /* [out] */ IClassInfo** klass)
{
    return E_CLASS_NOT_FOUND_EXCEPTION;
}

AutoPtr<IClassInfo> ClassLoader::FindLoadedClass(
    /* [in] */ const String& className)
{
    AutoPtr<IClassInfo> klass;
    HashMap<String, IClassInfo*>::Iterator it = mClassTable.Find(className);
    if (it != mClassTable.End()) {
        klass = it->mSecond;
    }
    return klass;
}

ECode ClassLoader::LoadClass(
    /* [in] */ const String& className,
    /* [out] */ IClassInfo** klass)
{
    return LoadClass(className, FALSE, klass);
}

ECode ClassLoader::LoadClass(
    /* [in] */ const String& className,
    /* [in] */ Boolean resolve,
    /* [out] */ IClassInfo** klass)
{
    VALIDATE_NOT_NULL(klass);
    *klass = NULL;
    AutoPtr<IClassInfo> clazz = FindLoadedClass(className);

    if (clazz == NULL) {
        mParent->LoadClass(className, FALSE, (IClassInfo**)&clazz);

        if (clazz == NULL) {
            FAIL_RETURN(FindClass(className, (IClassInfo**)&clazz));
        }
    }

    *klass = clazz;
    REFCOUNT_ADD(*klass);
    return NOERROR;
}

AutoPtr<IClassLoader> ClassLoader::GetClassLoader(
    /* [in] */ IClassInfo* clsInfo)
{
    AutoPtr<IInterface> obj;
    clsInfo->GetClassLoader((IInterface**)&obj);
    AutoPtr<IClassLoader> loader = IClassLoader::Probe(obj);
    if (loader == NULL) {
        loader = BootClassLoader::GetInstance();
    }
    return loader;
}

//-------------------------------------------------------
//  BootClassLoader
//-------------------------------------------------------
AutoPtr<BootClassLoader> BootClassLoader::sInstance;

AutoPtr<BootClassLoader> BootClassLoader::GetInstance()
{
    if (sInstance == NULL) {
        sInstance = new BootClassLoader();
    }

    return sInstance;
}

ECode BootClassLoader::FindClass(
    /* [in] */ const String& className,
    /* [out] */ IClassInfo** klass)
{
    String bootClassPath(gCore.mBootClassPathStr);

    AutoPtr< ArrayOf<String> > paths;
    if (bootClassPath.Equals(".")) {
        AutoPtr<IFile> dir;
        CFile::New(bootClassPath, (IFile**)&dir);
        dir->List((ArrayOf<String>**)&paths);
    }
    else {
        StringUtils::Split(bootClassPath, ":", (ArrayOf<String>**)&paths);
    }

    for (Int32 i = 0; i < paths->GetLength(); i++) {
        String path = (*paths)[i];
        AutoPtr<IModuleInfo> module;
        ECode ec = CReflector::AcquireModuleInfo(path, (IModuleInfo**)&module);
        if (FAILED(ec) || module == NULL) continue;
        ec = module->GetClassInfo(className, klass);
        if (SUCCEEDED(ec)) return NOERROR;
    }

    *klass = NULL;
    return E_CLASS_NOT_FOUND_EXCEPTION;
}

ECode BootClassLoader::LoadClass(
    /* [in] */ const String& className,
    /* [in] */ Boolean resolve,
    /* [out] */ IClassInfo** klass)
{
    VALIDATE_NOT_NULL(klass)
    *klass = NULL;
    AutoPtr<IClassInfo> clazz = FindLoadedClass(className);

    if (clazz == NULL) {
        FAIL_RETURN(FindClass(className, (IClassInfo**)&clazz));
    }

    *klass = clazz;
    REFCOUNT_ADD(*klass);
    return NOERROR;
}

}
}
