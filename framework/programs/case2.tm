#Q = {st,ckCnext,prepare,ck,cls_accept,cls_reject_moveL,cls_reject_moveR,accept,accept2,accept3,accept4,reject,reject2,reject3,reject4,reject5,moveR,moveL,halt_accept,halt_reject,prepare_2,moveL2}

#S = {a,b,c}

#G = {a,b,c,A,B,C,#,_,t,r,u,e,f,a,l,s}

#q0 = st

#B = _

#F = {halt_accept}

#N = 2

; State st: start
st a_ AA rr moveR
st b_ BB rr moveR
st c_ CC rr ckCnext
st AA AA ** cls_reject_moveL
st BB BB ** cls_reject_moveL
st CC CC ** cls_reject_moveL
st __ __ ** cls_reject_moveL

; State ckCnext
ckCnext a_ a_ rr moveR
ckCnext b_ b_ rr moveR
ckCnext c_ c_ rr moveR
ckCnext A* A* ll prepare
ckCnext B* B* ll prepare
ckCnext C* C* ll prepare
ckCnext __ __ ll prepare

; State prepare
prepare CC ## l* prepare_2
prepare_2 *# *# l* prepare_2
prepare_2 _# _# rr ck

; State ck
ck AA __ rr ck
ck BB __ rr ck
ck CC __ rr ck
ck AB AB rr cls_reject_moveL
ck AC AC rr cls_reject_moveL
ck BA BA rr cls_reject_moveL
ck BC BC rr cls_reject_moveL
ck CA CA rr cls_reject_moveL
ck CB CB rr cls_reject_moveL
ck #_ __ rr cls_accept

; State cls_accept
cls_accept *_ __ r* cls_accept
cls_accept __ __ ** accept

; State cls_reject
cls_reject_moveL ** ** l* cls_reject_moveL
cls_reject_moveL __ __ r* cls_reject_moveR
cls_reject_moveL _* _* r* cls_reject_moveR
cls_reject_moveR ** __ rr cls_reject_moveR
cls_reject_moveR _* __ *r cls_reject_moveR
cls_reject_moveR *_ __ r* cls_reject_moveR
cls_reject_moveR __ __ ** reject

; State accept*: write 'true' on 1st tape
accept __ t_ r* accept2
accept2 __ r_ r* accept3
accept3 __ u_ r* accept4
accept4 __ e_ r* halt_accept

; State reject*: write 'false' on 1st tape
reject __ f_ r* reject2
reject2 __ a_ r* reject3
reject3 __ l_ r* reject4
reject4 __ s_ r* reject5
reject5 __ e_ r* halt_reject

; State moveR
moveR a_ a_ rr moveR
moveR b_ b_ rr moveR
moveR c_ c_ rr moveR
moveR AA AA ll moveL
moveR BB BB ll moveL
moveR CC CC ll moveL
moveR __ __ ll moveL

; State moveL
moveL a_ AA ll moveL2
moveL b_ BB ll moveL2
moveL c_ CC ll moveL2
moveL AA AA ** cls_reject_moveL
moveL BB BB ** cls_reject_moveL
moveL CC CC ** cls_reject_moveL
moveL2 a_ a_ ll moveL2
moveL2 b_ b_ ll moveL2
moveL2 c_ c_ ll moveL2
moveL2 AA AA rr st
moveL2 BB BB rr st
moveL2 CC CC rr st
moveL2 __ __ ** cls_reject_moveL

