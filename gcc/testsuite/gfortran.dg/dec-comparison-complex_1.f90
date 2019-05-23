! { dg-do run }
! { dg-options "-std=legacy -fdec -Wconversion" }
!
! Test case contributed by Mark Eggleston  <mark.eggleston@codethink.com>
!

program convert
  complex(4) :: a
  complex(4) :: b
  a = 8HABCDABCD ! { dg-warning "Conversion from HOLLERITH" }
  b = transfer("ABCDABCD", b);
  ! Hollerith constants
  if (a.ne.8HABCDABCD) stop 1 ! { dg-warning "Conversion from HOLLERITH" }
  if (a.eq.8HABCEABCE) stop 2 ! { dg-warning "Conversion from HOLLERITH" }
  if (8HABCDABCD.ne.b) stop 3 ! { dg-warning "Conversion from HOLLERITH" }
  if (8HABCEABCE.eq.b) stop 4 ! { dg-warning "Conversion from HOLLERITH" }
  ! Character literals
  if (a.ne."ABCDABCD") stop 5 ! { dg-warning "Conversion from CHARACTER" }
  if (a.eq."ABCEABCE") stop 6 ! { dg-warning "Conversion from CHARACTER" }
  if ("ABCDABCD".ne.b) stop 7 ! { dg-warning "Conversion from CHARACTER" }
  if ("ABCEABCE".eq.b) stop 8 ! { dg-warning "Conversion from CHARACTER" }
end program
