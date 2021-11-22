Proc X
wrs _S0
rdi A
_bp-6 = 10
B = _bp-6
_bp-8 = 20
_bp-4 = _bp-8
_bp-10 = A * B
_bp-12 = _bp-4 + _bp-10
_bp-2 = _bp-12
wrs _S1
wri _bp-2
wrln
Endp X

Proc TWO
Call X
Endp TWO

Start proc TWO
