#pragma once

#include <algorithm>
#include <cassert>
#include <functional>
#include <memory>
#include <numeric>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>

inline namespace detail {
	template <typename T, typename = void>
	struct is_container : std::false_type { };

	template <typename T>
	struct is_container<T,
		std::void_t<
		decltype(std::declval<T>().begin()),
		decltype(std::declval<T>().end()),
		decltype(begin(std::declval<T>()) == std::declval<typename T::iterator>()),
		typename T::value_type
		>>
		: std::true_type{ };

	template <typename T>
	constexpr bool is_container_v = is_container<T>::value;

	constexpr bool keep_previous = true;

	#define PROCESSWRAPPER \
	virtual InputOutputTypes Process(const InputOutputTypes& src) override { \
		auto input = BaseType:: template Get<ProcessInput>(src); \
		return Process(input); \
	} \
	virtual InputOutputTypes Process(InputOutputTypes&& src) override { \
		if constexpr (keep_previous) { \
			auto laundered_src = BaseType:: template Get<ProcessInput>(std::move(src)); \
			auto dst = Process(laundered_src); \
			src = std::move(laundered_src); \
			return dst; \
		} \
		else \
			return Process(BaseType:: template Get<ProcessInput>(std::move(src))); \
	}
}

template <typename InputContainer, bool keep_previous = false>
class CommonProcessing {
public:
	using InitializationTypes = std::variant<
		//void,
		typename InputContainer::value_type,
		std::tuple<typename InputContainer::value_type, typename InputContainer::value_type>,
		InputContainer
	>;
	using InputOutputTypes = std::variant<
		InputContainer, 
		typename InputContainer::value_type
	>;

	static_assert(is_container_v<InputContainer>, "Expected container as a template parameter");
	virtual InputOutputTypes Process(const InputOutputTypes& src) = 0;
	virtual InputOutputTypes Process(InputOutputTypes&& src) = 0;
	virtual std::unique_ptr<CommonProcessing> Clone() const = 0;
	virtual std::unique_ptr<CommonProcessing> Clone(InitializationTypes&& values) const = 0;
	virtual ~CommonProcessing() = default;

	template <typename T>
	static auto Get(const InputOutputTypes& src) {
		return std::get<T>(src);
	}

	template <typename T>
	static auto Get(InputOutputTypes&& src) {
		return std::get<T>(std::move(src));
	}
};
