#pragma once

#include <string>
#include <vector>
#include "Action.h"
using std::string;
using std::vector;


class Volunteer;

enum class OrderStatus {
    PENDING,
    COLLECTING,
    DELIVERING,
    COMPLETED,
};

#define NO_VOLUNTEER -1

class Order {

    public:
        Order(int id, int customerId, int distance);
        int getId() const;
        int getCustomerId() const;
        void setStatus(OrderStatus status);
        void setDriverId(int driverId);
        void setCollectorId(int collectorId);
        int getCollectorId() const;
        int getDriverId() const;
        int getDistance ()const;
        OrderStatus getStatus() const;
        const string toString() const;
        string statustostring(OrderStatus orderStatus);
        Order* clone();
        


    private:
        const int id;
        const int customerId;
        const int distance;
        OrderStatus status;
        int collectorId; //Initialized to NO_VOLUNTEER if no collector has been assigned yet
        int driverId; //Initialized to NO_VOLUNTEER if no driver has been assigned yet
       
};