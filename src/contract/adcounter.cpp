#include <eosiolib/eosio.hpp>

using namespace eosio;

class [[eosio::contract]] adcounter: public eosio::contract {
public:
	using contract::contract;
	adcounter(name receiver, name code, datastream<const char*> ds):contract(receiver,code,ds),
																	counts(receiver,code.value){}

	void count(name user, std::string type) {
		// 设定这个方法的调用权限
		// 只有 shaokun11111 这个账号部署的合约有这个权限更改这个合约的内容
		require_auth(name("shaokun11111"));
		auto itr = counts.find(user.value);
		if(itr == counts.end()){
			counts.emplace("shaokun11111"_n, [&](auto& row){
				row.key = user;
				row.emplaced = type == "emplace" ? 1 : 0;
				row.modified = type == "modify" ? 1 : 0;
				row.erased = type == "erase" ? 1 : 0;
			});
		} else {
			counts.modify(itr,"shaokun11111"_n,[&](auto& row){
				if (type == "emplace")
				{
					row.emplaced += 1;
				}
				if (type == "modify")
				{
					row.modified += 1;
				}
				if (type == "erase")
				{
					row.erased += 1;
				}
			});
		}
	}
private:
	struct [[eosio::table]] counter
	{
		name key;
		uint64_t emplaced;
		uint64_t modified;
		uint64_t erased;

		uint64_t primary_key() const {return key.value;}
	};

	typedef eosio::multi_index<"counter"_n,counter> counter_index;
	counter_index counts;
};
EOSIO_DISPATCH(adcounter,(count));