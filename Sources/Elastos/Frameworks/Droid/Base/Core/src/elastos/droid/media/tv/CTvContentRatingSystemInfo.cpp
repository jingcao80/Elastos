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

#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/media/tv/CTvContentRatingSystemInfo.h"
#include "elastos/droid/net/CUriBuilder.h"
#include <elastos/utility/Objects.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Net::CUriBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

CAR_INTERFACE_IMPL_2(CTvContentRatingSystemInfo, Object, ITvContentRatingSystemInfo, IParcelable)

CAR_OBJECT_IMPL(CTvContentRatingSystemInfo)

CTvContentRatingSystemInfo::CTvContentRatingSystemInfo()
{
}

CTvContentRatingSystemInfo::~CTvContentRatingSystemInfo()
{
}

ECode CTvContentRatingSystemInfo::constructor()
{
    return NOERROR;
}

ECode CTvContentRatingSystemInfo::constructor(
    /* [in] */ IUri * xmlUri,
    /* [in] */ IApplicationInfo * applicationInfo)
{
    mXmlUri = xmlUri;
    mApplicationInfo = applicationInfo;
    return NOERROR;
}

ECode CTvContentRatingSystemInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((HANDLE*)(IInterface**)&obj);
    mXmlUri = IUri::Probe(obj);
    obj = NULL;
    source->ReadInterfacePtr((HANDLE*)(IInterface**)&obj);
    mApplicationInfo = IApplicationInfo::Probe(obj);
    return NOERROR;
}

ECode CTvContentRatingSystemInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInterfacePtr(mXmlUri);
    dest->WriteInterfacePtr(mApplicationInfo);
    return NOERROR;
}

ECode CTvContentRatingSystemInfo::IsSystemDefined(
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result)
    Int32 flags;
    mApplicationInfo->GetFlags(&flags);
    *result = (flags & IApplicationInfo::FLAG_SYSTEM) != 0;
    return NOERROR;
}

ECode CTvContentRatingSystemInfo::GetXmlUri(
    /* [out] */ IUri ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mXmlUri;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CTvContentRatingSystemInfo::CreateTvContentRatingSystemInfo(
    /* [in] */ Int32 xmlResourceId,
    /* [in] */ IApplicationInfo * applicationInfo,
    /* [out] */ ITvContentRatingSystemInfo ** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IUriBuilder> builder;
    CUriBuilder::New((IUriBuilder**)&builder);
    builder->Scheme(IContentResolver::SCHEME_ANDROID_RESOURCE);
    String pkgName;
    IPackageItemInfo::Probe(applicationInfo)->GetPackageName(&pkgName);
    builder->Authority(pkgName);
    builder->AppendPath(StringUtils::ToString(xmlResourceId));
    AutoPtr<IUri> uri;
    builder->Build((IUri**)&uri);
    return CTvContentRatingSystemInfo::New(uri, applicationInfo, result);
}


} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
