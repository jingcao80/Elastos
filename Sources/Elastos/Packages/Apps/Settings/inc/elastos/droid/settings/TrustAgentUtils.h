/*
 * Copyright (C) 2014 The Android Open Source Project
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
 * limitations under the License
 */

package com.android.settings;

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Service::Trust::ITrustAgentService;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ISlog;
using Elastos::Droid::Utility::IXml;

using Org::Xmlpull::V1::IXmlPullParser;


public class TrustAgentUtils {
    static const String TAG = "TrustAgentUtils";

    private static const String TRUST_AGENT_META_DATA = TrustAgentService.TRUST_AGENT_META_DATA;
    private static const String PERMISSION_PROVIDE_AGENT = Manifest::permission::PROVIDE_TRUST_AGENT;

    /**
     * @return TRUE, if the service in resolveInfo has the permission to provide a trust agent.
     */
    public static Boolean CheckProvidePermission(ResolveInfo resolveInfo, PackageManager pm) {
        String packageName = resolveInfo.serviceInfo.packageName;
        if (pm->CheckPermission(PERMISSION_PROVIDE_AGENT, packageName)
                != PackageManager.PERMISSION_GRANTED) {
            Logger::W(TAG, "Skipping agent because package " + packageName
                    + " does not have permission " + PERMISSION_PROVIDE_AGENT + ".");
            return FALSE;
        }
        return TRUE;
    }

    public static class TrustAgentComponentInfo {
        ComponentName componentName;
        String title;
        String summary;
    }

    public static ComponentName GetComponentName(ResolveInfo resolveInfo) {
        if (resolveInfo == NULL || resolveInfo.serviceInfo == NULL) return NULL;
        return new ComponentName(resolveInfo.serviceInfo.packageName, resolveInfo.serviceInfo.name);
    }

    public static TrustAgentComponentInfo GetSettingsComponent(
            PackageManager pm, ResolveInfo resolveInfo) {
        if (resolveInfo == NULL || resolveInfo.serviceInfo == NULL
                || resolveInfo.serviceInfo.metaData == NULL) return NULL;
        String cn = NULL;
        TrustAgentComponentInfo trustAgentComponentInfo = new TrustAgentComponentInfo();
        XmlResourceParser parser = NULL;
        Exception caughtException = NULL;
        try {
            parser = resolveInfo.serviceInfo->LoadXmlMetaData(pm, TRUST_AGENT_META_DATA);
            if (parser == NULL) {
                Slogger::W(TAG, "Can't find " + TRUST_AGENT_META_DATA + " meta-data");
                return NULL;
            }
            Resources res = pm->GetResourcesForApplication(resolveInfo.serviceInfo.applicationInfo);
            AttributeSet attrs = Xml->AsAttributeSet(parser);
            Int32 type;
            while ((type = parser->Next()) != XmlPullParser.END_DOCUMENT
                    && type != XmlPullParser.START_TAG) {
            }
            String nodeName = parser->GetName();
            if (!"trust-agent".Equals(nodeName)) {
                Slogger::W(TAG, "Meta-data does not start with trust-agent tag");
                return NULL;
            }
            TypedArray sa =
                    res->ObtainAttributes(attrs, R.styleable.TrustAgent);
            trustAgentComponentInfo.summary =
                    sa->GetString(R.styleable.TrustAgent_summary);
            trustAgentComponentInfo.title =
                    sa->GetString(R.styleable.TrustAgent_title);
            cn = sa->GetString(R.styleable.TrustAgent_settingsActivity);
            sa->Recycle();
        } catch (PackageManager.NameNotFoundException e) {
            caughtException = e;
        } catch (IOException e) {
            caughtException = e;
        } catch (XmlPullParserException e) {
            caughtException = e;
        } finally {
            if (parser != NULL) parser->Close();
        }
        if (caughtException != NULL) {
            Slogger::W(TAG, "Error parsing : " + resolveInfo.serviceInfo.packageName, caughtException);
            return NULL;
        }
        if (cn != NULL && cn->IndexOf('/') < 0) {
            cn = resolveInfo.serviceInfo.packageName + "/" + cn;
        }
        trustAgentComponentInfo.componentName = (cn == NULL) ? NULL : ComponentName->UnflattenFromString(cn);
        return trustAgentComponentInfo;
    }
}
