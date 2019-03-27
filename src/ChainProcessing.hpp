#include "CommonProcessing.hpp"
#include "Accumulator.hpp"
#include "ElementwiseMultiplier.hpp"
#include "InverseSign.hpp"
#include "Multiplier.hpp"
#include "Placeholder.hpp"

#include "FunctionEntry.hpp"

#include <filesystem>

#define NAMEOF(x) #x

template <typename InputContainer = std::vector<double>, bool keep_previous = false>
class ChainProcessing final {
private:
	using BaseProcessing = CommonProcessing<InputContainer, keep_previous>;

	// should be constexpr
	static auto GetMap() {
		std::unordered_map<std::string, std::unique_ptr<BaseProcessing>> map;
		map.emplace(NAMEOF(Multiplier),				new Multiplier<InputContainer, keep_previous>{});
		map.emplace(NAMEOF(Accumulator),			new Accumulator<InputContainer, keep_previous>{});
		map.emplace(NAMEOF(InverseSign),			new InverseSign<InputContainer, keep_previous>{});
		map.emplace(NAMEOF(ElementwiseMultiplier),	new ElementwiseMultiplier<InputContainer, keep_previous>{});
		map.emplace(NAMEOF(Placeholder),			new Placeholder<InputContainer, keep_previous>{});

		return map;
	}

	std::vector<std::unique_ptr<BaseProcessing>> processing_vector;
	std::vector<typename CommonProcessing<InputContainer>::InputOutputTypes> results_vector;

	void InitializeProcessingVector(
		std::vector<FunctionEntry<typename BaseProcessing::InitializationTypes>>&& read_functions,
		const std::unordered_map<std::string, std::unique_ptr<BaseProcessing>>& function_map
	) {
		for (auto&read_function : read_functions)
			if (!read_function)
				processing_vector.emplace_back(function_map.at(read_function)->Clone());
			else
				processing_vector.emplace_back(function_map.at(read_function)->Clone(std::move(read_function)));

		if constexpr (keep_previous) 
			results_vector.reserve(processing_vector.size() + 1); // + input
	}

public:
	// for debug purposes
	ChainProcessing() {
		static auto function_map = GetMap();
		// Initialize somehow
		std::vector<FunctionEntry<BaseProcessing::InitializationTypes>> read_functions = {
			{"Multiplier",				true,		std::tuple(-0.001, 1.0 / 125057)},
			{"ElementwiseMultiplier",	true,		InputContainer(1000000, 1)},
			{"Accumulator",				false,		0},
			{"InverseSign",				false,		0},
			{"Placeholder",				false,		0},
		};

		InitializeProcessingVector(std::move(read_functions), function_map);
	}

	ChainProcessing(const std::string& config_path) {
		static auto function_map = GetMap();
		auto read_functions = FunctionEntry<BaseProcessing::InitializationTypes>::ReadConfiguration(config_path, function_map);
		InitializeProcessingVector(std::move(read_functions), function_map);
	}

	auto Process(InputContainer src) {
		if constexpr (keep_previous) {
			results_vector.clear();
			results_vector.emplace_back(std::move(src));
			for (auto&& function_ptr : processing_vector)
				results_vector.emplace_back(function_ptr->Process(results_vector.back()));

			return std::move(results_vector.back());
		}
		else {
			typename CommonProcessing<InputContainer>::InputOutputTypes dst = std::move(src);

			for (auto&& function_ptr : processing_vector) {
				dst = function_ptr->Process(std::move(dst));
			}

			return dst;
		}
	}
};
