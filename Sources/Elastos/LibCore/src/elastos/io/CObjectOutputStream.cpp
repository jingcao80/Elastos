//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "CObjectOutputStream.h"
#include "CDataOutputStream.h"
#include "CByteArrayOutputStream.h"
#include "ObjectStreamConstants.h"
#include "CoreUtils.h"

using Elastos::Core::CoreUtils;

namespace Elastos {
namespace IO {

const Byte CObjectOutputStream::NOT_SC_BLOCK_DATA = (Byte) (ObjectStreamConstants::SC_BLOCK_DATA ^ 0xFF);

CAR_OBJECT_IMPL(CObjectOutputStream)
CAR_INTERFACE_IMPL_3(CObjectOutputStream, OutputStream, IObjectOutputStream, IObjectOutput, IDataOutput)
CObjectOutputStream::CObjectOutputStream()
    : mNestedLevels(0)
    , mEnableReplace(FALSE)
    , mCurrentHandle(0)
    , mProtocolVersion(0)
    , mSubclassOverridingImplementation(FALSE)
{
    assert(0 && "TODO");
    // mProxyClassDesc = ObjectStreamClass::Lookup(Proxy.class);
}

ECode CObjectOutputStream::constructor() /*throws IOException*/
{
    /*
     * WARNING - we should throw IOException if not called from a subclass
     * according to the JavaDoc. Add the test.
     */
    mSubclassOverridingImplementation = TRUE;
    return NOERROR;
}

ECode CObjectOutputStream::constructor(
    /* [in] */ IOutputStream* output) /*throws IOException*/
{
    if (IDataOutputStream::Probe(output)) {
        mOutput = IDataOutputStream::Probe(output);
    }
    else {
        CDataOutputStream::New(output, (IDataOutputStream**)&mOutput);
    }
    mEnableReplace = FALSE;
    mProtocolVersion = ObjectStreamConstants::PROTOCOL_VERSION_2;
    mSubclassOverridingImplementation = FALSE;

    ResetState();
    // So write...() methods can be used by
    // subclasses during WriteStreamHeader()
    mPrimitiveTypes = mOutput;
    // Has to be done here according to the specification
    WriteStreamHeader();
    mPrimitiveTypes = NULL;
    return NOERROR;
}

ECode CObjectOutputStream::AnnotateClass(
    /* [in] */ const ClassID& aClass) /*throws IOException*/
{
    // By default no extra info is saved. Subclasses can override
    return NOERROR;
}

ECode CObjectOutputStream::AnnotateProxyClass(
    /* [in] */ const ClassID& aClass) /*throws IOException*/
{
    // By default no extra info is saved. Subclasses can override
    return NOERROR;
}

void CObjectOutputStream::CheckWritePrimitiveTypes()
{
    if (mPrimitiveTypes == NULL) {
        // If we got here we have no Stream previously created
        // WARNING - if the stream does not grow, this code is wrong
        CByteArrayOutputStream::New(128, (IByteArrayOutputStream**)&mPrimitiveTypesBuffer);
        CDataOutputStream::New(IOutputStream::Probe(mPrimitiveTypesBuffer), (IDataOutputStream**)&mPrimitiveTypes);
    }
}

ECode CObjectOutputStream::Close() /*throws IOException*/
{
    // First Flush what is needed (primitive data, etc)
    FAIL_RETURN(Flush());
    return ICloseable::Probe(mOutput)->Close();
}

void CObjectOutputStream::ComputePutField()
{
    assert(0 && "TODO");
    // mCurrentPutField = new EmulatedFieldsForDumping(this, mCurrentClass);
}

ECode CObjectOutputStream::DefaultWriteObject() /*throws IOException*/
{
    if (mCurrentObject == NULL) {
        // throw new NotActiveException();
        return E_NOT_ACTIVE_EXCEPTION;
    }
    return WriteFieldValues(mCurrentObject, mCurrentClass);
}

ECode CObjectOutputStream::Drain() /*throws IOException*/
{
    if (mPrimitiveTypes == NULL || mPrimitiveTypesBuffer == NULL) {
        return NOERROR;
    }

    // If we got here we have a Stream previously created
    Int32 offset = 0;
    AutoPtr<ArrayOf<Byte> > written;
    FAIL_RETURN(mPrimitiveTypesBuffer->ToByteArray((ArrayOf<Byte>**)&written));
    // Normalize the primitive data
    while (offset < written->GetLength()) {
        Int32 toWrite = written->GetLength() - offset > 1024 ? 1024
                : written->GetLength() - offset;
        if (toWrite < 256) {
            FAIL_RETURN(mOutput->WriteByte(ObjectStreamConstants::TC_BLOCKDATA));
            FAIL_RETURN(mOutput->WriteByte((Byte) toWrite));
        }
        else {
            FAIL_RETURN(mOutput->WriteByte(ObjectStreamConstants::TC_BLOCKDATALONG));
            FAIL_RETURN(mOutput->WriteInt32(toWrite));
        }

        // write primitive types we had and the marker of end-of-buffer
        FAIL_RETURN(IOutputStream::Probe(mOutput)->Write(written, offset, toWrite));
        offset += toWrite;
    }

    // and now we're clean to a state where we can write an object
    mPrimitiveTypes = NULL;
    mPrimitiveTypesBuffer = NULL;
    return NOERROR;
}

ECode CObjectOutputStream::DumpCycle(
    /* [in] */ IInterface* obj,
    /* [out] */ Int32* result) /*throws IOException*/
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    // If the object has been saved already, save its handle only
    Int32 handle = 0;
    FAIL_RETURN(mObjectsWritten->Get(obj, &handle));
    if (handle != -1) {
        FAIL_RETURN(WriteCyclicReference(handle));
        *result = handle;
        return NOERROR;
    }
    *result = -1;
    return NOERROR;
}

Boolean CObjectOutputStream::EnableReplaceObject(
    /* [in] */ Boolean enable)
{
    Boolean originalValue = mEnableReplace;
    mEnableReplace = enable;
    return originalValue;
}

ECode CObjectOutputStream::Flush() /*throws IOException*/
{
    FAIL_RETURN(Drain());
    return IFlushable::Probe(mOutput)->Flush();
}

Int32 CObjectOutputStream::NextHandle()
{
    return mCurrentHandle++;
}

ECode CObjectOutputStream::PutFields(
    /* [out] */ IObjectOutputStreamPutField** result) /*throws IOException*/
{
    VALIDATE_NOT_NULL(result);
    if (mCurrentObject == NULL) {
        // throw new NotActiveException();
        return E_NOT_ACTIVE_EXCEPTION;
    }
    if (mCurrentPutField == NULL) {
        ComputePutField();
    }
    *result = IObjectOutputStreamPutField::Probe(mCurrentPutField);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

Int32 CObjectOutputStream::RegisterObjectWritten(
    /* [in] */ IInterface* obj)
{
    Int32 handle = NextHandle();
    Int32 tmp = 0;
    mObjectsWritten->Put(obj, handle, &tmp);
    return handle;
}

void CObjectOutputStream::RemoveUnsharedReference(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 previousHandle)
{
    Int32 tmp = 0;
    if (previousHandle != -1) {
        mObjectsWritten->Put(obj, previousHandle, &tmp);
    }
    else {
        mObjectsWritten->Remove(obj, &tmp);
    }
}

ECode CObjectOutputStream::ReplaceObject(
    /* [in] */ IInterface* object,
    /* [out] */ IInterface** result) /*throws IOException*/
{
    VALIDATE_NOT_NULL(result);
    // By default no object replacement. Subclasses can override
    *result = object;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CObjectOutputStream::Reset() /*throws IOException*/
{
    // First we Flush what we have
    FAIL_RETURN(Drain());
    /*
     * And dump a reset marker, so that the ObjectInputStream can reset
     * itself at the same point
     */
    FAIL_RETURN(mOutput->WriteByte(ObjectStreamConstants::TC_RESET));
    // Now we reset ourselves
    ResetState();
    return NOERROR;
}

void CObjectOutputStream::ResetSeenObjects()
{
    assert(0 && "TODO");
    // mObjectsWritten = new SerializationHandleMap();
    mCurrentHandle = ObjectStreamConstants::baseWireHandle;
}

void CObjectOutputStream::ResetState()
{
    ResetSeenObjects();
    mNestedLevels = 0;
}

ECode CObjectOutputStream::UseProtocolVersion(
    /* [in] */ Int32 version) /*throws IOException*/
{
    Boolean tmp = FALSE;
    if (mObjectsWritten->IsEmpty(&tmp), !tmp) {
        // throw new IllegalStateException("Cannot set protocol version when stream in use");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (version != ObjectStreamConstants::PROTOCOL_VERSION_1
            && version != ObjectStreamConstants::PROTOCOL_VERSION_2) {
        // throw new IllegalArgumentException("Unknown protocol: " + version);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mProtocolVersion = version;
    return NOERROR;
}

ECode CObjectOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length) /*throws IOException*/
{
    CheckWritePrimitiveTypes();
    return IOutputStream::Probe(mPrimitiveTypes)->Write(buffer, offset, length);
}

ECode CObjectOutputStream::Write(
    /* [in] */ Int32 value) /*throws IOException*/
{
    CheckWritePrimitiveTypes();
    return IOutputStream::Probe(mPrimitiveTypes)->Write(value);
}

ECode CObjectOutputStream::WriteBoolean(
    /* [in] */ Boolean value) /*throws IOException*/
{
    CheckWritePrimitiveTypes();
    return mPrimitiveTypes->WriteBoolean(value);
}

ECode CObjectOutputStream::WriteByte(
    /* [in] */ Int32 value) /*throws IOException*/
{
    CheckWritePrimitiveTypes();
    return mPrimitiveTypes->WriteByte(value);
}

ECode CObjectOutputStream::WriteBytes(
    /* [in] */ const String& value) /*throws IOException*/
{
    CheckWritePrimitiveTypes();
    return mPrimitiveTypes->WriteBytes(value);
}

ECode CObjectOutputStream::WriteChar(
    /* [in] */ Int32 value) /*throws IOException*/
{
    CheckWritePrimitiveTypes();
    return mPrimitiveTypes->WriteChar(value);
}

ECode CObjectOutputStream::WriteChars(
    /* [in] */ const String& value) /*throws IOException*/
{
    CheckWritePrimitiveTypes();
    return mPrimitiveTypes->WriteChars(value);
}

ECode CObjectOutputStream::WriteClassDesc(
    /* [in] */ IObjectStreamClass* classDesc,
    /* [in] */ Boolean unshared,
    /* [out] */ Int32* result) /*throws IOException*/
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    if (classDesc == NULL) {
        FAIL_RETURN(WriteNull());
        *result = -1;
        return NOERROR;
    }
    Int32 handle = -1;
    if (!unshared) {
        FAIL_RETURN(DumpCycle(classDesc, &handle));
    }
    if (handle == -1) {
        ClassID classToWrite;
        classDesc->ForClass(&classToWrite);
        Int32 previousHandle = -1;
        if (unshared) {
            FAIL_RETURN(mObjectsWritten->Get(classDesc, &previousHandle));
        }
        // If we got here, it is a new (non-NULL) classDesc that will have
        // to be registered as well
        handle = RegisterObjectWritten(classDesc);

        Boolean tmp = FALSE;
        if (classDesc->IsProxy(&tmp)) {
            FAIL_RETURN(mOutput->WriteByte(ObjectStreamConstants::TC_PROXYCLASSDESC));
            AutoPtr<ArrayOf<ClassID> > interfaces;
            assert(0 && "TODO");
            // classToWrite->GetInterfaces((ArrayOf<ClassID>**)&interfaces);
            mOutput->WriteInt32(interfaces->GetLength());
            for (Int32 i = 0; i < interfaces->GetLength(); i++) {
                String n;
                assert(0 && "TODO");
                // (*interfaces)[i]->GetName(&n);
                mOutput->WriteUTF(n);
            }
            FAIL_RETURN(AnnotateProxyClass(classToWrite));
            FAIL_RETURN(mOutput->WriteByte(ObjectStreamConstants::TC_ENDBLOCKDATA));
            Int32 i = 0;
            FAIL_RETURN(WriteClassDesc(mProxyClassDesc, FALSE, &i));
            if (unshared) {
                // remove reference to unshared object
                RemoveUnsharedReference(classDesc, previousHandle);
            }
            *result = handle;
            return NOERROR;
        }

        mOutput->WriteByte(ObjectStreamConstants::TC_CLASSDESC);
        if (mProtocolVersion == ObjectStreamConstants::PROTOCOL_VERSION_1) {
            FAIL_RETURN(WriteNewClassDesc(classDesc));
        }
        else {
            // So write...() methods can be used by
            // subclasses during WriteClassDescriptor()
            mPrimitiveTypes = mOutput;
            FAIL_RETURN(WriteClassDescriptor(classDesc));
            mPrimitiveTypes = NULL;
        }
        // Extra class info (optional)
        FAIL_RETURN(AnnotateClass(classToWrite));
        FAIL_RETURN(Drain()); // Flush primitive types in the annotation
        mOutput->WriteByte(ObjectStreamConstants::TC_ENDBLOCKDATA);
        AutoPtr<IObjectStreamClass> ss;
        assert(0 && "TODO");
        // classDesc->GetSuperclass((IObjectStreamClass**)&ss);
        Int32 i = 0;
        FAIL_RETURN(WriteClassDesc(ss, unshared, &i));
        if (unshared) {
            // remove reference to unshared object
            RemoveUnsharedReference(classDesc, previousHandle);
        }
    }
    *result = handle;
    return NOERROR;
}

ECode CObjectOutputStream::WriteCyclicReference(
    /* [in] */ Int32 handle) /*throws IOException*/
{
    FAIL_RETURN(mOutput->WriteByte(ObjectStreamConstants::TC_REFERENCE));
    return mOutput->WriteInt32(handle);
}

ECode CObjectOutputStream::WriteDouble(
    /* [in] */ Double value) /*throws IOException*/
{
    CheckWritePrimitiveTypes();
    return mPrimitiveTypes->WriteDouble(value);
}

ECode CObjectOutputStream::WriteFieldDescriptors(
    /* [in] */ IObjectStreamClass* classDesc,
    /* [in] */ Boolean externalizable) /*throws IOException*/
{
    ClassID loadedClass;
    classDesc->ForClass(&loadedClass);
    AutoPtr<ArrayOf<IObjectStreamField*> > fields;
    Int32 fieldCount = 0;

    // The fields of String are not needed since Strings are treated as
    // primitive types
    assert(0 && "TODO");
    // if (!externalizable && loadedClass != ObjectStreamClass::STRINGCLASS) {
    //     classDesc->Fields((ArrayOf<IObjectStreamField*>**)&fields);
    //     fieldCount = fields->GetLength();
    // }

    // Field count
    FAIL_RETURN(mOutput->WriteInt16(fieldCount));
    // Field names
    for (Int32 i = 0; i < fieldCount; i++) {
        AutoPtr<IObjectStreamField> f = (*fields)[i];
        Boolean wasPrimitive = FALSE;
        assert(0 && "TODO");
        // FAIL_RETURN(f->WriteField(mOutput, &wasPrimitive));
        if (!wasPrimitive) {
            String s;
            f->GetTypeString(&s);
            FAIL_RETURN(WriteObject(CoreUtils::Convert(s)));
        }
    }
    return NOERROR;
}

ECode CObjectOutputStream::WriteFields() /*throws IOException*/
{
    // Has to have fields to write
    if (mCurrentPutField == NULL) {
        // throw new NotActiveException();
        return E_NOT_ACTIVE_EXCEPTION;
    }
    return WriteFieldValues(mCurrentPutField);
}

ECode CObjectOutputStream::WriteFieldValues(
    /* [in] */ IEmulatedFieldsForDumping* emulatedFields) /*throws IOException*/
{
    assert(0 && "TODO");
    // Access internal fields which we can set/get. Users can't do this.
    // AutoPtr<EmulatedFields> accessibleSimulatedFields = emulatedFields->EmulatedFields();
    // for (EmulatedFields.ObjectSlot slot : accessibleSimulatedFields.slots()) {
    //     Object fieldValue = slot.getFieldValue();
    //     Class<?> type = slot.getField().getType();
    //     if (type == Int32.class) {
    //         FAIL_RETURN(mOutput->WriteInt32(fieldValue != NULL ? ((Integer) fieldValue).intValue() : 0));
    //     }
    //     else if (type == Byte.class) {
    //         FAIL_RETURN(mOutput->WriteByte(fieldValue != NULL ? ((Byte) fieldValue).byteValue() : 0));
    //     }
    //     else if (type == Char32.class) {
    //         FAIL_RETURN(mOutput->WriteChar(fieldValue != NULL ? ((Character) fieldValue).charValue() : 0));
    //     }
    //     else if (type == Int16.class) {
    //         FAIL_RETURN(mOutput->WriteInt16(fieldValue != NULL ? ((Short) fieldValue).shortValue() : 0));
    //     }
    //     else if (type == Boolean.class) {
    //         FAIL_RETURN(mOutput->WriteBoolean(fieldValue != NULL ? ((Boolean) fieldValue).booleanValue() : FALSE));
    //     }
    //     else if (type == Int64.class) {
    //         FAIL_RETURN(mOutput->WriteInt64(fieldValue != NULL ? ((Long) fieldValue).longValue() : 0));
    //     }
    //     else if (type == Float.class) {
    //         FAIL_RETURN(mOutput->WriteFloat(fieldValue != NULL ? ((Float) fieldValue).floatValue() : 0));
    //     }
    //     else if (type == Double.class) {
    //         FAIL_RETURN(mOutput->WriteDouble(fieldValue != NULL ? ((Double) fieldValue).doubleValue() : 0));
    //     }
    //     else {
    //         // Either array or Object
    //         FAIL_RETURN(WriteObject(fieldValue));
    //     }
    // }
    return NOERROR;
}

ECode CObjectOutputStream::WriteFieldValues(
    /* [in] */ IInterface* obj,
    /* [in] */ IObjectStreamClass* classDesc) /*throws IOException*/
{
    assert(0 && "TODO");
    // for (ObjectStreamField fieldDesc : classDesc.fields()) {
    //     try {
    //         Class<?> type = fieldDesc.getTypeInternal();
    //         Field field = classDesc.checkAndGetReflectionField(fieldDesc);
    //         if (field == NULL) {
    //             throw new InvalidClassException(classDesc.getName()
    //                     + " doesn't have a serializable field " + fieldDesc.getName()
    //                     + " of type " + type);
    //         }
    //         if (type == Byte.class) {
    //             FAIL_RETURN(mOutput->WriteByte(field.getByte(obj)));
    //         }
    //         else if (type == Char32.class) {
    //             FAIL_RETURN(mOutput->WriteChar(field.getChar(obj)));
    //         }
    //         else if (type == Double.class) {
    //             FAIL_RETURN(mOutput->WriteDouble(field.getDouble(obj)));
    //         }
    //         else if (type == Float.class) {
    //             FAIL_RETURN(mOutput->WriteFloat(field.getFloat(obj)));
    //         }
    //         else if (type == Int32.class) {
    //             FAIL_RETURN(mOutput->WriteInt32(field.getInt(obj)));
    //         }
    //         else if (type == Int64.class) {
    //             FAIL_RETURN(mOutput->WriteInt64(field.getLong(obj)));
    //         }
    //         else if (type == Int16.class) {
    //             FAIL_RETURN(mOutput->WriteInt16(field.getShort(obj)));
    //         }
    //         else if (type == Boolean.class) {
    //             FAIL_RETURN(mOutput->WriteBoolean(field.getBoolean(obj)));
    //         }
    //         else {
    //             // Reference types (including arrays).
    //             Object objField = field.get(obj);
    //             if (fieldDesc.isUnshared()) {
    //                 writeUnshared(objField);
    //             }
    //             else {
    //                 WriteObject(objField);
    //             }
    //         }
    //     } catch (IllegalAccessException iae) {
    //         // ObjectStreamField should have called setAccessible(TRUE).
    //         throw new AssertionError(iae);
    //     } catch (NoSuchFieldError nsf) {
    //         // The user defined serialPersistentFields but did not provide
    //         // the glue to transfer values in WriteObject, so we ended up using
    //         // the default mechanism but failed to set the emulated field.
    //         throw new InvalidClassException(classDesc.getName());
    //     }
    // }
    return NOERROR;
}

ECode CObjectOutputStream::WriteFloat(
    /* [in] */ Float value) /*throws IOException*/
{
    CheckWritePrimitiveTypes();
    return mPrimitiveTypes->WriteFloat(value);
}

ECode CObjectOutputStream::WriteHierarchy(
    /* [in] */ IInterface* object,
    /* [in] */ IObjectStreamClass* classDesc) /*throws IOException, NotActiveException*/
{
    if (object == NULL) {
        // throw new NotActiveException();
        return E_NOT_ACTIVE_EXCEPTION;
    }

    // assert(0 && "TODO");

    // // Fields are written from class closest to Object to leaf class
    // // (down the chain)
    // List<ObjectStreamClass> hierarchy = classDesc.getHierarchy();
    // for (Int32 i = 0, end = hierarchy.size(); i < end; ++i) {
    //     ObjectStreamClass osc = hierarchy.get(i);
    //     // Have to do this before calling DefaultWriteObject or anything
    //     // that calls DefaultWriteObject
    //     mCurrentObject = object;
    //     mCurrentClass = osc;

    //     // See if the object has a WriteObject method. If so, run it
    //     try {
    //         Boolean executed = FALSE;
    //         if (osc.hasMethodWriteObject()) {
    //             final Method method = osc.getMethodWriteObject();
    //             try {
    //                 method.invoke(object, new Object[] { this });
    //                 executed = TRUE;
    //             } catch (InvocationTargetException e) {
    //                 Throwable ex = e.getTargetException();
    //                 if (ex instanceof RuntimeException) {
    //                     throw (RuntimeException) ex;
    //                 }
    //             else if (ex instanceof Error) {
    //                     throw (Error) ex;
    //                 }
    //                 throw (IOException) ex;
    //             } catch (IllegalAccessException e) {
    //                 throw new RuntimeException(e.toString());
    //             }
    //         }

    //         if (executed) {
    //             Drain();
    //             mOutput->WriteByte(TC_ENDBLOCKDATA);
    //         }
    //         else {
    //             // If the object did not have a writeMethod, call
    //             // DefaultWriteObject
    //             DefaultWriteObject();
    //         }
    //     } finally {
    //         // Cleanup, needs to run always so that we can later detect
    //         // invalid calls to DefaultWriteObject
    //         mCurrentObject = NULL;
    //         mCurrentClass = NULL;
    //         mCurrentPutField = NULL;
    //     }
    // }
    return NOERROR;
}

ECode CObjectOutputStream::WriteInt32(
    /* [in] */ Int32 value) /*throws IOException*/
{
    CheckWritePrimitiveTypes();
    return mPrimitiveTypes->WriteInt32(value);
}

ECode CObjectOutputStream::WriteInt64(
    /* [in] */ Int64 value) /*throws IOException*/
{
    CheckWritePrimitiveTypes();
    return mPrimitiveTypes->WriteInt64(value);
}

ECode CObjectOutputStream::WriteNewArray(
    /* [in] */ IInterface* array,
    /* [in] */ const ClassID& arrayClass,
    /* [in] */ IObjectStreamClass* arrayClDesc,
    /* [in] */ const ClassID& componentType,
    /* [in] */ Boolean unshared,
    /* [out] */ Int32* result) /*throws IOException*/
{
    VALIDATE_NOT_NULL(result);
    mOutput->WriteByte(ObjectStreamConstants::TC_ARRAY);
    Int32 iv = 0;
    FAIL_RETURN(WriteClassDesc(arrayClDesc, FALSE, &iv));

    Int32 handle = NextHandle();
    if (!unshared) {
        Int32 tv = 0;
        mObjectsWritten->Put(array, handle, &tv);
    }

    // Now we have code duplication just because Java is typed. We have to
    // write N elements and assign to array positions, but we must typecast
    // the array first, and also call different methods depending on the
    // elements.

    assert(0 && "TODO");
    // if (componentType.isPrimitive()) {
    //     if (componentType == Int32.class) {
    //         Int32[] intArray = (Int32[]) array;
    //         mOutput->WriteInt32(intArray.length);
    //         for (Int32 i = 0; i < intArray.length; i++) {
    //             mOutput->WriteInt32(intArray[i]);
    //         }
    //     }
    //     else if (componentType == Byte.class) {
    //         Byte[] byteArray = (Byte[]) array;
    //         mOutput->WriteInt32(byteArray.length);
    //         mOutput->Write(byteArray, 0, byteArray.length);
    //     }
    //     else if (componentType == Char32.class) {
    //         Char32[] charArray = (Char32[]) array;
    //         mOutput->WriteInt32(charArray.length);
    //         for (Int32 i = 0; i < charArray.length; i++) {
    //             mOutput->WriteChar(charArray[i]);
    //         }
    //     }
    //     else if (componentType == Int16.class) {
    //         Int16[] shortArray = (Int16[]) array;
    //         mOutput->WriteInt32(shortArray.length);
    //         for (Int32 i = 0; i < shortArray.length; i++) {
    //             mOutput->WriteInt16(shortArray[i]);
    //         }
    //     }
    //     else if (componentType == Boolean.class) {
    //         Boolean[] booleanArray = (Boolean[]) array;
    //         mOutput->WriteInt32(booleanArray.length);
    //         for (Int32 i = 0; i < booleanArray.length; i++) {
    //             mOutput->WriteBoolean(booleanArray[i]);
    //         }
    //     }
    //     else if (componentType == Int64.class) {
    //         Int64[] longArray = (Int64[]) array;
    //         mOutput->WriteInt32(longArray.length);
    //         for (Int32 i = 0; i < longArray.length; i++) {
    //             mOutput->WriteInt64(longArray[i]);
    //         }
    //     }
    //     else if (componentType == Float.class) {
    //         Float[] floatArray = (Float[]) array;
    //         mOutput->WriteInt32(floatArray.length);
    //         for (Int32 i = 0; i < floatArray.length; i++) {
    //             mOutput->WriteFloat(floatArray[i]);
    //         }
    //     }
    //     else if (componentType == Double.class) {
    //         Double[] doubleArray = (Double[]) array;
    //         mOutput->WriteInt32(doubleArray.length);
    //         for (Int32 i = 0; i < doubleArray.length; i++) {
    //             mOutput->WriteDouble(doubleArray[i]);
    //         }
    //     }
    //     else {
    //         throw new InvalidClassException("Wrong base type in " + arrayClass.getName());
    //     }
    // }
    // else {
    //     // Array of Objects
    //     Object[] objectArray = (Object[]) array;
    //     mOutput->WriteInt32(objectArray.length);
    //     for (Int32 i = 0; i < objectArray.length; i++) {
    //         // TODO: This place is the opportunity for enhancement
    //         //      We can implement writing elements through fast-path,
    //         //      without setting up the context (see WriteObject()) for
    //         //      each element with public API
    //         WriteObject(objectArray[i]);
    //     }
    // }
    return handle;
}

ECode CObjectOutputStream::WriteNewClass(
    /* [in] */ const ClassID& object,
    /* [in] */ Boolean unshared,
    /* [out] */ Int32* result) /*throws IOException*/
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(mOutput->WriteByte(ObjectStreamConstants::TC_CLASS));

    // Instances of java.lang.Class are always Serializable, even if their
    // instances aren't (e.g. java.lang.Object.class).
    // We cannot call lookup because it returns NULL if the parameter
    // represents instances that cannot be serialized, and that is not what
    // we want.
    assert(0 && "TODO");
    AutoPtr<IObjectStreamClass> clDesc;
    // clDesc = ObjectStreamClass::LookupStreamClass(object);

    // The handle for the classDesc is NOT the handle for the class object
    // being dumped. We must allocate a new handle and return it.
    Boolean tmp = FALSE;
    if (clDesc->IsEnum(&tmp), tmp) {
        AutoPtr<IObjectStreamClass> os;
        FAIL_RETURN(WriteEnumDesc(clDesc, unshared, (IObjectStreamClass**)&os));
    }
    else {
        Int32 iv = 0;
        FAIL_RETURN(WriteClassDesc(clDesc, unshared, &iv));
    }

    Int32 handle = NextHandle();
    if (!unshared) {
        assert(0 && "TODO");
        // Int32 tmp = 0;
        // mObjectsWritten->Put(object, handle, &tmp);
    }

    *result = handle;
    return NOERROR;
}

ECode CObjectOutputStream::WriteNewClassDesc(
    /* [in] */ IObjectStreamClass* classDesc) /*throws IOException*/
{
    assert(0 && "TODO");
    // FAIL_RETURN(mOutput->WriteUTF(classDesc.getName()));
    // FAIL_RETURN(mOutput->WriteInt64(classDesc.getSerialVersionUID()));
    // Byte flags = 0;
    // classDesc->GetFlags(&flags);

    // Boolean externalizable = FALSE;
    // classDesc->IsExternalizable(&externalizable);

    // if (externalizable) {
    //     if (mProtocolVersion == ObjectStreamConstants::PROTOCOL_VERSION_1) {
    //         flags &= NOT_SC_BLOCK_DATA;
    //     }
    //     else {
    //         // Change for 1.2. Objects can be saved in old format
    //         // (PROTOCOL_VERSION_1) or in the 1.2 format (PROTOCOL_VERSION_2).
    //         flags |= ObjectStreamConstants::SC_BLOCK_DATA;
    //     }
    // }
    // FAIL_RETURN(mOutput->WriteByte(flags));
    // Int32 flags = 0;
    // classDesc->GetFlags(&flags);
    // if ((ObjectStreamConstants::SC_ENUM | ObjectStreamConstants::SC_SERIALIZABLE) != flags) {
    //     FAIL_RETURN(WriteFieldDescriptors(classDesc, externalizable));
    // }
    // else {
    //     // enum write no fields
    //     FAIL_RETURN(mOutput->WriteInt16(0));
    // }
    return NOERROR;
}

ECode CObjectOutputStream::WriteClassDescriptor(
    /* [in] */ IObjectStreamClass* classDesc) /*throws IOException*/
{
    return WriteNewClassDesc(classDesc);
}

ECode CObjectOutputStream::WriteNewException(
    /* [in] */ /*Exception ex*/) /*throws IOException*/
{
    FAIL_RETURN(mOutput->WriteByte(ObjectStreamConstants::TC_EXCEPTION));
    ResetSeenObjects();
    Int32 tmp = 0;
    FAIL_RETURN(WriteObjectInternal(NULL, /*ex, */FALSE, FALSE, FALSE, &tmp)); // No replacements
    ResetSeenObjects();
    return NOERROR;
}

ECode CObjectOutputStream::WriteNewObject(
    /* [in] */ IInterface* object,
    /* [in] */ const ClassID theClass,
    /* [in] */ IObjectStreamClass* clDesc,
    /* [in] */ Boolean unshared,
    /* [out] */ Int32* result) /*throws IOException*/
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    // Not String, not NULL, not array, not cyclic reference

    assert(0 && "TODO");
    // AutoPtr<IEmulatedFieldsForDumping> originalCurrentPutField = mCurrentPutField; // save
    // mCurrentPutField = NULL; // NULL it, to make sure one will be computed if
    // // needed

    // Boolean externalizable = clDesc.isExternalizable();
    // Boolean serializable = clDesc.isSerializable();
    // if (!externalizable && !serializable) {
    //     // Object is neither externalizable nor serializable. Error
    //     // throw new NotSerializableException(theClass.getName());
    //     return E_NOT_SERIALIZABLE_EXCEPTION;
    // }

    // // Either serializable or externalizable, now we can save info
    // FAIL_RETURN(mOutput->WriteByte(ObjectStreamConstants::TC_OBJECT));
    // Int32 iv = 0;
    // WriteClassDesc(clDesc, FALSE, &iv);
    // Int32 previousHandle = -1;
    // if (unshared) {
    //     mObjectsWritten->Get(object, &previousHandle);
    // }

    // Int32 handle = RegisterObjectWritten(object);

    // // This is how we know what to do in DefaultWriteObject. And it is also
    // // used by DefaultWriteObject to check if it was called from an invalid
    // // place.
    // // It allows writeExternal to call DefaultWriteObject and have it work.
    // mCurrentObject = object;
    // mCurrentClass = clDesc;
    // // try {
    //     if (externalizable) {
    //         Boolean noBlockData = mProtocolVersion == ObjectStreamConstants::PROTOCOL_VERSION_1;
    //         if (noBlockData) {
    //             mPrimitiveTypes = mOutput;
    //         }
    //         // Object is externalizable, just call its own method
    //         IExternalizable::Probe(object)->WriteExternal(this);
    //         if (noBlockData) {
    //             mPrimitiveTypes = NULL;
    //         }
    //         else {
    //             // Similar to the code in WriteHierarchy when object
    //             // implements WriteObject.
    //             // Any primitive data has to be flushed and a tag must be
    //             // written
    //             FAIL_RETURN(Drain());
    //             FAIL_RETURN(mOutput->WriteByte(ObjectStreamConstants::TC_ENDBLOCKDATA));
    //         }
    //     }
    //     else { // If it got here, it has to be Serializable
    //         // Object is serializable. Walk the class chain writing the
    //         // fields
    //         FAIL_RETURN(WriteHierarchy(object, mCurrentClass));
    //     }
    // // } finally {
    //     // Cleanup, needs to run always so that we can later detect invalid
    //     // calls to DefaultWriteObject
    //     if (unshared) {
    //         // remove reference to unshared object
    //         RemoveUnsharedReference(object, previousHandle);
    //     }
    //     mCurrentObject = NULL;
    //     mCurrentClass = NULL;
    //     mCurrentPutField = originalCurrentPutField;
    // // }

    // *result = handle;
    return NOERROR;
}

ECode CObjectOutputStream::WriteNewString(
    /* [in] */ const String& object,
    /* [in] */ Boolean unshared,
    /* [out] */ Int32* result) /*throws IOException*/
{
    VALIDATE_NOT_NULL(result);
    assert(0 && "TODO");
    // Int64 count = ModifiedUtf8.countBytes(object, FALSE);
    // Byte[] buffer;
    // Int32 offset = 0;
    // if (count <= 0xffff) {
    //     buffer = new Byte[1 + SizeOf.SHORT + (Int32) count];
    //     buffer[offset++] = ObjectStreamConstants::TC_STRING;
    //     Memory.pokeShort(buffer, offset, (Int16) count, ByteOrder.BIG_ENDIAN);
    //     offset += SizeOf.SHORT;
    // }
    // else {
    //     buffer = new Byte[1 + SizeOf.LONG + (Int32) count];
    //     buffer[offset++] = ObjectStreamConstants::TC_LONGSTRING;
    //     Memory.pokeLong(buffer, offset, count, ByteOrder.BIG_ENDIAN);
    //     offset += SizeOf.LONG;
    // }
    // ModifiedUtf8.encode(buffer, offset, object);
    // FAIL_RETURN(mOutput->Write(buffer, 0, buffer.length));

    // Int32 handle = NextHandle();
    // if (!unshared) {
    //     mObjectsWritten.put(object, handle);
    // }

    // *result = handle;
    return NOERROR;
}

ECode CObjectOutputStream::WriteNull() /*throws IOException*/
{
    return mOutput->WriteByte(ObjectStreamConstants::TC_NULL);
}

ECode CObjectOutputStream::WriteObject(
    /* [in] */ IInterface* object) /*throws IOException*/
{
    return WriteObject(object, FALSE);
}

ECode CObjectOutputStream::WriteUnshared(
    /* [in] */ IInterface* object) /*throws IOException*/
{
    return WriteObject(object, TRUE);
}

ECode CObjectOutputStream::WriteObject(
    /* [in] */ IInterface* object,
    /* [in] */ Boolean unshared) /*throws IOException*/
{
    Boolean setOutput = (mPrimitiveTypes == mOutput);
    if (setOutput) {
        mPrimitiveTypes = NULL;
    }
    // This is the specified behavior in JDK 1.2. Very bizarre way to allow
    // behavior overriding.
    if (mSubclassOverridingImplementation && !unshared) {
        FAIL_RETURN(WriteObjectOverride(object));
        return NOERROR;
    }

    ECode ec = NOERROR;
    do {
        // First we need to Flush primitive types if they were written
        ec = Drain();
        if (FAILED(ec)) {
            break;
        }
        // Actual work, and class-based replacement should be computed
        // if needed.
        Int32 iv = 0;
        ec = WriteObjectInternal(object, unshared, TRUE, TRUE, &iv);
        if (FAILED(ec)) {
            break;
        }
        if (setOutput) {
            mPrimitiveTypes = mOutput;
        }
    } while (0);

    if (FAILED(ec)) {
        // This will make it pass through until the top caller. Only the top caller writes the
        // exception (where it can).
        if (mNestedLevels == 0) {
            // try {
                WriteNewException(/*ioEx1*/);
            // } catch (IOException ioEx2) {
            //     // If writing the exception to the output stream causes another exception there
            //     // is no need to propagate the second exception or generate a third exception,
            //     // both of which might obscure details of the root cause.
            // }
        }
        // throw ioEx1; // and then we propagate the original exception
    }
    return ec;
}

ECode CObjectOutputStream::WriteObjectInternal(
    /* [in] */ IInterface* object,
    /* [in] */ Boolean unshared,
    /* [in] */ Boolean computeClassBasedReplacement,
    /* [in] */ Boolean computeStreamReplacement,
    /* [out] */ Int32* result) /*throws IOException*/
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    if (object == NULL) {
        WriteNull();
        *result = -1;
        return NOERROR;
    }
    if (!unshared) {
        Int32 handle = 0;
        FAIL_RETURN(DumpCycle(object, &handle));
        if (handle != -1) {
            *result = handle; // cyclic reference
            return NOERROR;
        }
    }

    assert(0 && "TODO");
    // // Non-NULL object, first time seen...
    // Class<?> objClass = object.getClass();
    // ObjectStreamClass clDesc = ObjectStreamClass.lookupStreamClass(objClass);

    // mNestedLevels++;
    // try {

    //     if (!(mEnableReplace && computeStreamReplacement)) {
    //         // Is it a Class ?
    //         if (objClass == ObjectStreamClass.CLASSCLASS) {
    //             return writeNewClass((Class<?>) object, unshared);
    //         }
    //         // Is it an ObjectStreamClass ?
    //         if (objClass == ObjectStreamClass.OBJECTSTREAMCLASSCLASS) {
    //             return WriteClassDesc((ObjectStreamClass) object, unshared);
    //         }
    //     }

    //     if (clDesc.isSerializable() && computeClassBasedReplacement) {
    //         if (clDesc.hasMethodWriteReplace()){
    //             Method methodWriteReplace = clDesc.getMethodWriteReplace();
    //             Object replObj;
    //             try {
    //                 replObj = methodWriteReplace.invoke(object, (Object[]) NULL);
    //             } catch (IllegalAccessException iae) {
    //                 replObj = object;
    //             } catch (InvocationTargetException ite) {
    //                 // WARNING - Not sure this is the right thing to do
    //                 // if we can't run the method
    //                 Throwable target = ite.getTargetException();
    //                 if (target instanceof ObjectStreamException) {
    //                     throw (ObjectStreamException) target;
    //                 }
    //             else if (target instanceof Error) {
    //                     throw (Error) target;
    //                 }
    //             else {
    //                     throw (RuntimeException) target;
    //                 }
    //             }
    //             if (replObj != object) {
    //                 // All over, class-based replacement off this time.
    //                 Int32 replacementHandle = WriteObjectInternal(replObj, FALSE, FALSE,
    //                         computeStreamReplacement);
    //                 // Make the original object also map to the same
    //                 // handle.
    //                 if (replacementHandle != -1) {
    //                     mObjectsWritten.put(object, replacementHandle);
    //                 }
    //                 return replacementHandle;
    //             }
    //         }

    //     }

    //     // We get here either if class-based replacement was not needed or
    //     // if it was needed but produced the same object or if it could not
    //     // be computed.
    //     if (mEnableReplace && computeStreamReplacement) {
    //         // Now we compute the stream-defined replacement.
    //         Object streamReplacement = replaceObject(object);
    //         if (streamReplacement != object) {
    //             // All over, class-based replacement off this time.
    //             Int32 replacementHandle = WriteObjectInternal(streamReplacement, FALSE,
    //                     computeClassBasedReplacement, FALSE);
    //             // Make the original object also map to the same handle.
    //             if (replacementHandle != -1) {
    //                 mObjectsWritten.put(object, replacementHandle);
    //             }
    //             return replacementHandle;
    //         }
    //     }

    //     // We get here if stream-based replacement produced the same object

    //     // Is it a Class ?
    //     if (objClass == ObjectStreamClass.CLASSCLASS) {
    //         return writeNewClass((Class<?>) object, unshared);
    //     }

    //     // Is it an ObjectStreamClass ?
    //     if (objClass == ObjectStreamClass.OBJECTSTREAMCLASSCLASS) {
    //         return WriteClassDesc((ObjectStreamClass) object, unshared);
    //     }

    //     // Is it a String ? (instanceof, but == is faster)
    //     if (objClass == ObjectStreamClass.STRINGCLASS) {
    //         return WriteNewString((String) object, unshared);
    //     }

    //     // Is it an Array ?
    //     if (objClass.isArray()) {
    //         return writeNewArray(object, objClass, clDesc, objClass
    //                 .getComponentType(), unshared);
    //     }

    //     if (object instanceof Enum) {
    //         return WriteNewEnum(object, objClass, unshared);
    //     }

    //     // Not a String or Class or Array. Default procedure.
    //     return writeNewObject(object, objClass, clDesc, unshared);
    // }
    // finally {
    //     mNestedLevels--;
    // }

    return NOERROR;
}

ECode CObjectOutputStream::WriteEnumDesc(
    /* [in] */ IObjectStreamClass* classDesc,
    /* [in] */ Boolean unshared,
    /* [out] */ IObjectStreamClass** result) /*throws IOException*/
{
    VALIDATE_NOT_NULL(result);
    // write classDesc, classDesc for enum is different

    // set flag for enum, the flag is (SC_SERIALIZABLE | SC_ENUM)
    classDesc->SetFlags((Byte) (ObjectStreamConstants::SC_SERIALIZABLE | ObjectStreamConstants::SC_ENUM));
    Int32 previousHandle = -1;
    if (unshared) {
        mObjectsWritten->Get(classDesc, &previousHandle);
    }
    Int32 handle = -1;
    if (!unshared) {
        FAIL_RETURN(DumpCycle(classDesc, &handle));
    }
    if (handle == -1) {
        ClassID classToWrite;
        classDesc->ForClass(&classToWrite);
        // If we got here, it is a new (non-NULL) classDesc that will have
        // to be registered as well
        RegisterObjectWritten(classDesc);

        FAIL_RETURN(mOutput->WriteByte(ObjectStreamConstants::TC_CLASSDESC));
        if (mProtocolVersion == ObjectStreamConstants::PROTOCOL_VERSION_1) {
            FAIL_RETURN(WriteNewClassDesc(classDesc));
        }
        else {
            // So write...() methods can be used by
            // subclasses during WriteClassDescriptor()
            mPrimitiveTypes = mOutput;
            FAIL_RETURN(WriteClassDescriptor(classDesc));
            mPrimitiveTypes = NULL;
        }
        // Extra class info (optional)
        FAIL_RETURN(AnnotateClass(classToWrite));
        FAIL_RETURN(Drain()); // Flush primitive types in the annotation
        FAIL_RETURN(mOutput->WriteByte(ObjectStreamConstants::TC_ENDBLOCKDATA));
        // write super class
        AutoPtr<IObjectStreamClass> superClassDesc;
        assert(0 && "TODO");
        // classDesc->GetSuperclass((IObjectStreamClass**)&superClassDesc);
        if (superClassDesc != NULL) {
            // super class is also enum
            superClassDesc->SetFlags((Byte) (ObjectStreamConstants::SC_SERIALIZABLE | ObjectStreamConstants::SC_ENUM));
            AutoPtr<IObjectStreamClass> os;
            FAIL_RETURN(WriteEnumDesc(superClassDesc, unshared, (IObjectStreamClass**)&os));
        }
        else {
            FAIL_RETURN(mOutput->WriteByte(ObjectStreamConstants::TC_NULL));
        }
        if (unshared) {
            // remove reference to unshared object
            RemoveUnsharedReference(classDesc, previousHandle);
        }
    }
    *result = classDesc;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CObjectOutputStream::WriteNewEnum(
    /* [in] */ IInterface* object,
    /* [in] */ const ClassID& theClass,
    /* [in] */ Boolean unshared,
    /* [out] */ Int32* result) /*throws IOException*/
{
    VALIDATE_NOT_NULL(result);
    // write new Enum
    AutoPtr<IEmulatedFieldsForDumping> originalCurrentPutField = mCurrentPutField; // save
    // NULL it, to make sure one will be computed if needed
    mCurrentPutField = NULL;

    mOutput->WriteByte(ObjectStreamConstants::TC_ENUM);
    assert(0 && "TODO");
    // while (theClass != NULL && !theClass.isEnum()) {
    //     // write enum only
    //     theClass = theClass.getSuperclass();
    // }
    AutoPtr<IObjectStreamClass> classDesc;
    assert(0 && "TODO");
    // classDesc = ObjectStreamClass.lookup(theClass);
    AutoPtr<IObjectStreamClass> os;
    WriteEnumDesc(classDesc, unshared, (IObjectStreamClass**)&os);

    Int32 previousHandle = -1;
    if (unshared) {
        mObjectsWritten->Get(object, &previousHandle);
    }
    Int32 handle = RegisterObjectWritten(object);

    AutoPtr<ArrayOf<IObjectStreamField*> > fields;
    assert(0 && "TODO");
    // fields = classDesc.getSuperclass().fields();
    // Only write field "name" for enum class, which is the second field of
    // enum, that is fields[1]. Ignore all non-fields and fields.length < 2
    if (fields != NULL && fields->GetLength() > 1) {
        assert(0 && "TODO");
        // Field field = classDesc.getSuperclass().checkAndGetReflectionField(fields[1]);
        // if (field == NULL) {
        //     throw new NoSuchFieldError();
        // }
        // try {
        //     String str = (String) field.get(object);
        //     Int32 strHandle = -1;
        //     if (!unshared) {
        //         strHandle = DumpCycle(str);
        //     }
        //     if (strHandle == -1) {
        //         WriteNewString(str, unshared);
        //     }
        // } catch (IllegalAccessException iae) {
        //     throw new AssertionError(iae);
        // }
    }

    if (unshared) {
        // remove reference to unshared object
        RemoveUnsharedReference(object, previousHandle);
    }
    mCurrentPutField = originalCurrentPutField;
    *result = handle;
    return NOERROR;
}

ECode CObjectOutputStream::WriteObjectOverride(
    /* [in] */ IInterface* object) /*throws IOException*/
{
    if (!mSubclassOverridingImplementation) {
        // Subclasses must override.
        // throw new IOException();
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode CObjectOutputStream::WriteInt16(
    /* [in] */ Int32 value) /*throws IOException*/
{
    CheckWritePrimitiveTypes();
    return mPrimitiveTypes->WriteInt16(value);
}

ECode CObjectOutputStream::WriteStreamHeader() /*throws IOException*/
{
    FAIL_RETURN(mOutput->WriteInt16(ObjectStreamConstants::STREAM_MAGIC));
    return mOutput->WriteInt16(ObjectStreamConstants::STREAM_VERSION);
}

ECode CObjectOutputStream::WriteUTF(
    /* [in] */ const String& value) /*throws IOException*/
{
    CheckWritePrimitiveTypes();
    return mPrimitiveTypes->WriteUTF(value);
}

ECode CObjectOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* buffer) /*throws IOException*/
{
    return OutputStream::Write(buffer);
}

} // namespace IO
} // namespace Elastos
