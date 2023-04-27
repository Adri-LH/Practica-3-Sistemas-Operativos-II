#include "Member.h"

class UserPremium{
    public:
    UserPremium(int id_user) {
            id=id_user;
            
    }

    private:

    int id;

};

class UserPremiumLimited : public UserPremium
{
public:
    UserPremiumLimited(int user_balance) {
            balance=user_balance;
            
    }
    
private:

int balance;


};

class UserFree : public UserPremium
{
public:
    
private:
    int balance=0;



};
