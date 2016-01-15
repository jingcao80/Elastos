#include "elastos/droid/media/tv/CTvInputInfoTvInputSettings.h"
#include "elastos/droid/media/tv/TvInputInfoTvInputSettings.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

CAR_SINGLETON_IMPL(CTvInputInfoTvInputSettings)
CAR_INTERFACE_IMPL(CTvInputInfoTvInputSettings, Singleton, ITvInputInfoTvInputSettings);

ECode CTvInputInfoTvInputSettings::GetHiddenTvInputIds(
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId,
    /* [out] */ ISet** result)
{
    return TvInputInfoTvInputSettings::GetHiddenTvInputIds(context, userId, result);
}

ECode CTvInputInfoTvInputSettings::GetCustomLabels(
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId,
    /* [out] */ IMap** result)
{
    return TvInputInfoTvInputSettings::GetCustomLabels(context, userId, result);
}

ECode CTvInputInfoTvInputSettings::PutHiddenTvInputs(
    /* [in] */ IContext* context,
    /* [in] */ ISet* hiddenInputIds,
    /* [in] */ Int32 userId)
{
    return TvInputInfoTvInputSettings::PutHiddenTvInputs(context, hiddenInputIds, userId);
}

ECode CTvInputInfoTvInputSettings::PutCustomLabels(
    /* [in] */ IContext* context,
    /* [in] */ IMap* customLabels,
    /* [in] */ Int32 userId)
{
    return TvInputInfoTvInputSettings::PutCustomLabels(context, customLabels, userId);
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
