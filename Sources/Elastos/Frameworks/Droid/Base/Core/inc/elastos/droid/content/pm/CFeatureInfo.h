
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
