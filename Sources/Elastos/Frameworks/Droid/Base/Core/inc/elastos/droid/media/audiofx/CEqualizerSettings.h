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

#ifndef __ELASTOS_DROID_MEDIA_AUDIOFX_CEQUALIZERSETTINGS_H__
#define __ELASTOS_DROID_MEDIA_AUDIOFX_CEQUALIZERSETTINGS_H__

#include "_Elastos_Droid_Media_Audiofx_CEqualizerSettings.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

/**
 * The Settings class regroups all bass boost parameters. It is used in
 * conjuntion with getProperties() and setProperties() methods to backup and restore
 * all parameters in a single call.
 */
CarClass(CEqualizerSettings)
    , public Object
    , public IEqualizerSettings
{
public:
    CEqualizerSettings();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /**
     * Settings class constructor from a key=value; pairs formatted string. The string is
     * typically returned by Settings.toString() method.
     * @throws IllegalArgumentException if the string is not correctly formatted.
     */
    CARAPI constructor(
        /* [in] */ const String& settings);

    CARAPI GetCurPreset(
        /* [out] */ Int16* curPreset);

    CARAPI SetCurPreset(
        /* [in] */ Int16 curPreset);

    CARAPI GetNumBands(
        /* [out] */ Int16* numBands);

    CARAPI SetNumBands(
        /* [in] */ Int16 numBands);

    CARAPI GetBandLevels(
        /* [out, callee] */ ArrayOf<Int16>** levels);

    CARAPI SetBandLevels(
        /* [in] */ ArrayOf<Int16>* levels);

    CARAPI ToString(
        /* [out] */ String* result);

public:
    Int16 mCurPreset;
    Int16 mNumBands;
    AutoPtr<ArrayOf<Int16> > mBandLevels;
};

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_AUDIOFX_CEQUALIZERSETTINGS_H__
