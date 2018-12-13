# accountmaker

**Description**
The account running this contract will accept EOS as payment and automatically create a new system account.
This can be used to create accounts for new EOS users.

Tables

Actions
### accountmaker::transfer
```bash
cleos push action eosio.token transfer '["exchangeaddress","accountmaker","5.0000 EOS","mynewaccount:PUBLIC_OWNER_KEY:PUBLIC_ACTIVE_KEY"]' -p exchangeaddress
```

```bash
2018-11-25T23:04:02.425 thread-0   apply_context.cpp:28          print_debug          ]
[(eosio.token,transfer)->accountmaker]: CONSOLE OUTPUT BEGIN =====================
eosio is sending accountmaker 5.0000 EOS
memo: brandnewacnt:EOS7XPiPuL3jbgpfS3FFmjtXK62Th9n2WZdvJb6XLygAghfx1W7Nb:EOS7XPiPuL3jbgpfS3FFmjtXK62Th9n2WZdvJb6XLygAghfx1W7Nb
name: brandnewacnt
active key: EOS7XPiPuL3jbgpfS3FFmjtXK62Th9n2WZdvJb6XLygAghfx1W7Nb
owner key: EOS7XPiPuL3jbgpfS3FFmjtXK62Th9n2WZdvJb6XLygAghfx1W7Nb
new account 'brandnewacnt' successfully created.
[(eosio.token,transfer)->accountmaker]: CONSOLE OUTPUT END   =====================
executed transaction: c740b28f5fb75c246da02a478e600f049f988d298836c362f17c279c2187e9f8  248 bytes  80931 us
#   eosio.token <= eosio.token::transfer        {"from":"eosio","to":"accountmaker","quantity":"5.0000 EOS","memo":"brandnewacnt:EOS7XPiPuL3jbgpfS3F...
#         eosio <= eosio.token::transfer        {"from":"eosio","to":"accountmaker","quantity":"5.0000 EOS","memo":"brandnewacnt:EOS7XPiPuL3jbgpfS3F...
#  accountmaker <= eosio.token::transfer        {"from":"eosio","to":"accountmaker","quantity":"5.0000 EOS","memo":"brandnewacnt:EOS7XPiPuL3jbgpfS3F...
>> eosio is sending accountmaker 5.0000 EOS
#         eosio <= eosio::newaccount            {"creator":"accountmaker","newact":"brandnewacnt","owner":{"threshold":1,"keys":[{"key":"EOS7XPiPuL3...
#         eosio <= eosio::buyrambytes           {"payer":"accountmaker","receiver":"brandnewacnt","bytes":8192}
#   eosio.token <= eosio.token::transfer        {"from":"accountmaker","to":"eosio.ram","quantity":"0.1194 EOS","memo":"buy ram"}
#  accountmaker <= eosio.token::transfer        {"from":"accountmaker","to":"eosio.ram","quantity":"0.1194 EOS","memo":"buy ram"}
#     eosio.ram <= eosio.token::transfer        {"from":"accountmaker","to":"eosio.ram","quantity":"0.1194 EOS","memo":"buy ram"}
#   eosio.token <= eosio.token::transfer        {"from":"accountmaker","to":"eosio.ramfee","quantity":"0.0007 EOS","memo":"ram fee"}
#  accountmaker <= eosio.token::transfer        {"from":"accountmaker","to":"eosio.ramfee","quantity":"0.0007 EOS","memo":"ram fee"}
#  eosio.ramfee <= eosio.token::transfer        {"from":"accountmaker","to":"eosio.ramfee","quantity":"0.0007 EOS","memo":"ram fee"}
#         eosio <= eosio::delegatebw            {"from":"accountmaker","receiver":"brandnewacnt","stake_net_quantity":"0.0500 EOS","stake_cpu_quanti...
#   eosio.token <= eosio.token::transfer        {"from":"accountmaker","to":"eosio.stake","quantity":"0.2000 EOS","memo":"stake bandwidth"}
#  accountmaker <= eosio.token::transfer        {"from":"accountmaker","to":"eosio.stake","quantity":"0.2000 EOS","memo":"stake bandwidth"}
#   eosio.stake <= eosio.token::transfer        {"from":"accountmaker","to":"eosio.stake","quantity":"0.2000 EOS","memo":"stake bandwidth"}
warning: transaction executed locally, but may not be confirmed by the network yet    ]
=== end of setup blockchain accounts and smart contract ===
2018-11-25T23:04:02.501 thread-0   producer_plugin.cpp:1419      produce_block        ] Produced block 0000001e844c5610... #30 @ 2018-11-25T23:04:02.500 signed by eosio [trxs: 4, lib: 29, confirmed: 0]
```