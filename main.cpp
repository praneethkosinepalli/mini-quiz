#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cctype>

using namespace std;

string ask(string prompt_text) {
    string user_input_line;
    cout << prompt_text;
    
    if (!getline(cin, user_input_line)) {   
        cout << "\n";
        exit(0);
    }
    return user_input_line;
}

string toUpper(string s) {
    string result = "";
    for (size_t i = 0; i < s.length(); i++) { 
        result += toupper(s[i]);
    }
    return result;
}

struct ParsedData {
    string parts[10];
    int size;
};

ParsedData split(const string& line, char separator) {
    ParsedData pd;
    pd.size = 0;
    string current_piece = "";
    
    for (size_t i = 0; i < line.size(); i++) {
        if (line[i] == separator) {
            if (pd.size < 10) {
                pd.parts[pd.size++] = current_piece;
            }
            current_piece = "";
        } else {
            if (line[i] != '\r') {   
                current_piece += line[i];
            }
        }
    }
    if (pd.size < 10) {
        pd.parts[pd.size++] = current_piece;
    }
    return pd;
}

int askNumber(string prompt, int low, int high) {
    int n = -999; 
    while (true) {
        string temp = ask(prompt);
        n = atoi(temp.c_str());
        
        if (n >= low && n <= high) {
            return n;
        }
        cout << "Please enter a number from " << low << " to " << high << ".\n";
    }
}

class Question {
protected:
    string text;
    string category;

public:
    Question(string t, string c) {
        text = t;
        category = c;
    }
    
    virtual ~Question() {}

    string getCategory() const { 
        return category; 
    }

    virtual void show() const = 0;               
    virtual string validKeys() const = 0;        
    virtual bool isCorrect(char key) const = 0;    
    virtual string correctAnswer() const = 0;      
};

class MCQQuestion : public Question {
private:
    string options[4];
    int answer_index;   

public:
    MCQQuestion(string t, string c, string a, string b, string cc, string d, int ans)
        : Question(t, c) {
        options[0] = a;
        options[1] = b;
        options[2] = cc;
        options[3] = d;
        answer_index = ans;
    }

    void show() const {
        cout << text << "\n";

        for (int i = 0; i < 4; i++) {
            char letter = 'A' + i;
            cout << "   " << letter << ") " << options[i] << "\n";
        }
    }
    
    string validKeys() const { 
        return "ABCD"; 
    }
    
    bool isCorrect(char key) const { 
        return key == ('A' + answer_index); 
    }
    
    string correctAnswer() const {
        char ans_letter = 'A' + answer_index;
        return string(1, ans_letter) + ") " + options[answer_index];
    }
};

class TrueFalseQuestion : public Question {
private:
    bool tf_answer;

public:
    TrueFalseQuestion(string t, string c, bool ans) : Question(t, c) {
        tf_answer = ans;
    }

    void show() const {
        cout << "True or False: " << text << "\n";
        cout << "   T) True\n   F) False\n";
    }
    
    string validKeys() const { 
        return "TF"; 
    }
    
    bool isCorrect(char key) const { 
        if (key == 'T') {
            return tf_answer == true;
        } else {
            return tf_answer == false;
        }
    }
    
    string correctAnswer() const { 
        if (tf_answer) {
            return "True";
        } else {
            return "False";
        }
    }
};

struct Entry {
    string name;
    int score;
};

class ScoreBoard {
private:
    Entry entries_list[5];
    int entry_count;
    int MAX_ENTRIES = 5; 

public:
    ScoreBoard() { 
        entry_count = 0;
        load(); 
    }

    void load() {
        entry_count = 0;
        ifstream infile("scores.txt");
        Entry temp_entry;
        
        while (entry_count < MAX_ENTRIES && infile >> temp_entry.score) {
            getline(infile, temp_entry.name);
            if (!temp_entry.name.empty() && temp_entry.name[0] == ' ') {
                temp_entry.name = temp_entry.name.substr(1);
            }
            entries_list[entry_count++] = temp_entry;
        }
        infile.close(); 
    }

    void save() const {
        ofstream outfile("scores.txt");
        for (int i = 0; i < entry_count; i++) {
            outfile << entries_list[i].score << " " << entries_list[i].name << "\n";
        }
        outfile.close();
    }

    void add(string playerName, int finalScore) {
        if (entry_count < MAX_ENTRIES) {
            entries_list[entry_count].name = playerName;
            entries_list[entry_count].score = finalScore;
            entry_count++;
        } else if (finalScore > entries_list[MAX_ENTRIES - 1].score) {
            entries_list[MAX_ENTRIES - 1].name = playerName;
            entries_list[MAX_ENTRIES - 1].score = finalScore;
        }

        for (int i = 0; i < entry_count; i++) {
            for (int j = 0; j + 1 < entry_count - i; j++) {
                if (entries_list[j].score < entries_list[j + 1].score) {
                    Entry swap_temp = entries_list[j];
                    entries_list[j] = entries_list[j + 1];
                    entries_list[j + 1] = swap_temp;
                }
            }
        }

        if (entry_count > MAX_ENTRIES) {
            entry_count = MAX_ENTRIES;
        }
        save();
    }

    void show() const {
        cout << "\n===== HIGH SCORES =====\n";
        if (entry_count == 0) {
            cout << "No scores yet. Be the first!\n";
        }
        for (int i = 0; i < entry_count; i++) {
            cout << i + 1 << ". " << entries_list[i].name << " - "
                 << entries_list[i].score << " pts\n";
        }
    }
};

class Quiz {
private:
    Question* allQuestions[500];
    int question_count;
    ScoreBoard board;

    string chooseCategory() {
        string category_list[100];
        int cat_count = 0;
        
        for (int i = 0; i < question_count; i++) {
            string current_cat = allQuestions[i]->getCategory();
            bool already_exists = false;
            
            for (int j = 0; j < cat_count; j++) {
                if (category_list[j] == current_cat) {
                    already_exists = true;
                }
            }
            if (!already_exists && cat_count < 100) {
                category_list[cat_count++] = current_cat;
            }
        }

        cout << "\nChoose a category:\n";
        for (int i = 0; i < cat_count; i++) {
            cout << "  " << i + 1 << ") " << category_list[i] << "\n";
        }
        cout << "  " << cat_count + 1 << ") All categories\n";

        int user_selection = askNumber("Your choice: ", 1, cat_count + 1);
        
        if (user_selection == cat_count + 1) {
            return "";
        }
        return category_list[user_selection - 1];
    }

    void shuffle(Question* q_vec[], int size) {
        for (int i = size - 1; i > 0; i--) {
            int random_index = rand() % (i + 1);
            Question* temp_ptr = q_vec[i];
            q_vec[i] = q_vec[random_index];
            q_vec[random_index] = temp_ptr;
        }
    }

public:
    Quiz() {
        question_count = 0;
    }

    ~Quiz() {
        for (int i = 0; i < question_count; i++) {
            delete allQuestions[i];
        }
    }

    int load(string filename) {
        ifstream file_stream(filename.c_str());
        string current_line;
        
        while (getline(file_stream, current_line) && question_count < 500) {
            if (current_line.empty() || current_line[0] == '#') {
                continue; 
            }
            
            ParsedData parsed_data = split(current_line, '|');

            if (parsed_data.parts[0] == "MCQ" && parsed_data.size == 8 && !parsed_data.parts[7].empty()) {
                int correct_ans_num = toupper(parsed_data.parts[7][0]) - 'A';
                if (correct_ans_num >= 0 && correct_ans_num <= 3) {
                    allQuestions[question_count++] =
                        new MCQQuestion(parsed_data.parts[2], parsed_data.parts[1], parsed_data.parts[3], parsed_data.parts[4], parsed_data.parts[5], parsed_data.parts[6], correct_ans_num);
                }
            }
            else if (parsed_data.parts[0] == "TF" && parsed_data.size == 4 && !parsed_data.parts[3].empty()) {
                bool tf_val = (toupper(parsed_data.parts[3][0]) == 'T');
                allQuestions[question_count++] =
                    new TrueFalseQuestion(parsed_data.parts[2], parsed_data.parts[1], tf_val);
            }
        }
        file_stream.close();
        return question_count;
    }

    void showScores() { 
        board.show(); 
    }

    void play() {
        string selected_cat = chooseCategory();
        
        Question* game_pool[500];
        int pool_count = 0;
        for (int i = 0; i < question_count; i++) {
            if (selected_cat == "" || allQuestions[i]->getCategory() == selected_cat) {
                game_pool[pool_count++] = allQuestions[i];
            }
        }

        if (pool_count == 0) {
            cout << "No questions found in this category!\n";
            return;
        }

        cout << "\n" << pool_count << " questions available.\n";
        int num_questions_to_play = askNumber("How many do you want to play? ", 1, pool_count);
        
        shuffle(game_pool, pool_count);

        int current_score = 0;
        int current_streak = 0;
        int correct_count = 0;
        int skips_remaining = 1;

        for (int i = 0; i < num_questions_to_play; i++) {
            cout << "\n--- Question " << i + 1 << " of " << num_questions_to_play
                 << "   [Score: " << current_score << "] ---\n";
            
            game_pool[i]->show();

            string valid_keys_str = game_pool[i]->validKeys();
            char user_key = ' ';
            bool question_skipped = false;
            
            while (true) {
                string input_prompt = "Your answer";
                if (skips_remaining > 0) {
                    input_prompt += " (S = skip, " + string(1, '0' + skips_remaining) + " left)";
                }
                
                string raw_input = toUpper(ask(input_prompt + ": "));

                if (raw_input.empty()) {
                    continue;
                }
                
                user_key = raw_input[0];
                
                if (user_key == 'S' && valid_keys_str.find('S') == string::npos) {
                    if (skips_remaining > 0) {
                        skips_remaining--;
                        question_skipped = true;
                        break;
                    } else {
                        cout << "No skips left!\n";
                    }
                } else if (valid_keys_str.find(user_key) != string::npos) {
                    break;
                } else {
                    cout << "Please type one of: " << valid_keys_str << "\n";
                }
            }

            if (question_skipped) {
                cout << "Skipped. No points lost.\n";
            } else if (game_pool[i]->isCorrect(user_key)) {
                current_streak++;
                correct_count++;
                
                int streak_bonus = (current_streak - 1) * 5;
                if (streak_bonus > 25) {
                    streak_bonus = 25;
                }
                
                int points_earned = 10 + streak_bonus;
                current_score += points_earned;
                
                cout << "Correct! +" << points_earned << " points";
                if (streak_bonus > 0) {
                    cout << " (streak bonus +" << streak_bonus << ")";
                }
                cout << "\n";
            } else {
                current_streak = 0; 
                cout << "Wrong! The answer was " << game_pool[i]->correctAnswer() << "\n";
            }
        }

        int percentage = correct_count * 100 / num_questions_to_play;
        cout << "\n===== GAME OVER =====\n";
        cout << "Correct answers: " << correct_count << " / " << num_questions_to_play
             << " (" << percentage << "%)\n";
        cout << "Final score    : " << current_score << " points\n";
        
        if (percentage == 100) {
            cout << "Perfect! You are a quiz master!\n";
        } else if (percentage >= 70) {
            cout << "Great job!\n";
        } else if (percentage >= 40) {
            cout << "Not bad, try again to improve!\n";
        } else {
            cout << "Keep practising, you will get better!\n";
        }

        string user_name = ask("\nEnter your name: ");
        if (user_name.empty()) {
            user_name = "Anonymous";
        }
        
        board.add(user_name, current_score);
        board.show();
    }
};

void showRules() {
    cout << "\n===== HOW TO PLAY =====\n";
    cout << "- Each correct answer gives 10 points.\n";
    cout << "- Answer correctly in a row for a streak bonus (+5 each, max +25).\n";
    cout << "- A wrong answer resets your streak.\n";
    cout << "- You can skip ONE question per game without losing anything.\n";
}

int main() {
    srand(time(NULL));   

    Quiz my_quiz_game;
    
    int loaded_count = my_quiz_game.load("questions.txt");
    if (loaded_count == 0) {
        cout << "Could not load questions.txt.\n";
        cout << "Run the program from the folder that contains it.\n";
        return 1;
    }

    cout << "==========================\n";
    cout << "      C++ MINI QUIZ\n";
    cout << "==========================\n";

    bool keep_running = true;
    while (keep_running) {
        cout << "\n1) Play\n2) High scores\n3) How to play\n4) Quit\n";
        int main_menu_choice = askNumber("Choose: ", 1, 4);

        if (main_menu_choice == 1) {
            my_quiz_game.play();
        } else if (main_menu_choice == 2) {
            my_quiz_game.showScores();
        } else if (main_menu_choice == 3) {
            showRules();
        } else {
            keep_running = false; 
        }
    }

    cout << "Thanks for playing!\n";
    return 0;
}