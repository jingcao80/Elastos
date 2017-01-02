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

#ifndef __ELASTOS_DROID_APP_CNOTIFICATIONBIGPICTURESTYLE_H__
#define __ELASTOS_DROID_APP_CNOTIFICATIONBIGPICTURESTYLE_H__

#include "_Elastos_Droid_App_CNotificationBigPictureStyle.h"
#include "elastos/droid/app/NotificationStyle.h"

using Elastos::Droid::Graphics::IBitmap;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Helper class for generating large-format notifications that include a large image attachment.
 *
 * This class is a "rebuilder": It consumes a Builder object and modifies its behavior, like so:
 * <pre class="prettyprint">
 * Notification noti = new Notification.BigPictureStyle(
 *      new Notification.Builder()
 *         .setContentTitle(&quot;New photo from &quot; + sender.toString())
 *         .setContentText(subject)
 *         .setSmallIcon(R.drawable.new_post)
 *         .setLargeIcon(aBitmap))
 *      .bigPicture(aBigBitmap)
 *      .build();
 * </pre>
 *
 * @see Notification#bigContentView
 */
CarClass(CNotificationBigPictureStyle)
    , public NotificationStyle
    , public INotificationBigPictureStyle
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CNotificationBigPictureStyle();

    virtual ~CNotificationBigPictureStyle();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ INotificationBuilder* builder);

    CARAPI SetBuilder(
        /* [in] */ INotificationBuilder* builder);

    /**
     * Overrides ContentTitle in the big form of the template.
     * This defaults to the value passed to setContentTitle().
     */
    CARAPI SetBigContentTitle(
        /* [in] */ ICharSequence* title);

    /**
     * Set the first line of text after the detail section in the big form of the template.
     */
    CARAPI SetSummaryText(
        /* [in] */ ICharSequence* cs);

    /**
     * Provide the bitmap to be used as the payload for the BigPicture notification.
     */
    CARAPI BigPicture(
        /* [in] */ IBitmap* b);

    /**
     * Override the large icon when the big notification is shown.
     */
    CARAPI BigLargeIcon(
        /* [in] */ IBitmap* b);

    /**
     * @hide
     */
    CARAPI AddExtras(
        /* [in] */ IBundle* extras);

    /**
     * @hide
     */
    CARAPI RestoreFromExtras(
        /* [in] */ IBundle* extras);

    /**
     * @hide
     */
    CARAPI PopulateBigContentView(
        /* [in] */ INotification* wip);

private:
    AutoPtr<IRemoteViews> MakeBigContentView();

private:
    AutoPtr<IBitmap> mPicture;
    AutoPtr<IBitmap> mBigLargeIcon;
    Boolean mBigLargeIconSet;// = false;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CNOTIFICATIONBIGPICTURESTYLE_H__
