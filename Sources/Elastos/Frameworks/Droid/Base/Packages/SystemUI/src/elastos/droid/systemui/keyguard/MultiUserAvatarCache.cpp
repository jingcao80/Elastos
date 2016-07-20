
#include "elastos/droid/systemui/keyguard/MultiUserAvatarCache.h"
#include "Elastos.Droid.View.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

MultiUserAvatarCache::MultiUserAvatarCache()
{
    mCache = CHashMap::New((IHashMap**)&mCache);
}

AutoPtr<MultiUserAvatarCache> MultiUserAvatarCache::GetInstance()
{
    if (sInstance == NULL) {
        sInstance = new MultiUserAvatarCache();
    }
    return sInstance;
}

ECode MultiUserAvatarCache::Clear(
    /* [in] */ Int32 userId)
{
    AutoPtr<IInteger32> key = CoreUtils::Convert(userId);
    return mCache->Remove(TO_IINTERFACE(key));
}

ECode MultiUserAvatarCache::Get(
    /* [in] */ Int32 userId,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable)

    AutoPtr<IInteger32> key = CoreUtils::Convert(userId);
    AutoPtr<IInterface> obj;
    mCache->Get(TO_IINTERFACE(key), (IInterface**)&obj);
    *drawable = IDrawable::Probe(obj);
    REFCOUNT_ADD(*drawable);
    return NOERROR;
}

ECode MultiUserAvatarCache::Put(
    /* [in] */ Int32 userId,
    /* [in] */ IDrawable* image)
{
    AutoPtr<IInteger32> key = CoreUtils::Convert(userId);
    return mCache->Put(TO_IINTERFACE(key), TO_IINTERFACE(image));
}


} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
