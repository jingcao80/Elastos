
#ifndef __ELASTOS_DROID_VIEW_CWINDOWINFO_H__
#define __ELASTOS_DROID_VIEW_CWINDOWINFO_H__

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_View_CWindowInfo.h"
#include "elastos/droid/utility/Pools.h"

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Utility::Pools;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CWindowInfo)
    , public Object
    , public IWindowInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CWindowInfo();

    CARAPI constructor();

    CARAPI ReadFromParcel(
        /* [in] */ IParcel *source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel *dest);

    static CARAPI Obtain(
        /* [out] */ IWindowInfo** info);

    static CARAPI Obtain(
        /* [in] */ IWindowInfo *other,
        /* [out] */ IWindowInfo** info);

    CARAPI Recycle();

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI SetType(
        /* [in] */ Int32 type);

    CARAPI SetLayer(
        /* [in] */ Int32 layer);

    CARAPI SetToken(
        /* [in] */ IBinder* token);

    CARAPI SetParentToken(
        /* [in] */ IBinder* token);

    CARAPI SetFocused(
        /* [in] */ Boolean focused);

    CARAPI SetBoundsInScreen(
        /* [in] */ IRect* rect);

    CARAPI SetChildTokens(
        /* [in] */ IList* tokens);

    CARAPI GetType(
        /* [out] */ Int32* type);

    CARAPI GetLayer(
        /* [out] */ Int32* layer);

    CARAPI GetToken(
        /* [out] */ IBinder** token);

    CARAPI GetParentToken(
        /* [out] */ IBinder** token);

    CARAPI GetFocused(
        /* [out] */ Boolean* focused);

    CARAPI GetBoundsInScreen(
        /* [out] */ IRect** rect);

    CARAPI GetChildTokens(
        /* [out] */ IList** tokens);

private:
    CARAPI_(void) Clear();

    CARAPI InitFromParcel(
        /* [in] */ IParcel* parcel);

private:
    static const Int32 MAX_POOL_SIZE;
    static AutoPtr<Pools::SynchronizedPool<IWindowInfo> > sPool;

    Int32 mType;
    Int32 mLayer;
    AutoPtr<IBinder> mToken;
    AutoPtr<IBinder> mParentToken;
    Boolean mFocused;
    AutoPtr<IRect> mBoundsInScreen;
    AutoPtr<IList> mChildTokens;
};

}// namespace View
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_VIEW_CWINDOWINFO_H__
