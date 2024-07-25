#include "serial_client_win.h"

#include <absl/status/status.h>
#include <windows.h>

namespace modbus {

namespace {

// Converts Parity enum to Windows parity value.
BYTE ParityToWin(Parity parity) {
  switch (parity) {
  case Parity::kNone:
    return NOPARITY;
  case Parity::kEven:
    return EVENPARITY;
  case Parity::kOdd:
    return ODDPARITY;
  default:
    return -1;
  }
}

// Converts StopBits enum to Windows stop bits value.
BYTE StopBitsToWin(int stop_bits) {
  switch (stop_bits) {
  case 1:
    return ONESTOPBIT;
  case 2:
    return TWOSTOPBITS;
  default:
    return -1;
  }
}

} // namespace

SerialWin::~SerialWin() { Close().IgnoreError(); }

absl::Status SerialWin::Open(const SerialParams &params) {
  if (handle_ != nullptr) {
    return absl::FailedPreconditionError("Serial port already open.");
  }

  std::wstring wport(params.port.begin(), params.port.end());
  handle_ = CreateFileW(wport.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr,
                        OPEN_EXISTING, 0, nullptr);
  if (handle_ == INVALID_HANDLE_VALUE) {
    return absl::InternalError("Failed to open serial port.");
  }

  // Configure serial port parameters.
  DCB dcb = {0};
  dcb.DCBlength = sizeof(dcb);
  if (!GetCommState(handle_, &dcb)) {
    return absl::InternalError("Failed to get serial port state.");
  }

  dcb.BaudRate = params.baud_rate;
  dcb.Parity = ParityToWin(params.parity);
  dcb.ByteSize = params.data_bits;
  dcb.StopBits = StopBitsToWin(params.stop_bits);

  if (!SetCommState(handle_, &dcb)) {
    return absl::InternalError("Failed to set serial port state.");
  }

  // Set timeouts.
  COMMTIMEOUTS timeouts = {0};
  timeouts.ReadIntervalTimeout = MAXDWORD;
  timeouts.ReadTotalTimeoutMultiplier = 0;
  timeouts.ReadTotalTimeoutConstant = 0;
  timeouts.WriteTotalTimeoutMultiplier = 0;
  timeouts.WriteTotalTimeoutConstant = 0;
  if (!SetCommTimeouts(handle_, &timeouts)) {
    return absl::InternalError("Failed to set serial port timeouts.");
  }

  return absl::OkStatus();
}

absl::Status SerialWin::Close() {
  if (handle_ != nullptr) {
    CloseHandle(handle_);
    handle_ = nullptr;
  }
  return absl::OkStatus();
}

absl::Status SerialWin::Write(const uint8_t *data, size_t length) {
  if (handle_ == nullptr) {
    return absl::FailedPreconditionError("Serial port not open.");
  }

  DWORD bytes_written = 0;
  if (!WriteFile(handle_, data, static_cast<DWORD>(length), &bytes_written,
                 nullptr)) {
    return absl::InternalError("Failed to write to serial port.");
  }
  if (bytes_written != length) {
    return absl::InternalError("Incomplete write to serial port.");
  }
  return absl::OkStatus();
}

absl::StatusOr<size_t> SerialWin::Read(uint8_t *buffer, size_t length,
                                       int timeout_ms) {
  if (handle_ == nullptr) {
    return absl::FailedPreconditionError("Serial port not open.");
  }

  DWORD bytes_read = 0;
  COMMTIMEOUTS original_timeouts, temp_timeouts;
  if (!GetCommTimeouts(handle_, &original_timeouts)) {
    return absl::InternalError("Failed to get serial port timeouts.");
  }

  // Set temporary timeout for reading.
  temp_timeouts = original_timeouts;
  temp_timeouts.ReadTotalTimeoutConstant = timeout_ms;
  if (!SetCommTimeouts(handle_, &temp_timeouts)) {
    return absl::InternalError("Failed to set serial port timeouts.");
  }

  if (!ReadFile(handle_, buffer, static_cast<DWORD>(length), &bytes_read,
                nullptr)) {
    // Restore original timeouts even if ReadFile fails.
    SetCommTimeouts(handle_, &original_timeouts);
    return absl::InternalError("Failed to read from serial port.");
  }

  // Restore original timeouts after reading.
  if (!SetCommTimeouts(handle_, &original_timeouts)) {
    return absl::InternalError("Failed to restore serial port timeouts.");
  }

  return bytes_read;
}

} // namespace modbus
