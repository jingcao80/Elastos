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

#ifndef __ORG_APACHE_HTTP_IMPL_CLIENT_REDIRECTLOCATIONS_H__
#define __ORG_APACHE_HTTP_IMPL_CLIENT_REDIRECTLOCATIONS_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/core/Object.h"

using Elastos::Net::IURI;
using Elastos::Utility::ISet;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

/**
 * A collection of URIs that were used as redirects.
 */
class RedirectLocations : public Object
{
public:
    RedirectLocations();

    /**
     * Returns true if this collection contains the given URI.
     */
    CARAPI_(Boolean) Contains(
        /* [in] */ IURI* uri);

    /**
     * Adds a new URI to the list of redirects.
     */
    CARAPI_(void) Add(
        /* [in] */ IURI* uri);

    /**
     * Removes a URI from the list of redirects.
     */
    CARAPI_(Boolean) Remove(
        /* [in] */ IURI* uri);

private:
    AutoPtr<ISet> mUris;
};

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CLIENT_REDIRECTLOCATIONS_H__
