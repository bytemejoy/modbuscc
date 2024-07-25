#ifndef MODBUS_TCP_CLIENT_H_
#define MODBUS_TCP_CLIENT_H_

#include <cstdint>
#include <string>
#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "modbus_client.h"

namespace modbus {

// Concrete Modbus client implementation using TCP/IP communication.
class TcpClient : public Client {
public:
  // Constructor taking the hostname, port, and timeout in milliseconds.
  TcpClient(const std::string &hostname, int port, int timeout_ms);

  ~TcpClient() override;

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
  int sockfd_ = -1;

  // Server address information.
  std::string hostname_;
  int port_;
};

} // namespace modbus

#endif // MODBUS_TCP_CLIENT_H_
