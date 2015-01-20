#ifdef _WIN32
#include <windows.h>
#endif
#ifdef unix
#include <iostream>
#endif
#include <RefeusProcess.h>

#ifdef _WIN32
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) {

  RefeusProcess r;
  char *command_line_cstr = GetCommandLine();
  std::string command_line = command_line_cstr;
  r.argParser(command_line);
  r.langCheck(GetUserDefaultLCID());
  r.start();
  //return r.start();
  //system("pause");
  }
  #endif
#ifdef unix
int main(int argc, char** argv) {
  RefeusProcess r;
  std::string command_line= "";
  for ( int i = 1
      ; i < argc
      ; i++
      ) {
    command_line.append(std::string(argv[i]).append(" "));
  }
  r.argParser(command_line);
  r.langCheck(1031);
  
  //rp.setEnvironment("window_size", "400x800");
  //rip.setExecutable("node.exe");
  //rip.setExecutableParameter("app.js");
  r.start();
  //return r.start();
  //rip.start();
  //system("pause");
}
#endif
