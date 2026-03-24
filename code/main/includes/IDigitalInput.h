#ifndef IDIGITALINPUT_H
#define IDIGITALINPUT_H

class IDigitalInput {
    public:
        // pure virtual function that must be implemented by any class that inherits
        virtual bool read() = 0;
};


#endif
