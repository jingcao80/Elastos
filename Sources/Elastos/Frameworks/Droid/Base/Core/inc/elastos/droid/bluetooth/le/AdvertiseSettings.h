#ifndef __ELASTOS_DROID_BLUETOOTH_LE_ADVERTISESETTINGS_H__
#define __ELASTOS_DROID_BLUETOOTH_LE_ADVERTISESETTINGS_H__

#include "Elastos.Droid.Bluetooth.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

/**
  * The {@link AdvertiseSettings} provide a way to adjust advertising preferences for each
  * Bluetooth LE advertisement instance. Use {@link AdvertiseSettings.Builder} to create an
  * instance of this class.
  */
class AdvertiseSettings
    : public Object
    , public IAdvertiseSettings
    , public IParcelable
{
public:
    /**
      * Builder class for {@link AdvertiseSettings}.
      */
    class Builder
        : public Object
        , public IAdvertiseSettingsBuilder
    {
    public:
        CAR_INTERFACE_DECL();

        Builder();

        /**
          * Set advertise mode to control the advertising power and latency.
          *
          * @param advertiseMode Bluetooth LE Advertising mode, can only be one of
          *            {@link AdvertiseSettings#ADVERTISE_MODE_LOW_POWER},
          *            {@link AdvertiseSettings#ADVERTISE_MODE_BALANCED}, or
          *            {@link AdvertiseSettings#ADVERTISE_MODE_LOW_LATENCY}.
          * @throws IllegalArgumentException If the advertiseMode is invalid.
          */
        CARAPI SetAdvertiseMode(
            /* [in] */ Int32 advertiseMode);

        /**
          * Set advertise TX power level to control the transmission power level for the advertising.
          *
          * @param txPowerLevel Transmission power of Bluetooth LE Advertising, can only be one of
          *            {@link AdvertiseSettings#ADVERTISE_TX_POWER_ULTRA_LOW},
          *            {@link AdvertiseSettings#ADVERTISE_TX_POWER_LOW},
          *            {@link AdvertiseSettings#ADVERTISE_TX_POWER_MEDIUM} or
          *            {@link AdvertiseSettings#ADVERTISE_TX_POWER_HIGH}.
          * @throws IllegalArgumentException If the {@code txPowerLevel} is invalid.
          */
        CARAPI SetTxPowerLevel(
            /* [in] */ Int32 txPowerLevel);

        /**
          * Set whether the advertisement type should be connectable or non-connectable.
          *
          * @param connectable Controls whether the advertisment type will be connectable (true)
          *                    or non-connectable (false).
          */
        CARAPI SetConnectable(
            /* [in] */ Boolean connectable);

        /**
          * Limit advertising to a given amount of time.
          * @param timeoutMillis Advertising time limit. May not exceed 180000 milliseconds.
          *                       A value of 0 will disable the time limit.
          * @throws IllegalArgumentException If the provided timeout is over 180000 ms.
          */
        CARAPI SetTimeout(
            /* [in] */ Int32 timeoutMillis);

        /**
          * Build the {@link AdvertiseSettings} object.
          */
        CARAPI Build(
            /* [out] */ IAdvertiseSettings** result);

    private:
        Int32 mMode;
        Int32 mTxPowerLevel;
        Int32 mTimeoutMillis;
        Boolean mConnectable;
    };

public:
    CAR_INTERFACE_DECL();

    AdvertiseSettings();

    /**
      * Returns the advertise mode.
      */
    CARAPI GetMode(
        /* [out] */ Int32* result);

    /**
      * Returns the TX power level for advertising.
      */
    CARAPI GetTxPowerLevel(
        /* [out] */ Int32* result);

    /**
      * Returns whether the advertisement will indicate connectable.
      */
    CARAPI IsConnectable(
        /* [out] */ Boolean* result);

    /**
      * Returns the advertising time limit in milliseconds.
      */
    CARAPI GetTimeout(
        /* [out] */ Int32* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* info);

    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI constructor(
        /* [in] */ Int32 advertiseMode,
        /* [in] */ Int32 advertiseTxPowerLevel,
        /* [in] */ Boolean advertiseConnectable,
        /* [in] */ Int32 advertiseTimeout);

    CARAPI constructor(
        /* [in] */ IParcel* in);

private:
    /**
      * The maximum limited advertisement duration as specified by the Bluetooth SIG
      */
    static const Int32 LIMITED_ADVERTISING_MAX_MILLIS = 180 * 1000;
    Int32 mAdvertiseMode;
    Int32 mAdvertiseTxPowerLevel;
    Int32 mAdvertiseTimeoutMillis;
    Boolean mAdvertiseConnectable;
};

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_LE_ADVERTISESETTINGS_H__
