#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

using namespace std;

string input_Pattern = "pattern5.txt" ; 
string input_Text = "text5.txt"; 
string output_file = "patternmatch5.txt" ; 

//prefixtable
vector<int> computeLPS(const string& pattern) {
    int m = pattern.size(); 
    vector<int> lps(m, 0); //store the length of the longest proper prefix which is also a suffix for every prefix of the pattern
    int j = 0;

    for (int i = 1; i < m;) {
        if (pattern[i] == pattern[j]) { //if a match occures 
            j++;
            lps[i] = j;
            i++;
        } else {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
    return lps;
}
//normal kmp
vector<int> kmp(const string& text, const string& pattern) {
    int n = text.size();
    int m = pattern.size();

    vector<int> lps = computeLPS(pattern);
    int i = 0;  // index for text
    int j = 0;  // index for pattern

    vector<int> occurrences;  // to store starting indices of found patterns

    while (i < n) {
        if (pattern[j] == text[i]) {
            i++;
            j++;
        }
        if (j == m) {
            occurrences.push_back(i - j); // pattern found, record the starting index
            j = lps[j - 1];  // reset j to continue the search
        } else if (i < n && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }

    return occurrences;
}
// kmp for dot symbol
vector<int> dotkmp(const string& text, const string& pattern) {
    int n = text.size();
    int m = pattern.size();

    vector<int> lps = computeLPS(pattern);
    int i = 0;  
    int j = 0;  

    vector<int> occurrences;  

    while (i < n) {
        if (pattern[j] == text[i]) {
            i++;
            j++;
        }
        if (pattern[j] == '.'){ //skips the dot 
            i++;
            j++;

        }
        if (j == m) {
            occurrences.push_back(i - j); 
            j = lps[j - 1];  
        } else if (i < n && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }

    return occurrences;
}
//kmp for range [ ]
vector<int> rangekmp(const string& text, const string& pattern){

     

    string test = pattern ;

    int start = test.find('['); //find index of [
    int end = test.find(']'); //find index of ] 

    string listing = test.substr(start+1 , end-start-1); //save the substring ie [substring] into the string listing 
    //cout << listing ;
    vector<int> occurrences; //array to store indexs where kmp finds a match

    for (int i = 0; i < listing.size(); ++i) {  //iterates over all the charcters in the listing 
    string character(1,listing[i]); 

    string temp = test; 

    temp.replace(start, end-start+1 , character); //change example_[ABC]_example to example_A_example , example_B_example etc .. 

    vector<int> kmpresult = kmp(text,temp); //run kmp normally on changed string 

    occurrences.insert(occurrences.end(),kmpresult.begin(),kmpresult.end()); //append all indexed to occurrences 

    }//this loop will run from A1 to An , where text[A1...An]text <--- pattern

    
    return occurrences ; 
}
// kmp for strings at the end $
vector<int> endswithkmp(const string& text, const string& pattern){

    string ptemp = pattern ; 
    string ttemp = text ;
    ttemp.push_back('$');   //appends $ to end of pattern 
    ptemp.push_back('$') ;  // appends $ to end of text(line of the text passed into the function) to make that match uniqe 
    
    return kmp(ttemp , ptemp); //now runs kmp normally 
    

}
// kmp for strings at the start ^
vector<int> startswithkmp(const string& text, const string& pattern){

    string ptemp = pattern ; 
    string ttemp = text ;
    ttemp.insert(ttemp.begin(), '^');       //same as $ but done to front to make front uniqe 
    ptemp.insert(ptemp.begin() , '^'); 
    //cout << temp << endl ; 
    return kmp(ttemp , ptemp); 
    

}
// return the pattern in file format as a string 
string file2pattern(string filename){ //opens the file which has the pattern and returns it as a string 


ifstream file(filename); 
    if (!file.is_open()) {
        
        return "error";
    }

    string line;
    string content; 

    while (getline(file, line)) {
        content += line;
       
    }

    file.close();

    return content ; 


}
// what is the symbol type used in the pattern 
int findsymboltype(string pattern){
    
    int dot=0 , star=0 , range=0 , endswith=0 , startswith=0;
    if (pattern.find('.') != std::string::npos) 
    dot =  1;
    if (pattern.find('[') != std::string::npos) 
    range =  1;
    if (pattern.find('$') != std::string::npos) 
    endswith =  1;
    if (pattern.find('^') != std::string::npos) 
    startswith =  1;

    if(dot+star+range+endswith+startswith == 0)
    return 0 ; //0 for no special pattern matching 
    else if(dot+star+range+endswith+startswith > 1)
    return -1 ; //if more that one special symbol is used return -1
    else if(dot == 1)
    return 1 ; //.
    else if(range == 1)
    return 2 ; //[]
    else if(endswith == 1)
    return 3 ; //$
    else if(startswith == 1)
    return 4 ; //^
    else 
    return 5 ; 

   


}
// main func 
int main() {
    
    string pattern_fromfile = file2pattern(input_Pattern); //turns the pattern from the file into string form 
    int symboltype = findsymboltype(pattern_fromfile);  // what symbol is in the pattern?

    ifstream text_file(input_Text); //opens the text file 
    ofstream out_file(output_file);

    if (!text_file.is_open()) {
        cout << "Unable to open file.\n"; 
        return 1;
    }

    string line;
    int linenumber = 0 ; 
    

    while (getline(text_file, line)) {//reads the file line by line and matches any instances in said line 

        string pattern = pattern_fromfile;

        linenumber++; 

        vector<int> indices; 
        if(symboltype == 0){
            //no regx 
            indices = kmp(line , pattern);  //normal pattern matching no special symbol used 


        }
        else if(symboltype == 1){
            // .  
            indices = dotkmp(line , pattern);

        }
        else if(symboltype == 2){
            // [] 
            indices = rangekmp(line , pattern);
        }
        else if(symboltype == 3){
            // $
            int s_index= pattern.find('$');  
            pattern.erase(s_index,1);   //removes the $ symbol from the pattern
            indices = endswithkmp(line,pattern);
        }
        else if(symboltype == 4){
            // ^
            int s_index= pattern.find('^'); 
            pattern.erase(s_index,1);   //removes the ^ symbol from the pattern 
            indices = startswithkmp(line,pattern); 
        }
        else {
            cout << "error"; 

        }
        
        if (!indices.empty()) {
            out_file << "Pattern found at Line number :" << linenumber <<  " Indices: ";
            cout  << "Pattern found at Line number :" << linenumber <<  " Indices: ";
            for (int index : indices) {
            out_file << index << " ";
            cout << index << " "; //prints the locations where matches were found 
            }
            out_file << endl;
            cout << endl; 
        
        }
        else {
            //cout << "empty" ; 
        }
        
    }

    text_file.close();
    out_file.close(); 
    return 0;
}
