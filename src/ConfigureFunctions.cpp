#include <windows.h>
#pragma comment(lib,"user32.lib")
#include <iostream>
#include <sstream>
#include <string>
#include <config.h>
#include <ConfigureFunctions.h>

ConfigureFunctions::ConfigureFunctions()
: environmentmap()
, portable(false)
, selected_iso_language("")
#ifdef _WIN32
, executable("refeus.exe")
#else
, executable("refeus.sh")
#endif
, debug(false) {
  parametersvector.push_back("startup.ini");
  configureAutoBackup(true);
}
/**
 * debug helper macro which allows the power of stringstream
 * during debugging. only works when the class it uses implemnts a debug function
 */
#define std_debug(expr) {std::stringstream ss; ss << expr; _debug(ss.str());}
/**
 * debug two strings, messagebox on win, stdout on linux
 */
void ConfigureFunctions::_debug(std::string message,std::string title){
  if ( !debug ){
    return;
  }
  if ( title == "" ){
    title = "debug";
  }
#ifdef _WIN32
  MessageBox(NULL,message.c_str(),title.c_str(), MB_OK);
#else
  std::cout << title << ": [" << message << "]" << std::endl << std::flush;
#endif
}

void ConfigureFunctions::configureAutoBackup(bool enabled){
  if ( enabled ){
    environmentmap["AUTO_BACKUP"] = "YES";
  } else {
    environmentmap["AUTO_BACKUP"] = "NO";
  }
}

/**
 * configure to enable certain features that are only available
 * with the infopool2 + webdav service that allows syncronizing and
 * storing document databases on remote systems
 */
void ConfigureFunctions::configureCloudSetting() {
   environmentmap["CLOUD_ENABLED"] = "true";
}

/**
 * configure debug mode
 */
void ConfigureFunctions::configureDebug() {
  #ifdef _WIN32
  executable = "code.exe"; /** start a commandline window to output debug messages*/
  #endif
  environmentmap["WKE_DEBUG"] = "YES";
  environmentmap["WKE_DEBUG_CONSOLE"] = "YES";
  debug = true;
}

/**
 * show a save-as dialog instead of the startup dialog
 * the given filename will be created and used as the docdb
 */
void ConfigureFunctions::configureNewRefeusDocument() {
   environmentmap["open_refeus_database"] = "true";
}

/**
 * run the infopool nodejs server
 * TODO: taskbar icon to close the application
 */
void ConfigureFunctions::configureInfopool() {
   environmentmap.clear();
   parametersvector.clear();
   #ifdef _WIN32
   executable = "node.exe";
   SetCurrentDirectory("infopool");
   #else
   executable = "node";
   std::string ip_path = DATAROOTDIR;
   ip_path+= "/infopool";
   chdir(ip_path.c_str());
   #endif
   parametersvector.push_back("app.js");
}

/**
  * Function langCheck takes as parameter the Language ID of the system
  * Checks if it is German->French->Polish->English
  * Then sets the appropriate Envir-Variables to the language that got 
  * matched first
  * \param api_language_code - windows-api language code
  */
void ConfigureFunctions::configureLanguageFromAPICode(int api_language_code) {
  switch ( api_language_code ){
    case 1031:  //German
      environmentmap["DSC_Language"] = "German";
      environmentmap["Language"] = "German";
      break;
    case 1036:  //French
      environmentmap["DSC_Language"] = "French";
      environmentmap["Language"] = "French";
      break;
    case 1045:  //Polish
      environmentmap["DSC_Language"] = "Polish";
      environmentmap["Language"] = "Polish";
      break;
    default:  //English
      environmentmap["DSC_Language"] = "English";
      environmentmap["Language"] = "English";
      break;
  }
}

/**
 * based on given string, configure application to load language
 * the implementation could use a better method to map from iso to win32 codes
 */
void ConfigureFunctions::configureLanguageFromIsoString(std::string iso_language) {
  selected_iso_language = iso_language;
}

void ConfigureFunctions::configureLanguage() {
  if ( selected_iso_language == "" ) {
    configureLanguageFromAPICode(GetUserDefaultLCID());
  } else if ( selected_iso_language == "en" ){
    configureLanguageFromAPICode(0); //default
  } else if ( selected_iso_language == "de" ){
    configureLanguageFromAPICode(1031);
  } else if ( selected_iso_language == "fr" ){
    configureLanguageFromAPICode(1036);
  } else if ( selected_iso_language == "pl" ){
    configureLanguageFromAPICode(1045);
  }
}
/**
 * show file-open dialog, not the default startup
 */
void ConfigureFunctions::configureOpenRefeusDocument() {
   environmentmap["open_refeus_database"] = "true";
}

/**
 * show given path_name and open directly
 */
void ConfigureFunctions::configureOpenRefeusDocument(std::string path_name) {
  char abs_path_name_c[MAX_PATH];
  GetFullPathName(path_name.c_str(),MAX_PATH,abs_path_name_c,NULL);
  std_debug("open: " << path_name << "@" << abs_path_name_c);
  environmentmap["refeus_database"] = abs_path_name_c;
  environmentmap["refeus_database_autostart"] = "true";
}

/**
 * configure to skip the database maintenance which can be anoying in
 * large document projects
 */
void ConfigureFunctions::configureSkipMaintenance(bool enabled){
  if ( enabled ){
    environmentmap["SKIP_MAINTENANCE"] = "YES";
  } else {
    environmentmap["SKIP_MAINTENANCE"] = "NO";
  }
}

/**
 * configure to set the default startup activity
 * only useful in combinantion with --open and other
 * options skipping the startup launcher
 */
 void ConfigureFunctions::configureStartupActivity(std::string activity_name){
   environmentmap["STARTUP_ACTIVITY"] = activity_name;
 }
 
/**
 * configure to set the portable option
 * only useful when there is a portable device
 */
void ConfigureFunctions::configurePortable(){
  environmentmap["PORTABLE"] = "true";
  portable = true;
}

/** 
 * Returns the full path of the directory that refeus.exe was called
 * but with //bin path added
 */
std::string ConfigureFunctions::configureBinPath(){
  char module_path_c[MAX_PATH];
  GetModuleFileName(NULL,module_path_c,sizeof(module_path_c));
  std::string module_path = module_path_c;
  std::string bin_path = module_path.substr(0,module_path.rfind("\\")) + "\\bin";
  return bin_path;
}

/** 
 * Configures REFEUS_DOCUMENTS_LOCATION depending on type of current drive
 * DRIVE_REMOVABLE = 2 DRIVE_FIXED(can also be flash drive) = 3
 * DRIVE_REMOTE(network) = 4
 */
void ConfigureFunctions::configureDriveType(){
   bool removable_device = ( GetDriveType( NULL ) == 2 );
  if ( portable ){
    configureRefeusSettingsLocation(configureBinPath());
  }
  else if ( removable_device ){
    configureRefeusSettingsLocation(configureBinPath());
  }
}

/**
 * configure to set REFEUS_DOCUMENTS_LOCATION
 */
void ConfigureFunctions::configureRefeusSettingsLocation(std::string refeus_set_location) {
   environmentmap["REFEUS_SETTINGS_LOCATION"] = refeus_set_location;
}

/**
 * stores the given key-value pair to apply it to the executable
 * that is started in the end
 */
void ConfigureFunctions::setEnvironment(std::string env_name, std::string env_value) {
  environmentmap[env_name] = env_value;
}

/**
 * display a dialog / terminal output with helpful information
 */
void ConfigureFunctions::usage() {
  const char* help_string = "--help displays all possible arguments\n"
       "--new opens a 'save as' dialog window\n"
       "--open \"C:/file_name\" starts refeus with specific file\n"
       "--plus sets the ini file to plus.ini\n"
       "--refeus sets the ini file to refeus.ini\n"
       "--debug sets the application to debug-mode (allows shift+ctrl+i)\n"
       "--cloud-enabled allows operations on webdav storage\n"
       "--[no]-auto-backup enable or disable the automatic backup each time refeus starts\n"
       "--[no]-skip-maintenance enable or disable the initial maintenance check when refeus opens\n" 
       "--language [de|en|fr*|pl*] set default language when application starts\n" 
       "--infopool start local infopool (plus only)\n"
       "--portable for taking data and settings from the application (.conf file)"
       "TODO: add more parameters"
       ;    
  #ifdef _WIN32
  MessageBox(NULL, help_string, "Refeus Executable Usage", MB_OK);
  #endif
  #ifdef unix
  std::cout << help_string << std::endl;
  #endif
}
