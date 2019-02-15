if [ ! -e "rsa-key.pem" ]; then
    openssl genrsa > rsa-key.pem
    openssl rsa -in rsa-key.pem -RSAPublicKey_out rsa-key-pub.pem
fi