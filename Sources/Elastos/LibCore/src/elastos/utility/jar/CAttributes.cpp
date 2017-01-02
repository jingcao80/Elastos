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

#include "CAttributes.h"
#include "CName.h"
#include "CHashMap.h"
#include "CString.h"

using Elastos::Utility::CHashMap;
using Elastos::Core::EIID_ICloneable;
using Elastos::Core::CString;

namespace Elastos {
namespace Utility {
namespace Jar {

CAR_INTERFACE_IMPL_3(CAttributes, Object, IAttributes, IMap, ICloneable)

CAR_OBJECT_IMPL(CAttributes)

ECode CAttributes::constructor()
{
    return CHashMap::New((IMap**)&mMap);
}

ECode CAttributes::constructor(
    /* [in] */ IAttributes* attributes)
{
    AutoPtr<ICloneable> res = ICloneable::Probe(((CAttributes*)attributes)->mMap);
    AutoPtr<IInterface> outface;
    ECode ec = res->Clone((IInterface**)&outface);
    mMap = IMap::Probe(outface);
    return ec;
}

ECode CAttributes::constructor(
    /* [in] */ Int32 size)
{
    return CHashMap::New(size, (IMap**)&mMap);
}

ECode CAttributes::Clear()
{
    return mMap->Clear();
}

ECode CAttributes::ContainsKey(
    /* [in] */ IInterface* key,
    /* [out] */ Boolean* result)
{
    return mMap->ContainsKey(key, result);
}

ECode CAttributes::ContainsValue(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    return mMap->ContainsValue(value, result);
}

ECode CAttributes::GetEntrySet(
    /* [out] */ ISet** entries)
{
    return mMap->GetEntrySet(entries);
}

ECode CAttributes::Get(
    /* [in] */ PInterface key,
    /* [out] */ PInterface* value)
{
    return mMap->Get(key, value);
}

ECode CAttributes::IsEmpty(
    /* [out] */ Boolean* result)
{
    return mMap->IsEmpty(result);
}

ECode CAttributes::GetKeySet(
    /* [out] */ ISet** keySet)
{
    return mMap->GetKeySet(keySet);
}

ECode CAttributes::Put(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value,
    /* [out] */ PInterface* oldValue)
{
    return mMap->Put(key, value, oldValue);
}

ECode CAttributes::Put(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value)
{
    if (IName::Probe(key) == NULL || ICharSequence::Probe(value) == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return mMap->Put(key, value);
}

ECode CAttributes::PutAll(
    /* [in] */ IMap* attrib)
{
    VALIDATE_NOT_NULL(attrib)

    if (!IAttributes::Probe(attrib)) {
        return E_CLASS_CAST_EXCEPTION;
    }

    return mMap->PutAll(attrib);
}

ECode CAttributes::Remove(
    /* [in] */ PInterface key,
    /* [out] */ PInterface* value)
{
    return mMap->Remove(key, value);
}

ECode CAttributes::Remove(
    /* [in] */ PInterface key)
{
    AutoPtr<IInterface> obj;
    return Remove(key, (PInterface*)&obj);
}

ECode CAttributes::GetSize(
    /* [out] */ Int32* size)
{
    return mMap->GetSize(size);
}

ECode CAttributes::GetValues(
    /* [out] */ ICollection** value)
{
    return mMap->GetValues(value);
}

ECode CAttributes::Clone(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<IAttributes> clone;
    FAIL_RETURN(CAttributes::New(this, (IAttributes**)&clone))
    *object = clone;
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode CAttributes::GetHashCode(
    /* [out] */ Int32* hash)
{
    return mMap->GetHashCode(hash);
}

ECode CAttributes::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (object == TO_IINTERFACE(this)) {
        *result = TRUE;
        return NOERROR;
    }

    IAttributes* othterObj = IAttributes::Probe(object);
    if (othterObj) {
        return mMap->Equals(((CAttributes*)othterObj)->mMap, result);
    }
    return NOERROR;
}

ECode CAttributes::GetValue(
    /* [in] */ IName* name,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    *value = String(NULL);

    AutoPtr<IInterface> obj;
    mMap->Get(name, (IInterface**)&obj);
    AutoPtr<ICharSequence> cs = ICharSequence::Probe(obj);
    if (cs) {
        return cs->ToString(value);
    }
    return NOERROR;
}

ECode CAttributes::GetValue(
    /* [in] */ const String& name,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    AutoPtr<IName> iname;
    FAIL_RETURN(CName::New(name, (IName**)&iname))
    return GetValue(iname, value);
}

ECode CAttributes::PutValue(
    /* [in] */ const String& name,
    /* [in] */ const String& val)
{
    String oldVal;
    return PutValue(name, val, &oldVal);
}

ECode CAttributes::PutValue(
    /* [in] */ const String& name,
    /* [in] */ const String& val,
    /* [out] */ String* oldVal)
{
    AutoPtr<IName> iname;
    FAIL_RETURN(CName::New(name, (IName**)&iname))
    AutoPtr<ICharSequence> cs;
    CString::New(val, (ICharSequence**)&cs);

    if (oldVal) {
        AutoPtr<IInterface> outface;
        mMap->Put(iname, cs, (IInterface**)&outface);
        ICharSequence* csRet = ICharSequence::Probe(outface);
        if (csRet) {
            csRet->ToString(oldVal);
        }
        else {
            *oldVal = String(NULL);
        }
    }
    else {
        mMap->Put(iname, cs, NULL);
    }
    return NOERROR;
}

} // namespace Jar
} // namespace Utility
} // namespace Elastos
