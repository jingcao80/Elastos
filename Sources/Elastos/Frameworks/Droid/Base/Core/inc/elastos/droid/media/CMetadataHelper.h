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

#ifndef __ELASTOS_DROID_MEDIA_CMETADATAHELPER_H__
#define __ELASTOS_DROID_MEDIA_CMETADATAHELPER_H__

#include "_Elastos_Droid_Media_CMetadataHelper.h"
#include "Elastos.Droid.Media.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMetadataHelper)
    , public Singleton
    , public IMetadataHelper
{
public:

    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * @return the last available system metadata id. Ids are
     *         1-indexed.
     * {@hide}
     */
    CARAPI LastSytemId(
        /* [out] */ Int32* result);

    /**
     * @return the first available cutom metadata id.
     * {@hide}
     */
    CARAPI FirstCustomId(
        /* [out] */ Int32* result);

    /**
     * @return the last value of known type. Types are 1-indexed.
     * {@hide}
     */
    CARAPI LastType(
        /* [out] */ Int32* result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMETADATAHELPER_H__
