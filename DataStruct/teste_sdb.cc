#include <set>
#include <functional>
#include <iostream>
#include "sdb.hh"
#include "utils.hh"

using namespace std;

struct teste {
	int a,b;
	teste(int a, int b) : a(a), b(b) { }
};

ostream& operator<<(ostream& stream, const teste& t) {
	stream << "(" << t.a << "," << t.b << ")";
	return stream;
}

int main(void) {
	SimpleDatabase<set<int> > sdb;
	SimpleDatabase<teste> sdbt;
	sdbt.insert(teste(0,0));
	sdbt.insert(teste(1,7));
	SDBIndex<teste, KeyMember(&teste::a), false>* id1 = sdbt.createIndex<KeyMember(&teste::a), false>();
	SDBIndex<teste, KeyMember(&teste::b), false>* id2 = sdbt.createIndex<KeyMember(&teste::b), false>();
	sdbt.insert(teste(2,10));
	sdbt.insert(teste(3,17));
	cout << *id1->find(2)->second << endl;
	cout << *id1->find(0)->second << endl;
	cout << *id2->find(17)->second << endl;
	cout << *id2->find(10)->second << endl;
	cout << (id1->find(4)!=id1->end()?"ta":"num ta") << endl;
	set<int> tmp;
	tmp.insert(7);
	tmp.insert(3);
	tmp.insert(1);
	sdb.insert(tmp);
	SDBIndex<set<int>, KeyElements<set<int> >, true>* index = sdb.createIndex<KeyElements<set<int> >, true>();
	tmp.clear();
	tmp.insert(7);
	tmp.insert(8);
	tmp.insert(9);
	sdb.insert(tmp);
	tmp.clear();
	tmp.insert(1);
	tmp.insert(9);
	sdb.insert(tmp);
	tmp.clear();
	foreach_pair(s,index->equal_range(9)) {
		foreach(n, *s->second) {
			cout << *n << ' ';
		}
		cout << '\n';
	}
	cout << '\n';
	sdb.erase(1,index);
	foreach_pair(s,index->equal_range(9)) {
		foreach(n, *s->second) {
			cout << *n << ' ';
		}
		cout << '\n';
	}


	return 0;
}
