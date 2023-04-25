#include "Member.h"

class User{
    public:
    User(int id_user,Membership member,int b) {
            id=id_user;
            switch (member)
            {
            case FREE:
                membership=FREE;
                balance=0;
                break;
            case PREMIUM_LIMITED:
                membership=PREMIUM_LIMITED;
                balance=b;
                break;
            case PREMIUM_UNLIMITED:
                membership=PREMIUM_UNLIMITED;
                break;
            default:
                break;
            }
        }
    
    private:

    int id;
    Membership membership;
    double balance;

};