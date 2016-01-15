
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_IMPL_CAMERAMETADATANATIVE_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_IMPL_CAMERAMETADATANATIVE_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>

#include <camera/CameraMetadata.h>

using Elastos::Droid::Hardware::Camera2::Utils::ITypeReference;
using Elastos::Droid::Hardware::Camera2::Params::IFace;
using Elastos::Droid::Hardware::Camera2::Params::ILensShadingMap;
using Elastos::Droid::Hardware::Camera2::Params::ITonemapCurve;
using Elastos::Droid::Hardware::Camera2::Params::IStreamConfigurationMap;
using Elastos::Droid::Hardware::Camera2::Marshal::IMarshaler;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Location::ILocation;
using Elastos::Core::Object;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Impl {

class CameraMetadataNative
    : public Object
    , public ICameraMetadataNative
    , public IParcelable
{
public:
    class Key
        : public Object
        , public ICameraMetadataNativeKey
    {
    public:
        CAR_INTERFACE_DECL()

        /**
         * Visible for testing only.
         *
         * <p>Use the CameraCharacteristics.Key, CaptureResult.Key, or CaptureRequest.Key
         * for application code or vendor-extended keys.</p>
         */
        Key(
            /* [in] */ const String& name,
            /* [in] */ ClassID type);


        /**
         * Visible for testing only.
         *
         * <p>Use the CameraCharacteristics.Key, CaptureResult.Key, or CaptureRequest.Key
         * for application code or vendor-extended keys.</p>
         */
        //@SuppressWarnings("unchecked")
        Key(
            /* [in] */ const String& name,
            /* [in] */ ITypeReference* typeReference);

        /**
         * Return a camelCase, period separated name formatted like:
         * {@code "root.section[.subsections].name"}.
         *
         * <p>Built-in keys exposed by the Android SDK are always prefixed with {@code "android."};
         * keys that are device/platform-specific are prefixed with {@code "com."}.</p>
         *
         * <p>For example, {@code CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP} would
         * have a name of {@code "android.scaler.streamConfigurationMap"}; whereas a device
         * specific key might look like {@code "com.google.nexus.data.private"}.</p>
         *
         * @return String representation of the key name
         */
        CARAPI GetName(
            /* [out] */ String* name);

        /**
         * {@inheritDoc}
         */
        //@Override
        CARAPI GetHashCode(
            /* [out] */ Int32* code);

       /**
         * Compare this key against other native keys, request keys, result keys, and
         * characteristics keys.
         *
         * <p>Two keys are considered equal if their name and type reference are equal.</p>
         *
         * <p>Note that the equality against non-native keys is one-way. A native key may be equal
         * to a result key; but that same result key will not be equal to a native key.</p>
         */
        //@SuppressWarnings("rawtypes")
        //@Override
        CARAPI Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* e);

        /**
         * <p>
         * Get the tag corresponding to this key. This enables insertion into the
         * native metadata.
         * </p>
         *
         * <p>This value is looked up the first time, and cached subsequently.</p>
         *
         * @return The tag numeric value corresponding to the string
         */
        CARAPI GetTag(
            /* [out] */ Int32* tag);

        /**
         * Get the raw class backing the type {@code T} for this key.
         *
         * <p>The distinction is only important if {@code T} is a generic, e.g.
         * {@code Range<Integer>} since the nested type will be erased.</p>
         */
        CARAPI GetType(
            /* [out] */ ClassID* type);

        /**
         * Get the type reference backing the type {@code T} for this key.
         *
         * <p>The distinction is only important if {@code T} is a generic, e.g.
         * {@code Range<Integer>} since the nested type will be retained.</p>
         */
        CARAPI GetTypeReference(
            /* [out] */ ITypeReference** ref);

    private:
        Boolean mHasTag;
        Int32 mTag;
        ClassID mType;
        AutoPtr<ITypeReference> mTypeReference;
        String mName;
        Int32 mHash;
    };

private:
    class GetCommand_AvailableFormats
        : public Object
        , public IGetCommand
    {
    public:
        CAR_INTERFACE_DECL();

        // @Override
        // @SuppressWarnings("unchecked")
        CARAPI GetValue(
            /* [in] */ ICameraMetadataNative* metadata,
            /* [in] */ ICameraMetadataNativeKey* key,
            /* [out] */ IInterface** outface);
    };

    class GetCommand_Faces
        : public Object
        , public IGetCommand
    {
    public:
        CAR_INTERFACE_DECL();

        // @Override
        // @SuppressWarnings("unchecked")
        CARAPI GetValue(
            /* [in] */ ICameraMetadataNative* metadata,
            /* [in] */ ICameraMetadataNativeKey* key,
            /* [out] */ IInterface** outface);
    };

    class GetCommand_FaceRectangles
        : public Object
        , public IGetCommand
    {
    public:
        CAR_INTERFACE_DECL();

        // @Override
        // @SuppressWarnings("unchecked")
        CARAPI GetValue(
            /* [in] */ ICameraMetadataNative* metadata,
            /* [in] */ ICameraMetadataNativeKey* key,
            /* [out] */ IInterface** outface);
    };

    class GetCommand_StreamConfigurationMap
        : public Object
        , public IGetCommand
    {
    public:
        CAR_INTERFACE_DECL();

        // @Override
        // @SuppressWarnings("unchecked")
        CARAPI GetValue(
            /* [in] */ ICameraMetadataNative* metadata,
            /* [in] */ ICameraMetadataNativeKey* key,
            /* [out] */ IInterface** outface);
    };

    class GetCommand_MaxRegions
        : public Object
        , public IGetCommand
    {
    public:
        CAR_INTERFACE_DECL();

        // @Override
        // @SuppressWarnings("unchecked")
        CARAPI GetValue(
            /* [in] */ ICameraMetadataNative* metadata,
            /* [in] */ ICameraMetadataNativeKey* key,
            /* [out] */ IInterface** outface);
    };

    class GetCommand_MaxNumOutputs
        : public Object
        , public IGetCommand
    {
    public:
        CAR_INTERFACE_DECL();

        // @Override
        // @SuppressWarnings("unchecked")
        CARAPI GetValue(
            /* [in] */ ICameraMetadataNative* metadata,
            /* [in] */ ICameraMetadataNativeKey* key,
            /* [out] */ IInterface** outface);
    };

    class GetCommand_TonemapCurve
        : public Object
        , public IGetCommand
    {
    public:
        CAR_INTERFACE_DECL();

        // @Override
        // @SuppressWarnings("unchecked")
        CARAPI GetValue(
            /* [in] */ ICameraMetadataNative* metadata,
            /* [in] */ ICameraMetadataNativeKey* key,
            /* [out] */ IInterface** outface);
    };

    class GetCommand_GpsLocation
        : public Object
        , public IGetCommand
    {
    public:
        CAR_INTERFACE_DECL();

        // @Override
        // @SuppressWarnings("unchecked")
        CARAPI GetValue(
            /* [in] */ ICameraMetadataNative* metadata,
            /* [in] */ ICameraMetadataNativeKey* key,
            /* [out] */ IInterface** outface);
    };

    class GetCommand_LensShadingMap
        : public Object
        , public IGetCommand
    {
    public:
        CAR_INTERFACE_DECL();

        // @Override
        // @SuppressWarnings("unchecked")
        CARAPI GetValue(
            /* [in] */ ICameraMetadataNative* metadata,
            /* [in] */ ICameraMetadataNativeKey* key,
            /* [out] */ IInterface** outface);
    };

    class SetCommand_AvailableFormats
        : public Object
        , public ISetCommand
    {
    public:
        CAR_INTERFACE_DECL();

        // @Override
        // @SuppressWarnings("unchecked")
        CARAPI SetValue(
            /* [in] */ ICameraMetadataNative* metadata,
            /* [in] */ IInterface* value);
    };

    class SetCommand_FaceRectangles
        : public Object
        , public ISetCommand
    {
    public:
        CAR_INTERFACE_DECL();

        // @Override
        // @SuppressWarnings("unchecked")
        CARAPI SetValue(
            /* [in] */ ICameraMetadataNative* metadata,
            /* [in] */ IInterface* value);
    };

    class SetCommand_Faces
        : public Object
        , public ISetCommand
    {
    public:
        CAR_INTERFACE_DECL();

        // @Override
        // @SuppressWarnings("unchecked")
        CARAPI SetValue(
            /* [in] */ ICameraMetadataNative* metadata,
            /* [in] */ IInterface* value);
    };

    class SetCommand_TonemapCurve
        : public Object
        , public ISetCommand
    {
    public:
        CAR_INTERFACE_DECL();

        // @Override
        // @SuppressWarnings("unchecked")
        CARAPI SetValue(
            /* [in] */ ICameraMetadataNative* metadata,
            /* [in] */ IInterface* value);
    };

    class SetCommand_GpsLocation
        : public Object
        , public ISetCommand
    {
    public:
        CAR_INTERFACE_DECL();

        // @Override
        // @SuppressWarnings("unchecked")
        CARAPI SetValue(
            /* [in] */ ICameraMetadataNative* metadata,
            /* [in] */ IInterface* value);
    };

public:
    CAR_INTERFACE_DECL();

    CameraMetadataNative();

    virtual ~CameraMetadataNative();

    CARAPI constructor();

    /**
     * Copy constructor - clone metadata
     */
    CARAPI constructor(
        /* [in] */ ICameraMetadataNative* requestId);

    /**
     * Move the contents from {@code other} into a new camera metadata instance.</p>
     *
     * <p>After this call, {@code other} will become empty.</p>
     *
     * @param other the previous metadata instance which will get pilfered
     * @return a new metadata instance with the values from {@code other} moved into it
     */
    static CARAPI Move(
        /* [in] */ ICameraMetadataNative* other,
        /* [out] */ ICameraMetadataNative** result);

    // public static final Parcelable.Creator<CameraMetadataNative> CREATOR =
    //         new Parcelable.Creator<CameraMetadataNative>() {
    //     @Override
    //     public CameraMetadataNative createFromParcel(Parcel in) {
    //         CameraMetadataNative metadata = new CameraMetadataNative();
    //         metadata.readFromParcel(in);
    //         return metadata;
    //     }

    //     @Override
    //     public CameraMetadataNative[] newArray(int size) {
    //         return new CameraMetadataNative[size];
    //     }
    // };

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * @hide
     */
    CARAPI Get(
        /* [in] */ ICameraCharacteristicsKey* key,
        /* [out] */ IInterface** result);

    /**
     * @hide
     */
    CARAPI Get(
        /* [in] */ ICaptureResultKey* key,
        /* [out] */ IInterface** result);

    /**
     * @hide
     */
    CARAPI Get(
        /* [in] */ ICaptureRequestKey* key,
        /* [out] */ IInterface** result);

    /**
     * Look-up a metadata field value by its key.
     *
     * @param key a non-{@code null} key instance
     * @return the field corresponding to the {@code key}, or {@code null} if no value was set
     */
    CARAPI Get(
        /* [in] */ ICameraMetadataNativeKey* key,
        /* [out] */ IInterface** result);

    //@Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /**
     * Set the global client-side vendor tag descriptor to allow use of vendor
     * tags in camera applications.
     *
     * @return int A native status_t value corresponding to one of the
     * {@link CameraBinderDecorator} integer constants.
     * @see CameraBinderDecorator#throwOnError
     *
     * @hide
     */
    static CARAPI NativeSetupGlobalVendorTagDescriptor(
        /* [out] */ Int32* descriptor);

    /**
     * Set a camera metadata field to a value. The field definitions can be
     * found in {@link CameraCharacteristics}, {@link CaptureResult}, and
     * {@link CaptureRequest}.
     *
     * @param key The metadata field to write.
     * @param value The value to set the field to, which must be of a matching
     * type to the key.
     */
    CARAPI Set(
        /* [in] */ ICameraMetadataNativeKey* key,
        /* [in] */ IInterface* value);

    CARAPI Set(
        /* [in] */ ICaptureRequestKey* key,
        /* [in] */ IInterface* value);

    CARAPI Set(
        /* [in] */ ICaptureResultKey* key,
        /* [in] */ IInterface* value);

    CARAPI Set(
        /* [in] */ ICameraCharacteristicsKey* key,
        /* [in] */ IInterface* value);

    /**
     * <p>Perform a 0-copy swap of the internal metadata with another object.</p>
     *
     * <p>Useful to convert a CameraMetadata into e.g. a CaptureRequest.</p>
     *
     * @param other Metadata to swap with
     * @throws NullPointerException if other was null
     * @hide
     */
    CARAPI Swap(
        /* [in] */ ICameraMetadataNative* other);

    /**
     * @hide
     */
    CARAPI GetEntryCount(
        /* [out] */ Int32* count);

    /**
     * Does this metadata contain at least 1 entry?
     *
     * @hide
     */
    CARAPI IsEmpty(
        /* [out] */ Boolean* result);

    /**
     * Convert a key string into the equivalent native tag.
     *
     * @throws IllegalArgumentException if the key was not recognized
     * @throws NullPointerException if the key was null
     *
     * @hide
     */
    static CARAPI GetTag(
        /* [in] */ const String& key,
        /* [out] */ Int32* tag);

    /**
     * Get the underlying native type for a tag.
     *
     * @param tag An integer tag, see e.g. {@link #getTag}
     * @return An int enum for the metadata type, see e.g. {@link #TYPE_BYTE}
     *
     * @hide
     */
    static CARAPI GetNativeType(
        /* [in] */ Int32 tag,
        /* [out] */ Int32* type);

    /**
     * <p>Updates the existing entry for tag with the new bytes pointed by src, erasing
     * the entry if src was null.</p>
     *
     * <p>An empty array can be passed in to update the entry to 0 elements.</p>
     *
     * @param tag An integer tag, see e.g. {@link #getTag}
     * @param src An array of bytes, or null to erase the entry
     *
     * @hide
     */
    CARAPI WriteValues(
        /* [in] */ Int32 tag,
        /* [in] */ ArrayOf<Byte>* src);

    /**
     * <p>Returns a byte[] of data corresponding to this tag. Use a wrapped bytebuffer to unserialize
     * the data properly.</p>
     *
     * <p>An empty array can be returned to denote an existing entry with 0 elements.</p>
     *
     * @param tag An integer tag, see e.g. {@link #getTag}
     *
     * @return {@code null} if there were 0 entries for this tag, a byte[] otherwise.
     * @hide
     */
    CARAPI ReadValues(
        /* [in] */ Int32 tag,
        /* [out, callee] */ ArrayOf<Byte>** values);

    /**
     * Dumps the native metadata contents to logcat.
     *
     * <p>Visibility for testing/debugging only. The results will not
     * include any synthesized keys, as they are invisible to the native layer.</p>
     *
     * @hide
     */
    CARAPI DumpToLog();

private:
    static CARAPI_(Boolean) InitStaticBlock();

    static CARAPI_(String) TranslateLocationProviderToProcess(
        /* [in] */ const String& provider);

    static CARAPI_(String) TranslateProcessToLocationProvider(
        /* [in] */ const String& process);

    CARAPI_(void) Close();

    CARAPI_(AutoPtr<IInterface>) GetBase(
        /* [in] */ ICameraCharacteristicsKey* key);

    CARAPI_(AutoPtr<IInterface>) GetBase(
        /* [in] */ ICaptureResultKey* key);

    CARAPI_(AutoPtr<IInterface>) GetBase(
        /* [in] */ ICaptureRequestKey* key);

    CARAPI_(AutoPtr<IInterface>) GetBase(
        /* [in] */ ICameraMetadataNativeKey* key);

    CARAPI_(AutoPtr<ArrayOf<Int32> >) GetAvailableFormats();

    CARAPI_(Boolean) SetFaces(
        /* [in] */ ArrayOf<IFace*>* faces);

    CARAPI_(AutoPtr<ArrayOf<IFace*> >) GetFaces();

    // Face rectangles are defined as (left, top, right, bottom) instead of
    // (left, top, width, height) at the native level, so the normal Rect
    // conversion that does (l, t, w, h) -> (l, t, r, b) is unnecessary. Undo
    // that conversion here for just the faces.
    CARAPI_(AutoPtr<ArrayOf<IRect*> >) GetFaceRectangles();

    CARAPI_(AutoPtr<ILensShadingMap>) GetLensShadingMap();

    CARAPI_(AutoPtr<ILocation>) GetGpsLocation();

    CARAPI_(Boolean) SetGpsLocation(
        /* [in] */ ILocation* l);

    CARAPI_(AutoPtr<IStreamConfigurationMap>) GetStreamConfigurationMap();

    CARAPI GetMaxRegions(
        /* [in] */ ICameraMetadataNativeKey* key,
        /* [out] */ Int32* regions);

    CARAPI GetMaxNumOutputs(
        /* [in] */ ICameraMetadataNativeKey* key,
        /* [out] */ Int32* num);

    CARAPI_(AutoPtr<ITonemapCurve>) GetTonemapCurve();

    CARAPI SetBase(
        /* [in] */ ICameraCharacteristicsKey* key,
        /* [in] */ IInterface* value);

    CARAPI SetBase(
        /* [in] */ ICaptureResultKey* key,
        /* [in] */ IInterface* value);

    CARAPI SetBase(
        /* [in] */ ICaptureRequestKey* key,
        /* [in] */ IInterface* value);

    CARAPI SetBase(
        /* [in] */ ICameraMetadataNativeKey* key,
        /* [in] */ IInterface* value);

    CARAPI_(Boolean) SetAvailableFormats(
        /* [in] */ ArrayOf<Int32>* value);

    /**
     * Convert Face Rectangles from managed side to native side as they have different definitions.
     * <p>
     * Managed side face rectangles are defined as: left, top, width, height.
     * Native side face rectangles are defined as: left, top, right, bottom.
     * The input face rectangle need to be converted to native side definition when set is called.
     * </p>
     *
     * @param faceRects Input face rectangles.
     * @return true if face rectangles can be set successfully. Otherwise, Let the caller
     *             (setBase) to handle it appropriately.
     */
    CARAPI_(Boolean) SetFaceRectangles(
        /* [in] */ ArrayOf<IRect*>* faceRects);

    CARAPI_(Boolean) SetTonemapCurve(
        /* [in] */ ITonemapCurve* tc);

    // Less safe access to native pointer. Does NOT throw any Java exceptions if NULL.
    CARAPI_(android::CameraMetadata*) CameraMetadata_getPointerNoThrow(
        /* [in] */ ICameraMetadataNative* thiz);

    // Safe access to native pointer from object. Throws if not possible to access.
    CARAPI CameraMetadata_getPointerThrow(
        /* [in] */ ICameraMetadataNative* thiz,
        /* [out] */ android::CameraMetadata** data,
        /* [in] */ const char* argName = "this");

    CARAPI_(Int64) NativeAllocate();

    CARAPI NativeAllocateCopy(
        /* [in] */ ICameraMetadataNative* other,
        /* [out] */ Int64* handle);

    CARAPI NativeWriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI NativeReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI NativeSwap(
        /* [in] */ ICameraMetadataNative* other);

    CARAPI_(void) NativeClose();

    CARAPI NativeIsEmpty(
        /* [out] */ Boolean* result);

    CARAPI NativeGetEntryCount(
        /* [out] */ Int32* count);

    CARAPI NativeReadValues(
        /* [in] */ Int32 tag,
        /* [out] */ ArrayOf<Byte>** values);

    CARAPI NativeWriteValues(
        /* [in] */ Int32 tag,
        /* [in] */ ArrayOf<Byte>* src);

    CARAPI NativeDump(); // dump to ALOGD

    static CARAPI NativeGetTagFromKey(
        /* [in] */ const String& keyName,
        /* [out] */ Int32* tag);

    static CARAPI NativeGetTypeFromTag(
        /* [in] */ Int32 tag,
        /* [out] */ Int32* type);

    static CARAPI_(void) NativeClassInit();

    /**
     * Get the marshaler compatible with the {@code key} and type {@code T}.
     *
     * @throws UnsupportedOperationException
     *          if the native/managed type combination for {@code key} is not supported
     */
    static CARAPI_(AutoPtr<IMarshaler>) GetMarshalerForKey(
        /* [in] */ ICameraMetadataNativeKey* key);

    //@SuppressWarnings({ "unchecked", "rawtypes" })
    static CARAPI_(void) RegisterAllMarshalers();

    /** Check if input arguments are all {@code null}.
     *
     * @param objs Input arguments for null check
     * @return {@code true} if input arguments are all {@code null}, otherwise {@code false}
     */
    static CARAPI_(Boolean) AreValuesAllNull(
        /* [in] */ ArrayOf<IInterface*>* objs);

private:
    static const String TAG;
    static const Boolean VERBOSE;

    static const String CELLID_PROCESS;
    static const String GPS_PROCESS;
    static const Int32 FACE_LANDMARK_SIZE;

    static Boolean initStaticBlock;

    // Use Command pattern here to avoid lots of expensive if/equals checks in get for overridden
    // metadata.
    static AutoPtr<IHashMap> sGetCommandMap;

    // Use Command pattern here to avoid lots of expensive if/equals checks in get for overridden
    // metadata.
    static AutoPtr<IHashMap> sSetCommandMap;

    Int64 mMetadataPtr; // native CameraMetadata*
};

} // namespace Impl
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_IMPL_CAMERAMETADATANATIVE_H__
