#include "elastos/droid/bluetooth/CBluetoothAdapter.h"
#include "elastos/droid/hardware/camera2/CameraCharacteristics.h"
#include "elastos/droid/internal/utility/cm/CQSConstants.h"
#include "elastos/droid/internal/utility/cm/QSUtils.h"
#include "elastos/droid/R.h"
#include "elastos/droid/text/TextUtils.h"

using Elastos::Droid::Bluetooth::CBluetoothAdapter;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Hardware::Camera2::CameraCharacteristics;
using Elastos::Droid::Hardware::Camera2::ICameraCharacteristics;
using Elastos::Droid::Hardware::Camera2::ICameraManager;
using Elastos::Droid::Hardware::Camera2::ICameraMetadata;
using Elastos::Droid::Hardware::ISensor;
using Elastos::Droid::Hardware::ISensorManager;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::R;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CString;
using Elastos::Core::IBoolean;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IIterable;
using Elastos::Utility::IIterator;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

Boolean QSUtils::sAvailableTilesFiltered;

ECode QSUtils::GetAvailableTiles(
    /* [in] */ IContext* context,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    FilterTiles(context);
    *result = IList::Probe(CQSConstants::TILES_AVAILABLE);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode QSUtils::GetDefaultTiles(
    /* [in] */ IContext* context,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IList> tiles;
    CArrayList::New((IList**)&tiles);
    String defaults;
    context->GetString(R::string::config_defaultQuickSettingsTiles, &defaults);
    if (!defaults.IsEmpty()) {
        AutoPtr<IPatternHelper> helper;
        CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
        String splitter;
        helper->Quote(String(","), &splitter);
        AutoPtr<ArrayOf<String> > array = TextUtils::Split(defaults, splitter);

        for (Int32 i = 0; i < array->GetLength(); i++) {
            String item = (*array)[i];
            if (item.IsEmpty()) {
                continue;
            }
            AutoPtr<ICharSequence> cs;
            CString::New(item, (ICharSequence**)&cs);
            tiles->Add(cs);
        }
        FilterTiles(context, tiles);
    }

    *result = tiles;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode QSUtils::GetDefaultTilesAsString(
    /* [in] */ IContext* context,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IList> list;
    GetDefaultTiles(context, (IList**)&list);
    AutoPtr<ICharSequence> cs;
    CString::New(String(","), (ICharSequence**)&cs);
    *result = TextUtils::Join(cs, IIterable::Probe(list));
    return NOERROR;
}

ECode QSUtils::DeviceSupportsLte(
    /* [in] */ IContext* ctx,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> service;
    ctx->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&service);
    AutoPtr<ITelephonyManager> tm = ITelephonyManager::Probe(service);

    Int32 cdmaMode, gsmMode;
    tm->GetLteOnCdmaMode(&cdmaMode);
// TODO: Need ITelephonyManager::GetLteOnGsmMode
    // tm->GetLteOnGsmMode(&gsmMode);
    assert(0 && "TODO");
    *result = (cdmaMode == IPhoneConstants::LTE_ON_CDMA_TRUE) || gsmMode != 0;
    return NOERROR;
}

ECode QSUtils::DeviceSupportsDdsSupported(
    /* [in] */ IContext* ctx,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> service;
    ctx->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&service);
    AutoPtr<ITelephonyManager> tm = ITelephonyManager::Probe(service);

    Boolean b;
    tm->IsMultiSimEnabled(&b);
    Elastos::Droid::Telephony::MultiSimVariants v;
    tm->GetMultiSimConfiguration(&v);
    *result = b && v == Elastos::Droid::Telephony::MultiSimVariants_DSDA;
    return NOERROR;
}

ECode QSUtils::DeviceSupportsMobileData(
    /* [in] */ IContext* ctx,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> service;
    ctx->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&service);
    AutoPtr<IConnectivityManager> cm = IConnectivityManager::Probe(service);
    return cm->IsNetworkSupported(IConnectivityManager::TYPE_MOBILE, result);
}

ECode QSUtils::DeviceSupportsBluetooth(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = CBluetoothAdapter::GetDefaultAdapter() != NULL;
    return NOERROR;
}

ECode QSUtils::DeviceSupportsNfc(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPackageManager> packageManager;
    context->GetPackageManager((IPackageManager**)&packageManager);
    return packageManager->HasSystemFeature(IPackageManager::FEATURE_NFC, result);
}

ECode QSUtils::DeviceSupportsFlashLight(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::CAMERA_SERVICE, (IInterface**)&service);
    AutoPtr<ICameraManager> cameraManager = ICameraManager::Probe(service);

    // try {
    AutoPtr<ArrayOf<String> > ids;
    cameraManager->GetCameraIdList((ArrayOf<String>**)&ids);

    for (Int32 i = 0; i < ids->GetLength(); i++) {
        String id = (*ids)[i];
        AutoPtr<ICameraCharacteristics> c;
        cameraManager->GetCameraCharacteristics(id, (ICameraCharacteristics**)&c);
        AutoPtr<IInterface> ib;
        c->Get(CameraCharacteristics::FLASH_INFO_AVAILABLE, (IInterface**)&ib);
        Boolean flashAvailable;
        IBoolean::Probe(ib)->GetValue(&flashAvailable);
        AutoPtr<IInterface> obj;
        c->Get(CameraCharacteristics::LENS_FACING, (IInterface**)&obj);
        AutoPtr<IInteger32> lensFacing = IInteger32::Probe(obj);
        if (flashAvailable != NULL
                && flashAvailable
                && lensFacing != NULL) {
            Int32 value;
            lensFacing->GetValue(&value);
            if (value == ICameraMetadata::LENS_FACING_BACK) {
                *result = TRUE;
                return NOERROR;
            }
        }
    }
    // } catch (CameraAccessException e) {
    //     // Ignore
    // }
    *result = FALSE;
    return NOERROR;
}

ECode QSUtils::DeviceSupportsCompass(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::SENSOR_SERVICE, (IInterface**)&service);
    AutoPtr<ISensorManager> sm = ISensorManager::Probe(service);

    AutoPtr<ISensor> sensor1, sensor2;
    sm->GetDefaultSensor(ISensor::TYPE_ACCELEROMETER, (ISensor**)&sensor1);
    sm->GetDefaultSensor(ISensor::TYPE_MAGNETIC_FIELD, (ISensor**)&sensor1);
    *result = sensor1 != NULL && sensor2 != NULL;
    return NOERROR;
}

void QSUtils::FilterTiles(
    /* [in] */ IContext* context,
    /* [in] */ IList* tiles)
{
    Boolean deviceSupportsMobile;
    DeviceSupportsMobileData(context, &deviceSupportsMobile);

    // Tiles that need conditional filtering
    AutoPtr<IIterator> iterator;
    tiles->GetIterator((IIterator**)&iterator);
    Boolean b;
    while (iterator->HasNext(&b), b) {
        AutoPtr<IInterface> obj;
        iterator->GetNext((IInterface**)&obj);
        String tileKey;
        ICharSequence::Probe(obj)->ToString(&tileKey);
        Boolean removeTile = FALSE;

        if (tileKey.Equals(IQSConstants::TILE_CELLULAR) ||
                tileKey.Equals(IQSConstants::TILE_HOTSPOT) ||
                tileKey.Equals(IQSConstants::TILE_DATA) ||
                tileKey.Equals(IQSConstants::TILE_ROAMING) ||
                tileKey.Equals(IQSConstants::TILE_APN)) {
            removeTile = !deviceSupportsMobile;
        }
        else if (tileKey.Equals(IQSConstants::TILE_DDS)) {
            DeviceSupportsDdsSupported(context, &removeTile);
            removeTile = !removeTile;
        }
        else if (tileKey.Equals(IQSConstants::TILE_FLASHLIGHT)) {
            DeviceSupportsFlashLight(context, &removeTile);
            removeTile = !removeTile;
        }
        else if (tileKey.Equals(IQSConstants::TILE_BLUETOOTH)) {
            DeviceSupportsBluetooth(&removeTile);
            removeTile = !removeTile;
        }
        else if (tileKey.Equals(IQSConstants::TILE_NFC)) {
            DeviceSupportsNfc(context, &removeTile);
            removeTile = !removeTile;
        }
        else if (tileKey.Equals(IQSConstants::TILE_COMPASS)) {
            DeviceSupportsCompass(context, &removeTile);
            removeTile = !removeTile;
        }

        if (removeTile) {
            iterator->Remove();
            AutoPtr<ICharSequence> cs;
            CString::New(tileKey, (ICharSequence**)&cs);
            tiles->Remove(cs);
        }
    }
}

void QSUtils::FilterTiles(
    /* [in] */ IContext* context)
{
    if (!sAvailableTilesFiltered) {
        FilterTiles(context, IList::Probe(CQSConstants::TILES_DEFAULT));
        sAvailableTilesFiltered = TRUE;
    }
}

} //namespace Cm
} //namespace Utility
} //namespace Internal
} //namespace Droid
} //namespace Elastos
