#ifndef SHADOW_NATIVE_CORE_HPP
#define SHADOW_NATIVE_CORE_HPP

#include <memory>
#include <utility>

namespace Shadow {

struct FailableFunctionStatus {
	enum Enum { Success, Failure };
};

template <typename T> using Reference = std::shared_ptr<T>;
template <typename T, typename... Args> constexpr Reference<T> CreateReference(Args&&... args) {
	return std::make_shared<T>(std::forward<Args>(args)...);
}

}

#endif /* SHADOW_NATIVE_CORE_HPP */