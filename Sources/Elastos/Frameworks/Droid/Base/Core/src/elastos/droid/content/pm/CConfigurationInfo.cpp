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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/pm/CConfigurationInfo.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL_2(CConfigurationInfo, Object, IConfigurationInfo, IParcelable)

CAR_OBJECT_IMPL(CConfigurationInfo)

CConfigurationInfo::CConfigurationInfo()
    : mReqTouchScreen(0)
    , mReqKeyboardType(0)
    , mReqNavigation(0)
    , mReqInputFeatures(0)
    , mReqGlEsVersion(0)
{}

CConfigurationInfo::~CConfigurationInfo()
{}

ECode CConfigurationInfo::constructor()
{
    return NOERROR;
}

ECode CConfigurationInfo::constructor(
    /* [in] */ IConfigurationInfo* orig)
{
    VALIDATE_NOT_NULL(orig)
    orig->GetReqTouchScreen(&mReqTouchScreen);
    orig->GetReqKeyboardType(&mReqKeyboardType);
    orig->GetReqNavigation(&mReqNavigation);
    orig->GetReqInputFeatures(&mReqInputFeatures);
    orig->GetReqGlEsVersion(&mReqGlEsVersion);
    return NOERROR;
}

ECode CConfigurationInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    // return "ConfigurationInfo{"
    //         + Integer.toHexString(System.identityHashCode(this))
    //         + " touchscreen = " + reqTouchScreen
    //         + " inputMethod = " + reqKeyboardType
    //         + " navigation = " + reqNavigation
    //         + " reqInputFeatures = " + reqInputFeatures
    //         + " reqGlEsVersion = " + reqGlEsVersion + "}";
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CConfigurationInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(source != NULL);
    source->ReadInt32(&mReqTouchScreen);
    source->ReadInt32(&mReqKeyboardType);
    source->ReadInt32(&mReqNavigation);
    source->ReadInt32(&mReqInputFeatures);
    source->ReadInt32(&mReqGlEsVersion);
    return NOERROR;
}

ECode CConfigurationInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(dest != NULL);
    dest->WriteInt32(mReqTouchScreen);
    dest->WriteInt32(mReqKeyboardType);
    dest->WriteInt32(mReqNavigation);
    dest->WriteInt32(mReqInputFeatures);
    dest->WriteInt32(mReqGlEsVersion);
    return NOERROR;
}

ECode CConfigurationInfo::GetGlEsVersion(
    /* [out] */ String* ver)
{
    VALIDATE_NOT_NULL(ver);

    Int32 major = ((mReqGlEsVersion & 0xffff0000) >> 16);
    Int32 minor = mReqGlEsVersion & 0x0000ffff;
    *ver = StringUtils::ToString(major) + String(".") + StringUtils::ToString(minor);
    return NOERROR;
}

ECode CConfigurationInfo::GetReqTouchScreen(
    /* [out] */ Int32* reqTouchScreen)
{
    VALIDATE_NOT_NULL(reqTouchScreen);
    *reqTouchScreen = mReqTouchScreen;
    return NOERROR;
}

ECode CConfigurationInfo::SetReqTouchScreen(
    /* [in] */ Int32 reqTouchScreen)
{
    mReqTouchScreen = reqTouchScreen;
    return NOERROR;
}

ECode CConfigurationInfo::GetReqKeyboardType(
    /* [out] */ Int32* reqKeyboardType)
{
    VALIDATE_NOT_NULL(reqKeyboardType);
    *reqKeyboardType = mReqKeyboardType;
    return NOERROR;
}

ECode CConfigurationInfo::SetReqKeyboardType(
    /* [in] */ Int32 reqKeyboardType)
{
    mReqKeyboardType = reqKeyboardType;
    return NOERROR;
}

ECode CConfigurationInfo::GetReqNavigation(
    /* [out] */ Int32* reqNavigation)
{
    VALIDATE_NOT_NULL(reqNavigation);
    *reqNavigation = mReqNavigation;
    return NOERROR;
}

ECode CConfigurationInfo::SetReqNavigation(
    /* [in] */ Int32 reqNavigation)
{
    mReqNavigation = reqNavigation;
    return NOERROR;
}

ECode CConfigurationInfo::GetReqInputFeatures(
    /* [out] */ Int32* reqInputFeatures)
{
    VALIDATE_NOT_NULL(reqInputFeatures);
    *reqInputFeatures = mReqInputFeatures;
    return NOERROR;
}

ECode CConfigurationInfo::SetReqInputFeatures(
    /* [in] */ Int32 reqInputFeatures)
{
    mReqInputFeatures = reqInputFeatures;
    return NOERROR;
}

ECode CConfigurationInfo::GetReqGlEsVersion(
    /* [out] */ Int32* reqGlEsVersion)
{
    VALIDATE_NOT_NULL(reqGlEsVersion);
    *reqGlEsVersion = mReqGlEsVersion;
    return NOERROR;
}

ECode CConfigurationInfo::SetReqGlEsVersion(
    /* [in] */ Int32 reqGlEsVersion)
{
    mReqGlEsVersion = reqGlEsVersion;
    return NOERROR;
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
