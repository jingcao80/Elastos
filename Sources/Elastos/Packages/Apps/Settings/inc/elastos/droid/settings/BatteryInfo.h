/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.settings;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IBatteryManager;
using Elastos::Droid::Os::IBatteryStats;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IIPowerManager;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Internal::App::IIBatteryStats;

public class BatteryInfo extends Activity {
    private TextView mStatus;
    private TextView mPower;
    private TextView mLevel;
    private TextView mScale;
    private TextView mHealth;
    private TextView mVoltage;
    private TextView mTemperature;
    private TextView mTechnology;
    private TextView mUptime;
    private IBatteryStats mBatteryStats;
    private IPowerManager mScreenStats;
    
    private static const Int32 EVENT_TICK = 1;
    
    private Handler mHandler = new Handler() {
        //@Override
        CARAPI HandleMessage(Message msg) {
            switch (msg.what) {
                case EVENT_TICK:
                    UpdateBatteryStats();
                    SendEmptyMessageDelayed(EVENT_TICK, 1000);
                    
                    break;
            }
        }
    };

    /**
     * Format a number of tenths-units as a decimal string without using a
     * conversion to Float.  E.g. 347 -> "34.7", -99 -> "-9.9"
     */
    private final String TenthsToFixedString(Int32 x) {
        Int32 tens = x / 10;
        // use Math.abs to avoid "-9.-9" about -99
        return Integer->ToString(tens) + "." + Math->Abs(x - 10 * tens);
    }

   /**
    *Listens for intent broadcasts
    */
    private IntentFilter   mIntentFilter;

    private BroadcastReceiver mIntentReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            String action = intent->GetAction();
            if (action->Equals(IIntent::ACTION_BATTERY_CHANGED)) {
                Int32 plugType = intent->GetIntExtra("plugged", 0);

                mLevel->SetText("" + intent->GetIntExtra("level", 0));
                mScale->SetText("" + intent->GetIntExtra("scale", 0));
                mVoltage->SetText("" + intent->GetIntExtra("voltage", 0) + " "
                        + GetString(R::string::battery_info_voltage_units));
                mTemperature->SetText("" + TenthsToFixedString(intent->GetIntExtra("temperature", 0))
                        + GetString(R::string::battery_info_temperature_units));
                mTechnology->SetText("" + intent->GetStringExtra("technology"));
                
                mStatus->SetText(Utils->GetBatteryStatus(GetResources(), intent));

                switch (plugType) {
                    case 0:
                        mPower->SetText(GetString(R::string::battery_info_power_unplugged));
                        break;
                    case BatteryManager.BATTERY_PLUGGED_AC:
                        mPower->SetText(GetString(R::string::battery_info_power_ac));
                        break;
                    case BatteryManager.BATTERY_PLUGGED_USB:
                        mPower->SetText(GetString(R::string::battery_info_power_usb));
                        break;
                    case BatteryManager.BATTERY_PLUGGED_WIRELESS:
                        mPower->SetText(GetString(R::string::battery_info_power_wireless));
                        break;
                    case (BatteryManager.BATTERY_PLUGGED_AC|BatteryManager.BATTERY_PLUGGED_USB):
                        mPower->SetText(GetString(R::string::battery_info_power_ac_usb));
                        break;
                    default:
                        mPower->SetText(GetString(R::string::battery_info_power_unknown));
                        break;
                }
                
                Int32 health = intent->GetIntExtra("health", BatteryManager.BATTERY_HEALTH_UNKNOWN);
                String healthString;
                if (health == BatteryManager.BATTERY_HEALTH_GOOD) {
                    healthString = GetString(R::string::battery_info_health_good);
                } else if (health == BatteryManager.BATTERY_HEALTH_OVERHEAT) {
                    healthString = GetString(R::string::battery_info_health_overheat);
                } else if (health == BatteryManager.BATTERY_HEALTH_DEAD) {
                    healthString = GetString(R::string::battery_info_health_dead);
                } else if (health == BatteryManager.BATTERY_HEALTH_OVER_VOLTAGE) {
                    healthString = GetString(R::string::battery_info_health_over_voltage);
                } else if (health == BatteryManager.BATTERY_HEALTH_UNSPECIFIED_FAILURE) {
                    healthString = GetString(R::string::battery_info_health_unspecified_failure);
                } else if (health == BatteryManager.BATTERY_HEALTH_COLD) {
                    healthString = GetString(R::string::battery_info_health_cold);
                } else {
                    healthString = GetString(R::string::battery_info_health_unknown);
                }
                mHealth->SetText(healthString);
            }
        }
    };

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        SetContentView(R.layout.battery_info);

        // create the IntentFilter that will be used to listen
        // to battery status broadcasts
        mIntentFilter = new IntentFilter();
        mIntentFilter->AddAction(IIntent::ACTION_BATTERY_CHANGED);
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();

        mStatus = (TextView)FindViewById(R.id.status);
        mPower = (TextView)FindViewById(R.id.power);
        mLevel = (TextView)FindViewById(R.id.level);
        mScale = (TextView)FindViewById(R.id.scale);
        mHealth = (TextView)FindViewById(R.id.health);
        mTechnology = (TextView)FindViewById(R.id.technology);
        mVoltage = (TextView)FindViewById(R.id.voltage);
        mTemperature = (TextView)FindViewById(R.id.temperature);
        mUptime = (TextView) FindViewById(R.id.uptime);
        
        // Get awake time plugged in and on battery
        mBatteryStats = IBatteryStats.Stub->AsInterface(ServiceManager->GetService(
                BatteryStats.SERVICE_NAME));
        mScreenStats = IPowerManager.Stub->AsInterface(ServiceManager->GetService(POWER_SERVICE));
        mHandler->SendEmptyMessageDelayed(EVENT_TICK, 1000);
        
        RegisterReceiver(mIntentReceiver, mIntentFilter);
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        mHandler->RemoveMessages(EVENT_TICK);
        
        // we are no longer on the screen stop the observers
        UnregisterReceiver(mIntentReceiver);
    }

    private void UpdateBatteryStats() {
        Int64 uptime = SystemClock->ElapsedRealtime();
        mUptime->SetText(DateUtils->FormatElapsedTime(uptime / 1000));
    }
    
}
