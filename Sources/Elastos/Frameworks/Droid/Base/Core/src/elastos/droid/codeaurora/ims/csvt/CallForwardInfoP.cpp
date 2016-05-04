/*
 * Copyright (c) 2013, The Linux Foundation. All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of The Linux Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.

 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
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

package org.codeaurora.ims.csvt;

using Elastos::Droid::Os::IParcel;
using Elastos::Droid::Os::IParcelable;

public class CallForwardInfoP implements Parcelable {

    public Int32             status;      /*1 = active, 0 = not active */
    public Int32             reason;      /* from TS 27.007 7.11 "reason" */
    public Int32             serviceClass; /*Sum of CommandsInterface.SERVICE_CLASS */
    public Int32             toa;         /* "type" from TS 27.007 7.11 */
    public String          number;      /* "number" from TS 27.007 7.11 */
    public Int32             timeSeconds; /* for CF no reply only */

    //@Override
    public Int32 DescribeContents() {
        return 0;
    }

    public CallForwardInfoP() {
    }

    //@Override
    CARAPI WriteToParcel(Parcel out, Int32 flags) {
        out->WriteInt(status);
        out->WriteInt(reason);
        out->WriteInt(toa);
        out->WriteString(number);
        out->WriteInt(timeSeconds);
        out->WriteInt(serviceClass);
    }

    public static const Parcelable.Creator<CallForwardInfoP> CREATOR = new
            Parcelable.Creator<CallForwardInfoP>() {

                //@Override
                public CallForwardInfoP CreateFromParcel(Parcel in) {
                    return new CallForwardInfoP(in);
                }

                //@Override
                public CallForwardInfoP[] NewArray(Int32 size) {
                    return new CallForwardInfoP[size];
                }
            };

    public CallForwardInfoP(Parcel in) {
        status = in->ReadInt();
        reason = in->ReadInt();
        toa = in->ReadInt();
        number = in->ReadString();
        timeSeconds = in->ReadInt();
        serviceClass = in->ReadInt();
    }
}
