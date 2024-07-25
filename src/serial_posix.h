#ifndef SERIAL_POSIX_H_
#define SERIAL_POSIX_H_

#include "serial.h"

namespace modbus {

// Concrete implementation of Serial for POSIX.
class SerialPosix : public Serial {
public:
  ~SerialPosix() override;

  absl::Status Open(const SerialParams &params) override;
  absl::Status Close() override;
  absl::Status Write(const uint8_t *data, size_t length) override;
  absl::StatusOr<size_t> Read(uint8_t *buffer, size_t length,
                              int timeout_ms) override;

private:
  int fd_ = -1;
};

} // namespace modbus

#endif // SERIAL_POSIX_H_
