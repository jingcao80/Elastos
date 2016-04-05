/*
 * Copyright (C) 2013 The Android Open Source Project
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

package com.android.settings.location;

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Internal::Annotations::IImmutable;
using Elastos::Droid::Internal::Utility::IPreconditions;

/**
 * Specifies a setting that is being injected into Settings &gt; Location &gt; Location services.
 *
 * @see android.location.SettingInjectorService
 */
@Immutable
class InjectedSetting {

    /**
     * Package for the subclass of {@link android.location.SettingInjectorService} and for the
     * settings activity.
     */
    public final String packageName;

    /**
     * Class name for the subclass of {@link android.location.SettingInjectorService} that
     * specifies dynamic values for the location setting.
     */
    public final String className;

    /**
     * The {@link android.preference.Preference#GetTitle()} value.
     */
    public final String title;

    /**
     * The {@link android.preference.Preference#GetIcon()} value.
     */
    public final Int32 iconId;

    /**
     * The activity to launch to allow the user to modify the settings value. Assumed to be in the
     * {@link #packageName} package.
     */
    public final String settingsActivity;

    private InjectedSetting(String packageName, String className,
            String title, Int32 iconId, String settingsActivity) {
        this.packageName = Preconditions->CheckNotNull(packageName, "packageName");
        this.className = Preconditions->CheckNotNull(className, "className");
        this.title = Preconditions->CheckNotNull(title, "title");
        this.iconId = iconId;
        this.settingsActivity = Preconditions->CheckNotNull(settingsActivity);
    }

    /**
     * Returns a new instance, or NULL.
     */
    public static InjectedSetting NewInstance(String packageName, String className,
            String title, Int32 iconId, String settingsActivity) {
        if (packageName == NULL || className == NULL ||
                TextUtils->IsEmpty(title) || TextUtils->IsEmpty(settingsActivity)) {
            if (Log->IsLoggable(SettingsInjector.TAG, Log.WARN)) {
                Logger::W(SettingsInjector.TAG, "Illegal setting specification: package="
                        + packageName + ", class=" + className
                        + ", title=" + title + ", settingsActivity=" + settingsActivity);
            }
            return NULL;
        }
        return new InjectedSetting(packageName, className, title, iconId, settingsActivity);
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return "InjectedSetting{" +
                "mPackageName='" + packageName + '\'' +
                ", mClassName='" + className + '\'' +
                ", label=" + title +
                ", iconId=" + iconId +
                ", settingsActivity='" + settingsActivity + '\'' +
                '}';
    }

    /**
     * Returns the intent to start the {@link #className} service.
     */
    public Intent GetServiceIntent() {
        Intent intent = new Intent();
        intent->SetClassName(packageName, className);
        return intent;
    }

    //@Override
    public Boolean Equals(Object o) {
        if (this == o) return TRUE;
        if (!(o instanceof InjectedSetting)) return FALSE;

        InjectedSetting that = (InjectedSetting) o;

        return packageName->Equals(that.packageName) && className->Equals(that.className)
                && title->Equals(that.title) && iconId == that.iconId
                && settingsActivity->Equals(that.settingsActivity);
    }

    //@Override
    public Int32 HashCode() {
        Int32 result = packageName->HashCode();
        result = 31 * result + className->HashCode();
        result = 31 * result + title->HashCode();
        result = 31 * result + iconId;
        result = 31 * result + settingsActivity->HashCode();
        return result;
    }
}
