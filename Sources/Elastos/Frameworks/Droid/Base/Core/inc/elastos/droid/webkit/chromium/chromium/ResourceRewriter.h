
#ifndef __ELASTOS_DROID_CHROMIUM_CHROMIUM_RESOURCEREWRITER_H__
#define __ELASTOS_DROID_CHROMIUM_CHROMIUM_RESOURCEREWRITER_H__

// import android.content.Context;
// import android.util.SparseArray;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Chromium {
namespace Chromium {

/**
 * Helper class used to fix up resource ids.
 * This is mostly a copy of the code in frameworks/base/core/java/android/app/LoadedApk.java.
 * TODO: Remove if a cleaner mechanism is provided (either public API or AAPT is changed to generate
 * this code).
 */
class ResourceRewriter
{
public:
    static CARAPI_(void) RewriteRValues(
        /* [in] */ IContext* ctx);
};

} // namespace Chromium
} // namespace Chromium
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_CHROMIUM_CHROMIUM_RESOURCEREWRITER_H__
