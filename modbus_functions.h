#ifndef MODBUS_FUNCTIONS_H_
#define MODBUS_FUNCTIONS_H_

#include <cstdint>
#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "modbus_client.h"

namespace modbus {

// --- Read Coils (Function Code 0x01) ---

absl::StatusOr<std::vector<bool>> ReadCoils(Client *client, uint8_t slave_id,
                                            uint16_t starting_address,
                                            uint16_t quantity);

// --- Read Discrete Inputs (Function Code 0x02) ---

absl::StatusOr<std::vector<bool>> ReadDiscreteInputs(Client *client,
                                                     uint8_t slave_id,
                                                     uint16_t starting_address,
                                                     uint16_t quantity);

// --- Read Holding Registers (Function Code 0x03) ---

absl::StatusOr<std::vector<uint16_t>>
ReadHoldingRegisters(Client *client, uint8_t slave_id,
                     uint16_t starting_address, uint16_t quantity);

// --- Read Input Registers (Function Code 0x04) ---

absl::StatusOr<std::vector<uint16_t>>
ReadInputRegisters(Client *client, uint8_t slave_id, uint16_t starting_address,
                   uint16_t quantity);

// --- Write Single Coil (Function Code 0x05) ---

absl::Status WriteSingleCoil(Client *client, uint8_t slave_id,
                             uint16_t output_address, bool value);

// --- Write Single Register (Function Code 0x06) ---

absl::Status WriteSingleRegister(Client *client, uint8_t slave_id,
                                 uint16_t register_address, uint16_t value);

// --- Write Multiple Coils (Function Code 0x0F) ---

absl::Status WriteMultipleCoils(Client *client, uint8_t slave_id,
                                uint16_t starting_address,
                                const std::vector<bool> &values);

// --- Write Multiple Registers (Function Code 0x10) ---

absl::Status WriteMultipleRegisters(Client *client, uint8_t slave_id,
                                    uint16_t starting_address,
                                    const std::vector<uint16_t> &values);

} // namespace modbus

#endif // MODBUS_FUNCTIONS_H_
