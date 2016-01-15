#ifndef __ELASTOS_DROID_MEDIA_TV_CTVSTREAMCONFIG_H__
#define __ELASTOS_DROID_MEDIA_TV_CTVSTREAMCONFIG_H__

#include "_Elastos_Droid_Media_Tv_CTvStreamConfig.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

/**
 * @hide
 */
// @SystemApi
CarClass(CTvStreamConfig)
    , public Object
    , public ITvStreamConfig
    , public IParcelable
{
public:
    friend class CTvStreamConfigBuilder;

    CTvStreamConfig();

    virtual ~CTvStreamConfig();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetStreamId(
        /* [out] */ Int32* result);

    CARAPI GetType(
        /* [out] */ Int32* result);

    CARAPI GetMaxWidth(
        /* [out] */ Int32* result);

    CARAPI GetMaxHeight(
        /* [out] */ Int32* result);

    CARAPI GetGeneration(
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    static String TAG;

    Int32 mStreamId;
    Int32 mType;
    // TODO: Revisit if max widht/height really make sense.
    Int32 mMaxWidth;
    Int32 mMaxHeight;
    /**
     * Generations are incremented once framework receives STREAM_CONFIGURATION_CHANGED event from
     * HAL module. Framework should throw away outdated configurations and get new configurations
     * via tv_input_device::get_stream_configurations().
     */
    Int32 mGeneration;
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_CTVSTREAMCONFIG_H__
