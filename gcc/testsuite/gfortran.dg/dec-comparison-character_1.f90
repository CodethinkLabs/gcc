! { dg-do run }
! { dg-options "-fdec -Wconversion" }
!
! Test case contributed by Mark Eggleston  <mark.eggleston@codethink.com>
!

program convert
  character(4) :: c = 4HJMAC ! { dg-warning "HOLLERITH to CHARACTER" }
  if (4HJMAC.ne.4HJMAC) stop 1 ! { dg-warning "HOLLERITH to CHARACTER" }
  if (4HJMAC.ne."JMAC") stop 2 ! { dg-warning "HOLLERITH to CHARACTER" }
  if (4HJMAC.eq."JMAN") stop 3 ! { dg-warning "HOLLERITH to CHARACTER" }
  if ("JMAC".eq.4HJMAN) stop 4 ! { dg-warning "HOLLERITH to CHARACTER" }
  if ("AAAA".eq.5HAAAAA) stop 5 ! { dg-warning "HOLLERITH to CHARACTER" }
  if ("BBBBB".eq.5HBBBB ) stop 6 ! { dg-warning "HOLLERITH to CHARACTER" }
  if (4HJMAC.ne.c) stop 7 ! { dg-warning "HOLLERITH to CHARACTER" }
  if (c.ne.4HJMAC) stop 8 ! { dg-warning "HOLLERITH to CHARACTER" }
end program

