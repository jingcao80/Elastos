
#ifndef __ELASTOS_DROID_RENDERSCRIPT_CRENDERSCRIPTHELPER_H__
#define __ELASTOS_DROID_RENDERSCRIPT_CRENDERSCRIPTHELPER_H__

#include "_Elastos_Droid_RenderScript_CRenderScriptHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace RenderScript {

CarClass(CRenderScriptHelper)
    , public Singleton
    , public IRenderScriptHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns an identifier that can be used to identify a particular
     * minor version of RS.
     *
     * @hide
     */
    CARAPI GetMinorID(
        /* [out] */ Int64* id);

     /**
     * Sets the directory to use as a persistent storage for the
     * renderscript object file cache.
     *
     * @hide
     * @param cacheDir A directory the current process can write to
     */
    CARAPI SetupDiskCache(
        /* [in] */ IFile* cacheDir);

    /**
     * @hide
     */
    CARAPI Create(
        /* [in] */ IContext* ctx,
        /* [in] */ Int32 sdkVersion,
        /* [out] */ IRenderScript** rs);

    /**
     * Create a RenderScript context.
     *
     * @hide
     * @param ctx The context.
     * @return RenderScript
     */
    CARAPI Create(
        /* [in] */ IContext* ctx,
        /* [in] */ Int32 sdkVersion,
        /* [in] */ RenderScriptContextType ct,
        /* [in] */ Int32 flags,
        /* [out] */ IRenderScript** rs);

    /**
     * Create a RenderScript context.
     *
     * @param ctx The context.
     * @return RenderScript
     */
    CARAPI Create(
        /* [in] */ IContext* ctx,
        /* [out] */ IRenderScript** rs);

    /**
     * Create a RenderScript context.
     *
     *
     * @param ctx The context.
     * @param ct The type of context to be created.
     * @return RenderScript
     */
    CARAPI Create(
        /* [in] */ IContext* ctx,
        /* [in] */ RenderScriptContextType ct,
        /* [out] */ IRenderScript** rs);

     /**
     * Create a RenderScript context.
     *
     *
     * @param ctx The context.
     * @param ct The type of context to be created.
     * @param flags The OR of the CREATE_FLAG_* options desired
     * @return RenderScript
     */
    CARAPI Create(
        /* [in] */ IContext* ctx,
        /* [in] */ RenderScriptContextType ct,
        /* [in] */ Int32 flags,
        /* [out] */ IRenderScript** rs);
};


} // namespace RenderScript
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_RENDERSCRIPT_CRENDERSCRIPTHELPER_H__
