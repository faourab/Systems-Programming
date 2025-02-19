#include "Customer.h"
#include <string>
#include "Volunteer.h"
#include "Order.h"
#include "WareHouse.h"

using std::string;
using std::vector;

Customer::Customer(int id, const string &name, int locationDistance, int maxOrders)
    : id(id), name(name), locationDistance(locationDistance), maxOrders(maxOrders), ordersId() {}

const string &Customer::getName() const
{
    return name;
}
int Customer::getId() const
{
    return id;
}

int Customer::getCustomerDistance() const
{
    return locationDistance;
}

int Customer::getMaxOrders() const
{
    return maxOrders;
}

int Customer::getNumOrders()const
{
    return ordersId.size();
}

bool Customer::canMakeOrder() const
{
    int ON = getNumOrders();
    return maxOrders > ON;
}

const vector<int> &Customer ::getOrdersIds() const
{
    return ordersId;
}

int Customer::addOrder(int orderId)
{
    for (unsigned i = 0; i < ordersId.size(); i++)
    {
        if (ordersId[i] == orderId)
        {
            return -1;
        }
    }
        this->ordersId.push_back(orderId);
        return orderId;
}

SoldierCustomer::SoldierCustomer(int id, string name, int locationDistance, int maxOrders) : Customer(id, name, locationDistance, maxOrders) {}

SoldierCustomer *SoldierCustomer::clone() const
{
    return new SoldierCustomer(*this);
}

string SoldierCustomer::toString()
{
    return "CustomerID:" + to_string(getId());
}

CivilianCustomer::CivilianCustomer(int id, string name, int locationDistance, int maxOrders) : Customer(id, name, locationDistance, maxOrders) {}

CivilianCustomer *CivilianCustomer::clone() const
{
    return new CivilianCustomer(*this);
}

string CivilianCustomer::toString()
{
    return "CustomerID :" + to_string(getId());
}
