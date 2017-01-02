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

#ifndef __ELASTOS_DROID_PROVIDER_CMEDIASTOREAUDIO_H__
#define __ELASTOS_DROID_PROVIDER_CMEDIASTOREAUDIO_H__

#include "_Elastos_Droid_Provider_CMediaStoreAudio.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CMediaStoreAudio)
    , public Singleton
    , public IMediaStoreAudio
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Converts a name to a "key" that can be used for grouping, sorting
     * and searching.
     * The rules that govern this conversion are:
     * - remove 'special' characters like ()[]'!?.,
     * - remove leading/trailing spaces
     * - convert everything to lowercase
     * - remove leading "the ", "an " and "a "
     * - remove trailing ", the|an|a"
     * - remove accents. This step leaves us with CollationKey data,
     *   which is not human readable
     *
     * @param name The artist or album name to convert
     * @return The "key" for the given name.
     */
    CARAPI KeyFor(
        /* [in] */ const String& name,
        /* [out] */ String* key);
};

} //namespace Provider
} //namespace Droid
} //namespace Elastos

#endif //
