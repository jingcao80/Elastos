
#include <v8.h>
#include <node.h>
#include "eldef.h"
#include "V8CarObject.h"
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
                v8::String::NewFromUtf8(isolate, "Wrong number of arguments!")));
        args.GetReturnValue().SetUndefined();
        return;
    }

    if (!args[0]->IsString() ||
        (args.Length() >= 2 && !args[1]->IsString())) {
        isolate->ThrowException(Exception::TypeError(
                v8::String::NewFromUtf8(isolate, "Wrong type of arguments!")));
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
        CarValue value;
        value.mType = CarDataType_Interface;
        value.mObjectValue = moduleInfo;
        Local<Object> v8Object = V8CarObject::New(isolate, value);
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
            StringBuilder sb("Create object failed! ClassName: ");
            sb += *u8ClassName;
            isolate->ThrowException(Exception::TypeError(
                    v8::String::NewFromUtf8(isolate, sb.ToString().string())));
            args.GetReturnValue().SetUndefined();
            return;
        }

        CarValue value;
        value.mType = CarDataType_Interface;
        value.mObjectValue = carObject;
        Local<Object> v8Object = V8CarObject::New(isolate, value);
        args.GetReturnValue().Set(v8Object);
        return;
    }

    Int32 paramCount = args.Length() - 2;
    AutoPtr<IConstructorInfo> constructorInfo;
    ec = classInfo->GetConstructorInfoByParamCount(paramCount,
            (IConstructorInfo**)&constructorInfo);
    if (FAILED(ec)) {
        StringBuilder sb("Wrong number of parameters! ClassName: ");
        sb += *u8ClassName;
        sb += " ParamCount: ";
        sb += paramCount;
        isolate->ThrowException(Exception::TypeError(
                v8::String::NewFromUtf8(isolate, sb.ToString().string())));
        args.GetReturnValue().SetUndefined();
        return;
    }

    AutoPtr< ArrayOf<IParamInfo*> > paramInfos = ArrayOf<IParamInfo*>::Alloc(paramCount);
    ec = constructorInfo->GetAllParamInfos(paramInfos);
    if (FAILED(ec)) {
        StringBuilder sb("GetAllParamInfos failed! ClassName: ");
        sb += *u8ClassName;
        isolate->ThrowException(Exception::TypeError(
                v8::String::NewFromUtf8(isolate, sb.ToString().string())));
        args.GetReturnValue().SetUndefined();
        return;
    }

    AutoPtr<IArgumentList> argumentlist;
    ec = constructorInfo->CreateArgumentList((IArgumentList**)&argumentlist);
    if (FAILED(ec)) {
        StringBuilder sb("CreateArgumentList failed! ClassName: ");
        sb += *u8ClassName;
        isolate->ThrowException(Exception::TypeError(
                v8::String::NewFromUtf8(isolate, sb.ToString().string())));
        args.GetReturnValue().SetUndefined();
        return;
    }

    // TODO: filling argumentlist
    assert(0);

    AutoPtr<IInterface> carObject;
    ec = constructorInfo->CreateObject(argumentlist, (IInterface**)&carObject);
    if (FAILED(ec)) {
        StringBuilder sb("Create object failed! ClassName: ");
        sb += *u8ClassName;
        isolate->ThrowException(Exception::TypeError(
                v8::String::NewFromUtf8(isolate, sb.ToString().string())));
        args.GetReturnValue().SetUndefined();
        return;
    }

    CarValue value;
    value.mType = CarDataType_Interface;
    value.mObjectValue = carObject;
    Local<Object> v8Object = V8CarObject::New(isolate, value);
    args.GetReturnValue().Set(v8Object);
}

} //namespace Node
} //namespace Elastos

void init(Handle<Object> exports, Handle<Object> module)
{
    NODE_SET_METHOD(exports, "require", Elastos::Node::Require);
}

NODE_MODULE(elastos, init)
