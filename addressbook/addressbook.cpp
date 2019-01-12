#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

using namespace eosio;

class [[eosio::contract]] addressbook : public eosio::contract {

public:
  using contract::contract;
  
  addressbook(name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds) {}

  [[eosio::action]]
  void upsert(name user, std::string first_name, std::string last_name) {
    require_auth(user);
    address_index addresses(_code, _code.value);
    auto iterator = addresses.find(user.value);
    if( iterator == addresses.end() )
    {
      addresses.emplace(user, [&]( auto& row ) {
       row.key = user;
       row.first_name = first_name;
       row.last_name = last_name;
      });
      send_summary(user, " successfully emplaced record to addressbook");
    }
    else {
      std::string changes;
      addresses.modify(iterator, user, [&]( auto& row ) {
        row.key = user;
        row.first_name = first_name;
        row.last_name = last_name;
      });
      send_summary(user, " successfully modified record to addressbook");
    }
  }

  [[eosio::action]]
  void erase(name user) {
    require_auth(user);

    address_index addresses(_self, _code.value);

    auto iterator = addresses.find(user.value);
    eosio_assert(iterator != addresses.end(), "Record does not exist");
    addresses.erase(iterator);
    send_summary(user, " successfully erased record from addressbook");
  }

  [[eosio::action]]
  void notify(name user, std::string msg) {
    require_auth(get_self());
    require_recipient(user);
  }

  void transfer(uint64_t receiver, uint64_t code){
    send_summary(name(code), "eosio.token transfer");
  }

private:
  struct [[eosio::table]] person {
    name key;
    std::string first_name;
    std::string last_name;
    uint64_t primary_key() const { return key.value; }
  };

  void send_summary(name user, std::string message) {
    action(
      permission_level{get_self(),"active"_n},
      get_self(),
      "notify"_n,
      std::make_tuple(user, name{user}.to_string() + message)
    ).send();
  };

  typedef eosio::multi_index<"people"_n, person> address_index;
  
};

extern "C" {
  void apply(uint64_t receiver, uint64_t code, uint64_t action) {
    addressbook _addressbook(name(receiver),name(code),datastream<const char*>(nullptr,0));
    if(code==receiver && action==name("upsert").value) {
      execute_action(name(receiver), name(code), &addressbook::upsert );
    }
    else if(code==receiver && action==name("notify").value) {
      execute_action(name(receiver), name(code), &addressbook::notify );
    }
    else if(code==receiver && action==name("erase").value) {
      execute_action(name(receiver), name(code), &addressbook::erase );
    }
    else if(code==name("eosio.token").value && action==name("transfer").value) {
      execute_action(name(receiver), name(code), &addressbook::transfer );
    }
  }
};