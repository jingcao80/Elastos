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

#ifndef __ELASTOS_DROID_CONTENT_CENTITY_H__
#define __ELASTOS_DROID_CONTENT_CENTITY_H__

#include "_Elastos_Droid_Content_CEntity.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IArrayList;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Content {

/**
 * A representation of a item using ContentValues. It contains one top level ContentValue
 * plus a collection of Uri, ContentValues tuples as subvalues. One example of its use
 * is in Contacts, where the top level ContentValue contains the columns from the RawContacts
 * table and the subvalues contain a ContentValues object for each row from the Data table that
 * corresponds to that RawContact. The uri refers to the Data table uri for each row.
 */
CarClass(CEntity)
    , public Object
    , public IEntity
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CEntity();

    virtual ~CEntity();

    CARAPI constructor(
        /* [in] */ IContentValues* values);

    CARAPI GetEntityValues(
        /* [out] */ IContentValues** entityValues);

    CARAPI GetSubValues(
        /* [out] */ IArrayList** subValues);

    CARAPI AddSubValue(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    AutoPtr<IContentValues> mValues;
    AutoPtr<IArrayList> mSubValues;
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CENTITY_H__
