
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
    /* [in] */ IRenderScript* rs)
{
    if(rs->mElement_BOOLEAN == NULL) {
        rs->mElement_BOOLEAN = CreateUser(rs, DataType::BOOLEAN);
    }
    return rs->mElement_BOOLEAN;
}

AutoPtr<IElement> Element::U8(
    /* [in] */ IRenderScript* rs)
{
    if(rs->mElement_U8 == NULL) {
        rs->mElement_U8 = CreateUser(rs, DataType::UNSIGNED_8);
    }
    return rs->mElement_U8;
}

AutoPtr<IElement> Element::I8(
    /* [in] */ IRenderScript* rs)
{
    if(rs->mElement_I8 == NULL) {
        rs->mElement_I8 = CreateUser(rs, DataType::SIGNED_8);
    }
    return rs->mElement_I8;
}

AutoPtr<IElement> Element::U16(
    /* [in] */ IRenderScript* rs)
{
    if(rs->mElement_U16 == NULL) {
        rs->mElement_U16 = CreateUser(rs, DataType::UNSIGNED_16);
    }
    return rs->mElement_U16;
}

AutoPtr<IElement> Element::I16(
    /* [in] */ IRenderScript* rs)
{
    if(rs->mElement_I16 == NULL) {
        rs->mElement_I16 = CreateUser(rs, DataType::SIGNED_16);
    }
    return rs->mElement_I16;
}

AutoPtr<IElement> Element::U32(
    /* [in] */ IRenderScript* rs)
{
    if(rs->mElement_U32 == NULL) {
        rs->mElement_U32 = CreateUser(rs, DataType::UNSIGNED_32);
    }
    return rs->mElement_U32;
}

AutoPtr<IElement> Element::I32(
    /* [in] */ IRenderScript* rs)
{
    if(rs->mElement_I32 == NULL) {
        rs->mElement_I32 = CreateUser(rs, DataType::SIGNED_32);
    }
    return rs->mElement_I32;
}

AutoPtr<IElement> Element::U64(
    /* [in] */ IRenderScript* rs)
{
    if(rs->mElement_U64 == NULL) {
        rs->mElement_U64 = CreateUser(rs, DataType::UNSIGNED_64);
    }
    return rs->mElement_U64;
}

AutoPtr<IElement> Element::I64(
    /* [in] */ IRenderScript* rs)
{
    if(rs->mElement_I64 == NULL) {
        rs->mElement_I64 = CreateUser(rs, DataType::SIGNED_64);
    }
    return rs->mElement_I64;
}

AutoPtr<IElement> Element::F32(
    /* [in] */ IRenderScript* rs)
{
    if(rs->mElement_F32 == NULL) {
        rs->mElement_F32 = CreateUser(rs, DataType::FLOAT_32);
    }
    return rs->mElement_F32;
}

AutoPtr<IElement> Element::F64(
    /* [in] */ IRenderScript* rs)
{
    if(rs->mElement_F64 == NULL) {
        rs->mElement_F64 = CreateUser(rs, DataType::FLOAT_64);
    }
    return rs->mElement_F64;
}

AutoPtr<IElement> Element::ELEMENT(
    /* [in] */ IRenderScript* rs)
{
    if(rs->mElement_ELEMENT == NULL) {
        rs->mElement_ELEMENT = CreateUser(rs, DataType::RS_ELEMENT);
    }
    return rs->mElement_ELEMENT;
}

AutoPtr<IElement> Element::TYPE(
    /* [in] */ IRenderScript* rs)
{
    if(rs->mElement_TYPE == NULL) {
        rs->mElement_TYPE = CreateUser(rs, DataType::RS_TYPE);
    }
    return rs->mElement_TYPE;
}

AutoPtr<IElement> Element::ALLOCATION(
    /* [in] */ IRenderScript* rs)
{
    if(rs->mElement_ALLOCATION == NULL) {
        rs->mElement_ALLOCATION = CreateUser(rs, DataType::RS_ALLOCATION);
    }
    return rs->mElement_ALLOCATION;
}

AutoPtr<IElement> Element::SAMPLER(
    /* [in] */ IRenderScript* rs)
{
    if(rs->mElement_SAMPLER == NULL) {
        rs->mElement_SAMPLER = CreateUser(rs, DataType::RS_SAMPLER);
    }
    return rs->mElement_SAMPLER;
}

AutoPtr<IElement> Element::SCRIPT(
    /* [in] */ IRenderScript* rs)
{
    if(rs->mElement_SCRIPT == NULL) {
        rs->mElement_SCRIPT = CreateUser(rs, DataType::RS_SCRIPT);
    }
    return rs->mElement_SCRIPT;
}

AutoPtr<IElement> Element::MESH(
    /* [in] */ IRenderScript* rs)
{
    if(rs->mElement_MESH == NULL) {
        rs->mElement_MESH = CreateUser(rs, DataType::RS_MESH);
    }
    return rs->mElement_MESH;
}

AutoPtr<IElement> Element::PROGRAM_FRAGMENT(
    /* [in] */ IRenderScript* rs)
{
    if(rs->mElement_PROGRAM_FRAGMENT == NULL) {
        rs->mElement_PROGRAM_FRAGMENT = CreateUser(rs, DataType::RS_PROGRAM_FRAGMENT);
    }
    return rs->mElement_PROGRAM_FRAGMENT;
}

AutoPtr<IElement> Element::PROGRAM_VERTEX(
    /* [in] */ IRenderScript* rs)
{
    if(rs->mElement_PROGRAM_VERTEX == NULL) {
        rs->mElement_PROGRAM_VERTEX = CreateUser(rs, DataType::RS_PROGRAM_VERTEX);
    }
    return rs->mElement_PROGRAM_VERTEX;
}

AutoPtr<IElement> Element::PROGRAM_RASTER(
    /* [in] */ IRenderScript* rs)
{
    if(rs->mElement_PROGRAM_RASTER == NULL) {
        rs->mElement_PROGRAM_RASTER = CreateUser(rs, DataType::RS_PROGRAM_RASTER);
    }
    return rs->mElement_PROGRAM_RASTER;
}

AutoPtr<IElement> Element::PROGRAM_STORE(
    /* [in] */ IRenderScript* rs)
{
    if(rs->mElement_PROGRAM_STORE == NULL) {
        rs->mElement_PROGRAM_STORE = CreateUser(rs, DataType::RS_PROGRAM_STORE);
    }
    return rs->mElement_PROGRAM_STORE;
}

AutoPtr<IElement> Element::FONT(
    /* [in] */ IRenderScript* rs)
{
    if(rs->mElement_FONT == NULL) {
        rs->mElement_FONT = CreateUser(rs, DataType::RS_FONT);
    }
    return rs->mElement_FONT;
}

AutoPtr<IElement> Element::A_8(
    /* [in] */ IRenderScript* rs)
{
    if(rs->mElement_A_8 == NULL) {
        rs->mElement_A_8 = CreatePixel(rs, (Handle64)DataType::UNSIGNED_8, ElementDataKind_PIXEL_A);
    }
    return rs->mElement_A_8;
}

AutoPtr<IElement> Element::RGB_565(
    /* [in] */ IRenderScript* rs)
{
    if(rs->mElement_RGB_565 == NULL) {
        rs->mElement_RGB_565 = CreatePixel(rs, (Handle64)DataType::UNSIGNED_5_6_5, ElementDataKind_PIXEL_RGB);
    }
    return rs->mElement_RGB_565;
}

AutoPtr<IElement> Element::RGB_888(
    /* [in] */ IRenderScript* rs)
{
    if(rs->mElement_RGB_888 == NULL) {
        rs->mElement_RGB_888 = CreatePixel(rs, (Handle64)DataType::UNSIGNED_8, ElementDataKind_PIXEL_RGB);
    }
    return rs->mElement_RGB_888;
}

AutoPtr<IElement> Element::RGBA_5551(
    /* [in] */ IRenderScript* rs)
{
    if(rs->mElement_RGBA_5551 == NULL) {
        rs->mElement_RGBA_5551 = CreatePixel(rs, (Handle64)DataType::UNSIGNED_5_5_5_1, ElementDataKind_PIXEL_RGBA);
    }
    return rs->mElement_RGBA_5551;
}


AutoPtr<IElement> Element::RGBA_4444(
    /* [in] */ IRenderScript* rs)
{
    if(rs->mElement_RGBA_4444 == NULL) {
        rs->mElement_RGBA_4444 = CreatePixel(rs, (Handle64)DataType::UNSIGNED_4_4_4_4, ElementDataKind_PIXEL_RGBA);
    }
    return rs->mElement_RGBA_4444;
}


AutoPtr<IElement> Element::RGBA_8888(
    /* [in] */ IRenderScript* rs)
{
    if(rs->mElement_RGBA_8888 == NULL) {
        rs->mElement_RGBA_8888 = CreatePixel(rs, (Handle64)DataType::UNSIGNED_8, ElementDataKind_PIXEL_RGBA);
    }
    return rs->mElement_RGBA_8888;
}


AutoPtr<IElement> Element::F32_2(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::F32_3(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::F32_4(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::F64_2(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::F64_3(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::F64_4(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::U8_2(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::U8_3(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::U8_4(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::I8_2(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::I8_3(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::I8_4(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::U16_2(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::U16_3(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::U16_4(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::I16_2(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::I16_3(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::I16_4(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::U32_2(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::U32_3(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::U32_4(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::I32_2(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::I32_3(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::I32_4(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::U64_2(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::U64_3(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::U64_4(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::I64_2(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::I64_3(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::I64_4(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::YUV(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::MATRIX_4X4(
    /* [in] */ IRenderScript* rs);

/** @deprecated use MATRIX_4X4
*/
AutoPtr<IElement> Element::MATRIX4X4(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::MATRIX_3X3(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::MATRIX_2X2(
    /* [in] */ IRenderScript* rs);

AutoPtr<IElement> Element::CreateVector(
    /* [in] */ IRenderScript* rs,
    /* [in] */ Handle64 dt,
    /* [in] */ Int32 size);

AutoPtr<IElement> Element::CreatePixel(
    /* [in] */ IRenderScript* rs,
    /* [in] */ Handle64 dt,
    /* [in] */ ElementDataKind dk);

ECode Element::IsCompatible(
    /* [in] */ IElement* e,
    /* [out] */ Boolean* isCompatible);

ECode Element::constructor(
    /* [in] */ Int64 id,
    /* [in] */ RenderScript* rs,
    /* [in] */ ArrayOf<IElement*>* e,
    /* [in] */ ArrayOf<String>* n,
    /* [in] */ ArrayOf<Int32>* as);

ECode Element::constructor(
    /* [in] */ Int64 id,
    /* [in] */ RenderScript* rs,
    /* [in] */ DataType* dt,
    /* [in] */ ElementDataKind dk,
    /* [in] */ Boolean norm,
    /* [in] */ Int32 size);

ECode Element::constructor(
    /* [in] */ Int64 id,
    /* [in] */ RenderScript* rs);

} // namespace RenderScript
} // namespace Droid
} // namespace Elastos
