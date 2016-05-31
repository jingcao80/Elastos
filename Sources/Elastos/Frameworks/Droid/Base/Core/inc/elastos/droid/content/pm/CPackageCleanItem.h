
#ifndef __ELASTOS_DROID_CONTENT_PM_CPACKAGECLEANITEM_H__
#define __ELASTOS_DROID_CONTENT_PM_CPACKAGECLEANITEM_H__

#include "_Elastos_Droid_Content_Pm_CPackageCleanItem.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/** @hide */
CarClass(CPackageCleanItem)
    , public Object
    , public IPackageCleanItem
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CPackageCleanItem();

    ~CPackageCleanItem();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 userId,
        /* [in] */ const String& packageName,
        /* [in] */ Boolean andCode);

    CARAPI Equals(
        /* [in] */ IPackageCleanItem* obj,
        /* [out] */ Boolean* result);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* code);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetUserId(
        /* [out] */ Int32* userId);

    CARAPI GetPackageName(
        /* [out] */ String* packageName);

    CARAPI GetAndCode(
        /* [out] */ Boolean* andCode);

    CARAPI SetUserId(
        /* [in] */ Int32 userId);

    CARAPI SetPackageName(
        /* [in] */ const String& packageName);

    CARAPI SetAndCode(
        /* [in] */ Boolean andCode);


public:
    Int32 mUserId;
    String mPackageName;
    Boolean mAndCode;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CPACKAGECLEANITEM_H__
