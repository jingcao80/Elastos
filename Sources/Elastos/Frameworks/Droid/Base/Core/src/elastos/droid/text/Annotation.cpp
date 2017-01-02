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

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/text/Annotation.h"

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL_3(Annotation, Object, IAnnotation, IParcelableSpan, IParcelable)

Annotation::Annotation()
{}

Annotation::~Annotation()
{}

ECode Annotation::constructor()
{
    return NOERROR;
}

ECode Annotation::constructor(
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    mKey = key;
    mValue = value;
    return NOERROR;
}

Int32 Annotation::GetSpanTypeId()
{
    return ITextUtils::ANNOTATION;
}

ECode Annotation::GetSpanTypeId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ITextUtils::ANNOTATION;
    return NOERROR;
}

String Annotation::GetKey()
{
    return mKey;
}

String Annotation::GetValue()
{
    return mValue;
}

ECode Annotation::GetKey(
    /* [out] */ String* key)
{
    VALIDATE_NOT_NULL(key)
    *key = mKey;
    return NOERROR;
}

ECode Annotation::GetValue(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mValue;
    return NOERROR;
}

ECode Annotation::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    FAIL_RETURN(source->ReadString(&mKey));
    FAIL_RETURN(source->ReadString(&mValue));
    return NOERROR;
}

ECode Annotation::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    FAIL_RETURN(dest->WriteString(mKey));
    FAIL_RETURN(dest->WriteString(mValue));
    return NOERROR;
}


} // namespace Text
} // namepsace Droid
} // namespace Elastos