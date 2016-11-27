
#include <elastos.h>
#include <stdio.h>

using namespace Elastos;

void DumpAnnotation(IAnnotationInfo* annotation)
{
    String name, nameSpace;
    annotation->GetName(&name);
    annotation->GetNamespace(&nameSpace);

    Int32 N;
    annotation->GetSize(&N);

    AutoPtr< ArrayOf<String> > keys;
    AutoPtr< ArrayOf<String> > values;
    annotation->GetKeys((ArrayOf<String>**)&keys);
    annotation->GetValues((ArrayOf<String>**)&values);

    printf("Annotation name: %s, namespace: %s, keyValueCount: %d\n", name.string(), nameSpace.string(), N);
    for (Int32 i = 0; i < N; i++) {
        printf("No.%d key: %s, value: %s\n", i, (*keys)[i].string(), (*values)[i].string());
    }
}

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

    Int32 annotationCount;
    classInfo->GetAnnotationCount(&annotationCount);
    AutoPtr< ArrayOf<IAnnotationInfo*> > annotations = ArrayOf<IAnnotationInfo*>::Alloc(annotationCount);
    classInfo->GetAllAnnotationInfos(annotations);
    printf("class %s has %d annotations:\n", klassName.string(), annotationCount);
    for (Int32 i = 0; i < annotationCount; i++) {
        DumpAnnotation((*annotations)[i]);
    }

    AutoPtr<IInterfaceInfo> interfaceInfo;
    classInfo->GetInterfaceInfo(String("IAnnotation"), (IInterfaceInfo**)&interfaceInfo);
    interfaceInfo->GetAnnotationCount(&annotationCount);
    annotations = ArrayOf<IAnnotationInfo*>::Alloc(annotationCount);
    interfaceInfo->GetAllAnnotationInfos(annotations);
    printf("interface IAnnotation has %d annotations:\n", annotationCount);
    for (Int32 i = 0; i < annotationCount; i++) {
        DumpAnnotation((*annotations)[i]);
    }

    // M4
    //
    ec = classInfo->GetMethodInfo(String("M4"), String("()E"), (IMethodInfo**)&methodInfo);
    if (FAILED(ec)) {
        printf("Acquire \"M4\" method info failed!\n");
        return ec;
    }

    methodInfo->GetAnnotationCount(&annotationCount);
    annotations = ArrayOf<IAnnotationInfo*>::Alloc(annotationCount);
    methodInfo->GetAllAnnotationInfos(annotations);
    printf("method M4 has %d annotations:\n", annotationCount);
    for (Int32 i = 0; i < annotationCount; i++) {
        DumpAnnotation((*annotations)[i]);
    }

    // M6
    //
    methodInfo = NULL;

    ec = classInfo->GetMethodInfo(String("M6"), String("()E"), (IMethodInfo**)&methodInfo);
    if (FAILED(ec)) {
        printf("Acquire \"M5\" method info failed!\n");
        return ec;
    }

    methodInfo->GetAnnotationCount(&annotationCount);
    annotations = ArrayOf<IAnnotationInfo*>::Alloc(annotationCount);
    methodInfo->GetAllAnnotationInfos(annotations);
    printf("method M6 has %d annotations:\n", annotationCount);
    for (Int32 i = 0; i < annotationCount; i++) {
        DumpAnnotation((*annotations)[i]);
    }

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
        constructorInfo->GetAnnotationCount(&annotationCount);
        annotations = ArrayOf<IAnnotationInfo*>::Alloc(annotationCount);
        constructorInfo->GetAllAnnotationInfos(annotations);
        printf("constructor %d has %d annotations:\n", i, annotationCount);
        for (Int32 i = 0; i < annotationCount; i++) {
            DumpAnnotation((*annotations)[i]);
        }
    }

    return 0;
}