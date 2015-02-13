#pragma once
#include <string>
#include <map>
#include <vector>


class ConfigureFunctions {
 
    private: bool portable;
    private: std::string selected_iso_language;
    public: std::string executable;
    public: std::vector<std::string> parametersvector;
    public: std::map<std::string,std::string> environmentmap;
    
    public: bool debug;
    public: void _debug(std::string message,std::string title = "");

    public: void configureAutoBackup(bool enabled);
    public: void configureCloudSetting();
    public: void configureDebug();
    public: void configureNewRefeusDocument();
    public: void configureInfopool();
    public: void configureOpenRefeusDocument();
    public: void configureOpenRefeusDocument(std::string pathname);
    public: void configureSkipMaintenance(bool enabled);
    public: void configureStartupActivity(std::string activity_name);
    public: void configureLanguageFromAPICode(int check);
    public: void configurePortable();
    public: void configureLanguageFromIsoString(std::string iso_language);
    public: void configureRefeusSettingsLocation(std::string refeus_set_location);
    
    public: void usage();
    public: ConfigureFunctions();
    public: std::string configureBinPath();
    public: void configureDriveType();
    public: void configureLanguage();
    public: void setEnvironment(std::string env_name, std::string env_value);
 
};
