
#ifndef __CLASSPATHPACKAGEINFO_H__
#define __CLASSPATHPACKAGEINFO_H__

#include <elastos/droid/ext/frameworkdef.h>
#include "elastos/droid/test/ClassPathPackageInfoSource.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Test {

class ClassPathPackageInfoSource;

class ClassPathPackageInfo : public Object
{
public:
    ClassPathPackageInfo(
        /* [in] */ ClassPathPackageInfoSource* source,
        /* [in] */ const String& packageName,
        /* [in] */ ISet* subpackageNames,
        /* [in] */ ISet* topLevelClasses);

    CARAPI_(AutoPtr<ISet>) GetSubpackages();

    CARAPI_(AutoPtr<ISet>) GetTopLevelClassesRecursive();

private:
    CARAPI_(void) AddTopLevelClassesTo(
        /* [in] */ ISet* set);

private:
    AutoPtr<ClassPathPackageInfoSource> mSource;
    String mPackageName;
    AutoPtr<ISet> mSubpackageNames;
    AutoPtr<ISet> mTopLevelClasses;
};

} // namespace Test
} // namespace Droid
} // namespace Elastos

#endif //__CLASSPATHPACKAGEINFO_H__
