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

#ifndef __ELASTOS_DROID_NET_CURIBUILDER_H__
#define __ELASTOS_DROID_NET_CURIBUILDER_H__

#include "_Elastos_Droid_Net_CUriBuilder.h"
#include "elastos/droid/net/Uri.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Helper class for building or manipulating URI references. Not safe for
 * concurrent use.
 *
 * <p>An absolute hierarchical URI reference follows the pattern:
 * {@code <scheme>://<authority><absolute path>?<query>#<fragment>}
 *
 * <p>Relative URI references (which are always hierarchical) follow one
 * of two patterns: {@code <relative or absolute path>?<query>#<fragment>}
 * or {@code //<authority><absolute path>?<query>#<fragment>}
 *
 * <p>An opaque URI follows this pattern:
 * {@code <scheme>:<opaque part>#<fragment>}
 *
 * <p>Use {@link Uri#buildUpon()} to obtain a builder representing an existing URI.
 */
CarClass(CUriBuilder)
    , public UriBuilder
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_CURIBUILDER_H__
