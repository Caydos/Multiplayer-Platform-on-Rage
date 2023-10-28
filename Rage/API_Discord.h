#ifndef API_DISCORD_H
#define API_DISCORD_H


struct User
{
    const char* name;
    const char* discriminator;
    std::int64_t id;
};

namespace API_Discord
{
	void Check(void);
    User GetUser(void);
}








#endif // !API_DISCORD_H
