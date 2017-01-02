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

#include "elastos/droid/hardware/display/DisplayViewport.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include "elastos/droid/graphics/CRect.h"

using Elastos::Droid::Graphics::CRect;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

CAR_INTERFACE_IMPL(DisplayViewport, Object, IDisplayViewport)

DisplayViewport::DisplayViewport()
{
    CRect::New((IRect**)&mLogicalFrame);
    CRect::New((IRect**)&mPhysicalFrame);
}

DisplayViewport::~DisplayViewport()
{}

ECode DisplayViewport::constructor()
{
    return NOERROR;
}

ECode DisplayViewport::CopyFrom(
    /* [in] */ IDisplayViewport* viewport)
{
    mValid = ((DisplayViewport*)viewport)->mValid;
    mDisplayId = ((DisplayViewport*)viewport)->mDisplayId;
    mOrientation = ((DisplayViewport*)viewport)->mOrientation;
    mLogicalFrame->Set(((DisplayViewport*)viewport)->mLogicalFrame);
    mPhysicalFrame->Set(((DisplayViewport*)viewport)->mPhysicalFrame);
    mDeviceWidth = ((DisplayViewport*)viewport)->mDeviceWidth;
    mDeviceHeight = ((DisplayViewport*)viewport)->mDeviceHeight;
    return NOERROR;
}

ECode DisplayViewport::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    StringBuilder sb;
    String tmp;
    sb += "DisplayViewport{valid=";
    sb += StringUtils::BooleanToString(mValid);;
    sb += ", displayId=";
    sb += StringUtils::ToString(mDisplayId);
    sb += ", orientation=";
    sb += StringUtils::ToString(mOrientation);
    sb += ", logicalFrame=";
    IObject::Probe(mLogicalFrame)->ToString(&tmp);
    sb += tmp;
    sb += ", physicalFrame=";
    IObject::Probe(mPhysicalFrame)->ToString(&tmp);
    sb += tmp;
    sb += ", deviceWidth=";
    sb += StringUtils::ToString(mDeviceWidth);
    sb += ", deviceHeight=";
    sb += StringUtils::ToString(mDeviceHeight);
    sb += "}";

    return sb.ToString(result);
}

ECode DisplayViewport::IsValid(
    /* [out] */ Boolean* valid)
{
    VALIDATE_NOT_NULL(valid)
    *valid = mValid;
    return NOERROR;
}

ECode DisplayViewport::SetValid(
    /* [in] */ Boolean valid)
{
    mValid = valid;
    return NOERROR;
}

ECode DisplayViewport::SetDisplayId(
    /* [in] */ Int32 displayId)
{
    mDisplayId = displayId;
    return NOERROR;
}

ECode DisplayViewport::GetDisplayId(
    /* [out] */ Int32* displayId)
{
    VALIDATE_NOT_NULL(displayId)
    *displayId = mDisplayId;
    return NOERROR;
}

ECode DisplayViewport::SetOrientation(
    /* [in] */ Int32 orientation)
{
    mOrientation = orientation;
    return NOERROR;
}

ECode DisplayViewport::GetOrientation(
    /* [out] */ Int32* orientation)
{
    VALIDATE_NOT_NULL(orientation)
    *orientation = mOrientation;
    return NOERROR;
}

ECode DisplayViewport::SetLogicalFrame(
    /* [in] */ IRect* rect)
{
    mLogicalFrame = rect;
    return NOERROR;
}

ECode DisplayViewport::GetLogicalFrame(
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect)
    *rect = mLogicalFrame;
    REFCOUNT_ADD(*rect)
    return NOERROR;
}

ECode DisplayViewport::SetPhysicalFrame(
    /* [in] */ IRect* rect)
{
    mPhysicalFrame = rect;
    return NOERROR;
}

ECode DisplayViewport::GetPhysicalFrame(
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect)
    *rect = mPhysicalFrame;
    REFCOUNT_ADD(*rect)
    return NOERROR;
}

ECode DisplayViewport::SetDeviceWidth(
    /* [in] */ Int32 width)
{
    mDeviceWidth = width;
    return NOERROR;
}

ECode DisplayViewport::GetDeviceWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width)
    *width = mDeviceWidth;
    return NOERROR;
}

ECode DisplayViewport::SetDeviceHeight(
    /* [in] */ Int32 height)
{
    mDeviceHeight = height;
    return NOERROR;
}

ECode DisplayViewport::GetDeviceHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height)
    *height = mDeviceHeight;
    return NOERROR;
}

} // namespace Display
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos