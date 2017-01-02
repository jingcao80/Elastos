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

#include "CAttributeListImpl.h"

namespace Org {
namespace Xml {
namespace Sax {
namespace Helpers {

CAR_INTERFACE_IMPL_2(CAttributeListImpl, Object, IAttributeList, IAttributeListImpl)

CAR_OBJECT_IMPL(CAttributeListImpl)

CAttributeListImpl::CAttributeListImpl()
{
}

ECode CAttributeListImpl::GetLength(
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length);

    *length = mNames.GetSize();

    return NOERROR;
}

ECode CAttributeListImpl::GetName(
    /* [in] */ Int32 i,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    if (i < 0 || i >= mNames.GetSize()) {
        *name = NULL;
    }

    *name = mNames[i];

    return NOERROR;
}

ECode CAttributeListImpl::GetType(
    /* [in] */ Int32 i,
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type);

    if (i < 0 || i >= mTypes.GetSize()) {
        *type = NULL;
    }

    *type = mTypes[i];

    return NOERROR;
}

ECode CAttributeListImpl::GetValue(
    /* [in] */ Int32 i,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    if (i < 0 || i >= mValues.GetSize()) {
        *value = NULL;
    }

    *value = mValues[i];

    return NOERROR;
}

ECode CAttributeListImpl::GetType(
    /* [in] */ const String& name,
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type);

    Int32 index = GetListIndex(&mNames, name);

    return GetType(index, type);
}

ECode CAttributeListImpl::GetValue(
    /* [in] */ const String& name,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    Int32 index = GetListIndex(&mNames, name);

    return GetValue(index, value);
}

ECode CAttributeListImpl::SetAttributeList(
    /* [in] */ IAttributeList* atts)
{
    Int32 count = 0;

    atts->GetLength(&count);

    Clear();

    if(count > 0){
        String name("");
        String type("");
        String value("");

        for (Int32 i = 0; i < count; i++) {
            atts->GetName(i, &name);
            atts->GetType(i, &type);
            atts->GetValue(i, &value);

            AddAttribute(name, type, value);
        }
    }

    return NOERROR;
}

ECode CAttributeListImpl::AddAttribute(
    /* [in] */ const String& name,
    /* [in] */ const String& type,
    /* [in] */ const String& value)
{
    mNames.PushBack(name);
    mTypes.PushBack(type);
    mValues.PushBack(value);

    return NOERROR;
}

ECode CAttributeListImpl::RemoveAttribute(
    /* [in] */ const String& name)
{
    int index = GetListIndex(&mNames, name);

    if (index != -1) {
        mNames.Remove(index);
        mTypes.Remove(index);
        mValues.Remove(index);
    }

    return NOERROR;
}

ECode CAttributeListImpl::Clear()
{
    mNames.Clear();
    mTypes.Clear();
    mValues.Clear();

    return NOERROR;
}

ECode CAttributeListImpl::constructor()
{
    return NOERROR;
}

ECode CAttributeListImpl::constructor(
    /* [in] */ IAttributeList* atts)
{
    SetAttributeList(atts);
    return NOERROR;
}

Int32 CAttributeListImpl::GetListIndex(
    /* [in] */ List<String>* list,
    /* [in] */ const String& value)
{
    VALIDATE_NOT_NULL(list);

    Int32 size = list->GetSize();

    for(Int32 i = 0; i < size; i++){
        if(value.Equals((*list)[i])) {
            return i;
        }
    }

    return -1;
}

} // namespace Helpers
} // namespace Sax
} // namespace Xml
} // namespace Org
