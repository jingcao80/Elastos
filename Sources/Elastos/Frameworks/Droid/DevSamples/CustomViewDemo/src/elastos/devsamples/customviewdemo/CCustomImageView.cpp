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

#include "CCustomImageView.h"

namespace Elastos {
namespace DevSamples {
namespace CustomViewDemo {

CAR_INTERFACE_IMPL(CCustomImageView, ImageView, ICustomImageView)

CAR_OBJECT_IMPL(CCustomImageView)

ECode CCustomImageView::SetImageName(
    /* [in] */ const String& name)
{
    mImageName = name;
    return NOERROR;
}

ECode CCustomImageView::GetImageName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mImageName;
    return NOERROR;
}

}// namespace CustomViewDemo
}// namespace DevSamples
}// namespace Elastos