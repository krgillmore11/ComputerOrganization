//P2 - Kevin Gillmore & Davison Krajcik
//Computer Orginization 2122
    .text                           //space for actual code
    .align  2                       //next even address
    .global main                    //makes visible to linker
main:                                                                    
    stp     x29, x30, [sp, -16]!    //push onto stack | stp = store pair
                                    //x30 is the link register       
    mov     x4, 1                   //intialialize retval to 1
    ldr     x5,=print2              //default for directory name
    cmp     x0, x4                  //if arc > 1
    bgt     m1                      //set arv[1]
    ldr     x0, x4                  //set first argv to dirname
    str     x21, [sp, -8]!          
    mov     x21, x4                 //preserving ponter to argv or lack of one '.'
    bl      opendir                 //returns null if bad dir
    cmp     x0, 0                   //check for null return
    beq     m2                      //goto perror
    bl      __errno_location        //get address of errno
    str     wzr, [x0]               //store zero at the address in memory
    ldr     w0, x21                 //takes null terminated string
top:                        
    bl      readdir                 //returns dirent object
    cmp     x0, 0                   //checks if it is null
    beq     bottom                  //if null goto bottom
    ldr     w0,=print1
    ldr     w1, [x21,#0]            //vv offsets for dirent struct - NOT CORRECT
    ldr     w2, [x21,#64]           //creating args for printf
    ldr     w3, [x21,#32]
    bl      printf
    b       top                     //contine while

bottom:
    bl      __errno_location        //get addres of erno
    cmp     x0, 0                   //check if null
    beq     b1                      //call perrror
    ldr     w0, [x21]               //load dir
    bl      closedir

b1: 
    ldr     w0,=print3
    bl      perror

ret:
    ldr     x21, [sp], 8            //pop from stack  
    ldp     x29, x30, [sp], 16      //pop from stack            
    mov     x0, xzr                 // return 0                         
    ret                  

m1:
    add     x1, [x1], 4
    mov     x4, x1            //argv[1] - argv is stored in r1
    //not working

m2:
    ldr     x21, x0                 //load dirname into argument 
    bl      perror                  //outputs bad directory error
    b       bottom                  //end program

    .section    .rodata             //const read only
print1:      .asciz      "%-20llu 0x%02x %s\n"   
                                    //^^format param for printf
    .section    .rodata             //const read only                                                                               
print2:      .asciz      "."        //default directory

    .section    .rodata             //const read only
print3:      .asciz      "readdir() failed"   
                                    //^^format param for printf
   

    .end
