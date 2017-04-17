#check all arthimetric works, and checking firmly for bne and beq

.test
addiu	$t0, $0, 3	
addiu	$t1, $0, 4	
subu	$t2, $t1, 2

beq	$t0, $t1, Wowequal
bne	$t0, $t1, Notequal

Notequal:
addu	$t3, $t0, $t2
or	$t0, $t1, $t3
add	$0, $0, $0
	
Wowequal:
addiu	$v0, $0, 1
subu 	$v1, $t1, $t0
add	$v0, $v0, $0