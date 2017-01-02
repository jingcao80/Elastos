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

#ifndef __ELASTOS_DROID_OS_PATTERNMATCHER_H__
#define __ELASTOS_DROID_OS_PATTERNMATCHER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Os {

class PatternMatcher
    : public Object
    , public IPatternMatcher
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    PatternMatcher();

    virtual ~PatternMatcher();

    CARAPI constructor();

    CARAPI constructor(
    /* [in] */ const String& pattern,
    /* [in] */ Int32 type);

    CARAPI GetPath(
        /* [out] */ String* path);

    CARAPI GetType(
        /* [out] */ Int32* type);

    CARAPI Match(
        /* [in] */ const String& str,
        /* [out] */ Boolean* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ToString(
        /* [out] */ String* str);

protected:
    static CARAPI_(Boolean) MatchPattern(
        /* [in] */ const String& pattern,
        /* [in] */ const String& match,
        /* [in] */ Int32 type);

private:
    String mPattern;
    Int32 mType;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_PATTERNMATCHER_H__
