#ifndef API_DISCORD_H
#define API_DISCORD_H


struct User
{
    const char* _name;
    const char* _discriminator;
    std::int64_t _id;
};

namespace API_Discord
{
	void Check(void);
    User GetUser(void);
}








#endif // !API_DISCORD_H
