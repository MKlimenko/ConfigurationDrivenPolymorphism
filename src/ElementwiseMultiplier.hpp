#pragma once

#include "CommonProcessing.hpp"

template <typename InputContainer, bool keep_previous = false>
class ElementwiseMultiplier final : public CommonProcessing <InputContainer, keep_previous> {
public:
	using BaseType = CommonProcessing<InputContainer, keep_previous>;
	using InitializationTypes = typename BaseType::InitializationTypes;
	using InputOutputTypes = typename BaseType::InputOutputTypes;
	using InitInput = std::vector<typename InputContainer::value_type>;
	using ProcessInput = InputContainer;
	using ProcessOutput = InputContainer;

	std::vector<typename InputContainer::value_type> values{};

private:
	ProcessOutput Process(const ProcessInput& src) {
		assert(src.size() == values.size());
		ProcessOutput dst{};
		dst.reserve(src.size());

		std::transform(src.begin(), src.end(), values.begin(), std::back_inserter(dst), [this](auto&lhs, auto&rhs) {
			return lhs * rhs;
		});

		return dst;
	}

	ProcessOutput Process(ProcessInput&& src) {
		return Process(src);
	}

	virtual std::unique_ptr<BaseType> Clone() const override {
		return std::unique_ptr<BaseType>(new ElementwiseMultiplier());
	}

public:
	ElementwiseMultiplier() = default;
	ElementwiseMultiplier(InitInput&& vec) : values(std::move(vec)) {}
	
	virtual std::unique_ptr<BaseType> Clone(InitializationTypes&& values) const override {
		auto parameter = std::move(std::get<InitInput>(values));
		return std::unique_ptr<BaseType>(new ElementwiseMultiplier(std::move(parameter)));
	}

	PROCESSWRAPPER
};