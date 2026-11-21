
#pragma once
#include <memory>
#include <string>
#include <vector>


class Car {
protected:
    int id_;
    std::string brand_, model_, plate_;
    double baseRate_;                
    bool available_;
    std::vector<std::string> damages_; 
    void changeRate(double delta);

public:
    static constexpr double kRateStep = 10.0; 

    Car(int id, const std::string& brand, const std::string& model,
        const std::string& plate, double baseRate);
    Car(const Car&) = default;
    virtual ~Car() = default;
    virtual double calculateRent(int days) const = 0; 
    virtual std::string category() const = 0;        
    virtual std::string describe() const;           

    int id() const { return id_; }
    bool isAvailable() const { return available_; }
    void setAvailable(bool v) { available_ = v; }
    void addDamage(const std::string& note) { damages_.push_back(note); }
    const std::vector<std::string>& damages() const { return damages_; }
    std::string name() const { return brand_ + " " + model_; }
    double baseRate() const { return baseRate_; }
};

class EconomyCar : public Car {
public:
    using Car::Car;
    double calculateRent(int days) const override;
    std::string category() const override { return "Economy"; }
    EconomyCar& operator++();      
    EconomyCar operator++(int);    
    EconomyCar& operator--();      
    EconomyCar operator--(int);   
};

class BusinessCar : public Car {
public:
    using Car::Car;
    double calculateRent(int days) const override;
    std::string category() const override { return "Business"; }
    BusinessCar& operator++();     
    BusinessCar operator++(int);   
    BusinessCar& operator--();    
    BusinessCar operator--(int);    
};

class SUVCar : public Car {
public:
    using Car::Car;
    double calculateRent(int days) const override;
    std::string category() const override { return "SUV"; }
    SUVCar& operator++();      
    SUVCar operator++(int);   
    SUVCar& operator--();     
    SUVCar operator--(int);   
};

enum class OrderStatus { Pending, Rejected, Approved, Paid, Closed };
std::string statusToString(OrderStatus s);

struct Order {
    int id;
    Car* car;              
    std::string clientName;
    std::string passport;
    int days;
    OrderStatus status = OrderStatus::Pending;
    std::string rejectReason;
    double rentCost = 0;
    double damageCost = 0;
    double total() const { return rentCost + damageCost; }
    Order& operator++();     
    Order operator++(int);   
    Order& operator--();     
    Order operator--(int);  
};

class RentalSystem {
    std::vector<std::unique_ptr<Car>> cars_;
    std::vector<Order> orders_;
    int nextOrderId_ = 1;
public:
    void addCar(std::unique_ptr<Car> car);
    const std::vector<std::unique_ptr<Car>>& cars() const { return cars_; }
    const std::vector<Order>& orders() const { return orders_; }

    Car* findCar(int id);
    Order* findOrder(int id);

    
    int createOrder(int carId, const std::string& name, const std::string& passport,
        int days, std::string& err);
    bool pay(int orderId, std::string& err);

   
    bool approve(int orderId, std::string& err);
    bool reject(int orderId, const std::string& reason, std::string& err);
    bool returnCar(int orderId, const std::string& damageNote, double damageFee,
        std::string& err);
};
