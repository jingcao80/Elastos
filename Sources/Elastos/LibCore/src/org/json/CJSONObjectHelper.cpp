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

#include "CJSONObjectHelper.h"
#include "JSONObject.h"

namespace Org {
namespace Json {

CAR_SINGLETON_IMPL(CJSONObjectHelper);

CAR_INTERFACE_IMPL(CJSONObjectHelper, Singleton, IJSONObjectHelper)

ECode CJSONObjectHelper::NumberToString(
    /* [in] */ INumber* number,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    return JSONObject::NumberToString(number, str);
}

ECode CJSONObjectHelper::Quote(
    /* [in] */ const String& data,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    return JSONObject::Quote(data, str);
}

ECode CJSONObjectHelper::Wrap(
    /* [in] */ IInterface* o,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);
    return JSONObject::Wrap(o, obj);
}

ECode CJSONObjectHelper::GetNULL(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);
    *obj = JSONObject::Object_NULL;
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

} //namespace Json
} //namespace Org
