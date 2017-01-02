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
