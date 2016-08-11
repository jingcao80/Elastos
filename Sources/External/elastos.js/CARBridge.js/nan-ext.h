#ifndef __CAR_BRIDGE_NAN_EXT_H
# define __CAR_BRIDGE_NAN_EXT_H

# include <cstddef>

# include <node.h>

# include <nan.h>

# include "macros.h"


# define NAN_METHOD_RETURN_NULL() info.GetReturnValue().SetNull()
# define NAN_METHOD_RETURN_UNDEFINED() info.GetReturnValue().SetUndefined()
# define NAN_METHOD_RETURN_EMPTY_STRING() info.GetReturnValue().SetEmptyString()
# define NAN_METHOD_RETURN_VALUE(value) info.GetReturnValue().Set(value)

# define NAN_GETTER_RETURN_NULL() info.GetReturnValue().SetNull()
# define NAN_GETTER_RETURN_UNDEFINED() info.GetReturnValue().SetUndefined()
# define NAN_GETTER_RETURN_EMPTY_STRING() info.GetReturnValue().SetEmptyString()
# define NAN_GETTER_RETURN_VALUE(value) info.GetReturnValue().Set(value)

# define NAN_SETTER_RETURN_NULL() info.GetReturnValue().SetNull()
# define NAN_SETTER_RETURN_UNDEFINED() info.GetReturnValue().SetUndefined()
# define NAN_SETTER_RETURN_EMPTY_STRING() info.GetReturnValue().SetEmptyString()
# define NAN_SETTER_RETURN_VALUE(value) info.GetReturnValue().Set(value)

# define NAN_PROPERTY_GETTER_RETURN_NULL() info.GetReturnValue().SetNull()
# define NAN_PROPERTY_GETTER_RETURN_UNDEFINED() info.GetReturnValue().SetUndefined()
# define NAN_PROPERTY_GETTER_RETURN_EMPTY_STRING() info.GetReturnValue().SetEmptyString()
# define NAN_PROPERTY_GETTER_RETURN_VALUE(value) info.GetReturnValue().Set(value)

# define NAN_PROPERTY_SETTER_RETURN_NULL() info.GetReturnValue().SetNull()
# define NAN_PROPERTY_SETTER_RETURN_UNDEFINED() info.GetReturnValue().SetUndefined()
# define NAN_PROPERTY_SETTER_RETURN_EMPTY_STRING() info.GetReturnValue().SetEmptyString()
# define NAN_PROPERTY_SETTER_RETURN_VALUE(value) info.GetReturnValue().Set(value)

# define NAN_PROPERTY_ENUMERATOR_RETURN_NULL() info.GetReturnValue().SetNull()
# define NAN_PROPERTY_ENUMERATOR_RETURN_UNDEFINED() info.GetReturnValue().SetUndefined()
# define NAN_PROPERTY_ENUMERATOR_RETURN_EMPTY_STRING() info.GetReturnValue().SetEmptyString()
# define NAN_PROPERTY_ENUMERATOR_RETURN_VALUE(value) info.GetReturnValue().Set(value)

# define NAN_PROPERTY_DELETER_RETURN_NULL() info.GetReturnValue().SetNull()
# define NAN_PROPERTY_DELETER_RETURN_UNDEFINED() info.GetReturnValue().SetUndefined()
# define NAN_PROPERTY_DELETER_RETURN_EMPTY_STRING() info.GetReturnValue().SetEmptyString()
# define NAN_PROPERTY_DELETER_RETURN_VALUE(value) info.GetReturnValue().Set(value)

# define NAN_PROPERTY_QUERY_RETURN_NULL() info.GetReturnValue().SetNull()
# define NAN_PROPERTY_QUERY_RETURN_UNDEFINED() info.GetReturnValue().SetUndefined()
# define NAN_PROPERTY_QUERY_RETURN_EMPTY_STRING() info.GetReturnValue().SetEmptyString()
# define NAN_PROPERTY_QUERY_RETURN_VALUE(value) info.GetReturnValue().Set(value)

# define NAN_INDEX_GETTER_RETURN_NULL() info.GetReturnValue().SetNull()
# define NAN_INDEX_GETTER_RETURN_UNDEFINED() info.GetReturnValue().SetUndefined()
# define NAN_INDEX_GETTER_RETURN_EMPTY_STRING() info.GetReturnValue().SetEmptyString()
# define NAN_INDEX_GETTER_RETURN_VALUE(value) info.GetReturnValue().Set(value)

# define NAN_INDEX_SETTER_RETURN_NULL() info.GetReturnValue().SetNull()
# define NAN_INDEX_SETTER_RETURN_UNDEFINED() info.GetReturnValue().SetUndefined()
# define NAN_INDEX_SETTER_RETURN_EMPTY_STRING() info.GetReturnValue().SetEmptyString()
# define NAN_INDEX_SETTER_RETURN_VALUE(value) info.GetReturnValue().Set(value)

# define NAN_INDEX_ENUMERATOR_RETURN_NULL() info.GetReturnValue().SetNull()
# define NAN_INDEX_ENUMERATOR_RETURN_UNDEFINED() info.GetReturnValue().SetUndefined()
# define NAN_INDEX_ENUMERATOR_RETURN_EMPTY_STRING() info.GetReturnValue().SetEmptyString()
# define NAN_INDEX_ENUMERATOR_RETURN_VALUE(value) info.GetReturnValue().Set(value)

# define NAN_INDEX_DELETER_RETURN_NULL() info.GetReturnValue().SetNull()
# define NAN_INDEX_DELETER_RETURN_UNDEFINED() info.GetReturnValue().SetUndefined()
# define NAN_INDEX_DELETER_RETURN_EMPTY_STRING() info.GetReturnValue().SetEmptyString()
# define NAN_INDEX_DELETER_RETURN_VALUE(value) info.GetReturnValue().Set(value)

# define NAN_INDEX_QUERY_RETURN_NULL() info.GetReturnValue().SetNull()
# define NAN_INDEX_QUERY_RETURN_UNDEFINED() info.GetReturnValue().SetUndefined()
# define NAN_INDEX_QUERY_RETURN_EMPTY_STRING() info.GetReturnValue().SetEmptyString()
# define NAN_INDEX_QUERY_RETURN_VALUE(value) info.GetReturnValue().Set(value)



CAR_BRIDGE_NAMESPACE_BEGIN

template<class T>
using CopyablePersistent = ::Nan::Persistent<T, ::Nan::CopyablePersistentTraits<T>>;

inline ::Nan::Maybe<bool> DefineOwnProperty(::v8::Local<::v8::Object> object,
        ::v8::Local<::v8::String> name,
        ::v8::Local<::v8::Value> value,
        ::v8::PropertyAttribute attributes = ::v8::None);

template<class T>
inline void SetMethod(::v8::Local<T> receiver,
        char const *name,
        ::Nan::FunctionCallback callback, ::v8::Local<::v8::Value> data = ::v8::Local<::v8::Value>(),
        ::v8::Local<::v8::Signature> signature = ::v8::Local<::v8::Signature>(),
        ::v8::PropertyAttribute attributes = ::v8::None);

template<class T>
inline void SetMethod(::v8::Local<T> receiver,
        char const *name,
        ::Nan::FunctionCallback callback, ::v8::Local<::v8::Value> data,
        ::v8::PropertyAttribute attributes);

template<class T>
inline void SetMethod(::v8::Local<T> receiver,
        char const *name,
        ::Nan::FunctionCallback callback,
        ::v8::PropertyAttribute attributes);

inline void SetPrototypeMethod(::v8::Local<::v8::FunctionTemplate> functionTemplate,
        char const *name,
        ::Nan::FunctionCallback callback, ::v8::Local<::v8::Value> data = ::v8::Local<::v8::Value>(),
        ::v8::PropertyAttribute attributes = ::v8::None);

inline void SetPrototypeMethod(::v8::Local<::v8::FunctionTemplate> functionTemplate,
        char const *name,
        ::Nan::FunctionCallback callback,
        ::v8::PropertyAttribute attributes);

inline void Export(::Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE target, char const *name, ::v8::Local<::v8::Value> value);

CAR_BRIDGE_NAMESPACE_END

# include "nan-ext-cc.h"

#endif
