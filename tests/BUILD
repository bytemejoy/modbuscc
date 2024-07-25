load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "modbus_client_test",
    srcs = ["modbus_client_test.cc"],
    deps = [
        "//:modbus_client",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "modbus_functions_test",
    srcs = ["modbus_functions_test.cc"],
    deps = [
        "//:modbus_functions",
        "@googletest//:gtest_main",
        "@googletest//:gtest",
    ],
)
