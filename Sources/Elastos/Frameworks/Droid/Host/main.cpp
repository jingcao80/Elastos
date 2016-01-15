
#include <stdio.h>
#include <elastos.h>
#include <elautoptr.h>

using namespace Elastos;

void Usage()
{
    printf("Host.ecx MODULE_NAME CLASS_NAME \n");
}

int main(int argc, char* argv[])
{
    AutoPtr<IModuleInfo> moduleInfo;
    AutoPtr<IClassInfo> classInfo;
    AutoPtr<IInterface> object;
    AutoPtr<IMethodInfo> methodInfo;
    AutoPtr<IArgumentList> argumentList;
    char* moduleName;
    char* klassName;

    if (argc < 3) {
        Usage();
        return 0;
    }

    moduleName = argv[1];
    klassName = argv[2];

    char path[260];
    strcpy(path, moduleName);
    ECode ec = _CReflector_AcquireModuleInfo(
            path, (IModuleInfo**)&moduleInfo);
    if (FAILED(ec)) {
        printf("Acquire \"%s\" module info failed!\n", moduleName);
        return -1;
    }

    ec = moduleInfo->GetClassInfo(
            klassName, (IClassInfo**)&classInfo);
    if (FAILED(ec)) {
        printf("Acquire \"%s\" class info failed!\n", klassName);
        return -1;
    }

    ec = classInfo->CreateObject((IInterface**)&object);
    if (FAILED(ec)) {
        printf("Create object failed!\n");
        return -1;
    }

    ec = classInfo->GetMethodInfo(
            "Main", (IMethodInfo**)&methodInfo);
    if (FAILED(ec)) {
        printf("Acquire \"Main\" method info failed!\n");
        return -1;
    }

    ec = methodInfo->CreateArgumentList((IArgumentList**)&argumentList);
    if (FAILED(ec)) {
        printf("Create \"Main\" method argument list failed!\n");
        return -1;
    }

    AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
    (*args)[0] = klassName;
    argumentList->SetInputArgumentOfCarArray(0, args.Get());

    ec = methodInfo->Invoke(object, argumentList);
    if (FAILED(ec)) {
        printf("Invoke \"Main\" method failed!\n");
        return -1;
    }

    return 0;
}