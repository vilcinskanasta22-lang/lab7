
#include "car_rental.h"
#include <iomanip>
#include <sstream>

Car::Car(int id, const std::string& brand, const std::string& model,
    const std::string& plate, double baseRate)
    : id_(id), brand_(brand), model_(model), plate_(plate),
    baseRate_(baseRate), available_(true) {
}

std::string Car::describe() const {
    std::ostringstream os;
    os << "[" << id_ << "] " << category() << " | " << brand_ << " " << model_
        << " | " << plate_ << " | " << std::fixed << std::setprecision(2)
        << baseRate_ << " UAH/day";
    if (!damages_.empty()) os << " | damages: " << damages_.size();
    return os.str();
}

double EconomyCar::calculateRent(int days) const {
    double sum = baseRate_ * days;
    if (days > 7) sum *= 0.9;
    return sum;
}

double BusinessCar::calculateRent(int days) const {
    return (baseRate_ + 15.0) * days;
}

double SUVCar::calculateRent(int days) const {
    return baseRate_ * days + 20.0;
}

std::string statusToString(OrderStatus s) {
    switch (s) {
    case OrderStatus::Pending:  return "Pending review";
    case OrderStatus::Rejected: return "Rejected";
    case OrderStatus::Approved: return "Approved (awaiting payment)";
    case OrderStatus::Paid:     return "Paid";
    case OrderStatus::Closed:   return "Closed";
    }
    return "?";
}

void Car::changeRate(double delta) {
    baseRate_ += delta;
    if (baseRate_ < 0) baseRate_ = 0;
}

EconomyCar& EconomyCar::operator++() {
    changeRate(kRateStep);
    return *this;
}

EconomyCar EconomyCar::operator++(int) {
    EconomyCar old(*this);
    ++(*this);
    return old;
}

EconomyCar& EconomyCar::operator--() {
    changeRate(-kRateStep);
    return *this;
}

EconomyCar EconomyCar::operator--(int) {
    EconomyCar old(*this);
    --(*this);
    return old;
}

BusinessCar& BusinessCar::operator++() {
    changeRate(kRateStep);
    return *this;
}

BusinessCar BusinessCar::operator++(int) {
    BusinessCar old(*this);
    ++(*this);
    return old;
}

BusinessCar& BusinessCar::operator--() {
    changeRate(-kRateStep);
    return *this;
}

BusinessCar BusinessCar::operator--(int) {
    BusinessCar old(*this);
    --(*this);
    return old;
}

SUVCar& SUVCar::operator++() {
    changeRate(kRateStep);
    return *this;
}

SUVCar SUVCar::operator++(int) {
    SUVCar old(*this);
    ++(*this);
    return old;
}

SUVCar& SUVCar::operator--() {
    changeRate(-kRateStep);
    return *this;
}

SUVCar SUVCar::operator--(int) {
    SUVCar old(*this);
    --(*this);
    return old;
}

Order& Order::operator++() {
    if (status == OrderStatus::Pending) {
        ++days;
        rentCost = car->calculateRent(days);
    }
    return *this;
}

Order Order::operator++(int) {
    Order old(*this);
    ++(*this);
    return old;
}

Order& Order::operator--() {
    if (status == OrderStatus::Pending && days > 1) {
        --days;
        rentCost = car->calculateRent(days);
    }
    return *this;
}

Order Order::operator--(int) {
    Order old(*this);
    --(*this);
    return old;
}

void RentalSystem::addCar(std::unique_ptr<Car> car) {
    cars_.push_back(std::move(car));
}

Car* RentalSystem::findCar(int id) {
    for (auto& c : cars_) if (c->id() == id) return c.get();
    return nullptr;
}

Order* RentalSystem::findOrder(int id) {
    for (auto& o : orders_) if (o.id == id) return &o;
    return nullptr;
}

int RentalSystem::createOrder(int carId, const std::string& name,
    const std::string& passport, int days, std::string& err) {
    Car* car = findCar(carId);
    if (!car) { err = "Car not found."; return -1; }
    if (!car->isAvailable()) { err = "Car is not available."; return -1; }
    if (days <= 0) { err = "Rental period must be positive."; return -1; }
    if (name.empty() || passport.empty()) { err = "Fill in the name and passport data."; return -1; }

    Order o;
    o.id = nextOrderId_++;
    o.car = car;
    o.clientName = name;
    o.passport = passport;
    o.days = days;
    o.rentCost = car->calculateRent(days); 
    car->setAvailable(false);             
    orders_.push_back(o);
    return o.id;
}

bool RentalSystem::approve(int id, std::string& err) {
    Order* o = findOrder(id);
    if (!o) { err = "Order not found."; return false; }
    if (o->status != OrderStatus::Pending) { err = "Request was already processed."; return false; }
    o->status = OrderStatus::Approved;
    return true;
}

bool RentalSystem::reject(int id, const std::string& reason, std::string& err) {
    Order* o = findOrder(id);
    if (!o) { err = "Order not found."; return false; }
    if (o->status != OrderStatus::Pending) { err = "Request was already processed."; return false; }
    o->status = OrderStatus::Rejected;
    o->rejectReason = reason;
    o->car->setAvailable(true); 
    return true;
}

bool RentalSystem::pay(int id, std::string& err) {
    Order* o = findOrder(id);
    if (!o) { err = "Order not found."; return false; }
    if (o->status != OrderStatus::Approved) { err = "Payment is only possible for an approved request."; return false; }
    o->status = OrderStatus::Paid;
    return true;
}

bool RentalSystem::returnCar(int id, const std::string& note, double fee, std::string& err) {
    Order* o = findOrder(id);
    if (!o) { err = "Order not found."; return false; }
    if (o->status != OrderStatus::Paid) { err = "Return is only possible for a paid order."; return false; }
    if (!note.empty()) {
        o->car->addDamage(note);
        o->damageCost = fee;
    }
    o->status = OrderStatus::Closed;
    o->car->setAvailable(true);
    return true;
}
