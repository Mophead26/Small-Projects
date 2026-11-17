#include <iostream>
#include <string>
#include <random>

int getRandomNumber() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distrb(1, 100);
    return distrb(gen);
}

int playGame() {
    int randomNumber = getRandomNumber();
    int guess;
    std::cout << "Guess the number between 1 and 100 /n" << std::endl;
    while (guess != randomNumber) {
        std::cout << "What's your guess? ";
        std::cin >> guess;
        if (guess > randomNumber){
            std::cout << "Too High" << std::endl;
        }
        else if (guess < randomNumber){
            std::cout << "Too Low" << std::endl;
        }
    }
    std::cout << "You guessed the number correctly!" << std::endl;
    std::cout << "Would you like to play again? y/n: ";
    std::string response;
    std::cin >> response;
    if (response == "y") {
        playGame();
    }
    else {
        std::cout << "Ok goodbye";
    }
    return 0;
}

int main() {
    std::cout << "High Low Game" << std::endl;
    std::cout << "Would you like to play? y/n: ";
    std::string response;
    std::cin >> response;
    if (response != "y") {
        std::cout << "Ok goodbye" << std::endl;
        return 0;
    }
    playGame();
}