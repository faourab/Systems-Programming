#include "Order.h"
using std::string;
using std::vector;
using namespace std;
Order::Order(int id, int customerId, int distance):
            id(id), customerId(customerId),distance(distance),status(OrderStatus::PENDING),collectorId(NO_VOLUNTEER),driverId(NO_VOLUNTEER)
        {}

        int Order::getId() const
        {
            return id;
        }

        int Order :: getCustomerId() const{
            return customerId;
        }
        void Order :: setStatus(OrderStatus status){
            this->status = status;
        }
        void Order :: setCollectorId(int collectorId){
            this->collectorId = collectorId;
        }
        void Order :: setDriverId(int driverId){
            this->driverId = driverId;
        }
        int Order :: getCollectorId() const{
            return collectorId;
        }
        int Order :: getDriverId() const{
            return driverId;
        }
        int Order::getDistance() const
        {
            return distance;
        }
        OrderStatus Order ::getStatus() const
        {
            return status;
        }

        const string Order::toString() const
        {
            string output;

            output += "orderid : " + to_string(getId()) ;
            output += "\n";
            output += "customer : " + to_string(getCustomerId()) += "\n";
            output += "orderstatus : ";
            if (getStatus()==OrderStatus::PENDING)
            output += "Pending\n";
            else if (getStatus()==OrderStatus::COLLECTING)
            output += "Collecting\n";
            else if (getStatus()==OrderStatus::DELIVERING)
            output += "Delivering\n";
            else if (getStatus()==OrderStatus::COMPLETED)
            output += "Completed\n";
            output += "Collectorid: ";
            if (getStatus()==OrderStatus::PENDING)
            output += "None\n";
            else
            output += std::to_string(collectorId); 
            output +="\n";
            output += "Driverid: ";
            if (getStatus()==OrderStatus::PENDING||getStatus()== OrderStatus::COLLECTING)
            output += "None\n";
            else
            output +=  std::to_string(driverId) ;
            return output;           
        }

        string Order::statustostring(OrderStatus orderStatus)
        { 
            if (orderStatus == OrderStatus::PENDING)
            return "Pending";
            else if (orderStatus == OrderStatus::COLLECTING)
            return "Collecting";
            else if (orderStatus == OrderStatus::DELIVERING)
            return "Delivering";
            else if (orderStatus == OrderStatus::COMPLETED)
            return  "Completed"; 
            return "";
        }

        Order *Order::clone()
        {
            return new Order(*this);
        }
