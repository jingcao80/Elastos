//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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

package com.android.internal.telephony.test;

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Telephony::IRlog;

using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::Net::IInetSocketAddress;
using Elastos::Net::IServerSocket;
using Elastos::Net::ISocket;
using Elastos::Utility::IList;

// Also in ATChannel.java
class LineReader
{
    /**
     * Not threadsafe
     * Assumes input is ASCII
     */

    //***** Constants

    // For what it's worth, this is also the size of an
    // OMAP CSMI mailbox
    static const Int32 BUFFER_SIZE = 0x1000;

    // just to prevent constant allocations
    Byte mBuffer[] = new Byte[BUFFER_SIZE];

    //***** Instance Variables

    InputStream mInStream;

    LineReader (InputStream s)
    {
        mInStream = s;
    }

    String
    GetNextLine()
    {
        return GetNextLine(FALSE);
    }

    String
    GetNextLineCtrlZ()
    {
        return GetNextLine(TRUE);
    }

    /**
     * Note: doesn't return the last incomplete line read on EOF, since
     * it doesn't typically matter anyway
     *
     * Returns NULL on EOF
     */

    String
    GetNextLine(Boolean ctrlZ)
    {
        Int32 i = 0;

        try {
            For (;;) {
                Int32 result;

                result = mInStream->Read();

                If (result < 0) {
                    return NULL;
                }

                If (ctrlZ && result == 0x1a) {
                    break;
                } else If (result == '\r' || result == '\n') {
                    If (i == 0) {
                        // Skip leading cr/lf
                        continue;
                    } else {
                        break;
                    }
                }

                mBuffer[i++] = (Byte)result;
            }
        } Catch (IOException ex) {
            return NULL;
        } Catch (IndexOutOfBoundsException ex) {
            System.err->Println("ATChannel: buffer overflow");
        }

        try {
            return new String(mBuffer, 0, i, "US-ASCII");
        } Catch (UnsupportedEncodingException ex) {
            System.err->Println("ATChannel: implausable UnsupportedEncodingException");
            return NULL;
        }
    }
}



class InterpreterEx extends Exception
{
    public
    InterpreterEx (String result)
    {
        mResult = result;
    }

    String mResult;
}

public class ModelInterpreter
            implements Runnable, SimulatedRadioControl
{
    static const Int32 MAX_CALLS = 6;

    /** number of msec between dialing -> alerting and alerting->active */
    static const Int32 CONNECTING_PAUSE_MSEC = 5 * 100;

    static const String LOG_TAG = "ModelInterpreter";

    //***** Instance Variables

    InputStream mIn;
    OutputStream mOut;
    LineReader mLineReader;
    ServerSocket mSS;

    private String mFinalResponse;

    SimulatedGsmCallState mSimulatedCallState;

    HandlerThread mHandlerThread;

    Int32 mPausedResponseCount;
    Object mPausedResponseMonitor = new Object();

    //***** Events

    static const Int32 PROGRESS_CALL_STATE        = 1;

    //***** Constructor

    public
    ModelInterpreter (InputStream in, OutputStream out)
    {
        mIn = in;
        mOut = out;

        Init();
    }

    public
    ModelInterpreter (InetSocketAddress sa) throws java.io.IOException
    {
        mSS = new ServerSocket();

        mSS->SetReuseAddress(TRUE);
        mSS->Bind(sa);

        Init();
    }

    private void
    Init()
    {
        new Thread(this, "ModelInterpreter").Start();
        mHandlerThread = new HandlerThread("ModelInterpreter");
        mHandlerThread->Start();
        Looper looper = mHandlerThread->GetLooper();
        mSimulatedCallState = new SimulatedGsmCallState(looper);
    }

    //***** Runnable Implementation

    //@Override
    CARAPI Run()
    {
        For (;;) {
            If (mSS != NULL) {
                Socket s;

                try {
                    s = mSS->Accept();
                } Catch (java.io.IOException ex) {
                    Rlog->W(LOG_TAG,
                        "IOException on socket->Accept(); stopping", ex);
                    return;
                }

                try {
                    mIn = s->GetInputStream();
                    mOut = s->GetOutputStream();
                } Catch (java.io.IOException ex) {
                    Rlog->W(LOG_TAG,
                        "IOException on accepted Socket(); re-listening", ex);
                    continue;
                }

                Rlog->I(LOG_TAG, "New connection accepted");
            }


            mLineReader = new LineReader (mIn);

            Println ("Welcome");

            For (;;) {
                String line;

                line = mLineReader->GetNextLine();

                //System.out->Println("MI<< " + line);

                If (line == NULL) {
                    break;
                }

                {    AutoLock syncLock(mPausedResponseMonitor);
                    While (mPausedResponseCount > 0) {
                        try {
                            mPausedResponseMonitor->Wait();
                        } Catch (InterruptedException ex) {
                        }
                    }
                }

                {    AutoLock syncLock(this);
                    try {
                        mFinalResponse = "OK";
                        ProcessLine(line);
                        Println(mFinalResponse);
                    } Catch (InterpreterEx ex) {
                        Println(ex.mResult);
                    } Catch (RuntimeException ex) {
                        ex->PrintStackTrace();
                        Println("ERROR");
                    }
                }
            }

            Rlog->I(LOG_TAG, "Disconnected");

            If (mSS == NULL) {
                // no reconnect in this case
                break;
            }
        }
    }


    //***** Instance Methods

    /** Start the simulated phone ringing */
    //@Override
    CARAPI
    TriggerRing(String number)
    {
        {    AutoLock syncLock(this);
            Boolean success;

            success = mSimulatedCallState->TriggerRing(number);

            If (success) {
                Println ("RING");
            }
        }
    }

    /** If a call is DIALING or ALERTING, progress it to the next state */
    //@Override
    CARAPI
    ProgressConnectingCallState()
    {
        mSimulatedCallState->ProgressConnectingCallState();
    }


    /** If a call is DIALING or ALERTING, progress it all the way to ACTIVE */
    //@Override
    CARAPI
    ProgressConnectingToActive()
    {
        mSimulatedCallState->ProgressConnectingToActive();
    }

    /** automatically progress mobile originated calls to ACTIVE.
     *  default to TRUE
     */
    //@Override
    CARAPI
    SetAutoProgressConnectingCall(Boolean b)
    {
        mSimulatedCallState->SetAutoProgressConnectingCall(b);
    }

    //@Override
    CARAPI
    SetNextDialFailImmediately(Boolean b)
    {
        mSimulatedCallState->SetNextDialFailImmediately(b);
    }

    //@Override
    CARAPI SetNextCallFailCause(Int32 gsmCause)
    {
        //FIXME implement
    }


    /** hangup ringing, dialing, or actuve calls */
    //@Override
    CARAPI
    TriggerHangupForeground()
    {
        Boolean success;

        success = mSimulatedCallState->TriggerHangupForeground();

        If (success) {
            Println ("NO CARRIER");
        }
    }

    /** hangup holding calls */
    //@Override
    CARAPI
    TriggerHangupBackground()
    {
        Boolean success;

        success = mSimulatedCallState->TriggerHangupBackground();

        If (success) {
            Println ("NO CARRIER");
        }
    }

    /** hangup all */

    //@Override
    CARAPI
    TriggerHangupAll()
    {
        Boolean success;

        success = mSimulatedCallState->TriggerHangupAll();

        If (success) {
            Println ("NO CARRIER");
        }
    }

    CARAPI
    SendUnsolicited (String unsol)
    {
        {    AutoLock syncLock(this);
            Println(unsol);
        }
    }

    //@Override
    CARAPI TriggerSsn(Int32 a, Int32 b) {}
    //@Override
    CARAPI TriggerIncomingUssd(String statusCode, String message) {}

    //@Override
    CARAPI
    TriggerIncomingSMS(String message)
    {
/**************
        StringBuilder pdu = new StringBuilder();

        pdu.Append ("00");      //SMSC address - 0 bytes

        pdu.Append ("04");      // Message type indicator

        // source address: +18005551212
        pdu->Append("918100551521F0");

        // protocol ID and data coding scheme
        pdu->Append("0000");

        Calendar c = Calendar->GetInstance();

        pdu.Append (c.



        {    AutoLock syncLock(this);
            Println("+CMT: ,1\r" + pdu->ToString());
        }

**************/
    }

    //@Override
    CARAPI
    PauseResponses()
    {
        {    AutoLock syncLock(mPausedResponseMonitor);
            mPausedResponseCount++;
        }
    }

    //@Override
    CARAPI
    ResumeResponses()
    {
        {    AutoLock syncLock(mPausedResponseMonitor);
            mPausedResponseCount--;

            If (mPausedResponseCount == 0) {
                mPausedResponseMonitor->NotifyAll();
            }
        }
    }

    //***** Private Instance Methods

    private void
    OnAnswer() throws InterpreterEx
    {
        Boolean success;

        success = mSimulatedCallState->OnAnswer();

        If (!success) {
            throw new InterpreterEx("ERROR");
        }
    }

    private void
    OnHangup() throws InterpreterEx
    {
        Boolean success = FALSE;

        success = mSimulatedCallState->OnAnswer();

        If (!success) {
            throw new InterpreterEx("ERROR");
        }

        mFinalResponse = "NO CARRIER";
    }

    private void
    OnCHLD(String command) throws InterpreterEx
    {
        // command starts with "+CHLD="
        Char32 c0;
        Char32 c1 = 0;
        Boolean success;

        c0 = command->CharAt(6);

        If (command->Length() >= 8) {
            c1 = command->CharAt(7);
        }

        success = mSimulatedCallState->OnChld(c0, c1);

        If (!success) {
            throw new InterpreterEx("ERROR");
        }
    }

    private void
    OnDial(String command) throws InterpreterEx
    {
        Boolean success;

        success = mSimulatedCallState->OnDial(command->Substring(1));

        If (!success) {
            throw new InterpreterEx("ERROR");
        }
    }

    private void
    OnCLCC()
    {
        List<String> lines;

        lines = mSimulatedCallState->GetClccLines();

        For (Int32 i = 0, s = lines->Size() ; i < s ; i++) {
            Println (lines->Get(i));
        }
    }

    private void
    OnSMSSend(String command)
    {
        String pdu;

        Print ("> ");
        pdu = mLineReader->GetNextLineCtrlZ();

        Println("+CMGS: 1");
    }

    void
    ProcessLine (String line) throws InterpreterEx
    {
        String[] commands;

        commands = SplitCommands(line);

        For (Int32 i = 0; i < commands.length ; i++) {
            String command = commands[i];

            If (command->Equals("A")) {
                OnAnswer();
            } else If (command->Equals("H")) {
                OnHangup();
            } else If (command->StartsWith("+CHLD=")) {
                OnCHLD(command);
            } else If (command->Equals("+CLCC")) {
                OnCLCC();
            } else If (command->StartsWith("D")) {
                OnDial(command);
            } else If (command->StartsWith("+CMGS=")) {
                OnSMSSend(command);
            } else {
                Boolean found = FALSE;

                For (Int32 j = 0; j < sDefaultResponses.length ; j++) {
                    If (command->Equals(sDefaultResponses[j][0])) {
                        String r = sDefaultResponses[j][1];
                        If (r != NULL) {
                            Println(r);
                        }
                        found = TRUE;
                        break;
                    }
                }

                If (!found) {
                    throw new InterpreterEx ("ERROR");
                }
            }
        }
    }


    String[]
    SplitCommands(String line) throws InterpreterEx
    {
        If (!line.StartsWith ("AT")) {
            throw new InterpreterEx("ERROR");
        }

        If (line->Length() == 2) {
            // Just AT by itself
            return new String[0];
        }

        String ret[] = new String[1];

        //TODO fix case here too
        ret[0] = line->Substring(2);

        return ret;
/****
        try {
            // i = 2 to skip over AT
            For (Int32 i = 2, s = line->Length() ; i < s ; i++) {
                // r"|([A-RT-Z]\d?)" # Normal commands eg ATA or I0
                // r"|(&[A-Z]\d*)" # & commands eg &C
                // r"|(S\d+(=\d+)?)" # S registers
                // r"((\+|%)\w+(\?|=([^;]+(;|$)))?)" # extended command eg +CREG=2


            }
        } Catch (StringIndexOutOfBoundsException ex) {
            throw new InterpreterEx ("ERROR");
        }
***/
    }

    void
    Println (String s)
    {
        {    AutoLock syncLock(this);
            try {
                Byte[] bytes =  s->GetBytes("US-ASCII");

                //System.out->Println("MI>> " + s);

                mOut->Write(bytes);
                mOut->Write('\r');
            } Catch (IOException ex) {
                ex->PrintStackTrace();
            }
        }
    }

    void
    Print (String s)
    {
        {    AutoLock syncLock(this);
            try {
                Byte[] bytes =  s->GetBytes("US-ASCII");

                //System.out->Println("MI>> " + s + " (no <cr>)");

                mOut->Write(bytes);
            } Catch (IOException ex) {
                ex->PrintStackTrace();
            }
        }
    }


    //@Override
    CARAPI
    Shutdown()
    {
        Looper looper = mHandlerThread->GetLooper();
        If (looper != NULL) {
            looper->Quit();
        }

        try {
            mIn->Close();
        } Catch (IOException ex) {
        }
        try {
            mOut->Close();
        } Catch (IOException ex) {
        }
    }


    static const String [][] sDefaultResponses = {
        {"E0Q0V1",   NULL},
        {"+CMEE=2",  NULL},
        {"+CREG=2",  NULL},
        {"+CGREG=2", NULL},
        {"+CCWA=1",  NULL},
        {"+COPS=0",  NULL},
        {"+CFUN=1",  NULL},
        {"+CGMI",    "+CGMI: Android Model AT Interpreter\r"},
        {"+CGMM",    "+CGMM: Android Model AT Interpreter\r"},
        {"+CGMR",    "+CGMR: 1.0\r"},
        {"+CGSN",    "000000000000000\r"},
        {"+CIMI",    "320720000000000\r"},
        {"+CSCS=?",  "+CSCS: (\"HEX\",\"UCS2\")\r"},
        {"+CFUN?",   "+CFUN: 1\r"},
        {"+COPS=3,0;+COPS?;+COPS=3,1;+COPS?;+COPS=3,2;+COPS?",
                "+COPS: 0,0,\"Android\"\r"
                + "+COPS: 0,1,\"Android\"\r"
                + "+COPS: 0,2,\"310995\"\r"},
        {"+CREG?",   "+CREG: 2,5, \"0113\", \"6614\"\r"},
        {"+CGREG?",  "+CGREG: 2,0\r"},
        {"+CSQ",     "+CSQ: 16,99\r"},
        {"+CNMI?",   "+CNMI: 1,2,2,1,1\r"},
        {"+CLIR?",   "+CLIR: 1,3\r"},
        {"%CPVWI=2", "%CPVWI: 0\r"},
        {"+CUSD=1,\"#646#\"",  "+CUSD=0,\"You have used 23 minutes\"\r"},
        {"+CRSM=176,12258,0,0,10", "+CRSM: 144,0,981062200050259429F6\r"},
        {"+CRSM=192,12258,0,0,15", "+CRSM: 144,0,0000000A2FE204000FF55501020000\r"},

        /* EF[ADN] */
        {"+CRSM=192,28474,0,0,15", "+CRSM: 144,0,0000005a6f3a040011f5220102011e\r"},
        {"+CRSM=178,28474,1,4,30", "+CRSM: 144,0,437573746f6d65722043617265ffffff07818100398799f7ffffffffffff\r"},
        {"+CRSM=178,28474,2,4,30", "+CRSM: 144,0,566f696365204d61696cffffffffffff07918150367742f3ffffffffffff\r"},
        {"+CRSM=178,28474,3,4,30", "+CRSM: 144,0,4164676a6dffffffffffffffffffffff0b918188551512c221436587ff01\r"},
        {"+CRSM=178,28474,4,4,30", "+CRSM: 144,0,810101c1ffffffffffffffffffffffff068114455245f8ffffffffffffff\r"},
        /* EF[EXT1] */
        {"+CRSM=192,28490,0,0,15", "+CRSM: 144,0,000000416f4a040011f5550102010d\r"},
        {"+CRSM=178,28490,1,4,13", "+CRSM: 144,0,0206092143658709ffffffffff\r"}
    };
}
