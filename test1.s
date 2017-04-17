		# Test 1
		#
		.text
		lui $t0, 0x0000
		addiu $t1, $t0, 1
		sw $t1, 0($t0)
		sw $t2, 4($t0)
		addu $t3, $1, 2
		and $t0, $t1, $t2
		addiu $v0, $t1, 2
		j	Here
		sll $s1, $t3, 3
		subu $t1, $t3, $t2
		
		
		Here:		
		addiu	$t0,$0,3
		addiu	$t1,$0,2
		ori 	$t4,$t1,2
		subu 	$t2,$t1,$t0
		sll	$t2,$t1,1
		addiu	$s1,$t0,4
		or	$t0,$s1,$t1
		addi	$0,$0,0 #unsupported instruction, terminate






