
#include "ElNode.h"
#include "config.h"

#if ENABLE(CAR_BRIDGE)

#include "etl/HashSet.h"
#include "elastos.h"

#include "CarClassCobjectV8.h"
#include "CarMethodCobject.h"
#include "CarFieldCobjectV8.h"

#include <utils/Log.h>

using namespace JSC::Bindings;

CarClassCobject::CarClassCobject(CobjectWrapper* objectWrapper, bool requireAnnotation)
    : mRequireAnnotation(requireAnnotation),mIsView(false)
{
    ECode ec = NOERROR;
    AutoPtr<IClassInfo> classInfo;
    AutoPtr<IInterfaceInfo> interfaceInfo;
    bool bClass = true;

    ec = CObject::ReflectClassInfo(objectWrapper->getInstance(), (IClassInfo**)&classInfo);
    if (FAILED(ec)) {
        bClass = false;
        interfaceInfo = IInterfaceInfo::Probe(objectWrapper->getDataTypeInfo());
    }

    Int32 N;
    if (bClass) {
        classInfo->GetMethodCount(&N);
    }
    else {
        interfaceInfo->GetMethodCount(&N);

        Elastos::String nameBuf_0;
        interfaceInfo->GetName(&nameBuf_0);
    }

    ArrayOf<IMethodInfo*>* methodInfos = ArrayOf<IMethodInfo*>::Alloc(N);

    if (bClass) {
        ec = classInfo->GetAllMethodInfos(methodInfos);
    }
    else {
        ec = interfaceInfo->GetAllMethodInfos(methodInfos);
    }
    if (FAILED(ec)) {
        LOG_ERROR("CarClassCobject::CarClassCobject unable to find methods on instance");
        return;
    }

    //Get the methods
    Elastos::String nameBuf;
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IMethodInfo> methodInfo = (*methodInfos)[i];
        methodInfo->GetName(&nameBuf);
        WTF::String methodName((const char*)nameBuf);

        //method
        MethodList* methodList = mMethods.get(methodName);
        if (!methodList) {
            methodList = new MethodList();
            mMethods.set(methodName, methodList);
        }
        CarMethod* carMethod = new CarMethodCobject(methodInfo);
        methodList->append(carMethod);

        //property
        Int32 paramCount;
        methodInfo->GetParamCount(&paramCount);
        if (paramCount != 1) {
            methodInfo->Release();
            methodInfo = NULL;
            continue;
        }

        AutoPtr<IParamInfo> paramInfo;
        methodInfo->GetParamInfoByIndex(0, (IParamInfo**)&paramInfo);
        ParamIOAttribute paramIOAttr;
        paramInfo->GetIOAttribute(&paramIOAttr);
        Int32 rwAttrib;
        if (methodName.startsWith(WTF::String("Get")) &&
            (paramIOAttr == ParamIOAttribute_CallerAllocOut || paramIOAttr == ParamIOAttribute_CalleeAllocOut)) {
            rwAttrib = CarField::READABLE;
        }
        else if (methodName.startsWith(WTF::String("Set")) && paramIOAttr == ParamIOAttribute_In) {
            rwAttrib = CarField::WRITEABLE;
        }
        else {
            methodInfo->Release();
            methodInfo = NULL;
            continue;
        }

        WTF::String fieldName = methodName.substring(3);
        CarField* carField = mFields.get(fieldName);
        if (!carField) {
                carField = new CarFieldCobject(
                    CarString(Elastos::String(fieldName.utf8().data())), rwAttrib);
            mFields.set(fieldName, carField);
        }
        else {
            rwAttrib |= carField->attr();
            carField->setAttr(rwAttrib);
        }
        carMethod = carField->fieldMethodList().get(methodName);
        if (!carMethod) {
            carMethod = new CarMethodCobject(methodInfo);
            carField->fieldMethodList().set(methodName, carMethod);
        }

        methodInfo->Release();
        methodInfo = NULL;
    }

    //Whether carInstance be an Elastos IView Control
    MethodList* methodList_IViewFlag = mMethods.get(WTF::String("Invalidate"));
    if (methodList_IViewFlag) {
        mIsView = true;
    }

    ArrayOf<IMethodInfo*>::Free(methodInfos);
}

CarClassCobject::~CarClassCobject()
{
    deleteAllValues(mFields);
    mFields.clear();

    MethodListMap::const_iterator end = mMethods.end();
    for (MethodListMap::const_iterator it = mMethods.begin(); it != end; ++it) {
        const MethodList* methodList = it->second;
        deleteAllValues(*methodList);
        delete methodList;
    }
    mMethods.clear();
}

MethodList CarClassCobject::methodsNamed(const char* name) const
{
    MethodList* methodList = mMethods.get(name);

    if (methodList) {
        return *methodList;
    }
    return MethodList();
}

CarField* CarClassCobject::fieldNamed(const char* name) const
{
    return mFields.get(name);
}

bool CarClassCobject::isView() const
{
    return mIsView;
}

#endif // ENABLE(JAVA_BRIDGE)
