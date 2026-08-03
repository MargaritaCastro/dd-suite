#ifndef DD_OPTIMIZATION_PROGRAM_SEQUENCINGSTATE_H
#define DD_OPTIMIZATION_PROGRAM_SEQUENCINGSTATE_H

#include <set>
#include <string>
#include <sstream>

using namespace std;

struct SequencingState {
    set<int> scheduled;   // jobs already assigned to a slot
    int      last_job;     // index of previous job; -1 = depot

    bool operator==(const SequencingState& o) const {
        return scheduled == o.scheduled &&
               last_job == o.last_job;
    }

    string to_string() const {
        ostringstream oss;
        oss << "[";
        bool first = true;
        for (int j : scheduled) {
            if (!first) oss << ", ";
            oss << j;
            first = false;
        }
        // Clave (scheduled, last_job):
        oss << "]_" << last_job;
        return oss.str();
    }
};

#endif //DD_OPTIMIZATION_PROGRAM_SEQUENCINGSTATE_H
