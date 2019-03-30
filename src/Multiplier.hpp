#pragma once

#include "CommonProcessing.hpp"

template <typename InputContainer, bool keep_previous = false>
class Multiplier final : public CommonProcessing <InputContainer, keep_previous> {
public:
	using BaseType = CommonProcessing<InputContainer, keep_previous>;
	using InitializationTypes = typename BaseType::InitializationTypes;
	using InputOutputTypes = typename BaseType::InputOutputTypes;
	using InitInput = std::tuple<typename InputContainer::value_type, typename InputContainer::value_type>;
	using ProcessInput = InputContainer;
	using ProcessOutput = InputContainer;

	InitInput value;

private:
	ProcessOutput Process(const ProcessInput& src) {
		ProcessOutput dst{};
		dst.resize(src.size());

		std::transform(std::execution::par_unseq, src.begin(), src.end(), dst.begin(), [this](auto&lhs) {
			return std::get<0>(value) + lhs * std::get<1>(value);
		});

		return dst;
	}

	ProcessOutput Process(ProcessInput&& src) {
		return Process(src);
	}

	virtual std::unique_ptr<BaseType> Clone() const override {
		return std::unique_ptr<BaseType>(new Multiplier());
	}

public:
	Multiplier() {}
	Multiplier(InitInput&& val) : value(std::move(val)) {}
	
	virtual std::unique_ptr<BaseType> Clone(InitializationTypes&& values) const override {
		auto parameter = std::move(std::get<InitInput>(values));
		return std::unique_ptr<BaseType>(new Multiplier(std::move(parameter)));
	}

	virtual InitializationTypes ReadParameter(tinyxml2::XMLElement* root) const {
		InitInput dst{};
		auto& first = std::get<0>(dst);
		auto& second = std::get<1>(dst);

		auto ptr = root->FirstChildElement();
		first = folly::to<typename InputContainer::value_type>(BaseType::ReadString(ptr));
		ptr = ptr->NextSiblingElement();
		second = folly::to<typename InputContainer::value_type>(BaseType::ReadString(ptr));

		return dst;
	}

	CALLWRAPPER

	static auto MultiplierReference(const ProcessInput& src, const std::tuple<typename InputContainer::value_type, typename InputContainer::value_type>& arguments) {
		ProcessOutput dst{};
		dst.resize(src.size());

		std::transform(std::execution::par_unseq, src.begin(), src.end(), dst.begin(), [&arguments](auto&lhs) {
			return std::get<0>(arguments) + lhs * std::get<1>(arguments);
		});

		return dst;
	}
};