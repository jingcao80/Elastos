#ifndef __ELASTOS_DROID_MEDIA_TV_CTVINPUTINFOTVINPUTSETTINGS_H__
#define __ELASTOS_DROID_MEDIA_TV_CTVINPUTINFOTVINPUTSETTINGS_H__

#include "_Elastos_Droid_Media_Tv_CTvInputInfoTvInputSettings.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Singleton;
using Elastos::Utility::IMap;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

/**
 * Utility class for putting and getting settings for TV input.
 *
 * @hide
 */
// @SystemApi
CarClass(CTvInputInfoTvInputSettings)
    , public Singleton
    , public ITvInputInfoTvInputSettings
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns a set of TV input IDs which are marked as hidden by user in the settings.
     *
     * @param context The application context
     * @param userId The user ID for the stored hidden input set
     * @hide
     */
    // @SystemApi
    CARAPI GetHiddenTvInputIds(
        /* [in] */ IContext * context,
        /* [in] */ Int32 userId,
        /* [out] */ ISet ** result);

    /**
     * Returns a map of TV input ID/custom label pairs set by the user in the settings.
     *
     * @param context The application context
     * @param userId The user ID for the stored hidden input map
     * @hide
     */
    // @SystemApi
    CARAPI GetCustomLabels(
        /* [in] */ IContext * context,
        /* [in] */ Int32 userId,
        /* [out] */ IMap ** result);

    /**
     * Stores a set of TV input IDs which are marked as hidden by user. This is expected to
     * be called from the settings app.
     *
     * @param context The application context
     * @param hiddenInputIds A set including all the hidden TV input IDs
     * @param userId The user ID for the stored hidden input set
     * @hide
     */
    // @SystemApi
    CARAPI PutHiddenTvInputs(
        /* [in] */ IContext * context,
        /* [in] */ ISet * hiddenInputIds,
        /* [in] */ Int32 userId);

    /**
     * Stores a map of TV input ID/custom label set by user. This is expected to be
     * called from the settings app.
     *
     * @param context The application context.
     * @param customLabels A map of TV input ID/custom label pairs
     * @param userId The user ID for the stored hidden input map
     * @hide
     */
    // @SystemApi
    CARAPI PutCustomLabels(
        /* [in] */ IContext * context,
        /* [in] */ IMap * customLabels,
        /* [in] */ Int32 userId);
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_CTVINPUTINFOTVINPUTSETTINGS_H__
