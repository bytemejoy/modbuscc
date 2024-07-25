load("@hedron_compile_commands//:refresh_compile_commands.bzl", "refresh_compile_commands")
load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "modbus_client",
    hdrs = ["modbus_client.h"],
    srcs = ["modbus_client.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "@abseil-cpp//absl/status",
        "@abseil-cpp//absl/status:statusor",
        ":serial",
    ],
)

cc_library(
    name = "serial",
    hdrs = ["serial.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@abseil-cpp//absl/status",
        "@abseil-cpp//absl/status:statusor",
    ],
)

cc_library(
    name = "serial_posix",
    hdrs = ["serial_posix.h"],
    srcs = ["serial_posix.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":serial",
        "@abseil-cpp//absl/status",
        "@abseil-cpp//absl/status:statusor",
    ],
)

# cc_library(
#     name = "serial_win",
#     hdrs = ["serial_win.h"],
#     srcs = ["serial_win.cc"],
#     visibility = ["//visibility:public"],
#     deps = [
#         ":serial",
#         "@abseil-cpp//absl/status",
#         "@abseil-cpp//absl/status:statusor",
#     ],
# )

cc_library(
    name = "modbus_tcp_client",
    hdrs = ["modbus_tcp_client.h"],
    srcs = ["modbus_tcp_client.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":modbus_client",
        "@abseil-cpp//absl/status",
        "@abseil-cpp//absl/status:statusor",
    ],
)

# cc_library(
#     name = "modbus_tcp_client_win",
#     hdrs = ["modbus_tcp_client_win.h"],
#     srcs = ["modbus_tcp_client_win.cc"],
#     visibility = ["//visibility:public"],
#     deps = [
#         ":modbus_tcp_client",
#         "@abseil-cpp//absl/status",
#         "@abseil-cpp//absl/status:statusor",
#     ],
# )

cc_library(
    name = "modbus_functions",
    hdrs = ["modbus_functions.h"],
    srcs = ["modbus_functions.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":modbus_client",
        "@abseil-cpp//absl/status",
        "@abseil-cpp//absl/status:statusor",
    ],
)

refresh_compile_commands(
    name = "refresh_compile_commands",
    targets = {
      "//:modbus_client": "",
      "//:modbus_serial": "",
      "//:modbus_tcp_client": "",
      "//:modbus_functions": "",
      "//tests:*": "",
    },
)
