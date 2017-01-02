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

#ifndef __ELASTOS_DROID_TEXT_CBIDIFORMATTERBUILDER_H__
#define __ELASTOS_DROID_TEXT_CBIDIFORMATTERBUILDER_H__

#include "_Elastos_Droid_Text_CBidiFormatterBuilder.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Droid {
namespace Text {

CarClass(CBidiFormatterBuilder)
    , public Object
    , public IBidiFormatterBuilder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBidiFormatterBuilder();

    ~CBidiFormatterBuilder();

    /**
     * Constructor.
     *
     */
    CARAPI constructor();

    /**
     * Constructor.
     *
     * @param rtlContext Whether the context directionality is RTL.
     */
    CARAPI constructor(
        /* [in] */ Boolean rtlContext);

    /**
     * Constructor.
     *
     * @param locale The context locale.
     */
    CARAPI constructor(
        /* [in] */ ILocale* locale);

    /**
     * Specifies whether the BidiFormatter to be built should also "reset" directionality before
     * a string being bidi-wrapped, not just after it. The default is false.
     */
    CARAPI StereoReset(
        /* [in] */ Boolean stereoReset);

    /**
     * Specifies the default directionality estimation algorithm to be used by the BidiFormatter.
     * By default, uses the first-strong heuristic.
     *
     * @param heuristic the {@code TextDirectionHeuristic} to use.
     * @return the builder itself.
     */
    CARAPI SetTextDirectionHeuristic(
        /* [in] */ ITextDirectionHeuristic* heuristic);

    /**
     * @return A BidiFormatter with the specified options.
     */
    CARAPI Build(
        /* [out] */ IBidiFormatter** ret);

private:
    /**
     * Initializes the builder with the given context directionality and default options.
     *
     * @param isRtlContext Whether the context is RTL or not.
     */
    CARAPI_(void) Initialize(
        /* [in] */ Boolean isRtlContext);

    static CARAPI_(AutoPtr<IBidiFormatter>) GetDefaultInstanceFromContext(
        /* [in] */ Boolean isRtlContext);

private:
    Boolean mIsRtlContext;
    Int32 mFlags;
    AutoPtr<ITextDirectionHeuristic> mTextDirectionHeuristic;
};

} // namespace Text
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_CBIDIFORMATTERBUILDER_H__
