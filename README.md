Authentification tools & documentation for the VM Labs Nuon
==============================================================

According to the internal documentation, it uses RSA-MD5 for authentification.
While MD5 is known for not being very strong, the key used to sign it (El-Gamal 1024-bits key) is a bit stronger unfortunely...
I attempted a dictionary attack against the key but i failed. (and my computer ran out of memory)

A passphrase was provided with the authentification tools. Unfortunely, even after trying it on an old distro (Mandrake 8),
and recompiling Gpg on my newer computer, i can confirm said passphrase does not work.

It is not "Past Is Prologue 6x7yc93".

I feel like there's more to it and somehow, employees must be able to guess the real password based on that.
Or it is a joke document and it's not the real one. (or it got redacted somehow dunno)

The only known software that can crack it is using PGPCrack-NG together with John the Ripper.
JtR itself for some reasons does not support the private key but it does when using PGPCrack-NG.
You will need a pretty beefy computer btw : 4GB of RAM will not cut it.
Get as much RAM as you can and use a lot of swap space.

https://github.com/kholia/PGPCrack-NG

Compile PGPCrack-Ng and use a linux distro like Void Linux which already provides binaries for john.
Note that on Void linux, i had to change line 50 to

# include <npth.h>

in sysutils.c

For conveninance, i exported the Application key in GPG_keys/secret.asc. Use it

Good luck, i sadly couldn't suceed.
