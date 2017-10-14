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

#include "elastos/droid/graphics/drawable/CVGroup.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CAR_OBJECT_IMPL(CVGroup)

ECode CVGroup::GetRotation(
    /* [out] */ Float* rotation)
{
    return VectorDrawable::VGroup::GetRotation(rotation);
}

//ECode CVGroup::SetRotation(
//    /* [in] */ Float rotation)
//{
//    VectorDrawable::VGroup::SetRotation(rotation);
//    return NOERROR;
//}

ECode CVGroup::GetPivotX(
    /* [out] */ Float* pivotx)
{
    return VectorDrawable::VGroup::GetPivotX(pivotx);
}

//ECode CVGroup::SetPivotX(
//    /* [in] */ Float pivotx)
//{
//    VectorDrawable::VGroup::SetPivotX(pivotx);
//    return NOERROR;
//}

ECode CVGroup::GetPivotY(
    /* [out] */ Float* pivoty)
{
    return VectorDrawable::VGroup::GetPivotY(pivoty);
}

//ECode CVGroup::SetPivotY(
//    /* [in] */ Float pivoty)
//{
//    VectorDrawable::VGroup::SetPivotY(pivoty);
//    return NOERROR;
//}

ECode CVGroup::GetScaleX(
    /* [out] */ Float* scalex)
{
    return VectorDrawable::VGroup::GetScaleX(scalex);
}

//ECode CVGroup::SetScaleX(
//    /* [in] */ Float scalex)
//{
//    VectorDrawable::VGroup::SetScaleX(scalex);;
//    return NOERROR;
//}

ECode CVGroup::GetScaleY(
    /* [out] */ Float* scaley)
{
    return VectorDrawable::VGroup::GetScaleY(scaley);
}

//ECode CVGroup::SetScaleY(
//    /* [in] */ Float scaley)
//{
//    VectorDrawable::VGroup::SetScaleY(scaley);
//    return NOERROR;
//}

ECode CVGroup::GetTranslateX(
    /* [out] */ Float* translatex)
{
    return VectorDrawable::VGroup::GetTranslateX(translatex);
}

//ECode CVGroup::SetTranslateX(
//    /* [in] */ Float translatex)
//{
//    VectorDrawable::VGroup::SetTranslateX(translatex);
//    return NOERROR;
//}

ECode CVGroup::GetTranslateY(
    /* [out] */ Float* translatey)
{
    return VectorDrawable::VGroup::GetTranslateY(translatey);
}

//ECode CVGroup::SetTranslateY(
//    /* [in] */ Float translatey)
//{
//    VectorDrawable::VGroup::SetTranslateY(translatey);;
//    return NOERROR;
//}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
