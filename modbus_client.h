#ifndef MODBUS_CLIENT_H_
#define MODBUS_CLIENT_H_

#include <cstdint>
#include <vector>

#include "absl/status/statusor.h"

namespace modbus {

// Enum class representing different Modbus function codes.
enum class FunctionCode : uint8_t {
  kReadCoils = 0x01,
  kReadDiscreteInputs = 0x02,
  kReadHoldingRegisters = 0x03,
  kReadInputRegisters = 0x04,
  kWriteSingleCoil = 0x05,
  kWriteSingleRegister = 0x06,
  kWriteMultipleCoils = 0x0F,
  kWriteMultipleRegisters = 0x10
};

// Enum class representing common Modbus exception codes.
enum class ExceptionCode : uint8_t {
  kIllegalFunction = 0x01,
  kIllegalDataAddress = 0x02,
  kIllegalDataValue = 0x03,
  kServerDeviceFailure = 0x04,
  kAcknowledge = 0x05,
  kServerDeviceBusy = 0x06,
  kMemoryParityError = 0x08,
  kGatewayPathUnavailable = 0x0A,
  kGatewayTargetDeviceFailedToRespond = 0x0B
};

// Abstract base class for a Modbus client.
class Client {
public:
  // Constructor taking the timeout in milliseconds.
  explicit Client(int timeout_ms) : timeout_ms_(timeout_ms) {}

  virtual ~Client() = default;

  // Method to update the timeout.
  void SetTimeout(int timeout_ms) { timeout_ms_ = timeout_ms; }

  // Sends a Modbus request and receives the response.
  // 'slave_id' is the Modbus slave ID (1-247).
  // 'function_code' is the Modbus function code.
  // 'request_data' is the request PDU data, without the slave ID and
  // function code.
  // Returns the response PDU data, without the slave ID and function code.
  virtual absl::StatusOr<std::vector<uint8_t>>
  SendReceive(uint8_t slave_id, FunctionCode function_code,
              const std::vector<uint8_t> &request_data) = 0;

  // Calculates the Modbus CRC16 checksum of the provided data.
  static uint16_t CalculateCrc16(const std::vector<uint8_t> &data);

protected:
  // Constructs a Modbus ADU (slave address + function code + data + CRC) from
  // provided parameters.
  static std::vector<uint8_t> BuildAdu(uint8_t slave_id,
                                       FunctionCode function_code,
                                       const std::vector<uint8_t> &data);

  // Timeout for Modbus communication in milliseconds.
  int timeout_ms_;
};

} // namespace modbus

#endif // MODBUS_CLIENT_H_
