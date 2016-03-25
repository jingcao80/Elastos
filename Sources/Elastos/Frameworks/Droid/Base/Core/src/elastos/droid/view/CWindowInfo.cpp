
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Os.h"
#include "elastos/droid/view/CWindowInfo.h"
#include "elastos/droid/graphics/CRect.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Graphics::CRect;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace View {

const Int32 CWindowInfo::MAX_POOL_SIZE = 10;
AutoPtr<Pools::SynchronizedPool<IWindowInfo> > CWindowInfo::sPool =
        new Pools::SynchronizedPool<IWindowInfo>(MAX_POOL_SIZE);

CAR_OBJECT_IMPL(CWindowInfo);
CAR_INTERFACE_IMPL_2(CWindowInfo, Object, IWindowInfo, IParcelable);
CWindowInfo::CWindowInfo()
    : mType(0)
    , mLayer(0)
    , mFocused(FALSE)
{
    CRect::New((IRect**)&mBoundsInScreen);
}

ECode CWindowInfo::constructor()
{
    return NOERROR;
}

ECode CWindowInfo::Obtain(
    /* [out] */ IWindowInfo** info)
{
    AutoPtr<IWindowInfo> window = sPool->AcquireItem();
    if (window == NULL) {
        CWindowInfo::New((IWindowInfo**)&window);
    }
    *info = window;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

ECode CWindowInfo::Obtain(
    /* [in] */ IWindowInfo *_other,
    /* [out] */ IWindowInfo** info)
{
    AutoPtr<IWindowInfo> _window;
    Obtain((IWindowInfo**)&_window);
    CWindowInfo* window = (CWindowInfo*)_window.Get();
    CWindowInfo* other = (CWindowInfo*)_other;
    window->mType = other->mType;
    window->mLayer = other->mLayer;
    window->mToken = other->mToken;
    window->mParentToken = other->mParentToken;
    window->mFocused = other->mFocused;
    window->mBoundsInScreen->Set(other->mBoundsInScreen);

    Boolean isEmpty = FALSE;
    if (other->mChildTokens != NULL && (other->mChildTokens->IsEmpty(&isEmpty), !isEmpty)) {
        if (window->mChildTokens == NULL) {
            CArrayList::New(ICollection::Probe(other->mChildTokens), (IList**)&window->mChildTokens);
        } else {
            window->mChildTokens->AddAll(ICollection::Probe(other->mChildTokens));
        }
    }

    *info = _window;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

ECode CWindowInfo::Recycle()
{
    Clear();
    // sPool->ReleaseItem(this);
    return NOERROR;
}

ECode CWindowInfo::WriteToParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->WriteInt32(mType);
    parcel->WriteInt32(mLayer);
    parcel->WriteInterfacePtr(mToken);
    parcel->WriteInterfacePtr(mParentToken);
    parcel->WriteInt32(mFocused ? 1 : 0);
    parcel->WriteInterfacePtr(mBoundsInScreen);

    Boolean isEmpty = FALSE;
    if (mChildTokens != NULL && (mChildTokens->IsEmpty(&isEmpty), !isEmpty)) {
        parcel->WriteInt32(1);
        Int32 size = 0;
        mChildTokens->GetSize(&size);
        parcel->WriteInt32(size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IBinder> binder;
            mChildTokens->Get(i, (IInterface**)&binder);
            parcel->WriteInterfacePtr(binder);
        }
    } else {
        parcel->WriteInt32(0);
    }
    return NOERROR;
}

ECode CWindowInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return InitFromParcel(source);
}

ECode CWindowInfo::ToString(
    /* [out] */ String* str)
{
    StringBuilder builder;
    builder.Append("WindowInfo[");
    builder.Append("type=");
    builder.Append(mType);
    builder.Append(", layer=");
    builder.Append(mLayer);
    builder.Append(", token=");
    builder.Append(mToken.Get());
    builder.Append(", bounds=");
    builder.Append(mBoundsInScreen.Get());
    builder.Append(", parent=");
    builder.Append(mParentToken.Get());
    builder.Append(", focused=");
    builder.Append(mFocused);
    builder.Append(", children=");
    builder.Append(mChildTokens.Get());
    builder.AppendChar(']');
    return builder.ToString(str);
}

ECode CWindowInfo::InitFromParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->ReadInt32(&mType);
    parcel->ReadInt32(&mLayer);
    parcel->ReadInterfacePtr((Handle32*)&mToken);
    parcel->ReadInterfacePtr((Handle32*)&mParentToken);
    Int32 value = 0;
    mFocused = ((parcel->ReadInt32(&value), value) == 1);
    parcel->ReadInterfacePtr((Handle32*)&mBoundsInScreen);

    Boolean hasChildren = ((parcel->ReadInt32(&value), value) == 1);
    if (hasChildren) {
        if (mChildTokens == NULL) {
            CArrayList::New((IList**)&mChildTokens);
        }
        parcel->ReadInt32(&value);
        for (Int32 i = 0; i < value; i++) {
            AutoPtr<IInterface> binder;
            parcel->ReadInterfacePtr((Handle32*)&binder);
            mChildTokens->Add(binder);
        }
    }
    return NOERROR;
}

void CWindowInfo::Clear()
{
    mType = 0;
    mLayer = 0;
    mToken = NULL;
    mParentToken = NULL;
    mFocused = FALSE;
    mBoundsInScreen->SetEmpty();
    if (mChildTokens != NULL) {
        mChildTokens->Clear();
    }
}

ECode CWindowInfo::SetType(
    /* [in] */ Int32 type)
{
    mType = type;
    return NOERROR;
}

ECode CWindowInfo::SetLayer(
    /* [in] */ Int32 layer)
{
    mLayer = layer;
    return NOERROR;
}

ECode CWindowInfo::SetToken(
    /* [in] */ IBinder* token)
{
    mToken = token;
    return NOERROR;
}

ECode CWindowInfo::SetParentToken(
    /* [in] */ IBinder* token)
{
    mParentToken = token;
    return NOERROR;
}

ECode CWindowInfo::SetFocused(
    /* [in] */ Boolean focused)
{
    mFocused = focused;
    return NOERROR;
}

ECode CWindowInfo::SetBoundsInScreen(
    /* [in] */ IRect* rect)
{
    mBoundsInScreen = rect;
    return NOERROR;
}

ECode CWindowInfo::SetChildTokens(
    /* [in] */ IList* tokens)
{
    mChildTokens = tokens;
    return NOERROR;
}

ECode CWindowInfo::GetType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mType;
    return NOERROR;
}

ECode CWindowInfo::GetLayer(
    /* [out] */ Int32* layer)
{
    VALIDATE_NOT_NULL(layer);
    *layer = mLayer;
    return NOERROR;
}

ECode CWindowInfo::GetToken(
    /* [out] */ IBinder** token)
{
    VALIDATE_NOT_NULL(token);
    *token = mToken;
    REFCOUNT_ADD(*token);
    return NOERROR;
}

ECode CWindowInfo::GetParentToken(
    /* [out] */ IBinder** token)
{
    VALIDATE_NOT_NULL(token);
    *token = mParentToken;
    REFCOUNT_ADD(*token);
    return NOERROR;
}

ECode CWindowInfo::GetFocused(
    /* [out] */ Boolean* focused)
{
    VALIDATE_NOT_NULL(focused);
    *focused = mFocused;
    return NOERROR;
}

ECode CWindowInfo::GetBoundsInScreen(
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);
    *rect = mBoundsInScreen;
    REFCOUNT_ADD(*rect);
    return NOERROR;
}

ECode CWindowInfo::GetChildTokens(
    /* [out] */ IList** tokens)
{
    VALIDATE_NOT_NULL(tokens);
    *tokens = mChildTokens;
    REFCOUNT_ADD(*tokens);
    return NOERROR;
}

}// namespace View
}// namespace Droid
}// namespace Elastos
