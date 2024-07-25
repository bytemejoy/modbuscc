load("@hedron_compile_commands//:refresh_compile_commands.bzl", "refresh_compile_commands")

refresh_compile_commands(
    name = "refresh_compile_commands",
    targets = {
      "//src:modbus_client": "",
      "//src:serial": "",
      "//src:modbus_tcp_client": "",
      "//src:modbus_functions": "",
      "//tests:*": "",
    },
)
