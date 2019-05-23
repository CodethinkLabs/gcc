! { dg-do run }
! { dg-options "-std=legacy -fdec -Wconversion" }
!
! Test case contributed by Mark Eggleston  <mark.eggleston@codethink.com>
!

program convert
  real(4) :: a
  real(4) :: b
  a = 4HABCD ! { dg-warning "Conversion from HOLLERITH" }
  b = transfer("ABCD", b)
  ! Hollerith constants
  if (a.ne.4HABCD) stop 1 ! { dg-warning "Conversion from HOLLERITH" }
  if (a.eq.4HABCE) stop 2 ! { dg-warning "Conversion from HOLLERITH" }
  if (4HABCD.ne.b) stop 3 ! { dg-warning "Conversion from HOLLERITH" }
  if (4HABCE.eq.b) stop 4 ! { dg-warning "Conversion from HOLLERITH" }
  if (4HABCE.lt.a) stop 5 ! { dg-warning "Conversion from HOLLERITH" }
  if (a.gt.4HABCE) stop 6 ! { dg-warning "Conversion from HOLLERITH" }
  if (4HABCE.le.a) stop 7 ! { dg-warning "Conversion from HOLLERITH" }
  if (a.ge.4HABCE) stop 8 ! { dg-warning "Conversion from HOLLERITH" }
  ! Character literals
  if (a.ne."ABCD") stop 9 ! { dg-warning "Conversion from CHARACTER" }
  if (a.eq."ABCE") stop 10 ! { dg-warning "Conversion from CHARACTER" }
  if ("ABCD".ne.b) stop 11 ! { dg-warning "Conversion from CHARACTER" }
  if ("ABCE".eq.b) stop 12 ! { dg-warning "Conversion from CHARACTER" }
  if ("ABCE".lt.a) stop 13 ! { dg-warning "Conversion from CHARACTER" }
  if (a.gt."ABCE") stop 14 ! { dg-warning "Conversion from CHARACTER" }
  if ("ABCE".le.a) stop 15 ! { dg-warning "Conversion from CHARACTER" }
  if (a.ge."ABCE") stop 16 ! { dg-warning "Conversion from CHARACTER" }
end program

