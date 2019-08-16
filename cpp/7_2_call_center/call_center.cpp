#include <iostream>
#include <deque>
#include <vector>
#include <chrono>
#include <ctime>
#include <thread>
#include <mutex>

using namespace std;

enum class Rank {None, Respondent, Manager, Director};

int GetRandom(int max){
  srand(time(NULL));
  return rand() % max;
}

class Callee{
  string name;
  Rank rank;
public:
  string getName(){
    return name;
  }
  Rank getRank(){
    return rank;
  }
protected:
  void setName(string n){
    name = n;
  }
  void setRank(Rank r){
    rank = r;
  }
};

class Respondent: public Callee{
public:
  Respondent(string n){
    setName(n);
    setRank(Rank::Respondent);
  }
  void print(){
    cout << "Respondent " << getName() << endl;
  }
};

class Manager: public Callee{
public:
  Manager(string n){
    setName(n);
    setRank(Rank::Manager);
  }
  void print(){
    cout << "Manager " << getName() << endl;
  }
};

class Director: public Callee{
public:
  Director(string n){
    setName(n);
    setRank(Rank::Director);
  }
  void print(){
    cout << "Director " << getName() << endl;
  }
};

class NoOne: public Callee{
public:
  NoOne(){
    setRank(Rank::None);
  }
};

class CallCenter{
  deque<Respondent> respondents;
  deque<Manager> managers;
  deque<Director> directors;
public:
  CallCenter(vector<Callee> & cs){
    for (auto c: cs){
      if (c.getRank() == Rank::Respondent) {
        respondents.push_back(*((Respondent*)&c));
      }
      if (c.getRank() == Rank::Manager) {
        managers.push_back(*((Manager*)&c));
      }
      if (c.getRank() == Rank::Director) {
        directors.push_back(*((Director*)&c));
      }
    }
  }

  void print(){
    for (auto r: respondents){
      r.print();
    }
    for (auto m: managers){
      m.print();
    }
    for (auto d: directors){
      d.print();
    }
  }

  Callee getAvailableCallee(){
    Callee c;
    if (!respondents.empty()){
      c = respondents.front();
      respondents.pop_front();
      return c;
    } else if (!managers.empty()){
      c = managers.front();
      managers.pop_front();
      return c;
    } else if (!directors.empty()){
      c = directors.front();
      directors.pop_front();
      return c;
    } else return NoOne();
  }

  void push_back_callee(Callee &c){
    if (c.getRank() == Rank::Respondent){
      Respondent* r = (Respondent*)(&c);
      respondents.push_back(*r);
    } else if (c.getRank() == Rank::Manager){
      Manager* m = (Manager*)(&c);
      managers.push_back(*m);
    } else {
      Director* d = (Director*)(&c);
      directors.push_back(*d);
    }
  }

};

class Caller{
  string name;
public:
  Caller();
  Caller(string n){
    name = n;
  }
  string getName(){
    return name;
  }
};


mutex mtx;

void Dispatch_call(Caller caller, CallCenter* cc){
  chrono::steady_clock::time_point dialed;
  chrono::steady_clock::time_point answered;
  chrono::steady_clock::time_point end;

  mtx.lock();
  cout << caller.getName() << " is calling..." << endl;
  dialed = chrono::steady_clock::now();
  Callee callee = cc->getAvailableCallee();
  if (callee.getRank() == Rank::None){
    cout << "No one answered " << caller.getName() << "'s call...'" << endl;
    mtx.unlock();
  } else {
    mtx.unlock();
    this_thread::sleep_for(std::chrono::seconds(GetRandom(2)));
    answered = chrono::steady_clock::now();
    mtx.lock();
    cout << "Call from "<< caller.getName() <<" answered by " << callee.getName() << endl;
    mtx.unlock();
    this_thread::sleep_for(std::chrono::seconds(GetRandom(7)));
    end = chrono::steady_clock::now();
    mtx.lock();
    cout << "Call ended between " << caller.getName() << " and " << callee.getName()
       << ". Duration: " << chrono::duration_cast<std::chrono::seconds>(end - answered).count() << " seconds" << endl;
    cc->push_back_callee(callee);
    mtx.unlock();}
  }


  int main(int argc, char const *argv[]) {
    vector<Callee> callees = {Respondent("Eloi"),Manager("Teixeira"),Director("Pereira")};
    CallCenter call_center(callees);
    vector<Caller> callers = {Caller("Alice"), Caller("Bob"), Caller("Artur"), Caller("Lourdes")};

    vector<thread> threads;
    for (auto& c: callers){
      threads.emplace_back(Dispatch_call, c, &call_center);
    }

    for (auto& t: threads){
      t.join();
    }

    return 0;
  };
