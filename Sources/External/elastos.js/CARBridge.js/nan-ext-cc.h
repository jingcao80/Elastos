#ifndef __CAR_BRIDGE_NAN_EXT_CC_H
# define __CAR_BRIDGE_NAN_EXT_CC_H

# include <node.h>

# include <nan.h>

# include "macros.h"



CAR_BRIDGE_NAMESPACE_BEGIN

inline ::Nan::Maybe<bool> DefineOwnProperty(::v8::Local<::v8::Object> object,
        ::v8::Local<::v8::String> name,
        ::v8::Local<::v8::Value> value,
        ::v8::PropertyAttribute attributes)
{
    return object->DefineOwnProperty(::Nan::GetCurrentContext(), name, value, attributes);
}

template<class T>
inline void SetMethod(::v8::Local<T> receiver,
        char const *name,
        ::Nan::FunctionCallback callback, ::v8::Local<::v8::Value> data, ::v8::Local<::v8::Signature> signature,
        ::v8::PropertyAttribute attributes)
{}

template<>
inline void SetMethod<::v8::Template>(::v8::Local<::v8::Template> template_,
        char const *name,
        ::Nan::FunctionCallback callback, ::v8::Local<::v8::Value> data, ::v8::Local<::v8::Signature> signature,
        ::v8::PropertyAttribute attributes)
{
    ::Nan::HandleScope scope;

    ::v8::Local<::v8::FunctionTemplate> methodTemplate;

    methodTemplate = ::Nan::New<::v8::FunctionTemplate>(callback, data, signature);

    ::Nan::SetTemplate(template_, ::Nan::New(name).ToLocalChecked(), methodTemplate, attributes);
}

template<>
inline void SetMethod<::v8::Object>(::v8::Local<::v8::Object> object,
        char const *name,
        ::Nan::FunctionCallback callback, ::v8::Local<::v8::Value> data, ::v8::Local<::v8::Signature> signature,
        ::v8::PropertyAttribute attributes)
{
    ::Nan::HandleScope scope;

    ::v8::Local<::v8::Function> method;

    method = ::Nan::GetFunction(::Nan::New<::v8::FunctionTemplate>(callback, data, signature)).ToLocalChecked();

    method->SetName(::Nan::New(name).ToLocalChecked());

    DefineOwnProperty(object, ::Nan::New(name).ToLocalChecked(), method, attributes);
}

template<class T>
inline void SetMethod(::v8::Local<T> receiver,
        char const *name,
        ::Nan::FunctionCallback callback, ::v8::Local<::v8::Value> data,
        ::v8::PropertyAttribute attributes)
{
    SetMethod(receiver, name, callback, data, ::v8::Local<::v8::Signature>(), attributes);
}

template<class T>
inline void SetMethod(::v8::Local<T> receiver,
        char const *name,
        ::Nan::FunctionCallback callback,
        ::v8::PropertyAttribute attributes)
{
    SetMethod(receiver, name, callback, ::v8::Local<::v8::Value>(), ::v8::Local<::v8::Signature>(), attributes);
}

inline void SetPrototypeMethod(::v8::Local<::v8::FunctionTemplate> functionTemplate,
        char const *name,
        ::Nan::FunctionCallback callback, ::v8::Local<::v8::Value> data,
        ::v8::PropertyAttribute attributes)
{
    SetMethod(functionTemplate->PrototypeTemplate(),
            name,
            callback, data, ::Nan::New<::v8::Signature>(functionTemplate),
            attributes);
}

inline void SetPrototypeMethod(::v8::Local<::v8::FunctionTemplate> functionTemplate,
        char const *name,
        ::Nan::FunctionCallback callback,
        ::v8::PropertyAttribute attributes)
{
    SetPrototypeMethod(functionTemplate, name, callback, ::v8::Local<::v8::Value>(), attributes);
}

inline void Export(::Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE target, char const *name, ::v8::Local<::v8::Value> value)
{
    ::Nan::Set(target, ::Nan::New<::v8::String>(name).ToLocalChecked(), value);
}

CAR_BRIDGE_NAMESPACE_END

#endif
