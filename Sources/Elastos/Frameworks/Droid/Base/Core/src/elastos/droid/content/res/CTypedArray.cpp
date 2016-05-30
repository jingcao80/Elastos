
#include "elastos/droid/content/res/CTypedArray.h"
#include "elastos/droid/content/res/CAssetManager.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include <elastos/core/Math.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>
#include <stdlib.h>

using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CAR_INTERFACE_IMPL(CTypedArray, Object, ITypedArray)

CAR_OBJECT_IMPL(CTypedArray)

AutoPtr<ITypedArray> CTypedArray::Obtain(
    /* [in] */ IResources* res,
    /* [in] */ Int32 len)
{
    CResources* r = (CResources*)res;
    assert(r != NULL && r->mTypedArrayPool != NULL);
    AutoPtr<ITypedArray> attrs = r->mTypedArrayPool->AcquireItem();
    if (attrs != NULL) {
        CTypedArray* cta = (CTypedArray*)attrs.Get();
        cta->mLength = len;
        cta->mRecycled = FALSE;

        Int32 fullLen = len * CAssetManager::STYLE_NUM_ENTRIES;
        if (cta->mData->GetLength() >= fullLen) {
            return attrs;
        }

        cta->mData = ArrayOf<Int32>::Alloc(fullLen);
        cta->mIndices = ArrayOf<Int32>::Alloc(1 + len);
        return attrs;
    }

    AutoPtr<ArrayOf<Int32> > a1 = ArrayOf<Int32>::Alloc(len*CAssetManager::STYLE_NUM_ENTRIES);
    AutoPtr<ArrayOf<Int32> > a2 = ArrayOf<Int32>::Alloc(1 + len);
    AutoPtr<ITypedArray> ta;
    ASSERT_SUCCEEDED(CTypedArray::New(res, a1, a2, len, (ITypedArray**)&ta));
    return ta;
}

CTypedArray::CTypedArray()
    : mLength(0)
    , mRecycled(FALSE)
{
}

CTypedArray::~CTypedArray()
{}

ECode CTypedArray::constructor(
    /* [in] */ IResources* resources,
    /* [in] */ ArrayOf<Int32>* data,
    /* [in] */ ArrayOf<Int32>* indices,
    /* [in] */ Int32 len)
{
    ASSERT_SUCCEEDED(CTypedValue::NewByFriend((CTypedValue**)&mValue));

    mResources = (CResources*)resources;
    mMetrics = mResources->mMetrics;
    mAssets = mResources->mAssets;
    mData = data;
    mIndices = indices;
    mLength = len;
    return NOERROR;
}

ECode CTypedArray::GetLength(
    /* [out] */ Int32* len)
{
    VALIDATE_NOT_NULL(len);
    *len = 0;
    if (mRecycled) {
        Logger::W("CTypedArray", "Already recycled!");
        return E_RUNTIME_EXCEPTION;
    }

    *len = mLength;
    return NOERROR;
}

ECode CTypedArray::GetIndexCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = 0;
    if (mRecycled) {
        Logger::W("CTypedArray", "Already recycled!");
        return E_RUNTIME_EXCEPTION;
    }
    *count = (*mIndices)[0];
    return NOERROR;
}

ECode CTypedArray::GetIndex(
    /* [in] */ Int32 at,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    *index = 0;
    if (mRecycled) {
        Logger::W("CTypedArray", "Already recycled!");
        return E_RUNTIME_EXCEPTION;
    }
    *index = (*mIndices)[1 + at];
    return NOERROR;
}

ECode CTypedArray::GetResources(
    /* [out] */ IResources** res)
{
    VALIDATE_NOT_NULL(res);
    if (mRecycled) {
        *res = NULL;
        return E_RUNTIME_EXCEPTION;
    }
    *res = (IResources*)mResources;
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CTypedArray::GetText(
    /* [in] */ Int32 index,
    /* [out] */ ICharSequence** csq)
{
    VALIDATE_NOT_NULL(csq);
    *csq = NULL;

    if (mRecycled) {
        Logger::W("CTypedArray", "Already recycled!");
        return E_RUNTIME_EXCEPTION;
    }

    index *= CAssetManager::STYLE_NUM_ENTRIES;
    AutoPtr< ArrayOf<Int32> > data = mData;
    Int32 type = (*data)[index + CAssetManager::STYLE_TYPE];
    if (type == ITypedValue::TYPE_NULL) {
        return NOERROR;
    }
    else if (type == ITypedValue::TYPE_STRING) {
        AutoPtr<ICharSequence> temp = LoadStringValueAt(index);
        *csq = temp;
        REFCOUNT_ADD(*csq);
        return NOERROR;
    }

    AutoPtr<CTypedValue> v = mValue;
    if (GetValueAt(index, v)) {
        Logger::W("CTypedArray", "Converting to string: %p", v.Get());
        return v->CoerceToString(csq);
    }
    Logger::W("CTypedArray", "getString of bad type: %d", type);
    return NOERROR;
}

ECode CTypedArray::GetString(
    /* [in] */ Int32 index,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String(NULL);

    if (mRecycled) {
        Logger::W("CTypedArray", "Already recycled!");
        return E_RUNTIME_EXCEPTION;
    }

    index *= CAssetManager::STYLE_NUM_ENTRIES;
    AutoPtr< ArrayOf<Int32> > data = mData;
    Int32 type = (*data)[index + CAssetManager::STYLE_TYPE];
    if (type == ITypedValue::TYPE_NULL) {
        return NOERROR;
    }
    else if (type == ITypedValue::TYPE_STRING) {
        AutoPtr<ICharSequence> csq = LoadStringValueAt(index);
        assert(csq);
        return csq->ToString(str);
    }

    AutoPtr<CTypedValue> v = mValue;
    if (GetValueAt(index, v)) {
        Logger::W("CTypedArray", "Converting to string: %p", v.Get());
        AutoPtr<ICharSequence> csq;
        if (FAILED(v->CoerceToString((ICharSequence**)&csq))) {
            *str = String(NULL);
            return NOERROR;
        }
        return csq->ToString(str);
    }
    Logger::W("CTypedArray", "getString of bad type: %d", type);
    *str = String(NULL);
    return NOERROR;
}

ECode CTypedArray::GetNonResourceString(
    /* [in] */ Int32 index,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String(NULL);

    if (mRecycled) {
        Logger::W("CTypedArray", "Already recycled!");
        return E_RUNTIME_EXCEPTION;
    }

    index *= CAssetManager::STYLE_NUM_ENTRIES;
    AutoPtr< ArrayOf<Int32> > data = mData;
    Int32 type = (*data)[index + CAssetManager::STYLE_TYPE];
    if (type == ITypedValue::TYPE_STRING) {
        Int32 cookie = (*data)[index + CAssetManager::STYLE_ASSET_COOKIE];
        if (cookie < 0) {
            AutoPtr<ICharSequence> csq;
            if (FAILED(mXml->GetPooledString((*data)[index + CAssetManager::STYLE_DATA],
                    (ICharSequence**)&csq))) {
                *str = String(NULL);
                return NOERROR;
            }
            return csq->ToString(str);
        }
    }

    return NOERROR;
}

ECode CTypedArray::GetNonConfigurationString(
    /* [in] */ Int32 index,
    /* [in] */ Int32 allowedChangingConfigs,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String(NULL);

    if (mRecycled) {
        Logger::W("CTypedArray", "Already recycled!");
        return E_RUNTIME_EXCEPTION;
    }

    index *= CAssetManager::STYLE_NUM_ENTRIES;
    AutoPtr< ArrayOf<Int32> > data = mData;
    Int32 type = (*data)[index + CAssetManager::STYLE_TYPE];
    if (((*data)[index + CAssetManager::STYLE_CHANGING_CONFIGURATIONS]
            & ~allowedChangingConfigs) != 0) {
        return NOERROR;
    }
    if (type == ITypedValue::TYPE_NULL) {
        return NOERROR;
    }
    else if (type == ITypedValue::TYPE_STRING) {
        AutoPtr<ICharSequence> csq = LoadStringValueAt(index);
        if (csq == NULL) {
            return NOERROR;
        }
        return csq->ToString(str);
    }
    Logger::W("CTypedArray", "getString of bad type: 0x%08x", type);
    return NOERROR;
}

ECode CTypedArray::GetBoolean(
    /* [in] */ Int32 index,
    /* [in] */ Boolean defValue,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    if (mRecycled) {
        Logger::W("CTypedArray", "Already recycled!");
        return E_RUNTIME_EXCEPTION;
    }

    index *= CAssetManager::STYLE_NUM_ENTRIES;
    AutoPtr< ArrayOf<Int32> > data = mData;
    Int32 type = (*data)[index + CAssetManager::STYLE_TYPE];
    if (type == ITypedValue::TYPE_NULL) {
        *value = defValue;
        return NOERROR;
    }
    else if (type >= ITypedValue::TYPE_FIRST_INT && type <= ITypedValue::TYPE_LAST_INT) {
        *value = (*data)[index + CAssetManager::STYLE_DATA] != 0;
        return NOERROR;
    }

    AutoPtr<CTypedValue> v = mValue;
    if (GetValueAt(index, v)) {
        Logger::W("CTypedArray", "Converting to boolean: %p", v.Get());
        AutoPtr<ICharSequence> csq;
        v->CoerceToString((ICharSequence**)&csq);
        *value = XmlUtils::ConvertValueToBoolean(csq, defValue);
        return NOERROR;
    }
    Logger::W("CTypedArray", "getBoolean of bad type: 0x%08x", type);
    *value = defValue;
    return NOERROR;
}

ECode CTypedArray::GetInt32(
    /* [in] */ Int32 index,
    /* [in] */ Int32 defValue,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = defValue;

    if (mRecycled) {
        Logger::W("CTypedArray", "Already recycled!");
        return E_RUNTIME_EXCEPTION;
    }

    index *= CAssetManager::STYLE_NUM_ENTRIES;
    AutoPtr< ArrayOf<Int32> > data = mData;
    Int32 type = (*data)[index + CAssetManager::STYLE_TYPE];
    if (type == ITypedValue::TYPE_NULL) {
        *value = defValue;
        return NOERROR;
    }
    else if (type >= ITypedValue::TYPE_FIRST_INT && type <= ITypedValue::TYPE_LAST_INT) {
        *value = (*data)[index + CAssetManager::STYLE_DATA];
        return NOERROR;
    }

    AutoPtr<CTypedValue> v = mValue;
    if (GetValueAt(index, v)) {
        Logger::W("CTypedArray", "Converting to Int32: %p", v.Get());
        AutoPtr<ICharSequence> csq;
        v->CoerceToString((ICharSequence**)&csq);
        *value = XmlUtils::ConvertValueToInt32(csq, defValue);
        return NOERROR;
    }
    Logger::W("CTypedArray", "getInt of bad type: 0x%08x", type);
    *value = defValue;
    return NOERROR;
}

ECode CTypedArray::GetFloat(
    /* [in] */ Int32 index,
    /* [in] */ Float defValue,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);
    *value = defValue;

    if (mRecycled) {
        Logger::W("CTypedArray", "Already recycled!");
        return E_RUNTIME_EXCEPTION;
    }

    index *= CAssetManager::STYLE_NUM_ENTRIES;
    AutoPtr< ArrayOf<Int32> > data = mData;
    Int32 type = (*data)[index + CAssetManager::STYLE_TYPE];
    if (type == ITypedValue::TYPE_NULL) {
        *value = defValue;
        return NOERROR;
    }
    else if (type == ITypedValue::TYPE_FLOAT) {
        *value = Elastos::Core::Math::Int32BitsToFloat((*data)[index + CAssetManager::STYLE_DATA]);
        return NOERROR;
    }
    else if (type >= ITypedValue::TYPE_FIRST_INT && type <= ITypedValue::TYPE_LAST_INT) {
        *value = (Float)(*data)[index + CAssetManager::STYLE_DATA];
        return NOERROR;
    }

    AutoPtr<CTypedValue> v = mValue;
    if (GetValueAt(index, v)) {
        Logger::W("CTypedArray", "Converting to float: %p", v.Get());
        AutoPtr<ICharSequence> csq;
        if (SUCCEEDED(v->CoerceToString((ICharSequence**)&csq))) {
            String str;
            csq->ToString(&str);
            *value = atof((const char*)str);
            return NOERROR;
        }
    }
    Logger::W("CTypedArray", "getFloat of bad type: 0x%08x", type);
    *value = defValue;
    return NOERROR;
}

ECode CTypedArray::GetColor(
    /* [in] */ Int32 index,
    /* [in] */ Int32 defValue,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = defValue;

    if (mRecycled) {
        Logger::W("CTypedArray", "Already recycled!");
        return E_RUNTIME_EXCEPTION;
    }

    index *= CAssetManager::STYLE_NUM_ENTRIES;
    AutoPtr< ArrayOf<Int32> > data = mData;
    Int32 type = (*data)[index + CAssetManager::STYLE_TYPE];
    if (type == ITypedValue::TYPE_NULL) {
        *value = defValue;
        return NOERROR;
    }
    else if (type >= ITypedValue::TYPE_FIRST_INT && type <= ITypedValue::TYPE_LAST_INT) {
        *value = (*data)[index + CAssetManager::STYLE_DATA];
        return NOERROR;
    }
    else if (type == ITypedValue::TYPE_STRING) {
        AutoPtr<CTypedValue> v = mValue;
        if (GetValueAt(index, v)) {
            Int32 resourceId;
            v->GetResourceId(&resourceId);
            AutoPtr<IColorStateList> csl;
            mResources->LoadColorStateList(v, resourceId, (IColorStateList**)&csl);
            return csl->GetDefaultColor(value);
        }
        *value = defValue;
        return NOERROR;
    }
    else if (type == ITypedValue::TYPE_ATTRIBUTE) {
        // throw new RuntimeException("Failed to resolve attribute at index " + index);
        return E_RUNTIME_EXCEPTION;
    }

    Logger::E("CTypedArray", "Can't convert to color: type=0x%08x", type);
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CTypedArray::GetColorStateList(
    /* [in] */ Int32 index,
    /* [out] */ IColorStateList** list)
{
    VALIDATE_NOT_NULL(list);
    *list = NULL;

    if (mRecycled) {
        Logger::W("CTypedArray", "Already recycled!");
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<CTypedValue> value = mValue;
    if (GetValueAt(index * CAssetManager::STYLE_NUM_ENTRIES, value)) {
        if (value->mType == ITypedValue::TYPE_ATTRIBUTE) {
            // throw new RuntimeException("Failed to resolve attribute at index " + index);
            return E_RUNTIME_EXCEPTION;
        }

        Int32 resourceId;
        value->GetResourceId(&resourceId);
        return mResources->LoadColorStateList(value, resourceId, list);
    }
    *list = NULL;
    return NOERROR;
}

ECode CTypedArray::GetInteger(
    /* [in] */ Int32 index,
    /* [in] */ Int32 defValue,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;

    if (mRecycled) {
        Logger::W("CTypedArray", "Already recycled!");
        return E_RUNTIME_EXCEPTION;
    }

    index *= CAssetManager::STYLE_NUM_ENTRIES;
    AutoPtr< ArrayOf<Int32> > data = mData;
    Int32 type = (*data)[index + CAssetManager::STYLE_TYPE];
    if (type == ITypedValue::TYPE_NULL) {
        *value = defValue;
        return NOERROR;
    }
    else if (type >= ITypedValue::TYPE_FIRST_INT && type <= ITypedValue::TYPE_LAST_INT) {
        *value = (*data)[index + CAssetManager::STYLE_DATA];
        return NOERROR;
    }
    else if (type == ITypedValue::TYPE_ATTRIBUTE) {
        // throw new RuntimeException("Failed to resolve attribute at index " + index);
        return NOERROR;
    }

    Logger::E("CTypedArray", "Can't convert to integer: type=0x%08x", type);
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CTypedArray::GetDimension(
    /* [in] */ Int32 index,
    /* [in] */ Float defValue,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;

    if (mRecycled) {
        Logger::W("CTypedArray", "Already recycled!");
        return E_RUNTIME_EXCEPTION;
    }

    index *= CAssetManager::STYLE_NUM_ENTRIES;
    AutoPtr< ArrayOf<Int32> > data = mData;
    Int32 type = (*data)[index + CAssetManager::STYLE_TYPE];
    if (type == ITypedValue::TYPE_NULL) {
        *value = defValue;
        return NOERROR;
    }
    else if (type == ITypedValue::TYPE_DIMENSION) {
        *value = CTypedValue::ComplexToDimension(
                (*data)[index + CAssetManager::STYLE_DATA], mMetrics);
        return NOERROR;
    }
    else if (type == ITypedValue::TYPE_ATTRIBUTE) {
        // throw new RuntimeException("Failed to resolve attribute at index " + index);
        return E_RUNTIME_EXCEPTION;
    }

    Logger::E("CTypedArray", "Can't convert to dimension: type=0x%08x", type);
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CTypedArray::GetDimensionPixelOffset(
    /* [in] */ Int32 index,
    /* [in] */ Int32 defValue,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;

    if (mRecycled) {
        Logger::W("CTypedArray", "Already recycled!");
        return E_RUNTIME_EXCEPTION;
    }

    index *= CAssetManager::STYLE_NUM_ENTRIES;
    AutoPtr< ArrayOf<Int32> > data = mData;
    Int32 type = (*data)[index + CAssetManager::STYLE_TYPE];
    if (type == ITypedValue::TYPE_NULL) {
        *value = defValue;
        return NOERROR;
    }
    else if (type == ITypedValue::TYPE_DIMENSION) {
        *value = CTypedValue::ComplexToDimensionPixelOffset(
                (*data)[index + CAssetManager::STYLE_DATA], mMetrics);
        return NOERROR;
    }
    else if (type == ITypedValue::TYPE_ATTRIBUTE) {
        // throw new RuntimeException("Failed to resolve attribute at index " + index);
        return E_RUNTIME_EXCEPTION;
    }

    Logger::E("CTypedArray", "Can't convert to dimension: type=0x%08x", type);
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CTypedArray::GetDimensionPixelSize(
    /* [in] */ Int32 index,
    /* [in] */ Int32 defValue,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;

    if (mRecycled) {
        Logger::W("CTypedArray", "Already recycled!");
        return E_RUNTIME_EXCEPTION;
    }

    index *= CAssetManager::STYLE_NUM_ENTRIES;
    AutoPtr< ArrayOf<Int32> > data = mData;
    Int32 type = (*data)[index + CAssetManager::STYLE_TYPE];
    if (type == ITypedValue::TYPE_NULL) {
        *value = defValue;
        return NOERROR;
    }
    else if (type == ITypedValue::TYPE_DIMENSION) {
        *value = CTypedValue::ComplexToDimensionPixelSize(
                (*data)[index + CAssetManager::STYLE_DATA], mMetrics);
        return NOERROR;
    }
    else if (type == ITypedValue::TYPE_ATTRIBUTE) {
        // throw new RuntimeException("Failed to resolve attribute at index " + index);
        return E_RUNTIME_EXCEPTION;
    }

    Logger::E("CTypedArray", "Can't convert to dimension: type=0x%08x", type);
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CTypedArray::GetLayoutDimension(
    /* [in] */ Int32 index,
    /* [in] */ const String& name,
    /* [out] */ Int32* dimension)
{
    VALIDATE_NOT_NULL(dimension);
    *dimension = 0;

    if (mRecycled) {
        Logger::E("CTypedArray", "Cannot make calls to a recycled instance!");
        return E_RUNTIME_EXCEPTION;
    }

    index *= CAssetManager::STYLE_NUM_ENTRIES;
    AutoPtr< ArrayOf<Int32> > data = mData;
    Int32 type = (*data)[index + CAssetManager::STYLE_TYPE];
    // Logger::I("CTypedArray", "GetLayoutDimension: index:%d, name:%s type:%08x", index, name.string(), type);
    if (type >= ITypedValue::TYPE_FIRST_INT && type <= ITypedValue::TYPE_LAST_INT) {
        *dimension = (*data)[index + CAssetManager::STYLE_DATA];
        return NOERROR;
    }
    else if (type == ITypedValue::TYPE_DIMENSION) {
        *dimension = CTypedValue::ComplexToDimensionPixelSize(
                (*data)[index + CAssetManager::STYLE_DATA], mMetrics);
        return NOERROR;
    }
    else if (type == ITypedValue::TYPE_ATTRIBUTE) {
        Logger::E("CTypedArray", "Failed to resolve attribute at index %d", index);
        return E_RUNTIME_EXCEPTION;
    }

    String des;
    GetPositionDescription(&des);
    Logger::E("CTypedArray", "%s: You must supply a %s attribute. index:%d, type:%d/%08x",
        des.string(), name.string(), index, type, type);
    return E_RUNTIME_EXCEPTION;
}

ECode CTypedArray::GetLayoutDimension(
    /* [in] */ Int32 index,
    /* [in] */ Int32 defValue,
    /* [out] */ Int32* dimension)
{
    VALIDATE_NOT_NULL(dimension);
    *dimension = defValue;

    if (mRecycled) {
        Logger::W("CTypedArray", "Already recycled!");
        return E_RUNTIME_EXCEPTION;
    }

    index *= CAssetManager::STYLE_NUM_ENTRIES;
    AutoPtr< ArrayOf<Int32> > data = mData;
    Int32 type = (*data)[index + CAssetManager::STYLE_TYPE];
    if (type >= ITypedValue::TYPE_FIRST_INT
            && type <= ITypedValue::TYPE_LAST_INT) {
        *dimension = (*data)[index + CAssetManager::STYLE_DATA];
        return NOERROR;
    }
    else if (type == ITypedValue::TYPE_DIMENSION) {
        *dimension = CTypedValue::ComplexToDimensionPixelSize(
                (*data)[index + CAssetManager::STYLE_DATA], mMetrics);
        return NOERROR;
    }

    *dimension = defValue;
    return NOERROR;
}

ECode CTypedArray::GetFraction(
    /* [in] */ Int32 index,
    /* [in] */ Int32 base,
    /* [in] */ Int32 pbase,
    /* [in] */ Float defValue,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);
    *value = defValue;

    if (mRecycled) {
        Logger::W("CTypedArray", "Already recycled!");
        return E_RUNTIME_EXCEPTION;
    }
    index *= CAssetManager::STYLE_NUM_ENTRIES;
    AutoPtr< ArrayOf<Int32> > data = mData;
    Int32 type = (*data)[index + CAssetManager::STYLE_TYPE];
    if (type == ITypedValue::TYPE_NULL) {
        *value = defValue;
        return NOERROR;
    }
    else if (type == ITypedValue::TYPE_FRACTION) {
        *value = CTypedValue::ComplexToFraction(
                (*data)[index + CAssetManager::STYLE_DATA], base, pbase);
        return NOERROR;
    }
    else if (type == ITypedValue::TYPE_ATTRIBUTE) {
        // throw new RuntimeException("Failed to resolve attribute at index " + index);
        return E_RUNTIME_EXCEPTION;
    }

    Logger::E("CTypedArray", "Can't convert to fraction: type=0x%08x", type);
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CTypedArray::GetResourceId(
    /* [in] */ Int32 index,
    /* [in] */ Int32 defValue,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = defValue;

    if (mRecycled) {
        Logger::W("CTypedArray", "Already recycled!");
        return E_RUNTIME_EXCEPTION;
    }

    index *= CAssetManager::STYLE_NUM_ENTRIES;
    AutoPtr< ArrayOf<Int32> > data = mData;
    if ((*data)[index + CAssetManager::STYLE_TYPE] != ITypedValue::TYPE_NULL) {
        Int32 resid = (*data)[index + CAssetManager::STYLE_RESOURCE_ID];
        if (resid != 0) {
            *value = resid;
            return NOERROR;
        }
    }
    *value = defValue;
    return NOERROR;
}

ECode CTypedArray::GetThemeAttributeId(
    /* [in] */ Int32 index,
    /* [in] */ Int32 defValue,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = defValue;

    if (mRecycled) {
        Logger::W("CTypedArray", "Already recycled!");
        return E_RUNTIME_EXCEPTION;
    }

    index *= CAssetManager::STYLE_NUM_ENTRIES;
    AutoPtr<ArrayOf<Int32> > data = mData;
    if ((*data)[index + CAssetManager::STYLE_TYPE] == ITypedValue::TYPE_ATTRIBUTE) {
        *value = (*data)[index + CAssetManager::STYLE_DATA];
    }
    return defValue;
}

ECode CTypedArray::GetDrawable(
    /* [in] */ Int32 index,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    *drawable = NULL;

    if (mRecycled) {
        Logger::W("CTypedArray", "Already recycled!");
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<CTypedValue> value = mValue;
    if (GetValueAt(index*CAssetManager::STYLE_NUM_ENTRIES, value)) {
        if (value->mType == ITypedValue::TYPE_ATTRIBUTE) {
            // throw new RuntimeException("Failed to resolve attribute at index " + index);
            return E_RUNTIME_EXCEPTION;
        }
        Int32 resourceId;
        value->GetResourceId(&resourceId);
        return mResources->LoadDrawable((ITypedValue*)value.Get(), value->mResourceId, mTheme, drawable);
    }
    *drawable = NULL;
    return NOERROR;
}

ECode CTypedArray::GetTextArray(
    /* [in] */ Int32 index,
    /* [out, callee] */ ArrayOf<ICharSequence*>** array)
{
    VALIDATE_NOT_NULL(array)
    *array = NULL;

    if (mRecycled) {
        Logger::W("CTypedArray", "Already recycled!");
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<CTypedValue> value = mValue;
    if (GetValueAt(index*CAssetManager::STYLE_NUM_ENTRIES, value)) {
        Int32 resourceId;
        value->GetResourceId(&resourceId);
        return mResources->GetTextArray(resourceId, array);
    }
    *array = NULL;
    return NOERROR;
}

ECode CTypedArray::GetValue(
    /* [in] */ Int32 index,
    /* [in, out] */ ITypedValue* outValue,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;
    VALIDATE_NOT_NULL(outValue);

    if (mRecycled) {
        Logger::W("CTypedArray", "Already recycled!");
        return E_RUNTIME_EXCEPTION;
    }

    *value = GetValueAt(index * CAssetManager::STYLE_NUM_ENTRIES, (CTypedValue*)outValue);
    return NOERROR;
}

ECode CTypedArray::GetType(
    /* [in] */ Int32 index,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)
    *type = 0;
    if (mRecycled) {
        Logger::W("CTypedArray", "Already recycled!");
        return E_RUNTIME_EXCEPTION;
    }

    index *= CAssetManager::STYLE_NUM_ENTRIES;
    *type = (*mData)[index + CAssetManager::STYLE_TYPE];
    return NOERROR;
}

ECode CTypedArray::HasValue(
    /* [in] */ Int32 index,
    /* [out] */ Boolean* hasValue)
{
    VALIDATE_NOT_NULL(hasValue);
    *hasValue = FALSE;

    if (mRecycled) {
        Logger::W("CTypedArray", "Already recycled!");
        return E_RUNTIME_EXCEPTION;
    }

    index *= CAssetManager::STYLE_NUM_ENTRIES;
    AutoPtr< ArrayOf<Int32> > data = mData;
    Int32 type = (*data)[index + CAssetManager::STYLE_TYPE];
    *hasValue = type != ITypedValue::TYPE_NULL;
    return NOERROR;
}

ECode CTypedArray::PeekValue(
    /* [in] */ Int32 index,
    /* [out] */ ITypedValue** value)
{
    VALIDATE_NOT_NULL(value);
    *value = NULL;

    if (mRecycled) {
        Logger::W("CTypedArray", "Already recycled!");
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<CTypedValue> v = mValue;
    if (GetValueAt(index * CAssetManager::STYLE_NUM_ENTRIES, v)) {
        *value = (ITypedValue*)v;
        REFCOUNT_ADD(*value);
        return NOERROR;
    }
    *value = NULL;
    return NOERROR;
}

ECode CTypedArray::GetPositionDescription(
    /* [out] */ String* des)
{
    VALIDATE_NOT_NULL(des);
    *des = String("");

    if (mRecycled) {
        Logger::W("CTypedArray", "Already recycled!");
        return E_RUNTIME_EXCEPTION;
    }

    if (mXml != NULL) {
        return mXml->GetPositionDescription(des);
    }
    *des = String("<internal>");
    return NOERROR;
}

ECode CTypedArray::Recycle()
{
    if (mRecycled) {
        Logger::W("CTypedArray", "Already recycled!");
        return E_RUNTIME_EXCEPTION;
    }

    mRecycled = TRUE;

    mXml = NULL;
    mTheme = NULL;

    mResources->mTypedArrayPool->ReleaseItem(this);
    return NOERROR;
}

ECode CTypedArray::ExtractThemeAttrs(
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (mRecycled) {
        // throw new RuntimeException("Cannot make calls to a recycled instance!");
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<ArrayOf<Int32> > attrs;

    AutoPtr<ArrayOf<Int32> > data = mData;
    Int32 N;
    GetLength(&N);
    for (Int32 i = 0; i < N; i++) {
        Int32 index = i * CAssetManager::STYLE_NUM_ENTRIES;
        if ((*data)[index + CAssetManager::STYLE_TYPE] != ITypedValue::TYPE_ATTRIBUTE) {
            continue;
        }

        // Null the entry so that we can safely call getZzz().
        (*data)[index + CAssetManager::STYLE_TYPE] = ITypedValue::TYPE_NULL;

        Int32 attr = (*data)[index + CAssetManager::STYLE_DATA];
        if (attr == 0) {
            // This attribute is useless!
            continue;
        }

        if (attrs == NULL) {
            attrs = ArrayOf<Int32>::Alloc(N);
        }
        (*attrs)[i] = attr;
    }

    *result = attrs;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CTypedArray::GetChangingConfigurations(
        /* [out] */ Int32* cfgs)
{
    VALIDATE_NOT_NULL(cfgs)
    *cfgs = 0;

    Int32 changingConfig = 0;

    AutoPtr<ArrayOf<Int32> > data = mData;
    Int32 N;
    GetLength(&N);
    for (Int32 i = 0; i < N; i++) {
        Int32 index = i * CAssetManager::STYLE_NUM_ENTRIES;
        Int32 type = (*data)[index + CAssetManager::STYLE_TYPE];
        if (type == ITypedValue::TYPE_NULL) {
            continue;
        }
        changingConfig |= (*data)[index + CAssetManager::STYLE_CHANGING_CONFIGURATIONS];
    }

    *cfgs = changingConfig;
    return NOERROR;
}

Boolean CTypedArray::GetValueAt(
    /* [in] */ Int32 index,
    /* [in, out] */ CTypedValue* outValue)
{
    assert(outValue);

    AutoPtr< ArrayOf<Int32> > data = mData;
    Int32 type = (*data)[index + CAssetManager::STYLE_TYPE];
    if (type == ITypedValue::TYPE_NULL) {
        return FALSE;
    }
    outValue->mType = type;
    outValue->mData = (*data)[index + CAssetManager::STYLE_DATA];
    outValue->mAssetCookie = (*data)[index + CAssetManager::STYLE_ASSET_COOKIE];
    outValue->mResourceId = (*data)[index + CAssetManager::STYLE_RESOURCE_ID];
    outValue->mChangingConfigurations = (*data)[index + CAssetManager::STYLE_CHANGING_CONFIGURATIONS];
    outValue->mDensity = (*data)[index + CAssetManager::STYLE_DENSITY];
    outValue->mString = (type == ITypedValue::TYPE_STRING) ?
            LoadStringValueAt(index) : NULL;
    return TRUE;
}

AutoPtr<ICharSequence> CTypedArray::LoadStringValueAt(
    /* [in] */ Int32 index)
{
    AutoPtr< ArrayOf<Int32> > data = mData;
    Int32 cookie = (*data)[index + CAssetManager::STYLE_ASSET_COOKIE];
    if (cookie < 0) {
        if (mXml != NULL) {
            AutoPtr<ICharSequence> csq;
            mXml->GetPooledString((*data)[index + CAssetManager::STYLE_DATA], (ICharSequence**)&csq);
            return csq;
        }
        return NULL;
    }

    CAssetManager* cam = (CAssetManager*)mAssets.Get();
    return cam->GetPooledStringForCookie(cookie, (*data)[index + CAssetManager::STYLE_DATA]);
}


} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos
