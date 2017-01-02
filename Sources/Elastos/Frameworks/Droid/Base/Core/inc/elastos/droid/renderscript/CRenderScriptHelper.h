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
