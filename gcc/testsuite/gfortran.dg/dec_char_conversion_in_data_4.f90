! { dg-do compile }
! { dg-options "-fdec -Wconversion" }
!
! Ensure that character type name is correctly reported.

program test
  integer(4) x
  data x / 'ABCD' / ! { dg-warning "CHARACTER\\(4\\) to INTEGER\\(4\\)" }
end program
