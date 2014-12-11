<<<<<<< HEAD
#ifdef _WIN32
=======
#ifdef win32
>>>>>>> origin/master
#include <windows.h>
#endif
#ifdef unix
#include <iostream>
#endif
<<<<<<< HEAD
#include <c:\build\refeus_executable_updated\include\RefeusFunctionsDefinitions.hpp>

#ifdef _WIN32
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) {

  RefeusProcess r;
=======

#include <RefeusProcess.hpp>
 
#ifdef win32
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) {
#endif
#ifdef unix
int main(int argc, char** argv) {
#endif
  RefeusProcess r;
  #ifdef win32
>>>>>>> origin/master
  char *command_line_cstr = GetCommandLine();
  std::string command_line = command_line_cstr;
  r.argParser(command_line);
  r.langCheck(GetUserDefaultLCID());
<<<<<<< HEAD
  //r.start();
  //return r.start();
  //system("pause");
  }
  #endif
#ifdef unix
int main(int argc, char** argv) {
  RefeusProcess r;
=======
  #endif
  #ifdef unix
>>>>>>> origin/master
  std::string command_line= "";
  for ( int i = 1
      ; i < argc
      ; i++
      ) {
    command_line.append(std::string(argv[i]).append(" "));
  }
  r.argParser(command_line);
  r.langCheck(1031);
<<<<<<< HEAD
  
  //rp.setEnvironment("window_size", "400x800");
  //rip.setExecutable("node.exe");
  //rip.setExecutableParameter("app.js");
  r.start();
  //return r.start();
=======
  #endif
  //rp.setEnvironment("window_size", "400x800");
  //rip.setExecutable("node.exe");
  //rip.setExecutableParameter("app.js");
  //r.start();
  return r.start();
>>>>>>> origin/master
  //rip.start();
  //system("pause");
 
}
#endif
