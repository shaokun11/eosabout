#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

using namespace eosio;

class [[eosio::contract]] addressbook : public eosio::contract {
public:
    using contract::contract;
    addressbook(name reciever, name code, datastream<const char*> ds) : contract(reciever,code,ds){}

    [[eosio::action]]
    void upsert(name user, std::string first_name, uint64_t age){
        require_auth(user);
        address_index addresses(_code,_code.value);
        auto itr = addresses.find(user.value);
        if(itr == addresses.end()){
            addresses.emplace(user,[&](auto& row){
                row.key = user;
                row.first_name = first_name;
                row.age = age;
            });
            send_summary(user, "emplace data");
        } else {
            addresses.modify(itr, user, [&](auto& row){
                        row.first_name = first_name;
                        row.age = age;
            });
            send_summary(user, "modify data");
        }
    }

    [[eosio::action]]
    void erase(name user){
        require_auth(user);
        address_index addresses(_self, _code.value);
        auto itr = addresses.find(user.value);
        eosio_assert(itr != addresses.end(),"record not exist");
        addresses.erase(itr);
        send_summary(user, "erase data");
    }

    [[eosio::action]]
    void notify(name user, std::string){
        require_auth(get_self());
        require_recipient(user);
    }

private:
    struct [[eosio::table]] person {
        name key;
        std::string first_name;
        uint64_t age;

        auto primary_key() const {
            return key.value;
        };
    };
    typedef multi_index<"person"_n, person> address_index;
 
    void send_summary(name user, std::string msg){
        action(
            permission_level{get_self(),"active"_n},
                get_self(),
                "notify"_n,
                std::make_tuple(user, name{user}.to_string() +msg)
            ).send();
    }
};
EOSIO_DISPATCH(addressbook,(upsert)(erase))
