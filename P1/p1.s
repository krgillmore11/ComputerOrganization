//P1 - Kevin Gillmore
//Computer Orginization 2122
    .text                           //space for actual code
    .align                          //adds "padding" not totally sure on it's usage yet, useful in certain code not here
    .global main                    //makes visible to linker
MyStrlen:
    mov     w1, 0                   //intializing counter
    mov     x4, x2                  //moving x21 into x4

m0:
    ldrb    w3, [x4], 1             //loads the byte into w3 and and increments forward by 1 char
    cbz     x3, m1                  //check if at end of string ie: looking if it is null
    add     w1, w1, 1               //increment the counter for second parameter of printf
    b       m0                      //loop

m1:
    ret                             //return to main

main:                                                                    
    stp     x29, x30, [sp, -16]!    //push onto stack | stp = store pair
                                    //x30 is the link register
                                    //essentially 'preserving' x21 and x30
                                    //[] = dereference | both are 8 bytes wide so thats why you subtract -16 | x means 8 bytes wide                   
    str     x21, [sp, -8]!          //same idea as above
    mov     x21, x1                 //argc -> x0, argv -> x1 | copy what is in x1 into x21
                                    //preserving x21 because by default it is in x1 which is a scratch register           
                                                                        
top:                                                                    
    ldr     x2, [x21], 8            //[x21] == *argv
                                    //argv++, old value in x2 |adding eight because addresses are 8 bytes wide so incremnting by 1          
    cbz     x2, bottom              //cbz stands for Conditional Branch if Zero - goes to bottom
    bl      MyStrlen                //to myStrlen
    ldr     x0, =print1             //string literal as first paramter
    bl      printf                  //branch linked | calling printf                  
    b       top                     //loop            
                                                                        
bottom:              
    ldr     x21, [sp], 8            //pop from stack                                       
    ldp     x29, x30, [sp], 16      //^^                  
    mov     x0, xzr                 // return 0                         
    ret                                                                 

    .section    .rodata             //const read only                                                                               
print1:      .asciz      "[%d] [%s]\n"    
                                    //^^ -formatting for printf | first parameter                           
    .end                            //marks the end =)
    