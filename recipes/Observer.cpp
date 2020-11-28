#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

class Observable;

class Observer 
{
public:
  virtual ~Observer() {}
  virtual void Update() = 0;
  void Observe(Observable *s);

private:
  Observable *m_subject;
};

class Observable 
{
public:
  void Register(Observer *x)
  {
    m_observers.push_back(x);
  }
  void UnRegister(Observer *x)
  {
    vector<Observer *>::iterator it = std::find(m_observers.begin(), m_observers.end(), x);
    m_observers.erase(it);
  }
  void Notify()
  {
    for (int i = 0; i < m_observers.size(); ++i)
    {
      Observer *x = m_observers[i];
      if (x) //not thread-safe
      {
        x->Update();
      }
    }
  }
private:
  vector<Observer *> m_observers;
};

void Observer::Observe(Observable *s)
{
  //If this pointer show in construct function, it's not thread safe. 
  s->Register(this);
}

class Foo : public Observer
{
public:
  virtual void Update()
  {
    cout << __LINE__ << "," << __FUNCTION__ << endl;
  }
};

class Lead : public Observer
{
public:
  virtual void Update()
  {
    cout << __LINE__ << "," << __FUNCTION__ << endl;
  }
};

int main()
{
  Observable subject;

  Foo foo;
  subject.Register(&foo);
  Lead lead;
  lead.Observe(&subject);

  subject.Notify();
  subject.UnRegister(&lead);
  subject.Notify();

  return 0;
}
