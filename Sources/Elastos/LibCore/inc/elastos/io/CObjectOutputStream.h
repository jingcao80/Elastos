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

#ifndef __ELASTOS_IO_COBJECTOUTPUTSTREAM_H__
#define __ELASTOS_IO_COBJECTOUTPUTSTREAM_H__

#include "_Elastos_IO_CObjectOutputStream.h"
#include "OutputStream.h"

namespace Elastos {
namespace IO {

/**
 * A specialized {@link OutputStream} that is able to write (serialize) Java
 * objects as well as primitive data types (Int32, Byte, Char32 etc.). The data can
 * later be loaded using an ObjectInputStream.
 *
 * @see ObjectInputStream
 * @see ObjectOutput
 * @see Serializable
 * @see Externalizable
 */
CarClass(CObjectOutputStream)
    , public OutputStream
    , public IObjectOutputStream
    , public IObjectOutput
    , public IDataOutput
    // , public IAutoCloseable
    // public ObjectStreamConstants
{
public:
    /**
     * PutField is an inner class to provide access to the persistent fields
     * that are written to the target stream.
     */
    class PutField
        : public Object
        , public IObjectOutputStreamPutField
    {
    public:
        CAR_INTERFACE_DECL()

        /**
         * Puts the value of the Boolean field identified by {@code name} to the
         * persistent field.
         *
         * @param name
         *            the name of the field to serialize.
         * @param value
         *            the value that is put to the persistent field.
         */
        virtual CARAPI Put(
            /* [in] */ const String& name,
            /* [in] */ Boolean value) = 0;

        /**
         * Puts the value of the character field identified by {@code name} to
         * the persistent field.
         *
         * @param name
         *            the name of the field to serialize.
         * @param value
         *            the value that is put to the persistent field.
         */
        virtual CARAPI Put(
            /* [in] */ const String& name,
            /* [in] */ Char32 value) = 0;

        /**
         * Puts the value of the Byte field identified by {@code name} to the
         * persistent field.
         *
         * @param name
         *            the name of the field to serialize.
         * @param value
         *            the value that is put to the persistent field.
         */
        virtual CARAPI PutByte(
            /* [in] */ const String& name,
            /* [in] */ Byte value) = 0;

        /**
         * Puts the value of the Int16 field identified by {@code name} to the
         * persistent field.
         *
         * @param name
         *            the name of the field to serialize.
         * @param value
         *            the value that is put to the persistent field.
         */
        virtual CARAPI Put(
            /* [in] */ const String& name,
            /* [in] */ Int16 value) = 0;

        /**
         * Puts the value of the integer field identified by {@code name} to the
         * persistent field.
         *
         * @param name
         *            the name of the field to serialize.
         * @param value
         *            the value that is put to the persistent field.
         */
        virtual CARAPI Put(
            /* [in] */ const String& name,
            /* [in] */ Int32 value) = 0;

        /**
         * Puts the value of the Int64 field identified by {@code name} to the
         * persistent field.
         *
         * @param name
         *            the name of the field to serialize.
         * @param value
         *            the value that is put to the persistent field.
         */
        virtual CARAPI Put(
            /* [in] */ const String& name,
            /* [in] */ Int64 value) = 0;

        /**
         * Puts the value of the Float field identified by {@code name} to the
         * persistent field.
         *
         * @param name
         *            the name of the field to serialize.
         * @param value
         *            the value that is put to the persistent field.
         */
        virtual CARAPI Put(
            /* [in] */ const String& name,
            /* [in] */ Float value) = 0;

        /**
         * Puts the value of the Double field identified by {@code name} to the
         * persistent field.
         *
         * @param name
         *            the name of the field to serialize.
         * @param value
         *            the value that is put to the persistent field.
         */
        virtual CARAPI Put(
            /* [in] */ const String& name,
            /* [in] */ Double value) = 0;

        /**
         * Puts the value of the Object field identified by {@code name} to the
         * persistent field.
         *
         * @param name
         *            the name of the field to serialize.
         * @param value
         *            the value that is put to the persistent field.
         */
        virtual CARAPI Put(
            /* [in] */ const String& name,
            /* [in] */ IInterface* value) = 0;

        /**
         * Writes the fields to the target stream {@code out}.
         *
         * @param out
         *            the target stream
         * @throws IOException
         *             if an error occurs while writing to the target stream.
         * @deprecated This method is unsafe and may corrupt the target stream.
         *             Use ObjectOutputStream#WriteFields() instead.
         */
        // @Deprecated
        virtual CARAPI Write(
            /* [in] */ IObjectOutput* out) /*throws IOException*/ = 0;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CObjectOutputStream();

    /**
     * Constructs a new {@code ObjectOutputStream}. This default constructor can
     * be used by subclasses that do not want to use the public constructor if
     * it allocates unneeded data.
     *
     * @throws IOException
     *             if an error occurs when creating this stream.
     */
    CARAPI constructor() /*throws IOException*/;

    /**
     * Constructs a new ObjectOutputStream that writes to the OutputStream
     * {@code output}.
     *
     * @param output
     *            the non-NULL OutputStream to filter writes on.
     *
     * @throws IOException
     *             if an error occurs while writing the object stream
     *             header
     */
    CARAPI constructor(
        /* [in] */ IOutputStream* output) /*throws IOException*/;

    /**
     * Closes this stream. Any buffered data is flushed. This implementation
     * closes the target stream.
     *
     * @throws IOException
     *             if an error occurs while closing this stream.
     */
    // @Override
    CARAPI Close() /*throws IOException*/;

    /**
     * Default method to write objects to this stream. Serializable fields
     * defined in the object's class and superclasses are written to the output
     * stream.
     *
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     * @throws NotActiveException
     *             if this method is not called from {@code WriteObject()}.
     * @see ObjectInputStream#defaultReadObject
     */
    CARAPI DefaultWriteObject() /*throws IOException*/;

    /**
     * Writes buffered data to the target stream and calls the {@code Flush}
     * method of the target stream.
     *
     * @throws IOException
     *             if an error occurs while writing to or flushing the output
     *             stream.
     */
    // @Override
    CARAPI Flush() /*throws IOException*/;

    /**
     * Gets this stream's {@code PutField} object. This object provides access
     * to the persistent fields that are eventually written to the output
     * stream. It is used to transfer the values from the fields of the object
     * that is currently being written to the persistent fields.
     *
     * @return the PutField object from which persistent fields can be accessed
     *         by name.
     * @throws IOException
     *             if an I/O error occurs.
     * @throws NotActiveException
     *             if this method is not called from {@code WriteObject()}.
     * @see ObjectInputStream#defaultReadObject
     */
    CARAPI PutFields(
        /* [out] */ IObjectOutputStreamPutField** result) /*throws IOException*/;

    /**
     * Resets the state of this stream. A marker is written to the stream, so
     * that the corresponding input stream will also perform a reset at the same
     * point. Objects previously written are no longer remembered, so they will
     * be written again (instead of a cyclical reference) if found in the object
     * graph.
     *
     * @throws IOException
     *             if {@code reset()} is called during the serialization of an
     *             object.
     */
    CARAPI Reset() /*throws IOException*/;

    /**
     * Sets the specified protocol version to be used by this stream.
     *
     * @param version
     *            the protocol version to be used. Use a {@code
     *            PROTOCOL_VERSION_x} constant from {@code
     *            java.io.ObjectStreamConstants}.
     * @throws IllegalArgumentException
     *             if an invalid {@code version} is specified.
     * @throws IOException
     *             if an I/O error occurs.
     * @see ObjectStreamConstants#PROTOCOL_VERSION_1
     * @see ObjectStreamConstants#PROTOCOL_VERSION_2
     */
    CARAPI UseProtocolVersion(
        /* [in] */ Int32 version) /*throws IOException*/;

    /**
     * Writes {@code count} bytes from the Byte array {@code buffer} starting at
     * offset {@code index} to the target stream. Blocks until all bytes are
     * written.
     *
     * @param buffer
     *            the buffer to write.
     * @param offset
     *            the index of the first Byte in {@code buffer} to write.
     * @param length
     *            the number of bytes from {@code buffer} to write to the output
     *            stream.
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     */
    // @Override
    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length) /*throws IOException*/;

    /**
     * Writes a single Byte to the target stream. Only the least significant
     * Byte of the integer {@code value} is written to the stream. Blocks until
     * the Byte is actually written.
     *
     * @param value
     *            the Byte to write.
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     */
    // @Override
    CARAPI Write(
        /* [in] */ Int32 value) /*throws IOException*/;

    /**
     * Writes a Boolean to the target stream.
     *
     * @param value
     *            the Boolean value to write to the target stream.
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     */
    CARAPI WriteBoolean(
        /* [in] */ Boolean value) /*throws IOException*/;

    /**
     * Writes a Byte (8 bit) to the target stream.
     *
     * @param value
     *            the Byte to write to the target stream.
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     */
    CARAPI WriteByte(
        /* [in] */ Int32 value) /*throws IOException*/;

    /**
     * Writes the string {@code value} as a sequence of bytes to the target
     * stream. Only the least significant Byte of each character in the string
     * is written.
     *
     * @param value
     *            the string to write to the target stream.
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     */
    CARAPI WriteBytes(
        /* [in] */ const String& value) /*throws IOException*/;

    /**
     * Writes a character (16 bit) to the target stream.
     *
     * @param value
     *            the character to write to the target stream.
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     */
    CARAPI WriteChar(
        /* [in] */ Int32 value) /*throws IOException*/;

    /**
     * Writes the string {@code value} as a sequence of characters to the target
     * stream.
     *
     * @param value
     *            the string to write to the target stream.
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     */
    CARAPI WriteChars(
        /* [in] */ const String& value) /*throws IOException*/;

    /**
     * Writes a Double (64 bit) to the target stream.
     *
     * @param value
     *            the Double to write to the target stream.
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     */
    CARAPI WriteDouble(
        /* [in] */ Double value) /*throws IOException*/;

    /**
     * Writes the fields of the object currently being written to the target
     * stream. The field values are buffered in the currently active {@code
     * PutField} object, which can be accessed by calling {@code putFields()}.
     *
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     * @throws NotActiveException
     *             if there are no fields to write to the target stream.
     * @see #putFields
     */
    CARAPI WriteFields() /*throws IOException*/;

    /**
     * Writes a Float (32 bit) to the target stream.
     *
     * @param value
     *            the Float to write to the target stream.
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     */
    CARAPI WriteFloat(
        /* [in] */ Float value) /*throws IOException*/;

    /**
     * Writes an integer (32 bit) to the target stream.
     *
     * @param value
     *            the integer to write to the target stream.
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     */
    CARAPI WriteInt32(
        /* [in] */ Int32 value) /*throws IOException*/;

    /**
     * Writes a Int64 (64 bit) to the target stream.
     *
     * @param value
     *            the Int64 to write to the target stream.
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     */
    CARAPI WriteInt64(
        /* [in] */ Int64 value) /*throws IOException*/;

    /**
     * Writes an object to the target stream.
     *
     * @param object
     *            the object to write to the target stream.
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     * @see ObjectInputStream#readObject()
     */
    CARAPI WriteObject(
        /* [in] */ IInterface* object) /*throws IOException*/;

    /**
     * Writes an unshared object to the target stream. This method is identical
     * to {@code WriteObject}, except that it always writes a new object to the
     * stream versus the use of back-referencing for identical objects by
     * {@code WriteObject}.
     *
     * @param object
     *            the object to write to the target stream.
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     * @see ObjectInputStream#readUnshared()
     */
    CARAPI WriteUnshared(
        /* [in] */ IInterface* object) /*throws IOException*/;

    /**
     * Writes a Int16 (16 bit) to the target stream.
     *
     * @param value
     *            the Int16 to write to the target stream.
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     */
    CARAPI WriteInt16(
        /* [in] */ Int32 value) /*throws IOException*/;

    /**
     * Writes a string encoded with {@link DataInput modified UTF-8} to the
     * target stream.
     *
     * @param value
     *            the string to write to the target stream.
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     */
    CARAPI WriteUTF(
        /* [in] */ const String& value) /*throws IOException*/;

    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* buffer);

protected:
    /**
     * Writes optional information for class {@code aClass} to the output
     * stream. This optional data can be read when deserializing the class
     * descriptor (ObjectStreamClass) for this class from an input stream. By
     * default, no extra data is saved.
     *
     * @param aClass
     *            the class to annotate.
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     * @see ObjectInputStream#resolveClass(ObjectStreamClass)
     */
    virtual CARAPI AnnotateClass(
        /* [in] */ const ClassID& aClass) /*throws IOException*/;

    /**
     * Writes optional information for a proxy class to the target stream. This
     * optional data can be read when deserializing the proxy class from an
     * input stream. By default, no extra data is saved.
     *
     * @param aClass
     *            the proxy class to annotate.
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     * @see ObjectInputStream#resolveProxyClass(String[])
     */
    virtual CARAPI AnnotateProxyClass(
        /* [in] */ const ClassID& aClass) /*throws IOException*/;

    /**
     * Writes buffered data to the target stream. This is similar to {@code
     * Flush} but the Flush is not propagated to the target stream.
     *
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     */
    virtual CARAPI Drain() /*throws IOException*/;

    /**
     * Enables object replacement for this stream. By default this is not
     * enabled. Only trusted subclasses (loaded with system class loader) are
     * allowed to change this status.
     *
     * @param enable
     *            {@code TRUE} to enable object replacement; {@code FALSE} to
     *            disable it.
     * @return the previous setting.
     * @see #replaceObject
     * @see ObjectInputStream#enableResolveObject
     */
    virtual CARAPI_(Boolean) EnableReplaceObject(
        /* [in] */ Boolean enable);

    /**
     * Allows trusted subclasses to substitute the specified original {@code
     * object} with a new object. Object substitution has to be activated first
     * with calling {@code EnableReplaceObject(TRUE)}. This implementation just
     * returns {@code object}.
     *
     * @param object
     *            the original object for which a replacement may be defined.
     * @return the replacement object for {@code object}.
     * @throws IOException
     *             if any I/O error occurs while creating the replacement
     *             object.
     * @see #EnableReplaceObject
     * @see ObjectInputStream#enableResolveObject
     * @see ObjectInputStream#resolveObject
     */
    virtual CARAPI ReplaceObject(
        /* [in] */ IInterface* object,
        /* [out] */ IInterface** result) /*throws IOException*/;

    /**
     * Writes a class descriptor to the target stream.
     *
     * @param classDesc
     *            the class descriptor to write to the target stream.
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     */
    virtual CARAPI WriteClassDescriptor(
        /* [in] */ IObjectStreamClass* classDesc) /*throws IOException*/;

    /**
     * Method to be overridden by subclasses to write {@code object} to the
     * target stream.
     *
     * @param object
     *            the object to write to the target stream.
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     */
    virtual CARAPI WriteObjectOverride(
        /* [in] */ IInterface* object) /*throws IOException*/;

    /**
     * Writes the {@link ObjectOutputStream} header to the target stream.
     *
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     */
    virtual CARAPI WriteStreamHeader() /*throws IOException*/;

private:
    /**
     * Do the necessary work to see if the receiver can be used to write
     * primitive types like Int32, Char32, etc.
     */
    void CheckWritePrimitiveTypes();

    /**
     * Computes the collection of emulated fields that users can manipulate to
     * store a representation different than the one declared by the class of
     * the object being dumped.
     *
     * @see #WriteFields
     * @see #WriteFieldValues(EmulatedFieldsForDumping)
     */
    CARAPI_(void) ComputePutField();

    /**
     * Dumps the parameter {@code obj} only if it is {@code NULL}
     * or an object that has already been dumped previously.
     *
     * @param obj
     *            Object to check if an instance previously dumped by this
     *            stream.
     * @return -1 if it is an instance which has not been dumped yet (and this
     *         method does nothing). The handle if {@code obj} is an
     *         instance which has been dumped already.
     *
     * @throws IOException
     *             If an error occurs attempting to save {@code NULL} or
     *             a cyclic reference.
     */
    CARAPI DumpCycle(
        /* [in] */ IInterface* obj,
        /* [out] */ Int32* result) /*throws IOException*/;

    /**
     * Return the next handle to be used to indicate cyclic
     * references being saved to the stream.
     *
     * @return the next handle to represent the next cyclic reference
     */
    CARAPI_(Int32) NextHandle();

    CARAPI_(Int32) RegisterObjectWritten(
        /* [in] */ IInterface* obj);

    /**
     * Remove the unshared object from the table, and restore any previous
     * handle.
     *
     * @param obj
     *            Non-NULL object being dumped.
     * @param previousHandle
     *            The handle of the previous identical object dumped
     */
    CARAPI_(void) RemoveUnsharedReference(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 previousHandle);

    /**
     * Reset the collection of objects already dumped by the receiver. If the
     * objects are found again in the object graph, the receiver will dump them
     * again, instead of a handle (cyclic reference).
     *
     */
    CARAPI_(void) ResetSeenObjects();

    /**
     * Reset the receiver. The collection of objects already dumped by the
     * receiver is reset, and internal structures are also reset so that the
     * receiver knows it is in a fresh clean state.
     *
     */
    CARAPI_(void) ResetState();

    /**
     * Write a class descriptor {@code classDesc} (an
     * {@code ObjectStreamClass}) to the stream.
     *
     * @param classDesc
     *            The class descriptor (an {@code ObjectStreamClass}) to
     *            be dumped
     * @param unshared
     *            Write the object unshared
     * @return the handle assigned to the class descriptor
     *
     * @throws IOException
     *             If an IO exception happened when writing the class
     *             descriptor.
     */
    CARAPI WriteClassDesc(
        /* [in] */ IObjectStreamClass* classDesc,
        /* [in] */ Boolean unshared,
        /* [out] */ Int32* result) /*throws IOException*/;

    /**
     * Writes a handle representing a cyclic reference (object previously
     * dumped).
     */
    CARAPI WriteCyclicReference(
        /* [in] */ Int32 handle) /*throws IOException*/;

    /**
     * Writes a collection of field descriptors (name, type name, etc) for the
     * class descriptor {@code classDesc} (an
     * {@code ObjectStreamClass})
     *
     * @param classDesc
     *            The class descriptor (an {@code ObjectStreamClass})
     *            for which to write field information
     * @param externalizable
     *            TRUE if the descriptors are externalizable
     *
     * @throws IOException
     *             If an IO exception happened when writing the field
     *             descriptors.
     *
     * @see #WriteObject(Object)
     */
    CARAPI WriteFieldDescriptors(
        /* [in] */ IObjectStreamClass* classDesc,
        /* [in] */ Boolean externalizable) /*throws IOException*/;

    /**
     * Writes a collection of field values for the emulated fields
     * {@code emulatedFields}
     *
     * @param emulatedFields
     *            an {@code EmulatedFieldsForDumping}, concrete subclass
     *            of {@code PutField}
     *
     * @throws IOException
     *             If an IO exception happened when writing the field values.
     *
     * @see #WriteFields
     * @see #WriteObject(Object)
     */
    CARAPI WriteFieldValues(
        /* [in] */ IEmulatedFieldsForDumping* emulatedFields) /*throws IOException*/;

    /**
     * Writes a collection of field values for the fields described by class
     * descriptor {@code classDesc} (an {@code ObjectStreamClass}).
     * This is the default mechanism, when emulated fields (an
     * {@code PutField}) are not used. Actual values to dump are fetched
     * directly from object {@code obj}.
     *
     * @param obj
     *            Instance from which to fetch field values to dump.
     * @param classDesc
     *            A class descriptor (an {@code ObjectStreamClass})
     *            defining which fields should be dumped.
     *
     * @throws IOException
     *             If an IO exception happened when writing the field values.
     *
     * @see #WriteObject(Object)
     */
    CARAPI WriteFieldValues(
        /* [in] */ IInterface* obj,
        /* [in] */ IObjectStreamClass* classDesc) /*throws IOException*/;

    /**
     * Walks the hierarchy of classes described by class descriptor
     * {@code classDesc} and writes the field values corresponding to
     * fields declared by the corresponding class descriptor. The instance to
     * fetch field values from is {@code object}. If the class
     * (corresponding to class descriptor {@code classDesc}) defines
     * private instance method {@code WriteObject} it will be used to
     * dump field values.
     *
     * @param object
     *            Instance from which to fetch field values to dump.
     * @param classDesc
     *            A class descriptor (an {@code ObjectStreamClass})
     *            defining which fields should be dumped.
     *
     * @throws IOException
     *             If an IO exception happened when writing the field values in
     *             the hierarchy.
     * @throws NotActiveException
     *             If the given object is not active
     *
     * @see #DefaultWriteObject
     * @see #WriteObject(Object)
     */
    CARAPI WriteHierarchy(
        /* [in] */ IInterface* object,
        /* [in] */ IObjectStreamClass* classDesc) /*throws IOException, NotActiveException*/;

    /**
     * Write array {@code array} of class {@code arrayClass} with
     * component type {@code componentType} into the receiver. It is
     * assumed the array has not been dumped yet. Returns
     * the handle for this object (array) which is dumped here.
     *
     * @param array
     *            The array object to dump
     * @param arrayClass
     *            A {@code java.lang.Class} representing the class of the
     *            array
     * @param componentType
     *            A {@code java.lang.Class} representing the array
     *            component type
     * @return the handle assigned to the array
     *
     * @throws IOException
     *             If an IO exception happened when writing the array.
     */
    CARAPI WriteNewArray(
        /* [in] */ IInterface* array,
        /* [in] */ const ClassID& arrayClass,
        /* [in] */ IObjectStreamClass* arrayClDesc,
        /* [in] */ const ClassID& componentType,
        /* [in] */ Boolean unshared,
        /* [out] */ Int32* result) /*throws IOException*/;

    /**
     * Write class {@code object} into the receiver. It is assumed the
     * class has not been dumped yet. Classes are not really dumped, but a class
     * descriptor ({@code ObjectStreamClass}) that corresponds to them.
     * Returns the handle for this object (class) which is dumped here.
     *
     * @param object
     *            The {@code java.lang.Class} object to dump
     * @return the handle assigned to the class being dumped
     *
     * @throws IOException
     *             If an IO exception happened when writing the class.
     */
    CARAPI WriteNewClass(
        /* [in] */ const ClassID& object,
        /* [in] */ Boolean unshared,
        /* [out] */ Int32* result) /*throws IOException*/;

    /**
     * Write class descriptor {@code classDesc} into the receiver. It is
     * assumed the class descriptor has not been dumped yet. The class
     * descriptors for the superclass chain will be dumped as well. Returns
     * the handle for this object (class descriptor) which is dumped here.
     *
     * @param classDesc
     *            The {@code ObjectStreamClass} object to dump
     *
     * @throws IOException
     *             If an IO exception happened when writing the class
     *             descriptor.
     */
    CARAPI WriteNewClassDesc(
        /* [in] */ IObjectStreamClass* classDesc) /*throws IOException*/;

    /**
     * Write exception {@code ex} into the receiver. It is assumed the
     * exception has not been dumped yet. Returns
     * the handle for this object (exception) which is dumped here.
     * This is used to dump the exception instance that happened (if any) when
     * dumping the original object graph. The set of seen objects will be reset
     * just before and just after dumping this exception object.
     *
     * When exceptions are found normally in the object graph, they are dumped
     * as a regular object, and not by this method. In that case, the set of
     * "known objects" is not reset.
     *
     * @param ex
     *            Exception object to dump
     *
     * @throws IOException
     *             If an IO exception happened when writing the exception
     *             object.
     */
    CARAPI WriteNewException(
        /* [in] */ /*Exception ex*/) /*throws IOException*/;

    /**
     * Write object {@code object} of class {@code theClass} into
     * the receiver. It is assumed the object has not been dumped yet.
     * Return the handle for this object which
     * is dumped here.
     *
     * If the object implements {@code Externalizable} its
     * {@code writeExternal} is called. Otherwise, all fields described
     * by the class hierarchy is dumped. Each class can define how its declared
     * instance fields are dumped by defining a private method
     * {@code WriteObject}
     *
     * @param object
     *            The object to dump
     * @param theClass
     *            A {@code java.lang.Class} representing the class of the
     *            object
     * @param unshared
     *            Write the object unshared
     * @return the handle assigned to the object
     *
     * @throws IOException
     *             If an IO exception happened when writing the object.
     */
    CARAPI WriteNewObject(
        /* [in] */ IInterface* object,
        /* [in] */ const ClassID theClass,
        /* [in] */ IObjectStreamClass* clDesc,
        /* [in] */ Boolean unshared,
        /* [out] */ Int32* result) /*throws IOException*/;

    /**
     * Write String {@code object} into the receiver. It is assumed the
     * String has not been dumped yet. Returns the handle for this object (String) which is dumped here.
     * Strings are saved encoded with {@link DataInput modified UTF-8}.
     *
     * @param object
     *            the string to dump.
     * @return the handle assigned to the String being dumped
     *
     * @throws IOException
     *             If an IO exception happened when writing the String.
     */
    CARAPI WriteNewString(
        /* [in] */ const String& object,
        /* [in] */ Boolean unshared,
        /* [out] */ Int32* result) /*throws IOException*/;

    /**
     * Write a special tag that indicates the value {@code NULL} into the
     * receiver.
     *
     * @throws IOException
     *             If an IO exception happened when writing the tag for
     *             {@code NULL}.
     */
    CARAPI WriteNull() /*throws IOException*/;

    CARAPI WriteObject(
        /* [in] */ IInterface* object,
        /* [in] */ Boolean unshared) /*throws IOException*/;

    /**
     * Write object {@code object} into the receiver's underlying stream.
     *
     * @param object
     *            The object to write
     * @param unshared
     *            Write the object unshared
     * @param computeClassBasedReplacement
     *            A Boolean indicating if class-based replacement should be
     *            computed (if supported) for the object.
     * @param computeStreamReplacement
     *            A Boolean indicating if stream-based replacement should be
     *            computed (if supported) for the object.
     * @return the handle assigned to the final object being dumped
     *
     * @throws IOException
     *             If an IO exception happened when writing the object
     *
     * @see ObjectInputStream#readObject()
     */
    CARAPI WriteObjectInternal(
        /* [in] */ IInterface* object,
        /* [in] */ Boolean unshared,
        /* [in] */ Boolean computeClassBasedReplacement,
        /* [in] */ Boolean computeStreamReplacement,
        /* [out] */ Int32* result) /*throws IOException*/;

    // write for Enum Class Desc only, which is different from other classes
    CARAPI WriteEnumDesc(
        /* [in] */ IObjectStreamClass* classDesc,
        /* [in] */ Boolean unshared,
        /* [out] */ IObjectStreamClass** result) /*throws IOException*/;

    CARAPI WriteNewEnum(
        /* [in] */ IInterface* object,
        /* [in] */ const ClassID& theClass,
        /* [in] */ Boolean unshared,
        /* [out] */ Int32* result) /*throws IOException*/;

private:
    /*
     * Mask to zero SC_BLOC_DATA bit.
     */
    static const Byte NOT_SC_BLOCK_DATA;

    /*
     * How many nested levels to WriteObject.
     */
    Int32 mNestedLevels;

    /*
     * Where we write to
     */
    AutoPtr<IDataOutputStream> mOutput;

    /*
     * If object replacement is enabled or not
     */
    Boolean mEnableReplace;

    /*
     * Where we write primitive types to
     */
    AutoPtr<IDataOutputStream> mPrimitiveTypes;

    /*
     * Where the write primitive types are actually written to
     */
    AutoPtr<IByteArrayOutputStream> mPrimitiveTypesBuffer;

    /*
     * Table mapping Object -> Integer (handle)
     */
    AutoPtr<ISerializationHandleMap> mObjectsWritten;

    /*
     * All objects are assigned an ID (integer handle)
     */
    Int32 mCurrentHandle;

    /*
     * Used by DefaultWriteObject
     */
    AutoPtr<IInterface> mCurrentObject;

    /*
     * Used by DefaultWriteObject
     */
    AutoPtr<IObjectStreamClass> mCurrentClass;

    /*
     * Either ObjectStreamConstants.PROTOCOL_VERSION_1 or
     * ObjectStreamConstants.PROTOCOL_VERSION_2
     */
    Int32 mProtocolVersion;

    /*
     * Used to keep track of the PutField object for the class/object being
     * written
     */
    AutoPtr<IEmulatedFieldsForDumping> mCurrentPutField;

    /*
     * Allows the receiver to decide if it needs to call WriteObjectOverride
     */
    Boolean mSubclassOverridingImplementation;

    /*
     * Descriptor for java.lang.reflect.Proxy
     */
    AutoPtr<IObjectStreamClass> mProxyClassDesc;
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_COBJECTOUTPUTSTREAM_H__
