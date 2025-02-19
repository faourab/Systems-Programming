#include "Action.h"
#include "WareHouse.h"
#include "Order.h"
#include "Volunteer.h"
#include "Customer.h"
#include "iostream"
using std::string;
using std::vector;
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;
// Constructor
WareHouse::WareHouse(const string &configFilePath) : isOpen(false), actionsLog(), volunteers(), pendingOrders(),
                                                     inProcessOrders(), completedOrders(), customers(), customerCounter(0), volunteerCounter(0), orderCounter(0)
{
    ifstream file(configFilePath);
    if (!file.is_open())
    {
        cerr << "error opening config file" << endl;
        exit(-1);
    }
    string line;
    while (getline(file, line))
    {
        istringstream sstream(line);
        string data;
        sstream >> data;
        if (data == "customer")
        {
            string name, type;
            int distance, maxOrders;
            sstream >> name >> type >> distance >> maxOrders;
            if (type == "soldier")
            {
                customers.push_back(new SoldierCustomer(customerCounter, name, distance, maxOrders));
            }
            else
            {
                customers.push_back(new CivilianCustomer(customerCounter, name, distance, maxOrders));
            }
            customerCounter++;
        }
        else if (data == "volunteer")
        {
            string name, role;
            int val1, val2, val3;
            sstream >> name >> role >> val1 >> val2 >> val3;
            if (role == "collector")
            {
                volunteers.push_back(new CollectorVolunteer(volunteerCounter, name, val1));
            }
            else if (role == "limited_collector")
            {
                volunteers.push_back(new LimitedCollectorVolunteer(volunteerCounter, name, val1, val2));
            }
            else if (role == "driver")
            {
                volunteers.push_back(new DriverVolunteer(volunteerCounter, name, val1, val2));
            }
            else if (role == "limited_driver")
            {
                volunteers.push_back(new LimitedDriverVolunteer(volunteerCounter, name, val1, val2, val3));
            }
            volunteerCounter++;
        }
    }
}
// Destructor
WareHouse::~WareHouse()
{
    cleanWareHouse();
}
// Assignment Operator
WareHouse &WareHouse::operator=(WareHouse &other)
{
    if (&other != this)
    {
        isOpen = other.isOpen;
        customerCounter = other.customerCounter;
        volunteerCounter = other.volunteerCounter;
        orderCounter = other.orderCounter;
        cleanWareHouse();
        for (unsigned int i = 0; i < other.actionsLog.size(); i++)
        {
            actionsLog.push_back(other.actionsLog[i]->clone());
        }
        for (unsigned int i = 0; i < other.volunteers.size(); i++)
        {
            volunteers.push_back(other.volunteers[i]->clone());
        }
        for (unsigned int i = 0; i < other.customers.size(); i++)
        {
            customers.push_back(other.customers[i]->clone());
        }
        for (unsigned int i = 0; i < other.pendingOrders.size(); i++)
        {
            pendingOrders.push_back(other.pendingOrders[i]->clone());
        }
        for (unsigned int i = 0; i < other.inProcessOrders.size(); i++)
        {
            inProcessOrders.push_back(other.inProcessOrders[i]->clone());
        }
        for (unsigned int i = 0; i < other.completedOrders.size(); i++)
        {
            completedOrders.push_back(other.completedOrders[i]->clone());
        }
    }
    return *this;
}
// Copy Constructor
WareHouse::WareHouse(WareHouse &other) : isOpen(other.isOpen), actionsLog(), volunteers(), pendingOrders(), inProcessOrders(), completedOrders(), customers(), customerCounter(other.customerCounter), volunteerCounter(other.volunteerCounter), orderCounter(other.orderCounter)
{
    for (const auto &action : other.actionsLog)
    {
        actionsLog.push_back(action->clone());
    }
    for (const auto &vol : other.volunteers)
    {
        volunteers.push_back(vol->clone());
    }
    for (const auto &customer : other.customers)
    {
        customers.push_back(customer->clone());
    }
    for (const auto &penO : other.pendingOrders)
    {
        pendingOrders.push_back(penO->clone());
    }
    for (const auto &inPO : other.inProcessOrders)
    {
        inProcessOrders.push_back(inPO->clone());
    }
    for (const auto &cO : other.completedOrders)
    {
        completedOrders.push_back(cO->clone());
    }
}
// Move Assignment Operator
WareHouse &WareHouse::operator=(WareHouse &&other)
{
    if (&other != this)
    {
        cleanWareHouse();

        isOpen = other.isOpen;
        customerCounter = other.customerCounter;
        volunteerCounter = other.volunteerCounter;
        orderCounter = other.orderCounter;

        actionsLog = std::move(other.actionsLog);
        volunteers = std::move(other.volunteers);
        customers = std::move(other.customers);
        pendingOrders = std::move(other.pendingOrders);
        inProcessOrders = std::move(other.inProcessOrders);
        completedOrders = std::move(other.completedOrders);

        other.isOpen = false;
        other.customerCounter = 0;
        other.volunteerCounter = 0;
        other.orderCounter = 0;
        other.cleanWareHouse();
    }
    return *this;
}

// Move Constructor
WareHouse::WareHouse(WareHouse &&other)
    : isOpen(other.isOpen),
      actionsLog(std::move(other.actionsLog)),
      volunteers(std::move(other.volunteers)),
      pendingOrders(std::move(other.pendingOrders)),
      inProcessOrders(std::move(other.inProcessOrders)),
      completedOrders(std::move(other.completedOrders)),
      customers(std::move(other.customers)),
      customerCounter(other.customerCounter),
      volunteerCounter(other.volunteerCounter),
      orderCounter(other.orderCounter)
{
    other.isOpen = false;
    other.customerCounter = 0;
    other.volunteerCounter = 0;
    other.orderCounter = 0;
    other.cleanWareHouse();
} 

void WareHouse::cleanWareHouse()
{
    for (Order *ord : pendingOrders)
    {
        delete ord;
        ord = nullptr;
    }
    for (Order *ord : inProcessOrders)
    {
        delete ord;
        ord = nullptr;
    }
    for (Order *ord : completedOrders)
    {
        delete ord;
        ord = nullptr;
    }
    for (Volunteer *vol : volunteers)
    {
        delete vol;
        vol = nullptr;
    }
    for (Customer *cus : customers)
    {
        delete cus;
        cus = nullptr;
    }
    for (BaseAction *act : actionsLog)
    {
        delete act;
        act = nullptr;
    }
    volunteers.clear();
    customers.clear();
    actionsLog.clear();
    inProcessOrders.clear();
    pendingOrders.clear();
    completedOrders.clear();
}

void WareHouse::start()
{
    open();
    cout << "The WareHouse is Open" << endl;
    while (isOpen == true)
    {
        // get a line from user
        string line;
        getline(cin, line);
        BaseAction *action;
        istringstream sstream(line);
        string type;
        sstream >> type;
        if (type == "order")
        {
            int val1;
            sstream >> val1;
            action = new AddOrder(val1);
            action->act(*this); // act the action object
        }
        else if (type == "step")
        {
            int val1;
            sstream >> val1;
            action = new SimulateStep(val1);
       

            action->act(*this); // act the action object
        }
        else if (type == "customer")
        {
            string str1, str2;
            int val1, val2;
            sstream >> str1 >> str2 >> val1 >> val2;
            action = new AddCustomer(str1, str2, val1, val2);


            action->act(*this);// act the action object
        }
        else if (type == "orderStatus")
        {
            int val1;
            sstream >> val1;
            action = new PrintOrderStatus(val1);
            action->act(*this); // act the action object
        }
        else if (type == "customerStatus")
        {
            int val1;
            sstream >> val1;
            action = new PrintCustomerStatus(val1);
        
            action->act(*this);// act the action object
        }
        else if (type == "volunteerStatus")
        {
            int val1;
            sstream >> val1;
            action = new PrintVolunteerStatus(val1);
       
            action->act(*this); // act the action object
        }
        else if (type == "log")
        {
            action = new PrintActionsLog();
      

            action->act(*this);  // act the action object
        }
        else if (type == "close")
        {
            action = new Close();
       
            action->act(*this); // act the action object
        }
        else if (type == "backup")
        {
            action = new BackupWareHouse();
       
            action->act(*this); // act the action object
        }
        else if (type == "restore")
        {
            action = new RestoreWareHouse();
            action->act(*this);// act the action object
        }
        else
        {
            cerr << "Unknown command " << type << endl;
            continue;
        }
        // add to actions log
        addAction(action);
    }
    for (Order *o : pendingOrders)
    {
        cout << o->toString() << endl;
    }
    for (Order *o : inProcessOrders)
    {
        cout << o->toString() << endl;
    }
    for (Order *o : completedOrders)
    {
        cout << o->toString() << endl;
    }
}

int WareHouse::getcustomercounter()
{
    return customerCounter;
}
int WareHouse::getvolunteerCounter()
{
    return volunteerCounter;
}
int WareHouse::getorderCounter()
{
    return orderCounter;
}
int WareHouse::setvolunteerId()
{
    int output = volunteerCounter;
    volunteerCounter++;
    return output;
}
int WareHouse::setOrderId()
{
    int output = orderCounter;
    orderCounter++;
    return output;
}
int WareHouse::setCustomer()
{
    int output = customerCounter;
    customerCounter++;
    return output;
}
bool WareHouse::volexist(int id)
{
    for (Volunteer *vol : volunteers)
    {
        if (vol->getId() == id)
        {
            return true;
        }
    }

    return false;
}
bool WareHouse::cusexist(int id)
{
    for (Customer *cus : customers)
    {
        if (cus->getId() == id)
        {
            return true;
        }
    }
    return false;
}
bool WareHouse::orderexist(int id)
{
    for (Order *ord : pendingOrders)
    {
        if (ord->getId() == id)
        {
            return true;
        }
    }
    for (Order *ord : inProcessOrders)
    {
        if (ord->getId() == id)
        {
            return true;
        }
    }
    for (Order *ord : completedOrders)
    {
        if (ord->getId() == id)
        {
            return true;
        }
    }
    return false;
}
void WareHouse::close()
{
    isOpen = false;
}
void WareHouse::open()
{
    isOpen = true;
}

vector<Volunteer *> &WareHouse::getVol()
{
    return volunteers;
}

vector<Order *> &WareHouse::getpenOrd()
{
    return pendingOrders;
}

vector<Order *> &WareHouse::getprocOrd()
{
    return inProcessOrders;
}

vector<Order *> &WareHouse::compOrd()
{
    return completedOrders;
}

vector<BaseAction *> &WareHouse::getactLog()
{
    return actionsLog;
}

vector<Customer *> &WareHouse::getCus()
{
    return customers;
}

void WareHouse::addtoPen(Order *order)
{
    pendingOrders.push_back(order);
}

void WareHouse::addtoInProc(Order *order)
{
    inProcessOrders.push_back(order);
}

void WareHouse::addtoComp(Order *order)
{
    completedOrders.push_back(order);
}

void WareHouse::addtoCus(Customer *customer)
{
    customers.push_back(customer);
}

void WareHouse::addtovol(Volunteer *volunteer)
{
    volunteers.push_back(volunteer);
}

void WareHouse::remofrompen(Order *order)
{
    for (unsigned int i = 0; i < pendingOrders.size(); i++)
    {
        if (pendingOrders[i] == order)
        {
            pendingOrders.erase(pendingOrders.begin() + i);
        }
    }
}

void WareHouse::remfromprocess(Order *order)
{
    for (unsigned int i = 0; i < inProcessOrders.size(); i++)
    {
        if (inProcessOrders[i] == order)
        {
            inProcessOrders.erase(inProcessOrders.begin() + i);
        }
    }
}

void WareHouse::remfromcomp(Order *order)
{

    for (unsigned int i = 0; i < completedOrders.size(); i++)
    {
        if (completedOrders[i] == order)
        {
            completedOrders.erase(completedOrders.begin() + i);
        }
    }
}

void WareHouse::remvol(Volunteer *volunteer)
{

    for (unsigned int i = 0; i < volunteers.size(); i++)
    {
        if (volunteers[i] == volunteer)
        {
            volunteers.erase(volunteers.begin() + i);
        }
    }
}

void WareHouse::addOrder(Order *order)
{
    pendingOrders.push_back(order);
}

void WareHouse::addAction(BaseAction *action)
{
    actionsLog.push_back(action);
}

Customer &WareHouse::getCustomer(int customerId) const
{
    Customer *cs = nullptr;
    for (unsigned int i = 0; i < customers.size(); i++)
    {
        if (customers[i]->getId() == customerId)
        {
            cs = customers[i];
        }
    }
    return *cs;
}

Volunteer &WareHouse::getVolunteer(int volunteerId) const
{
    Volunteer *vol = nullptr;
    for (unsigned int i = 0; i < volunteers.size(); i++)
    {
        if (volunteers[i]->getId() == volunteerId)
        {
            vol = volunteers[i];
        }
    }
    return *vol;
}

Order &WareHouse::getOrder(int orderId) const
{
    Order *ord = nullptr;
    for (unsigned int i = 0; i < pendingOrders.size(); i++)
    {
        if (pendingOrders[i]->getId() == orderId)
        {
            ord = pendingOrders[i];
        }
    }
    for (unsigned int i = 0; i < inProcessOrders.size(); i++)
    {
        if (inProcessOrders[i]->getId() == orderId)
        {
            ord = inProcessOrders[i];
        }
    }
    for (unsigned int i = 0; i < completedOrders.size(); i++)
    {
        if (completedOrders[i]->getId() == orderId)
        {
            ord = completedOrders[i];
        }
    }
    return *ord;
}

const vector<BaseAction *> &WareHouse::getActions() const
{
    return actionsLog;
}
