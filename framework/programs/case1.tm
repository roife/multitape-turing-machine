#Q = {st,cpA,cpB,prepare_multi,eatA,printC,reset,clr_right_reject,reject,reject2,reject3,reject4,reject5,reject6,reject7,reject8,reject9,reject10,reject11,reject12,reject13,reject14,halt,halt_rej}

#S = {a,b}

#G = {a,b,c,_,i,l,e,g,a,l,I,n,p,u,t}

#q0 = st

#B = _

#F = {halt}

#N = 3

st a__ a__ *** cpA
st b__ ___ r** clr_right_reject
st ___ ___ r** clr_right_reject

; copy A
cpA a__ _a_ rr* cpA
cpA b__ b__ *** cpB
cpA ___ ___ r** clr_right_reject

; copy B
cpB a__ ___ r** clr_right_reject
cpB b__ __b r*r cpB
cpB ___ ___ *ll prepare_multi

; prepare multiplication
prepare_multi _ab _ab *l* prepare_multi
prepare_multi __b __b **l prepare_multi
prepare_multi ___ ___ *rr eatA

; eatA
eatA _ab __b *r* printC

; printC
printC _ab cab r*r printC
printC __b c_b r*r printC
printC _a_ _a_ **l reset
printC ___ ___ **l halt

; reset
reset _ab _ab **l reset
reset _a_ _a_ **r eatA

; clr right reject
clr_right_reject a__ ___ r** clr_right_reject
clr_right_reject b__ ___ r** clr_right_reject
clr_right_reject ___ ___ *** reject

; reject: Illegal_Input
reject ___ I__ r** reject2
reject2 ___ l__ r** reject3
reject3 ___ l__ r** reject4
reject4 ___ e__ r** reject5
reject5 ___ g__ r** reject6
reject6 ___ a__ r** reject7
reject7 ___ l__ r** reject8
reject8 ___ ___ r** reject9
reject9 ___ I__ r** reject10
reject10 ___ n__ r** reject11
reject11 ___ p__ r** reject12
reject12 ___ u__ r** reject13
reject13 ___ t__ r** reject14
reject14 ___ ___ *** halt_rej
