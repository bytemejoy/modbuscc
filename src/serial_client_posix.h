#ifndef SERIAL_CLIENT_POSIX_H_
#define SERIAL_CLIENT_POSIX_H_

#include "absl/status/statusor.h"
#include <cstdint>
#include <memory>
#include <vector>

#include "modbus_client.h"
#include "serial_posix.h"

namespace modbus {

// Concrete Modbus client implementation using serial communication.
class SerialClient : public Client {
public:
  // Constructor taking ownership of a Serial object.
  explicit SerialClient(std::unique_ptr<Serial> serial, int timeout_ms);

  // Sends a Modbus request and receives the response.
  absl::StatusOr<std::vector<uint8_t>>
  SendReceive(uint8_t slave_id, FunctionCode function_code,
              const std::vector<uint8_t> &request_data) override;

private:
  std::unique_ptr<Serial> serial_;
};

} // namespace modbus

#endif // SERIAL_CLIENT_POSIX_H_
