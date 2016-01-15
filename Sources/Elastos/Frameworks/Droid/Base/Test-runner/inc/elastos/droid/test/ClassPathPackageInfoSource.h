

#ifndef __CLASSPATHPACKAGEINFOSOURCE_H__
#define __CLASSPATHPACKAGEINFOSOURCE_H__

#include <elastos/droid/ext/frameworkdef.h>
#include "elastos/droid/test/ClassPathPackageInfo.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Core::Object;
using Elastos::Core::IClassLoader;
using Elastos::IO::IFile;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Test {

class ClassPathPackageInfo;

class ClassPathPackageInfoSource : public Object
{
public:
    ClassPathPackageInfoSource();

    static CARAPI_(void) SetEpkPaths(
        /* [in] */ ArrayOf<String>* epkPaths);

    CARAPI_(AutoPtr<ClassPathPackageInfo>) GetPackageInfo(
        /* [in] */ const String& pkgName);

    CARAPI_(void) SetClassLoader(
        /* [in] */ IClassLoader* classLoader);

private:
    CARAPI_(AutoPtr<ClassPathPackageInfo>) CreatePackageInfo(
        /* [in] */ const String& packageName);

    /**
     * Finds all classes and sub packages that are below the packageName and
     * add them to the respective sets. Searches the package on the whole class
     * path.
     */
    CARAPI FindClasses(
        /* [in] */ const String& packageName,
        /* [in] */ ISet* classInfos,
        /* [in] */ ISet* subpackageNames);

    CARAPI ScanForEpkFiles(
        /* [in] */ IFile* source,
        /* [in] */ const String& packageName,
        /* [in] */ ISet* classInfos,
        /* [in] */ ISet* subpackageNames);

    /**
     * Finds all classes and sub packages that are below the packageName and
     * add them to the respective sets. Searches the package in a single apk file.
     */
    CARAPI FindClassesInEpk(
        /* [in] */ const String& epkPath,
        /* [in] */ const String& packageName,
        /* [in] */ ISet* classInfos,
        /* [in] */ ISet* subpackageNames);

    /**
     * Checks if a given file name represents a toplevel class.
     */
    static CARAPI_(Boolean) IsToplevelClass(
        /* [in] */ const String& fileName);

    static CARAPI_(AutoPtr< ArrayOf<String> >) GetClassPath();

private:
    HashMap<String, AutoPtr<ClassPathPackageInfo> > mCache;

    AutoPtr< ArrayOf<String> > mClassPath;
    static AutoPtr< ArrayOf<String> > sEpkPaths;
    AutoPtr<IClassLoader> mClassLoader;
};

} // namespace Test
} // namespace Droid
} // namespace Elastos

#endif //__CLASSPATHPACKAGEINFOSOURCE_H__
