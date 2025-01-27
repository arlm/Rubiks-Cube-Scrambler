#include <iostream>
#include <fstream>
#ifdef _WIN32
	#include <windows.h>
	#include <winuser.h>
#else
	#include <termios.h>
	#include <unistd.h>
#endif
#include <fcntl.h>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <vector>
#include "../libraries/main_header.h"

constexpr char moveSet[] = {'R', 'U', 'F', 'D', 'L', 'B'}; // Move-set
constexpr char moveSpecSet[] = {' ', '2', char(39)};	   // Move-set specialisation
constexpr int highestScrambleLength = 25;				   // Highest scramble length
constexpr int lowestScrambleLength = 20;				   // Lowest scramble length

#ifndef _WIN32
// Function to set terminal to raw mode
inline void configureTerminal(struct termios &original)
{
    struct termios newSettings;
    tcgetattr(STDIN_FILENO, &original);
	newSettings = original;
    newSettings.c_lflag &= ~(ICANON | ECHO);  // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
}

// Function to reset terminal to normal mode
inline void resetTerminal(struct termios &original)
{
	tcsetattr(STDIN_FILENO, TCSANOW, &original);
}

// Function to check if a key is pressed
inline bool is_key_pressed(char key)
{
    int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    int ch = getchar();

    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        if (ch == key)
        {
            return true;
        }
        ungetc(ch, stdin);
    }
    return false;
}
#endif

inline bool spacePressed()
{
	// Return false if spacebar is pressed else true
#ifdef _WIN32
    return (GetAsyncKeyState(VK_SPACE) == 0) ? false : true;
#else
    return !is_key_pressed(' ');
#endif

}

inline bool rPressed()
{
	// Return false if R is pressed else true

#ifdef _WIN32
	return (GetAsyncKeyState(0x52) == 0) ? false : true;
#else
    return !is_key_pressed('r');
#endif	
}

inline bool ePressed()
{
	// Return false if E is pressed else true

#ifdef _WIN32
	return (GetAsyncKeyState(0x45) == 0) ? false : true;
#else
    return !is_key_pressed('e');
#endif	
}

inline int randomNum(int lowerLimit, int upperLimit)
{
	// Generates scramble between the specified limit
	return (lowerLimit + (rand() % (upperLimit - lowerLimit + 1)));
}

char getOppositeMove(char move)
{
	// Returns opposite move
	if (move == 'R')
	{
		// L is opposite of R
		return 'L';
	}
	else if (move == 'U')
	{
		// Same as first condition
		return 'D';
	}
	else if (move == 'F')
	{
		return 'B';
	}
	else if (move == 'D')
	{
		return 'U';
	}
	else if (move == 'L')
	{
		return 'R';
	}
	else if (move == 'B')
	{
		return 'F';
	}

	return '-';
}

std::vector<char> scrambler()
{
	// This function is responsible for generation of scramble which is of good quality
	char previousMove, currentMove;												 // Variables for holding previously done and current move to be done
	int moveToDo, moveSpecToDo;													 // Indexes for moveSet and moveSpecialisationSet
	int scramblePt = 1;															 // Variable for current scramble point
	int scrambleLength = randomNum(lowestScrambleLength, highestScrambleLength); // Before generation of scramble, it'll hold randomly generated scramble length between 20 and 25

	// ! Below is a debug statement, toggle only if you wish to print scramble length before scramble
	// std::cout<<"("<<scrambleLength<<")"<<" ";
	std::vector<char> scramble; // Helper scramble vector for generating scramble

	// Set up initial move and move specialisation
	moveToDo = randomNum(0, 5);		// 0 and 5 are inputs because index of moveSet can vary between 0 and 5.
	moveSpecToDo = randomNum(0, 2); // 0 and 2 are inputs because index of moveSpecSet can vary between 0 and 2.

	// Push move to scramble vector
	currentMove = moveSet[moveToDo];
	scramble.push_back(currentMove);

	if (moveSpecToDo != 0)
	{
		// If moveSpecToDo==1 append '2' or if moveSpecToDo==2 append inverted comma
		scramble.push_back(moveSpecSet[moveSpecToDo]);
		scramble.push_back(' ');
	}
	else if (moveSpecToDo == 0)
	{
		// If standard move is chosen, push blank to scramble
		scramble.push_back(' ');
	}

	// Main scramble generation loop
	while (scramblePt < scrambleLength)
	{
		// Set up initial move and move specialisation
		moveToDo = randomNum(0, 5);		// 0 and 5 are inputs because index of moveSet can vary between 0 and 5.
		moveSpecToDo = randomNum(0, 2); // 0 and 2 are inputs because index of moveSpecSet can vary between 0 and 2.

		previousMove = currentMove;		 // Make previousMove = currentMove
		currentMove = moveSet[moveToDo]; // Assign currentMove as new generated move

		while (previousMove == currentMove || previousMove == getOppositeMove(currentMove))
		{
			// Keep generating currentMove until previousMove and currentMove are not equal or previousMove and currentMove's opposite move are not equal
			moveToDo = randomNum(0, 5);
			currentMove = moveSet[moveToDo];
		}
		// After generating currentMove, push it to scramble vector
		scramble.push_back(currentMove);

		if (moveSpecToDo != 0)
		{
			// If moveSpecToDo==1 append '2' or if moveSpecToDo==2 append inverted comma
			scramble.push_back(moveSpecSet[moveSpecToDo]);
			scramble.push_back(' ');
		}
		else if (moveSpecToDo == 0)
		{
			// If standard move is chosen, push blank-space to scramble
			scramble.push_back(' ');
		}
		// After completing each move, increment scramblePt
		scramblePt++;
	}
	// Return scramble vector
	return scramble;
}

double phaseOne()
{
	// This function is solely responsible for calculating time passed between solve
	clock_t initialTime = 0; // This variable holds initial time
	clock_t finalTime = 0;	 // This variable holds final time

	initialTime = clock(); // Assigning initial time
	hideCursor();

	while (true)
	{
		if (spacePressed() == false) 
		{
			if (finalTime == 0)
			{
				// As soon as space is pressed enter this if condition
				std::cout << "Start!" << std::endl;

				// As soon as space is pressed
				finalTime = clock(); // Record time after solve
			} 
			else 
			{
				clearLine();
				std::cout << "Solve!" << std::endl;
				showCursor();
				return double(double(finalTime - initialTime) / double(CLOCKS_PER_SEC));
			}
		}

		if (ePressed() == false) 
		{
			showCursor();
			break;
		}

		if (rPressed() == false) 
		{
			showCursor();
			break;
		}

		if (finalTime != 0) {
			clearLine();
			std::cout << double(double(clock() - initialTime) / double(CLOCKS_PER_SEC)) << "s";
		}
	}

	showCursor();
	return 0.0;
}

void scrambleAndPrint(std::vector<char> &scramble)
{
    clearScreen();

    scramble = scrambler();
    for (int i = 0; i < scramble.size(); i++)
    {
        // Prints scramble
        std::cout << scramble.at(i);
    }


    std::cout << "\n\n";
}

void clearScreen()
{
	std::cout << "\e[1;1H\e[2J";
}

void clearLine()
{
	std::cout << "\e[0G\e[0J";
}

void showCursor()
{
	std::cout << "\e[?25h";
}

void hideCursor()
{
	std::cout << "\e[?25l";
}

void runTimer(char *scrambleArray)
{
#ifndef _WIN32
	struct termios original;
    configureTerminal(original);
#endif

	srand(time(0));
	std::vector<char> scramble;
	
	scrambleAndPrint(scramble);

	double timeTaken;

	while (true)
	{
		if (rPressed() == false)
		{
			// After releasing r, clear screen and return scramble again
			scrambleAndPrint(scramble);
		}

		if (ePressed() == false)
		{

#ifndef _WIN32
			resetTerminal(original);
#endif
		// If e is pressed, the program will stop execution
			exit(0);
		}

		if (spacePressed() == false)
		{
			// As soon as space is pressed enter this if condition
			std::cout << "Ready!" << std::endl;

			// The below code pushes scramble and solve to external file for more calculations
			timeTaken = phaseOne();

			// After leaving space bar, print time passed.
			std::cout << "Time passed is " << timeTaken << "s" << std::endl;

			std::ofstream file;
			file.open("./times.txt", std::ios::app);
			for (int i = 0; i < scramble.size(); i++)
			{
				file << scramble.at(i);
			}
			file << ":: " << timeTaken << "s"
				<< "\n";
			file.close();

            scrambleAndPrint(scramble);
        }
	}

#ifndef _WIN32
	resetTerminal(original);
#endif
}
