
#ifndef __ELASTOS_DROID_MEDIA_AUDIOFX_CVISUALIZERMEASUREMENTPEAKRMS_H__
#define __ELASTOS_DROID_MEDIA_AUDIOFX_CVISUALIZERMEASUREMENTPEAKRMS_H__

#include "_Elastos_Droid_Media_Audiofx_CVisualizerMeasurementPeakRms.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

CarClass(CVisualizerMeasurementPeakRms)
    , public Object
    , public IVisualizerMeasurementPeakRms
{
public:
    CVisualizerMeasurementPeakRms();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

public:
    /**
     * The peak value in mB.
     */
    Int32 mPeak;
    /**
     * The RMS value in mB.
     */
    Int32 mRms;

};

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_MEDIA_AUDIOFX_CVISUALIZERMEASUREMENTPEAKRMS_H__
