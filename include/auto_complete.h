string trimming(string str) {
    string substring2 = "";
    size_t start = str.find_first_not_of(" ");
    size_t end = str.find_last_not_of(" ");
    if (start != string::npos && end != string::npos)
        substring2 = str.substr(start, end - start + 1);
    return substring2;
}

string TextEditor::get_text_from_end_till_stop() {
    auto buffer = m_TextView.get_buffer();
    Gtk::TextIter start = buffer->begin();
    Gtk::TextIter end = buffer->get_insert()->get_iter();
    string text = buffer->get_text(start,end);
    size_t pos = text.find_last_of(" ");
    if (pos != string::npos)
        text = text.substr(pos + 1);
    return text;
}


void TextEditor::on_textview_changed() {

    //removing all lables of suggestions
    auto children = m_WordList.get_children();
    for (auto it : children)
        m_WordList.remove(*it);

    string given_word = get_text_from_end_till_stop();
    if (given_word == "")
        return;

    vector<string> suggestion_words;
    //searching in cache
    for (const auto &word : cache_words) {
        if (word.compare(0, given_word.length(),given_word) == 0)
            if(find(suggestion_words.begin(),suggestion_words.end(),word) == suggestion_words.end())
                suggestion_words.push_back(word);
        if (suggestion_words.size() >= 10)
            break;
    }

    //searching in dictionary (p.txt)
    for (const auto &word : m_words) {
        if (word.compare(0, given_word.length(),given_word) == 0){
            if(find(suggestion_words.begin(),suggestion_words.end(),word) == suggestion_words.end())
                suggestion_words.push_back(word);
        }
        if (suggestion_words.size() >= 10)
            break;
    }

    int i = 1;
    for(auto word : suggestion_words){
        auto label = Gtk::make_managed<Gtk::Label>(to_string(i) + ". " + word);
        label->override_background_color(Gdk::RGBA("white"));
        label->override_color(Gdk::RGBA("black"));
        m_WordList.append(*label);
        label->show();
        i++;
    }
    show_all();
}
