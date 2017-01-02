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

#include "PreferenceChangeEvent.h"

namespace Elastos {
namespace Utility {
namespace Prefs {

CAR_INTERFACE_IMPL(PreferenceChangeEvent, EventObject, IPreferenceChangeEvent)

PreferenceChangeEvent::PreferenceChangeEvent()
{
}

ECode PreferenceChangeEvent::constructor(
    /* [in] */ Preferences* p,
    /* [in] */ const String& k,
    /* [in] */ const String& v)
{
    FAIL_RETURN(EventObject::constructor(p))
    mNode = p;
    mKey = k;
    mValue = v;
    return NOERROR;
}

ECode PreferenceChangeEvent::GetKey(
    /* [out] */ String* key)
{
    VALIDATE_NOT_NULL(key);
    *key = mKey;
    return NOERROR;
}

ECode PreferenceChangeEvent::GetNewValue(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mValue;
    return NOERROR;
}

ECode PreferenceChangeEvent::GetNode(
    /* [out] */ IPreferences** pf)
{
    VALIDATE_NOT_NULL(pf);
    *pf = mNode;
    REFCOUNT_ADD(*pf);
    return NOERROR;
}

ECode PreferenceChangeEvent::WriteObject(
    /* [in] */ IObjectOutputStream* out) /*throws IOException*/
{
    // throw new NotSerializableException();
    return E_NOT_SERIALIZABLE_EXCEPTION;
}

/**
 * This method always throws a <code>NotSerializableException</code>,
 * because this object cannot be serialized,
 */
ECode PreferenceChangeEvent::ReadObject(
    /* [in] */ IObjectInputStream* in) /*throws IOException*/
{
    // throw new NotSerializableException();
    return E_NOT_SERIALIZABLE_EXCEPTION;
}

ECode PreferenceChangeEvent::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = String("Elastos.Utility.Prefs.PreferenceChangeEvent");
    return NOERROR;
}

} // namespace Prefs
} // namespace Utility
} // namespace Elastos
