
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>

using namespace std;
using namespace std;

// Bot class

enum ESuit
{
    yellow,
    red,
    blue
};

class Deck {
public:
    vector<uint32_t> cards;

    Deck() {
        for (int i = 1; i <= 8; i++) {
            cards.push_back(1 << i); // yellow
            cards.push_back(1 << i + 8); // red 
            cards.push_back(1 << i + 16); // blue
        }
    }

    void shuffle() {
        random_shuffle(cards.begin(), cards.end());
    }

    uint32_t draw() {
        uint32_t card = cards.back();
        cards.pop_back();
        return card;
    }
    
};

class Hand{
public:
    uint32_t mask = 0;
    vector<uint32_t> cards;

    void addCard(uint32_t card) {
        mask |= card;
        cards.push_back(card);
    }

    void removeCard(uint32_t card) {
        mask &= ~card;
        cards.erase(remove(cards.begin(), cards.end(), card), cards.end());
    }
};

class GameState {
public:

    Deck deck;
    Hand hand;

    int score = 0;

    GameState() {
        hand = Hand();
        deck.shuffle();
        for (int i= 0; i < 5; i++) {
            hand.addCard(deck.draw());
        }
    }

    void removeCard(uint32_t card){
        
        mask &= ~card;
        hand.removeCard(card);
    }

    void updateScore(int points) {
        score += points;
    }

    // use masks to keep all cards status in one byte , there are 3 suits and 8 values
    // 1 means the card is in the deck or hand, 0 means it is not
    // at the beginning all cards are in the deck
    // 24 bits at 1
    uint32_t mask = 1 << 24 - 1;

    void makeMove(Move move) {
        move.apply(*this);
        deck.shuffle();
    }

};

class Move{
public:
    vector<uint32_t> cards;
    int points = 0;

    Move(vector<uint32_t> _cards, int _pts) : cards(_cards) , points(_pts){}
    Move() {}

    void apply(GameState& state) {
        if(cards.size() == 1){
            state.removeCard(cards[0]);
        }
        else{
            // 3 cards
            // check for points
            state.updateScore(points);

            for(auto card: cards) {
                state.removeCard(card);
            }
        }
    }
};

void dfs_fillCombinaison(uint32_t mask, GameState& state, vector<uint32_t>& combinaison, int idx, vector<Move>& moves){
    if(mask == 0){
        // check if all card have same color in combinaison
        // color is proection of 24 bits into 3
        int color0 = log2(combinaison[0]) / 8;
        int color1 = log2(combinaison[1]) / 8;
        int color2 = log2(combinaison[2]) / 8;

        int value0 = (int) log2(combinaison[0]) % 8;
        int value1 = (int) log2(combinaison[1]) % 8;
        int value2 = (int) log2(combinaison[2]) % 8;

        int points = min({value0+1, value1+1, value2+1})*10 + 40*(color0 == color1 && color1 == color2);

        moves.push_back(Move(combinaison, points));
        return;
    }
    if(idx >= state.hand.cards.size()){
        return;
    }

    for(int i = idx ; i < state.hand.cards.size(); i++){
        if(mask & state.hand.cards[i]){
            combinaison.push_back(state.hand.cards[i]);
            dfs_fillCombinaison(mask & ~(1 << i), state, combinaison,i+1, moves);
            combinaison.pop_back();
        }
    }
}

vector<Move> getPossibleMoves(GameState& state) {
    vector<Move> moves;

    // score a combinaison of 3 cards
    // check if good combinaison exists

    for (int i = 1; i <= 8; i++) {
        uint32_t mask = 1 << i;
        if ((state.hand.mask & mask) && (state.hand.mask & mask << 8) && (state.hand.mask & mask << 16)) {
            moves.push_back(Move({mask, mask << 8, mask << 16}, (i + 1) * 10));
        }
    }

    // suite de carte
    for(int i = 0; i < 6; i++){
        uint32_t mask = (1 << i) | (1 << i + 1) | (1 << i + 2);
        uint32_t combinaison = state.hand.mask | state.hand.mask >> 8 | state.hand.mask >> 16;

        if((combinaison & mask) == mask){
            vector<uint32_t> combinaison;
            dfs_fillCombinaison(mask, state, combinaison, 0, moves);
        }
    }

    for (uint32_t card : state.hand.cards) {
        moves.push_back(Move({card}, 0));
    }

    return moves;
}

class Node {
public:
    GameState state;
    Move move;
    Node* parent;
    vector<Node*> children;
    int totalReward;
    int numVisits;

    Node(GameState state, Move move, Node* parent)
        : state(state), move(move), parent(parent), totalReward(0), numVisits(0) {}

    Node* UCTSelectChild() {
        Node* bestChild = nullptr;
        double bestValue = -1;

        for (Node* child : children) {
            double uctValue = child->totalReward / (double)child->numVisits
                + sqrt(2 * log(numVisits) / (double)child->numVisits);
            if (uctValue > bestValue) {
                bestChild = child;
                bestValue = uctValue;
            }
        }

        return bestChild;
    }
};

class OkeyBot {
public:
    // ..
    int simulateRandomPlayout(GameState state) {
        while (!state.deck.cards.empty()) {
            vector<Move> moves = getPossibleMoves(state);
            if (moves.empty()) {
                break;
            }
            Move move = moves[rand() % moves.size()];
            state.makeMove(move);
        }
        return state.score;
    }

    Move UCT(GameState state , int NUM_SIMULATIONS) {
        Node* root = new Node(state, Move(), nullptr);

        for (int i = 0; i < NUM_SIMULATIONS; i++) {
            Node* node = root;
            GameState tempState = state;

            // Selection
            while (!node->children.empty()) {
                node = node->UCTSelectChild();
                tempState.makeMove(node->move);
            }

            // Expansio
            vector<Move> moves = getPossibleMoves(tempState);
            if (!moves.empty()) {
                for (Move move : moves) {
                    node->children.push_back(new Node(tempState, move, node));
                }
                node = node->children[rand() % node->children.size()];
                tempState.makeMove(node->move);
            }

            // Simulation
            int reward = simulateRandomPlayout(tempState);

            // Backpropagation
            while (node != nullptr) {
                node->numVisits++;
                node->totalReward += reward;
                node = node->parent;
            }
        }

        Node* bestChild = root->UCTSelectChild();
        return bestChild->move;
    }

    // ...
};