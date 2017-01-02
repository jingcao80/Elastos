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

#ifndef __ELASTOS_DROID_TEXT_CTEXTDIRECTIONHEURISTICS_H__
#define __ELASTOS_DROID_TEXT_CTEXTDIRECTIONHEURISTICS_H__

#include "_Elastos_Droid_Text_CTextDirectionHeuristics.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Text {

CarClass(CTextDirectionHeuristics)
    , public Singleton
    , public ITextDirectionHeuristics
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetLTR(
        /* [out] */ ITextDirectionHeuristic** tdh);

    CARAPI GetRTL(
        /* [out] */ ITextDirectionHeuristic** tdh);

    CARAPI GetFIRSTSTRONG_LTR(
        /* [out] */ ITextDirectionHeuristic** tdh);

    CARAPI GetFIRSTSTRONG_RTL(
        /* [out] */ ITextDirectionHeuristic** tdh);

    CARAPI GetANYRTL_LTR(
        /* [out] */ ITextDirectionHeuristic** tdh);

    CARAPI GetLOCALE(
        /* [out] */ ITextDirectionHeuristic** tdh);
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TEXT_CTEXTDIRECTIONHEURISTICS_H__
