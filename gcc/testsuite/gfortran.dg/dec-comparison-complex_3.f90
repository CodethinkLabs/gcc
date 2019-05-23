! { dg-do run }
! { dg-options "-std=legacy -fdec -fno-dec-comparisons -Wconversion" }
!
! Test case contributed by Mark Eggleston  <mark.eggleston@codethink.com>
!

program convert
  complex(4) :: a
  complex(4) :: b
  a = 8HABCDABCD ! { dg-warning "Conversion from HOLLERITH" }
  b = transfer("ABCDABCD", b);
  ! Hollerith constants
  if (a.ne.8HABCDABCD) stop 1 ! { dg-error "Operands of comparison" }
  if (a.eq.8HABCEABCE) stop 2 ! { dg-error "Operands of comparison" }
  if (8HABCDABCD.ne.b) stop 3 ! { dg-error "Operands of comparison" }
  if (8HABCEABCE.eq.b) stop 4 ! { dg-error "Operands of comparison" }
  ! character literals
  if (a.ne."ABCDABCD") stop 5 ! { dg-error "Operands of comparison" }
  if (a.eq."ABCEABCE") stop 6 ! { dg-error "Operands of comparison" }
  if ("ABCDABCD".ne.b) stop 7 ! { dg-error "Operands of comparison" }
  if ("ABCEABCE".eq.b) stop 8 ! { dg-error "Operands of comparison" }
end program
