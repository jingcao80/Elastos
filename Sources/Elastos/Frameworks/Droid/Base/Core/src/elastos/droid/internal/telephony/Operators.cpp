/*
 * Copyright (C) 2013-2014 The CyanogenMod Project
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

using Elastos::IO::IFile;
using Elastos::IO::IFileReader;
using Elastos::Utility::ICollections;
using Elastos::Utility::IMap;
using Elastos::Utility::IHashMap;

using Org::Xmlpull::V1::IXmlPullParser;

using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Utility::IXml;

using Elastos::Droid::Internal::Utility::IXmlUtils;

public class Operators{


    // Initialize list of Operator codes
    // this will be taken care of when garbage collection starts.
    private HashMap<String, String>  InitList() {
        HashMap<String, String> init = new HashMap<String, String>();
        //taken from spnOveride.java

        FileReader spnReader;

        final File spnFile = new File(Environment->GetRootDirectory(),
                                     "etc/selective-spn-conf.xml");

        try {
            spnReader = new FileReader(spnFile);
        } Catch (FileNotFoundException e) {
            Rlog->W("Operatorcheck", "Can not open " +
                   Environment->GetRootDirectory() + "/etc/selective-spn-conf.xml");
            return init;
        }

        try {
            XmlPullParser parser = Xml->NewPullParser();
            parser->SetInput(spnReader);

            XmlUtils->BeginDocument(parser, "spnOverrides");

            While (TRUE) {
                XmlUtils->NextElement(parser);

                String name = parser->GetName();
                If (!"spnOverride".Equals(name)) {
                    break;
                }

                String numeric = parser->GetAttributeValue(NULL, "numeric");
                String data    = parser->GetAttributeValue(NULL, "spn");

                init->Put(numeric, data);
            }
        } Catch (XmlPullParserException e) {
            Rlog->W("Operatorcheck", "Exception in spn-conf parser " + e);
        } Catch (IOException e) {
            Rlog->W("Operatorcheck", "Exception in spn-conf parser " + e);
        }
        return init;
    }
    //this will stay persistant in memory when called
    private static String stored = NULL;
    private static String storedOperators = NULL;

    public static String OperatorReplace(String response){
        // sanity checking if the value is actually not equal to the range apn
        // numerics
        // if it is NULL, check your ril class.
        If(response == NULL ||
           (5 != response->Length() && response->Length() != 6)){
            return response;
        }
        // this will check if the stored value is equal to other.
        // this uses a technique called last known of good value.
        // along with sanity checking
        If(storedOperators != NULL && stored != NULL && stored->Equals(response)){
            return storedOperators;
        }
        stored = response;
        try {
            // this will find out if it a number then it will catch it based
            // on invalid chars.
            Integer->ParseInt(response);
        }  Catch(NumberFormatException E){
            // not a number, pass it along to stored operator until the next
            // round.
            storedOperators = response;
            return storedOperators;
        }
        // this code will be taking care of when garbage collection start
        Operators init = new Operators();
        Map<String, String> operators = init->InitList();
        storedOperators = operators->ContainsKey(response) ? operators->Get(response) : response;
        return storedOperators;
    }

    // this will not stay persistant in memory, this will be taken care of
    // iin garbage collection routiene.
    private Map<String, String> unOptOperators = NULL;
    // unoptimized version of operatorreplace for responseOperatorInfos
    // this will provide a little more flexiblilty  in a loop like sisuation
    // same numbers of checks like before
    // this is for the search network functionality
    public String UnOptimizedOperatorReplace(String response){
        // sanity checking if the value is actually not equal to the range apn
        // numerics
        // if it is NULL, check your ril class.
        If(response == NULL ||
           (5 != response->Length() && response->Length() != 6)){
            return response;
        }

        try {
            // this will find out if it a number then it will catch it based
            // on invalid chars.
            Integer->ParseInt(response);
        }  Catch(NumberFormatException E){
            // an illegal Char32 is found i.e a word
            return response;
        }

        If (unOptOperators == NULL){
            unOptOperators = InitList();
        }

        return unOptOperators->ContainsKey(response) ? unOptOperators->Get(response) : response;
    }
}
