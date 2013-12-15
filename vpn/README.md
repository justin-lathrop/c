<center><h3>VPN</h3></center>
<hr />

VPN written in all C using OPENSSL and TUN/TAP interface to allow a computer to computer, gateway to gateway connection.  

This will encrypt all connections sent out of the gateways/computers and only be able to be decrypted by the correct certificate signed by the certificate authority given.

This has only been developed and tested for use on Ubuntu 12.04.  OpenSSL and TUN/TAP will need to be accessable upon compilation/runtime.

Will in runtime you can change your secret key (password) and refresh the connection at anytime.
