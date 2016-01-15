
#include <elastos.h>
#include <stdio.h>

using namespace Elastos;

int main(int argc, char** argv)
{
    printf("======== testAnnotation() ======\n");

    AutoPtr<IModuleInfo> moduleInfo;
    AutoPtr<IClassInfo> classInfo;
    AutoPtr<IInterface> object;
    AutoPtr<IMethodInfo> methodInfo;

    String moduleName("/data/data/com.elastos.runtime/elastos/Annotation.eco");
    String klassName("CAnnotation");

    ECode ec = _CReflector_AcquireModuleInfo(moduleName, (IModuleInfo**)&moduleInfo);
    if (FAILED(ec)) {
        printf("Acquire \"%s\" module info failed!\n", moduleName.string());
        return ec;
    }

    ec = moduleInfo->GetClassInfo(klassName, (IClassInfo**)&classInfo);
    if (FAILED(ec)) {
        printf("Acquire \"%s\" class info failed!\n", klassName.string());
        return ec;
    }

    // M4
    //
    ec = classInfo->GetMethodInfo(String("M4"), String("()E"), (IMethodInfo**)&methodInfo);
    if (FAILED(ec)) {
        printf("Acquire \"M4\" method info failed!\n");
        return ec;
    }

    String annotation;
    ec = methodInfo->GetAnnotation(&annotation);
    if (FAILED(ec)) {
        printf("Get \"M4\" method annotation failed!\n");
        return ec;
    }

    printf("M4 annotation: %s\n", annotation.string());

    methodInfo = NULL;

    // M5
    //
    ec = classInfo->GetMethodInfo(String("M5"), String("()E"), (IMethodInfo**)&methodInfo);
    if (FAILED(ec)) {
        printf("Acquire \"M5\" method info failed!\n");
        return ec;
    }

    ec = methodInfo->GetAnnotation(&annotation);
    if (FAILED(ec)) {
        printf("Get \"M5\" method annotation failed!\n");
        return ec;
    }

    printf("M5 annotation: %s\n", annotation.string());

    Int32 count;
    ec = classInfo->GetConstructorCount(&count);
    if (FAILED(ec)) {
        printf("Get constructor count failed!\n");
        return ec;
    }

    printf("constructor count is %d\n", count);

    AutoPtr< ArrayOf<IConstructorInfo*> > constructorInfos = ArrayOf<IConstructorInfo*>::Alloc(count);
    ec = classInfo->GetAllConstructorInfos(constructorInfos);
    if (FAILED(ec)) {
        printf("Get all constructorInfos failed!\n");
        return ec;
    }

    for (Int32 i = 0; i < count; i++) {
        IConstructorInfo* constructorInfo = (*constructorInfos)[i];
        ec = constructorInfo->GetAnnotation(&annotation);
        if (FAILED(ec)) {
            printf("Get constructor annotation failed!\n");
            return ec;
        }

        printf("%d constructor annotation: %s\n", i, annotation.string());
    }

    return 0;
}