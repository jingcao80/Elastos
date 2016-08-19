
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
    /* [in] */ IView* convertView)
{
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