#ifndef SERIAL_CLIENT_WIN_H_
#define SERIAL_CLIENT_WIN_H_

#include "serial.h"

namespace modbus {

// Concrete implementation of Serial for Windows.
class SerialWin : public Serial {
public:
  ~SerialWin() override;

  absl::Status Open(const SerialParams &params) override;
  absl::Status Close() override;
  absl::Status Write(const uint8_t *data, size_t length) override;
  absl::StatusOr<size_t> Read(uint8_t *buffer, size_t length,
                              int timeout_ms) override;

private:
  void *handle_ = nullptr;
};

} // namespace modbus

#endif // SERIAL_CLIENT_WIN_H_
