// Name        : main.cc
// Author      : Alan Buckley
// Copyright   : This is released under the same COPYRIGHT as TBX
// Description : Program start point. Setup tbx application and iconbar.
//

#include "tbx/application.h"
#include "tbx/autocreatelistener.h"
#include "IconbarHandler.h"

using namespace std;

int main()
{
   tbx::Application my_app("<TbxClip$Dir>");

   // Handler for clicks on iconbar
   IconbarHandler iconbar_handler;
   // Attach to iconbar resource when it's created
   my_app.set_autocreate_listener("Iconbar", &iconbar_handler);

   my_app.run();

   return 0;
}
