#include <bits/stdc++.h>
#include "mymemorypool.h"
using namespace std;

class foo {
 public:
  char s[10];
  int x, y;
  foo() {}
  foo(int x, int y) : x(x), y(y) {}
  foo(int x, int y, const char* _s) : x(x), y(y) {
    for (int i = 0; i < 10; ++i)
    s[i] = _s[i];
  }
};

MyMemoryPool<sizeof(foo)> pool;
vector<foo*> v;

int main() {
  pool.init(0x23233, 10);
  cout << "Hello, world!" << endl;
  for (int i = 0; i < 15; ++i) {
    pool.print();
    foo* pos = (foo*)pool.apply();
    if (pos == NULL) {
      cout << "No memory" << endl;
    }
    else {
      pos = new(pos) foo(2, 3, "123456789");
      v.push_back(pos);
    }
  }
  cout << v.size() << endl;
  for (int i = 0; i < v.size(); ++i) {
    cout << v[i]->x << " " << v[i]->y << " " << v[i]->s << endl;
  }
  for (int i = 0; i < 10; ++i) {
    pool.release(v.back());
    std::cout << "release " << v.back() << std::endl;
    v.pop_back();
  }
  pool.print();
    for (int i = 0; i < 12; ++i) {
    pool.print();
    foo* pos = (foo*)pool.apply();
    if (pos == NULL) {
      cout << "No memory" << endl;
    }
    else {
      pos = new(pos) foo(2, 3, "123456789");
      v.push_back(pos);
    }
  }
  cout << v.size() << endl;
  for (int i = 0; i < v.size(); ++i) {
    cout << v[i]->x << " " << v[i]->y << " " << v[i]->s << endl;
  }
  return 0;
}