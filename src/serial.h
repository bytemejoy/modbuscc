#ifndef SERIAL_H_
#define SERIAL_H_

#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

namespace modbus {

// Enum class representing different Modbus serial parity options.
enum class Parity { kNone, kEven, kOdd };

// Struct representing Modbus serial port configuration.
struct SerialParams {
  std::string port;
  int baud_rate;
  Parity parity;
  int data_bits;
  int stop_bits;
};

// Abstract base class for Modbus serial connection.
class Serial {
public:
  virtual ~Serial() = default;

  // Opens the serial port with the provided configuration.
  virtual absl::Status Open(const SerialParams &params) = 0;

  // Closes the serial port.
  virtual absl::Status Close() = 0;

  // Writes data to the serial port.
  virtual absl::Status Write(const uint8_t *data, size_t length) = 0;

  // Reads data from the serial port, waiting for at most 'timeout_ms'
  // milliseconds. Returns the number of bytes read.
  virtual absl::StatusOr<size_t> Read(uint8_t *buffer, size_t length,
                                      int timeout_ms) = 0;
};

} // namespace modbus

#endif // SERIAL_H_
