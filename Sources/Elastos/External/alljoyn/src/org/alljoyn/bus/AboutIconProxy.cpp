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

#include "org/alljoyn/bus/AboutIconProxy.h"
#include "org/alljoyn/bus/CAboutIcon.h"
#include "org/alljoyn/bus/NativeApi.h"

using Org::Alljoyn::Bus::Ifaces::EIID_IIcon;

namespace Org {
namespace Alljoyn {
namespace Bus {

CAR_INTERFACE_IMPL_2(AboutIconProxy, Object, IAboutIconProxy, IIcon)

ECode AboutIconProxy::constructor(
    /* [in] */ IBusAttachment* bus,
    /* [in] */ const String& busName,
    /* [in] */ Int32 sessionId)
{
    AutoPtr<IClassLoader> loader = GetModuleClassLoader();
    AutoPtr<IInterfaceInfo> itfcInfo;
    loader->LoadInterface(String("Org.Alljoyn.Bus.Ifaces.IIcon"), (IInterfaceInfo**)&itfcInfo);
    AutoPtr< ArrayOf<IInterfaceInfo*> > busInterfaces = ArrayOf<IInterfaceInfo*>::Alloc(1);
    busInterfaces->Set(0, itfcInfo);

    AutoPtr<IProxyBusObject> aboutIconProxy;
    bus->GetProxyBusObject(busName, IIcon::OBJ_PATH, sessionId,
        busInterfaces, (IProxyBusObject**)&aboutIconProxy);
    return aboutIconProxy->GetInterface(EIID_IIcon, (IInterface**)&mProxy);
}

ECode AboutIconProxy::GetAboutIcon(
    /* [out] */ IAboutIcon** aboutIcon)
{
    String mimeType, url;
    AutoPtr<ArrayOf<Byte> > content;
    FAIL_RETURN(GetMimeType(&mimeType))
    FAIL_RETURN(GetUrl(&url))
    FAIL_RETURN(GetContent((ArrayOf<Byte>**)&content))
    return CAboutIcon::New(mimeType, url, content, aboutIcon);
}

ECode AboutIconProxy::GetVersion(
    /* [out] */ Int16* ver)
{
    return mProxy->GetVersion(ver);
}

ECode AboutIconProxy::GetMimeType(
    /* [out] */ String* mimeType)
{
    return mProxy->GetMimeType(mimeType);
}

ECode AboutIconProxy::GetSize(
    /* [out] */ Int32* size)
{
    return mProxy->GetSize(size);
}

ECode AboutIconProxy::GetUrl(
    /* [out] */ String* url)
{
    return mProxy->GetUrl(url);
}

ECode AboutIconProxy::GetContent(
    /* [out, callee] */ ArrayOf<Byte>** content)
{
    return mProxy->GetContent(content);
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
