
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/accessibility/CAccessibilityWindowInfo.h"
#include "elastos/droid/view/accessibility/CAccessibilityInteractionClient.h"
#include "elastos/droid/graphics/CRect.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Graphics::CRect;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

const Boolean CAccessibilityWindowInfo::DEBUG = FALSE;
const Int32 CAccessibilityWindowInfo::UNDEFINED = -1;

const Int32 CAccessibilityWindowInfo::BOOLEAN_PROPERTY_ACTIVE = 1 << 0;
const Int32 CAccessibilityWindowInfo::BOOLEAN_PROPERTY_FOCUSED = 1 << 1;
const Int32 CAccessibilityWindowInfo::BOOLEAN_PROPERTY_ACCESSIBLITY_FOCUSED = 1 << 2;

const Int32 CAccessibilityWindowInfo::MAX_POOL_SIZE = 10;
AutoPtr<Pools::SynchronizedPool<IAccessibilityWindowInfo> > CAccessibilityWindowInfo::sPool =
        new Pools::SynchronizedPool<IAccessibilityWindowInfo>(MAX_POOL_SIZE);

CAR_INTERFACE_IMPL_2(CAccessibilityWindowInfo, Object, IAccessibilityWindowInfo, IParcelable)

CAR_OBJECT_IMPL(CAccessibilityWindowInfo)

CAccessibilityWindowInfo::CAccessibilityWindowInfo()
    : mType(UNDEFINED)
    , mLayer(UNDEFINED)
    , mBooleanProperties(0)
    , mId(UNDEFINED)
    , mParentId(UNDEFINED)
    , mConnectionId(UNDEFINED)
{
    ASSERT_SUCCEEDED(CRect::New((IRect**)&mBoundsInScreen));
}

CAccessibilityWindowInfo::~CAccessibilityWindowInfo()
{
}

ECode CAccessibilityWindowInfo::constructor()
{
    return NOERROR;
}

ECode CAccessibilityWindowInfo::GetType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mType;
    return NOERROR;
}

ECode CAccessibilityWindowInfo::SetType(
    /* [in] */ Int32 type)
{
    mType = type;
    return NOERROR;
}

ECode CAccessibilityWindowInfo::GetLayer(
    /* [out] */ Int32* layer)
{
    VALIDATE_NOT_NULL(layer);
    *layer = mLayer;
    return NOERROR;
}

ECode CAccessibilityWindowInfo::SetLayer(
    /* [in] */ Int32 layer)
{
    mLayer = layer;
    return NOERROR;
}

ECode CAccessibilityWindowInfo::GetRoot(
    /* [out] */ IAccessibilityNodeInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    if (mConnectionId == UNDEFINED) {
        return NOERROR;
    }

    AutoPtr<IAccessibilityInteractionClient> client = CAccessibilityInteractionClient::GetInstance();
    return client->FindAccessibilityNodeInfoByAccessibilityId(mConnectionId, mId, IAccessibilityNodeInfo::ROOT_NODE_ID,
            TRUE, IAccessibilityNodeInfo::FLAG_PREFETCH_DESCENDANTS, info);
}

ECode CAccessibilityWindowInfo::GetParent(
    /* [out] */ IAccessibilityWindowInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    if (mConnectionId == UNDEFINED || mParentId == UNDEFINED) {
        return NOERROR;
    }

    AutoPtr<IAccessibilityInteractionClient> client = CAccessibilityInteractionClient::GetInstance();
    return client->GetWindow(mConnectionId, mParentId, info);
}

ECode CAccessibilityWindowInfo::SetParentId(
    /* [in] */ Int32 parentId)
{
    mParentId = parentId;
    return NOERROR;
}

ECode CAccessibilityWindowInfo::GetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mId;
    return NOERROR;
}

ECode CAccessibilityWindowInfo::SetId(
    /* [in] */ Int32 id)
{
    mId = id;
    return NOERROR;
}

ECode CAccessibilityWindowInfo::SetConnectionId(
    /* [in] */ Int32 connectionId)
{
    mConnectionId = connectionId;
    return NOERROR;
}

ECode CAccessibilityWindowInfo::GetBoundsInScreen(
    /* [in, out] */ IRect* outBounds)
{
    return outBounds->Set(mBoundsInScreen);
}

ECode CAccessibilityWindowInfo::SetBoundsInScreen(
    /* [in] */ IRect* bounds)
{
    return mBoundsInScreen->Set(bounds);
}

ECode CAccessibilityWindowInfo::IsActive(
    /* [out] */ Boolean* active)
{
    VALIDATE_NOT_NULL(active);
    *active = GetBooleanProperty(BOOLEAN_PROPERTY_ACTIVE);
    return NOERROR;
}

ECode CAccessibilityWindowInfo::SetActive(
    /* [in] */ Boolean active)
{
    SetBooleanProperty(BOOLEAN_PROPERTY_ACTIVE, active);
    return NOERROR;
}

ECode CAccessibilityWindowInfo::IsFocused(
    /* [out] */ Boolean* focused)
{
    VALIDATE_NOT_NULL(focused);
    *focused = GetBooleanProperty(BOOLEAN_PROPERTY_FOCUSED);
    return NOERROR;
}

ECode CAccessibilityWindowInfo::SetFocused(
    /* [in] */ Boolean focused)
{
    SetBooleanProperty(BOOLEAN_PROPERTY_FOCUSED, focused);
    return NOERROR;
}

ECode CAccessibilityWindowInfo::IsAccessibilityFocused(
    /* [out] */ Boolean* focused)
{
    VALIDATE_NOT_NULL(focused);
    *focused = GetBooleanProperty(BOOLEAN_PROPERTY_ACCESSIBLITY_FOCUSED);
    return NOERROR;
}

ECode CAccessibilityWindowInfo::SetAccessibilityFocused(
    /* [in] */ Boolean focused)
{
    SetBooleanProperty(BOOLEAN_PROPERTY_ACCESSIBLITY_FOCUSED, focused);
    return NOERROR;
}

ECode CAccessibilityWindowInfo::GetChildCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    count = 0;
    if (mChildIds != NULL) {
        mChildIds->GetSize(count);
    }
    return NOERROR;
}

ECode CAccessibilityWindowInfo::GetChild(
    /* [in] */ Int32 index,
    /* [out] */ IAccessibilityWindowInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;
    if (mChildIds == NULL) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (mConnectionId == UNDEFINED) {
        return NOERROR;
    }

    Int64 id;
    mChildIds->Get(index, &id);
    Int32 childId = (Int32)id;

    AutoPtr<IAccessibilityInteractionClient> client = CAccessibilityInteractionClient::GetInstance();
    return client->GetWindow(mConnectionId, childId, info);
}

ECode CAccessibilityWindowInfo::AddChild(
    /* [in] */ Int32 childId)
{
    if (mChildIds == NULL) {
        CInt64Array::New((IInt64Array**)&mChildIds);
    }

    mChildIds->Add(childId);
    return NOERROR;
}

ECode CAccessibilityWindowInfo::Obtain(
    /* [out] */ IAccessibilityWindowInfo** info)
{
    VALIDATE_NOT_NULL(info);
    AutoPtr<IAccessibilityWindowInfo> _info = sPool->AcquireItem();

    if (_info == NULL) {
        AutoPtr<IAccessibilityWindowInfo> windowInfo;
        CAccessibilityWindowInfo::New((IAccessibilityWindowInfo**)&windowInfo);
        *info = windowInfo;
        REFCOUNT_ADD(*info);
        return NOERROR;
    }

    *info = _info;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

ECode CAccessibilityWindowInfo::Obtain(
    /* [in] */ IAccessibilityWindowInfo* info,
    /* [out] */ IAccessibilityWindowInfo** cloneInfo)
{
    AutoPtr<IAccessibilityWindowInfo> infoClone;
    Obtain((IAccessibilityWindowInfo**)&infoClone);

    AutoPtr<CAccessibilityWindowInfo> _info = (CAccessibilityWindowInfo*)info;
    AutoPtr<CAccessibilityWindowInfo> _infoClone = (CAccessibilityWindowInfo*)infoClone.Get();

    _infoClone->mType = _info->mType;
    _infoClone->mLayer = _info->mLayer;
    _infoClone->mBooleanProperties = _info->mBooleanProperties;
    _infoClone->mId = _info->mId;
    _infoClone->mParentId = _info->mParentId;
    _infoClone->mBoundsInScreen->Set(_info->mBoundsInScreen);

    Int32 size;
    if (_info->mChildIds != NULL && (_info->mChildIds->GetSize(&size), size) > 0) {
        if (_infoClone->mChildIds == NULL) {
            AutoPtr<IInterface> obj;
            ICloneable::Probe(_info->mChildIds)->Clone((IInterface**)&obj);
            _infoClone->mChildIds = IInt64Array::Probe(obj);
        }
        else {
            _infoClone->mChildIds->AddAll(_info->mChildIds);
        }
    }

    _infoClone->mConnectionId = _info->mConnectionId;

    *cloneInfo = (IAccessibilityWindowInfo*)_infoClone.Get();
    REFCOUNT_ADD(*cloneInfo);

    return NOERROR;
}

ECode CAccessibilityWindowInfo::Recycle()
{
    Clear();
    sPool->ReleaseItem((IAccessibilityWindowInfo*)this);
    return NOERROR;
}

ECode CAccessibilityWindowInfo::WriteToParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->WriteInt32(mType);
    parcel->WriteInt32(mLayer);
    parcel->WriteInt32(mBooleanProperties);
    parcel->WriteInt32(mId);
    parcel->WriteInt32(mParentId);
    parcel->WriteInterfacePtr((IInterface*)mBoundsInScreen);

    AutoPtr<IInt64Array> childIds = mChildIds;
    if (childIds == NULL) {
        parcel->WriteInt32(0);
    }
    else {
        Int32 childCount;
        childIds->GetSize(&childCount);
        parcel->WriteInt32(childCount);
        for (Int32 i = 0; i < childCount; ++i) {
            Int64 id;
            childIds->Get(i, &id);
            parcel->WriteInt32((Int32)id);
        }
    }

    parcel->WriteInt32(mConnectionId);
    return NOERROR;
}

ECode CAccessibilityWindowInfo::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->ReadInt32(&mType);
    parcel->ReadInt32(&mLayer);
    parcel->ReadInt32(&mBooleanProperties);
    parcel->ReadInt32(&mId);
    parcel->ReadInt32(&mParentId);

    AutoPtr<IInterface> obj;
    parcel->ReadInterfacePtr((Handle32*)&obj);
    mBoundsInScreen = IRect::Probe(obj);

    Int32 childCount;
    parcel->ReadInt32(&childCount);

    if (childCount > 0) {
        if (mChildIds == NULL) {
            CInt64Array::New(childCount, (IInt64Array**)&mChildIds);
        }
        for (Int32 i = 0; i < childCount; i++) {
            Int32 childId;
            parcel->ReadInt32(&childId);
            mChildIds->Add((Int64)childId);
        }
    }

    parcel->ReadInt32(&mConnectionId);
    return NOERROR;
}

ECode CAccessibilityWindowInfo::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash);
    *hash = mId;
    return NOERROR;
}

ECode CAccessibilityWindowInfo::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if ((IAccessibilityWindowInfo*)this == IAccessibilityWindowInfo::Probe(obj)) {
        *result = TRUE;
        return NOERROR;
    }

    if (obj == NULL) {
        return NOERROR;
    }

    AutoPtr<CAccessibilityWindowInfo> other = (CAccessibilityWindowInfo*)IAccessibilityWindowInfo::Probe(obj);
    *result = (mId == other->mId);
    return NOERROR;
}

ECode CAccessibilityWindowInfo::ToString(
    /* [out] */ String* s)
{
    VALIDATE_NOT_NULL(s);
    AutoPtr<StringBuilder> builder = new StringBuilder();
    builder->Append("AccessibilityWindowInfo[");
    builder->Append("id=");
    builder->Append(mId);
    builder->Append(", type=");
    builder->Append(TypeToString(mType));
    builder->Append(", layer=");
    builder->Append(mLayer);
    builder->Append(", bounds=");
    builder->Append(mBoundsInScreen);
    builder->Append(", focused=");
    Boolean res;
    builder->Append((IsFocused(&res), res));
    builder->Append(", active=");
    builder->Append((IsActive(&res), res));
    if (DEBUG) {
        builder->Append(", parent=");
        builder->Append(mParentId);
        builder->Append(", children=[");
        if (mChildIds != NULL) {
            Int32 childCount;
            mChildIds->GetSize(&childCount);
            for (Int32 i = 0; i < childCount; i++) {
                Int64 value;
                mChildIds->Get(i, &value);
                builder->Append(value);
                if (i < childCount - 1) {
                    builder->AppendChar(',');
                }
            }
        }
        else {
            builder->Append("null");
        }
        builder->AppendChar(']');
    }
    else {
        builder->Append(", hasParent=");
        builder->Append(mParentId != UNDEFINED);
        builder->Append(", hasChildren=");
        Int32 size;
        mChildIds->GetSize(&size);
        builder->Append(mChildIds != NULL && size > 0);
    }

    builder->AppendChar(']');
    *s = builder->ToString();
    return NOERROR;
}

void CAccessibilityWindowInfo::Clear()
{
    mType = UNDEFINED;
    mLayer = UNDEFINED;
    mBooleanProperties = 0;
    mId = UNDEFINED;
    mParentId = UNDEFINED;
    mBoundsInScreen->SetEmpty();
    if (mChildIds != NULL) {
        mChildIds->Clear();
    }
    mConnectionId = UNDEFINED;
}

Boolean CAccessibilityWindowInfo::GetBooleanProperty(
    /* [in] */ Int32 property)
{
    return (mBooleanProperties & property) != 0;
}

ECode CAccessibilityWindowInfo::SetBooleanProperty(
    /* [in] */ Int32 property,
    /* [in] */ Boolean value)
{
    if (value) {
        mBooleanProperties |= property;
    }
    else {
        mBooleanProperties &= ~property;
    }
    return NOERROR;
}

String CAccessibilityWindowInfo::TypeToString(
    /* [in] */ Int32 type)
{
    switch (type) {
        case TYPE_APPLICATION: {
            return String("TYPE_APPLICATION");
        }
        case TYPE_INPUT_METHOD: {
            return String("TYPE_INPUT_METHOD");
        }
        case TYPE_SYSTEM: {
            return String("TYPE_SYSTEM");
        }
        default:
            return String("<UNKNOWN>");
    }
}

ECode CAccessibilityWindowInfo::Changed(
    /* [in] */ IAccessibilityWindowInfo* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<CAccessibilityWindowInfo> info = (CAccessibilityWindowInfo*)other;
    if (info->mId != mId) {
        Slogger::E(DEBUG, "Not same window.");
        // throw new IllegalArgumentException("Not same window.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (info->mType != mType) {
        Slogger::E(DEBUG, "Not same type.");
        // throw new IllegalArgumentException("Not same type.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Boolean res;
    if (!(mBoundsInScreen->Equals(mBoundsInScreen, &res), res)) {
        *result = TRUE;
        return NOERROR;
    }
    if (mLayer != info->mLayer) {
        *result = TRUE;
        return NOERROR;
    }
    if (mBooleanProperties != info->mBooleanProperties) {
        *result = TRUE;
        return NOERROR;
    }
    if (mParentId != info->mParentId) {
        *result = TRUE;
        return NOERROR;
    }
    if (mChildIds == NULL) {
        if (info->mChildIds != NULL) {
            *result = TRUE;
            return NOERROR;
        }
    }
    else if ((IObject::Probe(mChildIds)->Equals(info->mChildIds, &res), !res)) {
        *result = TRUE;
        return NOERROR;
    }
    return NOERROR;
}

} // Accessibility
} // View
} // Droid
} // Elastos
