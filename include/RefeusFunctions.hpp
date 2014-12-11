#pragma once
#include <string>
#include <map>
#include <vector>

class RefeusProcess {
    private: std::map<std::string,std::string> environmentmap;
    private: std::string executable;
    private: std::vector<std::string> parametersvector;

    private: void configureCloudSetting();
    private: void configureDebug();
    private: void configureNewRefeusDocument();
    private: void configureOpenRefeusDocument(std::string pathname);
    private: void usage();

    public: RefeusProcess();
    public: bool argParser(std::string command_line);
    public: void langCheck(int check);
    public: void setEnvironment(std::string env_name, std::string env_value);
    public: std::vector<std::string> &split(const std::string &string_to_split, char *delimiter_character, std::vector<std::string> &element_vector);
    public: int start();
};
