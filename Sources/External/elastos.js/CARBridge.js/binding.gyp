{
  "targets": [
    {
      "target_name": "CARBridge",

      "dependencies": [
      ],

      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        "<!(echo $XDK_USER_INC)",
        "<!(echo $XDK_USER_INC)/../mirror/Elastos/LibCore/src/corelibrary",
        "../include"
      ],

      "sources": [
        "car-constant.cc",
        "car-data-type.cc",
        "car-function.cc",
        "car-function-adapter.cc",
        "car-imported-module.cc",
        "car-interface-adapter.cc",
        "car-module.cc",
        "car-namespace.cc",
        "car-object.cc",
        "car-type-alias.cc",
        "elastos-ext.cc",
        "error.cc",
        "js-2-car.cc",
        "libc-ext.cc",
        "parse-uri.cc",
        "require.cc",
        "this.cc",
        "util.cc"
      ],

      "cflags!": [
        "-fno-exceptions"
      ],

      "cflags": [
        "-std=c++11",
#       "-pedantic-errors",
        "-Wall",
        "-O0",
        "-fpermissive"
      ],

      "cflags_cc!": [
        "-fno-exceptions"
      ],

      "conditions": [
        [
          "dest_os == \"mac\"", {
            "xcode_settings": {
              "GCC_ENABLE_CPP_EXCEPTIONS": "YES"
            }
          }
        ],

        [
          "dest_os == \"windows\"", {
          }
        ],

        [
          "dest_os == \"linux\"", {
          }
        ],

        [
          "dest_os == \"android\"", {
            "defines": [
              "_GNUC",
              "__stdcall=",
              "__cdecl=",

              "HAVE_SYS_UIO_H",

              "_ELASTOS_BUG_EVENT_HANDLER_MAKE",
            ],

            "library_dirs": [
              "<!(echo $XDK_BUILD_PATH)/Prebuilt/Linux/usr/lib"
            ],

            "libraries": [
              "<!(echo $XDK_USER_LIB)/Elastos.Runtime.lib",
              "<!(echo $XDK_TARGETS)/Elastos.Runtime.eco",
              "<!(echo $XDK_TARGETS)/Elastos.CoreLibrary.eco"
            ]
          }
        ]
      ]
    }
  ]
}
