#ifndef MODBUS_TCP_CLIENT_WIN_H_
#define MODBUS_TCP_CLIENT_WIN_H_

#include <absl/status/status.h>
#include <cstdint>
#include <string>
#include <vector>

#include "modbus_client.h"

namespace modbus {

// Concrete Modbus client implementation using TCP/IP communication for Windows.
class TcpClientWin : public Client {
public:
  // Constructor taking the hostname, port, and timeout in milliseconds.
  TcpClientWin(const std::string &hostname, int port, int timeout_ms);

  ~TcpClientWin() override;

  // Connects to the Modbus TCP server.
  absl::Status Connect();

  // Disconnects from the Modbus TCP server.
  absl::Status Disconnect();

  // Sends a Modbus request and receives the response.
  absl::StatusOr<std::vector<uint8_t>>
  SendReceive(uint8_t slave_id, FunctionCode function_code,
              const std::vector<uint8_t> &request_data) override;

private:
  // Socket handle.
  SOCKET sockfd_ = INVALID_SOCKET;

  // Server address information.
  std::string hostname_;
  int port_;

  // Address info structure for the server.
  addrinfo *server_info_ = nullptr;
};

} // namespace modbus

#endif // MODBUS_TCP_CLIENT_WIN_H_
