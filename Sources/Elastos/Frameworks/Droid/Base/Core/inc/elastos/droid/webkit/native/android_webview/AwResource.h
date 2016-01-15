#ifndef __ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_AWRESOURCE_H__
#define __ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_AWRESOURCE_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Utility::ISparseArray;

// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;

// import java.io.IOException;
// import java.lang.ref.SoftReference;
// import java.util.NoSuchElementException;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace AndroidWebview {

/**
 * A class that defines a set of resource IDs and functionality to resolve
 * those IDs to concrete resources.
 */
//@JNINamespace("android_webview::AwResource")
class AwResource
    : public Object
{
public:
    static CARAPI_(void) SetResources(
        /* [in] */ IResources* resources);

    static CARAPI_(void) SetErrorPageResources(
        /* [in] */ Int32 loaderror,
        /* [in] */ Int32 nodomain);

    static CARAPI_(void) SetDefaultTextEncoding(
        /* [in] */ Int32 encoding);

    static CARAPI_(void) SetConfigKeySystemUuidMapping(
        /* [in] */ Int32 config);

    //@CalledByNative
    static CARAPI_(String) GetDefaultTextEncoding();

    //@CalledByNative
    static CARAPI_(String) GetNoDomainPageContent();

    //@CalledByNative
    static CARAPI_(String) GetLoadErrorPageContent();

    static CARAPI_(AutoPtr<ArrayOf<String> >) GetConfigKeySystemUuidMapping();

private:
    static CARAPI_(String) GetResource(
        /* [in] */ Int32 resid,
        /* [in] */ Int32 type);

    static CARAPI_(String) GetRawFileResourceContent(
        /* [in] */ Int32 resid);

//callback function declaration
public:
    static CARAPI_(void*) ElaAwResourceCallback_Init();

private:
    // The following resource ID's must be initialized by the embedder.

    // Raw resource ID for an HTML page to be displayed in the case of
    // a specific load error.
    static Int32 RAW_LOAD_ERROR;

    // Raw resource ID for an HTML page to be displayed in the case of
    // a generic load error. (It's called NO_DOMAIN for legacy reasons).
    static Int32 RAW_NO_DOMAIN;

    // Array resource ID for the configuration of platform specific key-systems.
    static Int32 STRING_ARRAY_CONFIG_KEY_SYSTEM_UUID_MAPPING;

    // The embedder should inject a Resources object that will be used
    // to resolve Resource IDs into the actual resources.
    static AutoPtr<IResources> sResources;

    // Loading some resources is expensive, so cache the results.
    //static SparseArray<SoftReference<String>> sResourceCache;
    //static AutoPtr<ISparseArray> sResourceCache;//TODO now web use HashMap replace this
    static HashMap<Int32, AutoPtr<IWeakReference> > sResourceCache;

    static const Int32 TYPE_STRING = 0;
    static const Int32 TYPE_RAW = 1;
};

} // namespace AndroidWebview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_AWRESOURCE_H__
