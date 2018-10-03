! { dg-do run }
! { dg-options "-fdec-char-data-as-int" }
!
! Test character to int conversion in DATA types
!
        PROGRAM char_int_data_type
          INTEGER*1 ai(2)

          DATA ai/'1',1/
          if(ai(1).NE.49) STOP 1
        END
