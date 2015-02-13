#include <windows.h>
#pragma comment(lib,"user32.lib")
#include <iostream>
#include <sstream>
#include <string>
#include <config.h>

#include <ArgParserConfiguration.h>
#include <ConfigureFunctions.h>
#define std_debug(expr) {std::stringstream ss; ss << expr; _debug(ss.str());}


/**
 * get the next argument from a vector that is split by blanks
 * @return string
 * @param arguments_separated_by_blank (unmodifyable vector, passed by reference for performance)
 * @param start_iterator reference to move iterator for the argParser
 * --last "noblank"
 * --last "with blank"
 * --last "with many blanks"
 * --last " "
 * --last " beforeblank"
 * --last "afterblank "
 * --last " centeredinblanks "
 * --last " escaped\"quote " (no working!)
 * --last " escaped \"quote with\" blank " (no working!)
 */
std::string ArgParserConfiguration::argParserNext(const std::vector<std::string> &arguments_separated_by_blank, std::vector<std::string>::iterator &start_iterator){
  std::vector<std::string>::iterator it;
  std::string next_arg = "";
  if ( start_iterator == arguments_separated_by_blank.end() ){
    return next_arg;
  }
  bool in_quotes = false;
  next_arg = "";
  for ( it = start_iterator
      ; it != arguments_separated_by_blank.end()
      ; it++
      ){
    if ( (*it).size() == 0 ){
      // empty string no next arg
      break;
    }

    if ( (*it).at(0) == '\"'
      && (*it).at((*it).size()-1) != '\"'
      && (*it).size() > 1
      ){
      //std_debug("begin quotes");
      next_arg = (*it).substr(1,(*it).size()-1);
      in_quotes = true;
      continue;
    }
    if ( (*it).at(0) == '\"'
      && (*it).at((*it).size()-1) == '\"'
      && (*it).size() > 1
      ){
      //std_debug("full-quoted");
      next_arg = (*it).substr(1,(*it).size()-2);
      break;
    }

    if ( in_quotes && (*it).at((*it).size()-1) == '\"' ){
      //std_debug("end quotes");
      next_arg+= " " + (*it).substr(0,(*it).size()-1);
      break;
    }

    if ( in_quotes ){
      next_arg+= " " + *it;
      continue;
    }
    next_arg+= *it;
    break;
  }
  start_iterator = ++it;
  //std_debug("next_arg: ret:[" << next_arg << "]");
  return next_arg;
}

/** Argument Parser parses takes as parameter the command line then parses
  *  all the arguments that are separated from each other with a single blank
  *  --help outputs all possible arguments
  *  --new opens a 'save file as' dialog window
  *  --open "C:/works with/spaces too.txt" but path name must be inside " "
  *  --refeus sets to refeus.ini 
  *  --plus sets to plus.ini
  *  --cloud-enabled sets CLOUD_ENABLED to true
  * "[c:/]path/to/filename" (only parameter, always quoted)
  */
bool ArgParserConfiguration::argParser(std::string command_line) {
  std_debug(command_line);
  std::vector<std::string> per_blank_vector;
  std::vector<std::string>::iterator it;
  std::string document_path;
  char module_path_c[MAX_PATH];
  GetModuleFileName(NULL,module_path_c,sizeof(module_path_c));
  std::string module_path = module_path_c; // including xxx.exe
  std::string quoted_module_path = "\"" + module_path + "\"";
  if ( command_line.find(module_path) == 0 ){
    std_debug("unquoted module: [" << command_line << "]");
    command_line = command_line.substr(module_path.size() + 1,command_line.size() - module_path.size() - 1);
  }
  if ( command_line.find(quoted_module_path) == 0 ){
    command_line = command_line.substr(quoted_module_path.size(),command_line.size() - quoted_module_path.size());
    std_debug("quoted module: [" << command_line << "]");
  }
  /** eat leading whitespaces */
  while ( command_line.size() && command_line.at(0) == ' ' ){
    command_line = command_line.substr(1,command_line.size() - 1);
  }
  /** skip parsing when and interpet all as quoted filename
   * may also match ["filename" --language "de"], but we wont care!
   */
  if ( command_line.size() > 2
    && command_line.at(0) == '\"'
    && command_line.at(command_line.size()-1) == '\"'
	){
	document_path = command_line.substr(1,command_line.size()-1);
	configureOpenRefeusDocument(document_path);
	return true;
  }
  //splitting command line per single blanks
  split(command_line, ' ', per_blank_vector);
  for ( it = per_blank_vector.begin()
      ; it < per_blank_vector.end()
      ; ++it
      ){
    //std_debug("param: [" << *it << "]");
    if ( *it == "--help"
       ||*it == "/?"
       ){
      usage();
      return false;
    }
    if ( *it == "--new" ){
      configureNewRefeusDocument();
    } else if ( *it == "--open" ){
      ++it; // scroll to next (careful, processing in for-loop)
      document_path = argParserNext(per_blank_vector, it);
      if ( document_path != "" ){
	    configureOpenRefeusDocument(document_path);
      } else {
        configureNewRefeusDocument();
      }
      if ( it == per_blank_vector.end() ){
        break;
      }
    } else if ( *it == "--plus" ){
      parametersvector.clear();
      parametersvector.push_back("plus.ini");
    } else if ( *it == "--refeus" ){
      parametersvector.clear();
      parametersvector.push_back("refeus.ini");
    } else if ( *it == "--cloud-enabled" ){
      configureCloudSetting();
    } else if ( *it == "--debug" ){
      configureDebug();
    } else if ( *it == "--language" ){
      ++it; // scroll to next (careful, processing in for-loop)
      std::string iso_language = argParserNext(per_blank_vector, it);
      configureLanguageFromIsoString(iso_language);
      if ( it == per_blank_vector.end() ){
        break;
      }
    } else if ( *it == "--auto-backup" ){
      configureAutoBackup(true);
    } else if ( *it == "--no-auto-backup" ){
      configureAutoBackup(false);
    } else if ( *it == "--skip-maintenance" ){
      configureSkipMaintenance(true);
    } else if ( *it == "--no-skip-maintenance" ){
      configureSkipMaintenance(false);
    } else if ( *it == "--startup-activity" ){
      ++it; // scroll to next (careful, processing in for-loop)
      std::string startup_activity = argParserNext(per_blank_vector, it);
      configureStartupActivity(startup_activity);
      if ( it == per_blank_vector.end() ){
        break;
      }
    } else if ( *it == "--infopool" ){
      configureInfopool();
    } else if ( *it == "--portable" ){
      configurePortable();
    } else {
      #ifndef _WIN32
      /* does not work in win32 because $0 is always part of the commandline and cannot be correctly predicted */
      usage();
      return false;
      #endif
    }
    /**
     * REFEUS_SETTINGS_LOCATION=[when set: ini-file for portable]
     * REFEUS_DOCUMENTS_LOCATION=path/to/documents
     * REFEUS_PICTURES_LOCATION=path/to/pictures
     */
  }
  return true;
}

/** \brief Split String into Vector using a delimter
  * \param string_to_split - some string to be split by a specific character
  * \param delimiter_character - character for splitting
  * \param element_vector - reference vector for storing the split result
  */
std::vector<std::string> &ArgParserConfiguration::split(
    const std::string &string_to_split, const char delimiter_character,
    std::vector<std::string> &element_vector) {
  std::stringstream sstream(string_to_split);
  std::string item;
  while ( std::getline(sstream, item, delimiter_character) ){
    element_vector.push_back(item);
  }
  return element_vector;
}
