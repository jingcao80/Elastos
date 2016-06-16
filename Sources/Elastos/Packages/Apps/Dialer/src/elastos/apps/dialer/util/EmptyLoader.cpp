
#include "util/EmptyLoader.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Util {

//=================================================================
// EmptyLoader::Callback
//=================================================================
CAR_INTERFACE_IMPL(EmptyLoader::Callback, Object, IEmptyLoaderCallback, ILoaderManagerLoaderCallbacks);

ECode EmptyLoader::Callback::constructor(
    /* [in] */ IContext* context)
{
    return context->GetApplicationContext((IContext**)&mContext);
}

ECode EmptyLoader::Callback::OnCreateLoader(
    /* [in] */ Int32 id,
    /* [in] */ IBundle* args,
    /* [out] */ ILoader** loader)
{
    VALUE_NOT_NULL(loader);
    return CEmptyLoader::New((ILoader**)&mContext);
}

ECode EmptyLoader::Callback::OnLoadFinished(
    /* [in] */ ILoader* loader,
    /* [in] */ IInterface* data)
{
    return NOERROR;
}

ECode EmptyLoader::Callback::OnLoaderReset(
    /* [in] */ ILoader* loader)
{
    return NOERROR;
}

//=================================================================
// EmptyLoader
//=================================================================
CAR_INTERFACE_IMPL(EmptyLoader, Loader, IEmptyLoader);

ECode EmptyLoader::constructor(
    /* [in] */ IContext* context)
{
    return Loader::constructor(context);
}

} // Util
} // Dialer
} // Apps
} // Elastos
