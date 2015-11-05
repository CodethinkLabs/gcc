! { dg-do compile }
! { dg-options "-std=extra-legacy" }
!
! Test kind specification in variable not in type
!
        PROGRAM spec_in_var
          INTEGER  ai*1/1/
          REAL ar*4/1.0/

          if(ai.NE.1) STOP 1
          if(abs(ar - 1.0) > 1.0D-6) STOP 2
        END
