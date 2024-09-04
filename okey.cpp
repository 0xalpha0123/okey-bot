/*
    rules of the game:

    0 - 299 pts : Bronze
    300 - 399 pts : Silver
    400 + pts : Gold

    It is a one player and 1 deck card game.
    The player has 5 cards in his/her hand.
    The deck is composed of yellow , red and blue suits of cards numbered from 1 to 8.

    Each turn the player can either discard a card and draw a new one from the deck or score a combinaison of 3 cards

    The combinaisons are:

    - 3 cards of the same value : (value+1) * 10 pts
    - suite of 3 cards : min(values) *10 pts + 40*samesuit 

    When a combinaison is done the cards are destroyed and the player can draw new cards.

    The goal is to maximize the score , decks are shuffles at the beginning of the game. The player always has 5cards in his/her hand.

    The player doesnt know the deck.

    The game stops when the deck is empty and no more combinaisons can be made.
*/

#include "okey.h"

// use opengl to display the game state waitng for the player to show the current state of his hand
#include <GLFW/glfw3.h>

int main()
{
    // create a game window that displays 24 cards in 3 rowas of 8 cards, numbered from 1 to 8 and 3 different colors

    // create a game state from player input (clicking on the cards)
    // they will appear in his hand ( a row of 5 cards athe bottom of the screen)

    GameState state;

    // game window:

    // create a window
    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit())
        return -1;

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(800, 600, "Okey Game", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // Render here
        glClear(GL_COLOR_BUFFER_BIT);

        // draw the deck
        // draw the player hand
        // draw the score

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }


    return 0;
}