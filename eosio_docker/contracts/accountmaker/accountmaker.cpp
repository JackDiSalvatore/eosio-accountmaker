#include <algorithm>
#include <cmath>
// #include <boost/optional.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/public_key.hpp>
#include <eosiolib/crypto.h>

#include "exchange_state.hpp"
#include "exchange_state.cpp"
#include "eosio.token.hpp"
#include "eosio.system.hpp"
#include "native.hpp"

#include "abieos_numeric.hpp"


#define EOS_SYMBOL symbol("EOS", 4)
#define RAMCORE_SYMBOL symbol("RAMCORE", 4)
#define RAM_SYMBOL symbol("RAM", 0)

namespace eosio {
  // Temporary authority until native is fixed. Ref: https://github.com/EOSIO/eos/issues/4669
   // struct wait_weight {
   //    uint32_t           wait_sec;
   //    uint16_t           weight;
   //    // explicit serialization macro is not necessary, used here only to improve compilation time
   //    EOSLIB_SERIALIZE( wait_weight, (wait_sec)(weight) )
   // };

   struct authority {
      uint32_t                              threshold;
      std::vector<eosiosystem::key_weight>               keys;
      std::vector<eosiosystem::permission_level_weight>  accounts;
      std::vector<eosiosystem::wait_weight>              waits;
      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE( authority, (threshold)(keys)(accounts)(waits) )
   };

  // eosiosystem::native::newaccount Doesn't seem to want to take authorities.
  // struct call {
  //   struct eosio {
  //     void newaccount(name creator, name name,
  //                     authority owner, authority active);
  //   };
  // };

  asset buyrambytes(uint32_t bytes) {
    eosiosystem::rammarket _rammarket( "eosio"_n, "eosio"_n.value );
    auto itr = _rammarket.find( RAMCORE_SYMBOL.raw() );
    auto tmp = *itr;
    return tmp.convert(asset(bytes, RAM_SYMBOL), EOS_SYMBOL);
  }
}  /// namspace eosio

using namespace eosio;

CONTRACT accountmaker : public eosio::contract {
  private:

    /****************************************************************************
     *                            F U N C T I O N S
     ***************************************************************************/

    time_point current_time_point() {
      const static time_point ct{ microseconds{ static_cast<int64_t>( current_time() ) } };
      return ct;
    }

    /****************************************************************************
     *                                T A B L E S
     ***************************************************************************/

  public:
    using contract::contract;

    // constructor
    accountmaker( name receiver, name code, datastream<const char*> ds ):
                  contract( receiver, code, ds ) {}

    /****************************************************************************
     *                              A C T I O N S
     ***************************************************************************/

    ACTION transfer( name         from,
                     name         to,
                     asset        quantity,
                     std::string  memo ) {
      std::array<char, 33> owner_pubkey_char;
      std::array<char, 33> active_pubkey_char;

      if ( from == _self || to != _self ) {
        // this is an outgoing transfer, do nothing
        return;
      }

      print(name(from), " is sending ", name(to), " ", quantity, "\n");
      print("memo: ", memo, "\n");

      eosio_assert(quantity.symbol == EOS_SYMBOL, "Must be EOS");
      eosio_assert(quantity.is_valid(), "Invalid token transfer");
      eosio_assert(quantity.amount > 0, "Quantity must be positive");

      /* Parse Memo into onwer/active keys */
      eosio_assert(memo.length() == 120 || memo.length() == 66, "Malformed Memo (not right length)");
      eosio_assert(memo[12] == ':' || memo[12] == '-', "Malformed Memo [12] == : or -");
      eosio_assert(memo[66] == ':' || memo[66] == '-', "Malformed Memo [66] == : or -");

      print("name: " , name(memo.substr(0, 12)), "\n");
      const name new_account_name = name( memo.substr(0, 12) );

      const std::string owner_key_str = memo.substr(13, 53);
      std::string active_key_str;

      if(memo[66] == ':' || memo[66] == '-') {
        // active key provided
        active_key_str = memo.substr(67, 120);
      } else {
        // active key is the same as owner
        active_key_str =  owner_key_str;
      }

      print("active key: ", active_key_str, "\n");
      print("owner key: ", owner_key_str, "\n");

      const abieos::public_key owner_pubkey =
            abieos::string_to_public_key( owner_key_str );

      const abieos::public_key active_pubkey =
            abieos::string_to_public_key( active_key_str );

      std::copy( owner_pubkey.data.begin(),
                 owner_pubkey.data.end(),
                 owner_pubkey_char.begin() );

      std::copy( active_pubkey.data.begin(),
                 active_pubkey.data.end(),
                 active_pubkey_char.begin() );

      const auto owner_auth = authority{
          1, {{{(uint8_t)abieos::key_type::k1, owner_pubkey_char}, 1}}, {}, {}};
      const auto active_auth = authority{
          1, {{{(uint8_t)abieos::key_type::k1, active_pubkey_char}, 1}}, {}, {}};

      const auto amount = buyrambytes(14 * 1024);
      print("amount: ", amount, "\n");
      // const auto ram_replace_amount = buyrambytes(256);
      const auto cpu = asset( 1500, EOS_SYMBOL );
      const auto net = asset(  500, EOS_SYMBOL );

      const auto fee = asset(std::max((quantity.amount + 119) / 200, 1000ll), EOS_SYMBOL);
      print("fee: ", fee, "\n");
      eosio_assert( cpu + net + amount + fee /*+ ram_replace_amount*/ <= quantity,
                    "Not enough money" );

      const auto remaining_balance = quantity - cpu - net - amount - fee /* - ram_replace_amount*/;
      print("remaining_balance: ", remaining_balance, "\n");

      // create account
      action(
        permission_level{get_self(),"active"_n},
        "eosio"_n,
        "newaccount"_n,
        std::make_tuple( _self, new_account_name, owner_auth, active_auth )
      ).send();
      // INLINE_ACTION_SENDER(call::eosio, newaccount)
      // (N(eosio), {{_self, N(active)}},
      //  {_self, account_to_create, owner_auth, active_auth});

      // buy ram
      action(
        permission_level{get_self(),"active"_n},
        "eosio"_n,
        "buyrambytes"_n,
        std::make_tuple( _self, new_account_name, amount/*8192KiB*/ )
      ).send();
      // INLINE_ACTION_SENDER(eosiosystem::system_contract, buyram)
      // (N(eosio), {{_self, N(active)}}, {_self, account_to_create, amount});

      // replace lost ram
      // INLINE_ACTION_SENDER(eosiosystem::system_contract, buyram)
      // (N(eosio), {{_self, N(active)}}, {_self, _self, ram_replace_amount});

      // // delegate and transfer cpu and net
      action(
        permission_level{get_self(),"active"_n},
        "eosio"_n,
        "delegatebw"_n,
        std::make_tuple( _self, new_account_name, net, cpu, 1/*transfer on*/ )
      ).send();
      // INLINE_ACTION_SENDER(eosiosystem::system_contract, delegatebw)
      // (N(eosio), {{_self, N(active)}}, {_self, account_to_create, net, cpu, 1});

      // fee
      // TODO: ADD FEE ACTION HERE
      action(
        permission_level{get_self(),"active"_n},
        "eosio.token"_n,
        "transfer"_n,
        std::make_tuple( _self, name("chestnutfees"), fee, std::string("account creation fee") )
      ).send();
      // INLINE_ACTION_SENDER(eosio::token, transfer)
      // (N(eosio.token), {{_self, N(active)}},
      //  {_self, string_to_name("saccountfees"), fee,
      //   std::string("Account creation fee")});

      if (remaining_balance.amount > 0) {
        action(
          permission_level{get_self(),"active"_n},
          "eosio.token"_n,
          "transfer"_n,
          std::make_tuple( _self, new_account_name, remaining_balance, std::string("initial balance") )
        ).send();
      //   // transfer remaining balance to new account
      //   INLINE_ACTION_SENDER(eosio::token, transfer)
      //   (N(eosio.token), {{_self, N(active)}},
      //    {_self, account_to_create, remaining_balance,
      //     std::string("Initial balance")});
      }

      // eosio_assert(false, "die");
      print("new account '", new_account_name, "' successfully created.");
    }

    ACTION hello( void ) {
      print("hello world\n");
    }

};

// specify the contract name, and export a public action: update
//EOSIO_DISPATCH( accountmaker, (update)(transfer) )

extern "C" {
  void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
    // accountmaker _accountmaker(receiver);
    //auto self = receiver;

    // if( code==receiver && action== name("update").value ) {
    //   execute_action( name(receiver), name(code), &accountmaker::update );
    // }
    // else if(code==receiver && action== name("notify").value) {
    //   execute_action(name(receiver), name(code), &accountmaker::notify );
    // }
    // else if(code==receiver && action== name("erase").value) {
    //   execute_action(name(receiver), name(code), &accountmaker::erase );
    // }
    if( code==receiver && action== name("hello").value ) {
      execute_action( name(receiver), name(code), &accountmaker::hello );
    }
    else if( code==name("eosio.token").value && action== name("transfer").value ) {
      execute_action( name(receiver), name(code), &accountmaker::transfer );
    }

  }
};
