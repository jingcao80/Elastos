#ifndef __ELASTOS_APPS_DIALER_UTIL_EMPTYLOADER_H__
#define __ELASTOS_APPS_DIALER_UTIL_EMPTYLOADER_H__

using Elastos::Droid::Content::Loader;

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Util {

/**
 * A {@link Loader} only used to make use of the {@link android.app.Fragment#setStartDeferred}
 * feature from an old-style fragment which doesn't use {@link Loader}s to load data.
 *
 * This loader never delivers results.  A caller fragment must destroy it when deferred fragments
 * should be started.
 */
class EmptyLoader
    : public Loader
    , public IEmptyLoader
{
public:
    /**
     * {@link LoaderCallbacks} which just generates {@link EmptyLoader}.  {@link #onLoadFinished}
     * and {@link #onLoaderReset} are no-op.
     */
    class Callback
        : public Object
        , public IEmptyLoaderCallback
        , public ILoaderManagerLoaderCallbacks
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
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
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);
};

} // Util
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_UTIL_EMPTYLOADER_H__
