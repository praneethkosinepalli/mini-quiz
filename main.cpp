// Mini Quiz Game with Scores - C++17
// Build: g++ -std=c++17 -O2 main.cpp -o quiz
#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

struct Question {
    std::string text;
    std::vector<std::string> options;  // always 4
    int answer;                        // 0..3
};

struct ScoreEntry {
    std::string name;
    int score;
};

const std::string QUESTIONS_FILE = "questions.txt";
const std::string SCORES_FILE = "scores.txt";
const int MAX_SCORES = 5;

// ---------- Loading ----------
std::vector<Question> loadQuestions(const std::string& path) {
    std::vector<Question> list;
    std::ifstream in(path);
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::vector<std::string> parts;
        std::stringstream ss(line);
        std::string part;
        while (std::getline(ss, part, '|')) parts.push_back(part);
        if (parts.size() != 6 || parts[5].empty()) continue;  // skip bad lines
        int ans = std::toupper(parts[5][0]) - 'A';
        if (ans < 0 || ans > 3) continue;
        list.push_back({parts[0], {parts[1], parts[2], parts[3], parts[4]}, ans});
    }
    return list;
}

std::vector<ScoreEntry> loadScores() {
    std::vector<ScoreEntry> scores;
    std::ifstream in(SCORES_FILE);
    std::string name;
    int score;
    while (in >> score && in.get() && std::getline(in, name))
        scores.push_back({name, score});
    return scores;
}

void saveScores(const std::vector<ScoreEntry>& scores) {
    std::ofstream out(SCORES_FILE);
    for (const auto& s : scores) out << s.score << ' ' << s.name << '\n';
}

// ---------- Display ----------
void showScores() {
    auto scores = loadScores();
    std::cout << "\n=== High Scores ===\n";
    if (scores.empty()) std::cout << "No scores yet. Be the first!\n";
    for (size_t i = 0; i < scores.size(); ++i)
        std::cout << i + 1 << ". " << scores[i].name << " - " << scores[i].score << " pts\n";
}

int readChoice(const std::string& prompt, const std::string& valid) {
    std::string input;
    while (true) {
        std::cout << prompt;
        if (!std::getline(std::cin, input)) return -1;  // EOF
        if (!input.empty()) {
            char c = std::toupper(input[0]);
            size_t pos = valid.find(c);
            if (pos != std::string::npos) return static_cast<int>(pos);
        }
        std::cout << "Please enter one of: " << valid << "\n";
    }
}

// ---------- Game ----------
void playGame(std::vector<Question> questions) {
    std::mt19937 rng(std::random_device{}());
    std::shuffle(questions.begin(), questions.end(), rng);

    int score = 0, streak = 0, correct = 0;
    const int total = static_cast<int>(questions.size());

    for (int i = 0; i < total; ++i) {
        const Question& q = questions[i];
        std::cout << "\nQuestion " << i + 1 << "/" << total << "  (Score: " << score << ")\n";
        std::cout << q.text << "\n";
        for (int j = 0; j < 4; ++j)
            std::cout << "  " << char('A' + j) << ") " << q.options[j] << "\n";

        int pick = readChoice("Your answer: ", "ABCD");
        if (pick < 0) return;  // input closed

        if (pick == q.answer) {
            ++streak; ++correct;
            int bonus = std::min(streak - 1, 5) * 5;
            score += 10 + bonus;
            std::cout << "Correct! +" << 10 + bonus;
            if (bonus) std::cout << " (streak bonus +" << bonus << ")";
            std::cout << "\n";
        } else {
            streak = 0;
            std::cout << "Wrong. The answer was " << char('A' + q.answer) << ") "
                      << q.options[q.answer] << "\n";
        }
    }

    std::cout << "\n=== Game Over ===\n";
    std::cout << "You got " << correct << " of " << total << " right.\n";
    std::cout << "Final score: " << score << " pts\n";

    std::cout << "Enter your name: ";
    std::string name;
    std::getline(std::cin, name);
    if (name.empty()) name = "Anonymous";

    auto scores = loadScores();
    scores.push_back({name, score});
    std::stable_sort(scores.begin(), scores.end(),
                     [](const ScoreEntry& a, const ScoreEntry& b) { return a.score > b.score; });
    if (scores.size() > MAX_SCORES) scores.resize(MAX_SCORES);
    saveScores(scores);
    showScores();
}

int main() {
    auto questions = loadQuestions(QUESTIONS_FILE);
    if (questions.empty()) {
        std::cerr << "Could not load any questions from " << QUESTIONS_FILE
                  << ". Run the program from the folder that contains it.\n";
        return 1;
    }

    std::cout << "====================\n   C++ MINI QUIZ\n====================\n";
    while (true) {
        std::cout << "\n1) Play\n2) High scores\n3) Quit\n";
        int c = readChoice("Choose: ", "123");
        if (c == 0) playGame(questions);
        else if (c == 1) showScores();
        else break;
    }
    std::cout << "Thanks for playing!\n";
    return 0;
}
