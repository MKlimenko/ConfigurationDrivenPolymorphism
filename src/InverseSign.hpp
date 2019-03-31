#pragma once

#include "CommonProcessing.hpp"

template <typename InputContainer, bool keep_previous = false>
class InverseSign final : public CommonProcessing <InputContainer, keep_previous> {
public:
	using BaseType = CommonProcessing<InputContainer, keep_previous>;
	using InitializationTypes = typename BaseType::InitializationTypes;
	using InputOutputTypes = typename BaseType::InputOutputTypes;
	using ProcessInput = typename InputContainer::value_type;
	using ProcessOutput = typename InputContainer::value_type;

private:
	ProcessOutput Process(const ProcessInput& src) {
		return -src;
	}

	ProcessOutput Process(ProcessInput&& src) {
		return Process(src);
	}

	virtual std::unique_ptr<BaseType> Clone(InitializationTypes&& values) const override {
		return std::unique_ptr<BaseType>(new InverseSign());
	}

public:
	virtual std::unique_ptr<BaseType> Clone() const override {
		return std::unique_ptr<BaseType>(new InverseSign());
	}

	virtual InitializationTypes ReadParameters(tinyxml2::XMLElement* root) const {
		return InitializationTypes{};
	}
	
	CALLWRAPPER

	static auto InverseSignReference(const ProcessInput& src) {
		return -src;
	}
};
