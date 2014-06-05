/*
 * libjingle
 * Copyright 2004--2011, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef POSIX
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#ifdef OPENBSD
#include <netinet/in_systm.h>
#endif
#ifndef __native_client__
#include <netinet/ip.h>
#endif
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif

#include <stdio.h>

#include "base/ipaddress.h"
#include "base/byteorder.h"
#include "base/nethelpers.h"
#include "base/logging.h"
#ifdef WIN32
#include "os/win32/win32.h"
#endif

namespace talk_base {

// Prefixes used for categorizing IPv6 addresses.
static const in6_addr kV4MappedPrefix = {{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                           0xFF, 0xFF, 0}}};
static const in6_addr k6To4Prefix = {{{0x20, 0x02, 0}}};
static const in6_addr kTeredoPrefix = {{{0x20, 0x01, 0x00, 0x00}}};
static const in6_addr kV4CompatibilityPrefix = {{{0}}};
static const in6_addr k6BonePrefix = {{{0x3f, 0xfe, 0}}};

bool IPAddress::strip_sensitive_ = false;

static bool IsPrivateV4(uint32 ip);
static in_addr ExtractMappedAddress(const in6_addr& addr);

uint32 IPAddress::v4AddressAsHostOrderInteger() const {
  if (family_ == AF_INET) {
    return NetworkToHost32(u_.ip4.s_addr);
  } else {
    return 0;
  }
}

size_t IPAddress::Size() const {
  switch (family_) {
    case AF_INET:
      return sizeof(in_addr);
    case AF_INET6:
      return sizeof(in6_addr);
  }
  return 0;
}


bool IPAddress::operator==(const IPAddress &other) const {
  if (family_ != other.family_) {
    return false;
  }
  if (family_ == AF_INET) {
    return memcmp(&u_.ip4, &other.u_.ip4, sizeof(u_.ip4)) == 0;
  }
  if (family_ == AF_INET6) {
    return memcmp(&u_.ip6, &other.u_.ip6, sizeof(u_.ip6)) == 0;
  }
  return family_ == AF_UNSPEC;
}

bool IPAddress::operator!=(const IPAddress &other) const {
  return !((*this) == other);
}

bool IPAddress::operator >(const IPAddress &other) const {
  return (*this) != other && !((*this) < other);
}

bool IPAddress::operator <(const IPAddress &other) const {
  // IPv4 is 'less than' IPv6
  if (family_ != other.family_) {
    if (family_ == AF_UNSPEC) {
      return true;
    }
    if (family_ == AF_INET && other.family_ == AF_INET6) {
      return true;
    }
    return false;
  }
  // Comparing addresses of the same family.
  switch (family_) {
    case AF_INET: {
      return NetworkToHost32(u_.ip4.s_addr) <
          NetworkToHost32(other.u_.ip4.s_addr);
    }
    case AF_INET6: {
      return memcmp(&u_.ip6.s6_addr, &other.u_.ip6.s6_addr, 16) < 0;
    }
  }
  // Catches AF_UNSPEC and invalid addresses.
  return false;
}

std::ostream& operator<<(std::ostream& os, const IPAddress& ip) {
  os << ip.ToString();
  return os;
}

in6_addr IPAddress::ipv6_address() const {
  return u_.ip6;
}

in_addr IPAddress::ipv4_address() const {
  return u_.ip4;
}

std::string IPAddress::ToString() const {
  if (family_ != AF_INET && family_ != AF_INET6) {
    return std::string();
  }
  char buf[INET6_ADDRSTRLEN] = {0};
  const void* src = &u_.ip4;
  if (family_ == AF_INET6) {
    src = &u_.ip6;
  }
  if (!talk_base::inet_ntop(family_, src, buf, sizeof(buf))) {
    return std::string();
  }
  return std::string(buf);
}

std::string IPAddress::ToSensitiveString() const {
  if (!strip_sensitive_)
    return ToString();

  switch (family_) {
    case AF_INET: {
      std::string address = ToString();
      size_t find_pos = address.rfind('.');
      if (find_pos == std::string::npos)
        return std::string();
      address.resize(find_pos);
      address += ".x";
      return address;
    }
    case AF_INET6: {
      // TODO(grunell): Return a string of format 1:2:3:x:x:x:x:x or such
      // instead of zeroing out.
      return TruncateIP(*this, 128 - 80).ToString();
    }
  }
  return std::string();
}

IPAddress IPAddress::Normalized() const {
  if (family_ != AF_INET6) {
    return *this;
  }
  if (!IPIsV4Mapped(*this)) {
    return *this;
  }
  in_addr addr = ExtractMappedAddress(u_.ip6);
  return IPAddress(addr);
}

IPAddress IPAddress::AsIPv6Address() const {
  if (family_ != AF_INET) {
    return *this;
  }
  in6_addr v6addr = kV4MappedPrefix;
  ::memcpy(&v6addr.s6_addr[12], &u_.ip4.s_addr, sizeof(u_.ip4.s_addr));
  return IPAddress(v6addr);
}

void IPAddress::set_strip_sensitive(bool enable) {
  strip_sensitive_ = enable;
}


bool IsPrivateV4(uint32 ip_in_host_order) {
  return ((ip_in_host_order >> 24) == 127) ||
      ((ip_in_host_order >> 24) == 10) ||
      ((ip_in_host_order >> 20) == ((172 << 4) | 1)) ||
      ((ip_in_host_order >> 16) == ((192 << 8) | 168)) ||
      ((ip_in_host_order >> 16) == ((169 << 8) | 254));
}

in_addr ExtractMappedAddress(const in6_addr& in6) {
  in_addr ipv4;
  ::memcpy(&ipv4.s_addr, &in6.s6_addr[12], sizeof(ipv4.s_addr));
  return ipv4;
}

bool IPFromAddrInfo(struct addrinfo* info, IPAddress* out) {
  if (!info || !info->ai_addr) {
    return false;
  }
  if (info->ai_addr->sa_family == AF_INET) {
    sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(info->ai_addr);
    *out = IPAddress(addr->sin_addr);
    return true;
  } else if (info->ai_addr->sa_family == AF_INET6) {
    sockaddr_in6* addr = reinterpret_cast<sockaddr_in6*>(info->ai_addr);
    *out = IPAddress(addr->sin6_addr);
    return true;
  }
  return false;
}

bool IPFromString(const std::string& str, IPAddress* out) {
  if (!out) {
    return false;
  }
  in_addr addr;
  if (talk_base::inet_pton(AF_INET, str.c_str(), &addr) == 0) {
    in6_addr addr6;
    if (talk_base::inet_pton(AF_INET6, str.c_str(), &addr6) == 0) {
      *out = IPAddress();
      return false;
    }
    *out = IPAddress(addr6);
  } else {
    *out = IPAddress(addr);
  }
  return true;
}

bool IPIsAny(const IPAddress& ip) {
  switch (ip.family()) {
    case AF_INET:
      return ip == IPAddress(INADDR_ANY);
    case AF_INET6:
      return ip == IPAddress(in6addr_any);
    case AF_UNSPEC:
      return false;
  }
  return false;
}

bool IPIsLoopback(const IPAddress& ip) {
  switch (ip.family()) {
    case AF_INET: {
      return ip == IPAddress(INADDR_LOOPBACK);
    }
    case AF_INET6: {
      return ip == IPAddress(in6addr_loopback);
    }
  }
  return false;
}

bool IPIsPrivate(const IPAddress& ip) {
  switch (ip.family()) {
    case AF_INET: {
      return IsPrivateV4(ip.v4AddressAsHostOrderInteger());
    }
    case AF_INET6: {
      in6_addr v6 = ip.ipv6_address();
      return (v6.s6_addr[0] == 0xFE && v6.s6_addr[1] == 0x80) ||
          IPIsLoopback(ip);
    }
  }
  return false;
}

bool IPIsUnspec(const IPAddress& ip) {
  return ip.family() == AF_UNSPEC;
}

size_t HashIP(const IPAddress& ip) {
  switch (ip.family()) {
    case AF_INET: {
      return ip.ipv4_address().s_addr;
    }
    case AF_INET6: {
      in6_addr v6addr = ip.ipv6_address();
      const uint32* v6_as_ints =
          reinterpret_cast<const uint32*>(&v6addr.s6_addr);
      return v6_as_ints[0] ^ v6_as_ints[1] ^ v6_as_ints[2] ^ v6_as_ints[3];
    }
  }
  return 0;
}

IPAddress TruncateIP(const IPAddress& ip, int length) {
  if (length < 0) {
    return IPAddress();
  }
  if (ip.family() == AF_INET) {
    if (length > 31) {
      return ip;
    }
    if (length == 0) {
      return IPAddress(INADDR_ANY);
    }
    int mask = (0xFFFFFFFF << (32 - length));
    uint32 host_order_ip = NetworkToHost32(ip.ipv4_address().s_addr);
    in_addr masked;
    masked.s_addr = HostToNetwork32(host_order_ip & mask);
    return IPAddress(masked);
  } else if (ip.family() == AF_INET6) {
    if (length > 127) {
      return ip;
    }
    if (length == 0) {
      return IPAddress(in6addr_any);
    }
    in6_addr v6addr = ip.ipv6_address();
    int position = length / 32;
    int inner_length = 32 - (length - (position * 32));
    // Note: 64bit mask constant needed to allow possible 32-bit left shift.
    uint32 inner_mask = 0xFFFFFFFFLL  << inner_length;
    uint32* v6_as_ints =
        reinterpret_cast<uint32*>(&v6addr.s6_addr);
    for (int i = 0; i < 4; ++i) {
      if (i == position) {
        uint32 host_order_inner = NetworkToHost32(v6_as_ints[i]);
        v6_as_ints[i] = HostToNetwork32(host_order_inner & inner_mask);
      } else if (i > position) {
        v6_as_ints[i] = 0;
      }
    }
    return IPAddress(v6addr);
  }
  return IPAddress();
}

int CountIPMaskBits(IPAddress mask) {
  uint32 word_to_count = 0;
  int bits = 0;
  switch (mask.family()) {
    case AF_INET: {
      word_to_count = NetworkToHost32(mask.ipv4_address().s_addr);
      break;
    }
    case AF_INET6: {
      in6_addr v6addr = mask.ipv6_address();
      const uint32* v6_as_ints =
          reinterpret_cast<const uint32*>(&v6addr.s6_addr);
      int i = 0;
      for (; i < 4; ++i) {
        if (v6_as_ints[i] != 0xFFFFFFFF) {
          break;
        }
      }
      if (i < 4) {
        word_to_count = NetworkToHost32(v6_as_ints[i]);
      }
      bits = (i * 32);
      break;
    }
    default: {
      return 0;
    }
  }
  if (word_to_count == 0) {
    return bits;
  }

  // Public domain bit-twiddling hack from:
  // http://graphics.stanford.edu/~seander/bithacks.html
  // Counts the trailing 0s in the word.
  unsigned int zeroes = 32;
  word_to_count &= -static_cast<int32>(word_to_count);
  if (word_to_count) zeroes--;
  if (word_to_count & 0x0000FFFF) zeroes -= 16;
  if (word_to_count & 0x00FF00FF) zeroes -= 8;
  if (word_to_count & 0x0F0F0F0F) zeroes -= 4;
  if (word_to_count & 0x33333333) zeroes -= 2;
  if (word_to_count & 0x55555555) zeroes -= 1;

  return bits + (32 - zeroes);
}

bool IPIsHelper(const IPAddress& ip, const in6_addr& tomatch, int length) {
  // Helper method for checking IP prefix matches (but only on whole byte
  // lengths). Length is in bits.
  in6_addr addr = ip.ipv6_address();
  return ::memcmp(&addr, &tomatch, (length >> 3)) == 0;
}

bool IPIs6Bone(const IPAddress& ip) {
  return IPIsHelper(ip, k6BonePrefix, 16);
}

bool IPIs6To4(const IPAddress& ip) {
  return IPIsHelper(ip, k6To4Prefix, 16);
}

bool IPIsSiteLocal(const IPAddress& ip) {
  // Can't use the helper because the prefix is 10 bits.
  in6_addr addr = ip.ipv6_address();
  return addr.s6_addr[0] == 0xFE && (addr.s6_addr[1] & 0xC0) == 0xC0;
}

bool IPIsULA(const IPAddress& ip) {
  // Can't use the helper because the prefix is 7 bits.
  in6_addr addr = ip.ipv6_address();
  return (addr.s6_addr[0] & 0xFE) == 0xFC;
}

bool IPIsTeredo(const IPAddress& ip) {
  return IPIsHelper(ip, kTeredoPrefix, 32);
}

bool IPIsV4Compatibility(const IPAddress& ip) {
  return IPIsHelper(ip, kV4CompatibilityPrefix, 96);
}

bool IPIsV4Mapped(const IPAddress& ip) {
  return IPIsHelper(ip, kV4MappedPrefix, 96);
}

int IPAddressPrecedence(const IPAddress& ip) {
  // Precedence values from RFC 3484-bis. Prefers native v4 over 6to4/Teredo.
  if (ip.family() == AF_INET) {
    return 30;
  } else if (ip.family() == AF_INET6) {
    if (IPIsLoopback(ip)) {
      return 60;
    } else if (IPIsULA(ip)) {
      return 50;
    } else if (IPIsV4Mapped(ip)) {
      return 30;
    } else if (IPIs6To4(ip)) {
      return 20;
    } else if (IPIsTeredo(ip)) {
      return 10;
    } else if (IPIsV4Compatibility(ip) || IPIsSiteLocal(ip) || IPIs6Bone(ip)) {
      return 1;
    } else {
      // A 'normal' IPv6 address.
      return 40;
    }
  }
  return 0;
}

}  // Namespace talk base