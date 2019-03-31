#pragma once

#include <algorithm>
#include <cassert>
#include <execution>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

#include "folly\Conv.h"
#include "tinyxml2.h"

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

	#define CALLWRAPPER \
	virtual InputOutputTypes operator()(const InputOutputTypes& src) override { \
		auto input = std::get<ProcessInput>(src); \
		return Process(input); \
	} \
	virtual InputOutputTypes operator()(InputOutputTypes&& src) override { \
		if constexpr (keep_previous) { \
			auto laundered_src = std::get<ProcessInput>(std::move(src)); \
			auto dst = Process(laundered_src); \
			src = std::move(laundered_src); \
			return dst; \
		} \
		else \
			return Process(std::get<ProcessInput>(std::move(src))); \
	}
}

template <typename InputContainer, bool keep_previous = false>
class CommonProcessing {
protected:
	static std::string GetElementPath(tinyxml2::XMLElement* caller) {
		std::string current_node_path{};
		tinyxml2::XMLNode* current_caller = caller;
		do {
			current_node_path = std::string(current_caller->Value()) + (current_node_path.empty() ? "" : "->" + current_node_path);
			current_caller = current_caller->Parent();
		} while (current_caller->Parent());

		return current_node_path;
	}

	static std::string ReadString(tinyxml2::XMLElement *caller) {
		if (caller == nullptr)
			throw std::runtime_error("Unable to find XML element");
		if (caller->GetText() == nullptr) {
			throw std::runtime_error("Empty element: " + GetElementPath(caller));
		}
		return caller->GetText();
	}

public:
	using InitializationTypes = std::variant<
		typename InputContainer::value_type,
		std::tuple<typename InputContainer::value_type, typename InputContainer::value_type>,
		InputContainer
	>;
	using InputOutputTypes = std::variant<
		InputContainer, 
		typename InputContainer::value_type
	>;

	static_assert(is_container_v<InputContainer>, "Expected container as a template parameter");

	virtual InputOutputTypes operator()(const InputOutputTypes& src) = 0;
	virtual InputOutputTypes operator()(InputOutputTypes&& src) = 0;
	virtual std::unique_ptr<CommonProcessing> Clone() const = 0;
	virtual std::unique_ptr<CommonProcessing> Clone(InitializationTypes&& values) const = 0;
	virtual InitializationTypes ReadParameters(tinyxml2::XMLElement* root) const = 0;
	virtual ~CommonProcessing() = default;
};
