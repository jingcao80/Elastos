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

#include "elastos/droid/text/CTextDirectionHeuristics.h"
#include "elastos/droid/text/TextDirectionHeuristics.h"

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL(CTextDirectionHeuristics, Singleton, ITextDirectionHeuristics)

CAR_SINGLETON_IMPL(CTextDirectionHeuristics)

ECode CTextDirectionHeuristics::GetLTR(
    /* [out] */ ITextDirectionHeuristic** tdh)
{
    VALIDATE_NOT_NULL(tdh)
    AutoPtr<ITextDirectionHeuristic> result = TextDirectionHeuristics::LTR;
    *tdh = result;
    REFCOUNT_ADD(*tdh)
    return NOERROR;
}

ECode CTextDirectionHeuristics::GetRTL(
    /* [out] */ ITextDirectionHeuristic** tdh)
{
    VALIDATE_NOT_NULL(tdh)
    AutoPtr<ITextDirectionHeuristic> result = TextDirectionHeuristics::RTL;
    *tdh = result;
    REFCOUNT_ADD(*tdh)
    return NOERROR;
}

ECode CTextDirectionHeuristics::GetFIRSTSTRONG_LTR(
    /* [out] */ ITextDirectionHeuristic** tdh)
{
    VALIDATE_NOT_NULL(tdh)
    AutoPtr<ITextDirectionHeuristic> result = TextDirectionHeuristics::FIRSTSTRONG_LTR;
    *tdh = result;
    REFCOUNT_ADD(*tdh)
    return NOERROR;
}

ECode CTextDirectionHeuristics::GetFIRSTSTRONG_RTL(
    /* [out] */ ITextDirectionHeuristic** tdh)
{
    VALIDATE_NOT_NULL(tdh)
    AutoPtr<ITextDirectionHeuristic> result = TextDirectionHeuristics::FIRSTSTRONG_RTL;
    *tdh = result;
    REFCOUNT_ADD(*tdh)
    return NOERROR;
}

ECode CTextDirectionHeuristics::GetANYRTL_LTR(
    /* [out] */ ITextDirectionHeuristic** tdh)
{
    VALIDATE_NOT_NULL(tdh)
    AutoPtr<ITextDirectionHeuristic> result = TextDirectionHeuristics::ANYRTL_LTR;
    *tdh = result;
    REFCOUNT_ADD(*tdh)
    return NOERROR;
}

ECode CTextDirectionHeuristics::GetLOCALE(
    /* [out] */ ITextDirectionHeuristic** tdh)
{
    VALIDATE_NOT_NULL(tdh)
    AutoPtr<ITextDirectionHeuristic> result = TextDirectionHeuristics::LOCALE;
    *tdh = result;
    REFCOUNT_ADD(*tdh)
    return NOERROR;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos
