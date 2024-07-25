#include "modbus_client.h"
#include "gtest/gtest.h"

namespace modbus {
namespace test {

TEST(ModbusClientTest, CalculateCrc16) {
  std::vector<uint8_t> test_data = {0x11, 0x01, 0x00, 0x08, 0x00, 0x08};
  uint16_t expected_crc = 0xA5A8;
  uint16_t calculated_crc = Client::CalculateCrc16(test_data);
  ASSERT_EQ(expected_crc, calculated_crc);
}

} // namespace test
} // namespace modbus
