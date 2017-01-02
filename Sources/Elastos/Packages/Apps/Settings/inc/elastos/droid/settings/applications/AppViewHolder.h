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

#ifndef __ELASTOS_DROID_SETTINGS_APPLICATIONS_APPVIEWHOLDER_H__
#define __ELASTOS_DROID_SETTINGS_APPLICATIONS_APPVIEWHOLDER_H__

#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/Object.h>
#include "elastos/droid/settings/applications/ApplicationsState.h"

using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Applications {

// View Holder used when displaying views
class AppViewHolder
    : public Object
{
public:
    TO_STRING_IMPL("AppViewHolder")

    static CARAPI_(AutoPtr<AppViewHolder>) CreateOrRecycle(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IView* convertView);

    CARAPI_(void) UpdateSizeText(
        /* [in] */ ICharSequence* invalidSizeStr,
        /* [in] */ Int32 whichSize);

public:
    AutoPtr<ApplicationsState::AppEntry> mEntry;
    AutoPtr<IView> mRootView;
    AutoPtr<ITextView> mAppName;
    AutoPtr<IImageView> mAppIcon;
    AutoPtr<ITextView> mAppSize;
    AutoPtr<ITextView> mDisabled;
    AutoPtr<ICheckBox> mCheckBox;
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_APPLICATIONS_APPVIEWHOLDER_H__