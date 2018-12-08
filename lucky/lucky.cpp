#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>


using namespace eosio;

class [[eosio::contract]] lucky : eosio::contract {

public:
	using contract::contract;
	lucky(name r,name c, datastream<const char*> ds):contract(r,c,ds){}

	[[eosio::action]]
	void play(name player, const asset& quantity) {
		require_auth(player);
		if(now() % 2 == 1){
			action(
			permission_level{get_self(),"active"_n},  //所需要的权限结构
			"eosio.token"_n,						  // 调用的合约名称
			"transfer"_n,							  // 合约的方法
			std::make_tuple(get_self(),player, quantity, std::string("shao kun game")) // 传递的参数
			).send();
		} else {
			action(
			permission_level{player,"active"_n},
			"eosio.token"_n,
			"transfer"_n,
			std::make_tuple(player,get_self(), quantity, std::string("shao kun game"))
			).send();
		}
	};
};
EOSIO_DISPATCH(lucky,(play))