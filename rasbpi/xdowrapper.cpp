#include "xdowrapper.h"

XdoWrapper::XdoWrapper():wrapped(nullptr)
{
    wrapped=xdo_new(nullptr);

}
