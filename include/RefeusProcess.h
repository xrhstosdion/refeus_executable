#pragma once
#include <string>
#include <map>
#include <vector>

class RefeusProcess {
    private: std::map<std::string,std::string> environmentmap;
    private: std::string executable;
    private: std::string selected_iso_language;
    private: std::vector<std::string> parametersvector;
    private: bool portable;
    
    private: bool debug;
    private: void _debug(std::string message,std::string title = "");

    private: std::string argParserNext(const std::vector<std::string> &arguments_separated_by_blank, std::vector<std::string>::iterator &start_iterator);
    private: void configureAutoBackup(bool enabled);
    private: void configureCloudSetting();
    private: void configureDebug();
    private: void configureNewRefeusDocument();
    private: void configureInfopool();
    private: void configureOpenRefeusDocument();
    private: void configureOpenRefeusDocument(std::string pathname);
    private: void configureSkipMaintenance(bool enabled);
    private: void configureStartupActivity(std::string activity_name);
    private: void configureLanguageFromAPICode(int check);
    private: void configurePortable();
    private: void configureLanguageFromIsoString(std::string iso_language);
    private: void configureRefeusSettingsLocation(std::string refeus_set_location);
    private: void usage();
    
    
    public: RefeusProcess();
    public: bool argParser(std::string command_line);
    public: std::string configureBinPath();
    public: void configureDriveType();
    public: void configureLanguage();
    public: void setEnvironment(std::string env_name, std::string env_value);
    public: std::vector<std::string> &split(const std::string &string_to_split, const char delimiter_character, std::vector<std::string> &element_vector);
    public: int start();

};
