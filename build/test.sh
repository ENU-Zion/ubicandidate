source config.sh
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE apply '["enujoshuaqiu"]' -p enujoshuaqiu
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE apply '["v"]' -p v
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE apply '["ubitestuser1"]' -p ubitestuser1
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE apply '["ubitestuser2"]' -p ubitestuser2
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE apply '["ubitestuser3"]' -p ubitestuser3
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE apply '["ubitestuser4"]' -p ubitestuser4
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE apply '["ubitestuser5"]' -p ubitestuser5
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE add '["enujoshuaqiu"]' -p qsx.io
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE add '["v"]' -p qsx.io
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE vote '["enujoshuaqiu","ubitestuser1",true,"good1"]' -p enujoshuaqiu
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE vote '["enujoshuaqiu","ubitestuser2",true,"good2"]' -p enujoshuaqiu
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE vote '["enujoshuaqiu","ubitestuser3",true,"good3"]' -p enujoshuaqiu
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE vote '["enujoshuaqiu","ubitestuser4",true,"good4"]' -p enujoshuaqiu
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE vote '["enujoshuaqiu","ubitestuser5",true,"good5"]' -p enujoshuaqiu
sleep 3s;
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE vote '["v","ubitestuser1",true,"bad1"]' -p v
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE vote '["v","ubitestuser2",true,"bad2"]' -p v
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE vote '["v","ubitestuser3",true,"bad3"]' -p v
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE vote '["v","ubitestuser4",true,"bad4"]' -p v
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE vote '["v","ubitestuser5",true,"bad5"]' -p v
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE activate '["ubitestuser1"]' -p v
sleep 2s;
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE vote '["ubitestuser1","ubitestuser2",true,"bad2"]' -p ubitestuser1
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE vote '["ubitestuser1","ubitestuser3",true,"bad3"]' -p ubitestuser1
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE vote '["ubitestuser1","ubitestuser4",true,"bad4"]' -p ubitestuser1
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE vote '["ubitestuser1","ubitestuser5",true,"bad5"]' -p ubitestuser1
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE activate '["ubitestuser2"]' -p v
sleep 2s;
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE vote '["ubitestuser2","ubitestuser3",true,"bad3"]' -p ubitestuser2
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE vote '["ubitestuser2","ubitestuser4",true,"bad4"]' -p ubitestuser2
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE vote '["ubitestuser2","ubitestuser5",true,"bad5"]' -p ubitestuser2
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE activate '["ubitestuser3"]' -p v
sleep 2s;
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE vote '["ubitestuser3","ubitestuser4",true,"bad4"]' -p ubitestuser3
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE vote '["ubitestuser3","ubitestuser5",true,"bad5"]' -p ubitestuser3
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE activate '["ubitestuser4"]' -p v
sleep 2s;
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE vote '["ubitestuser4","ubitestuser5",true,"bad5"]' -p ubitestuser4
sleep 2s;
enucli -u $API_URL --wallet-url=$WALLET_URL push action $CODE activate '["ubitestuser5"]' -p v
