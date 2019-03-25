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
		dst.reserve(src.size());

		std::transform(src.begin(), src.end(), std::back_inserter(dst), [this](auto&lhs) {
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

	PROCESSWRAPPER
};