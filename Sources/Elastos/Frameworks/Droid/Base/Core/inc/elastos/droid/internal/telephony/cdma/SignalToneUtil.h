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

