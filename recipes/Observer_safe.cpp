#include <vector>
#include <iostream>
#include <algorithm>
#include <memory>
#include <mutex>
using namespace std;

class Observable;

class Observer : public enable_shared_from_this<Observer>
{
public:
  virtual ~Observer() 
  {
//    m_subject->UnRegister(this);
  }
  virtual void Update() = 0;
  void Observe(Observable *s);

private:
  Observable *m_subject;
};

//not 100% thread safe
class Observable 
{
public:
  void Register(weak_ptr<Observer> x)
  {
    m_observers.push_back(x);
  }
//  void UnRegister(); //don't need
  void Notify()
  {
    m_mutex.lock();
    vector<weak_ptr<Observer>>::iterator it;
    for (it = m_observers.begin(); it != m_observers.end(); )
    {
      shared_ptr<Observer> obj(it->lock());
      if (obj) 
      {
        obj->Update();
        ++it;
      }
      else
      {
        printf("auto UnRegister\n");
        m_observers.erase(it); //auto UnRegister
      }
    }
    m_mutex.unlock();
  }

private:
  vector<weak_ptr<Observer>> m_observers;
  std::mutex m_mutex;
};

void Observer::Observe(Observable *s)
{
  //If this pointer show in construct function, it's not thread safe. 
  s->Register(shared_from_this());
  m_subject = s;
}

class Foo : public Observer
{
public:
  virtual void Update()
  {
    cout << "Foo Update()\n";
  }
};

class Lead : public Observer
{
public:
  virtual void Update()
  {
    cout << "Lead Update()\n";
  }
};

int main()
{
  Observable subject;

  shared_ptr<Foo> pfoo(new Foo);
  pfoo->Observe(&subject);
  {
    shared_ptr<Lead> plead(new Lead);
    plead->Observe(&subject);
    subject.Notify();
  }

  subject.Notify();
  return 0;
}
