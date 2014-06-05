/*
 * libjingle
 * Copyright 2009, Google Inc.
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

#include "base/gunit.h"
#include "base/macutils.h"

TEST(MacUtilsTest, GetOsVersionName) {
  talk_base::MacOSVersionName ver = talk_base::GetOSVersionName();
  LOG(LS_INFO) << "GetOsVersionName " << ver;
  EXPECT_NE(talk_base::kMacOSUnknown, ver);
}

TEST(MacUtilsTest, GetQuickTimeVersion) {
  std::string version;
  EXPECT_TRUE(talk_base::GetQuickTimeVersion(&version));
  LOG(LS_INFO) << "GetQuickTimeVersion " << version;
}

TEST(MacUtilsTest, RunAppleScriptCompileError) {
  std::string script("set value to to 5");
  EXPECT_FALSE(talk_base::RunAppleScript(script));
}

TEST(MacUtilsTest, RunAppleScriptRuntimeError) {
  std::string script("set value to 5 / 0");
  EXPECT_FALSE(talk_base::RunAppleScript(script));
}

#ifdef CARBON_DEPRECATED
TEST(MacUtilsTest, DISABLED_RunAppleScriptSuccess) {
#else
TEST(MacUtilsTest, RunAppleScriptSuccess) {
#endif
  std::string script("set value to 5");
  EXPECT_TRUE(talk_base::RunAppleScript(script));
}