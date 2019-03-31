#pragma once

#include "tinyxml2.h"

#include <filesystem>
#include <string>

template <typename variant_list>
class FunctionEntry {
private:
	std::string function_name{};
	variant_list parameters;
	bool has_arguments = false;

public:
	FunctionEntry(std::string name) :
		function_name(std::move(name)) {}

	FunctionEntry(std::string name, variant_list vl) : FunctionEntry(std::move(name), true, std::move(vl)) {}

	FunctionEntry(std::string name, bool p, variant_list vl) :
		function_name(std::move(name)),
		parameters(std::move(vl)),
		has_arguments(p) {}

	operator std::string& () {
		return function_name;
	}

	operator variant_list() && {
		return std::move(parameters);
	}

	operator bool() {
		return has_arguments;
	}

	template <typename map_type>
	static auto ReadConfiguration(const std::string& path, const std::unordered_map<std::string, map_type>& mapa) {
		std::vector<FunctionEntry> dst;
		tinyxml2::XMLDocument doc;
		auto read = doc.LoadFile(path.c_str());
		if (read != tinyxml2::XMLError::XML_SUCCESS)
			throw std::runtime_error(std::string("XML error: ") + tinyxml2::XMLDocument::ErrorIDToName(read));

		auto configuration = doc.FirstChildElement("Configuration");
		if (configuration == 0x0)
			throw std::runtime_error(R"(Document doesn't containt configuration)");

		for (auto child_function = configuration->FirstChildElement(); child_function; child_function = child_function->NextSiblingElement()) {
			std::string name = child_function->FirstChildElement("Name")->GetText();
			auto parameters = child_function->FirstChildElement("Parameters");
			if (parameters) 
				dst.emplace_back(std::move(name), mapa.at(name)->ReadParameters(parameters));
			else 
				dst.emplace_back(std::move(name));
		}

		return dst;
	}
};