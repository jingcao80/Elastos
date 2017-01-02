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

#ifndef __ELASTOS_DROID_DIALER_UTIL_EMPTYLOADER_H__
#define __ELASTOS_DROID_DIALER_UTIL_EMPTYLOADER_H__

#include "_Elastos.Droid.Dialer.h"
#include "Elastos.Droid.App.h"
#include "elastos/droid/content/Loader.h"

using Elastos::Droid::App::ILoaderManagerLoaderCallbacks;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::ILoader;
using Elastos::Droid::Content::Loader;
using Elastos::Droid::Os::IBundle;

namespace Elastos{
namespace Droid{
namespace Dialer {
namespace Util {

/**
 * A {@link Loader} only used to make use of the {@link android.app.Fragment#setStartDeferred}
 * feature from an old-style fragment which doesn't use {@link Loader}s to load data.
 *
 * This loader never delivers results.  A caller fragment must destroy it when deferred fragments
 * should be started.
 */
class EmptyLoader : public Loader
{
public:
    /**
     * {@link LoaderCallbacks} which just generates {@link EmptyLoader}.  {@link #onLoadFinished}
     * and {@link #onLoaderReset} are no-op.
     */
    class Callback
        : public Object
        , public ILoaderManagerLoaderCallbacks
    {
    public:
        CAR_INTERFACE_DECL()

        Callback(
            /* [in] */ IContext* context);

        // @Override
        CARAPI OnCreateLoader(
            /* [in] */ Int32 id,
            /* [in] */ IBundle* args,
            /* [out] */ ILoader** loader);

        // @Override
        CARAPI OnLoadFinished(
            /* [in] */ ILoader* loader,
            /* [in] */ IInterface* data);

        // @Override
        CARAPI OnLoaderReset(
            /* [in] */ ILoader* loader);

    private:
        AutoPtr<IContext> mContext;
    };

public:
    CARAPI constructor(
        /* [in] */ IContext* context);
};

} // Util
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_UTIL_EMPTYLOADER_H__
