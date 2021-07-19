#include <iostream>

#include <Client.h>

int main()
{
    silence::Client client("http://localhost:3000/");
    client.connect();

    return 0;
}