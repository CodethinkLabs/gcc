! { dg-do compile }
! { dg-options "-fdec -fno-dec-format-defaults" }
!
! Test case for the default field widths not enabled.
!

    character(50) :: buffer

    real*4 :: real_4
    real*8 :: real_8
    real*16 :: real_16
    integer :: len

    real_4 = 4.18
    write(buffer, '(A, F, A)') ':',real_4,':' ! { dg-error "Nonnegative width required" }

    real_4 = 0.00000018
    write(buffer, '(A, F, A)') ':',real_4,':' ! { dg-error "Nonnegative width required" }

    real_8 = 4.18
    write(buffer, '(A, F, A)') ':',real_8,':' ! { dg-error "Nonnegative width required" }

    real_16 = 4.18
    write(buffer, '(A, F, A)') ':',real_16,':' ! { dg-error "Nonnegative width required" }
end
