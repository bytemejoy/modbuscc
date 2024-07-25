#include "modbus_client.h"

#include <cassert>

namespace modbus {

// --- Client ---

std::vector<uint8_t> Client::BuildAdu(uint8_t slave_id,
                                      FunctionCode function_code,
                                      const std::vector<uint8_t> &data) {
  std::vector<uint8_t> adu = {slave_id, static_cast<uint8_t>(function_code)};
  adu.insert(adu.end(), data.begin(), data.end());

  uint16_t crc = CalculateCrc16(adu);
  adu.push_back(static_cast<uint8_t>(crc & 0xFF)); // Low byte
  adu.push_back(static_cast<uint8_t>(crc >> 8));   // High byte

  return adu;
}

uint16_t Client::CalculateCrc16(const std::vector<uint8_t> &data) {
  uint16_t crc = 0xFFFF;
  for (uint8_t byte : data) {
    crc ^= static_cast<uint16_t>(byte) << 8;
    for (int i = 0; i < 8; ++i) {
      if (crc & 0x8000) {
        crc = (crc << 1) ^ 0x1021;
      } else {
        crc <<= 1;
      }
    }
  }
  return crc;
}

} // namespace modbus
