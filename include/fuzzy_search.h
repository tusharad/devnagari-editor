vector<string> splitString(const string& input){
    vector<string> result;
    string temp = "";
    for(auto c : input){
        if(c == ' '){
            result.push_back(temp);
            temp = "";
        }else
            temp += c;
    }
    result.push_back(temp);
    return result;
}

string joinString(vector<string>& vec,int start,int size){
    string res = "";
    for(int i = start;i < start+size;i++)
        res += vec[i] + " ";
    return res;
}


int levensteinDistance(const string& searchingString, const string& chunk){
    int m = searchingString.length();
    int n = chunk.length();
    vector<vector<int>> dp(m+1,vector<int> (n+1));

    for(int i = 0;i <= m;i++)
        dp[i][0] = i;

    for(int i = 0;i <= n;i++)
        dp[0][i] = i;

    for(int i = 1;i <= m;i++){
        for(int j = 1;j <= n;++j){
            if(searchingString[i - 1] == chunk[j - 1])
                dp[i][j] = dp[i-1][j-1];
            else{
                dp[i][j] = 1 + min(dp[i - 1][j], dp[i][j - 1]);
            }
        }
    }
    return dp[m][n];
}

bool closerString(unordered_map<string,int>& mp,string& str){
    if(mp.find(str) != mp.end())
        return true;
    for(auto it : mp){
        if (((str.length() > it.first.length()) ? str.length() - it.first.length() : it.first.length()-str.length()) >= 3)
            continue;
        int distance = levensteinDistance(str,it.first);
        if(distance <= 2)
            return true;
    }
    return false;
}


void similarStrings(const string& inputString,const string& textString, priority_queue< pair<float,string>, vector<pair<float,string>> ,greater<pair<float,string>> > &pq){
    vector<string> inputWords = splitString(inputString);
    int inputLength = inputWords.size();

    vector<string> textWords = splitString(textString);
    int textLength = textWords.size();

    unordered_map<string,int> mp;
    for(auto word : inputWords)
        mp[word]++;

    int numberOfMatchedWords = 0;
    set<string> matchedWords;
    
    for(int i = 0;i < 2*inputLength;i++){
        if(mp.find(textWords[i]) != mp.end())
            matchedWords.insert(textWords[i]);
    }
    if(matchedWords.size() > inputLength/2)
        pq.push({matchedWords.size(),joinString(textWords, 0, 2*inputLength)});

    for(int i = 2*inputLength;i < textLength;i++){
        if(mp.find(textWords[i]) != mp.end())
            matchedWords.insert(textWords[i]);
        if(mp.find(textWords[i-2*inputLength]) != mp.end())
            matchedWords.erase(textWords[i-2*inputLength]);
        if (matchedWords.size() > inputLength / 2){
            int start = i-2*inputLength;
            int end = i;
            while(start < end && matchedWords.count(textWords[start]) != 1)
                start++;
            pq.push({matchedWords.size() - ((end-start+1) - inputLength)*0.125,joinString(textWords, start, end-start+1)});
        }
        if(pq.size() > 10)
            pq.pop();
    }

    /*
       for(int i = 0;i < textWords.size() - inputWords.size();i++){
       numberOfMatchedWords = 0;
       for(int j = i;j < i + inputWords.size();j++){
       if(mp.find(textWords[j]) != mp.end())
       numberOfMatchedWords++;
       }
       if (numberOfMatchedWords > inputLength / 2)
       pq.push({numberOfMatchedWords,joinString(textWords, i, inputLength)});
       if(pq.size() > 10)
       pq.pop();
       }
       */
} 


void TextEditor::searching(string& searchingString,vector<string>& result){
    priority_queue< pair<float,string>, vector<pair<float,string>> ,greater<pair<float,string>> > pq;
    for(auto file : filenames){
        ifstream input(file);
        string line;
        string chunk;
        while(getline(input,line)){
            chunk += line;
            if(chunk.size() >= 4096){
                similarStrings(searchingString,chunk,pq);
                chunk.clear();
            }
        }
        similarStrings(searchingString,chunk,pq);
        input.close();
    }
    while(!pq.empty()){
        result.push_back(pq.top().second);
        pq.pop();
    }
    reverse(result.begin(),result.end());
}

void TextEditor::fuzzy_search(string searchingString){
    vector<string> result;
    auto start = chrono::high_resolution_clock::now();
    searching(searchingString,result);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end-start;
    cout<<"Execution time: "<<duration.count()<<" seconds\n";

    int i = 1;
    for(auto suggestion : result){
        Gtk::Label* label = Gtk::manage(new Gtk::Label(to_string(i)+". "+suggestion));
        label->override_background_color(Gdk::RGBA("white"));
        label->override_color(Gdk::RGBA("black"));
        m_suggestionList->append(*label);
        i++;
    }
    show_all();
}
