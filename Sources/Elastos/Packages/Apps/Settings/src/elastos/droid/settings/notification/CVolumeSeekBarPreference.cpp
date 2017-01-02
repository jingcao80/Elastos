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

#include "Elastos.Droid.View.h"
#include "elastos/droid/settings/notification/CVolumeSeekBarPreference.h"
#include "../R.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Preference::EIID_ISeekBarVolumizerCallback;
using Elastos::Droid::Preference::EIID_IPreferenceManagerOnActivityStopListener;
using Elastos::Droid::Preference::IPreferenceManager;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

//===============================================================================
//                  CVolumeSeekBarPreference::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL(CVolumeSeekBarPreference::InnerListener, Object, IPreferenceManagerOnActivityStopListener)

CVolumeSeekBarPreference::InnerListener::InnerListener(
    /* [in] */ CVolumeSeekBarPreference* host)
    : mHost(host)
{}

ECode CVolumeSeekBarPreference::InnerListener::OnActivityStop()
{
    return mHost->OnActivityStop();
}

//===============================================================================
//                  CVolumeSeekBarPreference::SeekBarVolumizerCallback
//===============================================================================

CAR_INTERFACE_IMPL(CVolumeSeekBarPreference::SeekBarVolumizerCallback, Object, ISeekBarVolumizerCallback)

CVolumeSeekBarPreference::SeekBarVolumizerCallback::SeekBarVolumizerCallback(
    /* [in] */ CVolumeSeekBarPreference* host)
    : mHost(host)
{}

ECode CVolumeSeekBarPreference::SeekBarVolumizerCallback::OnSampleStarting(
    /* [in] */ ISeekBarVolumizer* sbv)
{
    if (mHost->mCallback != NULL) {
        mHost->mCallback->OnSampleStarting(sbv);
    }
    return NOERROR;
}

//===============================================================================
//                  CVolumeSeekBarPreference::OnBindViewSeekBarVolumizer
//===============================================================================

CVolumeSeekBarPreference::OnBindViewSeekBarVolumizer::OnBindViewSeekBarVolumizer()
{}

ECode CVolumeSeekBarPreference::OnBindViewSeekBarVolumizer::constructor(
    /* [in] */ CVolumeSeekBarPreference* host,
    /* [in] */ IContext* context,
    /* [in] */ Int32 streamType,
    /* [in] */ IUri* defaultUri,
    /* [in] */ ISeekBarVolumizerCallback* callback)
{
    mHost = host;
    return SeekBarVolumizer::constructor(context, streamType, defaultUri, callback);
}

ECode CVolumeSeekBarPreference::OnBindViewSeekBarVolumizer::OnProgressChanged(
    /* [in] */ ISeekBar* seekBar,
    /* [in] */ Int32 progress,
    /* [in] */ Boolean fromTouch)
{
    SeekBarVolumizer::OnProgressChanged(seekBar, progress, fromTouch);
    return mHost->mCallback->OnStreamValueChanged(mHost->mStream, progress);
}

//===============================================================================
//                  CVolumeSeekBarPreference
//===============================================================================

const String CVolumeSeekBarPreference::TAG("CVolumeSeekBarPreference");

CAR_OBJECT_IMPL(CVolumeSeekBarPreference)

CVolumeSeekBarPreference::CVolumeSeekBarPreference()
    : mStream(0)
{}

ECode CVolumeSeekBarPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    return SeekBarPreference::constructor(context, attrs, defStyleAttr, defStyleRes);
}

ECode CVolumeSeekBarPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode CVolumeSeekBarPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CVolumeSeekBarPreference::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CVolumeSeekBarPreference::SetStream(
    /* [in] */ Int32 stream)
{
    mStream = stream;
    return NOERROR;
}

ECode CVolumeSeekBarPreference::SetCallback(
    /* [in] */ IVolumeSeekBarPreferenceCallback* callback)
{
    mCallback = callback;
    return NOERROR;
}

ECode CVolumeSeekBarPreference::OnActivityStop()
{
    if (mVolumizer != NULL) {
        mVolumizer->Stop();
    }
    return NOERROR;
}

ECode CVolumeSeekBarPreference::OnBindView(
    /* [in] */ IView* view)
{
    SeekBarPreference::OnBindView(view);
    if (mStream == 0) {
        Logger::W(TAG, "No stream found, not binding volumizer");
        return NOERROR;
    }
    AutoPtr<IPreferenceManager> pm;
    GetPreferenceManager((IPreferenceManager**)&pm);
    AutoPtr<InnerListener> listener = new InnerListener(this);
    pm->RegisterOnActivityStopListener(listener);
    AutoPtr<IView> tmp;
    view->FindViewById(Elastos::Droid::R::id::seekbar, (IView**)&tmp);
    AutoPtr<ISeekBar> seekBar = ISeekBar::Probe(tmp);
    if (seekBar == mSeekBar) return NOERROR;
    mSeekBar = seekBar;
    AutoPtr<ISeekBarVolumizerCallback> sbvc = new SeekBarVolumizerCallback(this);

    AutoPtr<IUri> sampleUri = mStream == IAudioManager::STREAM_MUSIC ? GetMediaVolumeUri() : NULL;
    if (mVolumizer == NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        mVolumizer = new OnBindViewSeekBarVolumizer();
        mVolumizer->constructor(this, context, mStream, sampleUri, sbvc);
    }
    mVolumizer->SetSeekBar(mSeekBar);
    tmp = NULL;
    view->FindViewById(Elastos::Droid::R::id::icon, (IView**)&tmp);
    mIconView = IImageView::Probe(tmp);
    Int32 progress;
    IProgressBar::Probe(mSeekBar)->GetProgress(&progress);
    return mCallback->OnStreamValueChanged(mStream, progress);
}

ECode CVolumeSeekBarPreference::OnProgressChanged(
    /* [in] */ ISeekBar* seekBar,
    /* [in] */ Int32 progress,
    /* [in] */ Boolean fromTouch)
{
    SeekBarPreference::OnProgressChanged(seekBar, progress, fromTouch);
    return mCallback->OnStreamValueChanged(mStream, progress);
}

ECode CVolumeSeekBarPreference::ShowIcon(
    /* [in] */ Int32 resId)
{
    // Instead of using setIcon, which will trigger listeners, this just decorates the
    // preference temporarily with a new icon.
    if (mIconView != NULL) {
        mIconView->SetImageResource(resId);
    }
    return NOERROR;
}

AutoPtr<IUri> CVolumeSeekBarPreference::GetMediaVolumeUri()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    String pkgName;
    context->GetPackageName(&pkgName);
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    StringBuilder builder;
    builder += IContentResolver::SCHEME_ANDROID_RESOURCE;
    builder += "://";
    builder += pkgName;
    builder += "/";
    builder += R::raw::media_volume;
    AutoPtr<IUri> uri;
    helper->Parse(builder.ToString(), (IUri**)&uri);
    return uri;
}

} // namespace Notification
} // namespace Settings
} // namespace Droid
} // namespace Elastos