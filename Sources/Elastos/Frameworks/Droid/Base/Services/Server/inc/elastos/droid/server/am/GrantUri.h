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

#ifndef __ELASTOS_DROID_SERVER_AM_GRANTURI_H__
#define __ELASTOS_DROID_SERVER_AM_GRANTURI_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Net.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

// inner class of CActivityManagerService
class GrantUri : public Object
{
public:
    GrantUri(
        /* [in] */ Int32 sourceUserId,
        /* [in] */ IUri* uri,
        /* [in] */ Boolean prefix);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI_(Boolean) Equals(
        /* [in] */ GrantUri* other);

    // @Override
    CARAPI ToString(
        /* [out] */ String* info);

    CARAPI_(String) ToString();

    CARAPI_(String) ToSafeString();

    static CARAPI_(AutoPtr<GrantUri>) Resolve(
        /* [in] */ Int32 defaultSourceUserHandle,
        /* [in] */ IUri* uri);

public:
    const Int32 mSourceUserId;
    const AutoPtr<IUri> mUri;
    Boolean mPrefix;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Server::Am::GrantUri)

#endif //__ELASTOS_DROID_SERVER_AM_GRANTURI_H__
