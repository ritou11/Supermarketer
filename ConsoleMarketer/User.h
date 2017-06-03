#pragma once
enum USER_TYPE
{
    CONSUMER,
    ADMIN
};

class User
{
public:
    USER_TYPE type;
    string username;
    User() {
        username = "";
        type = CONSUMER;
    }
};

