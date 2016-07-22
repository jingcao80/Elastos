module.exports = {
    attr : {
        xmlns : {
            android : "http://schemas.android.com/apk/res/android",
        },
        package_name : "JSCalculator",
        android : {
            versionCode : "1",
            versionName : "1.0",
        },
    },

    uses_permission : {
        android : {
            names : [
                "android.permission.RECEIVE_BOOT_COMPLETED",
                "android.permission.READ_EXTERNAL_STORAGE",
                "android.permission.ACCESS_NETWORK_STATE",
            ],
        },
    },

    uses_sdk : {
        attr : {
            android : {
                minSdkVersion : "8",
                targetSdkVersion : "17",
            },
        },
    },

    application : {
        attr : {
            android : {
                icon : "@drawable/icon",
                label : "@string/app_name",
            },
        },

        r : "/data/temp/node/JSCalculator/R.js",

        activities : [
            {
                attr : {
                    android : {
                        name : "CCalculator",
                        label : "@string/app_name",
                        theme : "@android:style/Theme.NoTitleBar",
                    },
                },
                intent_filter : {
                    action : {
                        android : {
                            name : "android.intent.action.MAIN",
                        }
                    },
                    category : [
                        {
                            android : {
                                name : "android.intent.action.MAIN",
                            },
                            android : {
                                name : "android.intent.category.LAUNCHER",
                            },
                        },
                    ],
                },
                js : "/data/temp/node/JSCalculator/CCalculator.js",
            },
        ],
    },
};
