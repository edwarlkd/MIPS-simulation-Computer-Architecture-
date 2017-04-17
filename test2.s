# Short test case for your project.
#
# Note that this is by no means a comprehensive test!
#

		.text
		addu $t0, $0, 5
		addu $t1, $t0, 1	 	
	Loop:
		subu $t0, $t0, $t1  			
		beq $t0, $0, Exit
		j Loop
	Exit:
		addi $s3, $0, 1
		addi $0,$0,0 #unsupported instruction, terminate
		
