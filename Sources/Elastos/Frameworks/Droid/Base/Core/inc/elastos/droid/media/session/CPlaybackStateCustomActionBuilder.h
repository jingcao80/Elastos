#ifndef __ELASTOS_DROID_MEDIA_SESSION_CPLAYBACKSTATECUSTOMACTIONBUILDER_H__
#define __ELASTOS_DROID_MEDIA_SESSION_CPLAYBACKSTATECUSTOMACTIONBUILDER_H__

#include "_Elastos_Droid_Media_Session_CPlaybackStateCustomActionBuilder.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

/**
 * Builder for {@link CustomAction} objects.
 */
CarClass(CPlaybackStateCustomActionBuilder)
    , public Object
    , public IPlaybackStateCustomActionBuilder
{
public:
    CPlaybackStateCustomActionBuilder();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CPlaybackStateCustomActionBuilder();

    /**
     * Creates a {@link CustomAction} builder with the id, name, and icon set.
     *
     * @param action The action of the {@link CustomAction}.
     * @param name The display name of the {@link CustomAction}. This name will be displayed
     *             along side the action if the UI supports it.
     * @param icon The icon resource id of the {@link CustomAction}. This resource id
     *             must be in the same package as the {@link MediaSession}. It will be
     *             displayed with the custom action if the UI supports it.
     */
    CARAPI constructor(
        /* [in] */ const String& action,
        /* [in] */ ICharSequence * name,
        /* [in] */ Int32 icon);

    /**
     * Set optional extras for the {@link CustomAction}. These extras are meant to be
     * consumed by a {@link MediaController} if it knows how to handle them.
     * Keys should be fully qualified (e.g. "com.example.MY_ARG") to avoid collisions.
     *
     * @param extras Optional extras for the {@link CustomAction}.
     * @return this.
     */
    CARAPI SetExtras(
        /* [in] */ IBundle * extras);

    /**
     * Build and return the {@link CustomAction} instance with the specified values.
     *
     * @return A new {@link CustomAction} instance.
     */
    CARAPI Build(
        /* [out] */ IPlaybackStateCustomAction ** result);

private:
    String mAction;
    AutoPtr<ICharSequence> mName;
    Int32 mIcon;
    AutoPtr<IBundle> mExtras;
};

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_SESSION_CPLAYBACKSTATECUSTOMACTIONBUILDER_H__
