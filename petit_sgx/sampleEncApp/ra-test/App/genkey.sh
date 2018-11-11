if [ ! -e "rsa-key.pem" ]; then
    openssl genrsa > rsa-key.pem
fi

if [ ! -e "rsa-key-pub.pem" ]; then
    openssl rsa -in rsa-key.pem -RSAPublicKey_out rsa-key-pub.pem
fi
