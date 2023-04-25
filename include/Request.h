#include "User.h"

class Request
{
private:
    int id;
    User user;

public:
    Request(int id_request, User user_change,double b)
    {
        this->id = id_request;
    }
};
