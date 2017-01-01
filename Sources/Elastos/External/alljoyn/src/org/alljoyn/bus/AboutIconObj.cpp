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

#include "org/alljoyn/bus/AboutIconObj.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;
using Org::Alljoyn::Bus::Ifaces::EIID_IIcon;

namespace Org {
namespace Alljoyn {
namespace Bus {

const Int16 AboutIconObj::VERSION;

CAR_INTERFACE_IMPL_3(AboutIconObj, Object, IAboutIconObj, IIcon, IBusObject)

ECode AboutIconObj::constructor(
    /* [in] */ IBusAttachment* bus,
    /* [in] */ IAboutIcon* icon)
{
    if (bus == NULL) {
        Logger::E("AboutIconObj", "bus must not be null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (icon == NULL) {
        Logger::E("AboutIconObj", "icon must not be null");
        return E_NULL_POINTER_EXCEPTION;
    }
    mIcon = icon;
    return bus->RegisterBusObject(this, IIcon::OBJ_PATH);
}

ECode AboutIconObj::GetVersion(
    /* [out] */ Int16* ver)
{
    VALIDATE_NOT_NULL(ver)
    *ver = VERSION;
    return NOERROR;
}

ECode AboutIconObj::GetMimeType(
    /* [out] */ String* mimeType)
{
    return mIcon->GetMimeType(mimeType);
}

ECode AboutIconObj::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    AutoPtr<ArrayOf<Byte> > content;
    mIcon->GetContent((ArrayOf<Byte>**)&content);
    *size = content ? content->GetLength() : 0;
    return NOERROR;
}

ECode AboutIconObj::GetUrl(
    /* [out] */ String* url)
{
    return mIcon->GetUrl(url);
}

ECode AboutIconObj::GetContent(
    /* [out, callee] */ ArrayOf<Byte>** content)
{
    VALIDATE_NOT_NULL(content)
    mIcon->GetContent(content);
    if (*content == NULL) {
        *content = ArrayOf<Byte>::Alloc(0);
        (*content)->AddRef();
    }
    return NOERROR;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
