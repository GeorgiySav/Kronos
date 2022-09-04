#pragma once
#include "Threads.h"

namespace KRONOS {
	namespace SEARCH {

        class Search_Tree;
        class Thread_Manager {
        private:
            std::vector<Search_Thread> threads;
            Search_Tree* tree;
            int NUM_THREADS;

        public:
            Thread_Manager() : NUM_THREADS(1), tree(nullptr) {};
            ~Thread_Manager() {};

            void init(int numThreads, Search_Tree* tree) {
                threads.resize(numThreads);
                this->NUM_THREADS = numThreads;
                this->tree = tree;
                for (int i = 0; i < numThreads; i++) {
                    threads[i].init(i, tree);
                    threads[i].idle();
                }
            }

            void initSearch(std::vector<Position>* prevPoss, int curPly) {
                for (int i = 0; i < NUM_THREADS; i++) {
                    threads[i].initSearch(prevPoss, curPly);
                }
            }

            void beginSearch() {
                for (int i = 0; i < NUM_THREADS; i++) {
                    threads[i].wakeup();
                }
            }

            void stopIteration() {
                for (int i = 0; i < NUM_THREADS; i++) {
                    threads[i].stopIteration = true;
                }
            }

            int countNodes() {
                int count = 0;
                for (int i = 0; i < NUM_THREADS; i++) {
                    count += threads[i].numNodes;
                }
                return count;
            }

            Move getFirstValidMove() {
                for (int i = 0; i < NUM_THREADS; i++) {
                    if (threads[i].bestMoveThisIteration != NULL_MOVE) return threads[i].bestMoveThisIteration;
                }
            }

        };

	}
}

