
#ifndef __ELASTOS_DROID_CONTENT_PM_CFEATUREGROUPINFO_H__
#define __ELASTOS_DROID_CONTENT_PM_CFEATUREGROUPINFO_H__

#include "_Elastos_Droid_Content_Pm_CFeatureGroupInfo.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * A set of features that can be requested by an application. This corresponds
 * to information collected from the
 * AndroidManifest.xml's {@code <feature-group>} tag.
 */
CarClass(CFeatureGroupInfo)
    , public Object
    , public IFeatureGroupInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CFeatureGroupInfo();

    virtual ~CFeatureGroupInfo();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IFeatureGroupInfo* other);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI SetFeatures(
        /* [in] */ ArrayOf<IFeatureInfo*>* features);

    CARAPI GetFeatures(
        /* [out, callee] */ ArrayOf<IFeatureInfo*>** features);

public:
    AutoPtr<ArrayOf<IFeatureInfo*> > mFeatures;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CFEATUREGROUPINFO_H__
