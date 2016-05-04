/*
 * Copyright (C) 2007 The Android Open Source Project
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

package com.android.internal.telephony.cat;

using Elastos::Droid::Os::IParcel;
using Elastos::Droid::Os::IParcelable;

abstract class ValueObject {
    abstract ComprehensionTlvTag GetTag();
}

/**
 * Class for Command Details object of proactive commands from SIM.
 * {@hide}
 */
class CommandDetails extends ValueObject implements Parcelable {
    public Boolean compRequired;
    public Int32 commandNumber;
    public Int32 typeOfCommand;
    public Int32 commandQualifier;

    //@Override
    public ComprehensionTlvTag GetTag() {
        return ComprehensionTlvTag.COMMAND_DETAILS;
    }

    CommandDetails() {
    }

    public Boolean CompareTo(CommandDetails other) {
        Return (this.compRequired == other.compRequired &&
                this.commandNumber == other.commandNumber &&
                this.commandQualifier == other.commandQualifier &&
                this.typeOfCommand == other.typeOfCommand);
    }

    public CommandDetails(Parcel in) {
        compRequired = in->ReadInt() != 0;
        commandNumber = in->ReadInt();
        typeOfCommand = in->ReadInt();
        commandQualifier = in->ReadInt();
    }

    //@Override
    CARAPI WriteToParcel(Parcel dest, Int32 flags) {
        dest->WriteInt(compRequired ? 1 : 0);
        dest->WriteInt(commandNumber);
        dest->WriteInt(typeOfCommand);
        dest->WriteInt(commandQualifier);
    }

    public static const Parcelable.Creator<CommandDetails> CREATOR =
                                new Parcelable.Creator<CommandDetails>() {
        //@Override
        public CommandDetails CreateFromParcel(Parcel in) {
            return new CommandDetails(in);
        }

        //@Override
        public CommandDetails[] NewArray(Int32 size) {
            return new CommandDetails[size];
        }
    };

    //@Override
    public Int32 DescribeContents() {
        return 0;
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return "CmdDetails: compRequired=" + compRequired +
                " commandNumber=" + commandNumber +
                " typeOfCommand=" + typeOfCommand +
                " commandQualifier=" + commandQualifier;
    }
}

class DeviceIdentities extends ValueObject {
    public Int32 sourceId;
    public Int32 destinationId;

    //@Override
    ComprehensionTlvTag GetTag() {
        return ComprehensionTlvTag.DEVICE_IDENTITIES;
    }
}

// Container class to hold icon identifier value.
class IconId extends ValueObject {
    Int32 recordNumber;
    Boolean selfExplanatory;

    //@Override
    ComprehensionTlvTag GetTag() {
        return ComprehensionTlvTag.ICON_ID;
    }
}

// Container class to hold item icon identifier list value.
class ItemsIconId extends ValueObject {
    Int32 [] recordNumbers;
    Boolean selfExplanatory;

    //@Override
    ComprehensionTlvTag GetTag() {
        return ComprehensionTlvTag.ITEM_ICON_ID_LIST;
    }
}

class ActivateDescriptor extends ValueObject {
    public Int32 target;

    //@Override
    ComprehensionTlvTag GetTag() {
        return ComprehensionTlvTag.ACTIVATE_DESCRIPTOR;
    }
}
