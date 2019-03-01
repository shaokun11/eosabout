#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
using namespace eosio;

class [[eosio::contract]] paymsg : public eosio::contract {

public:
  using contract::contract;
  // 构造函数
  paymsg(name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds){}

  [[eosio::action]]
  void upmsg(name player, std::string msg) {
    require_auth(player);
    // 这个地方可以使用这种方式初始化table 真不知道是哪里的问题,醉了....
    paymessage_index paymessages(get_self(), _code.value);
    auto iterator = paymessages.find(player.value);
    eosio_assert(iterator != paymessages.end(), "you are not in shaokun paymsg game");
    eosio_assert(iterator->uamount >= 50000, "you must send more than 5 eos that can update your msg");
    paymessages.modify(iterator, player, [&]( auto& row ) {
        row.player = player;
        row.msg = msg;
        row.uamount = 0;  //置为0 ,下次更改必须花5eos以上
      });
  }

  void transfer(){
    // 反序列化得到st_transfer结构体对象
    // 就可以拿到transfer的交易信息了,而且是eosio.token发过的transfer
    auto transfer_data = unpack_action_data<st_transfer>();
    // eos是4位小数,由于不支持实际意义上的小数,所以这里要使用整形10000代替1eos
    eosio_assert(transfer_data.quantity.amount >= 10000, "must more than 1 eos provide");
    auto tr_from = transfer_data.from;
    if(tr_from == get_self()) {
        //自己转出去就不需要参加游戏了
        return;
    }
    auto tr_amount = transfer_data.quantity.amount;
    // https://github.com/EOSIO/eosio.cdt/blob/master/libraries/eosiolib/symbol.hpp
    if(transfer_data.quantity.symbol != eosio::symbol("EOS",4)){
        // 我们只收eos
        return;
    }
    auto tr_memo = transfer_data.memo;
    // 这里一定要按照这样的格式初始化table,不然无法写入数据
    // 理论上这种方式初始化的table权限是一样的呢 paymessage_index paymessages(get_self(), _code.value);
    paymessage_index paymessages(get_self(), get_self().value);
    auto itr = paymessages.find(transfer_data.from.value);
    if (itr == paymessages.end()){
        paymessages.emplace(get_self(), [&]( auto& row ) {
            row.player = tr_from;
            row.uamount = tr_amount;
            row.tamount = tr_amount;
            row.msg = "welcome to shaokun paygame, your pay msg is :" + tr_memo;
        });
    }else {
        paymessages.modify(itr, get_self(), [&]( auto& row ) {
            row.tamount += tr_amount;
            row.uamount += tr_amount ;
            row.msg = "welcome come back to shaokun paygame:"+ tr_memo;
        });
    }
  }

private:
 //只为反序列化使用,不用加table
  struct st_transfer {
      name    from;
      name    to;
      asset   quantity;
      std::string  memo;
  };

  struct [[eosio::table]] paymessage {
    name player;
    uint64_t tamount;  
    uint64_t uamount;  
    std::string msg;  
    uint64_t primary_key() const { return player.value; }
  };
  typedef eosio::multi_index<"paymessage"_n, paymessage> paymessage_index;
};

#define EOSIO_DISPATCH_CUSTOM( TYPE, MEMBERS ) \
extern "C" { \
   void apply( uint64_t receiver, uint64_t code, uint64_t action ) { \
      if( code == receiver || code == "eosio.token"_n.value && action == "transfer"_n.value) { \
         switch( action ) { \
            EOSIO_DISPATCH_HELPER( TYPE, MEMBERS ) \
         } \
         /* does not allow destructor of thiscontract to run: eosio_exit(0); */ \
      } \
   } \
} \

EOSIO_DISPATCH_CUSTOM( paymsg, (upmsg)(transfer) )