#include "serial_client_posix.h"

#include "absl/status/statusor.h"
#include <cassert>

namespace modbus {

// --- SerialClient ---

SerialClient::SerialClient(std::unique_ptr<Serial> serial, int timeout_ms)
    : Client(timeout_ms), serial_(std::move(serial)) {}

absl::StatusOr<std::vector<uint8_t>>
SerialClient::SendReceive(uint8_t slave_id, FunctionCode function_code,
                          const std::vector<uint8_t> &request_data) {
  // Build the Modbus ADU.
  std::vector<uint8_t> adu = BuildAdu(slave_id, function_code, request_data);

  // Send the request.
  auto status = serial_->Write(adu.data(), adu.size());
  if (!status.ok()) {
    return status;
  }

  // Read the response.
  std::vector<uint8_t> response_buffer(256); // Maximum ADU size
  absl::StatusOr<size_t> bytes_read = serial_->Read(
      response_buffer.data(), response_buffer.size(), timeout_ms_);
  if (!bytes_read.ok()) {
    return bytes_read.status();
  }

  if (bytes_read.value() < 3) {
    // Minimum response size: slave address + function code + error code
    return absl::InternalError("Modbus response too short.");
  }

  // Verify CRC.
  uint16_t received_crc =
      (static_cast<uint16_t>(response_buffer[bytes_read.value() - 1]) << 8) |
      static_cast<uint16_t>(response_buffer[bytes_read.value() - 2]);
  std::vector<uint8_t> response_data_for_crc(response_buffer.begin(),
                                             response_buffer.begin() +
                                                 bytes_read.value() - 2);
  if (received_crc != CalculateCrc16(response_data_for_crc)) {
    return absl::DataLossError("Modbus CRC mismatch.");
  }
  // Extract the PDU data from the response.
  std::vector<uint8_t> response_pdu(response_buffer.begin() + 2,
                                    response_buffer.begin() +
                                        bytes_read.value() - 2);
  return response_pdu;
}

} // namespace modbus
