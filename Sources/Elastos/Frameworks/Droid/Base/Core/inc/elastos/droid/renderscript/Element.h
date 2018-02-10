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

#ifndef __ELASTOS_DROID_RENDERSCRIPT_ELEMENT_H__
#define __ELASTOS_DROID_RENDERSCRIPT_ELEMENT_H__

#include "elastos/droid/renderscript/BaseObj.h"
#include "elastos/droid/renderscript/RenderScript.h"

namespace Elastos {
namespace Droid {
namespace RenderScript {

class Element
    : public BaseObj
    , public IElement
{
public:
    class DataType : public Object
    {
    public:
        static CARAPI_(AutoPtr<DataType>) GetDataType(
            /* [in] */ Int32 id);

    private:
        DataType(
            /* [in] */ Int32 id,
            /* [in] */ Int32 size);

        DataType(
            /* [in] */ Int32 id);

    public:
        static AutoPtr<DataType> NONE;
        // static AutoPtr<DataType> FLOAT_16,
        static AutoPtr<DataType> FLOAT_32;
        static AutoPtr<DataType> FLOAT_64;
        static AutoPtr<DataType> SIGNED_8;
        static AutoPtr<DataType> SIGNED_16;
        static AutoPtr<DataType> SIGNED_32;
        static AutoPtr<DataType> SIGNED_64;
        static AutoPtr<DataType> UNSIGNED_8;
        static AutoPtr<DataType> UNSIGNED_16;
        static AutoPtr<DataType> UNSIGNED_32;
        static AutoPtr<DataType> UNSIGNED_64;

        static AutoPtr<DataType> BOOLEAN;

        static AutoPtr<DataType> UNSIGNED_5_6_5;
        static AutoPtr<DataType> UNSIGNED_5_5_5_1;
        static AutoPtr<DataType> UNSIGNED_4_4_4_4;

        static AutoPtr<DataType> MATRIX_4X4;
        static AutoPtr<DataType> MATRIX_3X3;
        static AutoPtr<DataType> MATRIX_2X2;

        static AutoPtr<DataType> RS_ELEMENT;
        static AutoPtr<DataType> RS_TYPE;
        static AutoPtr<DataType> RS_ALLOCATION;
        static AutoPtr<DataType> RS_SAMPLER;
        static AutoPtr<DataType> RS_SCRIPT;
        static AutoPtr<DataType> RS_MESH;
        static AutoPtr<DataType> RS_PROGRAM_FRAGMENT;
        static AutoPtr<DataType> RS_PROGRAM_VERTEX;
        static AutoPtr<DataType> RS_PROGRAM_RASTER;
        static AutoPtr<DataType> RS_PROGRAM_STORE;
        static AutoPtr<DataType> RS_FONT;

        Int32 mID;
        Int32 mSize;
    };

public:
    Element();

    virtual ~Element() {}

    CAR_INTERFACE_DECL()

    /**
    * @return element size in bytes
    */
    CARAPI GetBytesSize(
        /* [out] */ Int32* value);

    /**
    * Returns the number of vector components. 2 for float2, 4 for
    * float4, etc.
    * @return element vector size
    */
    CARAPI GetVectorSize(
        /* [out] */ Int32* value);

    /**
     * Return if a element is too complex for use as a data source for a Mesh or
     * a Program.
     *
     * @return boolean
     */
    CARAPI IsComplex(
        /* [out] */ Boolean* isComplex);

    /**
    * Elements could be simple, such as an int or a float, or a
    * structure with multiple sub elements, such as a collection of
    * floats, float2, float4. This function returns zero for simple
    * elements or the number of sub-elements otherwise.
    * @return number of sub-elements in this element
    */
    CARAPI GetSubElementCount(
        /* [out] */ Int32* value);

    /**
    * For complex elements, this function will return the
    * sub-element at index
    * @param index index of the sub-element to return
    * @return sub-element in this element at given index
    */
    CARAPI GetSubElement(
        /* [in] */ Int32 index,
        /* [out] */ IElement** e);

    /**
    * For complex elements, this function will return the
    * sub-element name at index
    * @param index index of the sub-element
    * @return sub-element in this element at given index
    */
    CARAPI GetSubElementName(
        /* [in] */ Int32 index,
        /* [out] */ String* name);

    /**
    * For complex elements, some sub-elements could be statically
    * sized arrays. This function will return the array size for
    * sub-element at index
    * @param index index of the sub-element
    * @return array size of sub-element in this element at given index
    */
    CARAPI GetSubElementArraySize(
        /* [in] */ Int32 index,
        /* [out] */ Int32* value);

    /**
    * This function specifies the location of a sub-element within
    * the element
    * @param index index of the sub-element
    * @return offset in bytes of sub-element in this element at given index
    */
    CARAPI GetSubElementOffsetBytes(
        /* [in] */ Int32 index,
        /* [out] */ Int32* value);

    /**
    * @return element data type
    */
    CARAPI GetDataType(
        /* [out] */ ElementDataType* dt);

    /**
    * @return element data kind
    */
    CARAPI GetDataKind(
        /* [out] */ ElementDataKind* dk);

    /**
     * Utility function for returning an Element containing a single Boolean.
     *
     * @param rs Context to which the element will belong.
     *
     * @return Element
     */
    CARAPI_(AutoPtr<IElement>) BOOLEAN(
        /* [in] */ RenderScript* rs);

    /**
     * Utility function for returning an Element containing a single UNSIGNED_8.
     *
     * @param rs Context to which the element will belong.
     *
     * @return Element
     */
    CARAPI_(AutoPtr<IElement>) U8(
        /* [in] */ RenderScript* rs);

    /**
     * Utility function for returning an Element containing a single SIGNED_8.
     *
     * @param rs Context to which the element will belong.
     *
     * @return Element
     */
    CARAPI_(AutoPtr<IElement>) I8(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) U16(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) I16(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) U32(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) I32(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) U64(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) I64(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) F32(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) F64(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) ELEMENT(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) TYPE(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) ALLOCATION(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) SAMPLER(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) SCRIPT(
        /* [in] */ IRenderScript* rs);

    CARAPI_(AutoPtr<IElement>) MESH(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) PROGRAM_FRAGMENT(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) PROGRAM_VERTEX(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) PROGRAM_RASTER(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) PROGRAM_STORE(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) FONT(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) A_8(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) RGB_565(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) RGB_888(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) RGBA_5551(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) RGBA_4444(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) RGBA_8888(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) F32_2(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) F32_3(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) F32_4(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) F64_2(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) F64_3(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) F64_4(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) U8_2(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) U8_3(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) U8_4(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) I8_2(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) I8_3(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) I8_4(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) U16_2(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) U16_3(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) U16_4(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) I16_2(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) I16_3(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) I16_4(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) U32_2(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) U32_3(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) U32_4(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) I32_2(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) I32_3(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) I32_4(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) U64_2(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) U64_3(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) U64_4(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) I64_2(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) I64_3(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) I64_4(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) YUV(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) MATRIX_4X4(
        /* [in] */ RenderScript* rs);

    /** @deprecated use MATRIX_4X4
    */
    CARAPI_(AutoPtr<IElement>) MATRIX4X4(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) MATRIX_3X3(
        /* [in] */ RenderScript* rs);

    CARAPI_(AutoPtr<IElement>) MATRIX_2X2(
        /* [in] */ RenderScript* rs);

    /**
     * Create a custom vector element of the specified DataType and vector size.
     * DataKind will be set to USER. Only primitive types (FLOAT_32, FLOAT_64,
     * SIGNED_8, SIGNED_16, SIGNED_32, SIGNED_64, UNSIGNED_8, UNSIGNED_16,
     * UNSIGNED_32, UNSIGNED_64, BOOLEAN) are supported.
     *
     * @param rs The context associated with the new Element.
     * @param dt The DataType for the new Element.
     * @param size Vector size for the new Element.  Range 2-4 inclusive
     *             supported.
     *
     * @return Element
     */
    CARAPI CreateVector(
        /* [in] */ RenderScript* rs,
        /* [in] */ HANDLE dt,
        /* [in] */ Int32 size,
        /* [out] */ IElement** e);

    /**
     * Create a new pixel Element type.  A matching DataType and DataKind must
     * be provided.  The DataType and DataKind must contain the same number of
     * components.  Vector size will be set to 1.
     *
     * @param rs The context associated with the new Element.
     * @param dt The DataType for the new element.
     * @param dk The DataKind to specify the mapping of each component in the
     *           DataType.
     *
     * @return Element
     */
    CARAPI CreatePixel(
        /* [in] */ RenderScript* rs,
        /* [in] */ HANDLE dt,
        /* [in] */ ElementDataKind dk,
        /* [out] */ IElement** e);

    /**
     * Check if the current Element is compatible with another Element.
     * Primitive Elements are compatible if they share the same underlying
     * size and type (i.e. U8 is compatible with A_8). User-defined Elements
     * must be equal in order to be compatible. This requires strict name
     * equivalence for all sub-Elements (in addition to structural equivalence).
     *
     * @param e The Element to check compatibility with.
     *
     * @return boolean true if the Elements are compatible, otherwise false.
     */
    CARAPI IsCompatible(
        /* [in] */ IElement* e,
        /* [out] */ Boolean* isCompatible);

protected:
    CARAPI constructor(
        /* [in] */ Int64 id,
        /* [in] */ RenderScript* rs,
        /* [in] */ ArrayOf<IElement*>* e,
        /* [in] */ ArrayOf<String>* n,
        /* [in] */ ArrayOf<Int32>* as);

    CARAPI constructor(
        /* [in] */ Int64 id,
        /* [in] */ RenderScript* rs,
        /* [in] */ DataType* dt,
        /* [in] */ ElementDataKind dk,
        /* [in] */ Boolean norm,
        /* [in] */ Int32 size);

    CARAPI constructor(
        /* [in] */ Int64 id,
        /* [in] */ RenderScript* rs);

    // @Override
    CARAPI_(void) UpdateFromNative();

    /**
     * Create a custom Element of the specified DataType.  The DataKind will be
     * set to USER and the vector size to 1 indicating non-vector.
     *
     * @param rs The context associated with the new Element.
     * @param dt The DataType for the new element.
     * @return Element
     */
    static CARAPI_(AutoPtr<IElement>) CreateUser(
        /* [in] */ RenderScript* rs,
        /* [in] */ DataType* dt);

private:
    CARAPI_(void) UpdateVisibleSubElements();

protected:
    Int32 mSize;
    AutoPtr<ArrayOf<IElement*> > mElements;
    AutoPtr<ArrayOf<String> > mElementNames;
    AutoPtr<ArrayOf<Int32> > mArraySizes;
    AutoPtr<ArrayOf<Int32> > mOffsetInBytes;

    AutoPtr<ArrayOf<Int32> > mVisibleElementMap;

    AutoPtr<DataType> mType;
    ElementDataKind mKind;
    Boolean mNormalized;
    Int32 mVectorSize;
};

} // namespace RenderScript
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_RENDERSCRIPT_ELEMENT_H__
