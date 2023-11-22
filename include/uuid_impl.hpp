#ifndef SHADOW_NATIVE_UUID_IMPL_HPP
#define SHADOW_NATIVE_UUID_IMPL_HPP

// #define UUID_SYSTEM_GENERATOR
#include "ppk_assert_impl.hpp"
#include <algorithm>
#include <array>
#include <functional>
#include <iterator>
#include <random>
#include <uuid.h>

namespace Shadow {

inline uuids::uuid generateUUID() {

	std::random_device rd;
	auto seed_data = std::array<int, std::mt19937::state_size> {};
	std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
	std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
	std::mt19937 generator(seq);
	uuids::uuid_random_generator gen { generator };

	uuids::uuid const id = gen();

	PPK_ASSERT(!id.is_nil());

	return id;
}

}

#endif /* SHADOW_NATIVE_UUID_IMPL_HPP */