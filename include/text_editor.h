#include <algorithm>
#include <fstream>
#include <gtkmm.h>
#include <iostream>
#include <string>
#include <thread>
#include <exception>
#include <numeric>
#include <queue>
#include <chrono>
using namespace std;

class TextEditor : public Gtk::Window {
    public:
        TextEditor();
        virtual ~TextEditor();

    protected:
        void on_menu_file_save();
        void on_menu_file_open();
        void on_menu_file_read();
        void on_textview_changed();
        vector<string> read_data_file(const string);
        string get_text_from_end_till_stop();
        void remove_last_word_and_insert_selected(string);
        void add_lines(string);
        void insert_selected_row(const Gtk::ListBoxRow *);
        //fuzzy search
        void fuzzy_search(string);
        void searching(string&,vector<string>&);
        // Member widgets:
        Gtk::Box m_VBox;
        Gtk::MenuBar m_MenuBar;
        Gtk::ScrolledWindow m_ScrolledWindow;
        Gtk::TextView m_TextView;
        Gtk::Paned* pane;
        Glib::RefPtr<Gtk::TextBuffer> m_refTextBuffer;
        Gtk::ListBox m_WordList;
        Gtk::ListBox *m_suggestionList;
        vector<string> m_words;
        vector<string> cache_words;
        vector<string> filenames;
        ofstream cacheFile;
};

TextEditor::TextEditor() : m_VBox(Gtk::ORIENTATION_VERTICAL, 0) {
 

    set_title("Text Editor");
    set_default_size(800, 600);

    // Menu bar
    Gtk::MenuItem *item_menu_file = Gtk::manage(new Gtk::MenuItem("_File", true));
    Gtk::Menu *menu_file = Gtk::manage(new Gtk::Menu());

    item_menu_file->set_submenu(*menu_file);
    m_MenuBar.append(*item_menu_file);
    m_VBox.pack_start(m_MenuBar, Gtk::PACK_SHRINK);

    Gtk::MenuItem *item_menu_file_open =
        Gtk::manage(new Gtk::MenuItem("_Open", true));
    menu_file->append(*item_menu_file_open);
    // File menu
    Gtk::MenuItem *item_menu_file_save =
        Gtk::manage(new Gtk::MenuItem("_Save", true));
    menu_file->append(*item_menu_file_save);

    Gtk::MenuItem *item_menu_file_read =
        Gtk::manage(new Gtk::MenuItem("_Read", true));
    menu_file->append(*item_menu_file_read);

    item_menu_file_open->signal_activate().connect(
            sigc::mem_fun(*this, &TextEditor::on_menu_file_open));
    item_menu_file_save->signal_activate().connect(
            sigc::mem_fun(*this, &TextEditor::on_menu_file_save));
    item_menu_file_read->signal_activate().connect(
            sigc::mem_fun(*this, &TextEditor::on_menu_file_read));

    // Text view
    m_refTextBuffer = Gtk::TextBuffer::create();
    m_TextView.set_buffer(m_refTextBuffer);
    m_TextView.override_font(Pango::FontDescription("Sans 16"));

    m_ScrolledWindow.add(m_TextView);

    //right side suggestions
    m_suggestionList = Gtk::manage(new Gtk::ListBox());
    Gtk::ScrolledWindow* list_scroll = Gtk::manage(new Gtk::ScrolledWindow());
    list_scroll->add(*m_suggestionList);

    pane = Gtk::manage(new Gtk::Paned(Gtk::ORIENTATION_HORIZONTAL));
    pane->pack1(m_ScrolledWindow,true,true);
    pane->pack2(*list_scroll, false, true);
    pane->set_position(500);
    //m_VBox.pack_start(m_ScrolledWindow);
    m_VBox.pack_start(*pane,true,true,0);

    // ListBox for showing words
    m_WordList.set_size_request(-1,get_allocated_height() * 0.3); // Set the size to 30% of the window height
    m_VBox.pack_end(m_WordList, Gtk::PACK_SHRINK);

    add(m_VBox);

    // Connect signal for text buffer changes
    m_refTextBuffer->signal_changed().connect(sigc::mem_fun(*this, &TextEditor::on_textview_changed));

    m_WordList.signal_row_activated().connect([this](const Gtk::ListBoxRow *row) { insert_selected_row(row); });

    m_VBox.signal_key_press_event().connect([this](GdkEventKey *event) {
            if (event->keyval == GDK_KEY_0 && event->state & GDK_CONTROL_MASK) {

            // Get the selected text from the text editor
            Gtk::TextBuffer::iterator start, end;
            m_TextView.get_buffer()->get_selection_bounds(start, end);
            Glib::ustring selected_text = m_TextView.get_buffer()->get_text(start, end);
 
        Gtk::Label* search_label = Gtk::manage(new Gtk::Label("searching"));
        m_suggestionList->append(*search_label);
        search_label->show();

            // Create a new thread and call fuzzy_search() function
            thread search_thread([this,selected_text](){
                    fuzzy_search(selected_text);
                  });
            search_thread.detach();
            //search_thread.join();

 

            return false;
            }

            else if(event->keyval >= GDK_KEY_1 && event->keyval <= GDK_KEY_9 && event->state & GDK_CONTROL_MASK){
    
                int row_number = -1;
                switch(event->keyval){
                    case GDK_KEY_1:
                        row_number = 1;
                    break;
                    case GDK_KEY_2:
                        row_number = 2;
                    break;
                    case GDK_KEY_3:
                        row_number = 3;
                    break;
                    case GDK_KEY_4:
                        row_number = 4;
                    break;
                    case GDK_KEY_5:
                        row_number = 5;
                    break;
                    case GDK_KEY_6:
                        row_number = 6;
                    break;
                    case GDK_KEY_7:
                        row_number = 7;
                    break;
                    case GDK_KEY_8:
                        row_number = 8;
                    break;
                    case GDK_KEY_9:
                        row_number = 9;
                    break;
                }
            // Select the corresponding row in the ListBox
            if (row_number >= 0) {
                auto rows = m_suggestionList->get_children();
                if (row_number < rows.size()) {
                    Gtk::ListBoxRow *row =
                        dynamic_cast<Gtk::ListBoxRow *>(rows[row_number]);
                    if (row) {
                        m_suggestionList->select_row(*row);
                        insert_selected_row(row);
                    }
                    return true; // Stop event propagation
                }
            }

            }

            // Check if the Alt key and a number key were pressed
            if (event->state & GDK_MOD1_MASK) {
            int row_number = -1;
            switch (event->keyval) {
                case GDK_KEY_1:
                    row_number = 0;
                    break;
                case GDK_KEY_2:
                    row_number = 1;
                    break;
                case GDK_KEY_3:
                    row_number = 2;
                    break;
                case GDK_KEY_4:
                    row_number = 3;
                    break;
                case GDK_KEY_5:
                    row_number = 4;
                    break;
                case GDK_KEY_6:
                    row_number = 5;
                    break;
                case GDK_KEY_7:
                    row_number = 6;
                    break;
                case GDK_KEY_8:
                    row_number = 7;
                    break;
                case GDK_KEY_9:
                    row_number = 8;
                    break;
            }
            // Select the corresponding row in the ListBox
            if (row_number >= 0) {
                auto rows = m_WordList.get_children();
                if (row_number < rows.size()) {
                    Gtk::ListBoxRow *row =
                        dynamic_cast<Gtk::ListBoxRow *>(rows[row_number]);
                    if (row) {
                        m_WordList.select_row(*row);
                        insert_selected_row(row);
                    }
                    return true; // Stop event propagation
                }
            }
            }
            return false; // Let other signal handlers process the event
    });

    cache_words = read_data_file("text_files/cache_words.txt");
    reverse(cache_words.begin(),cache_words.end());
    m_words = read_data_file("text_files/p.txt");
    cacheFile.open("text_files/cache_words.txt",std::ios::app);
    show_all();
}

TextEditor::~TextEditor() {}

vector<string> TextEditor::read_data_file(const string filename) {
    ifstream file(filename);
    vector<string> words;
    if (file) {
        string line;
        while (getline(file, line)) {
            istringstream iss(line);
            string word;
            while (iss >> word) {
                words.push_back(word);
            }
        }
    }
    return words;
}

void TextEditor::insert_selected_row(const Gtk::ListBoxRow *row) {
    // Get the selected label widget
    const Gtk::Widget *child = row->get_child();
    Gtk::Label *label =
        dynamic_cast<Gtk::Label *>(const_cast<Gtk::Widget *>(child));
    // Get the text of the selected label
    string text = label->get_text();
    size_t pos = text.find(' '); // Find the position of the first space character
    if (pos != string::npos)     // Check if the space character was found
        text = text.substr(pos + 1);
    remove_last_word_and_insert_selected(text);
}

void TextEditor::remove_last_word_and_insert_selected(string text) {
    // Get the text buffer of the TextView
    auto buffer = m_TextView.get_buffer();
    Gtk::TextIter cursor = buffer->get_insert()->get_iter();
    // Move the cursor backward until a space character is found
    while (cursor.backward_char())
        if (cursor.get_char() == ' ')
            break;
    // If the cursor is not at the beginning of the buffer, move it forward by
    // one character
    if (!cursor.is_start())
        cursor.forward_char();
    // Set the new cursor position
    buffer->place_cursor(cursor);
    // Get the end of the buffer
    Gtk::TextIter end = cursor;

    while (end.forward_char())
        if (end.get_char() == ' ')
            break;

    // Delete the text from the cursor position until the end of the buffer
    buffer->erase(cursor, end);
    auto iter = buffer->get_insert()->get_iter();
    // Insert the text of the selected label at the caret position
    buffer->insert(iter, text);
    cacheFile<<text;
    cacheFile<<"\n";
    cache_words.push_back(text);
}
