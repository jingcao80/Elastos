#ifndef __ELASTOS_DROID_MEDIA_SESSION_CPLAYBACKSTATECUSTOMACTION_H__
#define __ELASTOS_DROID_MEDIA_SESSION_CPLAYBACKSTATECUSTOMACTION_H__

#include "_Elastos_Droid_Media_Session_CPlaybackStateCustomAction.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

/**
 * {@link PlaybackState.CustomAction CustomActions} can be used to extend the capabilities of
 * the standard transport controls by exposing app specific actions to
 * {@link MediaController MediaControllers}.
 */
CarClass(CPlaybackStateCustomAction)
    , public Object
    , public IPlaybackStateCustomAction
    , public IParcelable
{
public:
    CPlaybackStateCustomAction();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CPlaybackStateCustomAction();

    /**
     * Use {@link PlaybackState.CustomAction.Builder#build()}.
     */
    CARAPI constructor(
        /* [in] */ const String& action,
        /* [in] */ ICharSequence * name,
        /* [in] */ Int32 icon,
        /* [in] */ IBundle * extras);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * Returns the action of the {@link CustomAction}.
     *
     * @return The action of the {@link CustomAction}.
     */
    CARAPI GetAction(
        /* [out] */ String * result);

    /**
     * Returns the display name of this action. e.g. "Favorite"
     *
     * @return The display name of this {@link CustomAction}.
     */
    CARAPI GetName(
        /* [out] */ ICharSequence ** result);

    /**
     * Returns the resource id of the icon in the {@link MediaSession MediaSession's} package.
     *
     * @return The resource id of the icon in the {@link MediaSession MediaSession's} package.
     */
    CARAPI GetIcon(
        /* [out] */ Int32 * result);

    /**
     * Returns extras which provide additional application-specific information about the
     * action, or null if none. These arguments are meant to be consumed by a
     * {@link MediaController} if it knows how to handle them.
     *
     * @return Optional arguments for the {@link CustomAction}.
     */
    CARAPI GetExtras(
        /* [out] */ IBundle ** result);

    CARAPI ToString(
        /* [out] */ String * result);

public:
    String mAction;
    AutoPtr<ICharSequence> mName;
    Int32 mIcon;
    AutoPtr<IBundle> mExtras;
};

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_SESSION_CPLAYBACKSTATECUSTOMACTION_H__
