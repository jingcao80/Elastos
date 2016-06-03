#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SIGNALTONEUTIL_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SIGNALTONEUTIL_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

//import android.media.ToneGenerator;

using Elastos::Core::IInteger32;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

class SignalToneUtil
    : public Object
    , public ISignalToneUtil
{
public:
    CAR_INTERFACE_DECL();

    static CARAPI_(Int32) GetAudioToneFromSignalInfo(
        /* [in] */ Int32 signalType,
        /* [in] */ Int32 alertPitch,
        /* [in] */ Int32 signal);

    // Integer>, Integer
    static CARAPI_(AutoPtr<IHashMap>) InitHashMap();

private:
    // suppress default constructor for noninstantiability
    SignalToneUtil();

    // = new HashMap<Integer, Integer>();
    static CARAPI_(AutoPtr<IInteger32>) SignalParamHash(
        /* [in] */ Int32 signalType,
        /* [in] */ Int32 alertPitch,
        /* [in] */ Int32 signal);

private:
    // Hashmap to map signalInfo To AudioTone
    static AutoPtr<IHashMap> mHm; // Integer, Integer
};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SIGNALTONEUTIL_H__

