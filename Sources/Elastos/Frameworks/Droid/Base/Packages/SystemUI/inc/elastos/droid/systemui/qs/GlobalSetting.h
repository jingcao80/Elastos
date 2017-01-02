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

#ifndef __ELASTOS_DROID_SYSTEMUI_QS_GLOBALSETTING_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_GLOBALSETTING_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Content.h"
#include <elastos/droid/database/ContentObserver.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::SystemUI::StatusBar::Policy::IListenable;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {

/** Helper for managing a global setting. **/
class GlobalSetting
    : public ContentObserver
    , public IGlobalSetting
    , public IListenable
{
public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("GlobalSetting")

    GlobalSetting();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler,
        /* [in] */ const String& settingName);

    CARAPI GetValue(
        /* [out] */ Int32* result);

    CARAPI SetValue(
        /* [in] */ Int32 value);

    // @Override
    CARAPI SetListening(
        /* [in] */ Boolean listening);

    // @Override
    CARAPI OnChange(
        /* [in] */ Boolean selfChange);

protected:
    virtual CARAPI HandleValueChanged(
        /* [in] */ Int32 value) = 0;

private:
    AutoPtr<IContext> mContext;
    String mSettingName;
};

} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_GLOBALSETTING_H__
