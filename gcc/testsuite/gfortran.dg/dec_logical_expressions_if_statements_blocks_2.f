! { dg-do run }
! { dg-options "-fdec" }
!
! Allow logical expressions in if statements and blocks
!
        PROGRAM logical_exp_if_st_bl
          INTEGER ipos/1/
          INTEGER ineg/0/

          ! Test non logical variables
          if (ineg) STOP 1 ! { dg-warning "if it evaluates to nonzero" }
          if (0) STOP 2 ! { dg-warning "if it evaluates to nonzero" }

          ! Test non logical expressions in if statements
          if (MOD(ipos, 1)) STOP 3 ! { dg-warning "if it evaluates to nonzero" }

          ! Test non logical expressions in if blocks
          if (MOD(2 * ipos, 2)) then ! { dg-warning "if it evaluates to nonzero" }
            STOP 4
          endif
        END
