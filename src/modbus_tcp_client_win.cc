#include "modbus_tcp_client_win.h"

#include <absl/status/statusor.h>

#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

namespace modbus {

namespace {

// Default timeout for Modbus TCP communication (in milliseconds).
constexpr int kDefaultTcpTimeoutMs = 1000;

} // namespace

TcpClientWin::TcpClientWin(const std::string &hostname, int port,
                           int timeout_ms)
    : Client(timeout_ms), hostname_(hostname), port_(port) {}

TcpClientWin::~TcpClientWin() { Disconnect(); }

absl::Status TcpClientWin::Connect() {
  if (sockfd_ != INVALID_SOCKET) {
    return absl::FailedPreconditionError("Already connected to server.");
  }

  // Initialize Winsock
  WSADATA wsaData;
  int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (result != 0) {
    return absl::InternalError("WSAStartup failed.");
  }

  // Resolve hostname to IP address
  struct addrinfo hints;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  std::string port_str = std::to_string(port_);
  result =
      getaddrinfo(hostname_.c_str(), port_str.c_str(), &hints, &server_info_);
  if (result != 0) {
    WSACleanup();
    return absl::InternalError("getaddrinfo failed.");
  }

  // Create socket
  sockfd_ = socket(server_info_->ai_family, server_info_->ai_socktype,
                   server_info_->ai_protocol);
  if (sockfd_ == INVALID_SOCKET) {
    freeaddrinfo(server_info_);
    WSACleanup();
    return absl::InternalError("Failed to create socket.");
  }

  // Connect to server
  result = connect(sockfd_, server_info_->ai_addr,
                   static_cast<int>(server_info_->ai_addrlen));
  if (result == SOCKET_ERROR) {
    freeaddrinfo(server_info_);
    closesocket(sockfd_);
    WSACleanup();
    sockfd_ = INVALID_SOCKET;
    return absl::InternalError("Failed to connect to server.");
  }

  return absl::OkStatus();
}

absl::Status TcpClientWin::Disconnect() {
  if (sockfd_ != INVALID_SOCKET) {
    closesocket(sockfd_);
    sockfd_ = INVALID_SOCKET;
    freeaddrinfo(server_info_);
    WSACleanup();
  }
  return absl::OkStatus();
}

absl::StatusOr<std::vector<uint8_t>>
TcpClientWin::SendReceive(uint8_t slave_id, FunctionCode function_code,
                          const std::vector<uint8_t> &request_data) {
  if (sockfd_ == INVALID_SOCKET) {
    return absl::FailedPreconditionError("Not connected to server.");
  }

  // Set receive timeout
  DWORD timeout = static_cast<DWORD>(timeout_ms_);
  if (setsockopt(sockfd_, SOL_SOCKET, SO_RCVTIMEO,
                 reinterpret_cast<const char *>(&timeout),
                 sizeof(timeout)) == SOCKET_ERROR) {
    return absl::InternalError("Failed to set socket receive timeout.");
  }

  // Build the Modbus TCP ADU (same as in TcpClient).
  std::vector<uint8_t> adu = BuildAdu(slave_id, function_code, request_data);
  std::vector<uint8_t> tcp_adu = {
      0x00,
      0x01, // Transaction ID
      0x00,
      0x00, // Protocol ID
      static_cast<uint8_t>(adu.size() + 2),
      0x00,    // Length (PDU + unit ID)
      slave_id // Unit ID
  };
  tcp_adu.insert(tcp_adu.end(), adu.begin(), adu.end());

  // Send the request.
  int total_sent = 0;
  while (total_sent < tcp_adu.size()) {
    int sent = send(sockfd_,
                    reinterpret_cast<const char *>(tcp_adu.data() + total_sent),
                    static_cast<int>(tcp_adu.size() - total_sent), 0);
    if (sent == SOCKET_ERROR) {
      return absl::InternalError("Failed to send data to server.");
    }
    total_sent += sent;
  }

  // Receive the response (same as in TcpClient).
  uint8_t mbap_header[7];
  int received = recv(sockfd_, reinterpret_cast<char *>(mbap_header), 7, 0);
  if (received != 7) {
    return absl::InternalError("Failed to receive MBAP header.");
  }

  uint16_t pdu_length = (mbap_header[4] << 8) | mbap_header[5];
  std::vector<uint8_t> response_pdu(pdu_length);
  total_sent = 0;
  while (total_sent < pdu_length) {
    received = recv(sockfd_,
                    reinterpret_cast<char *>(response_pdu.data() + total_sent),
                    static_cast<int>(pdu_length - total_sent), 0);
    if (received == SOCKET_ERROR) {
      return absl::InternalError("Failed to receive PDU data.");
    }
    total_sent += received;
  }

  response_pdu.erase(response_pdu.begin()); // Remove unit ID
  return response_pdu;
}

} // namespace modbus
