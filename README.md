# C++ Mini Quiz

A console quiz game written in C++17 with scoring, streak bonuses and a saved top-5 high score table.

## Build and run

```bash
g++ -std=c++17 -O2 main.cpp -o quiz
./quiz
```

Run it from the folder containing `questions.txt`.

## Scoring

- 10 points per correct answer
- +5 bonus for each answer in a row (capped at +25)
- Top 5 scores are saved in `scores.txt`

## Adding questions

Edit `questions.txt`, one question per line:

```
question|option A|option B|option C|option D|correct letter
```

Lines starting with `#` are ignored.
