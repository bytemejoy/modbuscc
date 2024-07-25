#include "modbus_functions.h"

#include <cstdint>
#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

namespace modbus {

namespace {

// Helper function to check for exceptions in Modbus response.
absl::Status CheckForException(const std::vector<uint8_t> &response) {
  if (response.empty()) {
    return absl::InternalError("Empty Modbus response.");
  }

  if (response[0] & 0x80) {
    // Exception response
    if (response.size() >= 2) {
      ExceptionCode exception_code = static_cast<ExceptionCode>(response[1]);
      return absl::InternalError(
          "Modbus exception: " +
          std::to_string(static_cast<int>(exception_code)));
    } else {
      return absl::InternalError("Invalid Modbus exception response.");
    }
  }
  return absl::OkStatus();
}

} // namespace

// --- Read Coils ---

absl::StatusOr<std::vector<bool>> ReadCoils(Client *client, uint8_t slave_id,
                                            uint16_t starting_address,
                                            uint16_t quantity) {
  if (quantity < 1 || quantity > 2000) {
    return absl::InvalidArgumentError("Invalid quantity of coils.");
  }

  std::vector<uint8_t> request = {static_cast<uint8_t>(starting_address >> 8),
                                  static_cast<uint8_t>(starting_address & 0xFF),
                                  static_cast<uint8_t>(quantity >> 8),
                                  static_cast<uint8_t>(quantity & 0xFF)};

  auto response =
      client->SendReceive(slave_id, FunctionCode::kReadCoils, request);
  if (!response.ok()) {
    return response.status();
  }

  auto status = CheckForException(response.value());
  if (!status.ok()) {
    return status;
  }

  if (response.value().size() != (2 + (quantity + 7) / 8)) {
    return absl::InternalError("Invalid response size.");
  }

  std::vector<bool> coils(quantity);
  for (size_t i = 0; i < quantity; ++i) {
    uint8_t byte = response.value()[1 + (i / 8)];
    coils[i] = (byte >> (i % 8)) & 0x01;
  }
  return coils;
}

// --- Read Discrete Inputs ---

absl::StatusOr<std::vector<bool>> ReadDiscreteInputs(Client *client,
                                                     uint8_t slave_id,
                                                     uint16_t starting_address,
                                                     uint16_t quantity) {
  if (quantity < 1 || quantity > 2000) {
    return absl::InvalidArgumentError("Invalid quantity of inputs.");
  }

  std::vector<uint8_t> request = {static_cast<uint8_t>(starting_address >> 8),
                                  static_cast<uint8_t>(starting_address & 0xFF),
                                  static_cast<uint8_t>(quantity >> 8),
                                  static_cast<uint8_t>(quantity & 0xFF)};

  auto response =
      client->SendReceive(slave_id, FunctionCode::kReadDiscreteInputs, request);
  if (!response.ok()) {
    return response.status();
  }

  auto status = CheckForException(response.value());
  if (!status.ok()) {
    return status;
  }

  if (response.value().size() != (2 + (quantity + 7) / 8)) {
    return absl::InternalError("Invalid response size.");
  }

  std::vector<bool> inputs(quantity);
  for (size_t i = 0; i < quantity; ++i) {
    uint8_t byte = response.value()[1 + (i / 8)];
    inputs[i] = (byte >> (i % 8)) & 0x01;
  }
  return inputs;
}

// --- Read Holding Registers ---

absl::StatusOr<std::vector<uint16_t>>
ReadHoldingRegisters(Client *client, uint8_t slave_id,
                     uint16_t starting_address, uint16_t quantity) {
  if (quantity < 1 || quantity > 125) {
    return absl::InvalidArgumentError("Invalid quantity of registers.");
  }

  std::vector<uint8_t> request = {static_cast<uint8_t>(starting_address >> 8),
                                  static_cast<uint8_t>(starting_address & 0xFF),
                                  static_cast<uint8_t>(quantity >> 8),
                                  static_cast<uint8_t>(quantity & 0xFF)};

  auto response = client->SendReceive(
      slave_id, FunctionCode::kReadHoldingRegisters, request);
  if (!response.ok()) {
    return response.status();
  }

  auto status = CheckForException(response.value());
  if (!status.ok()) {
    return status;
  }

  if (response.value().size() != (2 + quantity * 2)) {
    return absl::InternalError("Invalid response size.");
  }

  std::vector<uint16_t> registers(quantity);
  for (size_t i = 0; i < quantity; ++i) {
    registers[i] =
        (response.value()[1 + i * 2] << 8) | response.value()[2 + i * 2];
  }
  return registers;
}

// --- Read Input Registers ---

absl::StatusOr<std::vector<uint16_t>>
ReadInputRegisters(Client *client, uint8_t slave_id, uint16_t starting_address,
                   uint16_t quantity) {
  if (quantity < 1 || quantity > 125) {
    return absl::InvalidArgumentError("Invalid quantity of registers.");
  }

  std::vector<uint8_t> request = {static_cast<uint8_t>(starting_address >> 8),
                                  static_cast<uint8_t>(starting_address & 0xFF),
                                  static_cast<uint8_t>(quantity >> 8),
                                  static_cast<uint8_t>(quantity & 0xFF)};

  auto response =
      client->SendReceive(slave_id, FunctionCode::kReadInputRegisters, request);
  if (!response.ok()) {
    return response.status();
  }

  auto status = CheckForException(response.value());
  if (!status.ok()) {
    return status;
  }

  if (response.value().size() != (2 + quantity * 2)) {
    return absl::InternalError("Invalid response size.");
  }

  std::vector<uint16_t> registers(quantity);
  for (size_t i = 0; i < quantity; ++i) {
    registers[i] =
        (response.value()[1 + i * 2] << 8) | response.value()[2 + i * 2];
  }
  return registers;
}

// --- Write Single Coil ---

absl::Status WriteSingleCoil(Client *client, uint8_t slave_id,
                             uint16_t output_address, bool value) {
  std::vector<uint8_t> request = {static_cast<uint8_t>(output_address >> 8),
                                  static_cast<uint8_t>(output_address & 0xFF),
                                  static_cast<uint8_t>(value ? 0xFF : 0x00),
                                  static_cast<uint8_t>(0x00)};

  auto response =
      client->SendReceive(slave_id, FunctionCode::kWriteSingleCoil, request);
  if (!response.ok()) {
    return response.status();
  }

  auto status = CheckForException(response.value());
  if (!status.ok()) {
    return status;
  }

  if (response.value().size() != request.size() ||
      response.value() != request) {
    return absl::InternalError("Invalid response data.");
  }

  return absl::OkStatus();
}

// --- Write Single Register ---

absl::Status WriteSingleRegister(Client *client, uint8_t slave_id,
                                 uint16_t register_address, uint16_t value) {
  std::vector<uint8_t> request = {static_cast<uint8_t>(register_address >> 8),
                                  static_cast<uint8_t>(register_address & 0xFF),
                                  static_cast<uint8_t>(value >> 8),
                                  static_cast<uint8_t>(value & 0xFF)};

  auto response = client->SendReceive(
      slave_id, FunctionCode::kWriteSingleRegister, request);
  if (!response.ok()) {
    return response.status();
  }

  auto status = CheckForException(response.value());
  if (!status.ok()) {
    return status;
  }

  if (response.value().size() != request.size() ||
      response.value() != request) {
    return absl::InternalError("Invalid response data.");
  }

  return absl::OkStatus();
}

// --- Write Multiple Coils ---

absl::Status WriteMultipleCoils(Client *client, uint8_t slave_id,
                                uint16_t starting_address,
                                const std::vector<bool> &values) {
  if (values.empty() || values.size() > 1968) {
    return absl::InvalidArgumentError("Invalid number of coils to write.");
  }

  std::vector<uint8_t> request = {
      static_cast<uint8_t>(starting_address >> 8),
      static_cast<uint8_t>(starting_address & 0xFF),
      static_cast<uint8_t>(values.size() >> 8),
      static_cast<uint8_t>(values.size() & 0xFF),
      static_cast<uint8_t>((values.size() + 7) / 8)};

  for (size_t i = 0; i < values.size(); ++i) {
    if ((i % 8) == 0) {
      request.push_back(0x00);
    }
    if (values[i]) {
      request.back() |= (1 << (i % 8));
    }
  }

  auto response =
      client->SendReceive(slave_id, FunctionCode::kWriteMultipleCoils, request);
  if (!response.ok()) {
    return response.status();
  }

  auto status = CheckForException(response.value());
  if (!status.ok()) {
    return status;
  }

  // Response only contains starting address and quantity of coils.
  if (response.value().size() != 4 || response.value()[0] != request[0] ||
      response.value()[1] != request[1] || response.value()[2] != request[2] ||
      response.value()[3] != request[3]) {
    return absl::InternalError("Invalid response data.");
  }

  return absl::OkStatus();
}

// --- Write Multiple Registers ---

absl::Status WriteMultipleRegisters(Client *client, uint8_t slave_id,
                                    uint16_t starting_address,
                                    const std::vector<uint16_t> &values) {
  if (values.empty() || values.size() > 123) {
    return absl::InvalidArgumentError("Invalid number of registers to write.");
  }

  std::vector<uint8_t> request = {static_cast<uint8_t>(starting_address >> 8),
                                  static_cast<uint8_t>(starting_address & 0xFF),
                                  static_cast<uint8_t>(values.size() >> 8),
                                  static_cast<uint8_t>(values.size() & 0xFF),
                                  static_cast<uint8_t>(values.size() * 2)};

  for (uint16_t value : values) {
    request.push_back(static_cast<uint8_t>(value >> 8));
    request.push_back(static_cast<uint8_t>(value & 0xFF));
  }

  auto response = client->SendReceive(
      slave_id, FunctionCode::kWriteMultipleRegisters, request);
  if (!response.ok()) {
    return response.status();
  }

  auto status = CheckForException(response.value());
  if (!status.ok()) {
    return status;
  }

  // Response only contains starting address and quantity of registers.
  if (response.value().size() != 4 || response.value()[0] != request[0] ||
      response.value()[1] != request[1] || response.value()[2] != request[2] ||
      response.value()[3] != request[3]) {
    return absl::InternalError("Invalid response data.");
  }

  return absl::OkStatus();
}

} // namespace modbus
