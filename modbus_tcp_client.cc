#include "modbus_tcp_client.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cassert>
#include <cstring>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

namespace modbus {

TcpClient::TcpClient(const std::string &hostname, int port, int timeout_ms)
    : Client(timeout_ms), hostname_(hostname), port_(port) {}

TcpClient::~TcpClient() { Disconnect().IgnoreError(); }

absl::Status TcpClient::Connect() {
  if (sockfd_ >= 0) {
    return absl::FailedPreconditionError("Already connected to server.");
  }

  // Create socket
  sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd_ < 0) {
    return absl::InternalError("Failed to create socket.");
  }

  // Resolve hostname to IP address
  struct hostent *server = gethostbyname(hostname_.c_str());
  if (server == nullptr) {
    close(sockfd_);
    sockfd_ = -1;
    return absl::InternalError("Failed to resolve hostname.");
  }

  // Prepare server address structure
  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
  serv_addr.sin_port = htons(port_);

  // Connect to server
  if (connect(sockfd_, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    close(sockfd_);
    sockfd_ = -1;
    return absl::InternalError("Failed to connect to server.");
  }

  return absl::OkStatus();
}

absl::Status TcpClient::Disconnect() {
  if (sockfd_ >= 0) {
    close(sockfd_);
    sockfd_ = -1;
  }
  return absl::OkStatus();
}

absl::StatusOr<std::vector<uint8_t>>
TcpClient::SendReceive(uint8_t slave_id, FunctionCode function_code,
                       const std::vector<uint8_t> &request_data) {
  if (sockfd_ < 0) {
    return absl::FailedPreconditionError("Not connected to server.");
  }

  // Set receive timeout
  struct timeval tv;
  tv.tv_sec = timeout_ms_ / 1000;
  tv.tv_usec = (timeout_ms_ % 1000) * 1000;
  if (setsockopt(sockfd_, SOL_SOCKET, SO_RCVTIMEO,
                 reinterpret_cast<const char *>(&tv), sizeof(tv)) < 0) {
    return absl::InternalError("Failed to set socket receive timeout.");
  }

  // Build the Modbus TCP ADU.
  std::vector<uint8_t> adu = BuildAdu(slave_id, function_code, request_data);

  // Prepend the MBAP header (transaction ID, protocol ID, length).
  // For simplicity, we'll use fixed values for transaction ID and protocol ID.
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
  size_t total_sent = 0;
  while (total_sent < tcp_adu.size()) {
    ssize_t sent = send(sockfd_, tcp_adu.data() + total_sent,
                        tcp_adu.size() - total_sent, 0);
    if (sent < 0) {
      return absl::InternalError("Failed to send data to server.");
    }
    total_sent += sent;
  }

  // Receive the response.
  // First, receive the MBAP header (7 bytes).
  uint8_t mbap_header[7];
  ssize_t received = recv(sockfd_, mbap_header, 7, 0);
  if (received != 7) {
    return absl::InternalError("Failed to receive MBAP header.");
  }

  // Extract the PDU length from the header.
  uint16_t pdu_length = (mbap_header[4] << 8) | mbap_header[5];

  // Receive the remaining PDU data.
  std::vector<uint8_t> response_pdu(pdu_length);
  total_sent = 0;
  while (total_sent < pdu_length) {
    received = recv(sockfd_, response_pdu.data() + total_sent,
                    pdu_length - total_sent, 0);
    if (received < 0) {
      return absl::InternalError("Failed to receive PDU data.");
    }
    total_sent += received;
  }

  // Remove the unit ID from the response PDU.
  response_pdu.erase(response_pdu.begin());

  return response_pdu;
}

} // namespace modbus
