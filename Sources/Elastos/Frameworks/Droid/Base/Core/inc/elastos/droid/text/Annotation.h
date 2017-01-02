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

#ifndef __ELASTOS_DROID_TEXT_Annotation_H__
#define __ELASTOS_DROID_TEXT_Annotation_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Text {

/**
 * Annotations are simple key-value pairs that are preserved across
 * TextView save/restore cycles and can be used to keep application-specific
 * data that needs to be maintained for regions of text.
 */
class Annotation
    : public Object
    , public IAnnotation
    , public IParcelableSpan
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    Annotation();

    virtual ~Annotation();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& key,
        /* [in] */ const String& value);

    CARAPI_(Int32) GetSpanTypeId();

    CARAPI GetSpanTypeId(
        /* [out] */ Int32* id);

    CARAPI_(String) GetKey();

    CARAPI_(String) GetValue();

    CARAPI GetKey(
        /* [out] */ String* key);

    CARAPI GetValue(
        /* [out] */ String* value);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    String mKey;
    String mValue;
};


} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TEXT_Annotation_H__
