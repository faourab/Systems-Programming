#pragma once
#include <string>
#include <vector>
using namespace std;

#include "Order.h"
#include "Customer.h"

class BaseAction;
class Volunteer;

// Warehouse responsible for Volunteers, Customers Actions, and Orders.


class WareHouse {

    public:
        WareHouse(const string &configFilePath);
        ~WareHouse();
        WareHouse& operator=( WareHouse &other);
        WareHouse( WareHouse &other);
         WareHouse& operator=( WareHouse &&other);
        WareHouse( WareHouse &&other);
        void cleanWareHouse();
        void start();
        void addOrder(Order* order);
        void addAction(BaseAction* action);
        Customer &getCustomer(int customerId) const;
        Volunteer &getVolunteer(int volunteerId) const;
        Order &getOrder(int orderId) const;
        const vector<BaseAction*> &getActions() const;
        vector <Volunteer*>& getVol();
        vector <Order*>& getpenOrd();
        vector <Order*>& getprocOrd();
        vector <Order*>& compOrd();
        vector <BaseAction*>& getactLog();
        vector <Customer*>& getCus();
        void addtoPen(Order* order);
        void addtoInProc(Order* order);
        void addtoComp(Order* order);
        void addtoCus(Customer* customer);
        void addtovol(Volunteer* volunteer);
        void remofrompen(Order* order);
        void remfromprocess(Order* order);
        void remfromcomp(Order* order);
        void remvol(Volunteer* volunteer);
        void remCus (Customer* Customer);
        int getcustomercounter();
        int getvolunteerCounter();
        int getorderCounter();
        int setvolunteerId();
        int setOrderId();
        int setCustomer();
        bool volexist(int id);
        bool cusexist(int id);
        bool orderexist(int id);
        void close();
        void open();
    private:
        bool isOpen;
        vector<BaseAction*> actionsLog;
        vector<Volunteer*> volunteers;
        vector<Order*> pendingOrders;
        vector<Order*> inProcessOrders;
        vector<Order*> completedOrders;
        vector<Customer*> customers;
        int customerCounter; //For assigning unique customer IDs
        int volunteerCounter; //For assigning unique volunteer IDs
        int orderCounter; //For assigning unique order IDs
       

};