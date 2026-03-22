class IActuatorDrive {
    public:
        // pure virtual functions that must be implemented by any class that inherits
        virtual void extend() = 0;
        virtual void retract() = 0;
        virtual void stop() = 0;
};