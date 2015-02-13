#pragma once
#include <ConfigureFunctions.h>

class ArgParserConfiguration: public ConfigureFunctions{
 
    private: std::string argParserNext(const std::vector<std::string> &arguments_separated_by_blank, std::vector<std::string>::iterator &start_iterator);
    private: std::vector<std::string> &split(const std::string &string_to_split, const char delimiter_character, std::vector<std::string> &element_vector);
    public: bool argParser(std::string command_line);
    
};
