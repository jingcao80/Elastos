
#include "CGeckoBatteryManager.h"
#include "elastos/droid/ext/frameworkext.h"
#include "stdio.h"
#include "GeckoAppShell.h"
#include <elastos/core/Math.h>

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Launcher {

#define kDefaultLevel         1.0
#define kDefaultCharging      TRUE
#define kDefaultRemainingTime -1.0
#define kUnknownRemainingTime -1.0

IDate* InitLastLevelChange()
{
    AutoPtr<IDate> date;
    //ASSERT_SUCCEEDED(CDate::New((IDate**)&date));
    return date;
}

AutoPtr<IDate>  CGeckoBatteryManager::sLastLevelChange = InitLastLevelChange();
Boolean CGeckoBatteryManager::sNotificationsEnabled = FALSE;
Double CGeckoBatteryManager::sLevel = kDefaultLevel;
Boolean CGeckoBatteryManager::sCharging = kDefaultCharging;
Double CGeckoBatteryManager::sRemainingTime = kDefaultRemainingTime;



ECode CGeckoBatteryManager::OnRecieve(
    /* [in] */ IContext *pContext,
    /* [in] */ IIntent *pIntent)
{
    String action;
    pIntent->GetAction(&action);
    if (!action.Equals(IIntent::ACTION_BATTERY_CHANGED)) {
        printf("GeckoBatteryManager" "Got an unexpected intent!");
        return NOERROR;
    }

    Boolean previousCharging = IsCharging();
    Double previousLevel = GetLevel();

    Boolean booleanExtra;
    pIntent->GetBooleanExtra(
                String("present")/*BatteryManager_EXTRA_PRESENT*/,
                false, &booleanExtra);
    if (booleanExtra) {
        Int32 plugged;
        pIntent->GetInt32Extra(String("plugged")/*BatteryManager_EXTRA_PLUGGED*/, -1, &plugged);

        if (plugged == -1) {
            sCharging = kDefaultCharging;
            printf("GeckoBatteryManager" "Failed to get the plugged status!");
        }
        else {
            // Likely, if plugged > 0, it's likely plugged and charging but the doc
            // isn't clear about that.
            sCharging = plugged != 0;
        }

        if (sCharging != previousCharging) {
            sRemainingTime = kUnknownRemainingTime;
            // The new remaining time is going to take some time to show up but
            // it's the best way to show a not too wrong value.
            ASSERT_SUCCEEDED(CDate::New(0, (IDate**)&sLastLevelChange));
        }

        // We need two doubles because sLevel is a double.
        Double current;
        Double max;
        pIntent->GetDoubleExtra(String("level")/*BatteryManager_EXTRA_LEVEL*/, -1, &current);
        pIntent->GetDoubleExtra(String("scale")/*BatteryManager_EXTRA_SCALE*/, -1, &max);

        if (current == -1 || max == -1) {
            printf("GeckoBatteryManager" "Failed to get battery level!");
            sLevel = kDefaultLevel;
        }
        else {
            sLevel = current / max;
        }

        if (sLevel == 1.0 && sCharging) {
            sRemainingTime = 0.0;
        }
        else if (sLevel != previousLevel) {
            // Estimate remaining time.
            Int64 changeTime;
            sLastLevelChange->GetTime(&changeTime);
            if (changeTime != 0) {
                AutoPtr<IDate> currentTime;
                ASSERT_SUCCEEDED(CDate::New((IDate**)&currentTime));

                Int64 curTime;
                currentTime->GetTime(&curTime);
                Int64 dt = (curTime - changeTime) / 1000;
                Double dLevel = sLevel - previousLevel;

                if (sCharging) {
                    if (dLevel < 0) {
                        printf("GeckoBatteryManager"  "When charging, level should increase!");
                        sRemainingTime = kUnknownRemainingTime;
                    }
                    else {
                        sRemainingTime = Math::Round(dt / dLevel * (1.0 - sLevel));
                    }
                }
                else {
                    if (dLevel > 0) {
                        printf("GeckoBatteryManager" "When discharging, level should decrease!");
                        sRemainingTime = kUnknownRemainingTime;
                    }
                    else {
                        sRemainingTime = Math::Round(dt / -dLevel * sLevel);
                    }
                }

                sLastLevelChange = currentTime;
            }
            else {
                // That's the first time we got an update, we can't do anything.
                CDate::New((IDate**)&sLastLevelChange);
            }
        }
    }
    else {
        sLevel = kDefaultLevel;
        sCharging = kDefaultCharging;
        sRemainingTime = kDefaultRemainingTime;
    }

    /*
     * We want to inform listeners if the following conditions are fulfilled:
     *  - we have at least one observer;
     *  - the charging state or the level has changed.
     *
     * Note: no need to check for a remaining time change given that it's only
     * updated if there is a level change or a charging change.
     *
     * The idea is to prevent doing all the way to the DOM code in the child
     * process to finally not send an event.
     */
    if (sNotificationsEnabled &&
        (previousCharging != IsCharging() || previousLevel != GetLevel())) {
        GeckoAppShell::NotifyBatteryChange(GetLevel(), IsCharging(), GetRemainingTime());
    }
    return NOERROR;
}

Boolean CGeckoBatteryManager::IsCharging()
{
    return sCharging;
}

Double CGeckoBatteryManager::GetLevel()
{
    return sLevel;
}

Double CGeckoBatteryManager::GetRemainingTime()
{
    return sRemainingTime;
}

Void CGeckoBatteryManager::EnableNotifications()
{
    sNotificationsEnabled = TRUE;
}

Void CGeckoBatteryManager::DisableNotifications()
{
    sNotificationsEnabled = FALSE;
}

AutoPtr<ArrayOf<Double> > CGeckoBatteryManager::GetCurrentInformation()
{
    AutoPtr<ArrayOf<Double> > info = ArrayOf<Double>::Alloc(3);
    (*info)[0] = GetLevel();
    (*info)[1] = IsCharging() ? 1.0 : 0.0;
    (*info)[2] = GetRemainingTime();
    return info;
}

} // namespace Launcher
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
