! { dg-do run }
! { dg-options "-fdec" }
!
! Test character to int conversion in DATA types
!
! Test case contributed by Jim MacArthur <jim.macarthur@codethink.co.uk>
! Modified by Mark Eggleston <mark.eggleston@codethink.com>
!
        PROGRAM char_int_data_type
          INTEGER*1 ai(2)

          DATA ai/'1',1/
          if(ai(1).NE.49) STOP 1
        END
