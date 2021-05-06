// This file is part of CAF, the C++ Actor Framework. See the file LICENSE in
// the main distribution directory for license terms and copyright or visit
// https://github.com/actor-framework/actor-framework/blob/master/LICENSE.

#include "caf/flow/batch.hpp"

#include "caf/binary_deserializer.hpp"
#include "caf/binary_serializer.hpp"
#include "caf/deserializer.hpp"
#include "caf/detail/meta_object.hpp"
#include "caf/serializer.hpp"

namespace caf::flow {

// -- batch::data --------------------------------------------------------------

batch::data::~data() {
  if (size_ > 0) {
    auto meta = detail::global_meta_object(item_type_);
    auto ptr = storage();
    do {
      meta->destroy(ptr);
      ptr += item_size_;
      --size_;
    } while (size_ > 0);
  }
}

template <class Inspector>
bool batch::data::save(Inspector& sink) const {
  CAF_ASSERT(size_ > 0);
  auto meta = detail::global_meta_object(item_type_);
  auto ptr = storage_;
  if (!sink.begin_sequence(size_))
    return false;
  auto len = size_;
  do {
    if constexpr (std::is_same_v<Inspector, binary_serializer>) {
      if (!meta->save_binary(sink, ptr))
        return false;
    } else {
      if (!meta->save(sink, ptr))
        return false;
    }
    ptr += item_size_;
    --len;
  } while (len > 0);
  return sink.end_sequence();
}

// -- batch --------------------------------------------------------------------

template <class Inspector>
bool batch::save_impl(Inspector& f) const {
  if (data_)
    return data_->save(f);
  else
    return f.begin_sequence(0) && f.end_sequence();
}

bool batch::save(serializer& f) const {
  return save_impl(f);
}

bool batch::save(binary_serializer& f) const {
  return save_impl(f);
}
template <class Inspector>
bool batch::load_impl(Inspector&) {
  // TODO: implement me
  return false;
  /*
  auto len = size_t{0};
  if (!f.begin_sequence(len))
    return false;
  if (len == 0) {
    data_.reset();
    return f.end_sequence();
  }
  */
}

bool batch::load(deserializer& f) {
  return load_impl(f);
}

bool batch::load(binary_deserializer& f) {
  return load_impl(f);
}

} // namespace caf::flow
