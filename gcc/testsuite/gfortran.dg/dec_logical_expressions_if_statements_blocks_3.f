! { dg-do compile }
! { dg-options "-fdec -fno-dec-non-logical-if" }
!
! Allow logical expressions in if statements and blocks
!
        PROGRAM logical_exp_if_st_bl
          INTEGER ipos/1/
          INTEGER ineg/0/

          ! Test non logical variables
          if (ineg) STOP 1 ! { dg-error "IF clause at" }
          if (0) STOP 2 ! { dg-error "IF clause at" }

          ! Test non logical expressions in if statements
          if (MOD(ipos, 1)) STOP 3 ! { dg-error "IF clause at" }

          ! Test non logical expressions in if blocks
          if (MOD(2 * ipos, 2)) then ! { dg-error "IF clause at" }
            STOP 4
          endif
        END
