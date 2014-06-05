// This file was GENERATED by command:
//     pump.py genericslot.h.pump
// DO NOT EDIT BY HAND!!!

/*
 * libjingle
 * Copyright 2014 Google Inc.
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

#ifndef TALK_BASE_GENERICSLOT_H_
#define TALK_BASE_GENERICSLOT_H_

// To generate genericslot.h from genericslot.h.pump, execute:
// /home/build/google3/third_party/gtest/scripts/pump.py genericslot.h.pump

// Generic-slots are pure slots that can be hooked up to signals. They're
// mainly intended to be used in tests where we want to check if a signal
// was invoked and what arguments were passed. NOTE: They do not do any
// lifetime management of the arguments received via callbacks.
//
// Example:
//   /* Some signal */
//   sigslot::signal1<int> foo;
//
//   /* We want to monitor foo in some test */
//   talk_base::GenericSlot1<int> slot(&foo, 0);
//   foo.emit(5);
//   EXPECT_TRUE(slot.callback_received());
//   EXPECT_EQ(5, *(slot.arg1()));
//

#include "base/constructormagic.h"
#include "base/sigslot.h"

namespace talk_base {

template <class A1>
class GenericSlot1 : public sigslot::has_slots<> {
 public:
  GenericSlot1(sigslot::signal1<A1>* signal,
                const A1& arg1_initial)
      : arg1_initial_(arg1_initial) {
    Reset();
    signal->connect(this, &GenericSlot1::OnSignalCallback);
  }

  void Reset() {
    callback_received_ = false;
    arg1_ = arg1_initial_;
  }

  bool callback_received() const { return callback_received_; }
  const A1& arg1() const { return arg1_; }

 private:
  void OnSignalCallback(A1 arg1) {
    callback_received_ = true;
    arg1_ = arg1;
  }

  bool callback_received_;
  A1 arg1_initial_, arg1_;

  DISALLOW_COPY_AND_ASSIGN(GenericSlot1);
};

template <class A1, class A2>
class GenericSlot2 : public sigslot::has_slots<> {
 public:
  GenericSlot2(sigslot::signal2<A1, A2>* signal,
                const A1& arg1_initial, const A2& arg2_initial)
      : arg1_initial_(arg1_initial), arg2_initial_(arg2_initial) {
    Reset();
    signal->connect(this, &GenericSlot2::OnSignalCallback);
  }

  void Reset() {
    callback_received_ = false;
    arg1_ = arg1_initial_;
    arg2_ = arg2_initial_;
  }

  bool callback_received() const { return callback_received_; }
  const A1& arg1() const { return arg1_; }
  const A2& arg2() const { return arg2_; }

 private:
  void OnSignalCallback(A1 arg1, A2 arg2) {
    callback_received_ = true;
    arg1_ = arg1;
    arg2_ = arg2;
  }

  bool callback_received_;
  A1 arg1_initial_, arg1_;
  A2 arg2_initial_, arg2_;

  DISALLOW_COPY_AND_ASSIGN(GenericSlot2);
};

template <class A1, class A2, class A3>
class GenericSlot3 : public sigslot::has_slots<> {
 public:
  GenericSlot3(sigslot::signal3<A1, A2, A3>* signal,
                const A1& arg1_initial, const A2& arg2_initial,
                    const A3& arg3_initial)
      : arg1_initial_(arg1_initial), arg2_initial_(arg2_initial),
          arg3_initial_(arg3_initial) {
    Reset();
    signal->connect(this, &GenericSlot3::OnSignalCallback);
  }

  void Reset() {
    callback_received_ = false;
    arg1_ = arg1_initial_;
    arg2_ = arg2_initial_;
    arg3_ = arg3_initial_;
  }

  bool callback_received() const { return callback_received_; }
  const A1& arg1() const { return arg1_; }
  const A2& arg2() const { return arg2_; }
  const A3& arg3() const { return arg3_; }

 private:
  void OnSignalCallback(A1 arg1, A2 arg2, A3 arg3) {
    callback_received_ = true;
    arg1_ = arg1;
    arg2_ = arg2;
    arg3_ = arg3;
  }

  bool callback_received_;
  A1 arg1_initial_, arg1_;
  A2 arg2_initial_, arg2_;
  A3 arg3_initial_, arg3_;

  DISALLOW_COPY_AND_ASSIGN(GenericSlot3);
};

template <class A1, class A2, class A3, class A4>
class GenericSlot4 : public sigslot::has_slots<> {
 public:
  GenericSlot4(sigslot::signal4<A1, A2, A3, A4>* signal,
                const A1& arg1_initial, const A2& arg2_initial,
                    const A3& arg3_initial, const A4& arg4_initial)
      : arg1_initial_(arg1_initial), arg2_initial_(arg2_initial),
          arg3_initial_(arg3_initial), arg4_initial_(arg4_initial) {
    Reset();
    signal->connect(this, &GenericSlot4::OnSignalCallback);
  }

  void Reset() {
    callback_received_ = false;
    arg1_ = arg1_initial_;
    arg2_ = arg2_initial_;
    arg3_ = arg3_initial_;
    arg4_ = arg4_initial_;
  }

  bool callback_received() const { return callback_received_; }
  const A1& arg1() const { return arg1_; }
  const A2& arg2() const { return arg2_; }
  const A3& arg3() const { return arg3_; }
  const A4& arg4() const { return arg4_; }

 private:
  void OnSignalCallback(A1 arg1, A2 arg2, A3 arg3, A4 arg4) {
    callback_received_ = true;
    arg1_ = arg1;
    arg2_ = arg2;
    arg3_ = arg3;
    arg4_ = arg4;
  }

  bool callback_received_;
  A1 arg1_initial_, arg1_;
  A2 arg2_initial_, arg2_;
  A3 arg3_initial_, arg3_;
  A4 arg4_initial_, arg4_;

  DISALLOW_COPY_AND_ASSIGN(GenericSlot4);
};

template <class A1, class A2, class A3, class A4, class A5>
class GenericSlot5 : public sigslot::has_slots<> {
 public:
  GenericSlot5(sigslot::signal5<A1, A2, A3, A4, A5>* signal,
                const A1& arg1_initial, const A2& arg2_initial,
                    const A3& arg3_initial, const A4& arg4_initial,
                    const A5& arg5_initial)
      : arg1_initial_(arg1_initial), arg2_initial_(arg2_initial),
          arg3_initial_(arg3_initial), arg4_initial_(arg4_initial),
          arg5_initial_(arg5_initial) {
    Reset();
    signal->connect(this, &GenericSlot5::OnSignalCallback);
  }

  void Reset() {
    callback_received_ = false;
    arg1_ = arg1_initial_;
    arg2_ = arg2_initial_;
    arg3_ = arg3_initial_;
    arg4_ = arg4_initial_;
    arg5_ = arg5_initial_;
  }

  bool callback_received() const { return callback_received_; }
  const A1& arg1() const { return arg1_; }
  const A2& arg2() const { return arg2_; }
  const A3& arg3() const { return arg3_; }
  const A4& arg4() const { return arg4_; }
  const A5& arg5() const { return arg5_; }

 private:
  void OnSignalCallback(A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5) {
    callback_received_ = true;
    arg1_ = arg1;
    arg2_ = arg2;
    arg3_ = arg3;
    arg4_ = arg4;
    arg5_ = arg5;
  }

  bool callback_received_;
  A1 arg1_initial_, arg1_;
  A2 arg2_initial_, arg2_;
  A3 arg3_initial_, arg3_;
  A4 arg4_initial_, arg4_;
  A5 arg5_initial_, arg5_;

  DISALLOW_COPY_AND_ASSIGN(GenericSlot5);
};
}  // namespace talk_base

#endif  // TALK_BASE_GENERICSLOT_H_
