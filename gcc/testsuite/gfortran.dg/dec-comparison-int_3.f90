! { dg-do compile }
! { dg-options "-fdec -fno-dec-comparisons -Wconversion" }
!
! Test case contributed by Mark Eggleston  <mark.eggleston@codethink.com>
!

program convert
  integer(4) :: a
  integer(4) :: b
  a = 4HABCD ! { dg-warning "Conversion from HOLLERITH" }
  b = transfer("ABCD", b)
  if (a.ne.4HABCD) stop 1 ! { dg-error "Operands of comparison" }
  if (a.eq.4HABCE) stop 2 ! { dg-error "Operands of comparison" }
  if (4HABCD.ne.b) stop 3 ! { dg-error "Operands of comparison" }
  if (4HABCE.eq.b) stop 4 ! { dg-error "Operands of comparison" }
  if (4HABCE.lt.a) stop 5 ! { dg-error "Operands of comparison" }
  if (a.gt.4HABCE) stop 6 ! { dg-error "Operands of comparison" }
  if (4HABCE.le.a) stop 7 ! { dg-error "Operands of comparison" }
  if (a.ge.4HABCE) stop 8 ! { dg-error "Operands of comparison" }
end program

