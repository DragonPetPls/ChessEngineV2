//
// Created by fabian on 10/8/24.
//

#include <queue>
#include <string>
#include <iostream>
#include <thread>
#include <sstream>
#include "Communication.h"

void Communication::startCommunication() {
    g.loadStartingPosition();
    std::thread(&Communication::worker, this).detach();

    //Adds all inputs at the end of the command queue
    std::string lastInput;
    while(lastInput != "quit"){
        getline(std::cin, lastInput);
        queueMtx.lock();
        commandQueue.push(lastInput);
        queueMtx.unlock();
        cv.notify_one();
    }
}



void Communication::uci() {
    output.lock();
    std::cout << "id name notCheckers" << std::endl;
    std::cout << "id author Fabian" << std::endl;
    std::cout << "v2" << std::endl;
    std::cout <<  "uciok" << std::endl;
    output.unlock();
}

void Communication::isready() {
    output.lock();
    std::cout << "readyok" << std::endl;
    output.unlock();
}



void Communication::go(int time) {

    gameMtx.lock();

    move m = e.getMove(g, time - 30);
    gameMtx.unlock();
    output.lock();

    std::cout << "bestmove " << numberToLetter(Game::getXCoord(m.startingSquare)) << (Game::getYCoord(m.startingSquare) + 1)
    << numberToLetter(Game::getXCoord(m.finalSquare)) << (Game::getYCoord(m.finalSquare) + 1);
    switch (m.promotion){
        case QUEEN: std::cout << "q" << std::endl; break;
        case ROOK: std::cout << "r" << std::endl; break;
        case BISHOP: std::cout << "b" << std::endl; break;
        case KNIGHT: std::cout << "n" << std::endl; break;
        default: std::cout << std::endl;
    }
    output.unlock();
}

void Communication::stop() {
    e.stopSearch();
}

char Communication::numberToLetter(short x) {

    switch(x){
        case 0: return 'a';
        case 1: return 'b';
        case 2: return 'c';
        case 3: return 'd';
        case 4: return 'e';
        case 5: return 'f';
        case 6: return 'g';
        case 7: return 'h';
        default: return ' ';
    }

    return 0;
}

void Communication::position(std::string command) {

    std::istringstream stream(command);
    std::string argument;
    std::vector<std::string> arguments;

    int moveIndex = 2;

    while (stream >> argument) {
        arguments.push_back(argument);
    }

    if(arguments.size() < 2){
        return;
    }

    if(arguments[1] == "startpos"){
        g.loadStartingPosition();

    } else if (arguments[1] == "fen"){
        std::string fen;
        for (int i = 2; i < arguments.size(); i++){
            if(arguments[i] == "moves"){
                moveIndex = i;
                break;
            }
            fen += arguments[i] + " ";
        }
        g.loadFen(fen);
        g.printGame();
    }

    if (arguments.size() < moveIndex + 1){
        return;
    }

    if(arguments[moveIndex] == "moves"){
        for(int i = moveIndex + 1; i < arguments.size(); i++){
            move m = g.stringToMove(arguments[i]);
           g.doMove(m);
#ifdef DEBUG
            output.lock();
            std::cout << m.getFromHorizontal() << m.getFromVertical() << m.getToHorizontal() << m.getToVertical() << m.getPromoteTo() << std::endl;
            g.printGame();
            output.unlock();
#endif
        }
    }

}

short Communication::letterToNumber(char c) {

    switch(c){
        case 'a': return 0;
        case 'b': return 1;
        case 'c': return 2;
        case 'd': return 3;
        case 'e': return 4;
        case 'f': return 5;
        case 'g': return 6;
        case 'h': return 7;
        default: return 9;
    }


    return 0;
}

void Communication::worker() {
    std::string command;
    std::string subcommand;

    while(command != "quit"){
        queueMtx.lock();
        if(commandQueue.empty()){
            queueMtx.unlock();
            std::unique_lock<std::mutex> lg(toDo);
            cv.wait(lg, [this] {return (!commandQueue.empty());});
            continue;
        }

        command = commandQueue.front();
        commandQueue.pop();
        queueMtx.unlock();

        subcommand = command.substr(0, command.find(" "));

        if(command == "uci"){
            uci();
        } else if (command == "isready"){
            isready();
        } else if (subcommand == "position"){
            position(command);
        } else if (subcommand == "go"){

            int time = 5000;

            std::istringstream stream(command);
            std::string argument;
            std::vector<std::string> arguments;

            while (stream >> argument) {
                arguments.push_back(argument);
            }

            for(int i = 0; i < arguments.size() - 1; i++){
                if(arguments[i] == "movetime"){
                    std::stringstream ss(arguments[i + 1]);
                    ss >> time;
                }
            }
            std::cout << time << std::endl;
            std::thread(&Communication::go, this, time).detach();
        } else if (subcommand == "eval"){
            std::cout << "Eval: " << e.evalPosition(g) << std::endl;
        }

    }
}
