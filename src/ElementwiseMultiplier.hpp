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

		dst.resize(src.size());

		std::transform(std::execution::par_unseq, src.begin(), src.end(), values.begin(), dst.begin(), [](auto&lhs, auto&rhs) {
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

	virtual InitializationTypes ReadParameters(tinyxml2::XMLElement* root) const {
		InitInput dst;
		auto arr = root->FirstChildElement();

		std::size_t cnt = 0;
		for (auto el = arr->FirstChildElement(); el; el = el->NextSiblingElement())
			++cnt;
		dst.reserve(cnt);
		for (auto el = arr->FirstChildElement(); el; el = el->NextSiblingElement()) 
			dst.emplace_back(folly::to<typename InputContainer::value_type>(BaseType::ReadString(el)));

		//dst.resize(10000000);

		return dst;
	}
	
	CALLWRAPPER

	static auto ElementwiseMultiplierReference(const ProcessInput& src, const std::vector<typename InputContainer::value_type>& kernels) {
		assert(src.size() == kernels.size());
		ProcessOutput dst{};

		dst.resize(src.size());

		std::transform(std::execution::par_unseq, src.begin(), src.end(), kernels.begin(), dst.begin(), [](auto&lhs, auto&rhs) {
			return lhs * rhs;
		});

		return dst;
	}
};