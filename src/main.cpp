#include "ChainProcessing.hpp"
#include <iostream>
#include <chrono>

int main() {
#if debug_constructors
	auto a = Multiplier<std::vector<double>>();

	std::vector<double> processing_vector(100000000);
	std::iota(processing_vector.begin(), processing_vector.end(), 0.0);
	a.Init(.0001);
	auto a_dst = a.GetResult(a.Process(processing_vector));

	auto b = Accumulator<std::vector<double>>();
	auto b_dst = b.GetResult(b.Process(a_dst));
#endif

#if debug_chaining
	using C = std::vector<double>;
	using Common = CommonProcessing<C>;
	std::vector<std::unique_ptr<Common>> processing_vector;
	processing_vector.emplace_back(new Multiplier<C>(0.0001));
	processing_vector.emplace_back(new Multiplier<C>(0.001));
	processing_vector.emplace_back(new Multiplier<C>(0.01));
	processing_vector.emplace_back(new Multiplier<C>(0.1));
	processing_vector.emplace_back(new Accumulator<C>());
	processing_vector.emplace_back(new InverseSign<C>());

	C data_vec(1000000);
	std::iota(data_vec.begin(), data_vec.end(), 0.0);

	std::vector<Common::InputOutputTypes> results;
	results.emplace_back(std::move(data_vec));
	for (auto&& function_ptr : processing_vector) 
		results.emplace_back(function_ptr->Process(std::move(results.back())));
	
#endif
	try {
		ChainProcessing c("..\\..\\..\\xml\\example.xml");
		std::vector<double> data_vec{ 0.1, -0.1, 0.2, -0.2 };
		auto dst = c.Process(std::move(data_vec));
	}
	catch (std::exception&e) {
		auto a = e;
	}
}