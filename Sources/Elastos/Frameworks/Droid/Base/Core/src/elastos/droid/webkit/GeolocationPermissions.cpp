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

#include "elastos/droid/webkit/GeolocationPermissions.h"
#include "elastos/droid/webkit/WebViewFactory.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(GeolocationPermissions, Object, IGeolocationPermissions);

ECode GeolocationPermissions::constructor()
{
    return NOERROR;
}

AutoPtr<IGeolocationPermissions> GeolocationPermissions::GetInstance()
{
    AutoPtr<IGeolocationPermissions> gp;
    WebViewFactory::GetProvider()->GetGeolocationPermissions((IGeolocationPermissions**)&gp);
    return gp;
}

ECode GeolocationPermissions::GetOrigins(
    /* [in] */ IValueCallback* callback)
{
    return NOERROR;
}

ECode GeolocationPermissions::GetAllowed(
    /* [in] */ const String& origin,
    /* [in] */ IValueCallback* callback)
{
    return NOERROR;
}

ECode GeolocationPermissions::Clear(
    /* [in] */ const String& origin)
{
    return NOERROR;
}

ECode GeolocationPermissions::Allow(
    /* [in] */ const String& origin)
{
    return NOERROR;
}

ECode GeolocationPermissions::ClearAll()
{
    return NOERROR;
}

ECode GeolocationPermissions::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = "GeolocationPermissions";
    return NOERROR;
}

} // namespace Webkit
} // namepsace Droid
} // namespace Elastos