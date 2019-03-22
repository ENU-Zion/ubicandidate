source config.sh
enucli -u $API_URL --wallet-url=$WALLET_URL set code $CODE ubicandidate.wasm -p $CODE@active
enucli -u $API_URL --wallet-url=$WALLET_URL set abi $CODE ubicandidate.abi -p $CODE@active
