/*
 * libjingle
 * Copyright 2012 Google Inc.
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

#ifndef TALK_BASE_SHA1DIGEST_H_
#define TALK_BASE_SHA1DIGEST_H_

#include "base/messagedigest.h"
#include "base/sha1.h"

namespace talk_base {

// A simple wrapper for our SHA-1 implementation.
class Sha1Digest : public MessageDigest {
 public:
  enum { kSize = SHA1_DIGEST_SIZE };
  Sha1Digest() {
    SHA1Init(&ctx_);
  }
  virtual size_t Size() const {
    return kSize;
  }
  virtual void Update(const void* buf, size_t len) {
    SHA1Update(&ctx_, static_cast<const uint8*>(buf), len);
  }
  virtual size_t Finish(void* buf, size_t len) {
    if (len < kSize) {
      return 0;
    }
    SHA1Final(&ctx_, static_cast<uint8*>(buf));
    SHA1Init(&ctx_);  // Reset for next use.
    return kSize;
  }

 private:
  SHA1_CTX ctx_;
};

}  // namespace talk_base

#endif  // TALK_BASE_SHA1DIGEST_H_
