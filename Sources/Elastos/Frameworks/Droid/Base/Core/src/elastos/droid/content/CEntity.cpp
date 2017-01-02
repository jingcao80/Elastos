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

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Net.h"
#include "elastos/droid/content/CEntity.h"
#include "elastos/droid/content/CEntityNamedContentValues.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(CEntity, Object, IEntity)

CAR_OBJECT_IMPL(CEntity)

CEntity::CEntity()
{
}

CEntity::~CEntity()
{
    mSubValues->Clear();
}

ECode CEntity::constructor(
    /* [in] */ IContentValues* values)
{
    mValues = values;
    CArrayList::New((IArrayList**)&mSubValues);
    return NOERROR;
}

ECode CEntity::GetEntityValues(
    /* [out] */ IContentValues** entityValues)
{
    VALIDATE_NOT_NULL(entityValues)
    *entityValues = mValues;
    REFCOUNT_ADD(*entityValues);
    return NOERROR;
}

ECode CEntity::GetSubValues(
    /* [out] */ IArrayList** subValues)
{
    VALIDATE_NOT_NULL(subValues)
    *subValues = mSubValues;
    REFCOUNT_ADD(*subValues)
    return NOERROR;
}

ECode CEntity::AddSubValue(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values)
{
    AutoPtr<IEntityNamedContentValues> entityValues;
    FAIL_RETURN(CEntityNamedContentValues::New(uri, values, (IEntityNamedContentValues**)&entityValues));
    return mSubValues->Add(TO_IINTERFACE(entityValues));
}

ECode CEntity::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("Entity: ");

    AutoPtr<IContentValues> values;
    FAIL_RETURN(GetEntityValues((IContentValues**)&values));
    sb.Append(Object::ToString(values));

    AutoPtr<IIterator> it;
    mSubValues->GetIterator((IIterator**)&it);
    IEntityNamedContentValues* namedValue;
    AutoPtr<IContentValues> contentValues;
    AutoPtr<IUri> uri;
    Boolean next;
    while (it->HasNext(&next), next) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        namedValue = IEntityNamedContentValues::Probe(obj);

        sb.Append("\n  ");
        uri = NULL;
        FAIL_RETURN(namedValue->GetUri((IUri**)&uri));
        FAIL_RETURN(sb.Append(Object::ToString(uri)));
        sb.Append("\n  -> ");
        contentValues = NULL;
        FAIL_RETURN(namedValue->GetValues((IContentValues**)&contentValues));
        FAIL_RETURN(sb.Append(Object::ToString(contentValues)));
    }

    return sb.ToString(str);
}

}
}
}

