#include "include/text_editor.h"
#include "include/menu_bar.h"
#include "include/fuzzy_search.h"
#include "include/auto_complete.h"

int main(int argc, char *argv[]) {
  auto app = Gtk::Application::create(argc, argv);

  TextEditor editor;
  app->run(editor);

  return 0;
}
