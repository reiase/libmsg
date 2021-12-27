#ifndef _LIBMSG_UTILS_HPP_
#define _LIBMSG_UTILS_HPP_
#include <pthread.h>

#include <chrono>
#include <cstdio>
#include <sstream>
#include <string>

namespace msg {

static char gDigitsLut[200] = {
    '0', '0', '0', '1', '0', '2', '0', '3', '0', '4', '0', '5', '0', '6', '0',
    '7', '0', '8', '0', '9', '1', '0', '1', '1', '1', '2', '1', '3', '1', '4',
    '1', '5', '1', '6', '1', '7', '1', '8', '1', '9', '2', '0', '2', '1', '2',
    '2', '2', '3', '2', '4', '2', '5', '2', '6', '2', '7', '2', '8', '2', '9',
    '3', '0', '3', '1', '3', '2', '3', '3', '3', '4', '3', '5', '3', '6', '3',
    '7', '3', '8', '3', '9', '4', '0', '4', '1', '4', '2', '4', '3', '4', '4',
    '4', '5', '4', '6', '4', '7', '4', '8', '4', '9', '5', '0', '5', '1', '5',
    '2', '5', '3', '5', '4', '5', '5', '5', '6', '5', '7', '5', '8', '5', '9',
    '6', '0', '6', '1', '6', '2', '6', '3', '6', '4', '6', '5', '6', '6', '6',
    '7', '6', '8', '6', '9', '7', '0', '7', '1', '7', '2', '7', '3', '7', '4',
    '7', '5', '7', '6', '7', '7', '7', '8', '7', '9', '8', '0', '8', '1', '8',
    '2', '8', '3', '8', '4', '8', '5', '8', '6', '8', '7', '8', '8', '8', '9',
    '9', '0', '9', '1', '9', '2', '9', '3', '9', '4', '9', '5', '9', '6', '9',
    '7', '9', '8', '9', '9'};

#define BEGIN2(n)               \
  do {                          \
    int t = (n);                \
    if (t < 10)                 \
      *p++ = '0' + t;           \
    else {                      \
      t *= 2;                   \
      *p++ = gDigitsLut[t];     \
      *p++ = gDigitsLut[t + 1]; \
    }                           \
  } while (0)
#define MIDDLE2(n)            \
  do {                        \
    int t = (n)*2;            \
    *p++ = gDigitsLut[t];     \
    *p++ = gDigitsLut[t + 1]; \
  } while (0)
#define BEGIN4(n)        \
  do {                   \
    int t4 = (n);        \
    if (t4 < 100)        \
      BEGIN2(t4);        \
    else {               \
      BEGIN2(t4 / 100);  \
      MIDDLE2(t4 % 100); \
    }                    \
  } while (0)
#define MIDDLE4(n)     \
  do {                 \
    int t4 = (n);      \
    MIDDLE2(t4 / 100); \
    MIDDLE2(t4 % 100); \
  } while (0)
#define BEGIN8(n)          \
  do {                     \
    uint32_t t8 = (n);     \
    if (t8 < 10000)        \
      BEGIN4(t8);          \
    else {                 \
      BEGIN4(t8 / 10000);  \
      MIDDLE4(t8 % 10000); \
    }                      \
  } while (0)
#define MIDDLE8(n)       \
  do {                   \
    uint32_t t8 = (n);   \
    MIDDLE4(t8 / 10000); \
    MIDDLE4(t8 % 10000); \
  } while (0)
#define MIDDLE16(n)           \
  do {                        \
    uint64_t t16 = (n);       \
    MIDDLE8(t16 / 100000000); \
    MIDDLE8(t16 % 100000000); \
  } while (0)

inline void u32toa_branchlut2(uint32_t x, char* p) {
  if (x < 100000000)
    BEGIN8(x);
  else {
    BEGIN2(x / 100000000);
    MIDDLE8(x % 100000000);
  }
  *p = 0;
}

inline void i32toa_branchlut2(int32_t x, char* p) {
  uint64_t t;
  if (x >= 0)
    t = x;
  else
    *p++ = '-', t = -uint32_t(x);
  u32toa_branchlut2(t, p);
}

inline void u64toa_branchlut2(uint64_t x, char* p) {
  if (x < 100000000)
    BEGIN8(x);
  else if (x < 10000000000000000) {
    BEGIN8(x / 100000000);
    MIDDLE8(x % 100000000);
  } else {
    BEGIN4(x / 10000000000000000);
    MIDDLE16(x % 10000000000000000);
  }
  *p = 0;
}

inline void i64toa_branchlut2(int64_t x, char* p) {
  uint64_t t;
  if (x >= 0)
    t = x;
  else
    *p++ = '-', t = -uint64_t(x);
  u64toa_branchlut2(t, p);
}

template <typename T>
std::string ToString(T x) {
  std::stringstream ss;
  ss << x;
  return ss.str();
}

template <>
inline std::string ToString<int32_t>(int32_t x) {
  char buffer[256];
  i32toa_branchlut2(x, buffer);
  return buffer;
}

template <>
inline std::string ToString<int64_t>(int64_t x) {
  char buffer[256];
  i64toa_branchlut2(x, buffer);
  return buffer;
}

template <>
inline std::string ToString<uint32_t>(uint32_t x) {
  char buffer[256];
  u32toa_branchlut2(x, buffer);
  return buffer;
}

template <>
inline std::string ToString<uint64_t>(uint64_t x) {
  char buffer[256];
  u64toa_branchlut2(x, buffer);
  return buffer;
}

template <typename T>
T StringToNum(const std::string& str) {
  std::istringstream iss(str);
  T num;
  iss >> num;
  return num;
}

inline std::string& trim(std::string& s) {
  if (!s.empty()) {
    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
  }
  return s;
}

int simple_http_post(const std::string& host, const std::string& port,
                     const std::string& query_path, const std::string& data);

std::string simple_http_get(const std::string& host, const std::string& port,
                            const std::string& query_path);

inline std::size_t get_timestamp_us() {
  return std::chrono::duration_cast<std::chrono::microseconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

inline std::string get_timestamp() {
  return ToString(std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::system_clock::now().time_since_epoch())
                      .count());
}

static const char* s_hextable = "0123456789abcdef";
inline std::string hex_encode(const std::string& s) {
  std::string ret;
  for (auto x : s) {
    ret += s_hextable[(uint8_t)(x) >> 4];
    ret += s_hextable[(uint8_t)(x)&0xf];
  }
  return ret;
}

inline uint8_t char_to_hex(char c) {
  if (c >= '0' && c <= '9') {
    return c - '0';
  } else if (c >= 'a' && c <= 'f') {
    return c - 'a' + 10;
  } else if (c >= 'A' && c >= 'F') {
    return c - 'A' + 10;
  } else {
    return 0xff;
  }
}

inline std::string hex_decode(const std::string& s) {
  std::string ret = "";
  if (s.size() % 2 != 0) {
    return ret;
  }
  for (uint i = 0; i < s.size(); i += 2) {
    uint8_t c = 0;
    uint8_t x = char_to_hex(s[i]);
    if (x == 0xff) return ret;
    c |= x << 4;
    x = char_to_hex(s[i + 1]);
    if (x == 0xff) return ret;
    c |= x;
    ret.append(1, c);
  }
  return ret;
}

inline std::string string_replace(std::string str, const std::string& target,
                                  const std::string& repl) {
  if (str.empty() || target.empty()) {
    return str;
  }

  size_t idx = 0;
  for (;;) {
    idx = str.find(target, idx);
    if (idx == std::string::npos) break;
    str.replace(idx, target.length(), repl);
    idx += repl.length();
  }
  return str;
}

class BigEndian {
 public:
  static uint8_t ReadUint8(const std::string& data, int offset) {
    return uint8_t(data[offset + 0]);
  }

  static uint16_t ReadUint16(const std::string& data, int offset) {
    return uint16_t((uint8_t)(data[offset + 1])) |
           uint16_t((uint8_t)(data[offset + 0])) << 8;
  }

  static uint32_t ReadUint24(const std::string& data, int offset) {
    return (uint32_t((uint8_t)(data[offset + 2])) |
            uint32_t((uint8_t)(data[offset + 1])) << 8 |
            uint32_t((uint8_t)(data[offset + 0])) << 16) &
           0x00FFFFFF;
  }

  static uint32_t ReadUint32(const std::string& data, int offset) {
    return uint32_t((uint8_t)(data[offset + 3])) |
           uint32_t((uint8_t)(data[offset + 2])) << 8 |
           uint32_t((uint8_t)(data[offset + 1])) << 16 |
           uint32_t((uint8_t)(data[offset + 0])) << 24;
  }

  static void WriteUint8(std::string& data, uint8_t x) { data.append(1, x); }

  static void WriteUint16(std::string& data, uint16_t x) {
    data.append(1, (uint8_t)(x >> 8));
    data.append(1, (uint8_t)(x));
  }

  static void WriteUint24(std::string& data, uint32_t x) {
    data.append(1, (uint8_t)(x >> 16));
    data.append(1, (uint8_t)(x >> 8));
    data.append(1, (uint8_t)(x));
  }

  static void WriteUint32(std::string& data, uint32_t x) {
    data.append(1, (uint8_t)(x >> 24));
    data.append(1, (uint8_t)(x >> 16));
    data.append(1, (uint8_t)(x >> 8));
    data.append(1, (uint8_t)(x));
  }
};

}  // namespace msg

#endif /* _LIBMSG_UTILS_HPP_ */
