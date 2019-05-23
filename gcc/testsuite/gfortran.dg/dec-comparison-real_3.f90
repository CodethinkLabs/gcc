! { dg-do run }
! { dg-options "-std=legacy -fdec -fno-dec-comparisons -Wconversion" }
!
! Test case contributed by Mark Eggleston  <mark.eggleston@codethink.com>
!

program convert
  real(4) :: a
  real(4) :: b
  a = 4HABCD ! { dg-warning "Conversion from HOLLERITH" }
  b = transfer("ABCD", b)
  ! Hollerith constants
  if (a.ne.4HABCD) stop 1 ! { dg-error "Operands of comparison" }
  if (a.eq.4HABCE) stop 2 ! { dg-error "Operands of comparison" }
  if (4HABCD.ne.b) stop 3 ! { dg-error "Operands of comparison" }
  if (4HABCE.eq.b) stop 4 ! { dg-error "Operands of comparison" }
  if (4HABCE.lt.a) stop 5 ! { dg-error "Operands of comparison" }
  if (a.gt.4HABCE) stop 6 ! { dg-error "Operands of comparison" }
  if (4HABCE.le.a) stop 7 ! { dg-error "Operands of comparison" }
  if (a.ge.4HABCE) stop 8 ! { dg-error "Operands of comparison" }
  ! Character literals
  if (a.ne."ABCD") stop 9 ! { dg-error "Operands of comparison" }
  if (a.eq."ABCE") stop 10 ! { dg-error "Operands of comparison" }
  if ("ABCD".ne.b) stop 11 ! { dg-error "Operands of comparison" }
  if ("ABCE".eq.b) stop 12 ! { dg-error "Operands of comparison" }
  if ("ABCE".lt.a) stop 13 ! { dg-error "Operands of comparison" }
  if (a.gt."ABCE") stop 14 ! { dg-error "Operands of comparison" }
  if ("ABCE".le.a) stop 15 ! { dg-error "Operands of comparison" }
  if (a.ge."ABCE") stop 16 ! { dg-error "Operands of comparison" }
end program

