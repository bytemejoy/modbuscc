#include "serial_posix.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

namespace modbus {

namespace {

// Converts Parity enum to termios parity value.
tcflag_t ParityToTermios(Parity parity) {
  switch (parity) {
  case Parity::kNone:
    return 0;
  case Parity::kEven:
    return PARENB;
  case Parity::kOdd:
    return PARENB | PARODD;
  default:
    return -1;
  }
}

} // namespace

SerialPosix::~SerialPosix() { Close().IgnoreError(); }

absl::Status SerialPosix::Open(const SerialParams &params) {
  if (fd_ >= 0) {
    return absl::FailedPreconditionError("Serial port already open.");
  }

  fd_ = open(params.port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd_ < 0) {
    return absl::InternalError("Failed to open serial port.");
  }

  // Configure serial port parameters.
  struct termios tty;
  tcgetattr(fd_, &tty);

  cfsetospeed(&tty, params.baud_rate);
  cfsetispeed(&tty, params.baud_rate);

  tty.c_cflag = CS8 | CLOCAL | CREAD;
  tty.c_iflag = IGNPAR;
  tty.c_oflag = 0;
  tty.c_lflag = 0;
  tty.c_cc[VMIN] = 0;
  tty.c_cc[VTIME] = 0;

  tcsetattr(fd_, TCSANOW, &tty);

  // Set parity.
  if (auto parity = ParityToTermios(params.parity); parity < 0) {
    return absl::InvalidArgumentError("Invalid parity value.");
  } else {
    tty.c_cflag |= parity;
  }

  // Set data bits.
  tty.c_cflag &= ~CSIZE;
  switch (params.data_bits) {
  case 5:
    tty.c_cflag |= CS5;
    break;
  case 6:
    tty.c_cflag |= CS6;
    break;
  case 7:
    tty.c_cflag |= CS7;
    break;
  case 8:
    tty.c_cflag |= CS8;
    break;
  default:
    return absl::InvalidArgumentError("Invalid data bits value.");
  }

  // Set stop bits.
  if (params.stop_bits == 2) {
    tty.c_cflag |= CSTOPB;
  } else if (params.stop_bits != 1) {
    return absl::InvalidArgumentError("Invalid stop bits value.");
  }

  tcsetattr(fd_, TCSANOW, &tty);

  return absl::OkStatus();
}

absl::Status SerialPosix::Close() {
  if (fd_ >= 0) {
    close(fd_);
    fd_ = -1;
  }
  return absl::OkStatus();
}

absl::Status SerialPosix::Write(const uint8_t *data, size_t length) {
  if (fd_ < 0) {
    return absl::FailedPreconditionError("Serial port not open.");
  }
  ssize_t bytes_written = write(fd_, data, length);
  if (bytes_written != length) {
    return absl::InternalError("Failed to write to serial port.");
  }
  return absl::OkStatus();
}

absl::StatusOr<size_t> SerialPosix::Read(uint8_t *buffer, size_t length,
                                         int timeout_ms) {
  if (fd_ < 0) {
    return absl::FailedPreconditionError("Serial port not open.");
  }

  fd_set read_fds;
  FD_ZERO(&read_fds);
  FD_SET(fd_, &read_fds);

  struct timeval timeout;
  timeout.tv_sec = timeout_ms / 1000;
  timeout.tv_usec = (timeout_ms % 1000) * 1000;

  int result = select(fd_ + 1, &read_fds, nullptr, nullptr, &timeout);
  if (result < 0) {
    return absl::InternalError("Error during serial read.");
  } else if (result == 0) {
    return 0; // Timeout
  } else {
    ssize_t bytes_read = read(fd_, buffer, length);
    if (bytes_read < 0) {
      return absl::InternalError("Failed to read from serial port.");
    }
    return bytes_read;
  }
}

} // namespace modbus
