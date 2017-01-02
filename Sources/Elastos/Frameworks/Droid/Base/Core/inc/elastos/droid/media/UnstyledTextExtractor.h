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

#ifndef __ELASTOS_DROID_MEDIA_UNSTYLEDTEXTEXTRACTOR_H__
#define __ELASTOS_DROID_MEDIA_UNSTYLEDTEXTEXTRACTOR_H__

#include "Elastos.Droid.Media.h"
#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>

using Elastos::Core::IArrayOf;
using Elastos::Core::IStringBuilder;
using Elastos::Utility::IVector;

namespace Elastos {
namespace Droid {
namespace Media {

class UnstyledTextExtractor
    : public Object
    , public IUnstyledTextExtractor
    , public ITokenizerOnTokenListener
{
public:
    UnstyledTextExtractor();

    virtual ~UnstyledTextExtractor();

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI OnData(
        /* [in] */ const String& s);

    CARAPI OnStart(
        /* [in] */ const String& tag,
        /* [in] */ ArrayOf<String>* classes,
        /* [in] */ const String& annotation);

    CARAPI OnEnd(
        /* [in] */ const String& tag);

    CARAPI OnTimeStamp(
        /* [in] */ Int64 timestampMs);

    CARAPI OnLineEnd();

    CARAPI GetText(
        /* [out, callee] */ ArrayOf<IArrayOf*>** result);

private:
    CARAPI_(void) Init();

public:
    AutoPtr<IStringBuilder> mLine;
    AutoPtr<IVector> mLines;
    AutoPtr<IVector> mCurrentLine;
    Int64 mLastTimestamp;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_UNSTYLEDTEXTEXTRACTOR_H__
