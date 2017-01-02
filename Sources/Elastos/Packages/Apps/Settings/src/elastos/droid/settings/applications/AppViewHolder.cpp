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

#include "elastos/droid/settings/applications/AppViewHolder.h"
#include "elastos/droid/settings/applications/ManageApplications.h"
#include "../R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Applications {

AutoPtr<AppViewHolder> AppViewHolder::CreateOrRecycle(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IView* _convertView)
{
    AutoPtr<IView> convertView = _convertView;
    if (convertView == NULL) {
        inflater->Inflate(R::layout::manage_applications_item, NULL, (IView**)&convertView);

        // Creates a ViewHolder and store references to the two children views
        // we want to bind data to.
        AutoPtr<AppViewHolder> holder = new AppViewHolder();
        holder->mRootView = convertView;
        AutoPtr<IView> tmp;
        convertView->FindViewById(R::id::app_name, (IView**)&tmp);
        holder->mAppName = ITextView::Probe(tmp);
        tmp = NULL;
        convertView->FindViewById(R::id::app_icon, (IView**)&tmp);
        holder->mAppIcon = IImageView::Probe(tmp);
        tmp = NULL;
        convertView->FindViewById(R::id::app_size, (IView**)&tmp);
        holder->mAppSize = ITextView::Probe(tmp);
        tmp = NULL;
        convertView->FindViewById(R::id::app_disabled, (IView**)&tmp);
        holder->mDisabled = ITextView::Probe(tmp);
        tmp = NULL;
        convertView->FindViewById(R::id::app_on_sdcard, (IView**)&tmp);
        holder->mCheckBox = ICheckBox::Probe(tmp);
        convertView->SetTag((IObject*)holder);
        return holder;
    }
    else {
        // Get the ViewHolder back to get fast access to the TextView
        // and the ImageView.
        AutoPtr<IInterface> obj;
        convertView->GetTag((IInterface**)&obj);
        return (AppViewHolder*)IObject::Probe(obj);
    }
}

void AppViewHolder::UpdateSizeText(
    /* [in] */ ICharSequence* invalidSizeStr,
    /* [in] */ Int32 whichSize)
{
    if (ManageApplications::DEBUG) {
        Logger::I(ManageApplications::TAG, "updateSizeText of %s %s: %s",
                mEntry->mLabel.string(), TO_CSTR(mEntry), mEntry->mSizeStr.string());
    }
    if (!mEntry->mSizeStr.IsNull()) {
        switch (whichSize) {
            case ManageApplications::SIZE_INTERNAL:
                mAppSize->SetText(CoreUtils::Convert(mEntry->mInternalSizeStr));
                break;
            case ManageApplications::SIZE_EXTERNAL:
                mAppSize->SetText(CoreUtils::Convert(mEntry->mExternalSizeStr));
                break;
            default:
                mAppSize->SetText(CoreUtils::Convert(mEntry->mSizeStr));
                break;
        }
    }
    else if (mEntry->mSize == ApplicationsState::SIZE_INVALID) {
        mAppSize->SetText(invalidSizeStr);
    }
}

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos