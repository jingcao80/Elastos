#ifndef __ELASTOS_DROID_MEDIA_TV_CTVINPUTINFOHELPER_H__
#define __ELASTOS_DROID_MEDIA_TV_CTVINPUTINFOHELPER_H__

#include "_Elastos_Droid_Media_Tv_CTvInputInfoHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

CarClass(CTvInputInfoHelper)
    , public Singleton
    , public ITvInputInfoHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Create a new instance of the TvInputInfo class,
     * instantiating it from the given Context and ResolveInfo.
     *
     * @param service The ResolveInfo returned from the package manager about this TV input service.
     * @hide
     */
    CARAPI CreateTvInputInfo(
        /* [in] */ IContext * context,
        /* [in] */ IResolveInfo * service,
        /* [out] */ ITvInputInfo ** result);

    /**
     * Create a new instance of the TvInputInfo class, instantiating it from the given Context,
     * ResolveInfo, and HdmiDeviceInfo.
     *
     * @param service The IResolveInfo * returned from the package manager about this TV input service.
     * @param hdmiDeviceInfo The HdmiDeviceInfo for a HDMI CEC logical device.
     * @param parentId The ID of this TV input's parent input. {@code null} if none exists.
     * @param iconUri The {@link android.net.Uri} to load the icon image. See
     *            {@link android.content.ContentResolver#openInputStream}. If it is {@code null},
     *            the application icon of {@code service} will be loaded.
     * @param label The label of this TvInputInfo. If it is {@code null} or empty, {@code service}
     *            label will be loaded.
     * @hide
     */
    CARAPI CreateTvInputInfo(
        /* [in] */ IContext * context,
        /* [in] */ IResolveInfo * service,
        /* [in] */ IHdmiDeviceInfo * hdmiDeviceInfo,
        /* [in] */ const String& parentId,
        /* [in] */ const String& label,
        /* [in] */ IUri * iconUri,
        /* [out] */ ITvInputInfo ** result);

    /**
     * Create a new instance of the TvInputInfo class, instantiating it from the given Context,
     * ResolveInfo, and TvInputHardwareInfo.
     *
     * @param service The IResolveInfo * returned from the package manager about this TV input service.
     * @param hardwareInfo The TvInputHardwareInfo for a TV input hardware device.
     * @param iconUri The {@link android.net.Uri} to load the icon image. See
     *            {@link android.content.ContentResolver#openInputStream}. If it is {@code null},
     *            the application icon of {@code service} will be loaded.
     * @param label The label of this TvInputInfo. If it is {@code null} or empty, {@code service}
     *            label will be loaded.
     * @hide
     */
    CARAPI CreateTvInputInfo(
        /* [in] */ IContext * context,
        /* [in] */ IResolveInfo * service,
        /* [in] */ ITvInputHardwareInfo * hardwareInfo,
        /* [in] */ const String& label,
        /* [in] */ IUri * iconUri,
        /* [out] */ ITvInputInfo ** result);
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_CTVINPUTINFOHELPER_H__
