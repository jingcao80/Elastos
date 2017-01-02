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

#ifndef __ELASTOS_DROID_CONTENT_CENTITYNAMEDCONTENTVALUES_H__
#define __ELASTOS_DROID_CONTENT_CENTITYNAMEDCONTENTVALUES_H__

#include "_Elastos_Droid_Content_CEntityNamedContentValues.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Content::IContentValues;

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CEntityNamedContentValues)
    , public Object
    , public IEntityNamedContentValues
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CEntityNamedContentValues();

    virtual ~CEntityNamedContentValues();

    CARAPI constructor(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values);

    CARAPI GetUri(
        /* [out] */ IUri** uri);

    CARAPI GetValues(
        /* [out] */ IContentValues** values);

private:
    AutoPtr<IUri> mUri;
    AutoPtr<IContentValues> mContentValues;

};

} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_CENTITYNAMEDCONTENTVALUES_H__
