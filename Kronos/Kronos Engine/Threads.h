#pragma once
#include <thread>
#include "utility.h"
#include <condition_variable>

#include "Move_Generation.h"
#include "consts.h"
#include "Evaluation.h"

namespace KRONOS {

    namespace SEARCH {

        struct Search_Data {
            // gamePly will be used to see what ply we are at in the game
            // threadPly will be used to access positions in threadPositions
            int gamePly;
            int threadPly;
            std::vector<Position> threadPositions;
            const std::vector<Position>* previousPositions;
            Move bestMoveThisIteration;
            EVALUATION::Evaluation eval;
            int numNodes;
            Move lastMove;

            std::vector<Move> killerMoves1;
            std::vector<Move> killerMoves2;

            int16_t history[2][6][64];

            std::vector<int16_t> evalHistory;

            Search_Data() : gamePly(0), threadPly(-1), previousPositions(nullptr), numNodes(0) {
                memset(history, 0, sizeof(history));
            }

            void initSearchData(std::vector<Position>* prevPoss, int curPly) {
                previousPositions = prevPoss;
                gamePly = curPly;
                threadPly = -1;
                threadPositions.clear();
                killerMoves1.clear();
                killerMoves2.clear();
                evalHistory.clear();
                threadPositions.resize(MAX_PLY - curPly);
                killerMoves1.resize(MAX_PLY - curPly);
                killerMoves2.resize(MAX_PLY - curPly);
                evalHistory.resize(MAX_PLY - curPly);
                memset(history, 0, sizeof(history));
                numNodes = 0;
                lastMove = NULL_MOVE;
            }

        };

        class Search_Tree;
        struct Search_Thread : public Search_Data {
            Search_Thread() : ID(0), searchTree(nullptr), exit(false) {
                sleep = true;
                exit = false;
            }

            Search_Thread(int threadID, Search_Tree* tree) : ID(threadID), searchTree(tree) {
                sleep = true;
                exit = false;
            }

            Search_Thread(const Search_Thread& other) : ID(other.ID), searchTree(other.searchTree) {
                sleep = true;
                exit = false;
            }

            ~Search_Thread() {
                exit = true;
                wakeup();
                thread.join();
            }

            void wait() {
                std::unique_lock<std::mutex> lk(threadLock);
                sleepCondition.wait(lk);
            }

            void wakeup() {
                sleep = false;
                sleepCondition.notify_one();
            }

            void init(int threadID, Search_Tree* tree) {
                ID = threadID;
                searchTree = tree;
                exit = false;
            }

            void initSearch(std::vector<Position>* prevPoss, int curPly) {
                initSearchData(prevPoss, curPly);
                exit = false;
            }

            void ponder();

            void idle() {
                exit = false;
                thread = std::thread(&Search_Thread::ponder, this);
            }

            void stopSearch() {
                exit = true;
            }

            void operator=(const Search_Thread& other) {
                ID = other.ID;
                searchTree = other.searchTree;
                exit = false;
            }

            int getNumNodes() { return numNodes; }

            int ID;
            bool stopIteration = false;

        private:
            bool sleep;
            bool exit;
            std::thread thread;

            Search_Tree* searchTree;

            std::condition_variable sleepCondition;
            std::mutex threadLock;

        };

    } // SEARCH

} // KRONOS