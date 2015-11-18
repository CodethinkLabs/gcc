! { dg-do compile }
! { dg-options "-std=extra-legacy" }
!
! Test add default SEQUENCE attribute to COMMON blocks
!
        PROGRAM sequence_att_common
          TYPE STRUCT1
            INTEGER*4      ID
            INTEGER*4      TYPE
            INTEGER*8      DEFVAL
            CHARACTER*(4) NAME
            LOGICAL*1      NIL
          END TYPE STRUCT1
          
          TYPE (STRUCT1) SINST
          COMMON /BLOCK1/ SINST
        END
