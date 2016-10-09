/*
 * Copyright 2010, The Android Open Source Project
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ElNode.h"
#include "config.h"

#if ENABLE(CAR_BRIDGE)

#include <wtf/PassOwnPtr.h>
#include <wtf/text/CString.h>
#include <wtf/text/StringBuilder.h>

#include "elastos.h"

#include "CarInstanceV8.h"
#include "CarClassV8.h"
#include "CarMethod.h"

#include "CarNPObjectV8.h"
//#if PLATFORM(ANDROID)
#include "npruntime_impl.h"
#include "npruntime_priv.h"
//#endif // PLATFORM(ANDROID)
#include "CarValueV8.h"

#include "CarUtilityPrivate.h"
#include "CarCallbackProxy.h"

#include <utils/Log.h>

namespace JSC {
namespace Bindings {

//--------exception----begin----
const char* ToCString(const v8::String::Utf8Value& value) {
    return *value ? *value : "<string conversion failed>";
}

void ReportException(v8::Isolate* isolate, v8::TryCatch* try_catch) {
    v8::HandleScope handle_scope(isolate);
    v8::String::Utf8Value exception(try_catch->Exception());
    const char* exception_string = ToCString(exception);
    v8::Handle<v8::Message> message = try_catch->Message();
    if (message.IsEmpty()) {
        // V8 didn't provide any extra information about this error; just
        // print the exception.
        ALOGD("js error====%s\n", exception_string);
    }
    else {
        // Print (filename):(line number): (message).
        v8::String::Utf8Value filename(message->GetScriptOrigin().ResourceName());
        const char* filename_string = ToCString(filename);
        int linenum = message->GetLineNumber();
        ALOGD("js error====%s:%i: %s\n", filename_string, linenum, exception_string);
        // Print line of source code.
        v8::String::Utf8Value sourceline(message->GetSourceLine());
        const char* sourceline_string = ToCString(sourceline);
        ALOGD("%s\n", sourceline_string);
        // Print wavy underline (GetUnderline is deprecated).
        int start = message->GetStartColumn();
        for (int i = 0; i < start; i++) {
        ALOGD(" ");
        }
        int end = message->GetEndColumn();
        for (int i = start; i < end; i++) {
        ALOGD("^");
        }
        ALOGD("\n");
        v8::String::Utf8Value stack_trace(try_catch->StackTrace());
        if (stack_trace.length() > 0) {
            const char* stack_trace_string = ToCString(stack_trace);
            ALOGD("%s\n", stack_trace_string);
        }

        ASSERT(0);
    }
}
//--------exception----end----

WebCore::LocalDOMWindow* getRootObject()
{
    static WebCore::LocalDOMWindow* rootObject = NULL;
    if (!rootObject) {
        // For Android 4.2.2
        //rootObject = WebCore::V8Proxy::retrieveWindow(WebCore::V8Proxy::currentContext());

        // For Android 5.0.1
        v8::Isolate* isolate = v8::Isolate::GetCurrent();
        rootObject = WebCore::currentDOMWindow(isolate);

        // For Node: do nothing, to be finished: binding to app/activity
    }
    return rootObject;
}

void convertNPVariantToCarValue_In(NPVariant value, CarValue* result)
{
    CarDataType carDataType = 0;

    AutoPtr<IDataTypeInfo> dataTypeInfo;
    if (result->mObjectWrapper.Get()) {
        dataTypeInfo = result->mObjectWrapper->getDataTypeInfo();
        if (dataTypeInfo.Get()) {
            dataTypeInfo->GetDataType(&carDataType);
        }
        else {
            //TODO:Shoud not reach here: all CObjectWrapper must have typeInfo
            carDataType = CarDataType_Interface;
            ALOGD("convertNPVariantToCarValue_In========dataTypeInfo not exist!");
        }
    }
    else {
        //TODO:Shoud not reach here: all CObjectWrapper must exist
        carDataType = CarDataType_Interface;
        ALOGD("convertNPVariantToCarValue_In========mObjectWrapper not exist!");
    }

    NPVariantType type = value.type;

    // const char* tmpType = ClassNameFromCarDataType((CarDataType)(carDataType));
    // ALOGD("====convertNPVariantToCarValue_In====begin====carDataTypeClassName:%s", tmpType);

    switch (carDataType) {
        case CarDataType_Int16:
            if (type == NPVariantType_Int32) {
                result->value.mInt16Value = (Elastos::Int16)(NPVARIANT_TO_INT32(value));
            }
            else if (type == NPVariantType_Double) {
                result->value.mInt16Value = (Elastos::Int16)(NPVARIANT_TO_DOUBLE(value));
            }
            break;
        case CarDataType_Int32:
        {
            Int32 _value;
            if (type == NPVariantType_Int32) {
                _value = (Elastos::Int32)(NPVARIANT_TO_INT32(value));
            }
            else if (type == NPVariantType_Double) {
                _value = (Elastos::Int32)(NPVARIANT_TO_DOUBLE(value));
            }

            if (result->mTagSetLocalPtr) {
                ALOGD("convertNPVariantToCarValue_In========mTagSetLocalPtr====begin====");

                CarQuintet* carArray;

                NPObject* object = NPVARIANT_IS_OBJECT(value) ? NPVARIANT_TO_OBJECT(value) : 0;
                NPVariant npvLength;
                bool success = _NPN_GetProperty(0, object, _NPN_GetStringIdentifier("length"), &npvLength);
                if (!success) {
                    // No length property so we don't know how many elements to put into the array.
                    // Treat this as an error.
                    // JSC sends null for an array that is not an array of strings or basic types,
                    // do this also in the unknown length case.
                    break;
                }

                // Convert to null if the length property is not a number.
                if (!NPVARIANT_IS_INT32(npvLength) && !NPVARIANT_IS_DOUBLE(npvLength)) {
                    break;
                }

                // Convert to null if the length property is out of bounds.
                double doubleLength = NPVARIANT_IS_INT32(npvLength) ? NPVARIANT_TO_INT32(npvLength) : NPVARIANT_TO_DOUBLE(npvLength);
                if (doubleLength < 0.0 || doubleLength > INT32_MAX) {
                    break;
                }

                Int32 length = static_cast<Int32>(doubleLength);

                for (Int32 i = 0; i < length; i++) {
                    NPVariant npvValue;
                    _NPN_GetProperty(0, object, _NPN_GetIntIdentifier(i), &npvValue);
                    Int32 iVal = 0;
                    if (NPVARIANT_IS_INT32(npvValue)) {
                        if(i == 0)carArray = ArrayOf<Elastos::Int32>::Alloc(length);
                        iVal = NPVARIANT_TO_INT32(npvValue);
                        (*(ArrayOf<Elastos::Int32>**)&carArray)->Set(i,iVal);
                    }
                    else if (NPVARIANT_IS_DOUBLE(npvValue)) {
                        if(i == 0)carArray = ArrayOf<Elastos::Int32>::Alloc(length);
                        iVal = NPVARIANT_TO_DOUBLE(npvValue);
                        (*(ArrayOf<Elastos::Int32>**)&carArray)->Set(i,iVal);
                    }
                    else if (NPVARIANT_IS_STRING(npvValue)) {
                        if(i == 0)carArray = ArrayOf<Elastos::String>::Alloc(length);
                        NPString src = NPVARIANT_TO_STRING(npvValue);
                        Elastos::String* _stringValue;
                        const char* utf8String = strdup(src.UTF8Characters);
                        _stringValue = new Elastos::String(utf8String);
                        Int32 length = _stringValue->GetLength();
                        if( !length ) {
                            *_stringValue = NULL;
                        }
                        (*(ArrayOf<Elastos::String>**)&carArray)->Set(i, *_stringValue);
                    }
                }

                _value = (Int32)carArray;
            }

            result->value.mInt32Value = _value;

            break;
        }
        case CarDataType_Int64:
            if (type == NPVariantType_Int32) {
                result->value.mInt64Value = (Elastos::Int64)(NPVARIANT_TO_INT32(value));
            }
            else if (type == NPVariantType_Double) {
                result->value.mInt64Value = (Elastos::Int64)(NPVARIANT_TO_DOUBLE(value));
            }
            break;
        case CarDataType_Byte:
            if (type == NPVariantType_Int32) {
                result->value.mByteValue = (Elastos::Byte)(NPVARIANT_TO_INT32(value));
            }
            else if (type == NPVariantType_Double) {
                result->value.mByteValue = (Elastos::Byte)(NPVARIANT_TO_DOUBLE(value));
            }
            break;
        case CarDataType_Float:
            if (type == NPVariantType_Int32) {
                result->value.mFloatValue = (Elastos::Float)(NPVARIANT_TO_INT32(value));
            }
            else if (type == NPVariantType_Double) {
                result->value.mFloatValue = (Elastos::Float)(NPVARIANT_TO_DOUBLE(value));
            }
            break;
        case CarDataType_Double:
            if (type == NPVariantType_Int32) {
                result->value.mDoubleValue = (Elastos::Double)(NPVARIANT_TO_INT32(value));
            }
            else if (type == NPVariantType_Double) {
                result->value.mDoubleValue = (Elastos::Double)(NPVARIANT_TO_DOUBLE(value));
            }
            break;
        case CarDataType_Char32:
        {
            NPString src = NPVARIANT_TO_STRING(value);
            result->value.mCharValue = Elastos::String(src.UTF8Characters).GetChar(0);
            break;
        }
        case CarDataType_String:
        {
#ifdef CONVERT_NULL_TO_EMPTY_STRING
            if (type == NPVariantType_Null) {
                result->mStringValue = "";
            } else
#else
            if (type == NPVariantType_String)
#endif
            {
                NPString src = NPVARIANT_TO_STRING(value);
                result->mStringValue = src.UTF8Characters;

                Int32 length = result->mStringValue.GetLength();
                if( !length ) {
                    result->mStringValue = NULL;
                }
            }
            else if (type == NPVariantType_Int32) {
                result->mStringValue = WTF::String::number(NPVARIANT_TO_INT32(value)).utf8().data();
            }
            else if (type == NPVariantType_Bool) {
                result->mStringValue = NPVARIANT_TO_BOOLEAN(value) ? "TRUE" : "FALSE";
            }
            else if (type == NPVariantType_Double) {
                result->mStringValue = WTF::String::number(NPVARIANT_TO_DOUBLE(value)).utf8().data();
            }
            else if (!NPVARIANT_IS_NULL(value)) {
                result->mStringValue = "undefined";
            }
            break;
        }
        case CarDataType_Boolean:
            if (type == NPVariantType_Bool) {
                result->value.mBooleanValue = (Elastos::Boolean)NPVARIANT_TO_BOOLEAN(value);
            }
            break;
        case CarDataType_EMuid:
        {
            NPObject* object = NPVARIANT_IS_OBJECT(value) ? NPVARIANT_TO_OBJECT(value) : NULL;
            NPVariant npvValue;
            _NPN_GetProperty(0, object, _NPN_GetIntIdentifier(0), &npvValue);
            result->value.mIid.mData1 = (Int32)(NPVARIANT_TO_DOUBLE(npvValue));
            _NPN_GetProperty(0, object, _NPN_GetIntIdentifier(1), &npvValue);
            result->value.mIid.mData2 = (Int16)(NPVARIANT_TO_DOUBLE(npvValue));
            _NPN_GetProperty(0, object, _NPN_GetIntIdentifier(2), &npvValue);
            result->value.mIid.mData3 = (Int16)(NPVARIANT_TO_DOUBLE(npvValue));
            _NPN_GetProperty(0, object, _NPN_GetIntIdentifier(3), &npvValue);
            object = NPVARIANT_IS_OBJECT(value) ? NPVARIANT_TO_OBJECT(npvValue) : NULL;
            for (Int32 i = 0; i < 8; i++) {
                _NPN_GetProperty(0, object, _NPN_GetIntIdentifier(i), &npvValue);
                result->value.mIid.mData4[i] = (Int8)(NPVARIANT_TO_DOUBLE(npvValue));
            }

            break;
        }
        case CarDataType_EGuid:
        {
            NPObject* objectGuid = NPVARIANT_IS_OBJECT(value) ? NPVARIANT_TO_OBJECT(value) : NULL;
            NPVariant npvValue;
            _NPN_GetProperty(0, objectGuid, _NPN_GetStringIdentifier("mClsid"), &npvValue);
            NPObject* object = NPVARIANT_IS_OBJECT(npvValue) ? NPVARIANT_TO_OBJECT(npvValue) : NULL;
            EMuid& iid = result->value.mCid.mClsid;
            _NPN_GetProperty(0, object, _NPN_GetIntIdentifier(0), &npvValue);
            iid.mData1 = (Int32)(NPVARIANT_TO_DOUBLE(npvValue));
            _NPN_GetProperty(0, object, _NPN_GetIntIdentifier(1), &npvValue);
            iid.mData2 = (Int16)(NPVARIANT_TO_DOUBLE(npvValue));
            _NPN_GetProperty(0, object, _NPN_GetIntIdentifier(2), &npvValue);
            iid.mData3 = (Int16)(NPVARIANT_TO_DOUBLE(npvValue));
            _NPN_GetProperty(0, object, _NPN_GetIntIdentifier(3), &npvValue);
            object = NPVARIANT_IS_OBJECT(value) ? NPVARIANT_TO_OBJECT(npvValue) : NULL;
            for (Int32 i = 0; i < 8; i++) {
                _NPN_GetProperty(0, object, _NPN_GetIntIdentifier(i), &npvValue);
                iid.mData4[i] = (Int8)(NPVARIANT_TO_DOUBLE(npvValue));
            }
            _NPN_GetProperty(0, objectGuid, _NPN_GetStringIdentifier("mUunm"), &npvValue);
            NPString str = NPVARIANT_TO_STRING(npvValue);
            strcpy(result->value.mCid.mUunm, str.UTF8Characters);
            _NPN_GetProperty(0, objectGuid, _NPN_GetStringIdentifier("mCarcode"), &npvValue);
            result->value.mCid.mCarcode = (Int32)(NPVARIANT_TO_DOUBLE(npvValue));

            break;
        }
        case CarDataType_ECode:
        {
            if (type == NPVariantType_Int32) {
                result->value.mECodeValue = (Elastos::UInt32)(NPVARIANT_TO_INT32(value));
            }
            else if (type == NPVariantType_Double) {
                result->value.mECodeValue = (Elastos::UInt32)(NPVARIANT_TO_DOUBLE(value));
            }
            break;
        }
        case CarDataType_Enum:
            if (type == NPVariantType_Int32) {
                result->value.mEnumValue = (Elastos::Int32)(NPVARIANT_TO_INT32(value));
            }
            else if (type == NPVariantType_Double) {
                result->value.mEnumValue = (Elastos::Int32)(NPVARIANT_TO_DOUBLE(value));
            }
            break;
        case CarDataType_ArrayOf:
//#if PLATFORM(ANDROID)
        // If we're converting to an array, see if the NPVariant has a length
        // property. If so, create a JNI array of the relevant length and to get
        // the elements of the NPVariant. When we interpret the JavaValue later,
        // we know that the array is represented as a JNI array.
        //
        // FIXME: This is a hack. We should not be using JNI here. We should
        // represent the JavaValue without JNI.
        {
            NPObject* object = NPVARIANT_IS_OBJECT(value) ? NPVARIANT_TO_OBJECT(value) : 0;

            if (!object) {
                ALOGD("convertNPVariantToCarValue_In CarDataType_ArrayOf 1 error: no input object parameter");
                return;
            }

            NPVariant npvLength;
            bool success = _NPN_GetProperty(0, object, _NPN_GetStringIdentifier("length"), &npvLength);
            if (!success) {
                // No length property so we don't know how many elements to put into the array.
                // Treat this as an error.
                // JSC sends null for an array that is not an array of strings or basic types,
                // do this also in the unknown length case.
                break;
            }

            // Convert to null if the length property is not a number.
            if (!NPVARIANT_IS_INT32(npvLength) && !NPVARIANT_IS_DOUBLE(npvLength)) {
                break;
            }

            // Convert to null if the length property is out of bounds.
            Int32 length = NPVARIANT_IS_INT32(npvLength) ? NPVARIANT_TO_INT32(npvLength) : (Int32)NPVARIANT_TO_DOUBLE(npvLength);
            if (length < 0 || length > INT32_MAX) {
                break;
            }

            CarDataType elementType = 0;
            AutoPtr<IDataTypeInfo> aElementDataTypeInfo;
            ICarArrayInfo::Probe(dataTypeInfo)->GetElementTypeInfo((IDataTypeInfo**)&aElementDataTypeInfo);
            aElementDataTypeInfo->GetDataType(&elementType);

            CarQuintet* carArray = NULL;
            switch (elementType) {
                case CarDataType_Int16:
                {
                    carArray = ArrayOf<Int16>::Alloc(length);

                    // Now iterate over each element and add to the array.
                    for (Int32 i = 0; i < length; i++) {
                        NPVariant npvValue;
                        _NPN_GetProperty(0, object, _NPN_GetIntIdentifier(i), &npvValue);
                        Int16 iVal = (Int16)NPVARIANT_TO_DOUBLE(npvValue);
                        reinterpret_cast< ArrayOf<Int16>* >(carArray)->Set(i, iVal);
                    }

                    result->value.mCarQuintet = carArray;
                    _CarQuintet_AddRef(result->value.mCarQuintet);

                    break;
                }
                case CarDataType_Int32:
                {
                    carArray = ArrayOf<Int32>::Alloc(length);

                    // Now iterate over each element and add to the array.
                    for (Int32 i = 0; i < length; i++) {
                        NPVariant npvValue;
                        _NPN_GetProperty(0, object, _NPN_GetIntIdentifier(i), &npvValue);
                        Int32 iVal = (Int32)NPVARIANT_TO_DOUBLE(npvValue);
                        reinterpret_cast< ArrayOf<Int32>* >(carArray)->Set(i, iVal);
                    }

                    result->value.mCarQuintet = carArray;
                    _CarQuintet_AddRef(result->value.mCarQuintet);
                    break;
                }
                case CarDataType_Int64:
                {
                    carArray = ArrayOf<Int64>::Alloc(length);

                    // Now iterate over each element and add to the array.
                    for (Int32 i = 0; i < length; i++) {
                        NPVariant npvValue;
                        _NPN_GetProperty(0, object, _NPN_GetIntIdentifier(i), &npvValue);
                        Int64 iVal = (Int64)NPVARIANT_TO_DOUBLE(npvValue);
                        reinterpret_cast< ArrayOf<Int64>* >(carArray)->Set(i, iVal);
                    }

                    result->value.mCarQuintet = carArray;
                    _CarQuintet_AddRef(result->value.mCarQuintet);
                    break;
                }
                case CarDataType_Byte:
                {
                    carArray = ArrayOf<Byte>::Alloc(length);

                    // Now iterate over each element and add to the array.
                    for (Int32 i = 0; i < length; i++) {
                        NPVariant npvValue;
                        _NPN_GetProperty(0, object, _NPN_GetIntIdentifier(i), &npvValue);
                        Byte iVal = (Byte)NPVARIANT_TO_DOUBLE(npvValue);
                        reinterpret_cast< ArrayOf<Byte>* >(carArray)->Set(i, iVal);
                    }

                    result->value.mCarQuintet = carArray;
                    _CarQuintet_AddRef(result->value.mCarQuintet);
                    break;
                }
                case CarDataType_Char32:
                {
                    carArray = ArrayOf<Char32>::Alloc(length);

                    // Now iterate over each element and add to the array.
                    for (Int32 i = 0; i < length; i++) {
                        NPVariant npvValue;
                        _NPN_GetProperty(0, object, _NPN_GetIntIdentifier(i), &npvValue);
                        NPString src = NPVARIANT_TO_STRING(npvValue);
                        Char32 iVal = Elastos::String(src.UTF8Characters).GetChar(0);
                        reinterpret_cast< ArrayOf<Char32>* >(carArray)->Set(i, iVal);
                    }

                    result->value.mCarQuintet = carArray;
                    _CarQuintet_AddRef(result->value.mCarQuintet);
                    break;
                }
                case CarDataType_Float:
                {
                    carArray = ArrayOf<Float>::Alloc(length);

                    // Now iterate over each element and add to the array.
                    for (Int32 i = 0; i < length; i++) {
                        NPVariant npvValue;
                        _NPN_GetProperty(0, object, _NPN_GetIntIdentifier(i), &npvValue);
                        Float iVal = (Float)NPVARIANT_TO_DOUBLE(npvValue);
                        reinterpret_cast< ArrayOf<Float>* >(carArray)->Set(i, iVal);
                    }

                    result->value.mCarQuintet = carArray;
                    _CarQuintet_AddRef(result->value.mCarQuintet);
                    break;
                }
                case CarDataType_Double:
                {
                    carArray = ArrayOf<Double>::Alloc(length);

                    // Now iterate over each element and add to the array.
                    for (Int32 i = 0; i < length; i++) {
                        NPVariant npvValue;
                        _NPN_GetProperty(0, object, _NPN_GetIntIdentifier(i), &npvValue);
                        Double iVal = (Double)NPVARIANT_TO_DOUBLE(npvValue);
                        reinterpret_cast< ArrayOf<Double>* >(carArray)->Set(i, iVal);
                    }

                    result->value.mCarQuintet = carArray;
                    _CarQuintet_AddRef(result->value.mCarQuintet);
                    break;
                }
                case CarDataType_String:
                {
                    carArray = ArrayOf<Elastos::String>::Alloc(length);

                    // Now iterate over each element and add to the array.
                    for (Int32 i = 0; i < length; i++) {
                        NPVariant npvValue;
                        _NPN_GetProperty(0, object, _NPN_GetIntIdentifier(i), &npvValue);
                        NPString src = NPVARIANT_TO_STRING(npvValue);
                        reinterpret_cast< ArrayOf<Elastos::String>* >(carArray)->Set(i, Elastos::String(src.UTF8Characters));
                    }

                    result->value.mCarQuintet = carArray;
                    _CarQuintet_AddRef(result->value.mCarQuintet);
                    break;
                }
                case CarDataType_Boolean:
                {
                    carArray = ArrayOf<Boolean>::Alloc(length);

                    // Now iterate over each element and add to the array.
                    for (Int32 i = 0; i < length; i++) {
                        NPVariant npvValue;
                        _NPN_GetProperty(0, object, _NPN_GetIntIdentifier(i), &npvValue);
                        Boolean iVal = (Boolean)NPVARIANT_TO_BOOLEAN(npvValue);
                        reinterpret_cast< ArrayOf<Boolean>* >(carArray)->Set(i, iVal);
                    }

                    result->value.mCarQuintet = carArray;
                    _CarQuintet_AddRef(result->value.mCarQuintet);
                    break;
                }
                case CarDataType_EMuid:
                    break;
                case CarDataType_EGuid:
                    break;
                case CarDataType_ECode:
                {
                    carArray = ArrayOf<ECode>::Alloc(length);

                    // Now iterate over each element and add to the array.
                    for (Int32 i = 0; i < length; i++) {
                        NPVariant npvValue;
                        _NPN_GetProperty(0, object, _NPN_GetIntIdentifier(i), &npvValue);
                        ECode iVal = (UInt32)NPVARIANT_TO_DOUBLE(npvValue);
                        reinterpret_cast< ArrayOf<ECode>* >(carArray)->Set(i, iVal);
                    }

                    result->value.mCarQuintet = carArray;
                    _CarQuintet_AddRef(result->value.mCarQuintet);
                    break;
                }
                case CarDataType_Enum:
                {
                    carArray = ArrayOf<Int32>::Alloc(length);

                    // Now iterate over each element and add to the array.
                    for (Int32 i = 0; i < length; i++) {
                        NPVariant npvValue;
                        _NPN_GetProperty(0, object, _NPN_GetIntIdentifier(i), &npvValue);
                        Int32 iVal = (Int32)NPVARIANT_TO_DOUBLE(npvValue);
                        reinterpret_cast< ArrayOf<Int32>* >(carArray)->Set(i, iVal);
                    }

                    result->value.mCarQuintet = carArray;
                    _CarQuintet_AddRef(result->value.mCarQuintet);
                    break;
                }
                case CarDataType_Interface:
                    break;
                default:
                    break;
            }

            break;
        }
//#endif // PLATFORM(ANDROID)
        case CarDataType_LocalPtr:  //deprecated
        {
            ALOGD("convertNPVariantToCarValue_In=====CarDataType_LocalPtr====");

            CarDataType elementType = 0;
            AutoPtr<IDataTypeInfo> aElementDataTypeInfo;
            (*(ILocalPtrInfo **)&dataTypeInfo)->GetTargetTypeInfo((IDataTypeInfo**)&aElementDataTypeInfo);
            aElementDataTypeInfo->GetDataType(&elementType);

            switch (elementType) {
                case CarDataType_Int16:
                {
                    CarValue* aCarValue = new CarValue();
                    aCarValue->mTypeInfo = aElementDataTypeInfo;
                    aCarValue->value.mInt16Value = (Elastos::Int16)(NPVARIANT_TO_DOUBLE(value));
                    result->value.mLocalPtr = aCarValue;
                    break;
                }
                case CarDataType_Int32:
                    ALOGD("convertNPVariantToCarValue_In=====CarDataType_LocalPtr<CarDataType_Int32>====");
                /*
                //to be used after, as a test, all the Int32 input now is Arrayof, and should translate to Array according to the element type of input js array.
                {
                    //ec = argumentList->SetInputArgumentOfCarArray(i, args[i].pArrayOfInt32Value);
                    CarValue* aCarValue = new CarValue();
                    aCarValue->mInt32Value = (Elastos::Int32)(NPVARIANT_TO_DOUBLE(value));
                    result->mLocalPtr = aCarValue;
                    break;
                }
                */
                case CarDataType_ArrayOf:
                {
                    ALOGD("convertNPVariantToCarValue_In=====CarDataType_LocalPtr<CarDataType_ArrayOf>====");

                    CarDataType localPtrElementDataType = 0;
                    AutoPtr<IDataTypeInfo> aLocalPtrElementDataTypeInfo;
                    AutoPtr<ICarArrayInfo> aCarArrayInfo = ICarArrayInfo::Probe(aElementDataTypeInfo);
                    if (aCarArrayInfo.Get()) {
                        aCarArrayInfo->GetElementTypeInfo((IDataTypeInfo**)&aLocalPtrElementDataTypeInfo);
                        aLocalPtrElementDataTypeInfo->GetDataType(&localPtrElementDataType);
                    }
                    else {
                        //TODO:just for test, to be finished
                        ALOGD("convertNPVariantToCarValue_In=====CarDataType_LocalPtr<CarDataType_ArrayOf>====element type not found, default is INT32 for LocalPtr test flag!");

                        localPtrElementDataType = CarDataType_Int32;
                    }

                    ALOGD("convertNPVariantToCarValue_In=====CarDataType_LocalPtr<CarDataType_ArrayOf>====type:%d", localPtrElementDataType);

                    CarValue* aCarValue = new CarValue();
                    CarQuintet* carArray = NULL;

                    NPObject* object = NPVARIANT_IS_OBJECT(value) ? NPVARIANT_TO_OBJECT(value) : 0;
                    NPVariant npvLength;
                    bool success = _NPN_GetProperty(0, object, _NPN_GetStringIdentifier("length"), &npvLength);
                    if (!success) {
                        ALOGD("convertNPVariantToCarValue_In=====CarDataType_LocalPtr<CarDataType_ArrayOf>====type:%d====error: can not get length!", localPtrElementDataType);
                        // No length property so we don't know how many elements to put into the array.
                        // Treat this as an error.
                        // JSC sends null for an array that is not an array of strings or basic types,
                        // do this also in the unknown length case.
                        break;
                    }

                    // Convert to null if the length property is not a number.
                    if (!NPVARIANT_IS_INT32(npvLength) && !NPVARIANT_IS_DOUBLE(npvLength)) {
                        ALOGD("convertNPVariantToCarValue_In=====CarDataType_LocalPtr<CarDataType_ArrayOf>====type:%d====error: type of length!", localPtrElementDataType);
                        break;
                    }

                    // Convert to null if the length property is out of bounds.
                    double doubleLength = NPVARIANT_IS_INT32(npvLength) ? NPVARIANT_TO_INT32(npvLength) : NPVARIANT_TO_DOUBLE(npvLength);
                    if (doubleLength < 0.0 || doubleLength > INT32_MAX)
                        break;

                    Int32 length = static_cast<Int32>(doubleLength);

                    ALOGD("convertNPVariantToCarValue_In=====CarDataType_LocalPtr<CarDataType_ArrayOf>====length:%d", length);

                    if (localPtrElementDataType <= 0) {
                        ALOGD("convertNPVariantToCarValue_In=====CarDataType_LocalPtr<CarDataType_ArrayOf>====Error: LocalPtrElementDataType");
                        return;
                    }

                    switch (localPtrElementDataType) {
                        case CarDataType_Int16:
                            carArray = ArrayOf<Elastos::Int16>::Alloc(length);
                            break;
                        case CarDataType_Int32:
                            carArray = ArrayOf<Elastos::Int32>::Alloc(length);
                            break;
                        case CarDataType_Int64:
                            carArray = ArrayOf<Elastos::Int64>::Alloc(length);
                            break;
                        case CarDataType_Byte:
                            carArray = ArrayOf<Elastos::Byte>::Alloc(length);
                            break;
                        case CarDataType_Float:
                            carArray = ArrayOf<Elastos::Float>::Alloc(length);
                            break;
                        case CarDataType_Double:
                            carArray = ArrayOf<Elastos::Double>::Alloc(length);
                            break;
                        case CarDataType_Char32:
                            carArray = ArrayOf<Elastos::Char32>::Alloc(length);
                            break;
                        case CarDataType_String:
                            carArray = ArrayOf<Elastos::String>::Alloc(length);
                            break;
                        case CarDataType_Boolean:
                            carArray = ArrayOf<Elastos::Boolean>::Alloc(length);
                            break;
                        case CarDataType_EMuid:
                            carArray = ArrayOf<Elastos::EMuid>::Alloc(length);
                            break;
                        case CarDataType_EGuid:
                            carArray = ArrayOf<Elastos::EGuid>::Alloc(length);
                            break;
                        case CarDataType_ECode:
                            carArray = ArrayOf<Elastos::ECode>::Alloc(length);
                            break;
                        case CarDataType_LocalPtr:
                            carArray = ArrayOf<LocalPtr>::Alloc(length);
                            break;
                        case CarDataType_LocalType:
                            //TODO
                            //carArray = ArrayOf<LocalType>::Alloc(length);
                            break;
                        case CarDataType_Enum:
                            //TODO
                            //carArray = ArrayOf<Elastos::Enum>::Alloc(length);
                            break;
                        case CarDataType_ArrayOf:
                            //TODO
                            //carArray = ArrayOf<Elastos::ArrayOf>::Alloc(length);
                            break;
                        case CarDataType_CppVector:
                            //TODO
                            //carArray = ArrayOf<Elastos::CppVector>::Alloc(length);
                            break;
                        case CarDataType_Struct:
                            //TODO
                            //carArray = ArrayOf<Struct>::Alloc(length);
                            break;
                        case CarDataType_Interface:
                            carArray = ArrayOf< AutoPtr<IInterface> >::Alloc(length);
                            break;
                        default:
                            ALOGD("convertNPVariantToCarValue_In=====CarDataType_LocalPtr<CarDataType_ArrayOf>====error: other type:%d", localPtrElementDataType);
                            break;
                    }

                    for (Int32 i = 0; i < length; i++) {
                        NPVariant npvValue;
                        bool success = _NPN_GetProperty(0, object, _NPN_GetIntIdentifier(i), &npvValue);
                        if (!success) {
                            ALOGD("convertNPVariantToCarValue_In=====CarDataType_LocalPtr<CarDataType_ArrayOf>====%d/%d====error: can not get element value", i, length);
                            continue;
                        }

                        Int32 iVal = 0;

                        switch (npvValue.type) {
                            case NPVariantType_Double:  //4
                            {
                                //iVal = NPVARIANT_TO_INT32(npvValue);
                                iVal = NPVARIANT_TO_DOUBLE(npvValue);
                                (*(ArrayOf<Elastos::Int32>**)&carArray)->Set(i,iVal);
                                break;
                            }
                            case NPVariantType_String:  //5
                            {
                                NPString src = NPVARIANT_TO_STRING(npvValue);
                                Elastos::String* _stringValue;
                                _stringValue = new Elastos::String(src.UTF8Characters);

                                Int32 length = _stringValue->GetLength();
                                if( !length ) {
                                    *_stringValue = NULL;
                                }
                                (*(ArrayOf<Elastos::String>**)&carArray)->Set(i, *_stringValue);
                                break;
                            }
                            case NPVariantType_Void:    //0
                                //TODO:undefined
                            case NPVariantType_Null:    //1
                                //TODO:null
                            case NPVariantType_Bool:    //2
                                //TODO:boolean
                            case NPVariantType_Int32:   //3
                                //NOT BE USED IN V8
                            case NPVariantType_Object:  //6
                                //TODO:object
                            default:
                                //error
                                ALOGD("convertNPVariantToCarValue_In=====TODO: NPVariantType:%d", npvValue.type);
                                break;
                        }   //switch

                    }   //for

                    _CarQuintet_AddRef(carArray);

                    aCarValue->mTypeInfo = aElementDataTypeInfo;
                    aCarValue->value.mCarQuintet = carArray;
                    result->value.mLocalPtr = aCarValue;

                    break;
                }
                case CarDataType_String:
                {
                    //ec = argumentList->SetInputArgumentOfCarArray(i, args[i].pArrayOfInt32Value);
                    break;
                }
                case CarDataType_Interface:
                {
                    //now be used when js transfer a js object as a result to pp value by native callback, just transfer the value to native code, no more need to do.
                    CarValue* aCarValue = new CarValue();
                    aCarValue->mTypeInfo = aElementDataTypeInfo;
                    aCarValue->value.mInt32Value = (Elastos::Int32)(NPVARIANT_TO_DOUBLE(value));
                    result->value.mLocalPtr = aCarValue;
                    break;
                }
                default:
                {
                    ALOGD("=================convertNPVariantToCarValue_In CarDataType_LocalPtr=========other==============ElementType:%d", elementType);
                    break;
                }
            }

            break;
        }
        case CarDataType_Interface:
        {
            switch (type) {
                case NPVariantType_Void:    //js undefined
                case NPVariantType_Null:    //js null
                    result->mObjectWrapper = NULL;
                    ALOGD("========convertNPVariantToCarValue_In CarDataType_Interface===========TODO:null/undefined js type to car interface");
                    break;
                case NPVariantType_Bool:
                    //Illegal parameter
                    result->mObjectWrapper = NULL;
                    ALOGD("========convertNPVariantToCarValue_In CarDataType_Interface===========TODO:js bool to car interface");
                    break;
                case NPVariantType_Int32:   //not used in v8
                case NPVariantType_Double:
                    //Illegal parameter
                    result->mObjectWrapper = NULL;
                    ALOGD("========convertNPVariantToCarValue_In CarDataType_Interface===========TODO:js number to car interface");
                    break;
                case NPVariantType_String:
                {
                    //Illegal parameter
                    result->mObjectWrapper = NULL;
                    ALOGD("========convertNPVariantToCarValue_In CarDataType_Interface===========TODO:js string to car interface");
                    break;
                }
                case NPVariantType_Object:
                {
                    NPObject* obj = NPVARIANT_TO_OBJECT(value);
                    if (obj->_class == WebCore::npScriptObjectClass) {
                        ALOGD("========convertNPVariantToCarValue_In CarDataType_Interface===========js object to car interface");
                        //js object, normaly, should be callback proxy
                        CarCallbackObject* cb = CarCallbackObject::S_CreateObject(IInterfaceInfo::Probe(result->mObjectWrapper->getDataTypeInfo()), obj);
                        result->mObjectWrapper->setInstance((IObject*)cb);
                    }
                    else {
                        ALOGD("========convertNPVariantToCarValue_In CarDataType_Interface===========car wrapper");
                        //car object wrapper
                        CarNPObject* carObj = reinterpret_cast<CarNPObject*>(obj);
                        CobjectWrapper* objectWrapper = carObj->mInstance->getInstance();
                        result->mObjectWrapper = objectWrapper;
                    }
                    break;
                }
                default :
                    ALOGD("========convertNPVariantToCarValue_In CarDataType_Interface===========TODO:the value is not a car wrapper or js type, type:%d", type);
                    break;
            }
            break;
        }
        default :
            break;
    }
}   //void convertNPVariantToCarValue_In

void convertCarValuesToNPVariant_bak(const CarMethod* method, CarValue* values, ArrayOf<Int32>* outParamsPosBuf, NPVariant* result)
{
    VOID_TO_NPVARIANT(*result);

    Int32 length = outParamsPosBuf->GetLength();
    Int32 pos = 0;

    if (length == 0) {
        VOID_TO_NPVARIANT(*result);
    }
    else if (length == 1) {
        pos = (*outParamsPosBuf)[0];
        convertCarValueToNPVariant(values[pos], result);
    }
    else {
        v8::Isolate* isolate = v8::Isolate::GetCurrent();

        v8::Local<v8::Object> pV8Object(v8::Object::New(isolate));

        AutoPtr< ArrayOf<NPVariant> > npvalues = ArrayOf<NPVariant>::Alloc(length);
        for (Int32 i = 0; i < length; i++) {
            pos = (*outParamsPosBuf)[i];

            //convert CarValue to NPVariant
            NPVariant* npvalue = &(*npvalues)[i];
            convertCarValueToNPVariant(values[pos], npvalue);

            //get output parameter name
            const char* paramName = method->parameterAt(pos).utf8().data();
            v8::Local<v8::String> v8Name(v8::String::NewFromUtf8(isolate,paramName));

            //convert NPVarient to V8 viables
            if (npvalue->type == NPVariantType_Null) {
                //TODO:null;
            }
            else if (npvalue->type == NPVariantType_String) {
                NPString src = NPVARIANT_TO_STRING(*npvalue);
                pV8Object->Set(v8Name, v8::String::NewFromUtf8(isolate,src.UTF8Characters));
            }
            else if (npvalue->type == NPVariantType_Int32) {
                pV8Object->Set(v8Name, v8::Int32::New(isolate,NPVARIANT_TO_INT32(*npvalue)));
            }
            else if (npvalue->type == NPVariantType_Bool) {
                pV8Object->Set(v8Name, v8::Boolean::New(isolate,NPVARIANT_TO_BOOLEAN(*npvalue)));
            }
            else if (npvalue->type == NPVariantType_Double) {
                pV8Object->Set(v8Name, v8::Number::New(isolate,NPVARIANT_TO_DOUBLE(*npvalue)));
            }
            else if (!NPVARIANT_IS_NULL(*npvalue)) {
                //TODO:undefined;
            }
        }

        WebCore::V8NPObject* pV8NPObject_ret = reinterpret_cast<WebCore::V8NPObject*>(_NPN_CreateObject(NULL, WebCore::npScriptObjectClass));
        new (&pV8NPObject_ret->v8Object) v8::Persistent<v8::Object>();

        pV8NPObject_ret->v8Object.Reset(isolate,pV8Object);
        pV8NPObject_ret->rootObject = getRootObject();
        result->type = NPVariantType_Object;
        result->value.objectValue = _NPN_RetainObject((NPObject*)pV8NPObject_ret);
    }
}   //void convertCarValuesToNPVariant_bak

void convertCarValuesToNPVariants(CarValue* values, Int32 count, NPVariant** result)
{
    *result = new NPVariant[count];
    for (Int32 i = 0; i < count; i++) {
        convertCarValueToNPVariant(values[i], &(*result)[i]);
    }
}   //void convertCarValuesToNPVariants

//void convertCarValuesToNPVariant(const CarMethod* method, CarValue* values, ArrayOf<Int32>* outParamsPosBuf, Boolean alwaysObject, NPVariant* result)
void convertCarValuesToNPVariant(const CarMethod* method, CarValue* values, ArrayOf<Int32>* outParamsPosBuf, NPVariant* result)
{
    Boolean alwaysObject = false;   //to be finished

    VOID_TO_NPVARIANT(*result);

    Int32 length = outParamsPosBuf->GetLength();
    Int32 pos = 0;

    if (length == 0) {
        VOID_TO_NPVARIANT(*result);
    }
    else if (!alwaysObject && length == 1) {
        pos = (*outParamsPosBuf)[0];
        _convertCarValueToNPVariant_In(values[pos], result);
    }
    else {
        v8::Isolate* isolate = v8::Isolate::GetCurrent();
        v8::Local<v8::Object> pV8Object(v8::Object::New(isolate));
        AutoPtr< ArrayOf<NPVariant> > npvalues = ArrayOf<NPVariant>::Alloc(length);

        for (Int32 i = 0; i < length; i++) {
            pos = (*outParamsPosBuf)[i];

            //convert CarValue to NPVariant
            NPVariant* npvalue = &(*npvalues)[i];
            _convertCarValueToNPVariant_In(values[pos], npvalue);

            //get output parameter name
            const char* paramName = method->parameterAt(pos).utf8().data();
            v8::Local<v8::String> v8Name(v8::String::NewFromUtf8(isolate,paramName));

            //convert NPVarient to V8 viables
            if (npvalue->type == NPVariantType_Null) {
                //TODO:null;
            }
            else if (npvalue->type == NPVariantType_String) {
                NPString src = NPVARIANT_TO_STRING(*npvalue);
                pV8Object->Set(v8Name, v8::String::NewFromUtf8(isolate,src.UTF8Characters));
            }
            else if (npvalue->type == NPVariantType_Int32) {
                pV8Object->Set(v8Name, v8::Int32::New(isolate,NPVARIANT_TO_INT32(*npvalue)));
            }
            else if (npvalue->type == NPVariantType_Bool) {
                pV8Object->Set(v8Name, v8::Boolean::New(isolate,NPVARIANT_TO_BOOLEAN(*npvalue)));
            }
            else if (npvalue->type == NPVariantType_Double) {
                pV8Object->Set(v8Name, v8::Number::New(isolate,NPVARIANT_TO_DOUBLE(*npvalue)));
            }
            else if (!NPVARIANT_IS_NULL(*npvalue)) {
                //TODO:undefined;
            }
        }

        WebCore::V8NPObject* pV8NPObject_ret = reinterpret_cast<WebCore::V8NPObject*>(_NPN_CreateObject(NULL, WebCore::npScriptObjectClass));
        new (&pV8NPObject_ret->v8Object) v8::Persistent<v8::Object>();

        pV8NPObject_ret->v8Object.Reset(isolate,pV8Object);
        pV8NPObject_ret->rootObject = getRootObject();
        result->type = NPVariantType_Object;
        result->value.objectValue = _NPN_RetainObject((NPObject*)pV8NPObject_ret);
    }
}   //void convertCarValuesToNPVariant

void _convertCarValueToNPVariant_In(CarValue& value, NPVariant* result)
{
    AutoPtr<IDataTypeInfo> dataTypeInfo;

    CarDataType carDataType = 0;
    //carDataType = value.mType;

    if (value.mObjectWrapper.Get()) {
        dataTypeInfo = value.mObjectWrapper->getDataTypeInfo();
        if (dataTypeInfo.Get()) {
            dataTypeInfo->GetDataType(&carDataType);
        }
        else {
            //TODO:Shoud not reach here: all CObjectWrapper must have typeInfo
            carDataType = CarDataType_Interface;
            ALOGD("_convertCarValueToNPVariant_In========dataTypeInfo not exist!");
        }
    }
    else {
        //TODO:Shoud not reach here: all CObjectWrapper must exist
        carDataType = CarDataType_Interface;
        ALOGD("_convertCarValueToNPVariant_In========mObjectWrapper not exist!");
    }

    //const char* tmpType = ClassNameFromCarDataType((CarDataType)(carDataType));
    //ALOGD("====convertCarValueToNPVariant====begin====carDataTypeClassName:%s", tmpType);

    VOID_TO_NPVARIANT(*result);

    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope scope(isolate);
    v8::Handle<v8::Context> context = isolate->GetCurrentContext();
    v8::Context::Scope contextScope(context);

    switch (carDataType) {
        case CarDataType_Int16:
            result->type = NPVariantType_Int32;
            result->value.intValue = value.value.mInt16Value;
            break;
        case CarDataType_Int32:
            result->type = NPVariantType_Int32;
            result->value.intValue = value.value.mInt32Value;
            break;
        case CarDataType_Int64:
            result->type = NPVariantType_Double;
            result->value.doubleValue = value.value.mInt64Value;
            break;
        case CarDataType_Byte:
            result->type = NPVariantType_Int32;
            result->value.intValue = value.value.mByteValue;
            break;
        case CarDataType_Char32:
        {
            AutoPtr<ArrayOf<Char32> > charArray = ArrayOf<Char32>::Alloc(1);
            charArray->Set(0, (Char32)value.value.mCharValue);
            const char* utf8String = strdup(Elastos::String(*charArray).string());
            STRINGZ_TO_NPVARIANT(utf8String, *result);
            break;
        }
        case CarDataType_Float:
        {
            Int32 ttt=123456;

            ALOGD("_convertCarValueToNPVariant_In========CarDataType_Float====value:%d====%lf====%x====%x", value.value.mFloatValue, value.value.mFloatValue, value.value.mFloatValue, ttt);
            result->type = NPVariantType_Double;
            result->value.doubleValue = value.value.mFloatValue;
            ALOGD("_convertCarValueToNPVariant_In========CarDataType_Float====doubleValue:%d====%lf====%x====%x", result->value.doubleValue, result->value.doubleValue, result->value.doubleValue, ttt);
            break;
        }
        case CarDataType_Double:
        {
            result->type = NPVariantType_Double;
            result->value.doubleValue = value.value.mDoubleValue;
            break;
        }
        case CarDataType_String:
        {
            //ALOGD("convertCarValueToNPVariant========CarDataType_String========");
            // This entire file will likely be removed usptream soon.
            if (value.mStringValue.IsNull()) {
                VOID_TO_NPVARIANT(*result);
                break;
            }
            const char* utf8String = strdup(value.mStringValue.string());
            // The copied string is freed in NPN_ReleaseVariantValue (see npruntime.cpp)
            STRINGZ_TO_NPVARIANT(utf8String, *result);
            break;
        }
        case CarDataType_Boolean:
            result->type = NPVariantType_Bool;
            result->value.boolValue = value.value.mBooleanValue;
            break;
        case CarDataType_EMuid:
        {
            EMuid& iid = value.value.mIid;
            v8::Local<v8::Array> v8Array(v8::Array::New(isolate,4));
            v8Array->Set(0, v8::Number::New(isolate,iid.mData1));
            v8Array->Set(1, v8::Number::New(isolate,iid.mData2));
            v8Array->Set(2, v8::Number::New(isolate,iid.mData3));

            v8::Handle<v8::Array> v8Array1(v8::Array::New(isolate,4));
            for(Int32 i = 0; i < 8; i++) {
                v8Array1->Set(i, v8::Number::New(isolate,iid.mData4[i]));
            }
            v8Array->Set(3, v8Array1);

            WebCore::V8NPObject* v8NPObject = reinterpret_cast<WebCore::V8NPObject*>(_NPN_CreateObject(NULL, WebCore::npScriptObjectClass));
            new (&v8NPObject->v8Object) v8::Persistent<v8::Object>();

            v8NPObject->v8Object.Reset(isolate,v8Array);
            v8NPObject->rootObject = getRootObject();

            result->type = NPVariantType_Object;
            result->value.objectValue = _NPN_RetainObject((NPObject*)v8NPObject);

            break;
        }   //case CarDataType_EMuid
        case CarDataType_EGuid:
        {
            EMuid& iid = value.value.mCid.mClsid;

            v8::Local<v8::Array> v8Array(v8::Array::New(isolate,4));
            v8Array->Set(0, v8::Number::New(isolate,iid.mData1));
            v8Array->Set(1, v8::Number::New(isolate,iid.mData2));
            v8Array->Set(2, v8::Number::New(isolate,iid.mData3));
            v8::Handle<v8::Array> v8Array1(v8::Array::New(isolate,4));
            for(Int32 i = 0; i < 8; i++){
                v8Array1->Set(i, v8::Number::New(isolate,iid.mData4[i]));
            }
            v8Array->Set(3, v8Array1);

            v8::Local<v8::Object> v8Object(v8::Object::New(isolate));
            v8Object->Set(v8::String::NewFromUtf8(isolate,"mClsid"), v8Array);
            v8Object->Set(v8::String::NewFromUtf8(isolate,"mUunm"), v8::String::NewFromUtf8(isolate,value.value.mCid.mUunm));
            v8Object->Set(v8::String::NewFromUtf8(isolate,"mCarcode"), v8::Number::New(isolate,value.value.mCid.mCarcode));

            WebCore::V8NPObject* v8NPObject = reinterpret_cast<WebCore::V8NPObject*>(_NPN_CreateObject(NULL, WebCore::npScriptObjectClass));
            new (&v8NPObject->v8Object) v8::Persistent<v8::Object>();

            v8NPObject->v8Object.Reset(isolate,v8Object);
            v8NPObject->rootObject = getRootObject();
            result->type = NPVariantType_Object;
            result->value.objectValue = _NPN_RetainObject((NPObject*)v8NPObject);

            break;
        }   //case CarDataType_EGuid
        case CarDataType_ECode:
            result->type = NPVariantType_Double;
            result->value.doubleValue = (UInt32)value.value.mECodeValue;
            break;
        case CarDataType_Enum:
            result->type = NPVariantType_Double;
            result->value.doubleValue = value.value.mEnumValue;
            break;
        case CarDataType_ArrayOf:
        {
            CarDataType elementType = 0;
            AutoPtr<IDataTypeInfo> aElementDataTypeInfo;
            ICarArrayInfo::Probe(dataTypeInfo)->GetElementTypeInfo((IDataTypeInfo**)&aElementDataTypeInfo);
            aElementDataTypeInfo->GetDataType(&elementType);

            switch (elementType) {
                case CarDataType_Int16:
                {
                    ArrayOf<Int16>* pArray = reinterpret_cast< ArrayOf<Int16>* >(value.value.mCarQuintet);
                    Int32 length = pArray->GetLength();

                    v8::Local<v8::Array> pV8Array(v8::Array::New(isolate,length));
                    for (Int32 i = 0; i < length; i++) {
                        pV8Array->Set(i, v8::Int32::New(isolate,(*pArray)[i]));
                    }

                    WebCore::V8NPObject* pV8NPObject_ret = reinterpret_cast<WebCore::V8NPObject*>(_NPN_CreateObject(NULL, WebCore::npScriptObjectClass));
                    new (&pV8NPObject_ret->v8Object) v8::Persistent<v8::Object>();

                    pV8NPObject_ret->v8Object.Reset(isolate,pV8Array);
                    pV8NPObject_ret->rootObject = getRootObject();

                    result->type = NPVariantType_Object;
                    result->value.objectValue = _NPN_RetainObject((NPObject*)pV8NPObject_ret);

                    break;
                }   //case CarDataType_Int16
                case CarDataType_Int32:
                {
                    ArrayOf<Int32>* pArray = reinterpret_cast< ArrayOf<Int32>* >(value.value.mCarQuintet);
                    Int32 length = pArray->GetLength();

                    v8::Local<v8::Array> pV8Array(v8::Array::New(isolate,length));
                    for (Int32 i = 0; i < length; i++) {
                        pV8Array->Set(i, v8::Int32::New(isolate,(*pArray)[i]));
                    }

                    WebCore::V8NPObject* pV8NPObject_ret = reinterpret_cast<WebCore::V8NPObject*>(_NPN_CreateObject(NULL, WebCore::npScriptObjectClass));
                    new (&pV8NPObject_ret->v8Object) v8::Persistent<v8::Object>();
                    pV8NPObject_ret->v8Object.Reset(isolate, pV8Array);

                    pV8NPObject_ret->rootObject = getRootObject();

                    result->type = NPVariantType_Object;
                    result->value.objectValue = _NPN_RetainObject((NPObject*)pV8NPObject_ret);

                    break;
                }   //case CarDataType_Int32
                case CarDataType_Int64:
                {
                    ArrayOf<Int64>* pArray = reinterpret_cast< ArrayOf<Int64>* >(value.value.mCarQuintet);
                    Int32 length = pArray->GetLength();

                    v8::Local<v8::Array> pV8Array(v8::Array::New(isolate,length));
                    for (Int32 i = 0; i < length; i++) {
                        pV8Array->Set(i, v8::Number::New(isolate,(*pArray)[i]));
                    }

                    WebCore::V8NPObject* pV8NPObject_ret = reinterpret_cast<WebCore::V8NPObject*>(_NPN_CreateObject(NULL, WebCore::npScriptObjectClass));
                    new (&pV8NPObject_ret->v8Object) v8::Persistent<v8::Object>();
                    pV8NPObject_ret->v8Object.Reset(isolate, pV8Array);

                    pV8NPObject_ret->rootObject = getRootObject();

                    result->type = NPVariantType_Object;
                    result->value.objectValue = _NPN_RetainObject((NPObject*)pV8NPObject_ret);

                    break;
                }   //case CarDataType_Int64
                case CarDataType_Byte:
                {
                    ArrayOf<Byte>* pArray = reinterpret_cast< ArrayOf<Byte>* >(value.value.mCarQuintet);
                    Int32 length = pArray->GetLength();

                    v8::Local<v8::Array> pV8Array(v8::Array::New(isolate,length));
                    for (Int32 i = 0; i < length; i++) {
                        pV8Array->Set(i, v8::Int32::New(isolate,(*pArray)[i]));
                    }

                    WebCore::V8NPObject* pV8NPObject_ret = reinterpret_cast<WebCore::V8NPObject*>(_NPN_CreateObject(NULL, WebCore::npScriptObjectClass));
                    new (&pV8NPObject_ret->v8Object) v8::Persistent<v8::Object>();
                    pV8NPObject_ret->v8Object.Reset(isolate, pV8Array);

                    pV8NPObject_ret->rootObject = getRootObject();

                    result->type = NPVariantType_Object;
                    result->value.objectValue = _NPN_RetainObject((NPObject*)pV8NPObject_ret);

                    break;
                }   //case CarDataType_Byte
                case CarDataType_Char32:
                {
                    ArrayOf<Char32>* pArray = reinterpret_cast< ArrayOf<Char32>* >(value.value.mCarQuintet);
                    Int32 length = pArray->GetLength();

                    v8::Local<v8::Array> pV8Array(v8::Array::New(isolate,length));
                    for (Int32 i = 0; i < length; i++){
                        AutoPtr<ArrayOf<Char32> > charArray = ArrayOf<Char32>::Alloc(1);
                        charArray->Set(0, (Char32)(*pArray)[i]);
                        const char* utf8String = strdup(Elastos::String(*charArray).string());
                        pV8Array->Set(i, v8::String::NewFromUtf8(isolate,utf8String));
                    }

                    WebCore::V8NPObject* pV8NPObject_ret = reinterpret_cast<WebCore::V8NPObject*>(_NPN_CreateObject(NULL, WebCore::npScriptObjectClass));
                    new (&pV8NPObject_ret->v8Object) v8::Persistent<v8::Object>();
                    pV8NPObject_ret->v8Object.Reset(isolate, pV8Array);

                    pV8NPObject_ret->rootObject = getRootObject();

                    result->type = NPVariantType_Object;
                    result->value.objectValue = _NPN_RetainObject((NPObject*)pV8NPObject_ret);

                    break;
                }   //case CarDataType_Char32
                case CarDataType_Float:
                {
                    ArrayOf<Float>* pArray = reinterpret_cast< ArrayOf<Float>* >(value.value.mCarQuintet);
                    Int32 length = pArray->GetLength();

                    v8::Local<v8::Array> pV8Array(v8::Array::New(isolate,length));
                    for (Int32 i = 0; i < length; i++) {
                        pV8Array->Set(i, v8::Number::New(isolate,(*pArray)[i]));
                    }

                    WebCore::V8NPObject* pV8NPObject_ret = reinterpret_cast<WebCore::V8NPObject*>(_NPN_CreateObject(NULL, WebCore::npScriptObjectClass));
                    new (&pV8NPObject_ret->v8Object) v8::Persistent<v8::Object>();
                    pV8NPObject_ret->v8Object.Reset(isolate, pV8Array);

                    pV8NPObject_ret->rootObject = getRootObject();

                    result->type = NPVariantType_Object;
                    result->value.objectValue = _NPN_RetainObject((NPObject*)pV8NPObject_ret);

                    break;
                }   //case CarDataType_Float
                case CarDataType_Double:
                {
                    ArrayOf<Double>* pArray = reinterpret_cast< ArrayOf<Double>* >(value.value.mCarQuintet);
                    Int32 length = pArray->GetLength();

                    v8::Local<v8::Array> pV8Array(v8::Array::New(isolate,length));
                    for (Int32 i = 0; i < length; i++) {
                        pV8Array->Set(i, v8::Number::New(isolate,(*pArray)[i]));
                    }

                    WebCore::V8NPObject* pV8NPObject_ret = reinterpret_cast<WebCore::V8NPObject*>(_NPN_CreateObject(NULL, WebCore::npScriptObjectClass));
                    new (&pV8NPObject_ret->v8Object) v8::Persistent<v8::Object>();
                    pV8NPObject_ret->v8Object.Reset(isolate, pV8Array);

                    pV8NPObject_ret->rootObject = getRootObject();

                    result->type = NPVariantType_Object;
                    result->value.objectValue = _NPN_RetainObject((NPObject*)pV8NPObject_ret);

                    break;
                }   //case CarDataType_Double
                case CarDataType_String:
                {
                    ArrayOf<Elastos::String>* pArray = reinterpret_cast< ArrayOf<Elastos::String>* >(value.value.mCarQuintet);
                    Int32 length = pArray->GetLength();

                    v8::Local<v8::Array> pV8Array(v8::Array::New(isolate,length));
                    for (Int32 i = 0; i < length; i++) {
                        const char* utf8String = strdup(((*pArray)[i]).string());
                        pV8Array->Set(i, v8::String::NewFromUtf8(isolate,utf8String));
                    }

                    WebCore::V8NPObject* pV8NPObject_ret = reinterpret_cast<WebCore::V8NPObject*>(_NPN_CreateObject(NULL, WebCore::npScriptObjectClass));
                    new (&pV8NPObject_ret->v8Object) v8::Persistent<v8::Object>();
                    pV8NPObject_ret->v8Object.Reset(isolate, pV8Array);

                    pV8NPObject_ret->rootObject = getRootObject();

                    result->type = NPVariantType_Object;
                    result->value.objectValue = _NPN_RetainObject((NPObject*)pV8NPObject_ret);

                    break;
                }   //CarDataType_String
                case CarDataType_Boolean:
                {
                    ArrayOf<Boolean>* pArray = reinterpret_cast< ArrayOf<Boolean>* >(value.value.mCarQuintet);
                    Int32 length = pArray->GetLength();

                    v8::Local<v8::Array> pV8Array(v8::Array::New(isolate,length));
                    for (Int32 i = 0; i < length; i++) {
                        pV8Array->Set(i, v8::Boolean::New(isolate,(*pArray)[i]));
                    }

                    WebCore::V8NPObject* pV8NPObject_ret = reinterpret_cast<WebCore::V8NPObject*>(_NPN_CreateObject(NULL, WebCore::npScriptObjectClass));
                    new (&pV8NPObject_ret->v8Object) v8::Persistent<v8::Object>();
                    pV8NPObject_ret->v8Object.Reset(isolate, pV8Array);

                    pV8NPObject_ret->rootObject = getRootObject();

                    result->type = NPVariantType_Object;
                    result->value.objectValue = _NPN_RetainObject((NPObject*)pV8NPObject_ret);

                    break;
                }   //case CarDataType_Boolean
/*----TODO------------------------------------------------------------
                case CarDataType_EMuid:
                    break;
                case CarDataType_EGuid:
                    break;
----------------------------------------------------------------*/
                case CarDataType_ECode:
                {
                    ArrayOf<ECode>* pArray = reinterpret_cast< ArrayOf<ECode>* >(value.value.mCarQuintet);
                    Int32 length = pArray->GetLength();

                    v8::Local<v8::Array> pV8Array(v8::Array::New(isolate,length));
                    for (Int32 i = 0; i < length; i++) {
                        pV8Array->Set(i, v8::Number::New(isolate,(UInt32)(*pArray)[i]));
                    }

                    WebCore::V8NPObject* pV8NPObject_ret = reinterpret_cast<WebCore::V8NPObject*>(_NPN_CreateObject(NULL, WebCore::npScriptObjectClass));
                    new (&pV8NPObject_ret->v8Object) v8::Persistent<v8::Object>();
                    pV8NPObject_ret->v8Object.Reset(isolate, pV8Array);

                    pV8NPObject_ret->rootObject = getRootObject();

                    result->type = NPVariantType_Object;
                    result->value.objectValue = _NPN_RetainObject((NPObject*)pV8NPObject_ret);

                    break;
                }   //CarDataType_ECode
                case CarDataType_Enum:
                {
                    ArrayOf<Int32>* pArray = reinterpret_cast< ArrayOf<Int32>* >(value.value.mCarQuintet);
                    Int32 length = pArray->GetLength();

                    v8::Local<v8::Array> pV8Array(v8::Array::New(isolate,length));
                    for (Int32 i = 0; i < length; i++) {
                        pV8Array->Set(i, v8::Int32::New(isolate,(*pArray)[i]));
                    }

                    WebCore::V8NPObject* pV8NPObject_ret = reinterpret_cast<WebCore::V8NPObject*>(_NPN_CreateObject(NULL, WebCore::npScriptObjectClass));
                    new (&pV8NPObject_ret->v8Object) v8::Persistent<v8::Object>();
                    pV8NPObject_ret->v8Object.Reset(isolate, pV8Array);

                    pV8NPObject_ret->rootObject = getRootObject();

                    result->type = NPVariantType_Object;
                    result->value.objectValue = _NPN_RetainObject((NPObject*)pV8NPObject_ret);

                    break;
                }   //case CarDataType_Enum
                case CarDataType_Interface:
                {
                    ArrayOf<IInterface*>* pArrayOfPInterface = (ArrayOf<IInterface*>*)value.value.mCarQuintet;
                    Int32 length = pArrayOfPInterface->GetLength();

                    ArrayOf<NPVariant>* pNPVariantArray = ArrayOf<NPVariant>::Alloc(length);

                    v8::Local<v8::Array> pV8Array(v8::Array::New(isolate,length));
                    for (Int32 i = 0; i < length; i++) {
                        IInterface* tempClassinfo = (*pArrayOfPInterface)[i];

                        NPVariant* tempNPVariant = &(*pNPVariantArray)[i];

                        (*pNPVariantArray)[i].type = NPVariantType_Object;

                        (*pNPVariantArray)[i].value.objectValue = CarInstanceToNPObject(new CarInstanceV8(new CobjectWrapper(tempClassinfo, aElementDataTypeInfo), true));

                        v8::Handle<v8::Value> tempV8Object = WebCore::convertNPVariantToV8Object(tempNPVariant, NULL, isolate);

                        pV8Array->Set(i, tempV8Object);
                    }

                    WebCore::V8NPObject* pV8NPObject_ret = reinterpret_cast<WebCore::V8NPObject*>(_NPN_CreateObject(NULL, WebCore::npScriptObjectClass));
                    new (&pV8NPObject_ret->v8Object) v8::Persistent<v8::Object>();
                    pV8NPObject_ret->v8Object.Reset(isolate, pV8Array);

                    pV8NPObject_ret->rootObject = getRootObject();

                    result->type = NPVariantType_Object;
                    result->value.objectValue = _NPN_RetainObject((NPObject*)pV8NPObject_ret);

                    break;
                }   //case CarDataType_Interface
                default: {
                    //
                    break;
                }
            }   //switch (elementType)
            break;
        }   //case CarDataType_ArrayOf
        case CarDataType_LocalPtr:  //deprecated
        {
            ArrayOf<Int32>* tempArray2 = reinterpret_cast< ArrayOf<Int32>* >(value.value.mCarQuintet);

            NPObject tempNPObject;
            //TODO:convert ArrayOf into NPObject
            result->type = NPVariantType_Object;
            result->value.objectValue = &tempNPObject;
            break;
        }   //case CarDataType_LocalPtr
        case CarDataType_Interface:
        {
            result->type = NPVariantType_Object;
            value.mObjectWrapper->setInstance(value.value.mObjectValue);
            CarInstanceV8* _instance = new CarInstanceV8(value.mObjectWrapper, true);
            result->value.objectValue = CarInstanceToNPObject(_instance);
            break;
        }   //case CarDataType_Interface
        default :
        {
            break;
        }
    }   //switch (carDataType)
}   //void _convertCarValueToNPVariant_In

void convertCarValueToNPVariant(CarValue& value, NPVariant* result)
{
    CarDataType carDataType = 0;
    carDataType = value.mType;

    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope scope(isolate);
    v8::Handle<v8::Context> context = isolate->GetCurrentContext();
    v8::Context::Scope contextScope(context);

    ParamIOAttribute carIOAttr = value.mIOAttribute;
    switch (carIOAttr) {
    case ParamIOAttribute_In: {
        _convertCarValueToNPVariant_In(value, result);
        break;
    }   //case ParamIOAttribute_In
    case ParamIOAttribute_CalleeAllocOut:
    {
        ALOGD("convertCarValueToNPVariant======TODO====ParamIOAttribute_CalleeAllocOut====");

        v8::Local<v8::Object> v8Object(v8::Object::New(isolate));

        v8::Local<v8::String> v8Name(v8::String::NewFromUtf8(isolate,"data"));
        v8Object->Set(v8Name, v8::String::NewFromUtf8(isolate,"Callback output parameters"));

        WebCore::V8NPObject* v8NPObject = reinterpret_cast<WebCore::V8NPObject*>(_NPN_CreateObject(NULL, WebCore::npScriptObjectClass));
        new (&v8NPObject->v8Object) v8::Persistent<v8::Object>();
        v8NPObject->v8Object.Reset(isolate, v8Object);

        v8NPObject->rootObject = getRootObject();

        result->type = NPVariantType_Object;
        result->value.objectValue = _NPN_RetainObject((NPObject*)v8NPObject);

        break;
    }   //case ParamIOAttribute_CalleeAllocOut
    case ParamIOAttribute_CallerAllocOut:
    {
        ALOGD("convertCarValueToNPVariant======ParamIOAttribute_CallerAllocOut======carDataType:%d", carDataType);

        switch (carDataType) {
            //case meta carDataTypes
            case CarDataType_Int16       : //1,
            case CarDataType_Int32       : //2,
            case CarDataType_Int64       : //3,
            case CarDataType_Byte        : //4,
            case CarDataType_Float       : //5,
            case CarDataType_Double      : //6,
            case CarDataType_Char32      : //7,
            case CarDataType_String      : //8,
            case CarDataType_Boolean     : //9,
            case CarDataType_EMuid       : //10,
            case CarDataType_EGuid       : //11,
            case CarDataType_ECode       : //12,
            //case CarDataType_LocalPtr    : //13,
            case CarDataType_LocalType   : //14,
            case CarDataType_Enum        : //15,
            case CarDataType_ArrayOf     : //16,
            case CarDataType_CppVector   : //17,
            case CarDataType_Struct      : //18,
            case CarDataType_Interface  : //19
            {
                //use v8 from node.js thread in EPK UI main thread
                v8::Local<v8::Object> v8Object(v8::Object::New(isolate));
                v8::Local<v8::String> v8Name(v8::String::NewFromUtf8(isolate,"data"));
                v8Object->Set(v8Name, v8::String::NewFromUtf8(isolate,"Callback output parameters"));

                WebCore::V8NPObject* v8NPObject = reinterpret_cast<WebCore::V8NPObject*>(_NPN_CreateObject(NULL, WebCore::npScriptObjectClass));
                new (&v8NPObject->v8Object) v8::Persistent<v8::Object>();
                v8NPObject->v8Object.Reset(isolate, v8Object);
                v8NPObject->rootObject = getRootObject();

                result->type = NPVariantType_Object;
                result->value.objectValue = _NPN_RetainObject((NPObject*)v8NPObject);

                break;
            }   //case meta carDataTypes
            case CarDataType_LocalPtr: //13,
            {
                ALOGD("convertCarValueToNPVariant======ReadParam======CarDataType_LocalPtr====");
                break;
            }
            default:
            {
                ALOGD("convertCarValueToNPVariant======CallerAllocOut======other======carDataType:%d", carDataType);
                break;
            }
        }   //switch (carDataType)
        break;
    }   //case ParamIOAttribute_CallerAllocOut
    default:
    {
        ALOGD("convertCarValueToNPVariant======TODO====Unknown ParamIOAttribute======");
        break;
    }   //default
    }   //switch (carIOAttr)

    //ALOGD("====convertCarValueToNPVariant====end====");
}   //void convertCarValueToNPVariant

void convertV8ValuesToCarValues(v8::Handle<v8::Value>* v8Args, Int32 count, CarValue** carArgs) {
    for (Int32 i = 0; i < count; i++) {
        convertV8ValueToCarValue(v8Args[i], (*carArgs)[i]);
    }
}

void convertV8ValueToCarValue(v8::Handle<v8::Value>& v8Arg, CarValue& carArg) {

    switch (carArg.mIOAttribute) {
    case ParamIOAttribute_In:
        //TODO:
        break;
    case ParamIOAttribute_CalleeAllocOut:
        //TODO:
    case ParamIOAttribute_CallerAllocOut:
    {
        convertV8ValueToCarValue_CallerAllocOut(v8Arg, carArg);
    }
    }   //switch
}

void convertV8ValueToCarValue_CallerAllocOut(v8::Handle<v8::Value>& v8Arg, CarValue& carArg) {

    v8::Isolate* isolate = v8::Isolate::GetCurrent();

    CarDataType ctype = carArg.mType;
    LocalPtr cptr = carArg.value.mLocalPtr;

    if (!cptr) {
        //Info
        ASSERT(0);
    }

    v8::Handle<v8::Value> outV8Handle = v8Arg;

    v8::Local<v8::Object> outV8Object(v8::Handle<v8::Object>::Cast(outV8Handle));

    WebCore::V8NPObject* v8NPObject = reinterpret_cast<WebCore::V8NPObject*>(_NPN_CreateObject(NULL, WebCore::npScriptObjectClass));
    new (&v8NPObject->v8Object) v8::Persistent<v8::Object>();
    v8NPObject->v8Object.Reset(isolate, outV8Object);
    v8NPObject->rootObject = getRootObject();

    NPVariant npvOutValue;
    _NPN_GetProperty(0, (NPObject*)v8NPObject, _NPN_GetStringIdentifier("data"), &npvOutValue);

    switch (ctype) {
        case CarDataType_Int16:
        {
            *(Int16*)cptr = (Int16)NPVARIANT_TO_DOUBLE(npvOutValue);
            break;
        }
        case CarDataType_Int32:
        {
            *(Int32*)cptr = (Int32)NPVARIANT_TO_DOUBLE(npvOutValue);
            break;
        }
        case CarDataType_Int64:
        {
            *(Int64*)cptr = (Int64)NPVARIANT_TO_DOUBLE(npvOutValue);
            break;
        }
        case CarDataType_Byte:
        {
            *(Byte*)cptr = (Byte)NPVARIANT_TO_DOUBLE(npvOutValue);
            break;
        }
        case CarDataType_Float:
        {
            *(Float*)cptr = (Float)NPVARIANT_TO_DOUBLE(npvOutValue);
            break;
        }
        case CarDataType_Double:
        {
            *(Double*)cptr = (Double)NPVARIANT_TO_DOUBLE(npvOutValue);
            break;
        }
        // case CarDataType_Char8:
        // {
        //     ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do====Char8====CarDataType:%d", ctype);
        //     break;
        // }
        // case CarDataType_Char16:
        // {
        //     ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do====Char16====CarDataType:%d", ctype);
        //     break;
        // }
        case CarDataType_Char32:
        {
            *(Char32*)cptr = (Char32)NPVARIANT_TO_DOUBLE(npvOutValue);
            break;
        }
        // case CarDataType_CString:
        // {
        //     ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do====CString====CarDataType:%d", ctype);
        //     break;
        // }
        case CarDataType_String:
        {
            NPString src = NPVARIANT_TO_STRING(npvOutValue);
            *(Elastos::String*)cptr = Elastos::String(src.UTF8Characters);
            break;
        }
        case CarDataType_Boolean:
        {
            *(Boolean*)cptr = (Boolean)NPVARIANT_TO_BOOLEAN(npvOutValue);
            break;
        }
        case CarDataType_EMuid:
        {
            ALOGD("convertV8ValueToCarValue_CallerAllocOut====get output param value===to do====EMuid====CarDataType:%d", ctype);
            break;
        }
        case CarDataType_EGuid:
        {
            ALOGD("convertV8ValueToCarValue_CallerAllocOut====get output param value===to do====EGuid====CarDataType:%d", ctype);
            break;
        }
        case CarDataType_ECode:
        {
            ALOGD("convertV8ValueToCarValue_CallerAllocOut====get output param value===to do====ECode====CarDataType:%d", ctype);
            break;
        }
        case CarDataType_LocalPtr:
        {
            ALOGD("convertV8ValueToCarValue_CallerAllocOut====get output param value===to do====LocalPtr====CarDataType:%d", ctype);
            break;
        }
        case CarDataType_LocalType:
        {
            ALOGD("convertV8ValueToCarValue_CallerAllocOut====get output param value===to do====LocalType====CarDataType:%d", ctype);
            break;
        }
        case CarDataType_Enum:
        {
            *(Int32*)cptr = (Int32)NPVARIANT_TO_DOUBLE(npvOutValue);
            break;
        }
        // case CarDataType_StringBuf:
        // {
        //     ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do====StringBuf====CarDataType:%d", ctype);
        //     break;
        // }
        case CarDataType_ArrayOf:
        {
            NPObject* object = NPVARIANT_IS_OBJECT(npvOutValue) ? NPVARIANT_TO_OBJECT(npvOutValue) : 0;

            if (!object) {
                ALOGD("convertV8ValueToCarValue_CallerAllocOut====CarDataType_ArrayOf==== 1 error: no return object parameter");
                return;
            }

            NPVariant npvLength;
            bool success = _NPN_GetProperty(0, object, _NPN_GetStringIdentifier("length"), &npvLength);
            if (!success) {
                // No length property so we don't know how many elements to put into the array.
                // Treat this as an error.
                // JSC sends null for an array that is not an array of strings or basic types,
                // do this also in the unknown length case.
                break;
            }

            // Convert to null if the length property is not a number.
            if (!NPVARIANT_IS_INT32(npvLength) && !NPVARIANT_IS_DOUBLE(npvLength)) {
                break;
            }

            // Convert to null if the length property is out of bounds.
            Int32 length = NPVARIANT_IS_INT32(npvLength) ? NPVARIANT_TO_INT32(npvLength) : (Int32)NPVARIANT_TO_DOUBLE(npvLength);
            if (length < 0 || length > INT32_MAX) {
                break;
            }

            CarQuintet* carArray = NULL;

            AutoPtr<IDataTypeInfo> paramTypeInfo = carArg.mTypeInfo;
            AutoPtr<IDataTypeInfo> elementDataTypeInfo;
            (*(ICarArrayInfo **)&paramTypeInfo)->GetElementTypeInfo((IDataTypeInfo**)&elementDataTypeInfo);
            CarDataType elementDataType;
            elementDataTypeInfo->GetDataType(&elementDataType);

            switch(elementDataType) {
                // case CarDataType_Int16:
                case CarDataType_Int32:
                {
                    ALOGD("convertV8ValueToCarValue_CallerAllocOut====output====ArrayOf======Int32====0");

                    carArray = ArrayOf<Int32>::Alloc(length);

                    // Now iterate over each element and add to the array.
                    for (Int32 i = 0; i < length; i++) {
                        NPVariant npvValue;
                        _NPN_GetProperty(0, object, _NPN_GetIntIdentifier(i), &npvValue);
                        Int32 iVal = (Int32)NPVARIANT_TO_DOUBLE(npvValue);
                        reinterpret_cast< ArrayOf<Int32>* >(carArray)->Set(i, iVal);
                    }

                    *(ArrayOf<Int32>**)cptr = (ArrayOf<Int32>*)carArray;
                    _CarQuintet_AddRef(carArray);

                    break;
                }
                // case CarDataType_Int64:
                // case CarDataType_Byte:
                // case CarDataType_Float:
                // case CarDataType_Double:
                // case CarDataType_Char8:
                // case CarDataType_Char16:
                // case CarDataType_Char32:
                // case CarDataType_CString:
                case CarDataType_String:
                {
                    ALOGD("convertV8ValueToCarValue_CallerAllocOut====output====ArrayOf======String====0");

                    carArray = ArrayOf<Elastos::String>::Alloc(length);

                    for (Int32 i = 0; i < length; i++) {
                        NPVariant npvValue;
                        _NPN_GetProperty(0, object, _NPN_GetIntIdentifier(i), &npvValue);
                        NPString src = NPVARIANT_TO_STRING(npvValue);
                        reinterpret_cast< ArrayOf<Elastos::String>* >(carArray)->Set(i, Elastos::String(src.UTF8Characters));
                    }

                    *(ArrayOf<Elastos::String>**)cptr = (ArrayOf<Elastos::String>*)carArray;
                    _CarQuintet_AddRef(carArray);

                    break;
                }
                // case CarDataType_Boolean:
                // case CarDataType_EMuid:
                // case CarDataType_EGuid:
                // case CarDataType_ECode:
                // case CarDataType_LocalPtr:
                // case CarDataType_LocalType:
                // case CarDataType_Enum:
                // case CarDataType_StringBuf:
                // case CarDataType_ArrayOf:
                // case CarDataType_BufferOf:
                // case CarDataType_MemoryOf:
                // case CarDataType_CppVector:
                // case CarDataType_Struct:
                // case CarDataType_Interface:
                default:
                {
                    ALOGD("convertV8ValueToCarValue_CallerAllocOut====LocalPtr======ArrayOf======other");
                    break;
                }
            }  //switch

            break;
        }   //case CarDataType_ArrayOf
        // case CarDataType_BufferOf:
        // {
        //     ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do====BufferOf====CarDataType:%d", ctype);
        //     break;
        // }
        // case CarDataType_MemoryBuf:
        // {
        //     ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do====MemoryBuf====CarDataType:%d", ctype);
        //     break;
        // }
        case CarDataType_CppVector:
        {
            ALOGD("convertV8ValueToCarValue_CallerAllocOut====get output param value===to do====CppVector====CarDataType:%d", ctype);
            break;
        }
        case CarDataType_Struct:
        {
            ALOGD("convertV8ValueToCarValue_CallerAllocOut====get output param value===to do====Struct====CarDataType:%d", ctype);
            break;
        }
        case CarDataType_Interface:
        {
            if (cptr) {
                ALOGD("convertV8ValueToCarValue_CallerAllocOut====get output param value====Interface====CarDataType:%d", ctype);
                if(NPVARIANT_IS_OBJECT(npvOutValue)) {
                    NPObject* obj = NPVARIANT_TO_OBJECT(npvOutValue);
                    CarNPObject* carObj = reinterpret_cast<CarNPObject*>(obj);
                    CobjectWrapper* objectWrapper = carObj->mInstance->getInstance();
                    *(IInterface**)cptr = objectWrapper->getInstance();
                }
                else {
                    ALOGD("convertV8ValueToCarValue_CallerAllocOut====get output param value====Error:No Valid Return Data!===");
                    *(IInterface**)cptr = NULL;
                    //ASSERT(0);
                }
            }
            else {
                ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value====Interface null====CarDataType:%d", ctype);
            }

            break;
        }
        default: {
            ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do===other====CarDataType:%d", ctype);
            break;
        }
    }   //switch (ctype)
}   //void convertV8ValueToCarValue_CallerAllocOut

} // namespace Bindings
} // namespace JSC

#endif // ENABLE(JAVA_BRIDGE)