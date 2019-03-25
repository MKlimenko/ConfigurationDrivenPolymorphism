#include "CommonProcessing.hpp"
#include "Accumulator.hpp"
#include "ElementwiseMultiplier.hpp"
#include "InverseSign.hpp"
#include "Multiplier.hpp"

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

		return map;
	}

	std::vector<std::unique_ptr<BaseProcessing>> processing_vector;
	
	struct FunctionEntry {
	private:
		using variant_list = typename BaseProcessing::InitializationTypes;
		std::string function_name{};
		bool has_arguments = false;
		variant_list parameters = 0.0;

	public:
		FunctionEntry(std::string name, bool p, variant_list vl) : 
			function_name(std::move(name)),
			has_arguments(p), 
			parameters(std::move(vl)) {}

		operator std::string& () {
			return function_name;
		}

		operator bool() {
			return has_arguments;
		}
		operator variant_list() && {
			return std::move(parameters);
		}
	};

public:
	ChainProcessing() {
		static auto function_map = GetMap();

		std::vector<FunctionEntry> read_functions = {
			{"Multiplier",				true, std::tuple(-0.001, 1.0 / 125057)},
			{"ElementwiseMultiplier",	true, InputContainer(1000000, 1)},
			{"Accumulator",				false, 0},
			{"InverseSign",				false, 0},
		};

		// Initialize somehow
		for (auto&read_function : read_functions)
			if(!read_function)
				processing_vector.emplace_back(function_map.at(read_function)->Clone());
			else
				processing_vector.emplace_back(function_map.at(read_function)->Clone(std::move(read_function)));

	}

	auto Process(InputContainer src) {
		std::vector<typename CommonProcessing<InputContainer>::InputOutputTypes> results;
		results.reserve(processing_vector.size() + 1); // + input
		results.emplace_back(std::move(src));
		for (auto&& function_ptr : processing_vector)
			results.emplace_back(function_ptr->Process(std::move(results.back())));

		if constexpr (keep_previous)
			return results;
		else
			return std::move(results.back());
	}
};
