/*
* Copyright (C) 2006, 2007, 2008, 2009 Google Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*
*     * Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following disclaimer
* in the documentation and/or other materials provided with the
* distribution.
*     * Neither the name of Google Inc. nor the names of its
* contributors may be used to endorse or promote products derived from
* this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "config.h"

#include "V8NPObject.h"

/*----------------------------------------------------------------
#include "HTMLPlugInElement.h"
----------------------------------------------------------------*/
#include "NPV8Object.h"
#include "V8DOMMap.h"
/*----------------------------------------------------------------
#include "V8HTMLAppletElement.h"
#include "V8HTMLEmbedElement.h"
#include "V8HTMLObjectElement.h"
#include "V8Helpers.h"
----------------------------------------------------------------*/
#include "V8NPUtils.h"
#include "V8Proxy.h"
#include "npruntime_impl.h"
#include "npruntime_priv.h"
#include <wtf/OwnArrayPtr.h>
#include "V8Utilities.h"

#include <utils/Log.h>

#define WASSERT(assertion) do \
    if (!(assertion)) { \
        WTFReportAssertionFailure(__FILE__, __LINE__, WTF_PRETTY_FUNCTION, #assertion); \
        CRASH(); \
    } \
while (0)

namespace WebCore {

enum InvokeFunctionType {
    InvokeMethod = 1,
    InvokeConstruct = 2,
    InvokeDefault = 3
};

struct IdentifierRep {
    int number() const { return m_isString ? 0 : m_value.m_number; }
    const char* string() const { return m_isString ? m_value.m_string : 0; }

    union {
        const char* m_string;
        int m_number;
    } m_value;
    bool m_isString;
};

// FIXME: need comments.
// Params: holder could be HTMLEmbedElement or NPObject
//static v8::Handle<v8::Value> npObjectInvokeImpl(const v8::Arguments& args, InvokeFunctionType functionId)
static void npObjectInvokeImpl(const v8::FunctionCallbackInfo<v8::Value>& info, InvokeFunctionType functionId)
{
    NPObject* npObject;

    npObject = v8ObjectToNPObject(info.Holder());

/*----------------------------------------------------------------
    // Verify that our wrapper wasn't using a NPObject which has already been deleted.
    if (!npObject || !_NPN_IsAlive(npObject))
        return throwError("NPObject deleted", V8Proxy::ReferenceError);
----------------------------------------------------------------*/

    // Wrap up parameters.
    int numArgs = info.Length();
    OwnArrayPtr<NPVariant> npArgs = adoptArrayPtr(new NPVariant[numArgs]);

    for (int i = 0; i < numArgs; i++) {
        convertV8ObjectToNPVariant(info[i], npObject, &npArgs[i]);
    }

    NPVariant result;
    VOID_TO_NPVARIANT(result);

    bool retval = true;
    switch (functionId) {
    case InvokeMethod:
        if (npObject->_class->invoke) {
            v8::Handle<v8::String> functionName = v8::Handle<v8::String>::Cast(info.Data());
            NPIdentifier identifier = getStringIdentifier(functionName);
            retval = npObject->_class->invoke(npObject, identifier, npArgs.get(), numArgs, &result);
        }
        break;
    case InvokeConstruct:
        if (npObject->_class->construct)
            retval = npObject->_class->construct(npObject, npArgs.get(), numArgs, &result);
        break;
    case InvokeDefault:
        if (npObject->_class->invokeDefault)
            retval = npObject->_class->invokeDefault(npObject, npArgs.get(), numArgs, &result);
        break;
    default:
        break;
    }
/*----------------------------------------------------------------
    if (!retval)
        throwError("Error calling method on NPObject.", V8Proxy::GeneralError);
----------------------------------------------------------------*/

    for (int i = 0; i < numArgs; i++) {
        _NPN_ReleaseVariantValue(&npArgs[i]);
    }

    // Unwrap return values.
    v8::Handle<v8::Value> returnValue = convertNPVariantToV8Object(&result, npObject);
    _NPN_ReleaseVariantValue(&result);

    //return returnValue;
    info.GetReturnValue().Set(returnValue);
}


void npObjectMethodHandler(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    npObjectInvokeImpl(info, InvokeMethod);
}

void npObjectInvokeDefaultHandler(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.IsConstructCall())
        npObjectInvokeImpl(info, InvokeConstruct);

    npObjectInvokeImpl(info, InvokeDefault);
}


static void weakTemplateCallback(v8::Persistent<v8::Value>, void* parameter);

// NPIdentifier is PrivateIdentifier*.
/*----------------------------------------------------------------
static WeakReferenceMap<PrivateIdentifier, v8::FunctionTemplate> staticTemplateMap(&weakTemplateCallback);
----------------------------------------------------------------*/

static void weakTemplateCallback(v8::Persistent<v8::Value> object, void* parameter)
{
/*----------------------------------------------------------------
    PrivateIdentifier* identifier = static_cast<PrivateIdentifier*>(parameter);
    ASSERT(identifier);
    ASSERT(staticTemplateMap.contains(identifier));

    staticTemplateMap.forget(identifier);
----------------------------------------------------------------*/
    //ASSERT(0);
}


static v8::Handle<v8::Value> npObjectGetProperty(v8::Local<v8::Object> self, NPIdentifier identifier, v8::Local<v8::Value> key)
{
    NPObject* npObject = v8ObjectToNPObject(self);

    v8::Isolate* isolate = v8::Isolate::GetCurrent();

/*----------------------------------------------------------------
    // Verify that our wrapper wasn't using a NPObject which
    // has already been deleted.
    if (!npObject || !_NPN_IsAlive(npObject))
        return throwError("NPObject deleted", V8Proxy::ReferenceError);
----------------------------------------------------------------*/

    if (npObject->_class->hasProperty && npObject->_class->hasProperty(npObject, identifier)
        && npObject->_class->getProperty) {

        NPVariant result;
        VOID_TO_NPVARIANT(result);
        if (!npObject->_class->getProperty(npObject, identifier, &result))
            return v8::Handle<v8::Value>();

        v8::Handle<v8::Value> returnValue = convertNPVariantToV8Object(&result, npObject);
        _NPN_ReleaseVariantValue(&result);
        return returnValue;

    }

    if (key->IsString() && npObject->_class->hasMethod && npObject->_class->hasMethod(npObject, identifier)) {

        PrivateIdentifier* id = static_cast<PrivateIdentifier*>(identifier);

/*----TODO------------------------------------------------------------
        v8::Persistent<v8::FunctionTemplate> functionTemplate = staticTemplateMap.get(id);
        // Cache templates using identifier as the key.

        if (functionTemplate.IsEmpty()) {
            // Create a new template.
            v8::Local<v8::FunctionTemplate> temp = v8::FunctionTemplate::New();
            temp->SetCallHandler(npObjectMethodHandler, key);
            functionTemplate = v8::Persistent<v8::FunctionTemplate>::New(temp);
            staticTemplateMap.set(id, functionTemplate);
        }
----------------------------------------------------------------*/

        v8::Local<v8::FunctionTemplate> functionTemplate = v8::FunctionTemplate::New(isolate);
        functionTemplate->SetCallHandler(npObjectMethodHandler, key);

        // FunctionTemplate caches function for each context.
        v8::Local<v8::Function> v8Function = functionTemplate->GetFunction();
        v8Function->SetName(v8::Handle<v8::String>::Cast(key));
        return v8Function;
    }

    return v8::Handle<v8::Value>();
}

void npObjectNamedPropertyGetter(v8::Local<v8::String> name, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    NPIdentifier identifier = getStringIdentifier(name);
    v8::Handle<v8::Value> value = npObjectGetProperty(info.Holder(), identifier, name);
    info.GetReturnValue().Set(value);
}

void npObjectIndexedPropertyGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    NPIdentifier identifier = _NPN_GetIntIdentifier(index);
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::Handle<v8::Value> value = npObjectGetProperty(info.Holder(), identifier, v8::Number::New(isolate,index));
    info.GetReturnValue().Set(value);
}

/*----------------------------------------------------------------
v8::Handle<v8::Value> npObjectGetNamedProperty(v8::Local<v8::Object> self, v8::Local<v8::String> name)
{
    NPIdentifier identifier = getStringIdentifier(name);
    return npObjectGetProperty(self, identifier, name);
}

v8::Handle<v8::Value> npObjectGetIndexedProperty(v8::Local<v8::Object> self, uint32_t index)
{
    NPIdentifier identifier = _NPN_GetIntIdentifier(index);
    return npObjectGetProperty(self, identifier, v8::Number::New(index));
}
----------------------------------------------------------------*/

void npObjectQueryProperty(v8::Local<v8::String> name, const v8::PropertyCallbackInfo<v8::Integer>& info)
{
    NPIdentifier identifier = getStringIdentifier(name);
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::Handle<v8::Value> value = npObjectGetProperty(info.Holder(), identifier, name).IsEmpty() ? v8::Handle<v8::Integer>() : v8::Integer::New(isolate,v8::None);
    info.GetReturnValue().Set(v8::Handle<v8::Integer>::Cast(value));
}

static v8::Handle<v8::Value> npObjectSetProperty(v8::Local<v8::Object> self, NPIdentifier identifier, v8::Local<v8::Value> value)
{
    NPObject* npObject = v8ObjectToNPObject(self);

    // Verify that our wrapper wasn't using a NPObject which has already been deleted.
    if (!npObject || !_NPN_IsAlive(npObject)) {
/*----------------------------------------------------------------
        throwError("NPObject deleted", V8Proxy::ReferenceError);
----------------------------------------------------------------*/
        return value;  // Intercepted, but an exception was thrown.
    }

    if (npObject->_class->hasProperty && npObject->_class->hasProperty(npObject, identifier)
        && npObject->_class->setProperty) {

        NPVariant npValue;
        VOID_TO_NPVARIANT(npValue);
        convertV8ObjectToNPVariant(value, npObject, &npValue);
        bool success = npObject->_class->setProperty(npObject, identifier, &npValue);
        _NPN_ReleaseVariantValue(&npValue);
        if (success)
            return value; // Intercept the call.
    }
/*----------------------------------------------------------------
    return notHandledByInterceptor();
----------------------------------------------------------------*/
    //ASSERT(0);
    v8::Handle<v8::Value> retValue;
    return retValue;
}


void npObjectNamedPropertySetter(v8::Local<v8::String> name, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    NPIdentifier identifier = getStringIdentifier(name);
    npObjectSetProperty(info.Holder(), identifier, value);
}

void npObjectIndexedPropertySetter(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    NPIdentifier identifier = _NPN_GetIntIdentifier(index);
    npObjectSetProperty(info.Holder(), identifier, value);
}

v8::Handle<v8::Value> npObjectSetNamedProperty(v8::Local<v8::Object> self, v8::Local<v8::String> name, v8::Local<v8::Value> value)
{
    NPIdentifier identifier = getStringIdentifier(name);
    return npObjectSetProperty(self, identifier, value);
}

v8::Handle<v8::Value> npObjectSetIndexedProperty(v8::Local<v8::Object> self, uint32_t index, v8::Local<v8::Value> value)
{
    NPIdentifier identifier = _NPN_GetIntIdentifier(index);
    return npObjectSetProperty(self, identifier, value);
}

void npObjectPropertyEnumerator(const v8::PropertyCallbackInfo<v8::Array>& info, bool namedProperty)
{
    NPObject* npObject = v8ObjectToNPObject(info.Holder());

    // Verify that our wrapper wasn't using a NPObject which
    // has already been deleted.
/*----------------------------------------------------------------
    if (!npObject || !_NPN_IsAlive(npObject))
        throwError("NPObject deleted", V8Proxy::ReferenceError);
----------------------------------------------------------------*/

    v8::Isolate* isolate = v8::Isolate::GetCurrent();

    if (NP_CLASS_STRUCT_VERSION_HAS_ENUM(npObject->_class) && npObject->_class->enumerate) {
        uint32_t count;
        NPIdentifier* identifiers;
        if (npObject->_class->enumerate(npObject, &identifiers, &count)) {
            v8::Handle<v8::Array> properties = v8::Array::New(isolate,count);
            for (uint32_t i = 0; i < count; ++i) {
                IdentifierRep* identifier = static_cast<IdentifierRep*>(identifiers[i]);
                if (namedProperty) {
                    properties->Set(v8::Integer::New(isolate,i), v8::String::NewFromUtf8(isolate,identifier->string()));
                }
                else {
                    properties->Set(v8::Integer::New(isolate,i), v8::Integer::New(isolate,identifier->number()));
                }
            }

            //return properties;
            info.GetReturnValue().Set(properties);
            return;
        }
    }

    return;
}

void npObjectNamedPropertyEnumerator(const v8::PropertyCallbackInfo<v8::Array>& info)
{
    npObjectPropertyEnumerator(info, true);
}

void npObjectIndexedPropertyEnumerator(const v8::PropertyCallbackInfo<v8::Array>& info)
{
    npObjectPropertyEnumerator(info, false);
}

static void weakNPObjectCallback(v8::Persistent<v8::Value>, void* parameter);

/*----------------------------------------------------------------*/
//static DOMWrapperMap<NPObject> staticNPObjectMap(&weakNPObjectCallback);
/*----------------------------------------------------------------*/

static void weakNPObjectCallback(v8::Persistent<v8::Value> object, void* parameter)
{
/*----------------------------------------------------------------
    NPObject* npObject = static_cast<NPObject*>(parameter);
    ASSERT(staticNPObjectMap.contains(npObject));
    ASSERT(npObject);

    // Must remove from our map before calling _NPN_ReleaseObject(). _NPN_ReleaseObject can call ForgetV8ObjectForNPObject, which
    // uses the table as well.
    staticNPObjectMap.forget(npObject);

    if (_NPN_IsAlive(npObject))
        _NPN_ReleaseObject(npObject);
----------------------------------------------------------------*/
    //ASSERT(0);
}


v8::Local<v8::Object> createV8ObjectForNPObject(NPObject* object, NPObject* root)
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();

    // If this is a v8 object, just return it.
    if (object->_class == npScriptObjectClass) {
        V8NPObject* v8NPObject = reinterpret_cast<V8NPObject*>(object);

        if (v8NPObject->v8Object.IsWeak()) {
            return v8::Local<v8::Object>::New(isolate,v8NPObject->v8Object);
        }
        else {
            return *reinterpret_cast<v8::Local<v8::Object>*>(const_cast<v8::Persistent<v8::Object>*>(&v8NPObject->v8Object));
        }
    }

/*----TODO------------------------------------------------------------
    // If we've already wrapped this object, just return it.
    if (staticNPObjectMap.contains(object))
        return v8::Local<v8::Object>::New(staticNPObjectMap.get(object));
    // FIXME: we should create a Wrapper type as a subclass of JSObject. It has two internal fields, field 0 is the wrapped
    // pointer, and field 1 is the type. There should be an api function that returns unused type id. The same Wrapper type
    // can be used by DOM bindings.
----------------------------------------------------------------*/

    static v8::Persistent<v8::FunctionTemplate> npObjectDesc_1;

    v8::Local<v8::FunctionTemplate> npObjectDesc;
    if (!npObjectDesc_1.IsEmpty()) {
        npObjectDesc = v8::Local<v8::FunctionTemplate>::New(isolate,npObjectDesc_1);
    }
    else {
        npObjectDesc = v8::Local<v8::FunctionTemplate>::New(isolate,v8::FunctionTemplate::New(isolate));
        npObjectDesc->InstanceTemplate()->SetInternalFieldCount(npObjectInternalFieldCount);
        npObjectDesc->InstanceTemplate()->SetNamedPropertyHandler(npObjectNamedPropertyGetter, npObjectNamedPropertySetter, npObjectQueryProperty, 0, npObjectNamedPropertyEnumerator);
        npObjectDesc->InstanceTemplate()->SetIndexedPropertyHandler(npObjectIndexedPropertyGetter, npObjectIndexedPropertySetter, 0, 0, npObjectIndexedPropertyEnumerator);
        npObjectDesc->InstanceTemplate()->SetCallAsFunctionHandler(npObjectInvokeDefaultHandler);
    }

    v8::Handle<v8::Function> v8Function = npObjectDesc->GetFunction();

    v8::Local<v8::Object> value = SafeAllocation::newInstance(v8Function);

    // If we were unable to allocate the instance, we avoid wrapping and registering the NP object.
    if (value.IsEmpty())
        return value;

/*----------------------------------------------------------------
    V8DOMWrapper::setDOMWrapper(value, npObjectTypeInfo(), object);
----------------------------------------------------------------*/
    ASSERT(value->InternalFieldCount() >= 2);
    value->SetAlignedPointerInInternalField(v8DOMWrapperObjectIndex, object);
    value->SetAlignedPointerInInternalField(v8DOMWrapperTypeIndex, npObjectTypeInfo());

    // KJS retains the object as part of its wrapper (see Bindings::CInstance).
    _NPN_RetainObject(object);

    _NPN_RegisterObject(object, root);

/*----------------------------------------------------------------
    // Maintain a weak pointer for v8 so we can cleanup the object.
    //v8::Persistent<v8::Object> weakRef = v8::Persistent<v8::Object>::New(value);
    v8::Persistent<v8::Object> weakRef = v8::Persistent<v8::Object>::New(isolate,value);
    staticNPObjectMap.set(object, weakRef);
----------------------------------------------------------------*/

    return value;
}

void forgetV8ObjectForNPObject(NPObject* object)
{
/*----------------------------------------------------------------
    if (staticNPObjectMap.contains(object)) {
        v8::HandleScope scope;
        v8::Persistent<v8::Object> handle(staticNPObjectMap.get(object));
        V8DOMWrapper::setDOMWrapper(handle, npObjectTypeInfo(), 0);
        staticNPObjectMap.forget(object);
        _NPN_ReleaseObject(object);
    }
----------------------------------------------------------------*/
    //ASSERT(0);
}

} // namespace WebCore
