class IDigitalInput {
    public:
        // pure virtual function that must be implemented by any class that inherits
        virtual bool read() = 0;
};
