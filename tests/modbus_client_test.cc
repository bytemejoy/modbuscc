#include "src/modbus_client.h"
#include "gtest/gtest.h"
#include <cstdint>

namespace modbus {
namespace test {

TEST(ModbusClientTest, ReadCoilsAdu) {
  std::vector<uint8_t> test_data = {0x00, 0x13, 0x00, 0x25};
  std::vector<uint8_t> expected_adu = {0x11, 0x01, 0x00, 0x13,
                                       0x00, 0x25, 0x0E, 0x84};
  std::vector<uint8_t> calculated_adu =
      BuildAdu(0x11, FunctionCode::kReadCoils, test_data);
  ASSERT_EQ(expected_adu, calculated_adu);
}

TEST(ModbusClientTest, ReadDiscreteInputsAdu) {
  std::vector<uint8_t> test_data = {0x00, 0xC4, 0x00, 0x16};
  std::vector<uint8_t> expected_adu = {0x11, 0x02, 0x00, 0xC4,
                                       0x00, 0x16, 0xBA, 0xA9};
  std::vector<uint8_t> calculated_adu =
      BuildAdu(0x11, FunctionCode::kReadDiscreteInputs, test_data);
  ASSERT_EQ(expected_adu, calculated_adu);
}

TEST(ModbusClientTest, ReadHoldingRegistersAdu) {
  std::vector<uint8_t> test_data = {0x00, 0x6B, 0x00, 0x03};
  std::vector<uint8_t> expected_adu = {0x11, 0x03, 0x00, 0x6B,
                                       0x00, 0x03, 0x76, 0x87};
  std::vector<uint8_t> calculated_adu =
      BuildAdu(0x11, FunctionCode::kReadHoldingRegisters, test_data);
  ASSERT_EQ(expected_adu, calculated_adu);
}

TEST(ModbusClientTest, ReadInputRegistersAdu) {
  std::vector<uint8_t> test_data = {0x00, 0x08, 0x00, 0x01};
  std::vector<uint8_t> expected_adu = {0x11, 0x04, 0x00, 0x08,
                                       0x00, 0x01, 0xB2, 0x98};
  std::vector<uint8_t> calculated_adu =
      BuildAdu(0x11, FunctionCode::kReadInputRegisters, test_data);
  ASSERT_EQ(expected_adu, calculated_adu);
}

TEST(ModbusClientTest, WriteSingleCoilAdu) {
  std::vector<uint8_t> test_data = {0x00, 0xAC, 0xFF, 0x00};
  std::vector<uint8_t> expected_adu = {0x11, 0x05, 0x00, 0xAC,
                                       0xFF, 0x00, 0x4E, 0x8B};
  std::vector<uint8_t> calculated_adu =
      BuildAdu(0x11, FunctionCode::kWriteSingleCoil, test_data);
  ASSERT_EQ(expected_adu, calculated_adu);
}

TEST(ModbusClientTest, WriteSingleRegisterAdu) {
  std::vector<uint8_t> test_data = {0x00, 0x01, 0x00, 0x03};
  std::vector<uint8_t> expected_adu = {0x11, 0x06, 0x00, 0x01,
                                       0x00, 0x03, 0x9A, 0x9B};
  std::vector<uint8_t> calculated_adu =
      BuildAdu(0x11, FunctionCode::kWriteSingleRegister, test_data);
  ASSERT_EQ(expected_adu, calculated_adu);
}

TEST(ModbusClientTest, WriteMultipleCoilsAdu) {
  std::vector<uint8_t> test_data = {0x00, 0x13, 0x00, 0x0A, 0x02, 0xCD, 0x01};
  std::vector<uint8_t> expected_adu = {0x11, 0x0F, 0x00, 0x13, 0x00, 0x0A,
                                       0x02, 0xCD, 0x01, 0xBF, 0x0B};
  std::vector<uint8_t> calculated_adu =
      BuildAdu(0x11, FunctionCode::kWriteMultipleCoils, test_data);
  ASSERT_EQ(expected_adu, calculated_adu);
}

TEST(ModbusClientTest, WriteMultipleRegistersAdu) {
  std::vector<uint8_t> test_data = {0x00, 0x01, 0x00, 0x02, 0x04,
                                    0x00, 0x0A, 0x01, 0x02};
  std::vector<uint8_t> expected_adu = {0x11, 0x10, 0x00, 0x01, 0x00, 0x02, 0x04,
                                       0x00, 0x0A, 0x01, 0x02, 0xC6, 0xF0};
  std::vector<uint8_t> calculated_adu =
      BuildAdu(0x11, FunctionCode::kWriteMultipleRegisters, test_data);
  ASSERT_EQ(expected_adu, calculated_adu);
}

} // namespace test
} // namespace modbus
