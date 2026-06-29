#include "private.h"
#include <string>
#include <vector>
#include <map>


LinkLister::LinkLister()
{
	curptr = 0;
	end = 0;
	root = new LinkNoder;
	if (root == 0) { return; }
	root->sbbID = -1;
	root->curSbbptr = 0;
	curptr = root;
	end = root;
}

LinkLister::~LinkLister()
{
	if (root != 0) {
		releaseLister(root);
		root = 0;
		end = 0;
		curptr = 0;
	}
}

bool LinkLister::fillRoot(StepBlockBase* v) {
	if (root == 0) { return false; }
	root->curSbbptr = v;
	root->sbbID = v->ID();
	return true;
}


bool LinkLister::Insert(StepBlockBase* v, int index) {
	if (curptr == root && root == end) {
		fillRoot(v);
		return true;
	}
	//if (curptr == 0) { return false; }
	//LinkNoder* prev = curptr;

	//curptr = new LinkNoder;
	//if (curptr == 0) { return false; }
	//curptr->id = NextId;
	//curptr->index = index;
	//curptr->curSbbptr = v;
	//prev->nextGrps.push_back(curptr);
	//NextId += 1;
	return true;
}

bool LinkLister::InsertAttend(int Aid, StepBlockBase* v, int index) {

	return true;
}

void LinkLister::isolated(std::vector<StepBlockBase*>& out) {


}

void LinkLister::LinkerPush(int sid, int eid) {
	std::map<int, std::vector<int>>::iterator fitr = linkGrp.find(sid);
	if (fitr != linkGrp.end()) {
		std::vector<int>::iterator fv = std::find(linkGrp[sid].begin(), linkGrp[sid].end(), eid);
		if (fv == linkGrp[sid].end()) {
			linkGrp[sid].push_back(eid);
		}
		return;
	}
	std::vector<int> v;
	v.push_back(eid);
	linkGrp[sid] = v;
}

int* LinkLister::NextStepID(int curid, unsigned& Nums) {
	Nums = 0;
	std::map<int, std::vector<int>>::iterator fitr = linkGrp.find(curid);
	if (fitr != linkGrp.end()) {
		Nums = fitr->second.size();
		return &(fitr->second)[0];
	}
	return 0;
}

std::string LinkLister::SaveStringGet() {
	stringstream ss;
	for (auto fitr = linkGrp.begin();
		fitr != linkGrp.end(); fitr++) {
		ss << std::to_string(fitr->first) << "-";
		for (int eis = 0; eis < fitr->second.size(); eis++) {
			ss << std::to_string(fitr->second[eis]) << ",";
		}
		ss << "\n";
	}
	std::string result = ss.str();
	return result;
}

bool LinkLister::LineCfgStringLoader(std::string str) {
	if (str.length() < 2) { return true; }
	std::vector<std::string> line = stringSplit(str, '-');
	if (line.size() < 2) { 
		
		return false; 
	}
	std::vector<std::string> cids = stringSplit(line[1], ',');
	std::vector<int> Cids;
	for (int jx = 0; jx < cids.size(); jx++) {
		if (cids[jx] == "\r" || cids[jx] == "\n") { continue; }
		Cids.push_back(atoi(cids[jx].c_str()));
	}
	std::pair<int, std::vector<int>> v(atoi(line[0].c_str()), Cids);
	linkGrp.insert(v);
	return true;
}


bool LinkLister::Remove(int id) {
	if (linkGrp.size() > 0) {
		std::map<int, std::vector<int>>::iterator fitr = linkGrp.find(id);
		if (fitr != linkGrp.end()) {
			linkGrp.erase(fitr);
		}
		for (auto sItr = linkGrp.begin(); sItr != linkGrp.end(); sItr++) {
			std::vector<int>::iterator ftitr = std::find(sItr->second.begin(), sItr->second.end(), id);
			if (ftitr < sItr->second.end()) {
				sItr->second.erase(ftitr);
			}
		}
	}
	return true;
}

LinkNoder* LinkLister::Find(int id) {



	return 0;
}


void LinkLister::prevnullsetUpdate(LinkNoder* c) {
	int pl = c->prevGrps.size();
	if (pl < 2) { return; }
	for (int x = 0; x < pl; x++) {
		LinkNoder* prev = c->prevGrps[x];
		if (prev->nextGrps.size() < 1) {
			prev->nextGrps.clear();
		}
		else {
			for (int m = 0; m < prev->nextGrps.size(); m++) {
				if (prev->nextGrps[m] == c) {
					prev->nextGrps[m] = 0;
				}
			}
		}
	}
}


void LinkLister::releaseLister(LinkNoder* r) {
	LinkNoder* next = r;
	int nextNums = 0;
	while (next != 0) {
		nextNums = next->nextGrps.size();
		if (nextNums < 1) {
			prevnullsetUpdate(next);
			delete next;
			break;
		}
		else if (nextNums == 1) {
			curptr = next;
			next = next->nextGrps[0];
			prevnullsetUpdate(curptr);
			delete curptr;
		}
		else {
			for (int i = 0; i < nextNums; i++) {
				releaseLister(next->nextGrps[i]);
			}
			next->nextGrps.clear();
			delete next;
			break;
		}
	}
}


void LinkLister::Clear() {
	releaseLister(root);
	root = 0;
	end = 0;
	curptr = 0;

	linkGrp.clear();
}
