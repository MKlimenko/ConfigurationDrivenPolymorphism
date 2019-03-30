#pragma once

#include "CommonProcessing.hpp"

template <typename InputContainer, bool keep_previous = false>
class Placeholder final : public CommonProcessing <InputContainer, keep_previous> {
public:
	using BaseType = CommonProcessing<InputContainer, keep_previous>;
	using InitializationTypes = typename BaseType::InitializationTypes;
	using InputOutputTypes = typename BaseType::InputOutputTypes;
	using ProcessInput = InputContainer;
	using ProcessOutput = InputContainer;

private:
	ProcessOutput Process(const ProcessInput& src) {
		return src;
	}

	ProcessOutput Process(ProcessInput&& src) {
		return std::move(src);
	}

	virtual std::unique_ptr<BaseType> Clone(InitializationTypes&& values) const override {
		return std::unique_ptr<BaseType>(new Placeholder());
	}
	
public:
	virtual std::unique_ptr<BaseType> Clone() const override {
		return std::unique_ptr<BaseType>(new Placeholder());
	}

	virtual InitializationTypes ReadParameter(tinyxml2::XMLElement* root) const {
		return InitializationTypes{};
	}

	CALLWRAPPER

	static auto PlaceholderReference(const ProcessInput& src) {
		return src;
	}	
	static auto PlaceholderReference(ProcessInput&& src) {
		return std::move(src);
	}
};
