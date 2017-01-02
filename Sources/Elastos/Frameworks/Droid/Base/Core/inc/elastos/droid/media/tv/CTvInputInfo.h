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

#ifndef __ELASTOS_DROID_MEDIA_TV_CTVINPUTINFO_H__
#define __ELASTOS_DROID_MEDIA_TV_CTVINPUTINFO_H__

#include "_Elastos_Droid_Media_Tv_CTvInputInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Utility::ISparseInt32Array;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

/**
 * This class is used to specify meta information of a TV input.
 */
CarClass(CTvInputInfo)
    , public Object
    , public ITvInputInfo
    , public IParcelable
{
public:
    CTvInputInfo();

    virtual ~CTvInputInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /**
     * Constructor.
     *
     * @param service The ResolveInfo returned from the package manager about this TV input service.
     * @param id ID of this TV input. Should be generated via generateInputId*().
     * @param parentId ID of this TV input's parent input. {@code null} if none exists.
     * @param type The type of this TV input service.
     */
    CARAPI constructor(
        /* [in] */ IResolveInfo* service,
        /* [in] */ const String& id,
        /* [in] */ const String& parentId,
        /* [in] */ Int32 type);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * Returns a unique ID for this TV input. The ID is generated from the package and class name
     * implementing the TV input service.
     */
    CARAPI GetId(
        /* [out] */ String* result);

    /**
     * Returns the parent input ID.
     * <p>
     * A TV input may have a parent input if the TV input is actually a logical representation of
     * a device behind the hardware port represented by the parent input.
     * For example, a HDMI CEC logical device, connected to a HDMI port, appears as another TV
     * input. In this case, the parent input of this logical device is the HDMI port.
     * </p><p>
     * Applications may group inputs by parent input ID to provide an easier access to inputs
     * sharing the same physical port. In the example of HDMI CEC, logical HDMI CEC devices behind
     * the same HDMI port have the same parent ID, which is the ID representing the port. Thus
     * applications can group the hardware HDMI port and the logical HDMI CEC devices behind it
     * together using this method.
     * </p>
     *
     * @return the ID of the parent input, if exists. Returns {@code null} if the parent input is
     *         not specified.
     */
    CARAPI GetParentId(
        /* [out] */ String* result);

    /**
     * Returns the information of the service that implements this TV input.
     */
    CARAPI GetServiceInfo(
        /* [out] */ IServiceInfo** result);

    /**
     * Returns the component of the service that implements this TV input.
     * @hide
     */
    CARAPI GetComponent(
        /* [out] */ IComponentName** result);

    /**
     * Returns an intent to start the setup activity for this TV input.
     */
    CARAPI CreateSetupIntent(
        /* [out] */ IIntent** result);

    /**
     * Returns an intent to start the settings activity for this TV input.
     */
    CARAPI CreateSettingsIntent(
        /* [out] */ IIntent** result);

    /**
     * Returns the type of this TV input.
     */
    CARAPI GetType(
        /* [out] */ Int32* result);

    /**
     * Returns the HDMI device information of this TV input.
     * @hide
     */
    CARAPI GetHdmiDeviceInfo(
        /* [out] */ IHdmiDeviceInfo** result);

    /**
     * Returns {@code true} if this TV input is pass-though which does not have any real channels in
     * TvProvider. {@code false} otherwise.
     *
     * @see TvContract#buildChannelUriForPassthroughInput(String)
     */
    CARAPI IsPassthroughInput(
        /* [out] */ Boolean* result);

    /**
     * Returns {@code true}, if a CEC device for this TV input is connected to an HDMI switch, i.e.,
     * the device isn't directly connected to a HDMI port.
     * @hide
     */
    CARAPI IsConnectedToHdmiSwitch(
        /* [out] */ Boolean* result);

    /**
     * Checks if this TV input is marked hidden by the user in the settings.
     *
     * @param context Supplies a {@link Context} used to check if this TV input is hidden.
     * @return {@code true} if the user marked this TV input hidden in settings. {@code false}
     *         otherwise.
     * @hide
     */
    CARAPI IsHidden(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    /**
     * Loads the user-displayed label for this TV input.
     *
     * @param context Supplies a {@link Context} used to load the label.
     * @return a CharSequence containing the TV input's label. If the TV input does not have
     *         a label, its name is returned.
     */
    CARAPI LoadLabel(
        /* [in] */ IContext* context,
        /* [out] */ ICharSequence** result);

    /**
     * Loads the custom label set by user in settings.
     *
     * @param context Supplies a {@link Context} used to load the custom label.
     * @return a CharSequence containing the TV input's custom label. {@code null} if there is no
     *         custom label.
     * @hide
     */
    CARAPI LoadCustomLabel(
        /* [in] */ IContext* context,
        /* [out] */ ICharSequence** result);

    /**
     * Loads the user-displayed icon for this TV input.
     *
     * @param context Supplies a {@link Context} used to load the icon.
     * @return a Drawable containing the TV input's icon. If the TV input does not have an icon,
     *         application's icon is returned. If it's unavailable too, {@code null} is returned.
     */
    CARAPI LoadIcon(
        /* [in] */ IContext* context,
        /* [out] */ IDrawable** result);

    /**
     * Create a new instance of the TvInputInfo class,
     * instantiating it from the given Context and ResolveInfo.
     *
     * @param service The ResolveInfo returned from the package manager about this TV input service.
     * @hide
     */
    static CARAPI CreateTvInputInfo(
        /* [in] */ IContext* context,
        /* [in] */ IResolveInfo* service,
        /* [out] */ ITvInputInfo** result);

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
    static CARAPI CreateTvInputInfo(
        /* [in] */ IContext* context,
        /* [in] */ IResolveInfo* service,
        /* [in] */ IHdmiDeviceInfo* hdmiDeviceInfo,
        /* [in] */ const String& parentId,
        /* [in] */ const String& label,
        /* [in] */ IUri* iconUri,
        /* [out] */ ITvInputInfo** result);

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
    static CARAPI CreateTvInputInfo(
        /* [in] */ IContext* context,
        /* [in] */ IResolveInfo* service,
        /* [in] */ ITvInputHardwareInfo* hardwareInfo,
        /* [in] */ const String& label,
        /* [in] */ IUri* iconUri,
        /* [out] */ ITvInputInfo** result);

    CARAPI GetHashCode(
        /* [out] */ Int32* code);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* isEqual);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI_(AutoPtr<IDrawable>) LoadServiceIcon(
        /* [in] */ IContext* context);

    static CARAPI CreateTvInputInfo(
        /* [in] */ IContext* context,
        /* [in] */ IResolveInfo* service,
        /* [in] */ const String& id,
        /* [in] */ const String& parentId,
        /* [in] */ Int32 inputType,
        /* [in] */ const String& label,
        /* [in] */ IUri* iconUri,
        /* [in] */ Boolean isConnectedToHdmiSwitch,
        /* [out] */ ITvInputInfo** result);

    /**
     * Used to generate an input id from a ComponentName.
     *
     * @param name the component name for generating an input id.
     * @return the generated input id for the given {@code name}.
     */
    static CARAPI_(String) GenerateInputIdForComponentName(
        /* [in] */ IComponentName* name);

    /**
     * Used to generate an input id from a ComponentName and HdmiDeviceInfo.
     *
     * @param name the component name for generating an input id.
     * @param deviceInfo HdmiDeviceInfo describing this TV input.
     * @return the generated input id for the given {@code name} and {@code deviceInfo}.
     */
    static CARAPI_(String) GenerateInputIdForHdmiDevice(
        /* [in] */ IComponentName* name,
        /* [in] */ IHdmiDeviceInfo* deviceInfo);

    /**
     * Used to generate an input id from a ComponentName and TvInputHardwareInfo
     *
     * @param name the component name for generating an input id.
     * @param hardwareInfo TvInputHardwareInfo describing this TV input.
     * @return the generated input id for the given {@code name} and {@code hardwareInfo}.
     */
    static CARAPI_(String) GenerateInputIdForHardware(
        /* [in] */ IComponentName* name,
        /* [in] */ ITvInputHardwareInfo* hardwareInfo);

private:
    static Boolean DEBUG;
    static String TAG;

    static AutoPtr<ISparseInt32Array> sHardwareTypeToTvInputType;

    static String XML_START_TAG_NAME;
    static String DELIMITER_INFO_IN_ID;
    static String PREFIX_HDMI_DEVICE;
    static String PREFIX_HARDWARE_DEVICE;
    static Int32 LENGTH_HDMI_PHYSICAL_ADDRESS;
    static Int32 LENGTH_HDMI_DEVICE_ID;

    AutoPtr<IResolveInfo> mService;
    String mId;
    String mParentId;

    // Attributes from XML meta data.
    String mSetupActivity;
    String mSettingsActivity;

    Int32 mType;
    AutoPtr<IHdmiDeviceInfo> mHdmiDeviceInfo;
    String mLabel;
    AutoPtr<IUri> mIconUri;
    Boolean mIsConnectedToHdmiSwitch;
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_CTVINPUTINFO_H__
