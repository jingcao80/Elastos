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

package com.android.internal.telephony;

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;

using Elastos::Droid::Internal::Telephony::IccCardConstants::IState;
using Elastos::Droid::Internal::Telephony::Uicc::IIccCardApplicationStatus;
using Elastos::Droid::Internal::Telephony::Uicc::IIccFileHandler;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;

/**
 * {@hide}
 * @Deprecated use UiccController.getUiccCard instead.
 *
 * Integrated Circuit Card (ICC) interface
 * An object of a class implementing this interface is used by external
 * Apps (specifically PhoneApp) to perform icc card related functionality.
 *
 * Apps (those that have access to Phone object) can retrieve this object
 * by calling phone->GetIccCard()
 *
 * This interface is implemented by IccCardProxy and the object PhoneApp
 * gets when it calls getIccCard is IccCardProxy.
 */
public interface IccCard {
    /**
     * @return combined Card and current App state
     */
    public State GetState();

    /**
     * @return IccRecords object belonging to current UiccCardApplication
     */
    public IccRecords GetIccRecords();

    /**
     * @return IccFileHandler object belonging to current UiccCardApplication
     */
    public IccFileHandler GetIccFileHandler();

    /**
     * Notifies handler of any transition into IccCardConstants.State.ABSENT
     */
    CARAPI RegisterForAbsent(Handler h, Int32 what, Object obj);
    CARAPI UnregisterForAbsent(Handler h);

    /**
     * Notifies handler of any transition into IccCardConstants.State.PERSO_LOCKED
     */
    CARAPI RegisterForPersoLocked(Handler h, Int32 what, Object obj);
    CARAPI UnregisterForPersoLocked(Handler h);

    /**
     * Notifies handler of any transition into IccCardConstants.State->IsPinLocked()
     */
    CARAPI RegisterForLocked(Handler h, Int32 what, Object obj);
    CARAPI UnregisterForLocked(Handler h);

    /**
     * Supply the ICC PIN to the ICC
     *
     * When the operation is complete, onComplete will be sent to its
     * Handler.
     *
     * onComplete.obj will be an AsyncResult
     *
     * ((AsyncResult)onComplete.obj).exception == NULL on success
     * ((AsyncResult)onComplete.obj).exception != NULL on fail
     *
     * If the supplied PIN is incorrect:
     * ((AsyncResult)onComplete.obj).exception != NULL
     * && ((AsyncResult)onComplete.obj).exception
     *       instanceof com.android.internal.telephony.gsm.CommandException)
     * && ((CommandException)(((AsyncResult)onComplete.obj).exception))
     *          .GetCommandError() == CommandException.Error.PASSWORD_INCORRECT
     */
    CARAPI SupplyPin (String pin, Message onComplete);

    /**
     * Supply the ICC PUK to the ICC
     */
    CARAPI SupplyPuk (String puk, String newPin, Message onComplete);

    /**
     * Supply the ICC PIN2 to the ICC
     */
    CARAPI SupplyPin2 (String pin2, Message onComplete);

    /**
     * Supply the ICC PUK2 to the ICC
     */
    CARAPI SupplyPuk2 (String puk2, String newPin2, Message onComplete);

    /**
     * Check whether Fdn (fixed dialing number) service is available.
     * @return TRUE if ICC fdn service available
     *         FALSE if ICC fdn service not available
    */
    public Boolean GetIccFdnAvailable();

    /**
     * Supply Depersonalization code to the RIL
     */
    CARAPI SupplyDepersonalization (String pin, String type, Message onComplete);

    /**
     * Check whether ICC pin lock is enabled
     * This is a sync call which returns the cached pin enabled state
     *
     * @return TRUE for ICC locked enabled
     *         FALSE for ICC locked disabled
     */
    public Boolean GetIccLockEnabled();

    /**
     * Check whether ICC Fdn (fixed dialing number) is enabled
     * This is a sync call which returns the cached pin enabled state
     *
     * @return TRUE for ICC fdn enabled
     *         FALSE for ICC fdn disabled
     */
    public Boolean GetIccFdnEnabled();

     /**
      * Set the ICC pin lock enabled or disabled
      * When the operation is complete, onComplete will be sent to its handler
      *
      * @param enabled "TRUE" for locked "FALSE" for unlocked.
      * @param password needed to change the ICC pin state, aka. Pin1
      * @param onComplete
      *        onComplete.obj will be an AsyncResult
      *        ((AsyncResult)onComplete.obj).exception == NULL on success
      *        ((AsyncResult)onComplete.obj).exception != NULL on fail
      */
     CARAPI SetIccLockEnabled (Boolean enabled,
             String password, Message onComplete);

     /**
      * Set the ICC fdn enabled or disabled
      * When the operation is complete, onComplete will be sent to its handler
      *
      * @param enabled "TRUE" for locked "FALSE" for unlocked.
      * @param password needed to change the ICC fdn enable, aka Pin2
      * @param onComplete
      *        onComplete.obj will be an AsyncResult
      *        ((AsyncResult)onComplete.obj).exception == NULL on success
      *        ((AsyncResult)onComplete.obj).exception != NULL on fail
      */
     CARAPI SetIccFdnEnabled (Boolean enabled,
             String password, Message onComplete);

     /**
      * Change the ICC password used in ICC pin lock
      * When the operation is complete, onComplete will be sent to its handler
      *
      * @param oldPassword is the old password
      * @param newPassword is the new password
      * @param onComplete
      *        onComplete.obj will be an AsyncResult
      *        ((AsyncResult)onComplete.obj).exception == NULL on success
      *        ((AsyncResult)onComplete.obj).exception != NULL on fail
      */
     CARAPI ChangeIccLockPassword(String oldPassword, String newPassword,
             Message onComplete);

     /**
      * Change the ICC password used in ICC fdn enable
      * When the operation is complete, onComplete will be sent to its handler
      *
      * @param oldPassword is the old password
      * @param newPassword is the new password
      * @param onComplete
      *        onComplete.obj will be an AsyncResult
      *        ((AsyncResult)onComplete.obj).exception == NULL on success
      *        ((AsyncResult)onComplete.obj).exception != NULL on fail
      */
     CARAPI ChangeIccFdnPassword(String oldPassword, String newPassword,
             Message onComplete);

    /**
     * Returns service provider name stored in ICC card.
     * If there is no service provider name associated or the record is not
     * yet available, NULL will be returned <p>
     *
     * Please use this value when display Service Provider Name in idle mode <p>
     *
     * Usage of this provider name in the UI is a common carrier requirement.
     *
     * Also available via Android property "gsm.sim.operator.alpha"
     *
     * @return Service Provider Name stored in ICC card
     *         NULL if no service provider name associated or the record is not
     *         yet available
     *
     */
    public String GetServiceProviderName ();

    /**
     * Checks if an Application of specified type present on the card
     * @param type is AppType to look for
     */
    public Boolean IsApplicationOnIcc(IccCardApplicationStatus.AppType type);

    /**
     * @return TRUE if a ICC card is present
     */
    public Boolean HasIccCard();

    /**
     * @return TRUE if ICC card is PIN2 blocked
     */
    public Boolean GetIccPin2Blocked();

    /**
     * @return TRUE if ICC card is PUK2 blocked
     */
    public Boolean GetIccPuk2Blocked();
}
