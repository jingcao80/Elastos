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

#ifndef __ELASTOS_DROID_TEXT_STYLE_RasterizerSpan_H__
#define __ELASTOS_DROID_TEXT_STYLE_RasterizerSpan_H__

#include "elastos/droid/text/style/CharacterStyle.h"

using Elastos::Droid::Graphics::IRasterizer;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

class RasterizerSpan
    : public CharacterStyle
    , public IRasterizerSpan
    , public IUpdateAppearance
{
public:
    CAR_INTERFACE_DECL()

    RasterizerSpan();

    virtual ~RasterizerSpan();

    CARAPI constructor(
        /* [in] */ IRasterizer* r);

    CARAPI GetRasterizer(
        /* [out] */ IRasterizer** rastersizer);

    //@Override
    CARAPI UpdateDrawState(
        /* [in] */ ITextPaint* ds);

private:
    AutoPtr<IRasterizer> mRasterizer;
};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_RasterizerSpan_H__
