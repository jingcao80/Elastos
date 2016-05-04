/*
 * Copyright (c) 2010-2013, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

package com.android.internal.telephony;

using Elastos::Droid::Text::ITextUtils;

using Elastos::Droid::Telephony::IRlog;

/**
 * Class holding all the information of a subscription from UICC Card.
 */
public class Subscription {
    private static const String LOG_TAG = "Subscription";

    public Int32 slotId;                       // Slot id
    public Int32 m3gppIndex;                   // Subscription index in the card for GSM
    public Int32 m3gpp2Index;                  // Subscription index in the card for CDMA
    public Int32 subId;                        // SUB 0 or SUB 1
    public SubscriptionStatus subStatus;      // DEACTIVATE = 0, ACTIVATE = 1,
                                             // ACTIVATED = 2, DEACTIVATED = 3, INVALID = 4;
    public String appId;
    public String appLabel;
    public String appType;
    public String iccId;

    private Boolean DEBUG = FALSE;

    /**
     * Subscription activation status
     */
    public enum SubscriptionStatus {
        SUB_DEACTIVATE,
            SUB_ACTIVATE,
            SUB_ACTIVATED,
            SUB_DEACTIVATED,
            SUB_INVALID
    }

    public static const Int32 SUBSCRIPTION_INDEX_INVALID = -1;

    public Subscription() {
        Clear();
    }

    CARAPI ToString(
        /* [out] */ String* str)
    {
        return "Subscription = { "
            + "slotId = " + slotId
            + ", 3gppIndex = " + m3gppIndex
            + ", 3gpp2Index = " + m3gpp2Index
            + ", subId = " + subId
            + ", subStatus = " + subStatus
            + ", appId = " + appId
            + ", appLabel = " + appLabel
            + ", appType = " + appType
            + ", iccId = " + iccId + " }";
    }

    public Boolean Equals(Subscription sub) {
        If (sub != NULL) {
            If ((slotId == sub.slotId) && (m3gppIndex == sub.m3gppIndex)
                    && (m3gpp2Index == sub.m3gpp2Index) && (subId == sub.subId)
                    && (subStatus == sub.subStatus)
                    && ((TextUtils->IsEmpty(appId) && TextUtils->IsEmpty(sub.appId))
                            || TextUtils->Equals(appId, sub.appId))
                    && ((TextUtils->IsEmpty(appLabel) && TextUtils->IsEmpty(sub.appLabel))
                            || TextUtils->Equals(appLabel, sub.appLabel))
                    && ((TextUtils->IsEmpty(appType) && TextUtils->IsEmpty(sub.appType))
                            || TextUtils->Equals(appType, sub.appType))
                    && ((TextUtils->IsEmpty(iccId) && TextUtils->IsEmpty(sub.iccId))
                            || TextUtils->Equals(iccId, sub.iccId))) {
                return TRUE;
            }
        } else {
            Rlog->D(LOG_TAG, "Subscription.equals: sub == NULL");
        }
        return FALSE;
    }

    /**
     * Return TRUE if the appIndex, appId, appLabel and iccId are matching.
     * @param sub
     * @return
     */
    public Boolean IsSame(Subscription sub) {
        // Not checking the subId, subStatus and slotId, which are related to the
        // activated status
        If (sub != NULL) {
            If (DEBUG) {
                Rlog->D(LOG_TAG, "IsSame(): this = " + m3gppIndex
                        + ":" + m3gpp2Index
                        + ":" + appId
                        + ":" + appType
                        + ":" + iccId);
                Rlog->D(LOG_TAG, "compare with = " + sub.m3gppIndex
                        + ":" + sub.m3gpp2Index
                        + ":" + sub.appId
                        + ":" + sub.appType
                        + ":" + sub.iccId);
            }
            If ((m3gppIndex == sub.m3gppIndex)
                    && (m3gpp2Index == sub.m3gpp2Index)
                    && ((TextUtils->IsEmpty(appId) && TextUtils->IsEmpty(sub.appId))
                            || TextUtils->Equals(appId, sub.appId))
                    && ((TextUtils->IsEmpty(appType) && TextUtils->IsEmpty(sub.appType))
                            || TextUtils->Equals(appType, sub.appType))
                    && ((TextUtils->IsEmpty(iccId) && TextUtils->IsEmpty(sub.iccId))
                            || TextUtils->Equals(iccId, sub.iccId))){
                return TRUE;
            }
        }
        return FALSE;
    }

    /**
     * Reset the subscription
     */
    CARAPI Clear() {
        slotId = SUBSCRIPTION_INDEX_INVALID;
        m3gppIndex = SUBSCRIPTION_INDEX_INVALID;
        m3gpp2Index = SUBSCRIPTION_INDEX_INVALID;
        subId = SUBSCRIPTION_INDEX_INVALID;
        subStatus = SubscriptionStatus.SUB_INVALID;
        appId = NULL;
        appLabel = NULL;
        appType = NULL;
        iccId = NULL;
    }

    /**
     * Copies the subscription parameters
     * @param from
     * @return
     */
    public Subscription CopyFrom(Subscription from) {
        If (from != NULL) {
            slotId = from.slotId;
            m3gppIndex = from.m3gppIndex;
            m3gpp2Index = from.m3gpp2Index;
            subId = from.subId;
            subStatus = from.subStatus;
            If (from.appId != NULL) {
                appId = new String(from.appId);
            }
            If (from.appLabel != NULL) {
                appLabel = new String(from.appLabel);
            }
            If (from.appType != NULL) {
                appType = new String(from.appType);
            }
            If (from.iccId != NULL) {
                iccId = new String(from.iccId);
            }
        }

        return this;
    }

    /**
     * Return the valid app Index (either 3gpp or 3gpp2 index)
     * @return
     */
    public Int32 GetAppIndex() {
        If (this.m3gppIndex != SUBSCRIPTION_INDEX_INVALID) {
            return this.m3gppIndex;
        } else {
            return this.m3gpp2Index;
        }
    }
}
