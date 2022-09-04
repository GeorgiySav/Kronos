#include "Threads.h"

#include "Search.h"

namespace KRONOS {
	namespace SEARCH {
		void Search_Thread::ponder() {
            while (!exit) {
                if (sleep) {
                    wait();
                }
                else {
                    searchTree->iterativeDeepening(*this);
                    sleep = true;
                }
            }
        }
	}
}