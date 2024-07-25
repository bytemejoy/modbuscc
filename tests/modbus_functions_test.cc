#include "modbus_functions.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace modbus {
namespace test {

class MockClient : public Client {
public:
  explicit MockClient(int timeout_ms) : Client(timeout_ms) {}
  MOCK_METHOD(absl::StatusOr<std::vector<uint8_t>>, SendReceive,
              (uint8_t slave_id, FunctionCode function_code,
               const std::vector<uint8_t> &request_data),
              (override));
};

// --- Test ReadCoils ---
TEST(ModbusFunctionsTest, ReadCoils_Success) {
  auto mock_client = std::make_unique<MockClient>(1000);
  EXPECT_CALL(*mock_client,
              SendReceive(1, FunctionCode::kReadCoils,
                          (std::vector<uint8_t>{0x00, 0x00, 0x00, 0x08})))
      .WillOnce(testing::Return(std::vector<uint8_t>{0x01, 0xCD}));

  auto result = ReadCoils(mock_client.get(), 1, 0, 8);
  ASSERT_TRUE(result.ok());
  ASSERT_THAT(result.value(), testing::ElementsAre(true, false, true, true,
                                                   false, false, true, true));
}

TEST(ModbusFunctionsTest, ReadCoils_Error) {
  auto mock_client = std::make_unique<MockClient>(1000);
  EXPECT_CALL(*mock_client,
              SendReceive(1, FunctionCode::kReadCoils, testing::_))
      .WillOnce(testing::Return(absl::InternalError("Timeout Error")));

  auto result = ReadCoils(mock_client.get(), 1, 0, 8);
  ASSERT_FALSE(result.ok());
  ASSERT_EQ(result.status().code(), absl::StatusCode::kInternal);
}

TEST(ModbusFunctionsTest, ReadCoils_Exception) {
  auto mock_client = std::make_unique<MockClient>(1000);
  EXPECT_CALL(*mock_client,
              SendReceive(1, FunctionCode::kReadCoils, testing::_))
      .WillOnce(testing::Return(
          std::vector<uint8_t>{0x81, 0x02})); // Exception response

  auto result = ReadCoils(mock_client.get(), 1, 0, 8);
  ASSERT_FALSE(result.ok());
  ASSERT_EQ(result.status().code(), absl::StatusCode::kInternal);
}

TEST(ModbusFunctionsTest, ReadCoils_InvalidQuantity) {
  auto mock_client = std::make_unique<MockClient>(1000);
  auto result = ReadCoils(mock_client.get(), 1, 0, 0);
  ASSERT_FALSE(result.ok());
  ASSERT_EQ(result.status().code(), absl::StatusCode::kInvalidArgument);

  result = ReadCoils(mock_client.get(), 1, 0, 2001);
  ASSERT_FALSE(result.ok());
  ASSERT_EQ(result.status().code(), absl::StatusCode::kInvalidArgument);
}

// --- Test ReadDiscreteInputs ---
TEST(ModbusFunctionsTest, ReadDiscreteInputs_Success) {
  auto mock_client = std::make_unique<MockClient>(1000);
  EXPECT_CALL(*mock_client,
              SendReceive(1, FunctionCode::kReadDiscreteInputs,
                          (std::vector<uint8_t>{0x00, 0x0A, 0x00, 0x05})))
      .WillOnce(testing::Return(std::vector<uint8_t>{0x01, 0xA5}));

  auto result = ReadDiscreteInputs(mock_client.get(), 1, 10, 5);
  ASSERT_TRUE(result.ok());
  ASSERT_THAT(result.value(),
              testing::ElementsAre(true, false, true, false, true));
}

TEST(ModbusFunctionsTest, ReadDiscreteInputs_Error) {
  auto mock_client = std::make_unique<MockClient>(1000);
  EXPECT_CALL(*mock_client,
              SendReceive(1, FunctionCode::kReadDiscreteInputs, testing::_))
      .WillOnce(testing::Return(absl::InternalError("Timeout Error")));

  auto result = ReadDiscreteInputs(mock_client.get(), 1, 10, 5);
  ASSERT_FALSE(result.ok());
  ASSERT_EQ(result.status().code(), absl::StatusCode::kInternal);
}

TEST(ModbusFunctionsTest, ReadDiscreteInputs_Exception) {
  auto mock_client = std::make_unique<MockClient>(1000);
  EXPECT_CALL(*mock_client,
              SendReceive(1, FunctionCode::kReadDiscreteInputs, testing::_))
      .WillOnce(testing::Return(
          std::vector<uint8_t>{0x82, 0x02})); // Exception response

  auto result = ReadDiscreteInputs(mock_client.get(), 1, 10, 5);
  ASSERT_FALSE(result.ok());
  ASSERT_EQ(result.status().code(), absl::StatusCode::kInternal);
}

TEST(ModbusFunctionsTest, ReadDiscreteInputs_InvalidQuantity) {
  auto mock_client = std::make_unique<MockClient>(1000);
  auto result = ReadDiscreteInputs(mock_client.get(), 1, 10, 0);
  ASSERT_FALSE(result.ok());
  ASSERT_EQ(result.status().code(), absl::StatusCode::kInvalidArgument);

  result = ReadDiscreteInputs(mock_client.get(), 1, 10, 2001);
  ASSERT_FALSE(result.ok());
  ASSERT_EQ(result.status().code(), absl::StatusCode::kInvalidArgument);
}

// --- Test ReadHoldingRegisters ---
TEST(ModbusFunctionsTest, ReadHoldingRegisters_Success) {
  auto mock_client = std::make_unique<MockClient>(1000);
  EXPECT_CALL(*mock_client,
              SendReceive(1, FunctionCode::kReadHoldingRegisters,
                          (std::vector<uint8_t>{0x00, 0x01, 0x00, 0x03})))
      .WillOnce(testing::Return(
          std::vector<uint8_t>{0x06, 0x00, 0x01, 0x00, 0x02, 0x00, 0x03}));

  auto result = ReadHoldingRegisters(mock_client.get(), 1, 1, 3);
  ASSERT_TRUE(result.ok());
  ASSERT_THAT(result.value(), testing::ElementsAre(1, 2, 3));
}

TEST(ModbusFunctionsTest, ReadHoldingRegisters_Error) {
  auto mock_client = std::make_unique<MockClient>(1000);
  EXPECT_CALL(*mock_client,
              SendReceive(1, FunctionCode::kReadHoldingRegisters, testing::_))
      .WillOnce(testing::Return(absl::InternalError("Timeout Error")));

  auto result = ReadHoldingRegisters(mock_client.get(), 1, 1, 3);
  ASSERT_FALSE(result.ok());
  ASSERT_EQ(result.status().code(), absl::StatusCode::kInternal);
}

TEST(ModbusFunctionsTest, ReadHoldingRegisters_Exception) {
  auto mock_client = std::make_unique<MockClient>(1000);
  EXPECT_CALL(*mock_client,
              SendReceive(1, FunctionCode::kReadHoldingRegisters, testing::_))
      .WillOnce(testing::Return(
          std::vector<uint8_t>{0x83, 0x02})); // Exception response

  auto result = ReadHoldingRegisters(mock_client.get(), 1, 1, 3);
  ASSERT_FALSE(result.ok());
  ASSERT_EQ(result.status().code(), absl::StatusCode::kInternal);
}

TEST(ModbusFunctionsTest, ReadHoldingRegisters_InvalidQuantity) {
  auto mock_client = std::make_unique<MockClient>(1000);
  auto result = ReadHoldingRegisters(mock_client.get(), 1, 1, 0);
  ASSERT_FALSE(result.ok());
  ASSERT_EQ(result.status().code(), absl::StatusCode::kInvalidArgument);

  result = ReadHoldingRegisters(mock_client.get(), 1, 1, 126);
  ASSERT_FALSE(result.ok());
  ASSERT_EQ(result.status().code(), absl::StatusCode::kInvalidArgument);
}

// --- Test ReadInputRegisters ---
TEST(ModbusFunctionsTest, ReadInputRegisters_Success) {
  auto mock_client = std::make_unique<MockClient>(1000);
  EXPECT_CALL(*mock_client,
              SendReceive(1, FunctionCode::kReadInputRegisters,
                          (std::vector<uint8_t>{0x00, 0x02, 0x00, 0x02})))
      .WillOnce(
          testing::Return(std::vector<uint8_t>{0x04, 0x13, 0x88, 0x00, 0x00}));

  auto result = ReadInputRegisters(mock_client.get(), 1, 2, 2);
  ASSERT_TRUE(result.ok());
  ASSERT_THAT(result.value(), testing::ElementsAre(5000, 0));
}

TEST(ModbusFunctionsTest, ReadInputRegisters_Error) {
  auto mock_client = std::make_unique<MockClient>(1000);
  EXPECT_CALL(*mock_client,
              SendReceive(1, FunctionCode::kReadInputRegisters, testing::_))
      .WillOnce(testing::Return(absl::InternalError("Timeout Error")));

  auto result = ReadInputRegisters(mock_client.get(), 1, 2, 2);
  ASSERT_FALSE(result.ok());
  ASSERT_EQ(result.status().code(), absl::StatusCode::kInternal);
}

TEST(ModbusFunctionsTest, ReadInputRegisters_Exception) {
  auto mock_client = std::make_unique<MockClient>(1000);
  EXPECT_CALL(*mock_client,
              SendReceive(1, FunctionCode::kReadInputRegisters, testing::_))
      .WillOnce(testing::Return(
          std::vector<uint8_t>{0x84, 0x02})); // Exception response

  auto result = ReadInputRegisters(mock_client.get(), 1, 2, 2);
  ASSERT_FALSE(result.ok());
  ASSERT_EQ(result.status().code(), absl::StatusCode::kInternal);
}

TEST(ModbusFunctionsTest, ReadInputRegisters_InvalidQuantity) {
  auto mock_client = std::make_unique<MockClient>(1000);
  auto result = ReadInputRegisters(mock_client.get(), 1, 2, 0);
  ASSERT_FALSE(result.ok());
  ASSERT_EQ(result.status().code(), absl::StatusCode::kInvalidArgument);

  result = ReadInputRegisters(mock_client.get(), 1, 2, 126);
  ASSERT_FALSE(result.ok());
  ASSERT_EQ(result.status().code(), absl::StatusCode::kInvalidArgument);
}
// --- Test WriteSingleCoil ---
TEST(ModbusFunctionsTest, WriteSingleCoil_Success) {
  auto mock_client = std::make_unique<MockClient>(1000);
  EXPECT_CALL(*mock_client,
              SendReceive(1, FunctionCode::kWriteSingleCoil,
                          (std::vector<uint8_t>{0x00, 0x13, 0xFF, 0x00})))
      .WillOnce(testing::Return(std::vector<uint8_t>{0x00, 0x13, 0xFF, 0x00}));

  auto result = WriteSingleCoil(mock_client.get(), 1, 19, true);
  ASSERT_TRUE(result.ok());
}

TEST(ModbusFunctionsTest, WriteSingleCoil_Error) {
  auto mock_client = std::make_unique<MockClient>(1000);
  EXPECT_CALL(*mock_client,
              SendReceive(1, FunctionCode::kWriteSingleCoil, testing::_))
      .WillOnce(testing::Return(absl::InternalError("Timeout Error")));

  auto result = WriteSingleCoil(mock_client.get(), 1, 19, true);
  ASSERT_FALSE(result.ok());
  ASSERT_EQ(result.code(), absl::StatusCode::kInternal);
}

TEST(ModbusFunctionsTest, WriteSingleCoil_Exception) {
  auto mock_client = std::make_unique<MockClient>(1000);
  EXPECT_CALL(*mock_client,
              SendReceive(1, FunctionCode::kWriteSingleCoil, testing::_))
      .WillOnce(testing::Return(
          std::vector<uint8_t>{0x85, 0x02})); // Exception response

  auto result = WriteSingleCoil(mock_client.get(), 1, 19, true);
  ASSERT_FALSE(result.ok());
  ASSERT_EQ(result.code(), absl::StatusCode::kInternal);
}

// --- Test WriteSingleRegister ---
TEST(ModbusFunctionsTest, WriteSingleRegister_Success) {
  auto mock_client = std::make_unique<MockClient>(1000);
  EXPECT_CALL(*mock_client,
              SendReceive(1, FunctionCode::kWriteSingleRegister,
                          (std::vector<uint8_t>{0x00, 0x02, 0x13, 0x88})))
      .WillOnce(testing::Return(std::vector<uint8_t>{0x00, 0x02, 0x13, 0x88}));

  auto result = WriteSingleRegister(mock_client.get(), 1, 2, 5000);
  ASSERT_TRUE(result.ok());
}

TEST(ModbusFunctionsTest, WriteSingleRegister_Error) {
  auto mock_client = std::make_unique<MockClient>(1000);
  EXPECT_CALL(*mock_client,
              SendReceive(1, FunctionCode::kWriteSingleRegister, testing::_))
      .WillOnce(testing::Return(absl::InternalError("Timeout Error")));

  auto result = WriteSingleRegister(mock_client.get(), 1, 2, 5000);
  ASSERT_FALSE(result.ok());
  ASSERT_EQ(result.code(), absl::StatusCode::kInternal);
}

TEST(ModbusFunctionsTest, WriteSingleRegister_Exception) {
  auto mock_client = std::make_unique<MockClient>(1000);
  EXPECT_CALL(*mock_client,
              SendReceive(1, FunctionCode::kWriteSingleRegister, testing::_))
      .WillOnce(testing::Return(
          std::vector<uint8_t>{0x86, 0x02})); // Exception response

  auto result = WriteSingleRegister(mock_client.get(), 1, 2, 5000);
  ASSERT_FALSE(result.ok());
  ASSERT_EQ(result.code(), absl::StatusCode::kInternal);
}

// --- Test WriteMultipleCoils ---
TEST(ModbusFunctionsTest, WriteMultipleCoils_Success) {
  auto mock_client = std::make_unique<MockClient>(1000);
  EXPECT_CALL(
      *mock_client,
      SendReceive(1, FunctionCode::kWriteMultipleCoils,
                  (std::vector<uint8_t>{0x00, 0x0F, 0x00, 0x05, 0x01, 0xA5})))
      .WillOnce(testing::Return(std::vector<uint8_t>{0x00, 0x0F, 0x00, 0x05}));

  std::vector<bool> coilValues{true, false, true, false, true};
  auto result = WriteMultipleCoils(mock_client.get(), 1, 15, coilValues);
  ASSERT_TRUE(result.ok());
}

TEST(ModbusFunctionsTest, WriteMultipleCoils_Error) {
  auto mock_client = std::make_unique<MockClient>(1000);
  EXPECT_CALL(*mock_client,
              SendReceive(1, FunctionCode::kWriteMultipleCoils, testing::_))
      .WillOnce(testing::Return(absl::InternalError("Timeout Error")));

  std::vector<bool> coilValues{true, false, true, false, true};
  auto result = WriteMultipleCoils(mock_client.get(), 1, 15, coilValues);
  ASSERT_FALSE(result.ok());
  ASSERT_EQ(result.code(), absl::StatusCode::kInternal);
}

TEST(ModbusFunctionsTest, WriteMultipleCoils_Exception) {
  auto mock_client = std::make_unique<MockClient>(1000);
  EXPECT_CALL(*mock_client,
              SendReceive(1, FunctionCode::kWriteMultipleCoils, testing::_))
      .WillOnce(testing::Return(
          std::vector<uint8_t>{0x8F, 0x02})); // Exception response

  std::vector<bool> coilValues{true, false, true, false, true};
  auto result = WriteMultipleCoils(mock_client.get(), 1, 15, coilValues);
  ASSERT_FALSE(result.ok());
  ASSERT_EQ(result.code(), absl::StatusCode::kInternal);
}

TEST(ModbusFunctionsTest, WriteMultipleCoils_InvalidInput) {
  auto mock_client = std::make_unique<MockClient>(1000);
  std::vector<bool> coilValues;
  auto result = WriteMultipleCoils(mock_client.get(), 1, 15, coilValues);
  ASSERT_FALSE(result.ok());
  ASSERT_EQ(result.code(), absl::StatusCode::kInvalidArgument);

  coilValues.resize(1969, true);
  result = WriteMultipleCoils(mock_client.get(), 1, 15, coilValues);
  ASSERT_FALSE(result.ok());
  ASSERT_EQ(result.code(), absl::StatusCode::kInvalidArgument);
}

// --- Test WriteMultipleRegisters ---
TEST(ModbusFunctionsTest, WriteMultipleRegisters_Success) {
  auto mock_client = std::make_unique<MockClient>(1000);
  EXPECT_CALL(
      *mock_client,
      SendReceive(1, FunctionCode::kWriteMultipleRegisters,
                  (std::vector<uint8_t>{0x00, 0x01, 0x00, 0x02, 0x04, 0x00,
                                        0x0A, 0x01, 0x90, 0x00, 0x00})))
      .WillOnce(testing::Return(std::vector<uint8_t>{0x00, 0x01, 0x00, 0x02}));

  std::vector<uint16_t> registerValues{10, 4000};
  auto result = WriteMultipleRegisters(mock_client.get(), 1, 1, registerValues);
  ASSERT_TRUE(result.ok());
}

TEST(ModbusFunctionsTest, WriteMultipleRegisters_Error) {
  auto mock_client = std::make_unique<MockClient>(1000);
  EXPECT_CALL(*mock_client,
              SendReceive(1, FunctionCode::kWriteMultipleRegisters, testing::_))
      .WillOnce(testing::Return(absl::InternalError("Timeout Error")));

  std::vector<uint16_t> registerValues{10, 4000};
  auto result = WriteMultipleRegisters(mock_client.get(), 1, 1, registerValues);
  ASSERT_FALSE(result.ok());
  ASSERT_EQ(result.code(), absl::StatusCode::kInternal);
}

TEST(ModbusFunctionsTest, WriteMultipleRegisters_Exception) {
  auto mock_client = std::make_unique<MockClient>(1000);
  EXPECT_CALL(*mock_client,
              SendReceive(1, FunctionCode::kWriteMultipleRegisters, testing::_))
      .WillOnce(testing::Return(
          std::vector<uint8_t>{0x90, 0x02})); // Exception response

  std::vector<uint16_t> registerValues{10, 4000};
  auto result = WriteMultipleRegisters(mock_client.get(), 1, 1, registerValues);
  ASSERT_FALSE(result.ok());
  ASSERT_EQ(result.code(), absl::StatusCode::kInternal);
}

TEST(ModbusFunctionsTest, WriteMultipleRegisters_InvalidInput) {
  auto mock_client = std::make_unique<MockClient>(1000);
  std::vector<uint16_t> registerValues;
  auto result = WriteMultipleRegisters(mock_client.get(), 1, 1, registerValues);
  ASSERT_FALSE(result.ok());
  ASSERT_EQ(result.code(), absl::StatusCode::kInvalidArgument);

  registerValues.resize(124, 10);
  result = WriteMultipleRegisters(mock_client.get(), 1, 1, registerValues);
  ASSERT_FALSE(result.ok());
  ASSERT_EQ(result.code(), absl::StatusCode::kInvalidArgument);
}

} // namespace test
} // namespace modbus
