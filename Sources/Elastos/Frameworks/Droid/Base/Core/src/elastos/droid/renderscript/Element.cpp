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

#include "elastos/droid/RenderScript/Element.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::RenderScript::EIID_IElement;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace RenderScript {

//==========================================================
// Element::DataType
//==========================================================

static AutoPtr<DataType> InitDataType(
    /* [in] */ Int32 id,
    /* [in] */ Int32 size)
{
    return new DataType(id, size);
}
AutoPtr<DataType> Element::DataType::NONE = InitDataType(0, 0);
AutoPtr<DataType> Element::DataType::FLOAT_32 = InitDataType(2, 4);
AutoPtr<DataType> Element::DataType::FLOAT_64 = InitDataType(3, 8);
AutoPtr<DataType> Element::DataType::SIGNED_8 = InitDataType(4, 1);
AutoPtr<DataType> Element::DataType::SIGNED_16 = InitDataType(5, 2);
AutoPtr<DataType> Element::DataType::SIGNED_32 = InitDataType(6, 4);
AutoPtr<DataType> Element::DataType::SIGNED_64 = InitDataType(7, 8);
AutoPtr<DataType> Element::DataType::UNSIGNED_8 = InitDataType(8, 1);
AutoPtr<DataType> Element::DataType::UNSIGNED_16 = InitDataType(9, 2);
AutoPtr<DataType> Element::DataType::UNSIGNED_32 = InitDataType(10, 4);
AutoPtr<DataType> Element::DataType::UNSIGNED_64 = InitDataType(11, 8);
AutoPtr<DataType> Element::DataType::BOOLEAN = (12, 1);
AutoPtr<DataType> Element::DataType::UNSIGNED_5_6_5 = InitDataType(13, 2);
AutoPtr<DataType> Element::DataType::UNSIGNED_5_5_5_1 = InitDataType(14, 2);
AutoPtr<DataType> Element::DataType::UNSIGNED_4_4_4_4 = InitDataType(15, 2);
AutoPtr<DataType> Element::DataType::MATRIX_4X4 = InitDataType(16, 64);
AutoPtr<DataType> Element::DataType::MATRIX_3X3 = InitDataType(17, 36);
AutoPtr<DataType> Element::DataType::MATRIX_2X2 = InitDataType(18, 16);

static AutoPtr<DataType> InitDataType2(
    /* [in] */ Int32 id)
{
    return new DataType(id);
}
AutoPtr<DataType> Element::DataType::RS_ELEMENT = InitDataType2(1000);
AutoPtr<DataType> Element::DataType::RS_TYPE = InitDataType2(1001);
AutoPtr<DataType> Element::DataType::RS_ALLOCATION = InitDataType2(1002);
AutoPtr<DataType> Element::DataType::RS_SAMPLER = InitDataType2(1003);
AutoPtr<DataType> Element::DataType::RS_SCRIPT = InitDataType2(1004);
AutoPtr<DataType> Element::DataType::RS_MESH = InitDataType2(1005);
AutoPtr<DataType> Element::DataType::RS_PROGRAM_FRAGMENT = InitDataType2(1006);
AutoPtr<DataType> Element::DataType::RS_PROGRAM_VERTEX = InitDataType2(1007);
AutoPtr<DataType> Element::DataType::RS_PROGRAM_RASTER = InitDataType2(1008);
AutoPtr<DataType> Element::DataType::RS_PROGRAM_STORE = InitDataType2(1009);
AutoPtr<DataType> Element::DataType::RS_FONT = InitDataType2(1010);

Element::DataType::DataType(
    /* [in] */ Int32 id,
    /* [in] */ Int32 size)
    : mID(id)
    , mSize(size)
{}

Element::DataType::DataType(
    /* [in] */ Int32 id)
    : mID(id)
    , mSize(4)
{
    if (RenderScript::sPointerSize == 8) {
        mSize = 32;
    }
}

AutoPtr<DataType> Element::DataType::GetDataType(
    /* [in] */ Int32 id)
{
    switch(id) {
        case ElementDataType_NONE:
            return NONE;
        case ElementDataType_FLOAT_32:
            return FLOAT_32;
        case ElementDataType_FLOAT_64:
            return FLOAT_64;
        case ElementDataType_SIGNED_8:
            return SIGNED_8;
        case ElementDataType_SIGNED_16:
            return SIGNED_16;
        case ElementDataType_SIGNED_32:
            return SIGNED_32;
        case ElementDataType_SIGNED_64:
            return SIGNED_64;
        case ElementDataType_UNSIGNED_8:
            return UNSIGNED_8;
        case ElementDataType_UNSIGNED_16:
            return UNSIGNED_16;
        case ElementDataType_UNSIGNED_32:
            return UNSIGNED_32;
        case ElementDataType_UNSIGNED_64:
            return UNSIGNED_64;
        case ElementDataType_BOOLEAN:
            return BOOLEAN;
        case ElementDataType_UNSIGNED_5_6_5:
            return UNSIGNED_5_6_5;
        case ElementDataType_UNSIGNED_5_5_5_1:
            return UNSIGNED_5_5_5_1;
        case ElementDataType_UNSIGNED_4_4_4_4:
            return UNSIGNED_4_4_4_4;
        case ElementDataType_MATRIX_4X4:
            return MATRIX_4X4;
        case ElementDataType_MATRIX_3X3:
            return MATRIX_3X3;
        case ElementDataType_MATRIX_2X2:
            return MATRIX_2X2;
        case ElementDataType_RS_ELEMENT:
            return RS_ELEMENT;
        case ElementDataType_RS_TYPE:
            return RS_TYPE;
        case ElementDataType_RS_ALLOCATION:
            return RS_ALLOCATION;
        case ElementDataType_RS_SAMPLER:
            return RS_SAMPLER;
        case ElementDataType_RS_SCRIPT:
            return RS_SCRIPT;
        case ElementDataType_RS_MESH:
            return RS_MESH;
        case ElementDataType_RS_PROGRAM_FRAGMENT:
            return RS_PROGRAM_FRAGMENT;
        case ElementDataType_RS_PROGRAM_VERTEX:
            return RS_PROGRAM_VERTEX;
        case ElementDataType_RS_PROGRAM_RASTER:
            return RS_PROGRAM_RASTER;
        case ElementDataType_RS_PROGRAM_STORE:
            return RS_PROGRAM_STORE;
        case ElementDataType_RS_FONT:
            return RS_FONT;
        default:
            return NULL;
    }
}


//==========================================================
// Element
//==========================================================

Element::Element()
    : mSize(0)
    , mNormalized(FALSE)
    , mVectorSize(0)
{}

CAR_INTERFACE_IMPL(Element, BaseObj, IElement)

void Element::UpdateVisibleSubElements()
{
    if (mElements == NULL) {
        return;
    }

    Int32 noPaddingFieldCount = 0;
    Int32 fieldCount = mElementNames->GetLength();
    // Find out how many elements are not padding
    for (Int32 ct = 0; ct < fieldCount; ct ++) {
        if ((*mElementNames)[ct].GetChar(0) != '#') {
            noPaddingFieldCount ++;
        }
    }
    mVisibleElementMap = ArrayOf<Int32>::Alloc(noPaddingFieldCount);

    // Make a map that points us at non-padding elements
    for (Int32 ct = 0, ctNoPadding = 0; ct < fieldCount; ct ++) {
        if ((*mElementNames)[ct].GetChar(0) != '#') {
            (*mVisibleElementMap)[ctNoPadding ++] = ct;
        }
    }
}

ECode Element::GetBytesSize(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mSize;
    return NOERROR;
}

ECode Element::GetVectorSize(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mVectorSize;
    return NOERROR;
}

ECode Element::IsComplex(
    /* [out] */ Boolean* isComplex)
{
    VALIDATE_NOT_NULL(isComplex)
    if (mElements == NULL) {
        *isComplex = FALSE;
        return NOERROR;
    }
    for (Int32 ct = 0; ct < mElements->GetLength(); ct++) {
        AutoPtr<Element> e = (Element*)(*mElements)[ct];
        if (e->mElements != NULL) {
            *isComplex = TRUE;
            return NOERROR;
        }
    }
    *isComplex = FALSE;
    return NOERROR;
}

ECode Element::GetSubElementCount(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    if (mVisibleElementMap == NULL) {
        *value = 0;
        return NOERROR;
    }
    *value = mVisibleElementMap->GetLength();
    return NOERROR;
}

ECode Element::GetSubElement(
    /* [in] */ Int32 index,
    /* [out] */ IElement** e)
{
    VALIDATE_NOT_NULL(e)
    *e = NULL;
    if (mVisibleElementMap == NULL) {
        Slogger::E("Element", "Element contains no sub-elements");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (index < 0 || index >= mVisibleElementMap->GetLength()) {
        Slogger::E("Element", "Illegal sub-element index");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *e = (*mElements)[(*mVisibleElementMap)[index]];
    REFCOUNT_ADD(*e)
    return NOERROR;
}

ECode Element::GetSubElementName(
    /* [in] */ Int32 index,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = String(NULL);
    if (mVisibleElementMap == NULL) {
        Slogger::E("Element", "Element contains no sub-elements");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (index < 0 || index >= mVisibleElementMap->GetLength()) {
        Slogger::E("Element", "Illegal sub-element index");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *name = (*mElementNames)[(*mVisibleElementMap)[index]];
    return NOERROR;
}

ECode Element::GetSubElementArraySize(
    /* [in] */ Int32 index,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;
    if (mVisibleElementMap == NULL) {
        Slogger::E("Element", "Element contains no sub-elements");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (index < 0 || index >= mVisibleElementMap->GetLength()) {
        Slogger::E("Element", "Illegal sub-element index");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *value = (*mArraySizes)[(*mVisibleElementMap)[index]];
    return NOERROR;
}

ECode Element::GetSubElementOffsetBytes(
    /* [in] */ Int32 index,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;
    if (mVisibleElementMap == NULL) {
        Slogger::E("Element", "Element contains no sub-elements");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (index < 0 || index >= mVisibleElementMap->GetLength()) {
        Slogger::E("Element", "Illegal sub-element index");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *value = (*mOffsetInBytes)[(*mVisibleElementMap)[index]];
}

ECode Element::GetDataType(
    /* [out] */ ElementDataType* dt)
{
    VALIDATE_NOT_NULL(dt)
    *dt = mType->mID;
    return NOERROR;
}

ECode Element::GetDataKind(
    /* [out] */ ElementDataKind* dk)
{
    VALIDATE_NOT_NULL(dk)
    *dk = mKind;
    return NOERROR;
}

AutoPtr<IElement> Element::BOOLEAN(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_BOOLEAN == NULL) {
        rs->mElement_BOOLEAN = CreateUser(rs, DataType::BOOLEAN);
    }
    return rs->mElement_BOOLEAN;
}

AutoPtr<IElement> Element::U8(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_U8 == NULL) {
        rs->mElement_U8 = CreateUser(rs, DataType::UNSIGNED_8);
    }
    return rs->mElement_U8;
}

AutoPtr<IElement> Element::I8(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_I8 == NULL) {
        rs->mElement_I8 = CreateUser(rs, DataType::SIGNED_8);
    }
    return rs->mElement_I8;
}

AutoPtr<IElement> Element::U16(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_U16 == NULL) {
        rs->mElement_U16 = CreateUser(rs, DataType::UNSIGNED_16);
    }
    return rs->mElement_U16;
}

AutoPtr<IElement> Element::I16(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_I16 == NULL) {
        rs->mElement_I16 = CreateUser(rs, DataType::SIGNED_16);
    }
    return rs->mElement_I16;
}

AutoPtr<IElement> Element::U32(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_U32 == NULL) {
        rs->mElement_U32 = CreateUser(rs, DataType::UNSIGNED_32);
    }
    return rs->mElement_U32;
}

AutoPtr<IElement> Element::I32(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_I32 == NULL) {
        rs->mElement_I32 = CreateUser(rs, DataType::SIGNED_32);
    }
    return rs->mElement_I32;
}

AutoPtr<IElement> Element::U64(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_U64 == NULL) {
        rs->mElement_U64 = CreateUser(rs, DataType::UNSIGNED_64);
    }
    return rs->mElement_U64;
}

AutoPtr<IElement> Element::I64(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_I64 == NULL) {
        rs->mElement_I64 = CreateUser(rs, DataType::SIGNED_64);
    }
    return rs->mElement_I64;
}

AutoPtr<IElement> Element::F32(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_F32 == NULL) {
        rs->mElement_F32 = CreateUser(rs, DataType::FLOAT_32);
    }
    return rs->mElement_F32;
}

AutoPtr<IElement> Element::F64(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_F64 == NULL) {
        rs->mElement_F64 = CreateUser(rs, DataType::FLOAT_64);
    }
    return rs->mElement_F64;
}

AutoPtr<IElement> Element::ELEMENT(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_ELEMENT == NULL) {
        rs->mElement_ELEMENT = CreateUser(rs, DataType::RS_ELEMENT);
    }
    return rs->mElement_ELEMENT;
}

AutoPtr<IElement> Element::TYPE(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_TYPE == NULL) {
        rs->mElement_TYPE = CreateUser(rs, DataType::RS_TYPE);
    }
    return rs->mElement_TYPE;
}

AutoPtr<IElement> Element::ALLOCATION(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_ALLOCATION == NULL) {
        rs->mElement_ALLOCATION = CreateUser(rs, DataType::RS_ALLOCATION);
    }
    return rs->mElement_ALLOCATION;
}

AutoPtr<IElement> Element::SAMPLER(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_SAMPLER == NULL) {
        rs->mElement_SAMPLER = CreateUser(rs, DataType::RS_SAMPLER);
    }
    return rs->mElement_SAMPLER;
}

AutoPtr<IElement> Element::SCRIPT(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_SCRIPT == NULL) {
        rs->mElement_SCRIPT = CreateUser(rs, DataType::RS_SCRIPT);
    }
    return rs->mElement_SCRIPT;
}

AutoPtr<IElement> Element::MESH(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_MESH == NULL) {
        rs->mElement_MESH = CreateUser(rs, DataType::RS_MESH);
    }
    return rs->mElement_MESH;
}

AutoPtr<IElement> Element::PROGRAM_FRAGMENT(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_PROGRAM_FRAGMENT == NULL) {
        rs->mElement_PROGRAM_FRAGMENT = CreateUser(rs, DataType::RS_PROGRAM_FRAGMENT);
    }
    return rs->mElement_PROGRAM_FRAGMENT;
}

AutoPtr<IElement> Element::PROGRAM_VERTEX(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_PROGRAM_VERTEX == NULL) {
        rs->mElement_PROGRAM_VERTEX = CreateUser(rs, DataType::RS_PROGRAM_VERTEX);
    }
    return rs->mElement_PROGRAM_VERTEX;
}

AutoPtr<IElement> Element::PROGRAM_RASTER(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_PROGRAM_RASTER == NULL) {
        rs->mElement_PROGRAM_RASTER = CreateUser(rs, DataType::RS_PROGRAM_RASTER);
    }
    return rs->mElement_PROGRAM_RASTER;
}

AutoPtr<IElement> Element::PROGRAM_STORE(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_PROGRAM_STORE == NULL) {
        rs->mElement_PROGRAM_STORE = CreateUser(rs, DataType::RS_PROGRAM_STORE);
    }
    return rs->mElement_PROGRAM_STORE;
}

AutoPtr<IElement> Element::FONT(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_FONT == NULL) {
        rs->mElement_FONT = CreateUser(rs, DataType::RS_FONT);
    }
    return rs->mElement_FONT;
}

AutoPtr<IElement> Element::A_8(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_A_8 == NULL) {
        rs->mElement_A_8 = CreatePixel(rs, (Handle64)DataType::UNSIGNED_8, ElementDataKind_PIXEL_A);
    }
    return rs->mElement_A_8;
}

AutoPtr<IElement> Element::RGB_565(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_RGB_565 == NULL) {
        rs->mElement_RGB_565 = CreatePixel(rs, (Handle64)DataType::UNSIGNED_5_6_5, ElementDataKind_PIXEL_RGB);
    }
    return rs->mElement_RGB_565;
}

AutoPtr<IElement> Element::RGB_888(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_RGB_888 == NULL) {
        rs->mElement_RGB_888 = CreatePixel(rs, (Handle64)DataType::UNSIGNED_8, ElementDataKind_PIXEL_RGB);
    }
    return rs->mElement_RGB_888;
}

AutoPtr<IElement> Element::RGBA_5551(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_RGBA_5551 == NULL) {
        rs->mElement_RGBA_5551 = CreatePixel(rs, (Handle64)DataType::UNSIGNED_5_5_5_1, ElementDataKind_PIXEL_RGBA);
    }
    return rs->mElement_RGBA_5551;
}


AutoPtr<IElement> Element::RGBA_4444(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_RGBA_4444 == NULL) {
        rs->mElement_RGBA_4444 = CreatePixel(rs, (Handle64)DataType::UNSIGNED_4_4_4_4, ElementDataKind_PIXEL_RGBA);
    }
    return rs->mElement_RGBA_4444;
}


AutoPtr<IElement> Element::RGBA_8888(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_RGBA_8888 == NULL) {
        rs->mElement_RGBA_8888 = CreatePixel(rs, (Handle64)DataType::UNSIGNED_8, ElementDataKind_PIXEL_RGBA);
    }
    return rs->mElement_RGBA_8888;
}

AutoPtr<IElement> Element::F32_2(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_FLOAT_2 == NULL) {
        rs->mElement_FLOAT_2 = CreateVector(rs, (Handle64)DataType::FLOAT_32, 2);
    }
    return rs->mElement_FLOAT_2;
}

AutoPtr<IElement> Element::F32_3(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_FLOAT_3 == NULL) {
        rs->mElement_FLOAT_3 = CreateVector(rs, (Handle64)DataType::FLOAT_32, 3);
    }
    return rs->mElement_FLOAT_3;
}

AutoPtr<IElement> Element::F32_4(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_FLOAT_4 == NULL) {
        rs->mElement_FLOAT_4 = CreateVector(rs, (Handle64)DataType::FLOAT_32, 4);
    }
    return rs->mElement_FLOAT_4;
}

AutoPtr<IElement> Element::F64_2(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_DOUBLE_2 == NULL) {
        rs->mElement_DOUBLE_2 = CreateVector(rs, (Handle64)DataType::FLOAT_64, 2);
    }
    return rs->mElement_DOUBLE_2;
}

AutoPtr<IElement> Element::F64_3(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_DOUBLE_3 == NULL) {
        rs->mElement_DOUBLE_3 = CreateVector(rs, (Handle64)DataType::FLOAT_64, 3);
    }
    return rs->mElement_DOUBLE_3;
}

AutoPtr<IElement> Element::F64_4(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_DOUBLE_4 == NULL) {
        rs->mElement_DOUBLE_4 = CreateVector(rs, (Handle64)DataType::FLOAT_64, 4);
    }
    return rs->mElement_DOUBLE_4;
}

AutoPtr<IElement> Element::U8_2(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_UCHAR_2 == NULL) {
        rs->mElement_UCHAR_2 = CreateVector(rs, (Handle64)DataType::UNSIGNED_8, 2);
    }
    return rs->mElement_UCHAR_2;
}

AutoPtr<IElement> Element::U8_3(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_UCHAR_3 == NULL) {
        rs->mElement_UCHAR_3 = CreateVector(rs, (Handle64)DataType::UNSIGNED_8, 3);
    }
    return rs->mElement_UCHAR_3;
}

AutoPtr<IElement> Element::U8_4(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_UCHAR_4 == NULL) {
        rs->mElement_UCHAR_4 = CreateVector(rs, (Handle64)DataType::UNSIGNED_8, 4);
    }
    return rs->mElement_UCHAR_4;
}

AutoPtr<IElement> Element::I8_2(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_CHAR_2 == NULL) {
        rs->mElement_CHAR_2 = CreateVector(rs, (Handle64)DataType::SIGNED_8, 2);
    }
    return rs->mElement_CHAR_2;
}

AutoPtr<IElement> Element::I8_3(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_CHAR_3 == NULL) {
        rs->mElement_CHAR_3 = CreateVector(rs, (Handle64)DataType::SIGNED_8, 3);
    }
    return rs->mElement_CHAR_3;
}

AutoPtr<IElement> Element::I8_4(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_CHAR_4 == NULL) {
        rs->mElement_CHAR_4 = CreateVector(rs, (Handle64)DataType::SIGNED_8, 4);
    }
    return rs->mElement_CHAR_4;
}

AutoPtr<IElement> Element::U16_2(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_USHORT_2 == NULL) {
        rs->mElement_USHORT_2 = CreateVector(rs, (Handle64)DataType::UNSIGNED_16, 2);
    }
    return rs->mElement_USHORT_2;
}

AutoPtr<IElement> Element::U16_3(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_USHORT_3 == NULL) {
        rs->mElement_USHORT_3 = CreateVector(rs, (Handle64)DataType::UNSIGNED_16, 3);
    }
    return rs->mElement_USHORT_3;
}

AutoPtr<IElement> Element::U16_4(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_USHORT_4 == NULL) {
        rs->mElement_USHORT_4 = CreateVector(rs, (Handle64)DataType::UNSIGNED_16, 4);
    }
    return rs->mElement_USHORT_4;
}

AutoPtr<IElement> Element::I16_2(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_SHORT_2 == NULL) {
        rs->mElement_SHORT_2 = CreateVector(rs, (Handle64)DataType::SIGNED_16, 2);
    }
    return rs->mElement_SHORT_2;
}

AutoPtr<IElement> Element::I16_3(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_SHORT_3 == NULL) {
        rs->mElement_SHORT_3 = CreateVector(rs, (Handle64)DataType::SIGNED_16, 3);
    }
    return rs->mElement_SHORT_3;
}

AutoPtr<IElement> Element::I16_4(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_SHORT_4 == NULL) {
        rs->mElement_SHORT_4 = CreateVector(rs, (Handle64)DataType::SIGNED_16, 4);
    }
    return rs->mElement_SHORT_4;
}

AutoPtr<IElement> Element::U32_2(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_UINT_2 == NULL) {
        rs->mElement_UINT_2 = CreateVector(rs, (Handle64)DataType::UNSIGNED_32, 2);
    }
    return rs->mElement_UINT_2;
}

AutoPtr<IElement> Element::U32_3(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_UINT_3 == NULL) {
        rs->mElement_UINT_3 = CreateVector(rs, (Handle64)DataType::UNSIGNED_32, 3);
    }
    return rs->mElement_UINT_3;
}

AutoPtr<IElement> Element::U32_4(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_UINT_4 == NULL) {
        rs->mElement_UINT_4 = CreateVector(rs, (Handle64)DataType::UNSIGNED_32, 4);
    }
    return rs->mElement_UINT_4;
}

AutoPtr<IElement> Element::I32_2(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_INT_2 == NULL) {
        rs->mElement_INT_2 = CreateVector(rs, (Handle64)DataType::SIGNED_32, 2);
    }
    return rs->mElement_INT_2;
}

AutoPtr<IElement> Element::I32_3(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_INT_3 == NULL) {
        rs->mElement_INT_3 = CreateVector(rs, (Handle64)DataType::SIGNED_32, 3);
    }
    return rs->mElement_INT_3;
}

AutoPtr<IElement> Element::I32_4(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_INT_4 == NULL) {
        rs->mElement_INT_4 = CreateVector(rs, (Handle64)DataType::SIGNED_32, 4);
    }
    return rs->mElement_INT_4;
}

AutoPtr<IElement> Element::U64_2(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_ULONG_2 == NULL) {
        rs->mElement_ULONG_2 = CreateVector(rs, (Handle64)DataType::UNSIGNED_64, 2);
    }
    return rs->mElement_ULONG_2;
}

AutoPtr<IElement> Element::U64_3(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_ULONG_3 == NULL) {
        rs->mElement_ULONG_3 = CreateVector(rs, (Handle64)DataType::UNSIGNED_64, 3);
    }
    return rs->mElement_ULONG_3;
}

AutoPtr<IElement> Element::U64_4(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_ULONG_4 == NULL) {
        rs->mElement_ULONG_4 = CreateVector(rs, (Handle64)DataType::UNSIGNED_64, 4);
    }
    return rs->mElement_ULONG_4;
}

AutoPtr<IElement> Element::I64_2(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_LONG_2 == NULL) {
        rs->mElement_LONG_2 = CreateVector(rs, (Handle64)DataType::SIGNED_64, 2);
    }
    return rs->mElement_LONG_2;
}

AutoPtr<IElement> Element::I64_3(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_LONG_2 == NULL) {
        rs->mElement_LONG_2 = CreateVector(rs, (Handle64)DataType::SIGNED_64, 3);
    }
    return rs->mElement_LONG_2;
}

AutoPtr<IElement> Element::I64_4(
    /* [in] */ RenderScript* rs)
{
    if(rs->mElement_LONG_2 == NULL) {
        rs->mElement_LONG_2 = CreateVector(rs, (Handle64)DataType::SIGNED_64, 4);
    }
    return rs->mElement_LONG_2;
}

AutoPtr<IElement> Element::YUV(
    /* [in] */ RenderScript* rs)
{
    if (rs->mElement_YUV == NULL) {
        rs->mElement_YUV = CreatePixel(rs, (Handle64)DataType::UNSIGNED_8, ElementDataKind_PIXEL_YUV);
    }
    return rs->mElement_YUV;
}

AutoPtr<IElement> Element::MATRIX_4X4(
    /* [in] */ RenderScript* rs)
{
    if (rs->mElement_MATRIX_4X4 == NULL) {
        rs->mElement_MATRIX_4X4 = CreateUser(rs, (Handle64)DataType::MATRIX_4X4);
    }
    return rs->mElement_MATRIX_4X4;
}

AutoPtr<IElement> Element::MATRIX4X4(
    /* [in] */ RenderScript* rs)
{
    return MATRIX_4X4(rs);
}

AutoPtr<IElement> Element::MATRIX_3X3(
    /* [in] */ RenderScript* rs)
{
    if (rs->mElement_MATRIX_3X3 == NULL) {
        rs->mElement_MATRIX_3X3 = CreateUser(rs, (Handle64)DataType::MATRIX_3X3);
    }
    return rs->mElement_MATRIX_3X3;
}

AutoPtr<IElement> Element::MATRIX_2X2(
    /* [in] */ RenderScript* rs)
{
    if (rs->mElement_MATRIX_2X2 == NULL) {
        rs->mElement_MATRIX_2X2 = CreateUser(rs, (Handle64)DataType::MATRIX_2X2);
    }
    return rs->mElement_MATRIX_2X2;
}

ECode Element::constructor(
    /* [in] */ Int64 id,
    /* [in] */ RenderScript* rs,
    /* [in] */ ArrayOf<IElement*>* e,
    /* [in] */ ArrayOf<String>* n,
    /* [in] */ ArrayOf<Int32>* as)
{
    BaseObj::constructor(id, rs);
    mSize = 0;
    mVectorSize = 1;
    mElements = e;
    mElementNames = n;
    mArraySizes = as;
    mType = DataType::NONE;
    mKind = ElementDataKind_USER;
    mOffsetInBytes = ArrayOf<Int32>::Alloc(mElements->GetLength());
    for (Int32 ct = 0; ct < mElements->GetLength(); ct++ ) {
        (*mOffsetInBytes)[ct] = mSize;
        mSize += (*mElements)[ct].mSize * (*mArraySizes)[ct];
    }
    UpdateVisibleSubElements();
    return NOERROR;
}

ECode Element::constructor(
    /* [in] */ Int64 id,
    /* [in] */ RenderScript* rs,
    /* [in] */ DataType* dt,
    /* [in] */ ElementDataKind dk,
    /* [in] */ Boolean norm,
    /* [in] */ Int32 size)
{
    BaseObj::constructor(id, rs);
    if ((dt->mID != ElementDataType_UNSIGNED_5_6_5) &&
        (dt->mID != ElementDataType_UNSIGNED_4_4_4_4) &&
        (dt->mID != ElementDataType_UNSIGNED_5_5_5_1)) {
        if (size == 3) {
            mSize = dt->mSize * 4;
        }
        else {
            mSize = dt->mSize * size;
        }
    } else {
        mSize = dt->mSize;
    }
    mType = dt;
    mKind = dk;
    mNormalized = norm;
    mVectorSize = size;
    return NOERROR;
}

ECode Element::constructor(
    /* [in] */ Int64 id,
    /* [in] */ RenderScript* rs)
{
    return BaseObj::constructor(id, rs);
}

void Element::UpdateFromNative()
{
    BaseObj::UpdateFromNative();

    // we will pack mType; mKind; mNormalized; mVectorSize; NumSubElements
    AutoPtr<ArrayOf<Int32> > dataBuffer = ArrayOf<Int32>::Alloc(5);
    Int64 id;
    GetID(mRS, &id);
    mRS->NElementGetNativeData(id, dataBuffer);

    mNormalized = (*dataBuffer)[2] == 1 ? TRUE : FALSE;
    mVectorSize = (*dataBuffer)[3];
    mSize = 0;
    mType = DataType::GetDataType((*dataBuffer)[0]);
    mSize = mType->mSize * mVectorSize;
    mKind = (*dataBuffer)[1];
    // for (DataType dt: DataType.values()) {
    //     if(dt.mID == dataBuffer[0]){
    //         mType = dt;
    //         mSize = mType.mSize * mVectorSize;
    //     }
    // }
    // for (DataKind dk: ElementDataType_values()) {
    //     if(dk.mID == dataBuffer[1]){
    //         mKind = dk;
    //     }
    // }

    Int32 numSubElements = (*dataBuffer)[4];
    if(numSubElements > 0) {
        mElements = ArrayOf<IElement*>::Alloc(numSubElements);
        mElementNames = ArrayOf<String>::Alloc(numSubElements);
        mArraySizes = ArrayOf<Int32>::Alloc(numSubElements);
        mOffsetInBytes = ArrayOf<Int32>::Alloc(numSubElements);

        AutoPtr<ArrayOf<Int64> > subElementIds = ArrayOf<Int64>::Alloc(numSubElements);
        Int64 id;
        GetID(mRS, &id);
        mRS->NElementGetSubElements(id, subElementIds, mElementNames, mArraySizes);
        for(Int32 i = 0; i < numSubElements; i ++) {
            AutoPtr<IElement> e = (IElement*)new Element((*subElementIds)[i], mRS);
            mElements->Set(i, e);
            (*mElements)[i]->UpdateFromNative();
            (*mOffsetInBytes)[i] = mSize;
            mSize += (*mElements)[i]->mSize * (*mArraySizes)[i];
        }
    }
    UpdateVisibleSubElements();
}

AutoPtr<IElement> Element::CreateUser(
    /* [in] */ RenderScript* rs,
    /* [in] */ DataType* dt)
{
    ElementDataKind dk = ElementDataKind_USER;
    Boolean norm = FALSE;
    Int32 vecSize = 1;
    Int64 id;
    rs->NElementCreate(dt->mID, dk, norm, vecSize, &id);
    return (IElement*)new Element(id, rs, dt, dk, norm, vecSize);
}

ECode Element::CreateVector(
    /* [in] */ RenderScript* rs,
    /* [in] */ Handle64 _dt,
    /* [in] */ Int32 size,
    /* [out] */ IElement** e)
{
    VALIDATE_NOT_NULL(e)
    *e = NULL;
    AutoPtr<DataType> dt = (DataType*)_dt;
    if (size < 2 || size > 4) {
        Slogger::E("Element", "Vector size out of range 2-4.");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    switch (dt) {
        // Support only primitive integer/float/boolean types as vectors.
        case ElementDataType_FLOAT_32:
        case ElementDataType_FLOAT_64:
        case ElementDataType_SIGNED_8:
        case ElementDataType_SIGNED_16:
        case ElementDataType_SIGNED_32:
        case ElementDataType_SIGNED_64:
        case ElementDataType_UNSIGNED_8:
        case ElementDataType_UNSIGNED_16:
        case ElementDataType_UNSIGNED_32:
        case ElementDataType_UNSIGNED_64:
        case ElementDataType_BOOLEAN: {
            ElementDataKind dk = ElementDataKind_USER;
            Boolean norm = FALSE;
            Int64 id;
            rs->NElementCreate(dt->mID, dk, norm, size, &id);
            *e = (IElement*)new Element(id, rs, dt, dk, norm, size);
            REFCOUNT_ADD(*e)
            return NOERROR;
        }

        default: {
            Slogger::E("Element", "Cannot create vector of non-primitive type.");
            return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
}

ECode Element::CreatePixel(
    /* [in] */ RenderScript* rs,
    /* [in] */ Handle64 _dt,
    /* [in] */ ElementDataKind dk,
    /* [out] */ IElement** e)
{
    VALIDATE_NOT_NULL(e)
    *e = NULL;
    AutoPtr<DataType> dt = (DataType*)_dt;
    if (!(dk == ElementDataType_PIXEL_L ||
          dk == ElementDataType_PIXEL_A ||
          dk == ElementDataType_PIXEL_LA ||
          dk == ElementDataType_PIXEL_RGB ||
          dk == ElementDataType_PIXEL_RGBA ||
          dk == ElementDataType_PIXEL_DEPTH ||
          dk == ElementDataType_PIXEL_YUV)) {
        Slogger::E("Element", "Unsupported DataKind");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (!(dt->mID == ElementDataType_UNSIGNED_8 ||
          dt->mID == ElementDataType_UNSIGNED_16 ||
          dt->mID == ElementDataType_UNSIGNED_5_6_5 ||
          dt->mID == ElementDataType_UNSIGNED_4_4_4_4 ||
          dt->mID == ElementDataType_UNSIGNED_5_5_5_1)) {
        Slogger::E("Element", "Unsupported DataType");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (dt->mID == ElementDataType_UNSIGNED_5_6_5 && dk != ElementDataType_PIXEL_RGB) {
        Slogger::E("Element", "Bad kind and type combo");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (dt->mID == ElementDataType_UNSIGNED_5_5_5_1 && dk != ElementDataType_PIXEL_RGBA) {
        Slogger::E("Element", "Bad kind and type combo");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (dt->mID == ElementDataType_UNSIGNED_4_4_4_4 && dk != ElementDataType_PIXEL_RGBA) {
        Slogger::E("Element", "Bad kind and type combo");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (dt->mID == ElementDataType_UNSIGNED_16 && dk != ElementDataType_PIXEL_DEPTH) {
        Slogger::E("Element", "Bad kind and type combo");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 size = 1;
    switch (dk) {
        case ElementDataKind_PIXEL_LA:
            size = 2;
            break;
        case ElementDataKind_PIXEL_RGB:
            size = 3;
            break;
        case ElementDataKind_PIXEL_RGBA:
            size = 4;
            break;
        case ElementDataKind_PIXEL_DEPTH:
            size = 2;
            break;
    }

    Boolean norm = TRUE;
    Int64 id;
    rs->NElementCreate(dt->mID, dk, norm, size, &id);
    *e = (IElement*)new Element(id, rs, dt, dk, norm, size);
    REFCOUNT_ADD(*e)
    return NOERROR;
}

ECode Element::IsCompatible(
    /* [in] */ IElement* e,
    /* [out] */ Boolean* isCompatible)
{
    VALIDATE_NOT_NULL(isCompatible)
    // Try strict BaseObj equality to start with.
    if (this->Equals(e)) {
        *isCompatible = TRUE;
        return NOERROR;
    }

    AutoPtr<Element> obj = (Element*)e;
    // Ignore mKind because it is allowed to be different (user vs. pixel).
    // We also ignore mNormalized because it can be different. The mType
    // field must not be NONE since we require name equivalence for
    // all user-created Elements.
    return ((mSize == obj->mSize) &&
            (mType->mID != ElementDataType_NONE) &&
            (mType->mID == obj->mType->mID) &&
            (mVectorSize == obj->mVectorSize));
}


} // namespace RenderScript
} // namespace Droid
} // namespace Elastos
