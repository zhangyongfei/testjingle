// This file was GENERATED by command:
//     pump.py callback.h.pump
// DO NOT EDIT BY HAND!!!

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

// To generate callback.h from callback.h.pump, execute:
// /home/build/google3/third_party/gtest/scripts/pump.py callback.h.pump

// Callbacks are callable object containers. They can hold a function pointer
// or a function object and behave like a value type. Internally, data is
// reference-counted, making copies and pass-by-value inexpensive.
//
// Callbacks are typed using template arguments.  The format is:
//   CallbackN<ReturnType, ParamType1, ..., ParamTypeN>
// where N is the number of arguments supplied to the callable object.
// Callbacks are invoked using operator(), just like a function or a function
// object. Default-constructed callbacks are "empty," and executing an empty
// callback does nothing. A callback can be made empty by assigning it from
// a default-constructed callback.
//
// Callbacks are similar in purpose to std::function (which isn't available on
// all platforms we support) and a lightweight alternative to sigslots. Since
// they effectively hide the type of the object they call, they're useful in
// breaking dependencies between objects that need to interact with one another.
// Notably, they can hold the results of Bind(), std::bind*, etc, without
// needing
// to know the resulting object type of those calls.
//
// Sigslots, on the other hand, provide a fuller feature set, such as multiple
// subscriptions to a signal, optional thread-safety, and lifetime tracking of
// slots. When these features are needed, choose sigslots.
//
// Example:
//   int sqr(int x) { return x * x; }
//   struct AddK {
//     int k;
//     int operator()(int x) const { return x + k; }
//   } add_k = {5};
//
//   Callback1<int, int> my_callback;
//   cout << my_callback.empty() << endl;  // true
//
//   my_callback = Callback1<int, int>(&sqr);
//   cout << my_callback.empty() << endl;  // false
//   cout << my_callback(3) << endl;  // 9
//
//   my_callback = Callback1<int, int>(add_k);
//   cout << my_callback(10) << endl;  // 15
//
//   my_callback = Callback1<int, int>();
//   cout << my_callback.empty() << endl;  // true

#ifndef TALK_BASE_CALLBACK_H_
#define TALK_BASE_CALLBACK_H_

#include "base/logging.h"
#include "base/refcount.h"
#include "base/scoped_ref_ptr.h"

namespace talk_base {

template <class R>
class Callback0 {
 public:
  // Default copy operations are appropriate for this class.
  Callback0() {}
  template <class T> Callback0(const T& functor)
      : helper_(new RefCountedObject< HelperImpl<T> >(functor)) {}
  R operator()() {
    if (empty())
      return R();
    return helper_->Run();
  }
  bool empty() const { return !helper_; }

 private:
  struct Helper : RefCountInterface {
    virtual ~Helper() {}
    virtual R Run() = 0;
  };
  template <class T> struct HelperImpl : Helper {
    explicit HelperImpl(const T& functor) : functor_(functor) {}
    virtual R Run() {
      return functor_();
    }
    T functor_;
  };
  scoped_refptr<Helper> helper_;
};

template <class R,
          class P1>
class Callback1 {
 public:
  // Default copy operations are appropriate for this class.
  Callback1() {}
  template <class T> Callback1(const T& functor)
      : helper_(new RefCountedObject< HelperImpl<T> >(functor)) {}
  R operator()(P1 p1) {
    if (empty())
      return R();
    return helper_->Run(p1);
  }
  bool empty() const { return !helper_; }

 private:
  struct Helper : RefCountInterface {
    virtual ~Helper() {}
    virtual R Run(P1 p1) = 0;
  };
  template <class T> struct HelperImpl : Helper {
    explicit HelperImpl(const T& functor) : functor_(functor) {}
    virtual R Run(P1 p1) {
      return functor_(p1);
    }
    T functor_;
  };
  scoped_refptr<Helper> helper_;
};

template <class R,
          class P1,
          class P2>
class Callback2 {
 public:
  // Default copy operations are appropriate for this class.
  Callback2() {}
  template <class T> Callback2(const T& functor)
      : helper_(new RefCountedObject< HelperImpl<T> >(functor)) {}
  R operator()(P1 p1, P2 p2) {
    if (empty())
      return R();
    return helper_->Run(p1, p2);
  }
  bool empty() const { return !helper_; }

 private:
  struct Helper : RefCountInterface {
    virtual ~Helper() {}
    virtual R Run(P1 p1, P2 p2) = 0;
  };
  template <class T> struct HelperImpl : Helper {
    explicit HelperImpl(const T& functor) : functor_(functor) {}
    virtual R Run(P1 p1, P2 p2) {
      return functor_(p1, p2);
    }
    T functor_;
  };
  scoped_refptr<Helper> helper_;
};

template <class R,
          class P1,
          class P2,
          class P3>
class Callback3 {
 public:
  // Default copy operations are appropriate for this class.
  Callback3() {}
  template <class T> Callback3(const T& functor)
      : helper_(new RefCountedObject< HelperImpl<T> >(functor)) {}
  R operator()(P1 p1, P2 p2, P3 p3) {
    if (empty())
      return R();
    return helper_->Run(p1, p2, p3);
  }
  bool empty() const { return !helper_; }

 private:
  struct Helper : RefCountInterface {
    virtual ~Helper() {}
    virtual R Run(P1 p1, P2 p2, P3 p3) = 0;
  };
  template <class T> struct HelperImpl : Helper {
    explicit HelperImpl(const T& functor) : functor_(functor) {}
    virtual R Run(P1 p1, P2 p2, P3 p3) {
      return functor_(p1, p2, p3);
    }
    T functor_;
  };
  scoped_refptr<Helper> helper_;
};

template <class R,
          class P1,
          class P2,
          class P3,
          class P4>
class Callback4 {
 public:
  // Default copy operations are appropriate for this class.
  Callback4() {}
  template <class T> Callback4(const T& functor)
      : helper_(new RefCountedObject< HelperImpl<T> >(functor)) {}
  R operator()(P1 p1, P2 p2, P3 p3, P4 p4) {
    if (empty())
      return R();
    return helper_->Run(p1, p2, p3, p4);
  }
  bool empty() const { return !helper_; }

 private:
  struct Helper : RefCountInterface {
    virtual ~Helper() {}
    virtual R Run(P1 p1, P2 p2, P3 p3, P4 p4) = 0;
  };
  template <class T> struct HelperImpl : Helper {
    explicit HelperImpl(const T& functor) : functor_(functor) {}
    virtual R Run(P1 p1, P2 p2, P3 p3, P4 p4) {
      return functor_(p1, p2, p3, p4);
    }
    T functor_;
  };
  scoped_refptr<Helper> helper_;
};

template <class R,
          class P1,
          class P2,
          class P3,
          class P4,
          class P5>
class Callback5 {
 public:
  // Default copy operations are appropriate for this class.
  Callback5() {}
  template <class T> Callback5(const T& functor)
      : helper_(new RefCountedObject< HelperImpl<T> >(functor)) {}
  R operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) {
    if (empty())
      return R();
    return helper_->Run(p1, p2, p3, p4, p5);
  }
  bool empty() const { return !helper_; }

 private:
  struct Helper : RefCountInterface {
    virtual ~Helper() {}
    virtual R Run(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) = 0;
  };
  template <class T> struct HelperImpl : Helper {
    explicit HelperImpl(const T& functor) : functor_(functor) {}
    virtual R Run(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) {
      return functor_(p1, p2, p3, p4, p5);
    }
    T functor_;
  };
  scoped_refptr<Helper> helper_;
};
}  // namespace talk_base

#endif  // TALK_BASE_CALLBACK_H_
