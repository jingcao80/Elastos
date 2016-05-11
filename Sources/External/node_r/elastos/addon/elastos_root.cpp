
// #include "elastos_root.h"

#include <v8.h>
#include <node.h>
#include <eldef.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using namespace v8;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Node {

void Require(const FunctionCallbackInfo<Value>& args)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    if (args.Length() < 1) {
        // Throw an Error that is passed back to JavaScript
        isolate->ThrowException(Exception::TypeError(
                v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
        args.GetReturnValue().SetUndefined();
        return;
    }

    if (!args[0]->IsString() ||
        (args.Length() >= 2 && !args[1]->IsString())) {
        isolate->ThrowException(Exception::TypeError(
                v8::String::NewFromUtf8(isolate, "Wrong type of arguments")));
        args.GetReturnValue().SetUndefined();
        return;
    }

    v8::String::Utf8Value u8ModuleName(args[0]);
    AutoPtr<IModuleInfo> moduleInfo;
    ECode ec = CReflector::AcquireModuleInfo(
            Elastos::String(*u8ModuleName), (IModuleInfo**)&moduleInfo);
    if (FAILED(ec)) {
        StringBuilder sb("Acquire module info failed! ModuleName: ");
        sb += *u8ModuleName;
        isolate->ThrowException(Exception::TypeError(
                v8::String::NewFromUtf8(isolate, sb.ToString().string())));
        args.GetReturnValue().SetUndefined();
        return;
    }

    if (args.Length() == 1) {
        // CarValue value;
        // value.mType = CarDataType_Interface;
        // value.mObjectValue = moduleInfo;
        // NPVariant result;
        // ConvertCarValueToNPVariant(value, &result);

        // v8::Local<v8::Object> v8Object = WebCore::createV8ObjectForNPObject(
        //     NPVARIANT_TO_OBJECT(result),
        //     /* NPVariant root */ NULL
        // );
        Local<Object> v8Object;
        args.GetReturnValue().Set(v8Object);
        return;
    }

    v8::String::Utf8Value u8ClassName(args[1]);
    AutoPtr<IClassInfo> classInfo;
    ec = moduleInfo->GetClassInfo(
            Elastos::String(*u8ClassName), (IClassInfo**)&classInfo);
    if (FAILED(ec)) {
        StringBuilder sb("Acquire class info failed! ClassName: ");
        sb += *u8ClassName;
        isolate->ThrowException(Exception::TypeError(
                v8::String::NewFromUtf8(isolate, sb.ToString().string())));
        args.GetReturnValue().SetUndefined();
        return;
    }

    if (args.Length() == 2) {
        AutoPtr<IInterface> carObject;
        ec = classInfo->CreateObject((IInterface**)&carObject);
        if (FAILED(ec)) {
            isolate->ThrowException(Exception::TypeError(
                    v8::String::NewFromUtf8(isolate, "Create object failed!")));
            args.GetReturnValue().SetUndefined();
            return;
        }

        Local<Object> v8Object;
        args.GetReturnValue().Set(v8Object);
        return;
    }

    //TODO: create object with parameters
    // Elastos::Int32 constructorCount;
    // ec = classInfo->GetConstructorCount(&constructorCount);
    // if (FAILED(ec)) {
    //     errorInfo = v8::String::NewFromUtf8(isolate,"GetConstructorCount failed!");
    //     break;
    // }
    // BufferOf<IConstructorInfo*>* constructorInfos;
    // constructorInfos = BufferOf<IConstructorInfo*>::Alloc(constructorCount);
    // ec = classInfo->GetAllConstructorInfos(constructorInfos);
    // if (FAILED(ec)) {
    //     errorInfo = v8::String::NewFromUtf8(isolate,"GetAllConstructorInfos failed!");
    //     break;
    // }
    // Elastos::Int32 paramCount;
    // IConstructorInfo* constructorInfo = NULL;
    // Elastos::Int32 argc = info.Length() - 2;
    // for (Elastos::Int32 i = 0; i < constructorCount; i ++) {
    //     IConstructorInfo *tmpConstructor = (*constructorInfos)[i];
    //     ec = tmpConstructor->GetParamCount(&paramCount);
    //     if (FAILED(ec)) {
    //         //
    //     }
    //     // TODO: Actually we should compare the parameters count, but also the
    //     // parameters data type. but now we just compare the parameter count.
    //     if (paramCount == argc) {
    //         constructorInfo = tmpConstructor;
    //         break;
    //     }
    // }
    // if (!constructorInfo) {
    //     errorInfo = v8::String::NewFromUtf8(isolate,"can not find class constructor!");
    //     break;
    // }

    // BufferOf<IParamInfo*>* paramInfos = NULL;
    // paramInfos = BufferOf<IParamInfo*>::Alloc(paramCount);
    // ec = constructorInfo->GetAllParamInfos(paramInfos);
    // if (FAILED(ec)) {
    //     errorInfo = v8::String::NewFromUtf8(isolate,"GetAllParamInfos failed!");
    //     break;
    // }
    // IArgumentList* argumentList = NULL;
    // ec = constructorInfo->CreateArgumentList(&argumentList);
    // if (FAILED(ec)) {
    //     errorInfo = v8::String::NewFromUtf8(isolate,"CreateArgumentList failed!");
    //     break;
    // }
    // //TODO:convert js parms to argumentlist
    // ec = constructorInfo->CreateObject(argumentList, (IInterface**)&carObject);
    // if (FAILED(ec)) {
    //     errorInfo = v8::String::NewFromUtf8(isolate,"ConstructorInfo CreateObject failed!");
    //     break;
    // }
    // retObject = carObject;
}

} //namespace Node
} //namespace Elastos

void init(Handle<Object> exports, Handle<Object> module) {
    NODE_SET_METHOD(exports, "require", Elastos::Node::Require);
    // NODE_SET_METHOD(exports, "receive", JSC::Bindings::Receive);
    // NODE_SET_METHOD(exports, "SetEnqueueUIMessagePtr", JSC::Bindings::SetEnqueueUIMessagePtr);
    // NODE_SET_METHOD(exports, "GetNodeBridge", JSC::Bindings::GetNodeBridge);
}

NODE_MODULE(elastos, init)