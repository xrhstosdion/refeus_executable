#include <windows.h>
#include "refeus_functions.cpp"
 
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
					LPSTR lpCmdLine, int nCmdShow ){                       
  RefeusProcess r, rp, rip;
  r.argParser("");
  rp.argParser(GetCommandLine());
  rp.setEnvironment("refeus_database", "first_start_example.docdb");
  //rp.setEnvironment("window_size", "400x800");
  //rip.setExecutable("node.exe");
  //rip.setExecutableParameter("app.js");
  //r.start();
  rp.langCheck(GetUserDefaultLCID());
  rp.start();
  //rip.start();
  //system("pause");
}
