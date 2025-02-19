#include "../include/Action.h"
#include "../include/WareHouse.h"
#include "../include/Order.h"
#include "../include/Volunteer.h"
#include "../include/Customer.h"
#include "iostream"

using std::string;
using std::vector;
using namespace std;

SimulateStep ::SimulateStep(int numOfSteps) : BaseAction(), numOfSteps(numOfSteps) {}

void SimulateStep::act(WareHouse &wareHouse)
{
    for (int i = 0; i < numOfSteps; i++)
    {
        for (auto it = wareHouse.getpenOrd().begin(); it != wareHouse.getpenOrd().end();)
        {
            bool added = false;
            if ((*it)->getStatus() == OrderStatus::PENDING)
            {
                for (unsigned int it1 = 0; it1 < wareHouse.getVol().size(); it1++)
                {
                    if (wareHouse.getVol()[it1]->canTakeOrder(*(*it)))
                    {
                        wareHouse.getVol()[it1]->acceptOrder(*(*it));
                        (*it)->setCollectorId(wareHouse.getVol()[it1]->getId());
                        (*it)->setStatus(OrderStatus::COLLECTING);
                        wareHouse.addtoInProc((*it));
                        it = wareHouse.getpenOrd().erase(it);
                        added = true;
                        break;
                    }
                }
            }
            else if ((*it)->getStatus() == OrderStatus::COLLECTING)
            {

                for (Volunteer *it2 : (wareHouse.getVol()))
                {

                    if (it2->canTakeOrder(*(*it)))
                    {
                        if (it2->help() >= 0)
                        {
                            it2->acceptOrder(*(*it));
                            (*it)->setDriverId(it2->getId());
                            (*it)->setStatus(OrderStatus::COMPLETED);
                            wareHouse.addtoComp((*it));
                            it = wareHouse.getpenOrd().erase(it);
                            added = true;
                            break;
                        }
                        else
                        {
                            it2->acceptOrder(*(*it));
                            (*it)->setDriverId(it2->getId());
                            (*it)->setStatus(OrderStatus::DELIVERING);

                            wareHouse.addtoInProc((*it));
                            it = wareHouse.getpenOrd().erase(it);
                            added = true;
                            break;
                        }
                    }
                }
            }
            if (added == false)
            {
                it++;
            }
        }

        for (Volunteer *it2 : (wareHouse.getVol()))
        {
            it2->step();
        }
        for (auto it = wareHouse.getprocOrd().begin(); it != wareHouse.getprocOrd().end();)
        {
            bool added = false;
            if ((*it)->getStatus() == OrderStatus ::COLLECTING)
            {
                if (wareHouse.getVolunteer((*it)->getCollectorId()).getCompletedOrderId() == (*it)->getId())
                {
                    wareHouse.addtoPen((*it));
                    it = wareHouse.getprocOrd().erase(it);
                    added = true;
                }
                else if ((*it)->getStatus() == OrderStatus::DELIVERING)
                {
                    if (wareHouse.getVolunteer((*it)->getDriverId()).getCompletedOrderId() == (*it)->getId())
                    {
                        (*it)->setStatus(OrderStatus::COMPLETED);
                        wareHouse.addtoComp((*it));
                        it = wareHouse.getprocOrd().erase(it);
                        added = true;
                    }
                }
            }
            int k = 0;
            for (Volunteer *v : wareHouse.getVol())
            {
                
                if (!v->hasOrdersLeft() && !v->isBusy())
                {
                    delete v;
                    v = nullptr;
                    wareHouse.getVol().erase(wareHouse.getVol().begin() + k);
                }
                k++;

            }
            if (added == false)
            {
                it++;
            }
        }
    }

    complete();
}

std::string SimulateStep::toString() const
{
    return "step " + to_string(numOfSteps);
}

SimulateStep *SimulateStep::clone() const
{
    return new SimulateStep(*this);
}

AddOrder::AddOrder(int id) : BaseAction(), customerId(id) {}

void AddOrder::act(WareHouse &wareHouse)
{
    if (wareHouse.cusexist(customerId))
    {
        if (wareHouse.getCustomer(customerId).canMakeOrder())
        {
            Order *ord = new Order(wareHouse.setOrderId(), customerId, wareHouse.getCustomer(customerId).getCustomerDistance());
            wareHouse.getCustomer(customerId).addOrder(ord->getId());
            wareHouse.addOrder(ord);
            complete();
        }
        else
        {
            error("Cannot place this order");
        }
    }
    else
    {
        error("Cannot place this order");
    }
}

string AddOrder::toString() const
{
    return "order " + to_string(customerId);
}

AddOrder *AddOrder::clone() const
{
    return new AddOrder(*this);
}

PrintOrderStatus ::PrintOrderStatus(int id) : BaseAction(), orderId(id) {}

void PrintOrderStatus::act(WareHouse &wareHouse)
{
    Order &order = wareHouse.getOrder(orderId);
    if (wareHouse.orderexist(orderId))
    {
        cout << order.toString() << endl;
        complete();
    }
    else
    {
        error("Order doesnt exist");
    }
}
PrintOrderStatus *PrintOrderStatus::clone() const
{
    return new PrintOrderStatus(*this);
}

string PrintOrderStatus::toString() const
{
    string output;
    output = "orderStatus " + to_string(orderId);
    if (getStatus() == ActionStatus::COMPLETED)
    {
        return output + "COMPLETED";
    }
    else
        return output + "ERROR";
}

PrintActionsLog::PrintActionsLog() : BaseAction() {}

void PrintActionsLog::act(WareHouse &wareHouse)
{
    for (BaseAction *action : wareHouse.getactLog())
    {
        if (action->getStatus() == ActionStatus ::COMPLETED)
        {
            cout << action->toString() << " Completed" << endl;
        }
        else
            cout << action->toString() << "Error" << endl;
    }
}

PrintActionsLog *PrintActionsLog::clone() const
{
    return new PrintActionsLog(*this);
}

string PrintActionsLog::toString() const
{
    return "log";
}

BackupWareHouse::BackupWareHouse() : BaseAction() {}

void BackupWareHouse::act(WareHouse &wareHouse)
{
    if (backup != nullptr)
    {
        delete backup;
        backup = nullptr;
    }
    backup = new WareHouse(wareHouse);
    complete();
}

BackupWareHouse *BackupWareHouse::clone() const
{
    return new BackupWareHouse(*this);
}

string BackupWareHouse::toString() const
{
    return "backup";
}

PrintCustomerStatus::PrintCustomerStatus(int customerId) : BaseAction(), customerId(customerId) {}

void PrintCustomerStatus::act(WareHouse &wareHouse)
{
    if (wareHouse.cusexist(customerId))
    {
        Customer &Customer = wareHouse.getCustomer(customerId);
        cout << "CustomerId: " << to_string(customerId) << endl;
        for (int orderId : Customer.getOrdersIds())
        {
            cout << "OrderId: " << to_string(orderId) << endl;
            cout << "OrderStatus: " << wareHouse.getOrder(orderId).statustostring(wareHouse.getOrder(orderId).getStatus()) << endl;
        }
        cout << "Orders left: " << to_string(Customer.getMaxOrders() - Customer.getNumOrders()) << endl;
        complete();
    }
    else
    {
        error("Customer doesnt exist");
    }
}

PrintCustomerStatus *PrintCustomerStatus::clone() const
{
    return new PrintCustomerStatus(*this);
}

string PrintCustomerStatus::toString() const
{
    return "CustomerId: " + to_string(customerId);
}

RestoreWareHouse::RestoreWareHouse() : BaseAction() {}

void RestoreWareHouse::act(WareHouse &wareHouse)
{
    if (backup != nullptr)
        wareHouse =(*backup) ;
    else
    {
        std::cout << "backup is not avilable " << endl;
    }
    complete();
}

RestoreWareHouse *RestoreWareHouse::clone() const
{
    return new RestoreWareHouse(*this);
}

string RestoreWareHouse::toString() const
{
    return "RestoreWarehouse ";
}

BaseAction::BaseAction() : errorMsg(), status() {}

void BaseAction::error(string errorMsg)
{
    status = ActionStatus::ERROR;
    cout << errorMsg << endl;
}

string BaseAction::getErrorMsg() const
{
    return errorMsg;
}

ActionStatus BaseAction::getStatus() const
{
    return ActionStatus();
}

void BaseAction::complete()
{
    status = ActionStatus ::COMPLETED;
}

AddCustomer::AddCustomer(string customerName, string customerType, int distance, int maxOrders) : BaseAction(), customerName(customerName), customerType(customerType == "soldier" ? CustomerType::Soldier : CustomerType::Civilian), distance(distance), maxOrders(maxOrders) {}

void AddCustomer::act(WareHouse &wareHouse)
{
    if (customerType == CustomerType ::Civilian)
    {
        wareHouse.addtoCus(new CivilianCustomer(wareHouse.setCustomer(), customerName, distance, maxOrders));
    }
    else
    {
        wareHouse.addtoCus(new SoldierCustomer(wareHouse.setCustomer(), customerName, distance, maxOrders));
    }
    complete();
}

AddCustomer *AddCustomer::clone() const
{
    return new AddCustomer(*this);
}

string AddCustomer::toString() const
{
    string output;
    output + customerName;
    if (customerType == CustomerType ::Civilian)
    {
        output += "Civilian ";
    }
    else
    {
        output += "Soldier ";
    }
    output + to_string(distance);
    output + to_string(maxOrders);
    return output;
}

PrintVolunteerStatus::PrintVolunteerStatus(int id) : BaseAction(), VolunteerId(id) {}

void PrintVolunteerStatus::act(WareHouse &wareHouse)
{
    if (wareHouse.volexist(VolunteerId))
    {
        Volunteer &vol = wareHouse.getVolunteer(VolunteerId);
        cout << vol.toString() << endl;
        complete();
    }
    else
        error("Volunteer doesnt exist");
}

PrintVolunteerStatus *PrintVolunteerStatus::clone() const
{
    return new PrintVolunteerStatus(*this);
}

string PrintVolunteerStatus::toString() const
{
    return string();
}

Close::Close() : BaseAction() {}

void Close::act(WareHouse &wareHouse)
{
    for (Order *ord : wareHouse.getpenOrd())
    {
        cout << "OrderId: " + to_string(ord->getId()) << "CustomerId: " + to_string(ord->getCustomerId()) << "OrderStatus: " + ord->statustostring(ord->getStatus()) << endl;
    }
    for (Order *ord : wareHouse.getprocOrd())
    {
        cout << "OrderId: " + to_string(ord->getId()) << "CustomerId: " + to_string(ord->getCustomerId()) << "OrderStatus: " + ord->statustostring(ord->getStatus()) << endl;
    }
    for (Order *ord : wareHouse.compOrd())
    {
        cout << "OrderId: " + to_string(ord->getId()) << "CustomerId: " + to_string(ord->getCustomerId()) << "OrderStatus: " + ord->statustostring(ord->getStatus()) << endl;
    }
    complete();
    wareHouse.close();
}

Close *Close::clone() const
{
    return new Close(*this);
}

string Close::toString() const
{
    return "close";
}
