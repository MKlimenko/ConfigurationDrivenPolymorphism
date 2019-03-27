#pragma once

#include "CommonProcessing.hpp"

template <typename InputContainer, bool keep_previous = false>
class Accumulator final : public CommonProcessing <InputContainer, keep_previous> {
public:
	using BaseType = CommonProcessing<InputContainer, keep_previous>;
	using InitializationTypes = typename BaseType::InitializationTypes;
	using InputOutputTypes = typename BaseType::InputOutputTypes;
	using ProcessInput = InputContainer;
	using ProcessOutput = typename InputContainer::value_type;

private:
	ProcessOutput Process(const ProcessInput& src) {
		return std::accumulate(src.begin(), src.end(), ProcessOutput());
	}

	ProcessOutput Process(ProcessInput&& src) {
		return Process(src);
	}

	virtual std::unique_ptr<BaseType> Clone(InitializationTypes&& values) const override {
		return std::unique_ptr<BaseType>(new Accumulator());
	}
	
public:
	virtual std::unique_ptr<BaseType> Clone() const override {
		return std::unique_ptr<BaseType>(new Accumulator());
	}

	virtual InitializationTypes ReadParameter(tinyxml2::XMLElement* root) const {
		return InitializationTypes{};
	}

	PROCESSWRAPPER

	static auto Reference(const ProcessInput& src) {
		return std::accumulate(src.begin(), src.end(), ProcessOutput());
	}	
	static auto Reference(ProcessInput&& src) {
		return Reference(src);
	}
};
