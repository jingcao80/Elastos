/*
 * Copyright (C) 2012 The Android Open Source Project
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

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Service::Dreams::IDreamService;
using Elastos::Droid::Service::Dreams::IIDreamManager;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::IXml;

using Org::Xmlpull::V1::IXmlPullParser;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IComparator;
using Elastos::Utility::IList;

public class DreamBackend {
    private static const String TAG = DreamSettings.class->GetSimpleName() + ".Backend";

    public static class DreamInfo {
        CharSequence caption;
        Drawable icon;
        Boolean isActive;
        public ComponentName componentName;
        public ComponentName settingsComponentName;

        //@Override
        CARAPI ToString(
        /* [out] */ String* str)
    {
            StringBuilder sb = new StringBuilder(DreamInfo.class->GetSimpleName());
            sb->AppendChar32('[').Append(caption);
            if (isActive)
                sb->Append(",active");
            sb->AppendChar32(',').Append(componentName);
            if (settingsComponentName != NULL)
                sb->Append("settings=").Append(settingsComponentName);
            return sb->AppendChar32(']').ToString();
        }
    }

    private final Context mContext;
    private final IDreamManager mDreamManager;
    private final DreamInfoComparator mComparator;
    private final Boolean mDreamsEnabledByDefault;
    private final Boolean mDreamsActivatedOnSleepByDefault;
    private final Boolean mDreamsActivatedOnDockByDefault;

    public DreamBackend(Context context) {
        mContext = context;
        mDreamManager = IDreamManager.Stub->AsInterface(
                ServiceManager->GetService(DreamService.DREAM_SERVICE));
        mComparator = new DreamInfoComparator(GetDefaultDream());
        mDreamsEnabledByDefault = context->GetResources()
                .GetBoolean(R.bool.config_dreamsEnabledByDefault);
        mDreamsActivatedOnSleepByDefault = context->GetResources()
                .GetBoolean(R.bool.config_dreamsActivatedOnSleepByDefault);
        mDreamsActivatedOnDockByDefault = context->GetResources()
                .GetBoolean(R.bool.config_dreamsActivatedOnDockByDefault);
    }

    public List<DreamInfo> GetDreamInfos() {
        Logd("GetDreamInfos()");
        ComponentName activeDream = GetActiveDream();
        PackageManager pm = mContext->GetPackageManager();
        Intent dreamIntent = new Intent(DreamService.SERVICE_INTERFACE);
        List<ResolveInfo> resolveInfos = pm->QueryIntentServices(dreamIntent,
                PackageManager.GET_META_DATA);
        List<DreamInfo> dreamInfos = new ArrayList<DreamInfo>(resolveInfos->Size());
        for (ResolveInfo resolveInfo : resolveInfos) {
            if (resolveInfo.serviceInfo == NULL)
                continue;
            DreamInfo dreamInfo = new DreamInfo();
            dreamInfo.caption = resolveInfo->LoadLabel(pm);
            dreamInfo.icon = resolveInfo->LoadIcon(pm);
            dreamInfo.componentName = GetDreamComponentName(resolveInfo);
            dreamInfo.isActive = dreamInfo.componentName->Equals(activeDream);
            dreamInfo.settingsComponentName = GetSettingsComponentName(pm, resolveInfo);
            dreamInfos->Add(dreamInfo);
        }
        Collections->Sort(dreamInfos, mComparator);
        return dreamInfos;
    }

    public ComponentName GetDefaultDream() {
        if (mDreamManager == NULL)
            return NULL;
        try {
            return mDreamManager->GetDefaultDreamComponent();
        } catch (RemoteException e) {
            Logger::W(TAG, "Failed to get default dream", e);
            return NULL;
        }
    }

    public CharSequence GetActiveDreamName() {
        ComponentName cn = GetActiveDream();
        if (cn != NULL) {
            PackageManager pm = mContext->GetPackageManager();
            try {
                ServiceInfo ri = pm->GetServiceInfo(cn, 0);
                if (ri != NULL) {
                    return ri->LoadLabel(pm);
                }
            } catch (PackageManager.NameNotFoundException exc) {
                return NULL; // uninstalled?
            }
        }
        return NULL;
    }

    public Boolean IsEnabled() {
        return GetBoolean(ISettingsSecure::SCREENSAVER_ENABLED, mDreamsEnabledByDefault);
    }

    CARAPI SetEnabled(Boolean value) {
        Logd("SetEnabled(%s)", value);
        SetBoolean(ISettingsSecure::SCREENSAVER_ENABLED, value);
    }

    public Boolean IsActivatedOnDock() {
        return GetBoolean(ISettingsSecure::SCREENSAVER_ACTIVATE_ON_DOCK, mDreamsActivatedOnDockByDefault);
    }

    CARAPI SetActivatedOnDock(Boolean value) {
        Logd("SetActivatedOnDock(%s)", value);
        SetBoolean(ISettingsSecure::SCREENSAVER_ACTIVATE_ON_DOCK, value);
    }

    public Boolean IsActivatedOnSleep() {
        return GetBoolean(ISettingsSecure::SCREENSAVER_ACTIVATE_ON_SLEEP, mDreamsActivatedOnSleepByDefault);
    }

    CARAPI SetActivatedOnSleep(Boolean value) {
        Logd("SetActivatedOnSleep(%s)", value);
        SetBoolean(ISettingsSecure::SCREENSAVER_ACTIVATE_ON_SLEEP, value);
    }

    private Boolean GetBoolean(String key, Boolean def) {
        return Settings.Secure->GetInt(mContext->GetContentResolver(), key, def ? 1 : 0) == 1;
    }

    private void SetBoolean(String key, Boolean value) {
        Settings.Secure->PutInt(mContext->GetContentResolver(), key, value ? 1 : 0);
    }

    CARAPI SetActiveDream(ComponentName dream) {
        Logd("SetActiveDream(%s)", dream);
        if (mDreamManager == NULL)
            return;
        try {
            ComponentName[] dreams = { dream };
            mDreamManager->SetDreamComponents(dream == NULL ? NULL : dreams);
        } catch (RemoteException e) {
            Logger::W(TAG, "Failed to set active dream to " + dream, e);
        }
    }

    public ComponentName GetActiveDream() {
        if (mDreamManager == NULL)
            return NULL;
        try {
            ComponentName[] dreams = mDreamManager->GetDreamComponents();
            return dreams != NULL && dreams.length > 0 ? dreams[0] : NULL;
        } catch (RemoteException e) {
            Logger::W(TAG, "Failed to get active dream", e);
            return NULL;
        }
    }

    CARAPI LaunchSettings(DreamInfo dreamInfo) {
        Logd("LaunchSettings(%s)", dreamInfo);
        if (dreamInfo == NULL || dreamInfo.settingsComponentName == NULL)
            return;
        mContext->StartActivity(new Intent()->SetComponent(dreamInfo.settingsComponentName));
    }

    CARAPI Preview(DreamInfo dreamInfo) {
        Logd("Preview(%s)", dreamInfo);
        if (mDreamManager == NULL || dreamInfo == NULL || dreamInfo.componentName == NULL)
            return;
        try {
            mDreamManager->TestDream(dreamInfo.componentName);
        } catch (RemoteException e) {
            Logger::W(TAG, "Failed to preview " + dreamInfo, e);
        }
    }

    CARAPI StartDreaming() {
        Logd("StartDreaming()");
        if (mDreamManager == NULL)
            return;
        try {
            mDreamManager->Dream();
        } catch (RemoteException e) {
            Logger::W(TAG, "Failed to dream", e);
        }
    }

    private static ComponentName GetDreamComponentName(ResolveInfo resolveInfo) {
        if (resolveInfo == NULL || resolveInfo.serviceInfo == NULL)
            return NULL;
        return new ComponentName(resolveInfo.serviceInfo.packageName, resolveInfo.serviceInfo.name);
    }

    private static ComponentName GetSettingsComponentName(PackageManager pm, ResolveInfo resolveInfo) {
        if (resolveInfo == NULL
                || resolveInfo.serviceInfo == NULL
                || resolveInfo.serviceInfo.metaData == NULL)
            return NULL;
        String cn = NULL;
        XmlResourceParser parser = NULL;
        Exception caughtException = NULL;
        try {
            parser = resolveInfo.serviceInfo->LoadXmlMetaData(pm, DreamService.DREAM_META_DATA);
            if (parser == NULL) {
                Logger::W(TAG, "No " + DreamService.DREAM_META_DATA + " meta-data");
                return NULL;
            }
            Resources res = pm->GetResourcesForApplication(resolveInfo.serviceInfo.applicationInfo);
            AttributeSet attrs = Xml->AsAttributeSet(parser);
            Int32 type;
            while ((type=parser->Next()) != XmlPullParser.END_DOCUMENT
                    && type != XmlPullParser.START_TAG) {
            }
            String nodeName = parser->GetName();
            if (!"dream".Equals(nodeName)) {
                Logger::W(TAG, "Meta-data does not start with dream tag");
                return NULL;
            }
            TypedArray sa = res->ObtainAttributes(attrs, R.styleable.Dream);
            cn = sa->GetString(R.styleable.Dream_settingsActivity);
            sa->Recycle();
        } catch (NameNotFoundException e) {
            caughtException = e;
        } catch (IOException e) {
            caughtException = e;
        } catch (XmlPullParserException e) {
            caughtException = e;
        } finally {
            if (parser != NULL) parser->Close();
        }
        if (caughtException != NULL) {
            Logger::W(TAG, "Error parsing : " + resolveInfo.serviceInfo.packageName, caughtException);
            return NULL;
        }
        if (cn != NULL && cn->IndexOf('/') < 0) {
            cn = resolveInfo.serviceInfo.packageName + "/" + cn;
        }
        return cn == NULL ? NULL : ComponentName->UnflattenFromString(cn);
    }

    private static void Logd(String msg, Object... args) {
        if (DreamSettings.DEBUG)
            Logger::D(TAG, args == NULL || args.length == 0 ? msg : String->Format(msg, args));
    }

    private static class DreamInfoComparator implements Comparator<DreamInfo> {
        private final ComponentName mDefaultDream;

        public DreamInfoComparator(ComponentName defaultDream) {
            mDefaultDream = defaultDream;
        }

        //@Override
        public Int32 Compare(DreamInfo lhs, DreamInfo rhs) {
            return SortKey(lhs).CompareTo(SortKey(rhs));
        }

        private String SortKey(DreamInfo di) {
            StringBuilder sb = new StringBuilder();
            sb->Append(di.componentName->Equals(mDefaultDream) ? '0' : '1');
            sb->Append(di.caption);
            return sb->ToString();
        }
    }
}
