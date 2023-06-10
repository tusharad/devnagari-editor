void TextEditor::on_menu_file_open() {
  // Create a file chooser dialog
  Gtk::FileChooserDialog dialog("Please choose a file",
                                Gtk::FILE_CHOOSER_ACTION_OPEN);
  dialog.set_transient_for(*this);

  // Add response buttons to the dialog
  dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
  dialog.add_button("_Open", Gtk::RESPONSE_OK);

  // Show the dialog and wait for a response
  int result = dialog.run();

  if (result == Gtk::RESPONSE_OK) {
    // Get the selected file path
    string filename = dialog.get_filename();

    // Read the contents of the selected file into a string
    ifstream infile(filename);
    stringstream buffer;
    buffer << infile.rdbuf();
    string file_content = buffer.str();
    infile.close();

    // Set the contents of the text buffer to the contents of the selected file
    m_refTextBuffer->set_text(file_content);
  }
}

void TextEditor::on_menu_file_save() {
  cout << "Save clicked" << endl;
  // Add your save logic here
  Gtk::FileChooserDialog dialog("Please choose a file",
                                Gtk::FILE_CHOOSER_ACTION_SAVE);
  dialog.set_transient_for(*this);

  // Add response buttons to the dialog
  dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
  dialog.add_button("_Save", Gtk::RESPONSE_OK);

  // Show the dialog and wait for a response
  int result = dialog.run();

  if (result == Gtk::RESPONSE_OK) {
    // Get the selected file path
    string filename = dialog.get_filename();

    // Save the contents of the text buffer to the selected file
    ofstream outfile(filename);
    outfile << m_refTextBuffer->get_text();
    outfile.close();
  }
}

void TextEditor::on_menu_file_read() {
  // Create a file chooser dialog
  Gtk::FileChooserDialog dialog("Please choose a file",
                                Gtk::FILE_CHOOSER_ACTION_OPEN);
  dialog.set_select_multiple(true);
  dialog.set_transient_for(*this);

  // Add response buttons to the dialog
  dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
  dialog.add_button("_Read", Gtk::RESPONSE_OK);

  // Show the dialog and wait for a response
  int result = dialog.run();

  if (result == Gtk::RESPONSE_OK) {
    // Get the selected file paths
    filenames = dialog.get_filenames();
  }
}
