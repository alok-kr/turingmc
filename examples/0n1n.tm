# Turing machine
# {0^n1^n | n>= 1}

Q : [q0,q1,q2,q3,q4]
A : [0,1]
i : q0
F : [q4]
#BLANK : b
D :
 d(q0,0) : (q1,{x,R})
 d(q0,y) : (q3,{y,R})
 d(q1,0) : (q1,{0,R})
 d(q1,y) : (q1,{y,R})
 d(q1,1) : (q2,{y,L})
 d(q2,0) : (q2,{0,L})
 d(q2,y) : (q2,{y,L})
 d(q2,x) : (q0,{x,R})
 d(q3,y) : (q3,{y,R})
 d(q3,b) : (q4,{b,R})
