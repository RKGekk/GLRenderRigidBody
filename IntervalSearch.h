#ifndef INTERVALSEARCH_H
#define INTERVALSEARCH_H

#include <vector>
using namespace std;

struct IntervalVal {
	int val;
	int id;
};

struct IntervalGroup {
	int from;
	int to;
	int id;
};

class IntervalSearch {

private:
	vector<IntervalVal> vals;

public:
	IntervalSearch() {

	}

	void push(IntervalVal v) {
		vals.push_back(v);
	}

	vector<IntervalGroup> getIntervals() {

		vector<IntervalGroup> result;

		int interStart = 0;
		int interEnd = 0;
		int id = 0;

		for(int i = 0; i < vals.size(); ++i) {
			id = vals[i].id;
			interStart = i;
			interEnd = i;
			
			for(int j = i + 1; j < (vals.size() + 1); ++j) {
				if(j == vals.size()) {
					interEnd = j - 1;
					i = interEnd;
					result.push_back(IntervalGroup {interStart, interEnd, id});
					break;
				}
				if(id != vals[j].id) {
					interEnd = j - 1;
					i = interEnd;
					result.push_back(IntervalGroup {interStart, interEnd, id});
					break;
				}
			}
		}

		return result;
	}
};

#endif