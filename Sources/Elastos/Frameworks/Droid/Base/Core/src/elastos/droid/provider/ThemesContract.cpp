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

#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/ThemesContract.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Net::Uri;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Provider {

static AutoPtr<IUri> InitAuthrityUri()
{
    AutoPtr<IUri> uri;
    Uri::Parse(String("content://") + IThemesContract::AUTHORITY, (IUri**)&uri);
    return uri;
}

static AutoPtr<IUri> InitURI(
    /* [in] */ const char* str)
{
    AutoPtr<IUri> AUTHORITY_URI = InitAuthrityUri();

    AutoPtr<IUri> uri;
    Uri::WithAppendedPath(AUTHORITY_URI, String(str), (IUri**)&uri);
    return uri;
}

//============================================================================
//              ThemesContract
//============================================================================

const AutoPtr<IUri> ThemesContract::AUTHORITY_URI = InitAuthrityUri();

//============================================================================
//              ThemesContract::ThemesColumns
//============================================================================

const AutoPtr<IUri> ThemesContract::ThemesColumns::CONTENT_URI = InitURI("themes");


//============================================================================
//              ThemesContract::PreviewColumns
//============================================================================

const AutoPtr<IUri> ThemesContract::PreviewColumns::CONTENT_URI = InitURI("previews");
const AutoPtr<IUri> ThemesContract::PreviewColumns::APPLIED_URI = InitURI("applied_previews");

//============================================================================
//              ThemesContract::MixnMatchColumns
//============================================================================

const AutoPtr<IUri> ThemesContract::MixnMatchColumns::CONTENT_URI = InitURI("mixnmatch");

static AutoPtr<ArrayOf<String> > InitROWS()
{
    AutoPtr<ArrayOf<String> > rows = ArrayOf<String>::Alloc(11);
    (*rows)[0] = IThemesContractMixnMatchColumns::KEY_HOMESCREEN;
    (*rows)[1] = IThemesContractMixnMatchColumns::KEY_LOCKSCREEN;
    (*rows)[2] = IThemesContractMixnMatchColumns::KEY_ICONS;
    (*rows)[3] = IThemesContractMixnMatchColumns::KEY_STATUS_BAR;
    (*rows)[4] = IThemesContractMixnMatchColumns::KEY_BOOT_ANIM;
    (*rows)[5] = IThemesContractMixnMatchColumns::KEY_FONT;
    (*rows)[6] = IThemesContractMixnMatchColumns::KEY_NOTIFICATIONS;
    (*rows)[7] = IThemesContractMixnMatchColumns::KEY_RINGTONE;
    (*rows)[8] = IThemesContractMixnMatchColumns::KEY_ALARM;
    (*rows)[9] = IThemesContractMixnMatchColumns::KEY_OVERLAYS;
    (*rows)[10] = IThemesContractMixnMatchColumns::KEY_NAVIGATION_BAR;
    return rows;
}
const AutoPtr<ArrayOf<String> > ThemesContract::MixnMatchColumns::ROWS = InitROWS();

ECode ThemesContract::MixnMatchColumns::ComponentToImageColName(
    /* [in] */ const String& component,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = NULL;

    if (component.Equals(IThemesContractMixnMatchColumns::KEY_HOMESCREEN)) {
        *str = IThemesContractThemesColumns::HOMESCREEN_URI;
        return NOERROR;
    }
    else if (component.Equals(IThemesContractMixnMatchColumns::KEY_LOCKSCREEN)) {
        *str = IThemesContractThemesColumns::LOCKSCREEN_URI;
        return NOERROR;
    }
    else if (component.Equals(IThemesContractMixnMatchColumns::KEY_BOOT_ANIM)) {
        *str = IThemesContractThemesColumns::BOOT_ANIM_URI;
        return NOERROR;
    }
    else if (component.Equals(IThemesContractMixnMatchColumns::KEY_FONT)) {
        *str = IThemesContractThemesColumns::FONT_URI;
        return NOERROR;
    }
    else if (component.Equals(IThemesContractMixnMatchColumns::KEY_ICONS)) {
        *str = IThemesContractThemesColumns::ICON_URI;
        return NOERROR;
    }
    else if (component.Equals(IThemesContractMixnMatchColumns::KEY_STATUS_BAR)) {
        *str = IThemesContractThemesColumns::STATUSBAR_URI;
        return NOERROR;
    }
    else if (component.Equals(IThemesContractMixnMatchColumns::KEY_NOTIFICATIONS)) {
        Slogger::E("ThemesContract", "Notifications mixnmatch component does not have a related column");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (component.Equals(IThemesContractMixnMatchColumns::KEY_RINGTONE)) {
        Slogger::E("ThemesContract", "Ringtone mixnmatch component does not have a related column");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (component.Equals(IThemesContractMixnMatchColumns::KEY_OVERLAYS)) {
        *str = IThemesContractThemesColumns::OVERLAYS_URI;
        return NOERROR;
    }
    else if (component.Equals(IThemesContractMixnMatchColumns::KEY_STATUS_BAR)) {
        Slogger::E("ThemesContract", "Status bar mixnmatch component does not have a related column");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (component.Equals(IThemesContractMixnMatchColumns::KEY_NAVIGATION_BAR)) {
        Slogger::E("ThemesContract", "Navigation bar mixnmatch component does not have a related column");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode ThemesContract::MixnMatchColumns::ComponentToMixNMatchKey(
    /* [in] */ const String& component,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = NULL;

    if (component.Equals(IThemesContractThemesColumns::MODIFIES_LAUNCHER)) {
        *str = IThemesContractMixnMatchColumns::KEY_HOMESCREEN;
    }
    else if (component.Equals(IThemesContractThemesColumns::MODIFIES_ICONS)) {
        *str = IThemesContractMixnMatchColumns::KEY_ICONS;
    }
    else if (component.Equals(IThemesContractThemesColumns::MODIFIES_LOCKSCREEN)) {
        *str = IThemesContractMixnMatchColumns::KEY_LOCKSCREEN;
    }
    else if (component.Equals(IThemesContractThemesColumns::MODIFIES_FONTS)) {
        *str = IThemesContractMixnMatchColumns::KEY_FONT;
    }
    else if (component.Equals(IThemesContractThemesColumns::MODIFIES_BOOT_ANIM)) {
        *str = IThemesContractMixnMatchColumns::KEY_BOOT_ANIM;
    }
    else if (component.Equals(IThemesContractThemesColumns::MODIFIES_ALARMS)) {
        *str = IThemesContractMixnMatchColumns::KEY_ALARM;
    }
    else if (component.Equals(IThemesContractThemesColumns::MODIFIES_NOTIFICATIONS)) {
        *str = IThemesContractMixnMatchColumns::KEY_NOTIFICATIONS;
    }
    else if (component.Equals(IThemesContractThemesColumns::MODIFIES_RINGTONES)) {
        *str = IThemesContractMixnMatchColumns::KEY_RINGTONE;
    }
    else if (component.Equals(IThemesContractThemesColumns::MODIFIES_OVERLAYS)) {
        *str = IThemesContractMixnMatchColumns::KEY_OVERLAYS;
    }
    else if (component.Equals(IThemesContractThemesColumns::MODIFIES_STATUS_BAR)) {
        *str = IThemesContractMixnMatchColumns::KEY_STATUS_BAR;
    }
    else if (component.Equals(IThemesContractThemesColumns::MODIFIES_NAVIGATION_BAR)) {
        *str = IThemesContractMixnMatchColumns::KEY_NAVIGATION_BAR;
    }
    return NOERROR;
}

ECode ThemesContract::MixnMatchColumns::MixNMatchKeyToComponent(
    /* [in] */ const String& mixnmatchKey,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = NULL;

    if (mixnmatchKey.Equals(IThemesContractMixnMatchColumns::KEY_HOMESCREEN)) {
        *str = IThemesContractThemesColumns::MODIFIES_LAUNCHER;
    }
    else if (mixnmatchKey.Equals(IThemesContractMixnMatchColumns::KEY_ICONS)) {
        *str = IThemesContractThemesColumns::MODIFIES_ICONS;
    }
    else if (mixnmatchKey.Equals(IThemesContractMixnMatchColumns::KEY_LOCKSCREEN)) {
        *str = IThemesContractThemesColumns::MODIFIES_LOCKSCREEN;
    }
    else if (mixnmatchKey.Equals(IThemesContractMixnMatchColumns::KEY_FONT)) {
        *str = IThemesContractThemesColumns::MODIFIES_FONTS;
    }
    else if (mixnmatchKey.Equals(IThemesContractMixnMatchColumns::KEY_BOOT_ANIM)) {
        *str = IThemesContractThemesColumns::MODIFIES_BOOT_ANIM;
    }
    else if (mixnmatchKey.Equals(IThemesContractMixnMatchColumns::KEY_ALARM)) {
        *str = IThemesContractThemesColumns::MODIFIES_ALARMS;
    }
    else if (mixnmatchKey.Equals(IThemesContractMixnMatchColumns::KEY_NOTIFICATIONS)) {
        *str = IThemesContractThemesColumns::MODIFIES_NOTIFICATIONS;
    }
    else if (mixnmatchKey.Equals(IThemesContractMixnMatchColumns::KEY_RINGTONE)) {
        *str = IThemesContractThemesColumns::MODIFIES_RINGTONES;
    }
    else if (mixnmatchKey.Equals(IThemesContractMixnMatchColumns::KEY_OVERLAYS)) {
        *str = IThemesContractThemesColumns::MODIFIES_OVERLAYS;
    }
    else if (mixnmatchKey.Equals(IThemesContractMixnMatchColumns::KEY_STATUS_BAR)) {
        *str = IThemesContractThemesColumns::MODIFIES_STATUS_BAR;
    }
    else if (mixnmatchKey.Equals(IThemesContractMixnMatchColumns::KEY_NAVIGATION_BAR)) {
        *str = IThemesContractThemesColumns::MODIFIES_NAVIGATION_BAR;
    }
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
