#include "Volunteer.h"
#include "Order.h"
#include "WareHouse.h"


using std::string;
using std::vector;

Volunteer::Volunteer(int id, const string &name): 
completedOrderId(NO_ORDER),activeOrderId(NO_ORDER), id(id),name(name) {}


int Volunteer::getId() const
{
    return id;
}

const string &Volunteer::getName() const
{
    return name ;
}

int Volunteer::getActiveOrderId() const
{
    return activeOrderId;
}

int Volunteer::getCompletedOrderId() const
{
    return completedOrderId;
}

bool Volunteer::isBusy() const
{   
    return (getActiveOrderId() != NO_ORDER );  
}

CollectorVolunteer::CollectorVolunteer(int id, string name, int coolDown):
Volunteer(id, name),coolDown(coolDown),timeLeft(coolDown){}


CollectorVolunteer *CollectorVolunteer::clone() const
{
    return new CollectorVolunteer(*this);
}

void CollectorVolunteer::step()
{
    if (decreaseCoolDown())
    {
        completedOrderId=activeOrderId;
        activeOrderId=NO_ORDER;
    }
}

int CollectorVolunteer::getCoolDown() const
{
    return coolDown;
}

int CollectorVolunteer::getTimeLeft() const
{
    return timeLeft;
}

bool CollectorVolunteer::decreaseCoolDown()
{
    timeLeft--;
 if (timeLeft == 0)
 {
    return true;
 }
 else
 {
    return false;
 }
}
 bool CollectorVolunteer::hasOrdersLeft() const
    {
        return true;
    }
bool CollectorVolunteer::canTakeOrder(const Order &order) const
{
    return (!this->isBusy() && (order.getStatus() == OrderStatus::PENDING));
   
    
}
void CollectorVolunteer::acceptOrder(const Order &order)
{
    if (this->canTakeOrder(order))
    {
        activeOrderId = order.getId();
        timeLeft = coolDown;
    }
    
}

string CollectorVolunteer::toString() const
{
    string output;
    output += "CollectorVolunteer " + to_string(this->getId()) += "\n";  
    output += "IsBusy: ";
    if (this-> isBusy())
    {
        output += "true \n";
        output += "OrderId :" + to_string(this->getActiveOrderId()) += "\n";
        output += "TimeLeft :" + to_string(this->getTimeLeft())+= "\n";
    }
    else
    {
        output += "false \n";
        output += "None \n";     
        output += "None \n";     
    }   
        output += "OrdersLeft : No Limit";      

        return output;
}

int CollectorVolunteer::help()
{
    return -1;
}

LimitedCollectorVolunteer::LimitedCollectorVolunteer(int id, string name, int coolDown, int maxOrders):
CollectorVolunteer(id,name,coolDown),maxOrders(maxOrders),ordersLeft(maxOrders){}





LimitedCollectorVolunteer *LimitedCollectorVolunteer::clone() const
{
    return new LimitedCollectorVolunteer(*this);
}

bool LimitedCollectorVolunteer::hasOrdersLeft() const
{
    return (ordersLeft > 0);
}

bool LimitedCollectorVolunteer::canTakeOrder(const Order &order) const
{
    return (CollectorVolunteer::canTakeOrder(order) && this->hasOrdersLeft());
}

void LimitedCollectorVolunteer::acceptOrder(const Order &order)
{
    if (this->canTakeOrder(order))
    {
      CollectorVolunteer :: acceptOrder(order);
        ordersLeft --;
    }
    
}

int LimitedCollectorVolunteer::getMaxOrders() const
{
    return maxOrders;
}

int LimitedCollectorVolunteer::getNumOrdersLeft() const
{
    return ordersLeft;
}

string LimitedCollectorVolunteer::toString() const
{
   string output;
    output += "LimitedCollectorVolunteer " + to_string(this->getId()) += "\n";  
    output += "IsBusy: ";
    if (this-> isBusy())
    {
        output += "true \n";
        output += "OrderId :" + to_string(this->getActiveOrderId()) += "\n";
        output += "TimeLeft :" + to_string(this->getTimeLeft())+= "\n";
    }
    else
    {
        output += "false \n";
        output += "None \n";     
        output += "None \n";     
    }   
        output += "OrdersLeft : " + to_string(this->getNumOrdersLeft());      

        return output;
}

int LimitedCollectorVolunteer::help()
{
    return -1;
}

DriverVolunteer::DriverVolunteer(int id, string name, int maxDistance, int distancePerStep):
Volunteer(id, name),maxDistance(maxDistance),distancePerStep(distancePerStep),distanceLeft(maxDistance) {}


DriverVolunteer *DriverVolunteer::clone() const
{
    return new DriverVolunteer(*this);
}

int DriverVolunteer::getDistanceLeft() const
{
    return distanceLeft;
}

int DriverVolunteer::getMaxDistance() const
{
    return maxDistance;
}

int DriverVolunteer::getDistancePerStep() const
{
    return distancePerStep;
}

bool DriverVolunteer::decreaseDistanceLeft()
{
    distanceLeft -= distancePerStep;
    if (distanceLeft <= 0)
    {
        distanceLeft = 0;
        return true ;
    }
    else
    {
     
        return false;
    }    
}

bool DriverVolunteer::canTakeOrder(const Order &order) const
{
    return (!this->isBusy() && (order.getStatus() == OrderStatus::COLLECTING) && (order.getDistance() <= this->getMaxDistance()));
    
}

bool DriverVolunteer::hasOrdersLeft() const
{
    return true;
}

void DriverVolunteer::acceptOrder(const Order &order)
{
    if (this->canTakeOrder(order))
    {
        activeOrderId = order.getId();
        distanceLeft = order.getDistance();
    }
}

void DriverVolunteer::step()

{
    if (decreaseDistanceLeft())
    {
    completedOrderId=activeOrderId;
    activeOrderId=NO_ORDER;
    }
}

string DriverVolunteer::toString() const
{
    string output;
    output += "DriverVolunteer " + to_string(getId()) += "\n";  
    output += "IsBusy: ";
    if (this-> isBusy())
    {
        output += "true \n";
        output += "OrderId :" + to_string(this->getActiveOrderId()) += "\n";
        output += "DistanceLeft :" + to_string(this->getDistanceLeft())+= "\n";
    }
    else
    {
        output += "false \n";
        output += "None \n";     
        output += "None \n";     
    }   
        output += "OrdersLeft : No Limit";      

        return output;
    }

    int DriverVolunteer::help()
    {
        return distancePerStep;
    }

LimitedDriverVolunteer::LimitedDriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep, int maxOrders):
DriverVolunteer(id,name,maxDistance,distancePerStep),maxOrders(maxOrders), ordersLeft(maxOrders) {}


LimitedDriverVolunteer *LimitedDriverVolunteer::clone() const
{
    return new LimitedDriverVolunteer(*this);
}

int LimitedDriverVolunteer::getMaxOrders() const
{
    return maxOrders;
}

int LimitedDriverVolunteer::getNumOrdersLeft() const
{
    return ordersLeft;
}

bool LimitedDriverVolunteer::hasOrdersLeft() const
{
    return ordersLeft>0;
}

bool LimitedDriverVolunteer::canTakeOrder(const Order &order) const
{
     return (!this->isBusy() && (order.getStatus() == OrderStatus::COLLECTING) && (order.getDistance() <= this->getMaxDistance())
     && this-> hasOrdersLeft());
}

void LimitedDriverVolunteer::acceptOrder(const Order &order)
{
         if (this->canTakeOrder(order))
    {
       DriverVolunteer :: acceptOrder(order);
       ordersLeft--;
    }
}

string LimitedDriverVolunteer::toString() const
{
    string output;
    output += "LimitedDriverVolunteer " + to_string(getId()) += "\n";  
    output += "IsBusy: ";
    if (this-> isBusy())
    {
        output += "true \n";
        output += "OrderId :" + to_string(this->getActiveOrderId()) += "\n";
        output += "DistanceLeft :" + to_string(this->getDistanceLeft())+= "\n";
    }
    else
    {
        output += "false \n";
        output += "None \n";     
        output += "None \n";     
    }   
        output += "OrdersLeft : " +  to_string(this->getNumOrdersLeft());      

        return output;
}

int LimitedDriverVolunteer::help()
{
    return getDistancePerStep();
}
