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

#ifndef __ELASTOS_DROID_CONTENT_PM_CFEATUREINFO_H__
#define __ELASTOS_DROID_CONTENT_PM_CFEATUREINFO_H__

#include "_Elastos_Droid_Content_Pm_CFeatureInfo.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * A single feature that can be requested by an application. This corresponds
 * to information collected from the
 * AndroidManifest.xml's &lt;uses-feature&gt; tag.
 */
CarClass(CFeatureInfo)
    , public Object
    , public IFeatureInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CFeatureInfo();

    virtual ~CFeatureInfo();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IFeatureInfo* orig);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * This method extracts the major and minor version of reqGLEsVersion attribute
     * and returns it as a string. Say reqGlEsVersion value of 0x00010002 is returned
     * as 1.2
     * @return String representation of the reqGlEsVersion attribute
     */
    CARAPI GetGlEsVersion(
        /* [out] */ String* ver);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI SetName(
        /* [in] */ const String& name);

    CARAPI GetReqGlEsVersion(
        /* [out] */ Int32* reqGlEsVersion);

    CARAPI SetReqGlEsVersion(
        /* [in] */ Int32 reqGlEsVersion);

    CARAPI GetFlags(
        /* [out] */ Int32* flags);

    CARAPI SetFlags(
        /* [in] */ Int32 flags);

public:
    /**
     * The name of this feature, for example "android.hardware.camera".  If
     * this is null, then this is an OpenGL ES version feature as described
     * in {@link #reqGlEsVersion}.
     */
    String mName;

    /**
     * The GLES version used by an application. The upper order 16 bits represent the
     * major version and the lower order 16 bits the minor version.  Only valid
     * if {@link #name} is null.
     */
    Int32 mReqGlEsVersion;

    /**
     * Additional flags.  May be zero or more of {@link #FLAG_REQUIRED}.
     */
    Int32 mFlags;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CFEATUREINFO_H__
