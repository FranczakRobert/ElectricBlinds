#include "MotorL298NDriver.hpp"
#include "DriverManager.hpp"

MotorL298NDriver::MotorL298NDriver(DriverManager *driverManager) {
    this->driverManager = driverManager;
}

MotorL298NDriver::~MotorL298NDriver()
{
}

ErrorCode MotorL298NDriver::init()
{
    return ErrorCode();
}

ErrorCode MotorL298NDriver::deinit()
{
    return ErrorCode();
}

ErrorCode MotorL298NDriver::start()
{
    return ErrorCode();
}

ErrorCode MotorL298NDriver::stop()
{
    return ErrorCode();
}
