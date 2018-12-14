echo '=== pay for new account ==='
cleos push action eosio.token transfer '["eosio","accountmaker","25.0000 EOS","brandnewacnt:EOS7XPiPuL3jbgpfS3FFmjtXK62Th9n2WZdvJb6XLygAghfx1W7Nb:EOS7XPiPuL3jbgpfS3FFmjtXK62Th9n2WZdvJb6XLygAghfx1W7Nb"]' -p eosio
cleos get account brandnewacnt
