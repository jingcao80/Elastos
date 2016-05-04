/*
 * Copyright (c) 2014, The Linux Foundation. All rights reserved.
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
 *
 */

package com.android.internal.telephony.cat;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Telephony::ITelephonyManager;

using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::Cat::ICatService;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardApplicationStatus::IAppType;
using Elastos::Droid::Internal::Telephony::Uicc::IIccFileHandler;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;


/**
 * Class that creates the CatServices for each card.
 *
 * {@hide}
 */
public class CatServiceFactory {

    private static CatService sCatServices[] = NULL;
    private static const Int32 sSimCount = TelephonyManager->GetDefault()->GetSimCount();

    // Protects singleton instance lazy initialization.
    private static const Object sInstanceLock = new Object();

    /**
     * Used for instantiating the Service from the Card.
     *
     * @param ci CommandsInterface object
     * @param context phone app context
     * @param ic Icc card
     * @param slotId to know the index of card
     * @return The only Service object in the system
     */
    public static CatService MakeCatService(CommandsInterface ci,
            Context context, UiccCard ic, Int32 slotId) {
        UiccCardApplication ca = NULL;
        IccFileHandler fh = NULL;

        If (sCatServices == NULL) {
            sCatServices = new CatService[sSimCount];
        }

        If (ci == NULL || context == NULL || ic == NULL) return NULL;

        //get first valid filehandler in the card.
        For (Int32 i = 0; i < ic->GetNumApplications(); i++) {
            ca = ic->GetApplicationIndex(i);
            If (ca != NULL && (ca->GetType() != AppType.APPTYPE_UNKNOWN)) {
                fh = ca->GetIccFileHandler();
                break;
            }
        }

        Synchronized (sInstanceLock) {
            If (fh == NULL) return NULL;

            If (sCatServices[slotId] == NULL) {
                sCatServices[slotId] = new CatService(ci, context, fh, slotId);
            }
        }
        return sCatServices[slotId];
    }

    public static CatService GetCatService(Int32 slotId) {
        Return ((sCatServices == NULL) ? NULL : sCatServices[slotId]);
    }

    public static void DisposeCatService (Int32 slotId) {
        If (sCatServices != NULL) {
            sCatServices[slotId].Dispose();
            sCatServices[slotId] = NULL;
        }
    }
}
