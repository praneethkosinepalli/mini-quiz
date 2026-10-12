# C++ Mini Quiz

A simple console quiz game written in C++ for CSE 202 (Object Oriented Programming with C++).

## Features
- Multiple choice and True/False questions
- Choose a category and how many questions to play
- Questions come in a random order every game
- Scoring with streak bonus, plus one free "skip" per game
- Top 5 high scores saved in `scores.txt`
- Add your own questions by editing `questions.txt` (no recompiling needed)

## OOP concepts used
- **Classes and objects**: `Quiz`, `ScoreBoard`
- **Inheritance**: `MCQQuestion` and `TrueFalseQuestion` inherit from `Question`
- **Polymorphism**: `Question` has virtual functions (`show`, `isCorrect`, ...) that each child class defines in its own way
- **Encapsulation**: data members are private/protected and used through member functions
- **Constructors / destructors**: the `Quiz` destructor frees all questions

## How to run

You need a C++ compiler (g++). Keep `main.cpp` and `questions.txt` in the same folder.

```
g++ main.cpp -o quiz
./quiz          (Windows: quiz.exe)
```

## Scoring
- 10 points per correct answer
- +5 bonus for each correct answer in a row (maximum +25)
- A wrong answer resets the streak; a skipped question changes nothing

## Question file format

```
MCQ|category|question|A|B|C|D|correct letter
TF|category|statement|T or F
```

Lines starting with `#` are ignored. A new category name shows up in the menu automatically.
